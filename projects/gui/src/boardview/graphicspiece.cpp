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

#include "graphicspiece.h"
#include <QSvgRenderer>

#include <qdebug.h>

// 绘制棋子
GraphicsPiece::GraphicsPiece(const Chess::Piece& piece,
			     qreal squareSize,
			     const QString& elementId,
			     QSvgRenderer* renderer,
			     QGraphicsItem* parent)
	: QGraphicsObject(parent),
	  m_piece(piece),
	  m_rect(-squareSize / 2, -squareSize / 2,
		  squareSize, squareSize),
	  m_elementId(elementId),
	  m_renderer(renderer),
	  m_container(nullptr)
{
	squareSize *= 1.2;
	m_boundingRect.setRect(-squareSize / 2, -squareSize / 2,
		squareSize, squareSize);
	setAcceptedMouseButtons(Qt::LeftButton);
	setCacheMode(DeviceCoordinateCache);
}

int GraphicsPiece::type() const
{
	return Type;
}

	

QRectF GraphicsPiece::boundingRect() const
{
	return m_boundingRect;
}

void GraphicsPiece::paint(QPainter* painter,
			  const QStyleOptionGraphicsItem* option,
			  QWidget* widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	QRectF bounds(m_renderer->boundsOnElement(m_elementId));
	qreal ar = bounds.width() / bounds.height();
	qreal width = m_rect.width() * 0.95;  // was 0.8 棋子相对格子的比例

	if (pieceSelected) {
		width *= 1.2;
	}

	if (ar > 1.0)
	{
		bounds.setWidth(width);
		bounds.setHeight(width / ar);
	}
	else
	{
		bounds.setHeight(width);
		bounds.setWidth(width * ar);
	}
	bounds.moveCenter(m_rect.center());

	m_renderer->render(painter, m_elementId, bounds);
}

Chess::Piece GraphicsPiece::pieceType() const
{
	return m_piece;
}

QGraphicsItem* GraphicsPiece::container() const
{
	return m_container;
}

void GraphicsPiece::setContainer(QGraphicsItem* item)
{
	m_container = item;
}

void GraphicsPiece::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	pieceSelected = true;
	QGraphicsObject::mousePressEvent(event);
	update();
}

void GraphicsPiece::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	pieceSelected = false;
	QGraphicsObject::mouseReleaseEvent(event);
	update();
}

void GraphicsPiece::restoreParent()
{
	if (parentItem() == nullptr && m_container != nullptr)
	{
		QPointF point(m_container->mapFromScene(pos()));
		setParentItem(m_container);
		setPos(point);
	}
}
