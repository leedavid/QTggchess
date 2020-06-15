/*
    This file is part of GGzero Chess.

    GGzero Chess is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    GGzero Chess is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with GGzero Chess.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma execution_character_set("utf-8")

#include "mainwindow.h"
#include "openingbook.h"
#include "polyglotbook.h"

#include <QAction>
#include <QHBoxLayout>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>
#include <QDockWidget>
#include <QTreeView>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QClipboard>
#include <QWindow>
#include <QSettings>
#include <QDesktopWidget>
#include <qtoolbutton.h>


#include <board/boardfactory.h>
#include <chessgame.h>
#include <timecontrol.h>
#include <enginemanager.h>
#include <gamemanager.h>
#include <playerbuilder.h>
#include <chessplayer.h>
#include <humanbuilder.h>
#include <enginebuilder.h>
#include <tournament.h>

#include "cutechessapp.h"
#include "gameviewer.h"
#include "movelist.h"
#include "newgamedlg.h"
#include "newtournamentdialog.h"
#include "chessclock.h"
#include "plaintextlog.h"
#include "gamedatabasemanager.h"
#include "pgntagsmodel.h"
#include "gametabbar.h"
#include "evalhistory.h"
#include "evalwidget.h"
#include "boardview/boardscene.h"
#include "tournamentresultsdlg.h"
#include <engineconfiguration.h>

#include "BoardEditor.h"
#include "random.h"

#include <pgnstream.h>
#include <pgngameentry.h>

#ifdef QT_DEBUG
#include <modeltest.h>
#endif

MainWindow::TabData::TabData(ChessGame* game, Tournament* tournament)
	: m_id(game),
	  m_game(game),
	  m_pgn(game->pgn()),
	  m_tournament(tournament),
	  m_finished(false)	{}

MainWindow::MainWindow(ChessGame* game)
	: m_game(nullptr),
	  m_closing(false),
	  m_readyToClose(false),
	  m_firstTabAutoCloseEnabled(true),
	m_myClosePreTab(false),  
	m_pcap(nullptr),
	m_autoClickCap(nullptr)
	//m_bAutomaticLinking(false)
{
	
	//this->setContextMenuPolicy(Qt::CustomContextMenu);  // �Ҽ��˵�
	
	setAttribute(Qt::WA_DeleteOnClose, true);
	setDockNestingEnabled(true);

	m_gameViewer = new GameViewer(Qt::Horizontal, nullptr, true);
	for (int i = 0; i < 2; i++)
	{
		Chess::Side side = Chess::Side::Type(i);
		m_gameViewer->chessClock(side)->setPlayerName(side.toString());
	}
	m_gameViewer->setContentsMargins(6, 6, 6, 6);

	m_moveList = new MoveList(this);
	m_tagsModel = new PgnTagsModel(this);
	#ifdef QT_DEBUG
	new ModelTest(m_tagsModel, this);
	#endif

	m_evalHistory = new EvalHistory(this);					// ��ʷ���ߴ���
	m_evalWidgets[0] = new EvalWidget(this);				// PV ·������
	m_evalWidgets[1] = new EvalWidget(this);

	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->addWidget(m_gameViewer);

	// The content margins look stupid when used with dock widgets
	mainLayout->setContentsMargins(0, 0, 0, 0);

	QWidget* mainWidget = new QWidget(this);
	mainWidget->setLayout(mainLayout);
	setCentralWidget(mainWidget);

	createActions();
	createMenus();
	createToolBars();
	createDockWindows();

	// ״̬��
	statusBar()->showMessage("http://www.ggzero.cn");

	connect(m_moveList, SIGNAL(moveClicked(int,bool)),			     // ��������߲�
	        m_gameViewer, SLOT(viewMove(int,bool)));
	connect(m_moveList, SIGNAL(commentClicked(int, QString)),        // ����岽˵��
		this, SLOT(editMoveComment(int, QString)));
	connect(m_gameViewer, SIGNAL(moveSelected(int)),
		m_moveList, SLOT(selectMove(int)));

	connect(m_gameViewer->boardScene(), SIGNAL(MouseRightClicked()),
		this, SLOT(onMouseRightClicked()));   // �����Ҽ��˵�


	connect(CuteChessApplication::instance()->gameManager(),
		SIGNAL(finished()), this, SLOT(onGameManagerFinished()),
		Qt::QueuedConnection);

	//connect(this, SIGNAL(customContextMenuRequested), this, SLOT(showContextMenu));

	readSettings();
	addGame(game);
}

MainWindow::~MainWindow()
{
	if (m_pcap != nullptr) {
		while (m_pcap->isRunning()) {
			m_pcap->on_stop();
			wait(1);
		}
	}
	if (m_autoClickCap != nullptr) {
		while (m_autoClickCap->isRunning()) {
			m_autoClickCap->on_stop();
			wait(1);
		}
	}
	delete m_pcap;
	delete m_autoClickCap;
}

void MainWindow::createActions()
{
	m_editBoardAct = new QAction(tr("�༭����"), this);

	m_newGameAct = new QAction(tr("&�½��Ծ�..."), this);
	m_newGameAct->setShortcut(QKeySequence::New);

	m_openPgnAct = new QAction(tr("�� PGN �Ծ�"), this);
	m_openPgnAct->setShortcut(QKeySequence::Open);        //

	m_closeGameAct = new QAction(tr("&�ر�"), this);
	#ifdef Q_OS_WIN32
	m_closeGameAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W));
	#else
	m_closeGameAct->setShortcut(QKeySequence::Close);
	#endif

	m_saveGameAct = new QAction(tr("&����Ծ�"), this);
	m_saveGameAct->setShortcut(QKeySequence::Save);

	m_saveGameAsAct = new QAction(tr("&�Ծ����Ϊ..."), this);
	m_saveGameAsAct->setShortcut(QKeySequence::SaveAs);

	m_copyFenAct = new QAction(tr("&���� FEN"), this);
	QAction* copyFenSequence = new QAction(m_gameViewer);
	copyFenSequence->setShortcut(QKeySequence::Copy);
	copyFenSequence->setShortcutContext(Qt::WidgetWithChildrenShortcut);
	m_gameViewer->addAction(copyFenSequence);

	m_pasteFenAct = new QAction(tr("&ճ�� FEN"), this);
	m_pasteFenAct->setShortcut(QKeySequence(QKeySequence::Paste));

	m_copyPgnAct = new QAction(tr("&���� PGN"), this);

	m_flipBoardAct = new QAction(tr("&���·�ת����"), this);

	m_adjudicateDrawAct = new QAction(tr("&�ж�����"), this);
	m_adjudicateWhiteWinAct = new QAction(tr("�ж���ʤ"), this);
	m_adjudicateBlackWinAct = new QAction(tr("�ж���ʤ"), this);

	m_changeBoardPicAct = new QAction(tr("��������ͼƬ"), this);

	m_changeBoardBackGroundAct = new QAction(tr("��������"), this);

	m_resignGameAct = new QAction(tr("����"), this);

	m_quitGameAct = new QAction(tr("&�˳�"), this);
	m_quitGameAct->setMenuRole(QAction::QuitRole);
	#ifdef Q_OS_WIN32
	m_quitGameAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
	#else
	m_quitGameAct->setShortcut(QKeySequence::Quit);
	#endif

	m_newTournamentAct = new QAction(tr("&�½�����..."), this);
	m_stopTournamentAct = new QAction(tr("&ֹͣ����"), this);
	m_showTournamentResultsAct = new QAction(tr("&�������..."), this);

	m_showSettingsAct = new QAction(tr("&ͨ������"), this);
	m_showSettingsAct->setMenuRole(QAction::PreferencesRole);

	m_showGameDatabaseWindowAct = new QAction(tr("&�Ծ����ݿ�"), this);

	m_showGameWallAct = new QAction(tr("&��ǰ�Ծ�"), this);

	m_minimizeAct = new QAction(tr("&��С��"), this);
	m_minimizeAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_M));

	m_showPreviousTabAct = new QAction(tr("&��ʾǰһ��"), this);
	#ifdef Q_OS_MAC
	m_showPreviousTabAct->setShortcut(QKeySequence(Qt::MetaModifier + Qt::ShiftModifier + Qt::Key_Tab));
	#else
	m_showPreviousTabAct->setShortcut(QKeySequence(Qt::ControlModifier + Qt::ShiftModifier + Qt::Key_Tab));
	#endif

	m_showNextTabAct = new QAction(tr("&��ʾ��һ��"), this);
	#ifdef Q_OS_MAC
	m_showNextTabAct->setShortcut(QKeySequence(Qt::MetaModifier + Qt::Key_Tab));
	#else
	m_showNextTabAct->setShortcut(QKeySequence(Qt::ControlModifier + Qt::Key_Tab));
	#endif

	m_aboutAct = new QAction(tr("&���ڼѼѽ���..."), this);
	m_aboutAct->setMenuRole(QAction::AboutRole);

	//this->m_gameViewer->boardScene()->changeBoardPicture();
	connect(m_changeBoardPicAct, SIGNAL(triggered()), this->m_gameViewer->boardScene(), SLOT(OnchangeBoardPicture()));
	connect(m_changeBoardBackGroundAct, SIGNAL(triggered()), this->m_gameViewer->boardScene(), SLOT(OnChangeBackGround()));

	connect(m_editBoardAct, SIGNAL(triggered()), this, SLOT(editBoard()));
	connect(m_newGameAct, SIGNAL(triggered()), this, SLOT(newGame()));
	
	connect(m_openPgnAct, SIGNAL(triggered()), this, SLOT(OpenPgnGame()));

	connect(m_copyFenAct, SIGNAL(triggered()), this, SLOT(copyFen()));
	connect(m_pasteFenAct, SIGNAL(triggered()), this, SLOT(pasteFen()));
	connect(copyFenSequence, SIGNAL(triggered()), this, SLOT(copyFen()));
	connect(m_copyPgnAct, SIGNAL(triggered()), this, SLOT(copyPgn()));
	connect(m_flipBoardAct, SIGNAL(triggered()),
		m_gameViewer->boardScene(), SLOT(flip()));
	connect(m_closeGameAct, &QAction::triggered, this, [=]()
	{
		auto focusWindow = CuteChessApplication::activeWindow();
		if (!focusWindow)
			return;

		auto focusMainWindow = qobject_cast<MainWindow*>(focusWindow);
		if (focusMainWindow)
		{
			focusMainWindow->closeCurrentGame();
			return;
		}

		focusWindow->close();
	});

	auto app = CuteChessApplication::instance();

	connect(m_saveGameAct, SIGNAL(triggered()), this, SLOT(save()));
	connect(m_saveGameAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

	connect(m_adjudicateDrawAct, SIGNAL(triggered()), this, SLOT(adjudicateDraw()));
	connect(m_adjudicateWhiteWinAct, SIGNAL(triggered()), this, SLOT(adjudicateWhiteWin()));
	connect(m_adjudicateBlackWinAct, SIGNAL(triggered()), this, SLOT(adjudicateBlackWin()));

	connect(m_resignGameAct, SIGNAL(triggered()), this, SLOT(resignGame()));

	connect(m_quitGameAct, &QAction::triggered,
		app, &CuteChessApplication::onQuitAction);

	connect(m_newTournamentAct, SIGNAL(triggered()), this, SLOT(newTournament()));

	connect(m_minimizeAct, &QAction::triggered, this, [=]()
	{
		auto focusWindow = app->focusWindow();
		if (focusWindow != nullptr)
		{
			focusWindow->showMinimized();
		}
	});

	connect(m_showSettingsAct, SIGNAL(triggered()),
		app, SLOT(showSettingsDialog()));

	connect(m_showTournamentResultsAct, SIGNAL(triggered()),
		app, SLOT(showTournamentResultsDialog()));

	connect(m_showGameDatabaseWindowAct, SIGNAL(triggered()),
		app, SLOT(showGameDatabaseDialog()));

	connect(m_showGameWallAct, SIGNAL(triggered()),
		app, SLOT(showGameWall()));

	connect(m_aboutAct, SIGNAL(triggered()), this, SLOT(showAboutDialog()));
}

void MainWindow::createMenus()
{
	m_gameMenu = menuBar()->addMenu(tr("&��Ϸ"));
	m_gameMenu->addAction(m_editBoardAct);
	m_gameMenu->addAction(m_newGameAct);
	m_gameMenu->addAction(m_openPgnAct);
	m_gameMenu->addSeparator();
	m_gameMenu->addAction(m_closeGameAct);
	m_gameMenu->addAction(m_saveGameAct);
	m_gameMenu->addAction(m_saveGameAsAct);
	m_gameMenu->addSeparator();
	m_gameMenu->addAction(m_copyFenAct);
	m_gameMenu->addAction(m_copyPgnAct);
	m_gameMenu->addAction(m_pasteFenAct);
	m_gameMenu->addSeparator();
	m_gameMenu->addAction(m_adjudicateDrawAct);
	m_gameMenu->addAction(m_adjudicateWhiteWinAct);
	m_gameMenu->addAction(m_adjudicateBlackWinAct);
	m_gameMenu->addSeparator();
	m_gameMenu->addAction(m_resignGameAct);
	m_gameMenu->addSeparator();
	m_gameMenu->addAction(m_quitGameAct);

	m_tournamentMenu = menuBar()->addMenu(tr("&����"));
	m_tournamentMenu->addAction(m_newTournamentAct);
	m_tournamentMenu->addAction(m_stopTournamentAct);
	m_tournamentMenu->addAction(m_showTournamentResultsAct);
	m_stopTournamentAct->setEnabled(false);

	m_toolsMenu = menuBar()->addMenu(tr("&����"));
	m_toolsMenu->addAction(m_showSettingsAct);
        m_toolsMenu->addAction(m_showGameDatabaseWindowAct);

	m_viewMenu = menuBar()->addMenu(tr("&��ͼ"));
	m_viewMenu->addAction(m_flipBoardAct);
	m_viewMenu->addSeparator();

	m_windowMenu = menuBar()->addMenu(tr("&����"));
	addDefaultWindowMenu();

	connect(m_windowMenu, SIGNAL(aboutToShow()), this,
		SLOT(onWindowMenuAboutToShow()));

	m_helpMenu = menuBar()->addMenu(tr("&����"));
	m_helpMenu->addAction(m_aboutAct);
}

void MainWindow::createToolBars()
{
	m_tabBar = new GameTabBar();
	m_tabBar->setDocumentMode(true);
	m_tabBar->setTabsClosable(true);   
	m_tabBar->setMovable(false);
	m_tabBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	connect(m_tabBar, SIGNAL(currentChanged(int)),
		this, SLOT(onTabChanged(int)));
	connect(m_tabBar, SIGNAL(tabCloseRequested(int)),
		this, SLOT(onTabCloseRequested(int)));
	connect(m_showPreviousTabAct, SIGNAL(triggered()),
		m_tabBar, SLOT(showPreviousTab()));
	connect(m_showNextTabAct, SIGNAL(triggered()),
		m_tabBar, SLOT(showNextTab()));

	QToolBar* toolBar = new QToolBar(tr("Game Tabs"));
	toolBar->setObjectName("GameTabs");
	toolBar->setVisible(false);
	toolBar->setFloatable(false);
	toolBar->setMovable(false);
	toolBar->setAllowedAreas(Qt::TopToolBarArea);
	toolBar->addWidget(m_tabBar);
	addToolBar(toolBar);


	//QAction* actLinkChessBoard;   // ������������
	//QAction* actEngineThink;      // ������˼��
	//QAction* actEngineStop;       // ������ֹͣ
	//QAction* actEngineAnalyze;    // ���������

	// ���˵�������
	this->mainToolbar = new QToolBar(this);
	this->mainToolbar->setObjectName(QStringLiteral("mainToolBar"));
	this->mainToolbar->setToolButtonStyle(Qt::ToolButtonIconOnly); //  ToolButtonTextUnderIcon); ToolButtonIconOnly
	this->mainToolbar->setMovable(true);
	this->mainToolbar->setAllowedAreas(Qt::TopToolBarArea);
	this->mainToolbar->setVisible(true);
	this->addToolBar(Qt::TopToolBarArea, this->mainToolbar);

	// ����ִ��
	this->tbtnEnginePlayRed = new QToolButton(this);
	this->tbtnEnginePlayRed->setCheckable(true);
	this->tbtnEnginePlayRed->setObjectName(QStringLiteral("EnginePlayRed"));
	QIcon iconEnginePlayRed;
	iconEnginePlayRed.addFile(QStringLiteral(":/icon/com_red.png"), QSize(), QIcon::Normal, QIcon::Off);
	this->tbtnEnginePlayRed->setIcon(iconEnginePlayRed);
	this->tbtnEnginePlayRed->setToolTip("����ִ������");
	this->mainToolbar->addWidget(this->tbtnEnginePlayRed);
	connect(this->tbtnEnginePlayRed, SIGNAL(toggled(bool)), this, SLOT(onPlayRedToggled(bool)));

	// ����ִ��
	this->tbtnEnginePlayBlack = new QToolButton(this);
	this->tbtnEnginePlayBlack->setCheckable(true);
	this->tbtnEnginePlayBlack->setObjectName(QStringLiteral("EnginePlayBlack"));
	QIcon iconEnginePlayBlack;
	iconEnginePlayBlack.addFile(QStringLiteral(":/icon/com_blue.png"), QSize(), QIcon::Normal, QIcon::Off);
	this->tbtnEnginePlayBlack->setIcon(iconEnginePlayBlack);
	this->tbtnEnginePlayBlack->setToolTip("����ִ������");
	this->mainToolbar->addWidget(this->tbtnEnginePlayBlack);
	connect(this->tbtnEnginePlayBlack, SIGNAL(toggled(bool)), this, SLOT(onPlayBlackToggled(bool)));

	// ��������������
	this->actEngineStop = new QAction(this);
	this->actEngineStop->setObjectName(QStringLiteral("EngineStop"));
	QIcon iconEngineStop;
	iconEngineStop.addFile(QStringLiteral(":/icon/stop.ico"),
		QSize(), QIcon::Normal, QIcon::Off);
	this->actEngineStop->setIcon(iconEngineStop);
	this->actEngineStop->setText("��������");
    this->actEngineStop->setToolTip("��������������");
	//this->mainToolbar->addAction(this->actEngineStop);

	// �����������̣��췽����
	this->tbtnLinkChessBoardRed = new QToolButton(this);
	this->tbtnLinkChessBoardRed->setCheckable(true);
	this->tbtnLinkChessBoardRed->setObjectName(QStringLiteral("LinkChessBoardRed"));
	QIcon iconLinkChessBoardRed;
	iconLinkChessBoardRed.addFile(QStringLiteral(":/icon/RedLink.png"),
		QSize(), QIcon::Normal, QIcon::Off);
	this->tbtnLinkChessBoardRed->setIcon(iconLinkChessBoardRed);
	this->tbtnLinkChessBoardRed->setText("�췽����");
	this->tbtnLinkChessBoardRed->setToolTip("������������, �ҷ�ִ��");
	this->mainToolbar->addWidget(this->tbtnLinkChessBoardRed);
	connect(this->tbtnLinkChessBoardRed, SIGNAL(toggled(bool)), this, SLOT(onLinkRedToggled(bool)));


	//onLinkRedToggled

	// �����������̣��ڷ�����
	this->tbtnLinkChessBoardBlack = new QToolButton(this);
	this->tbtnLinkChessBoardBlack->setCheckable(true);
	this->tbtnLinkChessBoardBlack->setObjectName(QStringLiteral("LinkChessBoardBlack"));
	QIcon iconLinkChessBoardBlack;
	iconLinkChessBoardBlack.addFile(QStringLiteral(":/icon/BlackLink.png"),
		QSize(), QIcon::Normal, QIcon::Off);
	this->tbtnLinkChessBoardBlack->setIcon(iconLinkChessBoardBlack);
	this->tbtnLinkChessBoardBlack->setText("�췽����");
	this->tbtnLinkChessBoardBlack->setToolTip("������������, �ҷ�ִ��");
	this->mainToolbar->addWidget(this->tbtnLinkChessBoardBlack);
	connect(this->tbtnLinkChessBoardBlack, SIGNAL(toggled(bool)), this, SLOT(onLinkBlackToggled(bool)));

	// ȫ�Զ�������������
	this->tbtnLinkAuto = new QToolButton(this);
	this->tbtnLinkAuto->setCheckable(true);
	this->tbtnLinkAuto->setObjectName(QStringLiteral("LinkChessBoardAuto"));
	QIcon iconLinkChessBoardAuto;
	iconLinkChessBoardAuto.addFile(QStringLiteral(":/icon/autolink.png"),
		QSize(), QIcon::Normal, QIcon::Off);
	this->tbtnLinkAuto->setIcon(iconLinkChessBoardAuto);
	this->tbtnLinkAuto->setText("ȫ�Զ�����");
	this->tbtnLinkAuto->setToolTip("ȫ�Զ�������������");
	this->mainToolbar->addWidget(this->tbtnLinkAuto);
	connect(this->tbtnLinkAuto, SIGNAL(toggled(bool)), this, SLOT(onLinkAutomaticToggled(bool)));


	QWidget* empty = new QWidget();
	empty->setFixedSize(10, 20);
	this->mainToolbar->addWidget(empty);

	//QComboBox* cbtnLinkBoard;            // ���ߵ�����
	this->cbtnLinkBoard = new QComboBox(this);
	this->cbtnLinkBoard->setObjectName(QStringLiteral("cbtnLinkBoard"));
	this->cbtnLinkBoard->setToolTip("�ı����߷���");
	QStringList strList;
	//strList << "��������" << "��������" ;
	strList << "��������" << "��������";
	this->cbtnLinkBoard->addItems(strList);
	connect(this->cbtnLinkBoard, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onLinkBoardCombox(const QString&)));
	this->mainToolbar->addWidget(this->cbtnLinkBoard);

	
	//int sel = this->cbtnLinkBoard->currentIndex();
	//QSettings().setValue("ui/linkboard_curSel", sel);

	int sel = QSettings().value("ui/linkboard_curSel").toInt();
	this->cbtnLinkBoard->setCurrentIndex(sel);
	

	QWidget* empty2 = new QWidget();
	empty2->setFixedSize(10, 20);
	this->mainToolbar->addWidget(empty2);

	//cbtnLinkEngine
	this->cbtnLinkEngine = new QComboBox(this);
	this->cbtnLinkEngine->setObjectName(QStringLiteral("cbtnLinkEngine"));
	this->cbtnLinkEngine->setToolTip("ѡ�����ߵ�����");

	//
	EngineManager* m_engineManager =
		CuteChessApplication::instance()->engineManager();

	for (int i = 0; i < m_engineManager->engineCount(); i++) {
		this->cbtnLinkEngine->addItem(m_engineManager->engineAt(i).name());
	}

	//QSet<QString> qset = m_engineManager->engineNames();
	//for (auto v : qset) {
	//	this->cbtnLinkEngine->addItem(v);
	//}
	this->mainToolbar->addWidget(this->cbtnLinkEngine);

	sel = QSettings().value("ui/linkboard_curEngine").toInt();
	this->cbtnLinkEngine->setCurrentIndex(sel);


	//QSettings().setValue("ui/linkboard", true);

	//bool b = QSettings().value("ui/linkboard").toBool();

	//int a = 0;

	//http://c.biancheng.net/view/1849.html

	//QAction* actLinkChessBoardRed;      // ������������
	//QAction* actLinkChessBoardBlack;    // ������������

	//// ������������
	//this->actLinkChessBoard = new QAction(this);
	//this->actLinkChessBoard->setObjectName(QStringLiteral("LinkChessBoard"));
	//QIcon iconLinkChessBoard;
	//iconLinkChessBoard.addFile(QStringLiteral(":/icon/Links.ico"),
	//	QSize(), QIcon::Normal, QIcon::Off);
	//this->actLinkChessBoard->setIcon(iconLinkChessBoard);
	//this->actLinkChessBoard->setText("����");
	//this->actLinkChessBoard->setToolTip("������������");

	//// ������˼��
	//this->actEngineThink = new QAction(this);
	//this->actEngineThink->setObjectName(QStringLiteral("EngineThink"));
	//QIcon iconEngineThink;
	//iconEngineThink.addFile(QStringLiteral(":/icon/thought-balloon.ico"),
	//	QSize(), QIcon::Normal, QIcon::Off);
	//this->actEngineThink->setIcon(iconEngineThink);
	//this->actEngineThink->setText("˼��");
	//this->actEngineThink->setToolTip("������˼����ǰ��֣����Զ�����");

	//// ���������
	//this->actEngineAnalyze = new QAction(this);
	//this->actEngineAnalyze->setObjectName(QStringLiteral("EngineAnalyze"));
	//QIcon iconEngineAnalyze;
	//iconEngineAnalyze.addFile(QStringLiteral(":/icon/analyze.ico"),
	//	QSize(), QIcon::Normal, QIcon::Off);
	//this->actEngineAnalyze->setIcon(iconEngineAnalyze);
	//this->actEngineAnalyze->setText("����");
	//this->actEngineAnalyze->setToolTip("�����������ǰ���");

	

	//// ��������
	//this->actEngineSetting = new QAction(this);
	//this->actEngineSetting->setObjectName(QStringLiteral("EngineSetting"));
	//QIcon iconEngineSetting;
	//iconEngineSetting.addFile(QStringLiteral(":/icon/Settings.ico"),
	//	QSize(), QIcon::Normal, QIcon::Off);
	//this->actEngineSetting->setIcon(iconEngineSetting);
	//this->actEngineSetting->setText("����");
	//this->actEngineSetting->setToolTip("�����������");

	//this->mainToolbar->addAction(this->actLinkChessBoard);
	//this->mainToolbar->addAction(this->actEngineThink);
	//this->mainToolbar->addAction(this->actEngineAnalyze);
	//this->mainToolbar->addAction(this->actEngineStop);   
	//this->mainToolbar->addAction(this->actEngineSetting);   


	//connect(this->actLinkChessBoard, &QAction::triggered, this, &MainWindow::onLXchessboard);


	//connect(m_tabBar, SIGNAL(currentChanged(int)),
	//	this, SLOT(onTabChanged(int)));
}

void MainWindow::createDockWindows()
{
	// Engine debug
	QDockWidget* engineDebugDock = new QDockWidget(tr("�������"), this);
	engineDebugDock->setObjectName("EngineDebugDock");
	m_engineDebugLog = new PlainTextLog(engineDebugDock);
	engineDebugDock->setWidget(m_engineDebugLog);
	engineDebugDock->close();
	addDockWidget(Qt::BottomDockWidgetArea, engineDebugDock);

	// Evaluation history
	auto evalHistoryDock = new QDockWidget(tr("��ʷ��������"), this);
	evalHistoryDock->setObjectName("EvalHistoryDock");
	evalHistoryDock->setWidget(m_evalHistory);
	addDockWidget(Qt::BottomDockWidgetArea, evalHistoryDock);

	// Players' eval widgets
	auto whiteEvalDock = new QDockWidget(tr("�췽����"), this);
	whiteEvalDock->setObjectName("WhiteEvalDock");
	whiteEvalDock->setWidget(m_evalWidgets[Chess::Side::White]);
	addDockWidget(Qt::RightDockWidgetArea, whiteEvalDock);
	auto blackEvalDock = new QDockWidget(tr("�ڷ�����"), this);
	blackEvalDock->setObjectName("BlackEvalDock");
	blackEvalDock->setWidget(m_evalWidgets[Chess::Side::Black]);
	addDockWidget(Qt::RightDockWidgetArea, blackEvalDock);

	m_whiteEvalDock = whiteEvalDock;
	m_blackEvalDock = blackEvalDock;


	// Move list
	QDockWidget* moveListDock = new QDockWidget(tr("����"), this);
	moveListDock->setObjectName("MoveListDock");
	moveListDock->setWidget(m_moveList);
	addDockWidget(Qt::RightDockWidgetArea, moveListDock);
	splitDockWidget(moveListDock, whiteEvalDock, Qt::Horizontal);
	splitDockWidget(whiteEvalDock, blackEvalDock, Qt::Vertical);

	// Tags
	QDockWidget* tagsDock = new QDockWidget(tr("��ǩ"), this);
	tagsDock->setObjectName("TagsDock");
	QTreeView* tagsView = new QTreeView(tagsDock);
	tagsView->setModel(m_tagsModel);
	tagsView->setAlternatingRowColors(true);
	tagsView->setRootIsDecorated(false);
	tagsDock->setWidget(tagsView);

	addDockWidget(Qt::RightDockWidgetArea, tagsDock);

	tabifyDockWidget(moveListDock, tagsDock);
	moveListDock->raise();

	// Add toggle view actions to the View menu
	m_viewMenu->addAction(moveListDock->toggleViewAction());
	m_viewMenu->addAction(tagsDock->toggleViewAction());
	m_viewMenu->addAction(engineDebugDock->toggleViewAction());
	m_viewMenu->addAction(evalHistoryDock->toggleViewAction());
	m_viewMenu->addAction(whiteEvalDock->toggleViewAction());
	m_viewMenu->addAction(blackEvalDock->toggleViewAction());
}

void MainWindow::readSettings()
{
	// https://blog.csdn.net/liang19890820/article/details/50513695

	QSettings s;
	s.beginGroup("ui");
	s.beginGroup("mainwindow");

	restoreGeometry(s.value("geometry").toByteArray());
	// Workaround for https://bugreports.qt.io/browse/QTBUG-16252
	if (isMaximized())
		setGeometry(QApplication::desktop()->availableGeometry(this));
	restoreState(s.value("window_state").toByteArray());

	s.endGroup();
	s.endGroup();
}

void MainWindow::writeSettings()
{
	QSettings s;
	s.beginGroup("ui");
	s.beginGroup("mainwindow");

	s.setValue("geometry", saveGeometry());
	s.setValue("window_state", saveState());

	s.endGroup();
	s.endGroup();

	int sel = this->cbtnLinkBoard->currentIndex();
	QSettings().setValue("ui/linkboard_curSel", sel);

	sel = this->cbtnLinkEngine->currentIndex();
	QSettings().setValue("ui/linkboard_curEngine", sel);
	
}

void MainWindow::addGame(ChessGame* game)
{
	Tournament* tournament = qobject_cast<Tournament*>(QObject::sender());
	//Chess::Capture* pcap = new Chess::Capture(game, this);

	TabData tab(game, tournament);


	if (tournament)
	{
		int index = tabIndex(tournament, true);
		if (index != -1)
		{
			delete m_tabs[index].m_pgn;
			m_tabs[index] = tab;

			m_tabBar->setTabText(index, genericTitle(tab));
			if (!m_closing && m_tabBar->currentIndex() == index)
				setCurrentGame(tab);

			return;
		}
	}
	else
		connect(game, SIGNAL(finished(ChessGame*)),
			this, SLOT(onGameFinished(ChessGame*)));

	m_tabs.append(tab);
	m_tabBar->setCurrentIndex(m_tabBar->addTab(genericTitle(tab)));

	// close initial tab if unused and if enabled by settings
	if (m_tabs.size() >= 2
	&&  m_firstTabAutoCloseEnabled)
	{
		if (QSettings().value("ui/close_unused_initial_tab", true).toBool()
		&&  !m_tabs[0].m_game.isNull()
		&&  m_tabs[0].m_game.data()->moves().isEmpty())
			closeTab(0);

		m_firstTabAutoCloseEnabled = false;
		//m_myClosePreTab = true;
	}
	else {
		if (m_myClosePreTab) {
			closeTab(0);
			m_myClosePreTab = false;
		}
	}

	if (m_tabs.size() >= 2)
		m_tabBar->parentWidget()->show();
}

void MainWindow::removeGame(int index)
{
	//if (index == -1) {
	//	return;
	//}
	
	Q_ASSERT(index != -1);

	m_tabs.removeAt(index);
	m_tabBar->removeTab(index);

	if (m_tabs.size() == 1)
		m_tabBar->parentWidget()->hide();
}

void MainWindow::destroyGame(ChessGame* game)
{
	Q_ASSERT(game != nullptr);

	int index = tabIndex(game);

	if (index == -1) {
		return;  // by LGL
	}

	Q_ASSERT(index != -1);
	TabData tab = m_tabs.at(index);

	removeGame(index);

	if (tab.m_tournament == nullptr)
		game->deleteLater();

	delete tab.m_pgn;

	if (m_tabs.isEmpty())
		close();
}

void MainWindow::setCurrentGame(const TabData& gameData)
{
	if (gameData.m_game == m_game && m_game != nullptr)
		return;

	for (int i = 0; i < 2; i++)
	{
		ChessPlayer* player(m_players[i]);
		if (player != nullptr)
		{
			disconnect(player, nullptr, m_engineDebugLog, nullptr);
			disconnect(player, nullptr,
			           m_gameViewer->chessClock(Chess::Side::White), nullptr);
			disconnect(player, nullptr,
			           m_gameViewer->chessClock(Chess::Side::Black), nullptr);
		}
	}

	if (m_game != nullptr)
	{
		m_game->pgn()->setTagReceiver(nullptr);
		m_gameViewer->disconnectGame();
		disconnect(m_game, nullptr, m_moveList, nullptr);

		ChessGame* tmp = m_game;
		m_game = nullptr;

		// QObject::disconnect() is not atomic, so we need to flush
		// all pending events from the previous game before switching
		// to the next one.
		tmp->lockThread();
		CuteChessApplication::processEvents();
		tmp->unlockThread();

		// If the call to CuteChessApplication::processEvents() caused
		// a new game to be selected as the current game, then our
		// work here is done.
		if (m_game != nullptr)
			return;
	}

	m_game = gameData.m_game;

	lockCurrentGame();

	m_engineDebugLog->clear();

	m_moveList->setGame(m_game, gameData.m_pgn);
	m_evalHistory->setGame(m_game);

	if (m_game == nullptr)
	{
		m_gameViewer->setGame(gameData.m_pgn);
		m_evalHistory->setPgnGame(gameData.m_pgn);

		for (int i = 0; i < 2; i++)
		{
			Chess::Side side = Chess::Side::Type(i);
			auto clock = m_gameViewer->chessClock(side);
			clock->stop();
			clock->setInfiniteTime(true);
			QString name = nameOnClock(gameData.m_pgn->playerName(side),
						   side);
			clock->setPlayerName(name);
		}

		m_tagsModel->setTags(gameData.m_pgn->tags());

		updateWindowTitle();
		updateMenus();

		for (auto evalWidget : m_evalWidgets)
			evalWidget->setPlayer(nullptr);

		return;
	}
	else
		m_gameViewer->setGame(m_game);

	m_tagsModel->setTags(gameData.m_pgn->tags());
	gameData.m_pgn->setTagReceiver(m_tagsModel);

	for (int i = 0; i < 2; i++)
	{
		Chess::Side side = Chess::Side::Type(i);
		ChessPlayer* player(m_game->player(side));
		m_players[i] = player;

		connect(player, SIGNAL(debugMessage(QString)),
			m_engineDebugLog, SLOT(appendPlainText(QString)));

		auto clock = m_gameViewer->chessClock(side);

		clock->stop();
		QString name = nameOnClock(player->name(), side);
		clock->setPlayerName(name);
		connect(player, SIGNAL(nameChanged(QString)),
			clock, SLOT(setPlayerName(QString)));

		clock->setInfiniteTime(player->timeControl()->isInfinite());

		if (player->state() == ChessPlayer::Thinking)
			clock->start(player->timeControl()->activeTimeLeft());
		else
			clock->setTime(player->timeControl()->timeLeft());

		connect(player, SIGNAL(startedThinking(int)),
			clock, SLOT(start(int)));
		connect(player, SIGNAL(stoppedThinking()),
			clock, SLOT(stop()));
		m_evalWidgets[i]->setPlayer(player);
	}

	if (m_game->boardShouldBeFlipped())
		m_gameViewer->boardScene()->flip();

	updateMenus();
	updateWindowTitle();
	unlockCurrentGame();
}

int MainWindow::tabIndex(ChessGame* game) const
{
	Q_ASSERT(game != nullptr);

	for (int i = 0; i < m_tabs.size(); i++)
	{
		if (m_tabs.at(i).m_id == game)
			return i;
	}

	return -1;
}

int MainWindow::tabIndex(Tournament* tournament, bool freeTab) const
{
	Q_ASSERT(tournament != nullptr);

	for (int i = 0; i < m_tabs.size(); i++)
	{
		const TabData& tab = m_tabs.at(i);

		if (tab.m_tournament == tournament
		&&  (!freeTab || (tab.m_game == nullptr || tab.m_finished)))
			return i;
	}

	return -1;
}

void MainWindow::onTabChanged(int index)
{
	if (index == -1 || m_closing)
		m_game = nullptr;
	else
		setCurrentGame(m_tabs.at(index));
}

void MainWindow::onTabCloseRequested(int index)
{
	const TabData& tab = m_tabs.at(index);

	if (tab.m_tournament && tab.m_game)
	{
		auto btn = QMessageBox::question(this, tr("����������"),
			   tr("�����Ҫֹͣ��ǰ��������?"));
		if (btn != QMessageBox::Yes)
			return;
	}

	closeTab(index);
}

void MainWindow::closeTab(int index)
{
	const TabData& tab = m_tabs.at(index);

	if (tab.m_game == nullptr)
	{
		delete tab.m_pgn;
		removeGame(index);

		if (m_tabs.isEmpty())
			close();

		return;
	}

	//if (m_bAutomaticLinking) {
	//	destroyGame(tab.m_game);
	//	return;
	//}

	if (tab.m_finished)
		destroyGame(tab.m_game);
	else
	{
		connect(tab.m_game, SIGNAL(finished(ChessGame*)),
			this, SLOT(destroyGame(ChessGame*)));
		QMetaObject::invokeMethod(tab.m_game, "stop", Qt::QueuedConnection);
	}
}

void MainWindow::closeCurrentGame()
{
	closeTab(m_tabBar->currentIndex());
}

void MainWindow::editBoard() {
	BoardEditorDlg dlgEditBoard(m_tabs.at(m_tabBar->currentIndex()).m_game->board(), this);
	if (dlgEditBoard.exec() != QDialog::Accepted)
		return;

	QString fen = dlgEditBoard.fenString();
	qDebug() << "�༭fen: " << fen;

	QString variant = m_game.isNull() || m_game->board() == nullptr ?
		"standard" : m_game->board()->variant();

	auto board = Chess::BoardFactory::create(variant);
	board->setFenString(fen);

	//board->legalMoves();
	auto game = new ChessGame(board, new PgnGame());
	game->setTimeControl(TimeControl("inf"));
	game->setStartingFen(fen);
	game->pause();

	connect(game, &ChessGame::initialized, this, &MainWindow::addGame);
	connect(game, &ChessGame::startFailed, this, &MainWindow::onGameStartFailed);

	CuteChessApplication::instance()->gameManager()->newGame(game,
		new HumanBuilder(CuteChessApplication::userName()),
		new HumanBuilder(CuteChessApplication::userName()));
}

void MainWindow::newGame()  // �½�һ����Ϸ
{
	EngineManager* engineManager = CuteChessApplication::instance()->engineManager();
	NewGameDialog dlg(engineManager, this);
	if (dlg.exec() != QDialog::Accepted)             // �����ȡ��
		return;

	auto game = dlg.createGame();
	if (!game)
	{
		QMessageBox::critical(this, tr("Could not initialize game"),
				      tr("The game could not be initialized "
					 "due to an invalid opening."));
		return;
	}

	PlayerBuilder* builders[2] = {
		dlg.createPlayerBuilder(Chess::Side::White),
		dlg.createPlayerBuilder(Chess::Side::Black)
	};

	if (builders[game->board()->sideToMove()]->isHuman())
		game->pause();

	// Start the game in a new tab
	connect(game, SIGNAL(initialized(ChessGame*)),
		this, SLOT(addGame(ChessGame*)));
	connect(game, SIGNAL(startFailed(ChessGame*)),
		this, SLOT(onGameStartFailed(ChessGame*)));
	CuteChessApplication::instance()->gameManager()->newGame(game,             // ������������ӵ�Tab����
		builders[Chess::Side::White], builders[Chess::Side::Black]);
}

void MainWindow::onGameStartFailed(ChessGame* game)
{
	QMessageBox::critical(this, tr("Game Error"), game->errorString());
}

void MainWindow::onGameFinished(ChessGame* game)
{
	int tIndex = tabIndex(game);
	if (tIndex == -1)
		return;

	auto& tab = m_tabs[tIndex];
	tab.m_finished = true;
	QString title = genericTitle(tab);
	m_tabBar->setTabText(tIndex, title);
	if (game == m_game)
	{
		// Finished tournament games are destroyed immediately
		// so we can't touch the game object any more.
		if (tab.m_tournament)
			m_game = nullptr;
		updateWindowTitle();
		updateMenus();
	}

	// ����fen�ļ������㵼��ѧϰ
	//static std::string directory =
	//	CommandLine::BinaryDirectory() + "/data-" + Random::Get().GetString(12);
	// It's fine if it already exists.
	//CreateDirectory(directory.c_str());

	if (!game->pgn()->isNull()
		&& game->pgn()->moves().length() > 12) {  // ����5���ű���

		QString runPath = QCoreApplication::applicationDirPath();
		QString expDir = runPath + "/pgngame/";

		QDir* tempDir = new QDir;
		if (!tempDir->exists(expDir)) {
			if (!tempDir->mkdir(expDir)) {		
				QMessageBox::warning(this, tr("����"), expDir + tr(" �����½�Ŀ¼"));
				return ;
			}
		}
		QString fenFile = expDir + Chess::Random::Get().GetString(12) + ".pgn";		
		game->pgn()->writeOnePgnGame(fenFile);
	}


	// save game notation of non-tournament games to default PGN file
	if (!tab.m_tournament
	&&  !game->pgn()->isNull()
	&&  	(  !game->pgn()->moves().isEmpty()   // ignore empty games
		|| !game->pgn()->result().isNone())) // without adjudication
	{
		QString fileName = QSettings().value("games/default_pgn_output_file", QString())
					      .toString();

		if (!fileName.isEmpty())
			game->pgn()->write(fileName);
			//TODO: reaction on error
	}
}

void MainWindow::OpenPgnGame()
{
	
	QString filePGN = QFileDialog::getOpenFileName(this,
		"�� PGN ��ʽ�����",
		QString(),
		tr("PGN ��ʽ (*.pgn);;All Files (*.*)"));
	

	QFile file(filePGN);
	QFileInfo fileInfo(filePGN);
	if (!fileInfo.exists())
	{
		QMessageBox::information(this, "������", "PGN �ļ������ڣ�");
		return;
	}

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QMessageBox::information(this, "������", "PGN ���������򿪣�");
		return;
	}

	PgnGame* pgnGame = new PgnGame();
	{
		PgnStream in(&file);		
		if (!pgnGame->read(in)) {
			return;
		}
	}

	QString variant = m_game.isNull() || m_game->board() == nullptr ?
		"standard" : m_game->board()->variant();

	auto board = Chess::BoardFactory::create(variant);
	board->setFenString(board->defaultFenString());

	//auto game = new ChessGame(board, new PgnGame());
	auto game = new ChessGame(board, pgnGame);
	game->setTimeControl(TimeControl("inf"));
	game->pause();

	connect(game, &ChessGame::initialized, this, &MainWindow::addGame);
	connect(game, &ChessGame::startFailed, this, &MainWindow::onGameStartFailed);

	CuteChessApplication::instance()->gameManager()->newGame(game,
		new HumanBuilder(CuteChessApplication::userName()),
		new HumanBuilder(CuteChessApplication::userName()));
}

void MainWindow::newTournament()
{
	NewTournamentDialog dlg(CuteChessApplication::instance()->engineManager(), this);
	if (dlg.exec() != QDialog::Accepted)
		return;

	GameManager* manager = CuteChessApplication::instance()->gameManager();

	Tournament* t = dlg.createTournament(manager);
	auto resultsDialog = CuteChessApplication::instance()->tournamentResultsDialog();
	connect(t, SIGNAL(finished()),
		this, SLOT(onTournamentFinished()));
	connect(t, SIGNAL(gameStarted(ChessGame*, int, int, int)),
		this, SLOT(addGame(ChessGame*)));
	connect(t, SIGNAL(gameFinished(ChessGame*, int, int, int)),
		resultsDialog, SLOT(update()));
	connect(t, SIGNAL(gameFinished(ChessGame*, int, int, int)),
		this, SLOT(onGameFinished(ChessGame*)));
	t->start();

	connect(m_stopTournamentAct, &QAction::triggered, [=]()
	{
		auto btn = QMessageBox::question(this, tr("Stop tournament"),
			   tr("Do you really want to stop the ongoing tournament?"));
		if (btn != QMessageBox::Yes)
		{
			m_closing = false;
			return;
		}

		t->stop();
	});
	m_newTournamentAct->setEnabled(false);
	m_stopTournamentAct->setEnabled(true);
	resultsDialog->setTournament(t);
}

void MainWindow::onTournamentFinished()
{
	Tournament* tournament = qobject_cast<Tournament*>(QObject::sender());
	Q_ASSERT(tournament != nullptr);

	m_stopTournamentAct->disconnect();

	QString error = tournament->errorString();
	QString name = tournament->name();

	tournament->deleteLater();
	m_newTournamentAct->setEnabled(true);
	m_stopTournamentAct->setEnabled(false);

	if (m_closing)
	{
		closeAllGames();
		return;
	}

	m_showTournamentResultsAct->trigger();

	if (!error.isEmpty())
	{
		QMessageBox::critical(this,
				      tr("Tournament error"),
				      tr("Tournament \"%1\" finished with an error.\n\n%2")
				      .arg(name, error));
	}

	CuteChessApplication::alert(this);
}

void MainWindow::onWindowMenuAboutToShow()
{
	m_windowMenu->clear();

	addDefaultWindowMenu();
	m_windowMenu->addSeparator();

	const QList<MainWindow*> gameWindows =
		CuteChessApplication::instance()->gameWindows();

	for (int i = 0; i < gameWindows.size(); i++)
	{
		MainWindow* gameWindow = gameWindows.at(i);

		QAction* showWindowAction = m_windowMenu->addAction(
			gameWindow->windowListTitle(), this, SLOT(showGameWindow()));
		showWindowAction->setData(i);
		showWindowAction->setCheckable(true);

		if (gameWindow == this)
			showWindowAction->setChecked(true);
	}
}

void MainWindow::showGameWindow()
{
	if (QAction* action = qobject_cast<QAction*>(sender()))
		CuteChessApplication::instance()->showGameWindow(action->data().toInt());
}

void MainWindow::updateWindowTitle()
{
	// setWindowTitle() requires "[*]" (see docs)
	const TabData& gameData(m_tabs.at(m_tabBar->currentIndex()));
	setWindowTitle(genericTitle(gameData) + QLatin1String("[*]"));
}

QString MainWindow::windowListTitle() const
{
	const TabData& gameData(m_tabs.at(m_tabBar->currentIndex()));

	#ifndef Q_OS_MAC
	if (isWindowModified())
		return genericTitle(gameData) + QLatin1String("*");
	#endif

	return genericTitle(gameData);
}

bool MainWindow::isMoveValid(const Chess::GenericMove& move){
	return m_gameViewer->isMoveValid(move);
}
	



QString MainWindow::genericTitle(const TabData& gameData) const
{
	QString white;
	QString black;
	Chess::Result result;
	if (gameData.m_game)
	{
		white = gameData.m_game->player(Chess::Side::White)->name();
		black = gameData.m_game->player(Chess::Side::Black)->name();
		result = gameData.m_game->result();
	}
	else
	{
		white = gameData.m_pgn->playerName(Chess::Side::White);
		black = gameData.m_pgn->playerName(Chess::Side::Black);
		result = gameData.m_pgn->result();
	}

	if (result.isNone())
		return tr("%1 vs %2").arg(white, black);
	else
		return tr("%1 vs %2 (%3)")
		       .arg(white, black, result.toShortString());
}

void MainWindow::updateMenus()
{
	QPointer<ChessPlayer> white = m_players[Chess::Side::White];
	QPointer<ChessPlayer> black = m_players[Chess::Side::Black];
	bool isHumanGame =  (!white.isNull() && white->isHuman())
			 || (!black.isNull() && black->isHuman());
	bool gameOn = (!m_game.isNull() && !m_game->isFinished());
	m_adjudicateBlackWinAct->setEnabled(gameOn);
	m_adjudicateWhiteWinAct->setEnabled(gameOn);
	m_adjudicateDrawAct->setEnabled(gameOn);
	m_resignGameAct->setEnabled(gameOn && isHumanGame);
}

QString MainWindow::nameOnClock(const QString& name, Chess::Side side) const
{
	QString text = name;
	bool displaySide = QSettings().value("ui/display_players_sides_on_clocks", false)
				      .toBool();
	if (displaySide)
		text.append(QString(" (%1)").arg(side.toString()));
	return text;
}

void MainWindow::editMoveComment(int ply, const QString& comment)
{
	bool ok;
	QString text = QInputDialog::getMultiLineText(this, tr("Edit move comment"),
						      tr("Comment:"), comment, &ok);
	if (ok && text != comment)
	{
		lockCurrentGame();
		PgnGame* pgn(m_tabs.at(m_tabBar->currentIndex()).m_pgn);
		PgnGame::MoveData md(pgn->moves().at(ply));
		md.comment = text;
		pgn->setMove(ply, md);
		unlockCurrentGame();

		m_moveList->setMove(ply, md.move, md.moveString, text);
	}
}

void MainWindow::onMouseRightClicked(/*QGraphicsSceneContextMenuEvent* event*/)
{
	

	QMenu mymenu;
	mymenu.addAction(m_newGameAct);
	mymenu.addAction(m_saveGameAct);

	mymenu.addSeparator();

	mymenu.addAction(m_copyFenAct);
	mymenu.addAction(m_pasteFenAct);

	mymenu.addSeparator();

	mymenu.addAction(m_changeBoardPicAct);	
	mymenu.addAction(m_changeBoardBackGroundAct);

	mymenu.addSeparator();
	
	//mymenu.addAction(tbtnLinkAuto->defaultAction());

	mymenu.exec(QCursor::pos());
	
	//m_viewMenu->addAction(moveListDock->toggleViewAction());
}

