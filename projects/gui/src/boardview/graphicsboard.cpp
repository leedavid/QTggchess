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

#include <board/square.h>
#include "graphicspiece.h"

#include "graphicsboard.h"
#include <QApplication>
#include <QMargins>
#include <QPainter>
#include <QPalette>
#include <QPropertyAnimation>
#include <QSettings>
#include <QDir>

namespace {

class TargetHighlights : public QGraphicsObject
{
	public:
		TargetHighlights(QGraphicsItem* parentItem = nullptr)
			: QGraphicsObject(parentItem)
		{
			setFlag(ItemHasNoContents);
		}
		virtual QRectF boundingRect() const
		{
			return QRectF();
		}
		virtual void paint(QPainter* painter,
				   const QStyleOptionGraphicsItem* option,
				   QWidget* widget)
		{
			Q_UNUSED(painter);
			Q_UNUSED(option);
			Q_UNUSED(widget);
		}
};

} // anonymous namespace

GraphicsBoard::GraphicsBoard(int files,
			     int ranks,
			     qreal squareSize,
			     QGraphicsItem* parent)
	: QGraphicsItem(parent),
	  m_files(files),
	  m_ranks(ranks),
	  m_squareSize(squareSize),
	  m_coordSize(squareSize / 2.0),
	  m_lightColor(QColor(0xff, 0xce, 0x9e)),
	  m_darkColor(QColor(0x20, 0x20, 0x20)), // m_darkColor(QColor(0xd1, 0x8b, 0x47)),
	  //m_darkColor(QColor(0xff, 0xff, 0xff)), // m_darkColor(QColor(0xd1, 0x8b, 0x47)),
	  //m_darkColor(QColor(0xaa, 0xaa, 0xaa)), // m_darkColor(QColor(0xd1, 0x8b, 0x47)),
	  m_squares(files * ranks),
	  m_highlightAnim(nullptr),
	  m_flipped(false)
{
	Q_ASSERT(files > 0);
	Q_ASSERT(ranks > 0);

	m_rect.setSize(QSizeF(squareSize * files, squareSize * ranks));
	m_rect.moveCenter(QPointF(0, 0));
	m_textColor = QApplication::palette().text().color();

	setCacheMode(DeviceCoordinateCache);

	this->getBoardPic();
}

void GraphicsBoard::getBoardPic()
{
	QString file = QSettings().value("ui/board_pic").toString();
	if (file == nullptr) {
		file = "board.png";
	}
	QString picPath = QCoreApplication::applicationDirPath() + "/image/board/" + file;
	QPixmap pix = QPixmap(picPath);
	//QSize pixSize = pix.size();
	//pixSize.scale(QSize( m_rect.width()*2, m_rect.height() * 2), Qt::AspectRatioMode::KeepAspectRatio);
	//QPixmap scaledPix = pix.scaled(pixSize,
	//	Qt::KeepAspectRatio,
	//	Qt::SmoothTransformation
	//);
	scaledPix = pix.scaled((m_rect.width() + 20), (m_rect.height() + 20));
}

void GraphicsBoard::changeBoardPicture()
{
	// 得到所有的文件
	QStringList nameFilters;
	nameFilters << "*.png";
	QString dirpath = QCoreApplication::applicationDirPath() + "/image/board/";
	QDir dir = QDir(dirpath);
	QStringList files = dir.entryList(nameFilters, QDir::Files | QDir::Readable, QDir::Name);

	QString prefile = QSettings().value("ui/board_pic").toString();
	if (prefile == nullptr) {
		prefile = "board.png";
	}
	int num = 0;
	for (QString file : files) {
		if (prefile == file) {
			break;
		}
		num++;
	}
	num++;

	if (num >= files.length()) {
		num = 0;
	}
	prefile = files[num];
	QSettings().setValue("ui/board_pic", prefile);

	getBoardPic();

	this->update();
}

GraphicsBoard::~GraphicsBoard()
{
	delete m_highlightAnim;
}

int GraphicsBoard::type() const
{
	return Type;
}

