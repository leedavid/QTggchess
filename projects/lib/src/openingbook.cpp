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

#include "openingbook.h"
#include <QString>
#include <QFile>
#include <QDataStream>
#include <QtDebug>
#include "pgngame.h"
#include "pgnstream.h"
#include "mersenne.h"

//#include "ConnectionPool.h"
//#include "databasemanager.h"



QDataStream& operator>>(QDataStream& in, OpeningBook* book)
{
	while (in.status() == QDataStream::Ok)
	{
		quint64 key;
		OpeningBook::Entry entry = book->readEntry(in, &key);
		book->addEntry(entry, key);
	}

	return in;
}

QDataStream& operator<<(QDataStream& out, const OpeningBook* book)
{
	OpeningBook::Map::const_iterator it;
	for (it = book->m_map.constBegin(); it != book->m_map.constEnd(); ++it)
		book->writeEntry(it, out);

	return out;
}

OpeningBook::OpeningBook(BookMoveMode mode)
	: m_mode(mode)
{
}

OpeningBook::~OpeningBook()
{
}

QString getRandomString(int length)
{
	qsrand(QDateTime::currentMSecsSinceEpoch());//为随机值设定一个seed

	const char chrs[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	int chrs_size = sizeof(chrs);

	char* ch = new char[length + 1];
	memset(ch, 0, length + 1);
	int randomx = 0;
	for (int i = 0; i < length; ++i)
	{
		randomx = rand() % (chrs_size - 1);
		ch[i] = chrs[randomx];
	}

	QString ret(ch);
	delete[] ch;
	return ret;
}

bool OpeningBook::read(const QString& filename)
{
   
	this->m_filename = filename;

	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", getRandomString(10));
	db.setDatabaseName(this->m_filename);
	
	if (!db.open()) {
		qWarning("打不开开局库文件 %s",
			qUtf8Printable(this->m_filename));
		return false;
	}
	db.close();
	return true;
}

bool OpeningBook::write(const QString& filename) const
{
	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly))
		return false;

	QDataStream out(&file);
	out << this;

	return true;
}



void OpeningBook::addEntry(const Entry& entry, quint64 key)
{
	Map::iterator it = m_map.find(key);
	while (it != m_map.end() && it.key() == key)
	{
		Entry& tmp = it.value();
		if (tmp.move == entry.move)
		{
			//tmp.weight += entry.weight;
			return;
		}
		++it;
	}
	
	m_map.insert(key, entry);
}

int OpeningBook::import(const PgnGame& pgn, int maxMoves)
{
	Q_ASSERT(maxMoves > 0);

	Chess::Side winner(pgn.result().winner());
	int loserMod = -1;
	quint16 weight = 1;
	maxMoves = qMin(maxMoves, pgn.moves().size());
	int ret = maxMoves;

	if (!winner.isNull())
	{
		loserMod = int(pgn.startingSide() == winner);
		weight = 2;
		ret = (ret - loserMod) / 2 + loserMod;
	}

	const QVector<PgnGame::MoveData>& moves = pgn.moves();
	for (int i = 0; i < maxMoves; i++)
	{
		// Skip the loser's moves
		if ((i % 2) != loserMod)
		{
			Entry entry = { moves.at(i).move, weight };
			addEntry(entry, moves.at(i).key);
		}
	}

	return ret;
}

int OpeningBook::import(PgnStream& in, int maxMoves)
{
	Q_ASSERT(maxMoves > 0);

	if (!in.isOpen())
		return 0;

	int moveCount = 0;
	while (in.status() == PgnStream::Ok)
	{
		PgnGame game;
		game.read(in, maxMoves);
		if (game.moves().isEmpty())
			break;

		moveCount += import(game, maxMoves);
	}

	return moveCount;
}

