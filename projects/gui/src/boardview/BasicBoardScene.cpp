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

#pragma execution_character_set("utf-8")

#include "BasicBoardScene.h"
#include <QSvgRenderer>
#include <QGraphicsSceneMouseEvent>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QGraphicsPolygonItem>
#include <QGraphicsTextItem>
#include <QSettings>
#include <memory>
#include <algorithm>
#include <board/board.h>
#include "graphicsboard.h"
#include "graphicspiecereserve.h"
#include "graphicspiece.h"
#include "piecechooser.h"
#include "pgngame.h"
#include "board/boardfactory.h"

namespace {

	const qreal s_squareSize = 50;

} // anonymous namespace

BasicBoardScene::BasicBoardScene(QObject* parent)
	: QGraphicsScene(parent),
	m_squares(nullptr),
	m_renderer(new QSvgRenderer((QCoreApplication::applicationDirPath() + "/image/default.svg"), this))
{
	this->setBackgroundBrush(QColor(0xf6, 0xf5, 0xf0));  // 纸质书的感觉'

	PgnGame pgn;
	m_boards[0] = pgn.createBoard(); // start board
	m_boards[1] = nullptr;
	m_boards[2] = Chess::BoardFactory::create(pgn.variant());
	m_boards[2]->setFenString("3k5/9/9/9/9/9/9/9/9/5K3 w");
}

BasicBoardScene::~BasicBoardScene()
{
	delete m_boards[0];
	delete m_boards[2];
}


void BasicBoardScene::setBoard(Chess::Board* board)
{
	clear();
	m_boards[1] = board;
}

QString BasicBoardScene::fenString() const
{
	std::unique_ptr<Chess::Board> btmp{ Chess::BoardFactory::create("standard") };
	btmp->reset();
	for (int x = 0; x < btmp->width(); x++)
	{
		for (int y = 0; y < btmp->height(); y++)
		{
			Chess::Square sq(x, y);
			GraphicsPiece* gpiece = m_squares->pieceAt(sq);
			if (gpiece) {
				btmp->setSquare(btmp->squareIndex(sq), gpiece->pieceType());
			}
			else {
				btmp->setSquare(btmp->squareIndex(sq), Chess::Piece::NoPiece);
			}
		}
	}
	btmp->setSideToMove(m_redFirst ? Chess::Side::White : Chess::Side::Black);
	return btmp->fenString();
}

void BasicBoardScene::populate(int idx)
{
	Chess::Board* startBoard = m_boards[0];
	Q_ASSERT(startBoard != nullptr);

	clear();

	m_squares = new GraphicsBoard(startBoard->width(), startBoard->height(), s_squareSize);
	addItem(m_squares);

	m_reserve = new GraphicsPieceReserve(s_squareSize);
	m_reserve->setX(m_squares->boundingRect().right() +
			        m_reserve->boundingRect().right() + 7);
	addItem(m_reserve);

	setSceneRect(itemsBoundingRect());

	for (int x = 0; x < startBoard->width(); x++)
	{
		for (int y = 0; y < startBoard->height(); y++)
		{
			Chess::Square sq(x, y);
			GraphicsPiece* piece(createPiece(startBoard->pieceAt(sq)));   // 放上棋子

			if (piece != nullptr) {
				piece->setFlag(QGraphicsItem::ItemIsMovable, true);
				m_reserve->addPiece(piece);
			}
		}
	}

	Chess::Board* activeBoard = m_boards[idx];
	Q_ASSERT(activeBoard != nullptr);

	for (int x = 0; x < startBoard->width(); x++)
	{
		for (int y = 0; y < startBoard->height(); y++)
		{
			Chess::Square sq(x, y);
			Chess::Piece piece = activeBoard->pieceAt(sq);

			if (piece.isValid()) {
				GraphicsPiece* gpiece(m_reserve->takePiece(piece));
				gpiece->setFlag(QGraphicsItem::ItemIsMovable, true);
				m_squares->setSquare(sq, gpiece);
			}
		}
	}
}


void BasicBoardScene::flip()
{
	Chess::Board* startBoard = m_boards[0];

	m_squares->setFlipped(!m_squares->isFlipped());

	QParallelAnimationGroup* group = new QParallelAnimationGroup;

	for (int y = 0; y < startBoard->height(); y++)
	{
		for (int x = 0; x < startBoard->width(); x++)
		{
			auto sq = Chess::Square(x, y);
			auto pc = m_squares->pieceAt(sq);
			if (!pc)
				continue;
			pc->setPos(m_squares->squarePos(sq));
		}
	}

	group->start(QAbstractAnimation::DeleteWhenStopped);
}


void BasicBoardScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	GraphicsPiece* piece = pieceAt(event->scenePos());

	if (piece == nullptr)
		return;

	m_sourcePos = piece->scenePos();
	m_sourceParent = piece->parentItem();
	Q_ASSERT(m_sourceParent != nullptr);

	if (piece->parentItem() == m_squares)
		m_squares->takePieceAt(m_squares->squareAt(piece->pos()));
	else
		m_reserve->takePiece(piece->pieceType());

	piece->setParentItem(nullptr);
	piece->setPos(m_sourcePos);

	QGraphicsScene::mousePressEvent(event);
}

void BasicBoardScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	GraphicsPiece* piece = nullptr;
	if (event->button() == Qt::LeftButton) {
		GraphicsPiece* piece = qgraphicsitem_cast<GraphicsPiece*>(mouseGrabberItem());
		if (piece == nullptr)
			return QGraphicsScene::mouseReleaseEvent(event);

		auto items = piece->collidingItems();

		for (QGraphicsItem* item : items) {
			if (item == m_squares) {
				QPointF targetPos(m_squares->mapFromScene(event->scenePos()));
				Chess::Square target(m_squares->squareAt(targetPos));
				GraphicsPiece* pieceOld = m_squares->takePieceAt(target);
				if (pieceOld != nullptr)
					m_reserve->addPiece(pieceOld);
				if (target.isValid()) {
					m_squares->setSquare(target, piece);
					return QGraphicsScene::mouseReleaseEvent(event);
				}
			}
			else if (item == m_reserve) {
				m_reserve->addPiece(piece);
				return QGraphicsScene::mouseReleaseEvent(event);
			}
		}

		// 棋子落在不该落的地方， 放回棋子
		piece->setParentItem(m_sourceParent);
		piece->setPos(m_sourcePos);
	}
	else if (event->button() == Qt::RightButton) {
		piece = pieceAt(event->scenePos());
		if (piece == nullptr)
			return QGraphicsScene::mouseReleaseEvent(event);

		m_reserve->addPiece(piece);
	}

	QGraphicsScene::mouseReleaseEvent(event);
}


QPointF BasicBoardScene::squarePos(const Chess::Square& square) const
{
	return m_squares->mapToScene(m_squares->squarePos(square));
}

GraphicsPiece* BasicBoardScene::pieceAt(const QPointF& pos) const
{
	const auto items = this->items(pos);
	for (auto item : items)
	{
		GraphicsPiece* piece = qgraphicsitem_cast<GraphicsPiece*>(item);
		if (piece != nullptr)
			return piece;
	}

	return nullptr;
}

GraphicsPiece* BasicBoardScene::createPiece(const Chess::Piece& piece)
{
	Q_ASSERT(m_boards[0] != nullptr);
	Q_ASSERT(m_renderer != nullptr);
	Q_ASSERT(m_squares != nullptr);

	if (!piece.isValid())
		return nullptr;

	return new GraphicsPiece(piece,
		s_squareSize,
		m_boards[0]->representation(piece),
		m_renderer);
}