void MainWindow::copyFen()
{
	QClipboard* cb = CuteChessApplication::clipboard();
	QString fen(m_gameViewer->board()->fenString());
	if (!fen.isEmpty())
		cb->setText(fen);
}

void MainWindow::pasteFen()
{
	auto cb = CuteChessApplication::clipboard();
	if (cb->text().isEmpty())
		return;

	QString fen = cb->text().trimmed();
	QStringList stFList = fen.split("fen");		// by LGL
	if (stFList.length() > 1) {
		fen = stFList[1].trimmed();		
	}

	QString variant = m_game.isNull() || m_game->board() == nullptr ?
				"standard" : m_game->board()->variant();

	auto board = Chess::BoardFactory::create(variant);
	if (!board->setFenString(fen))
	{
		QMessageBox msgBox(QMessageBox::Critical,
				   tr("FEN error"),
				   tr("Invalid FEN string for the \"%1\" variant:")
				   .arg(variant),
				   QMessageBox::Ok, this);
		msgBox.setInformativeText(cb->text());
		msgBox.exec();

		delete board;
		return;
	}

	//board->legalMoves();

	auto game = new ChessGame(board, new PgnGame());
	game->setTimeControl(TimeControl("inf"));
	game->setStartingFen(fen);
	game->pause();

	connect(game, &ChessGame::initialized, this, &MainWindow::addGame);
	connect(game, &ChessGame::startFailed, this, &MainWindow::onGameStartFailed);

	CuteChessApplication::instance()->gameManager()->newGame(game,
		new HumanBuilder(CuteChessApplication::userName()),
		new HumanBuilder(CuteChessApplication::userName()));
}


