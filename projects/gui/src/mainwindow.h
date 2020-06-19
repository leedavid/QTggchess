/*
    This file is part of GGzero Chess.
    Copyright (C) 2008-2018 GGzero Chess authors

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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPointer>
#include <QToolbutton>
#include <QCombobox>
#include <board/side.h>
#include <QGraphicsScene>
#include <humanbuilder.h>
#include <QMutex>

#include "capture.h"
#include "linkboard.h"
#include "translatingslider.h"
//#include "TestThread.h"

namespace Chess {
	class Board;
	class Move;
}
class QMenu;
class QAction;
class QCloseEvent;
class QTabBar;
class GameViewer;
class MoveList;
class PlainTextLog;
class PgnGame;
class ChessGame;
class ChessPlayer;
class PgnTagsModel;
class Tournament;
class GameTabBar;
class EvalHistory;
class EvalWidget;
class PgnStream;
class Capture;

/**
 * MainWindow
*/
class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		explicit MainWindow(ChessGame* game);
		virtual ~MainWindow();
		QString windowListTitle() const;

		
		//bool m_bLinkMayNewGame; 
		bool isMoveValid(const Chess::GenericMove& move);

		//TestThread* t;

	public slots:
		void addGame(ChessGame* game);

	protected:
		virtual void closeEvent(QCloseEvent* event);
		void closeCurrentGame();

	private slots:
		void slotEditBoard();
		void slotNewGame();
		void slotOpenPgnGame();
		void slotNewTournament();
		void onWindowMenuAboutToShow();
		void slotShowGameWindow();
		void slotUpdateWindowTitle();
		void slotUpdateMenus();
		bool slotSave();
		bool saveAs();
		void onTabChanged(int index);
		void onTabCloseRequested(int index);
		void slotCloseTab(int index);
		void slotDestroyGame(ChessGame* game);
		void onTournamentFinished();
		void onGameManagerFinished();
		void onGameStartFailed(ChessGame* game);
		void onGameFinished(ChessGame* game);
		void editMoveComment(int ply, const QString& comment);
		void onMouseRightClicked(/*QGraphicsSceneContextMenuEvent* event*/);
		void copyFen();
		void pasteFen();
		//void msgFen(QString fen);              // 这个是临时的
		void copyPgn();
		void showAboutDialog();
		void slotCloseAllGames();
		void slotAdjudicateDraw();
		void slotAdjudicateWhiteWin();
		void slotAdjudicateBlackWin();
		void slotResignGame();

		void slotProcessCapMsg(Chess::stCaptureMsg msg);

		// 連线棋盘
		void onLXchessboardStart();
		void onLXchessboardStop();

		PlayerBuilder* mainCreatePlayerBuilder(Chess::Side side, bool isCPU) const;
		
		void onPlayRedToggled(bool checked);	// 电脑执红	
		void onPlayBlackToggled(bool checked);  // 电脑执黑
		void onPlayWhich(); // , Chess::Side side);

		void onLinkRedToggled(bool checked); // 电脑执红连线
		void onLinkBlackToggled(bool checked); // 电脑执红连线
		void onLinkAutomaticToggled(bool checked);        
		void onLinkWhich(bool checked);

		void onLinkBoardCombox(const QString&);

		/** Value for Move Interval is changed */
		void slotMoveIntervalChanged(int);
		/** Value for Move Interval Slider is changed */
		void slotSetSliderText(int value = -1);

	private:
		struct TabData
		{
			explicit TabData(ChessGame* m_game, Tournament* m_tournament = nullptr);

			ChessGame* m_id;
			QPointer<ChessGame> m_game;
			PgnGame* m_pgn;
			Tournament* m_tournament;
			bool m_finished;			
		};

		void createActions();
		void createMenus();
		void createToolBars();
		void createDockWindows();
		void createStatus();             // 状态栏
		void readSettings();
		void writeSettings();
		QString genericTitle(const TabData& gameData) const;
		QString nameOnClock(const QString& name, Chess::Side side) const;
		void lockCurrentGame();
		void unlockCurrentGame();
		bool saveGame(const QString& fileName);
		bool askToSave();
		void setCurrentGame(const TabData& gameData);
		void removeGame(int index);
		int tabIndex(ChessGame* game) const;
		int tabIndex(Tournament* tournament, bool freeTab = false) const;
		void addDefaultWindowMenu();
		void adjudicateGame(Chess::Side winner);

		QMenu* m_gameMenu;
		QMenu* m_tournamentMenu;
		QMenu* m_toolsMenu;
		QMenu* m_viewMenu;
		QMenu* m_windowMenu;
		QMenu* m_helpMenu;

		QToolBar* mainToolbar;              // 主菜单工具条
		QToolButton* tbtnEnginePlayRed;     // 引擎执红
		QToolButton* tbtnEnginePlayBlack;   // 引擎执黑
		QToolButton* tbtnLinkChessBoardRed;      // 连接其它棋盘，红方电脑 
		QToolButton* tbtnLinkChessBoardBlack;    // 连接其它棋盘，黑方电脑

		

		QToolButton* tbtnLinkAuto;           // 全自动连接其它棋盘

		//QAction* actEngineThink;			// 让引擎思考
		QAction* actEngineStop;				// 让引擎停止思考，立即出步
		QAction* actEngineAnalyze;			// 让引擎分析
		QAction* actEngineSetting;			// 引擎设置参数


		GameTabBar* m_tabBar;		    

		GameViewer* m_gameViewer;
		MoveList* m_moveList;
		PgnTagsModel* m_tagsModel;

		QAction* m_quitGameAct;
		QAction* m_editBoardAct;
		QAction* m_newGameAct;
		QAction* m_openPgnAct;             
		QAction* m_adjudicateBlackWinAct;
		QAction* m_adjudicateWhiteWinAct;
		QAction* m_adjudicateDrawAct;
		QAction* m_resignGameAct;
		QAction* m_closeGameAct;
		QAction* m_saveGameAct;
		QAction* m_saveGameAsAct;
		QAction* m_copyFenAct;
		QAction* m_pasteFenAct;
		QAction* m_copyPgnAct;
		QAction* m_flipBoardAct;
		QAction* m_newTournamentAct;
		QAction* m_stopTournamentAct;
		QAction* m_showTournamentResultsAct;
		QAction* m_minimizeAct;
		QAction* m_showGameDatabaseWindowAct;
		QAction* m_showGameWallAct;
		QAction* m_showPreviousTabAct;
		QAction* m_showNextTabAct;
		QAction* m_aboutAct;
		QAction* m_showSettingsAct;

		QAction* m_changeBoardPicAct;         // 改变棋盘图片
		QAction* m_changeBoardBackGroundAct;  // 改变背景

		//QAction* m_showSettingsAct;

		PlainTextLog* m_engineDebugLog;

		EvalHistory* m_evalHistory;
		EvalWidget* m_evalWidgets[2];

		QDockWidget* m_whiteEvalDock;
		QDockWidget* m_blackEvalDock;

		QPointer<ChessGame> m_game;
		QPointer<ChessPlayer> m_players[2];
		QList<TabData> m_tabs;

		QString m_currentFile;
		bool m_closing;
		bool m_readyToClose;

		bool m_firstTabAutoCloseEnabled;
		bool m_myClosePreTab;

		Chess::Capture* m_pcap;            // 一个界面只有一个
		Chess::Capture* m_autoClickCap;    // 全自动挂机	

		// 状态栏 ---------------------------------------------------------------------------
		QLabel* m_status1;
		QLabel* m_status2;
		QLabel* m_status3;
		QComboBox* cbtnLinkBoard;             // 连线的棋盘
		QComboBox* cbtnLinkEngine;            // 连线的引擎

		QPointer<Chess::TranslatingSlider> m_sliderSpeed;
		QLabel* m_sliderText;

		// 状态栏 ---------------------------------------------------------------------------


		//bool m_bAutomaticLinking; 

		void wait(int msec) {
			//this->sleep
			QTime dieTime = QTime::currentTime().addMSecs(msec);
			while (QTime::currentTime() < dieTime)
				QCoreApplication::processEvents(QEventLoop::AllEvents, 100);			
		}

		OpeningBook* GetOpeningBook(int& depth) const;
		QString preverb();      // 谚语
};

#endif // MAINWINDOW_H
