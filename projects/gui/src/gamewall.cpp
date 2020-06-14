

#pragma execution_character_set("utf-8")

#include "gamewall.h"

#include <QPointer>

#include <chessplayer.h>
#include <chessgame.h>
#include <gamemanager.h>

#include "tilelayout.h"
#include "boardview/boardscene.h"
#include "boardview/boardview.h"
#include "chessclock.h"
#include "cutechessapp.h"


class GameWallWidget : public QWidget
{
	Q_OBJECT

	public:
		explicit GameWallWidget(QWidget* parent);
		virtual ~GameWallWidget();

		void setGame(ChessGame* game);

	private:
		ChessClock* m_clocks[2];
		BoardScene* m_scene;
		BoardView* m_view;
		QPointer<ChessPlayer> m_players[2];
};

GameWallWidget::GameWallWidget(QWidget* parent)
	: QWidget(parent)
{
	QHBoxLayout* clockLayout = new QHBoxLayout();
	for (int i = 0; i < 2; i++)
	{
		m_players[i] = nullptr;

		m_clocks[i] = new ChessClock();
		clockLayout->addWidget(m_clocks[i]);

		Chess::Side side = Chess::Side::Type(i);
		m_clocks[i]->setPlayerName(side.toString());
	}
	clockLayout->insertSpacing(1, 20);

	m_scene = new BoardScene(this);
	m_view = new BoardView(m_scene);

	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->addLayout(clockLayout);
	mainLayout->addWidget(m_view);
	mainLayout->setContentsMargins(0, 0, 0, 0);

	setLayout(mainLayout);

	//this->setContextMenuPolicy(Qt::CustomContextMenu);  // ÓÒ¼ü²Ëµ¥
	//connect(this, SIGNAL(customContextMenuRequested), this, SLOT(showContextMenu));
}

GameWallWidget::~GameWallWidget()
{
}

void GameWallWidget::setGame(ChessGame* game)
{
	game->lockThread();
	connect(game, SIGNAL(fenChanged(QString)),
		m_scene, SLOT(setFenString(QString)));
	connect(game, SIGNAL(moveMade(Chess::GenericMove, QString, QString)),
		m_scene, SLOT(makeMove(Chess::GenericMove)));
	connect(game, SIGNAL(humanEnabled(bool)),
		m_view, SLOT(setEnabled(bool)));
	connect(game, SIGNAL(finished(ChessGame*, Chess::Result)),
		m_scene, SLOT(onGameFinished(ChessGame*, Chess::Result)));

	for (int i = 0; i < 2; i++)
	{
		if (m_players[i])
			m_players[i]->disconnect(m_clocks[i]);

		ChessPlayer* player(game->player(Chess::Side::Type(i)));
		m_players[i] = player;

		if (player->isHuman())
			connect(m_scene, SIGNAL(humanMove(Chess::GenericMove, Chess::Side)),
				player, SLOT(onHumanMove(Chess::GenericMove, Chess::Side)));

		m_clocks[i]->setPlayerName(player->name());
		connect(player, SIGNAL(nameChanged(QString)),
			m_clocks[i], SLOT(setPlayerName(QString)));

		m_clocks[i]->setInfiniteTime(player->timeControl()->isInfinite());

		if (player->state() == ChessPlayer::Thinking)
			m_clocks[i]->start(player->timeControl()->activeTimeLeft());
		else
			m_clocks[i]->setTime(player->timeControl()->timeLeft());

		connect(player, SIGNAL(startedThinking(int)),
			m_clocks[i], SLOT(start(int)));
		connect(player, SIGNAL(stoppedThinking()),
			m_clocks[i], SLOT(stop()));
	}

	m_scene->setBoard(game->pgn()->createBoard());
	m_scene->populate();

	if (game->boardShouldBeFlipped())
		m_scene->flip();

	for (const Chess::Move& move : game->moves())
		m_scene->makeMove(move);

	game->unlockThread();

	m_view->setEnabled(!game->isFinished() &&
			   game->playerToMove()->isHuman());
}


GameWall::GameWall(GameManager* manager, QWidget *parent)
	: QWidget(parent)
{
	Q_ASSERT(manager != nullptr);

	setLayout(new TileLayout());

	const auto activeGames = manager->activeGames();
	for (ChessGame* game : activeGames)
	{
		if (!game->isFinished())
			addGame(game);
	}

	connect(manager, SIGNAL(gameStarted(ChessGame*)),
		this, SLOT(addGame(ChessGame*)));
	connect(manager, SIGNAL(gameDestroyed(ChessGame*)),
		this, SLOT(removeGame(ChessGame*)));
}

GameWallWidget* GameWall::getFreeWidget()
{
	if (!m_gamesToRemove.isEmpty())
		return m_gamesToRemove.takeFirst();

	auto widget = new GameWallWidget(this);
	layout()->addWidget(widget);

	return widget;
}

void GameWall::addGame(ChessGame* game)
{
	Q_ASSERT(game != nullptr);

	if (m_games.contains(game))
		return;

	auto widget = getFreeWidget();
	widget->setGame(game);

	m_games[game] = widget;
}

void GameWall::removeGame(ChessGame* game)
{
	if (!m_games.contains(game))
		return;
	m_gamesToRemove.append(m_games.take(game));
}

//void GameWall::showContextMenu(const QPoint& pos)
//{
//	int a = 0;
//}

#include "gamewall.moc"