QRectF GraphicsBoard::boundingRect() const
{
	const auto margins = QMarginsF(m_coordSize, m_coordSize,
				       m_coordSize, m_coordSize);
	return m_rect.marginsAdded(margins);
}
// 绘制棋盘 坐标
// https://www.twblogs.net/a/5c10cacfbd9eee5e41838c05/zh-cn
void GraphicsBoard::paint(QPainter* painter,
			  const QStyleOptionGraphicsItem* option,
			  QWidget* widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	// 画背景图
	//QString pic = QCoreApplication::applicationDirPath() + "/image/bg.jpg";

    //QString picPath = QCoreApplication::applicationDirPath() + "/image/backgroud/board.png";	
	//QPixmap pix = QPixmap(picPath);
	//QSize pixSize = pix.size();
	//pixSize.scale(QSize( m_rect.width()*2, m_rect.height() * 2), Qt::AspectRatioMode::KeepAspectRatio);
	//QPixmap scaledPix = pix.scaled(pixSize,
	//	Qt::KeepAspectRatio,
	//	Qt::SmoothTransformation
	//);
	//QPixmap scaledPix = pix.scaled(m_rect.width()+125, m_rect.height()+125);

	//pixSize.scale(m_rect.size(), Qt::KeepAspectRatio);
	//painter->drawPixmap(0, 0, scaledPix);

	painter->drawPixmap(m_rect.left()-10,m_rect.top()-10, scaledPix);

	QRectF rect(m_rect.topLeft(), QSizeF(m_squareSize, m_squareSize));
	const qreal rLeft = rect.left();

	
	QPen pen(Qt::SolidLine);
	//pen.setWidth(10);
	//pen.setColor(Qt::green);
	//painter->setPen(pen);

	//rect.moveLeft(rLeft);

	qreal y = m_rect.top() + m_squareSize / 2;
	qreal x = m_rect.left() + m_squareSize / 2;
	qreal off = m_squareSize/10;
	qreal cWidth = m_squareSize / 15;

	// 初始化描绘四个绿色点座标
	//painter->drawPoint(x, y);
	//painter->drawPoint(x + m_squareSize * 9, y);
	//painter->drawPoint(x, y + m_squareSize * 10);
	//painter->drawPoint(x+m_squareSize * 9, y+m_squareSize * 10);

	// 设置画笔颜色和宽度
	//pen.setColor(Qt::black);
	pen.setColor(m_darkColor);
	pen.setWidth(cWidth);
	painter->setPen(pen);

	// 1.画棋盘的轮廓线
	painter->drawLine(x-off, y-off, m_squareSize * 8 + x+off, y-off);
	painter->drawLine(x-off, y-off, x-off, m_squareSize * 9 + y+off);
	painter->drawLine(m_squareSize * 8 + x+off, y-off, m_squareSize * 8 + x+off, m_squareSize * 9 + y+off);
	painter->drawLine(x-off, m_squareSize * 9 + y+off, m_squareSize * 8 + x+off, m_squareSize * 9 + y+off);

	// 2.画棋盘的16条垂直线
	qreal xWidth = m_squareSize / 30;	// 
	pen.setWidth(xWidth);
	painter->setPen(pen);

	for (int i = 1; i < 8; i++) {
		painter->drawLine(x+m_squareSize * i, y, x+m_squareSize * i, y+m_squareSize * 4);
		painter->drawLine(x+m_squareSize * i, y+m_squareSize * 5,x+m_squareSize * i, y+m_squareSize * 9);
	}

	// 3.楚河 汉界 二条竖短线
	painter->drawLine(x, y, x, y+m_squareSize * 9);
	painter->drawLine(x+m_squareSize * 8, y, x+m_squareSize * 8, y+m_squareSize * 9);


	// 4.画棋盘的10条水平线
	for (int i = 0; i <= 9; i++)
		painter->drawLine(x, y+ m_squareSize * i, x+ m_squareSize * 8, y+ m_squareSize * i);

	// 5.画棋盘“士”行走的斜线
	painter->drawLine(x+m_squareSize * 3, y, x + m_squareSize * 5, y + m_squareSize * 2);
	painter->drawLine(x+m_squareSize * 5, y, x + m_squareSize * 3, y + m_squareSize * 2);
	painter->drawLine(x + m_squareSize * 3, y + m_squareSize * 7, x + m_squareSize * 5, y + m_squareSize * 9);
	painter->drawLine(x + m_squareSize * 5, y + m_squareSize * 7, x + m_squareSize * 3, y + m_squareSize * 9);

	QPointF point1(x+m_squareSize * 1.2, y+m_squareSize * 4.7);

	//QFont f = QFont("Microsoft YaHei");  // KaiTi
	QFont f = QFont("KaiTi");  // KaiTi
	//f.setPointSizeF(m_squareSize / 2);
	f.setPixelSize(m_squareSize / 1.5);
	painter->setPen(Qt::gray);
	painter->setFont(f);
	//painter->setFont(QFont("KaiTi", m_squareSize/2));	

	painter->drawText(point1, "楚 河");
	QPointF point2(x + m_squareSize * 5.2, y + m_squareSize * 4.7);
	painter->drawText(point2, "汉 界");


	// 7. 直角折线
	pen.setWidth(m_squareSize/20);
	painter->setPen(pen);

	qreal r5 = m_squareSize / 10;
	qreal r15 = m_squareSize / 4;

	for (int j = 2; j > 0; j--)
	{
		for (int i = 1; i < 5; i++)  /* 兵的井字格 */
		{
			QPoint points1[3] = {
				QPoint(x+m_squareSize * (2 * i - 2) + r5,  y+m_squareSize * (9 - j * 3) - r15),
				QPoint(x+m_squareSize * (2 * i - 2) + r5,  y+m_squareSize * (9 - j * 3) - r5),
				QPoint(x+m_squareSize * (2 * i - 2) + r15, y+m_squareSize * (9 - j * 3) - r5),
			};
			painter->drawPolyline(points1, 3);

			QPoint points2[3] = {
				QPoint(x+m_squareSize * (2 * i + 0) - r15, y+m_squareSize * (9 - j * 3) - r5),
				QPoint(x+m_squareSize * (2 * i + 0) - r5,  y+m_squareSize * (9 - j * 3) - r5),
				QPoint(x+m_squareSize * (2 * i + 0) - r5,  y+m_squareSize * (9 - j * 3) - r15),
			};
			painter->drawPolyline(points2, 3);

			QPoint points3[3] = {
				QPoint(x+m_squareSize * (2 * i - 2) + r5,  y+m_squareSize * (9 - j * 3) + r15),
				QPoint(x+m_squareSize * (2 * i - 2) + r5,  y+m_squareSize * (9 - j * 3) + r5),
				QPoint(x+m_squareSize * (2 * i - 2) + r15, y+m_squareSize * (9 - j * 3) + r5),
			};
			painter->drawPolyline(points3, 3);

			QPoint points4[3] = {
				QPoint(x+m_squareSize * (2 * i + 0) - r15, y+m_squareSize * (9 - j * 3) + r5),
				QPoint(x+m_squareSize * (2 * i + 0) - r5,  y+m_squareSize * (9 - j * 3) + r5),
				QPoint(x+m_squareSize * (2 * i + 0) - r5,  y+m_squareSize * (9 - j * 3) + r15),
			};											  
			painter->drawPolyline(points4, 3);


			if (i < 3)   /* 炮的井字格 */
			{
				QPoint points5[3] = {
					QPoint(x+m_squareSize * pow(2, 2 * i - 1)- m_squareSize - r15,  y+m_squareSize * (pow(j + 1,2) - 2) - r5),
					QPoint(x+m_squareSize * pow(2, 2 * i - 1) - m_squareSize - r5,   y+m_squareSize * (pow(j + 1,2) - 2) - r5),
					QPoint(x+m_squareSize * pow(2, 2 * i - 1) - m_squareSize - r5,   y+m_squareSize * (pow(j + 1,2) - 2) - r15),
				};
				painter->drawPolyline(points5, 3);

				QPoint points6[3] = {
					QPoint(x+m_squareSize * pow(2, 2 * i - 1) - m_squareSize + r15,  y+m_squareSize * (pow(j + 1,2) - 2) - r5),
					QPoint(x+m_squareSize * pow(2, 2 * i - 1) - m_squareSize + r5,   y+m_squareSize * (pow(j + 1,2) - 2) - r5),
					QPoint(x+m_squareSize * pow(2, 2 * i - 1) - m_squareSize + r5,   y+m_squareSize * (pow(j + 1,2) - 2) - r15),
				};																					  
				painter->drawPolyline(points6, 3);

				QPoint points7[3] = {
					QPoint(x+m_squareSize * pow(2, 2 * i - 1) - m_squareSize - r15,  y+m_squareSize * (pow(j + 1,2) - 2) + r5),
					QPoint(x+m_squareSize * pow(2, 2 * i - 1) - m_squareSize - r5,   y+m_squareSize * (pow(j + 1,2) - 2) + r5),
					QPoint(x+m_squareSize * pow(2, 2 * i - 1) - m_squareSize - r5,   y+m_squareSize * (pow(j + 1,2) - 2) + r15),
				};
				painter->drawPolyline(points7, 3);

				QPoint points8[3] = {
					QPoint(x+m_squareSize * pow(2, 2 * i - 1) - m_squareSize + r15,  y+m_squareSize * (pow(j + 1,2) - 2) + r5),
					QPoint(x+m_squareSize * pow(2, 2 * i - 1) - m_squareSize + r5,   y+m_squareSize * (pow(j + 1,2) - 2) + r5),
					QPoint(x+m_squareSize * pow(2, 2 * i - 1) - m_squareSize + r5,   y+m_squareSize * (pow(j + 1,2) - 2) + r15),
				};
				painter->drawPolyline(points8, 3);
			}

		}

	}

	/*
	// paint squares
	for (int y = 0; y < m_ranks; y++)
	{
		rect.moveLeft(rLeft);
		for (int x = 0; x < m_files; x++)
		{
			if ((x % 2) == (y % 2))
				painter->fillRect(rect, m_lightColor);    // 亮色
			else
				painter->fillRect(rect, m_darkColor);     // 暗色
			rect.moveLeft(rect.left() + m_squareSize);
		}
		rect.moveTop(rect.top() + m_squareSize);
	}
	*/

	//auto font = painter->font();
	//font.setPointSizeF(font.pointSizeF() * 0.7);
	//painter->setFont(font);
	//painter->setPen(m_textColor);

	//// paint file coordinates
	//const QString alphabet = "abcdefghijklmnopqrstuvwxyz";
	//for (int i = 0; i < m_files; i++)
	//{
	//	const qreal tops[] = {m_rect.top() - m_coordSize,
	//	                      m_rect.bottom()};
	//	for (const auto top : tops)
	//	{
	//		rect = QRectF(m_rect.left() + (m_squareSize * i), top,
	//		              m_squareSize, m_coordSize);
	//		int file = m_flipped ? m_files - i - 1 : i;
	//		painter->drawText(rect, Qt::AlignCenter, alphabet[file]);
	//	}
	//}

	// paint rank coordinates
	//for (int i = 0; i < m_ranks; i++)
	//{
	//	const qreal lefts[] = {m_rect.left() - m_coordSize,
	//	                       m_rect.right()};
	//	for (const auto left : lefts)
	//	{
	//		rect = QRectF(left, m_rect.top() + (m_squareSize * i),
	//		              m_coordSize, m_squareSize);
	//		int rank = m_flipped ? i + 1 : m_ranks - i;
	//		const auto num = QString::number(rank);
	//		painter->drawText(rect, Qt::AlignCenter, num);
	//	}
	//}
}