void MainWindow::showAboutDialog()
{
	this->mainToolbar->setVisible(true);
	
	QString html;
	html += "<h3>" + QString("�Ѽѽ��� %1")
		.arg(CuteChessApplication::applicationVersion()) + "</h3>";
	html += "<p>" + tr("Qt �汾 %1").arg(qVersion()) + "</p>";
	html += "<p>" + tr("�汾���� 2019-2020 ") + "</p>";
	html += "<p>" + tr("���� Lee David") + "</p>";
	html += "<p>" + tr("��л��ʹ�üѼ��������") + "</p>";
	html += "<a href=\"http://elo.ggzero.cn\">GGzeroѵ����վ</a><br>";
	html += "<a href=\"http://bbs.ggzero.cn\">�ٷ���̳</a><br>";
	html += "<a href=\"https://jq.qq.com/?_wv=1027&k=5FxO79E\">����QQȺ</a><br>";
	QMessageBox::about(this, tr("�Ѽѽ���"), html);
	
}

// 
void MainWindow::lockCurrentGame()
{
	if (m_game != nullptr)
		m_game->lockThread();
}

void MainWindow::unlockCurrentGame()
{
	if (m_game != nullptr)
		m_game->unlockThread();
}

bool MainWindow::save()
{
	if (m_currentFile.isEmpty())
		return saveAs();

	return saveGame(m_currentFile);
}