QList<OpeningBook::Entry> OpeningBook::entriesFromDisk(quint64 key) const
{
	QList<Entry> entries;

	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", getRandomString(10));
	db.setDatabaseName(this->m_filename);
	if (!db.open()) {
		qWarning("打不开开局库文件 %s",
			qUtf8Printable(this->m_filename));
		return entries;
	}

	QSqlQuery query(db);

	//key = 0x628d04d7c9c144ae;						// 开局初始hash
	//qint64 ikey = 0xa1ead1f6470e07ee;
	//quint64 ukey = 0xa1ead1f6470e07ee;

	QString sql = "select * from bhobk where vkey = ?";
	query.prepare(sql);
	qint64 ikey = key;
	if (ikey > 0) {		
		query.bindValue(0, key);
	}
	else {
		double dkey;
		memcpy(&dkey, &key, sizeof(qint64));	// SQlite3 不认负的 uint64 key
		query.bindValue(0, dkey);
	}


	if (query.exec()) {

		Entry entry;

		query.first();
		while (query.isValid()) {

			quint32 mbh = query.value("vmove").toInt();

			// 这儿要转换一下
			int from = mbh >> 8;
			int to = mbh & 0xff;

			int fx = from % 16-3;
			int fy = 12 - from / 16;
			Chess::Square sqfrom = Chess::Square(fx, fy);

			int tx = to % 16 - 3;
			int ty = 12 - to / 16;
			Chess::Square sqto = Chess::Square(tx, ty);
			entry.move = Chess::GenericMove(sqfrom, sqto);

			//Chess::Move move = m_board->moveFromGenericMove(bookMove);				// 

			entry.vscore = query.value("vscore").toInt();
			//entry.win_count = query.value("vwin").toInt();
			//entry.draw_count = query.value("vdraw").toInt();
			//entry.lost_connt = query.value("vlost").toInt();
			entry.valid = query.value("vvalid").toInt();
			//entry.comments = query.value("vmemo").toString();
			//entry.vindex = query.value("vindex").toInt();

			//if (entry.vscore == 0) {
			//	entry.vscore = 1;            // 最小给一个1分，防止除0出错
			//}

			if(entry.vscore > 0 && entry.valid == 1){  // 只选择大于0分，且是有效的棋步走棋
				entries << entry;
			}

			if (!query.next()) {   // 移动到下一条，并判断是不是到未尾了
				break;
			}
		}		
	}

	db.close();

	return entries;
}

QList<OpeningBook::Entry> OpeningBook::entries(quint64 key) const
{
	return entriesFromDisk(key);
}


Chess::GenericMove OpeningBook::move(quint64 key) const
{
	Chess::GenericMove move;
	
	// There can be multiple entries/moves with the same key.
	// We need to find them all to choose the best one
	const auto entries = this->entries(key);
	if (entries.isEmpty())
		return move;
	
	// Calculate the total weight of all available moves
	//int totalWeight = 0;
	//for (const Entry& entry : entries)
	//	totalWeight += entry.weight;
	//if (totalWeight <= 0)
	//	return move;

	int totalWeight = 0;
	for (const Entry& entry : entries)
		totalWeight += entry.vscore;
	if (totalWeight < 0)
		return move;

	// Pick a move randomly, with the highest-weighted move having
	// the highest probability of getting picked.	
	if (m_mode == BookRandom) {
		int pick = Mersenne::random() % totalWeight;	
		int currentWeight = 0;
		for (const Entry& entry : entries)
		{
			currentWeight += entry.vscore;
			if (currentWeight > pick)
				return entry.move;
		}
	}
	else {
		int bestScore = 0;	
		// 1. 求得最高分
		for (const Entry& entry : entries) {
			if (entry.vscore >= bestScore) {
				bestScore = entry.vscore;				
			}
		}
		// 2. 将全部的最高分集中起来
		QList<Entry> BestEntries;
		for (const Entry& entry : entries) {
			if (entry.vscore == bestScore) {
				BestEntries << entry;
			}
		}
		// 3. 再在几个最佳步中随机选择
		int totalWeight = 0;
		for (const Entry& entry : BestEntries)
			totalWeight += entry.vscore;
		if (totalWeight < 0)
			return move;

		int pick = Mersenne::random() % totalWeight;
		int currentWeight = 0;
		for (const Entry& entry : BestEntries)
		{
			currentWeight += entry.vscore;
			if (currentWeight > pick)
				return entry.move;
		}
	}	
	
	return move;
}