Chess::Square GraphicsBoard::squareAt(const QPointF& point) const
{
	if (!m_rect.contains(point))
		return Chess::Square();

	int col = (point.x() + m_rect.width() / 2) / m_squareSize;
	int row = (point.y() + m_rect.height() / 2) / m_squareSize;

	if (m_flipped)
		return Chess::Square(m_files - col - 1, row);
	return Chess::Square(col, m_ranks - row - 1);
}

QPointF GraphicsBoard::squarePos(const Chess::Square& square) const
{
	if (!square.isValid())
		return QPointF();

	qreal x = m_rect.left() + m_squareSize / 2;
	qreal y = m_rect.top() + m_squareSize / 2;

	if (m_flipped)
	{
		x += m_squareSize * (m_files - square.file() - 1);
		y += m_squareSize * square.rank();
	}
	else
	{
		x += m_squareSize * square.file();
		y += m_squareSize * (m_ranks - square.rank() - 1);
	}

	return QPointF(x, y);
}

Chess::Piece GraphicsBoard::pieceTypeAt(const Chess::Square& square) const
{
	GraphicsPiece* piece = pieceAt(square);
	if (piece == nullptr)
		return Chess::Piece();
	return piece->pieceType();
}

GraphicsPiece* GraphicsBoard::pieceAt(const Chess::Square& square) const
{
	if (!square.isValid())
		return nullptr;

	GraphicsPiece* piece = m_squares.at(squareIndex(square));
	Q_ASSERT(piece == nullptr || piece->container() == this);
	return piece;
}