bool MainWindow::saveAs()
{
	const QString fileName = QFileDialog::getSaveFileName(
		this,
		tr("����Ծ�"),
		QString(),
		tr("PGN ��ʽ (*.pgn);;All Files (*.*)"),
		nullptr,
		QFileDialog::DontConfirmOverwrite);
	if (fileName.isEmpty())
		return false;

	return saveGame(fileName);
}

bool MainWindow::saveGame(const QString& fileName)
{
	lockCurrentGame();
	bool ok = m_tabs.at(m_tabBar->currentIndex()).m_pgn->write(fileName);
	unlockCurrentGame();

	if (!ok)
		return false;

	m_currentFile = fileName;
	setWindowModified(false);

	return true;
}

void MainWindow::copyPgn()
{
	QString str("");
	QTextStream s(&str);
	PgnGame* pgn = m_tabs.at(m_tabBar->currentIndex()).m_pgn;
	if (pgn == nullptr)
		return;
	s << *pgn;

	QClipboard* cb = CuteChessApplication::clipboard();
	cb->setText(s.readAll());
}

void MainWindow::onGameManagerFinished()
{
	m_readyToClose = true;
	close();
}

void MainWindow::closeAllGames()
{
	auto app = CuteChessApplication::instance();
	app->closeDialogs();

	for (int i = m_tabs.size() - 1; i >= 0; i--)
		closeTab(i);

	if (m_tabs.isEmpty())
		app->gameManager()->finish();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	if (m_readyToClose)
	{
		writeSettings();
		return QMainWindow::closeEvent(event); // clazy:exclude=returning-void-expression
	}

	if (askToSave())
	{
		m_closing = true;

		if (m_stopTournamentAct->isEnabled())
			m_stopTournamentAct->trigger();
		else
			closeAllGames();
	}

	event->ignore();
}

