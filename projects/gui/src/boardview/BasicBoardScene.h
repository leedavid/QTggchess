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

#ifndef BASICBOARDSCENE_H
#define BASICBOARDSCENE_H

#include <QGraphicsScene>
#include <QMultiMap>
#include <QPointer>
#include <board/square.h>
#include <board/genericmove.h>
#include <board/boardtransition.h>
#include <board/result.h>
#include "graphicspiecereserve.h"
namespace Chess
{
	class Board;
	class Move;
	class Side;
	class Piece;
}
class ChessGame;
class QSvgRenderer;
class QAbstractAnimation;
class QPropertyAnimation;
class GraphicsBoard;
class GraphicsPieceReserve;
class GraphicsPiece;
class PieceChooser;


/*!
 * \brief A graphical surface for displaying a chessgame.
 *
 */
class BasicBoardScene : public QGraphicsScene
{
	Q_OBJECT

public:
	explicit BasicBoardScene(QObject* parent = nullptr);
	virtual ~BasicBoardScene();

	/*! Returns the current internal board object. */
	Chess::Board* board() const;
	/*!
	 * Clears the scene and sets \a board as the internal board.
	 *
	 * The scene takes ownership of the board, so it's usually
	 * best to give the scene its own copy of a board.
	 */
	void setBoard(Chess::Board* board);

	QString fenString() const;

	void setRedFirst(bool redFist) {
		m_redFirst = redFist;
	}

public slots:
	/*!
	 * Clears the scene, creates a new board, and populates
	 * it with chess pieces.
	 */
	void populate(int idx);

	/*! Flips the board, with animation. */
	void flip();

signals:
	/*!
	 * This signal is emitted when a human player has made a move.
	 *
	 * The move is guaranteed to be legal.
	 * The move was made by the player on \a side side.
	 */
	void humanMove(const Chess::GenericMove& move,
		const Chess::Side& side);

protected:
	// Inherited from QGraphicsScene
	virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

private slots:


private:

	QPointF squarePos(const Chess::Square& square) const;
	GraphicsPiece* pieceAt(const QPointF& pos) const;
	GraphicsPiece* createPiece(const Chess::Piece& piece);
	
	bool m_redFirst;
	Chess::Board* m_boards[3];
	QPointF m_sourcePos;
	QGraphicsItem* m_sourceParent = nullptr;
	GraphicsBoard* m_squares = nullptr;
	GraphicsPieceReserve* m_reserve = nullptr;
	QSvgRenderer* m_renderer = nullptr;
};

#endif // BASICBOARDSCENE_H
