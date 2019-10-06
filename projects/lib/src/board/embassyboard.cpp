/*
    This file is part of Cute Chess.
    Copyright (C) 2008-2018 Cute Chess authors

    Cute Chess is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Cute Chess is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Cute Chess.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "embassyboard.h"


namespace Chess {

EmbassyBoard::EmbassyBoard()
	: CapablancaBoard()
{
}

Board* EmbassyBoard::copy() const
{
	return new EmbassyBoard(*this);
}

QString EmbassyBoard::variant() const
{
	return "embassy";
}

QString EmbassyBoard::defaultFenString() const
{
	return "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1";
}

//int EmbassyBoard::castlingFile(WesternBoard::CastlingSide castlingSide) const
//{
//	Q_ASSERT(castlingSide != NoCastlingSide);
//	return castlingSide == QueenSide ? 1 : 7; // b-file and h-file
//}

} // namespace Chess