bool MainWindow::askToSave()
{
	if (isWindowModified())
	{
		QMessageBox::StandardButton result;
		result = QMessageBox::warning(this, QApplication::applicationName(),
			tr("�Ծ����б䶯.\n��Ҫ������?"),
				QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

		if (result == QMessageBox::Save)
			return save();
		else if (result == QMessageBox::Cancel)
			return false;
	}
	return true;
}

void MainWindow::adjudicateDraw()
{
	adjudicateGame(Chess::Side::NoSide);
}

void MainWindow::adjudicateWhiteWin()
{
	adjudicateGame(Chess::Side::White);
}

void MainWindow::adjudicateBlackWin()
{
	adjudicateGame(Chess::Side::Black);
}

void MainWindow::adjudicateGame(Chess::Side winner)
{
	if (!m_game)
		return;

	auto result = Chess::Result(Chess::Result::Adjudication,
				    winner,
				    tr("�û��þ�"));
	QMetaObject::invokeMethod(m_game, "onAdjudication",
				  Qt::QueuedConnection,
				  Q_ARG(Chess::Result, result));
}

OpeningBook* MainWindow::GetOpeningBook(int& depth) const   // �õ����ֿ����Ϣ
{
	QString file = QSettings().value("games/opening_book/file").toString();
	if (file.isEmpty())
		return nullptr;
	auto mode = OpeningBook::BookRandom;

	bool bestM = QSettings().value("games/opening_book/disk_access").toBool();
	if (bestM) {
		mode = OpeningBook::BookBest;
	}

	depth = QSettings().value("games/opening_book/depth").toInt();

	auto book = new PolyglotBook(mode);

	if (!book->read(file))
	{
		delete book;
		return nullptr;
	}

	return book;
}

void MainWindow::resignGame()
{
	if (m_game.isNull() || m_game->isFinished())
		return;

	ChessPlayer * player = m_game->playerToMove();
	if (!player->isHuman())
	{
		player = m_game->playerToWait();
		if (!player->isHuman())
			return;
	}
	Chess::Side side = player->side();
	auto result = Chess::Result(Chess::Result::Resignation,
				    side.opposite());
	QMetaObject::invokeMethod(m_game, "onResignation",
				  Qt::QueuedConnection,
				  Q_ARG(Chess::Result, result));
}

// ������Ϣ
void MainWindow::processCapMsg(Chess::stCaptureMsg msg)
{
	// �õ���ǰ����Ϸ�����ǣ�Ӧ�õõ���ǰ��chessgame
	
	switch (msg.mType) {
	case Chess::stCaptureMsg::eText:
		QMessageBox::warning(this, msg.title, msg.text);
		break;
	case Chess::stCaptureMsg::eMove:
		// �ж��߲��ǲ��ǺϷ�
		if (isMoveValid(msg.m) == false) {  // ��ֹ�ظ������߲�
			return;   
		}
		m_gameViewer->viewLinkMove(msg.m);
		break;
	case Chess::stCaptureMsg::eSetFen:
	{
		QString fen = msg.text;
		if (this->tbtnLinkAuto->isChecked()) {

			//if (m_pcap == nullptr)
			//	m_pcap = new Chess::Capture(this);

			//while (m_pcap->m_isRuning) {
			//	m_pcap->on_stop();
			//	this->wait(1);
			//	m_game->stop();
			//}
			

			if (fen.contains("w -", Qt::CaseSensitive)) {
				//m_onLinkRedToggled = true;
				this->tbtnLinkChessBoardRed->setChecked(true);
				this->tbtnLinkChessBoardBlack->setChecked(false);
			}
			else {
				this->tbtnLinkChessBoardRed->setChecked(false);
				this->tbtnLinkChessBoardBlack->setChecked(true);
			}			

			//m_pcap->m_bMainGetFenAlready = true;
			//m_pcap->on_start();

		}

		if (this->tbtnLinkChessBoardRed->isChecked() || this->tbtnLinkChessBoardBlack->isChecked()) {  // �췽��������
			bool ok = true;

			// �õ���ǰ������
			QSettings s;

			s.beginGroup("games");
			//const QString variant1 = s.value("variant").toString();
			const QString variant = "standard"; // s.value("variant").toString();    // ��Ϸ����

			auto board = Chess::BoardFactory::create(variant);
			//board->SetAutoLinkStat(true);

			//QString fen = "2bakab2/9/9/8R/pnpNP4/3r5/c3c4/1C2B2C1/4A4/2BAK4 w - - 0 1";

			//QString fen(m_gameViewer->board()->fenString());
			//if (!fen.isEmpty() && !board->setFenString(fen))
			//{
				//auto palette = ui->m_fenEdit->palette();
				//palette.setColor(QPalette::Text, Qt::red);
				//ui->m_fenEdit->setPalette(palette);
				//m_isValid = false;
				//emit statusChanged(false);
			//	fen = "";
			//}
			//board->setFenString(fen);

			auto pgn = new PgnGame();
			pgn->m_autoLink = true;           // ������ʱΪ0
			pgn->setSite(QSettings().value("pgn/site").toString());
			auto game = new ChessGame(board, pgn);
			game->isLinkBoard = true;        // ������ֽ�����ֱ��ɾ��
	

			game->setStartingFen(fen);
			//this->m_gameViewer->viewPreviousMove2(game->board());  //

			// ʱ�����
			TimeControl m_timeControl;
			m_timeControl.readSettings(&s);
			game->setTimeControl(m_timeControl);

			// �ö�����
			s.beginGroup("draw_adjudication");
			GameAdjudicator m_adjudicator;
			m_adjudicator.setDrawThreshold(s.value("move_number").toInt(),
				s.value("move_count").toInt(),
				s.value("score").toInt());
			s.endGroup();
			game->setAdjudicator(m_adjudicator);

			//auto suite = ui->m_gameSettings->openingSuite();          // ���ֳ�ʼ�����趨
			//if (suite)
			//{
			//	int depth = ui->m_gameSettings->openingSuiteDepth();
			//	ok = game->setMoves(suite->nextGame(depth));
			//	delete suite;
			//}


			//connect(ui->m_polyglotFileEdit, &QLineEdit::textChanged,
			//	[=](const QString& file)
			//	{
			//		QSettings().setValue("games/opening_book/file", file);
			//	});
			//connect(ui->m_polyglotDepthSpin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
			//	[=](int depth)
			//	{
			//		QSettings().setValue("games/opening_book/depth", depth);
			//	});

		  // QString 

			int depth;
			auto book = this->GetOpeningBook(depth);

			//auto book = ui->m_gameSettings->openingBook();           // ���ֿ�
			//if (book)
			//{
			//	int depth = ui->m_gameSettings->bookDepth();
			//	game->setBookOwnership(true);

			//	for (int i = 0; i < 2; i++)
			//	{
			//		auto side = Chess::Side::Type(i);
			//		if (playerType(side) == CPU)
			//			game->setOpeningBook(book, side, depth);
			//	}
			//}

			if (book) {
				if (this->tbtnLinkChessBoardRed->isChecked()) {
					game->setOpeningBook(book, Chess::Side::White, depth);
				}
				if (this->tbtnLinkChessBoardBlack->isChecked()) {
					game->setOpeningBook(book, Chess::Side::Black, depth);
				}
			}

			s.endGroup();   // 

			if (!ok)
			{
				delete game;
				QMessageBox::critical(this, tr("Could not initialize game"),
					tr("The game could not be initialized "
						"due to an invalid opening."));
				return;
			}

			//auto game = this->m_game;

			game->isGetSetting = true;    // ��������ú���


			//bool isWhiteCPU = (side == Chess::Side::White);
			//bool isBlackCPU = (side == Chess::Side::Black);

			PlayerBuilder* builders[2] = {
				mainCreatePlayerBuilder(Chess::Side::White, this->tbtnLinkChessBoardRed->isChecked()),
				mainCreatePlayerBuilder(Chess::Side::Black, this->tbtnLinkChessBoardBlack->isChecked())
			};

			//EvalWidget* m_evalWidgets[2];
			if (this->tbtnLinkChessBoardRed->isChecked()) {
				m_whiteEvalDock->show();
				m_blackEvalDock->hide();
			}
			if (this->tbtnLinkChessBoardBlack->isChecked()) {
				m_blackEvalDock->show();
				m_whiteEvalDock->hide();
			}
			

			// 
			////this->m_w

			//Chess::Side sss = game->board()->sideToMove();

			//if (builders[game->board()->sideToMove()]->isHuman())
			//	game->pause();


			m_myClosePreTab = true; // ����ǰһ������

			// Start the game in a new tab
			connect(game, SIGNAL(initialized(ChessGame*)),
				this, SLOT(addGame(ChessGame*)));
			connect(game, SIGNAL(startFailed(ChessGame*)),
				this, SLOT(onGameStartFailed(ChessGame*)));
			CuteChessApplication::instance()->gameManager()->newGame(game,             // ������������ӵ�Tab����
				builders[Chess::Side::White], builders[Chess::Side::Black]);      

			// ���������߲���capture
			// void moveMade(const Chess::GenericMove& move,
			//nst QString& sanString,
			//	const QString& comment);

			//connect(game, SIGNAL(moveMade(Chess::GenericMove, QString, QString)),
			//	m_scene, SLOT(makeMove(Chess::GenericMove)));

			connect(game, SIGNAL(moveMade(Chess::GenericMove, QString, QString)),
				m_pcap, SLOT(ProcessBoardMove(const Chess::GenericMove &)));

			// ����Ǻ췽�ߣ��Ͳ���Ҫ��ת����
			//connect(m_flipBoardAct, SIGNAL(triggered()),
			//	m_gameViewer->boardScene(), SLOT(flip()));

			//m_gameViewer->boardScene()->flip();
		}
	}
		break;
	default:
		break;
	}
}

void MainWindow::onLXchessboardStart(){

	QString catName = this->cbtnLinkBoard->currentText();

	if(m_pcap==nullptr)
		m_pcap = new Chess::Capture(this, catName);

	m_pcap->on_start();
}

void MainWindow::onLXchessboardStop(){
	
	m_pcap->on_stop();
	//delete pcap;

	m_game->stop(); 
}

PlayerBuilder* MainWindow::mainCreatePlayerBuilder(Chess::Side side, bool isCPU) const
{
	(void)side;

	if (isCPU) //(playerType(side) == CPU)
	{
		
		//auto config =  m_engineConfig[side];
		
		EngineManager* engineManager = CuteChessApplication::instance()->engineManager();

		int sel = this->cbtnLinkEngine->currentIndex();   // ����ѡ��
		auto config = engineManager->engineAt(sel);

		QSettings s;
		s.beginGroup("games");

		bool ponder = s.value("pondering").toBool();
		config.setPondering(ponder);
		// �Ƿ�Ҫ��̨˼��
		// ui->m_gameSettings->applyEngineConfiguration(&config);

		s.endGroup();

		return new EngineBuilder(config);
		
	}
	bool ignoreFlag = QSettings().value("games/human_can_play_after_timeout",
		true).toBool();
	return new HumanBuilder(CuteChessApplication::userName(), ignoreFlag);
}

// �췽����˼����ť
void MainWindow::onPlayRedToggled(bool checked) {

	//this->mainToolbar->setVisible(true);
	
	// ��ֹ������ť
	tbtnLinkChessBoardRed->setDisabled(checked);
	tbtnLinkChessBoardBlack->setDisabled(checked);
	tbtnLinkAuto->setDisabled(checked);

	this->onPlayWhich();  //  , Chess::Side::White);	

}

// �ڷ�����˼����ť
void MainWindow::onPlayBlackToggled(bool checked) {

	
	
	// ��ֹ������ť
	tbtnLinkChessBoardRed->setDisabled(checked);
	tbtnLinkChessBoardBlack->setDisabled(checked);
	tbtnLinkAuto->setDisabled(checked);

	this->onPlayWhich(); // , Chess::Side::Black);
}

void MainWindow::onPlayWhich() //, Chess::Side side)
{
	//(void)checked;
	if (this->tbtnEnginePlayRed->isChecked() || this->tbtnEnginePlayBlack->isChecked()) {

		
		//EngineConfiguration config = CuteChessApplication::instance()->engineManager()->engineAt(0);
		//m_tabs.at(m_tabBar->currentIndex()).m_game->setPlayer(Chess::Side::White, (new EngineBuilder(config))->create(nullptr, nullptr, this, nullptr));

		//auto cur_game = m_tabs.at(m_tabBar->currentIndex()).m_game;
		//this->m_game

		//QString stFen = m_game->startingFen();
		//QVector<Chess::Move> moves = m_game->moves();

		//auto game = this->m_game;

		//if (this->m_game->isGetSetting == false) {
		//if(1){

			//int preIndex = m_tabBar->currentIndex();

			//EngineManager* engineManager = CuteChessApplication::instance()->engineManager();			
			bool ok = true;

			// �õ���ǰ������
			QSettings s;

			s.beginGroup("games");
			//const QString variant1 = s.value("variant").toString();
			const QString variant = "standard"; // s.value("variant").toString();    // ��Ϸ����

			auto board = Chess::BoardFactory::create(variant);

			//QString fen = "2bakab2/9/9/8R/pnpNP4/3r5/c3c4/1C2B2C1/4A4/2BAK4 w - - 0 1";

			QString fen(m_gameViewer->board()->fenString());
			//if (!fen.isEmpty() && !board->setFenString(fen))
			//{
				//auto palette = ui->m_fenEdit->palette();
				//palette.setColor(QPalette::Text, Qt::red);
				//ui->m_fenEdit->setPalette(palette);
				//m_isValid = false;
				//emit statusChanged(false);
			//	fen = "";
			//}

			auto pgn = new PgnGame();
			pgn->setSite(QSettings().value("pgn/site").toString());
			auto game = new ChessGame(board, pgn);

			game->setStartingFen(fen);

			// ʱ�����
			TimeControl m_timeControl;
			m_timeControl.readSettings(&s);
			game->setTimeControl(m_timeControl);

			// �ö�����
			s.beginGroup("draw_adjudication");
			GameAdjudicator m_adjudicator;
			m_adjudicator.setDrawThreshold(s.value("move_number").toInt(),
				s.value("move_count").toInt(),
				s.value("score").toInt());
			s.endGroup();
			game->setAdjudicator(m_adjudicator);

			//auto suite = ui->m_gameSettings->openingSuite();          // ���ֳ�ʼ�����趨
			//if (suite)
			//{
			//	int depth = ui->m_gameSettings->openingSuiteDepth();
			//	ok = game->setMoves(suite->nextGame(depth));
			//	delete suite;
			//}

			

			//auto book = ui->m_gameSettings->openingBook();           // ���ֿ�
			//if (book)
			//{
			//	int depth = ui->m_gameSettings->bookDepth();
			//	game->setBookOwnership(true);

			//	for (int i = 0; i < 2; i++)
			//	{
			//		auto side = Chess::Side::Type(i);
			//		if (playerType(side) == CPU)
			//			game->setOpeningBook(book, side, depth);
			//	}
			//}

			s.endGroup();   // 

			if (!ok)
			{
				delete game;
				QMessageBox::critical(this, tr("Could not initialize game"),
					tr("The game could not be initialized "
						"due to an invalid opening."));
				return;
			}

			//auto game = this->m_game;

			game->isGetSetting = true;    // ��������ú���


			//bool isWhiteCPU = (side == Chess::Side::White);
			//bool isBlackCPU = (side == Chess::Side::Black);

			int depth;
			auto book = this->GetOpeningBook(depth);

			//auto book = ui->m_gameSettings->openingBook();           // ���ֿ�
			//if (book)
			//{
			//	int depth = ui->m_gameSettings->bookDepth();
			//	game->setBookOwnership(true);

			//	for (int i = 0; i < 2; i++)
			//	{
			//		auto side = Chess::Side::Type(i);
			//		if (playerType(side) == CPU)
			//			game->setOpeningBook(book, side, depth);
			//	}
			//}

			if (book) {
				if (this->tbtnEnginePlayRed->isChecked()) {
					game->setOpeningBook(book, Chess::Side::White, depth);
				}
				if (this->tbtnEnginePlayBlack->isChecked()) {
					game->setOpeningBook(book, Chess::Side::Black, depth);
				}
			}


			//EvalWidget* m_evalWidgets[2];
			if (this->tbtnEnginePlayRed->isChecked()) {
				m_whiteEvalDock->show();
				m_blackEvalDock->hide();
			}
			if (this->tbtnEnginePlayBlack->isChecked()) {
				m_blackEvalDock->show();
				m_whiteEvalDock->hide();
			}


			PlayerBuilder* builders[2] = {
				mainCreatePlayerBuilder(Chess::Side::White, this->tbtnEnginePlayRed->isChecked()),
				mainCreatePlayerBuilder(Chess::Side::Black, this->tbtnEnginePlayBlack->isChecked())
			};


			if (builders[game->board()->sideToMove()]->isHuman())
				game->pause();


			m_myClosePreTab = true; // ����ǰһ������

			// Start the game in a new tab
			connect(game, SIGNAL(initialized(ChessGame*)),
				this, SLOT(addGame(ChessGame*)));
			connect(game, SIGNAL(startFailed(ChessGame*)),
				this, SLOT(onGameStartFailed(ChessGame*)));
			CuteChessApplication::instance()->gameManager()->newGame(game,             // ������������ӵ�Tab����
				builders[Chess::Side::White], builders[Chess::Side::Black]);

			//if (m_tabs.size() >= 2) {
			//	closeTab(preIndex);
			//}

		//}
	}
	else { // ֹͣ����
	    // destroyGame(m_game);
		// this->m_game->isGetSetting = false;
		//resignGame();
		//closeTab(m_tabBar->currentIndex());
		//int ci = m_tabBar->currentIndex();
		//int count = m_tabBar->count();

		//if (m_tabs.size() >= 2){
		//	closeTab(m_tabBar->currentIndex());
		//}

		//int currentIndex() const;
		//int count() const;
		m_game->stop();
	}
}

void MainWindow::onLinkRedToggled(bool checked)
{
	if (tbtnLinkAuto->isChecked()) return;  
	
	tbtnEnginePlayRed->setDisabled(checked);
	tbtnEnginePlayBlack->setDisabled(checked);
	tbtnLinkChessBoardBlack->setDisabled(checked);
	tbtnLinkAuto->setDisabled(checked);

	this->onLinkWhich(checked);  //  , Chess::Side::White);
}
void MainWindow::onLinkBlackToggled(bool checked)
{
	if (tbtnLinkAuto->isChecked()) return;  
	
	tbtnEnginePlayRed->setDisabled(checked);
	tbtnEnginePlayBlack->setDisabled(checked);
	tbtnLinkChessBoardRed->setDisabled(checked);
	tbtnLinkAuto->setDisabled(checked);

	this->onLinkWhich(checked);  //  , Chess::Side::White);
}

void MainWindow::onLinkAutomaticToggled(bool checked)
{
	tbtnEnginePlayRed->setDisabled(checked);
	tbtnEnginePlayBlack->setDisabled(checked);
	tbtnLinkChessBoardRed->setDisabled(checked);
	tbtnLinkChessBoardBlack->setDisabled(checked);

	//
	QString catName = this->cbtnLinkBoard->currentText();
	
	if (checked) {
		if (m_autoClickCap == nullptr)
			m_autoClickCap = new Chess::Capture(this,catName,true);
		m_autoClickCap->on_start();

		if (m_pcap == nullptr)
			m_pcap = new Chess::Capture(this, catName);

		while (m_pcap->isRunning()) {
			m_pcap->on_stop();
			this->wait(1);
			m_game->stop();
		}
		m_pcap->on_start();	
	}
	else {

		while (m_pcap->isRunning()) {
			m_autoClickCap->on_stop();
			m_pcap->on_stop();
			m_game->stop();
		}

		this->tbtnLinkChessBoardRed->setChecked(false);
		this->tbtnLinkChessBoardBlack->setChecked(false);

		//m_bAutomaticLinking = false;
	}
}

void MainWindow::onLinkWhich(bool checked)
{
	if (checked) {
		onLXchessboardStart();  // ��ʼ����
	}
	else {
		onLXchessboardStop();
	}
}

void MainWindow::onLinkBoardCombox(const QString& txt)
{
	if (this->m_pcap != nullptr) {
		this->m_pcap->SetCatlogName(txt);
	}
	if (this->m_autoClickCap != nullptr) {
		this->m_autoClickCap->SetCatlogName(txt);
	}
}
//void MainWindow::onChangeBoardPicAct()
//{
//	this->m_gameViewer->boardScene()->changeBoardPicture();
//}
/*
void MainWindow::showContextMenu(const QPoint& pos)
{
	int a = 0;
}*/


void MainWindow::addDefaultWindowMenu()
{
	m_windowMenu->addAction(m_minimizeAct);
	m_windowMenu->addSeparator();
	m_windowMenu->addAction(m_showGameWallAct);
	m_windowMenu->addSeparator();
	m_windowMenu->addAction(m_showPreviousTabAct);
	m_windowMenu->addAction(m_showNextTabAct);
}