GraphicsPiece* GraphicsBoard::takePieceAt(const Chess::Square& square)
{
	int index = squareIndex(square);
	if (index == -1)
		return nullptr;

	GraphicsPiece* piece = m_squares.at(index);
	if (piece == nullptr)
		return nullptr;

	m_squares[index] = nullptr;
	piece->setParentItem(nullptr);
	piece->setContainer(nullptr);

	return piece;
}

void GraphicsBoard::clearSquares()
{
	qDeleteAll(m_squares);
	m_squares.clear();
}

void GraphicsBoard::setSquare(const Chess::Square& square, GraphicsPiece* piece)
{
	Q_ASSERT(square.isValid());

	int index = squareIndex(square);
	delete m_squares[index];

	if (piece == nullptr)
		m_squares[index] = nullptr;
	else
	{
		m_squares[index] = piece;
		piece->setContainer(this);
		piece->setParentItem(this);
		piece->setPos(squarePos(square));
	}
}

void GraphicsBoard::movePiece(const Chess::Square& source,
			      const Chess::Square& target)
{
	GraphicsPiece* piece = pieceAt(source);
	Q_ASSERT(piece != nullptr);

	m_squares[squareIndex(source)] = nullptr;
	setSquare(target, piece);
}



int GraphicsBoard::squareIndex(const Chess::Square& square) const
{
	if (!square.isValid())
		return -1;

	return square.rank() * m_files + square.file();
}

void GraphicsBoard::clearHighlights()
{
	if (m_highlightAnim != nullptr)
	{
		m_highlightAnim->setDirection(QAbstractAnimation::Backward);
		m_highlightAnim->start(QAbstractAnimation::DeleteWhenStopped);
		m_highlightAnim = nullptr;
	}
}

void GraphicsBoard::setHighlights(const QList<Chess::Square>& squares)
{
	clearHighlights();
	if (squares.isEmpty())
		return;

	TargetHighlights* targets = new TargetHighlights(this);

	QRectF rect;
	rect.setSize(QSizeF(m_squareSize / 3, m_squareSize / 3));
	rect.moveCenter(QPointF(0, 0));
	QPen pen(Qt::white, m_squareSize / 20);
	QBrush brush(Qt::black);

	for (const auto& sq : squares)
	{
		QGraphicsEllipseItem* dot = new QGraphicsEllipseItem(rect, targets);

		dot->setCacheMode(DeviceCoordinateCache);
		dot->setPen(pen);
		dot->setBrush(brush);
		dot->setPos(squarePos(sq));
	}

	m_highlightAnim = new QPropertyAnimation(targets, "opacity");
	targets->setParent(m_highlightAnim);

	m_highlightAnim->setStartValue(0.0);
	m_highlightAnim->setEndValue(1.0);
	m_highlightAnim->setDuration(500);
	m_highlightAnim->setEasingCurve(QEasingCurve::InOutQuad);
	m_highlightAnim->start(QAbstractAnimation::KeepWhenStopped);
}

bool GraphicsBoard::isFlipped() const
{
	return m_flipped;
}

void GraphicsBoard::setFlipped(bool flipped)
{
	if (flipped == m_flipped)
		return;

	clearHighlights();
	m_flipped = flipped;
	update();
}


