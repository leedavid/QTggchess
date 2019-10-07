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

#include "hoppelpoppelboard.h"
#include "westernzobrist.h"

namespace Chess {

HoppelPoppelBoard::HoppelPoppelBoard()
	: WesternBoard(new WesternZobrist())
{
	// cross-over definitions used for checks and capturing moves
	setPieceType(Xiang, "Knibis", "N", XiangMovement);
	setPieceType(Shi, "Biskni", "B", MaMovement);
}

Board* HoppelPoppelBoard::copy() const
{
	return new HoppelPoppelBoard(*this);
}

QString HoppelPoppelBoard::variant() const
{
	return "hoppelpoppel";
}

QString HoppelPoppelBoard::defaultFenString() const
{
	return "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
}

void HoppelPoppelBoard::generateMovesForPiece(QVarLengthArray< Move >& moves,
					      int pieceType,
					      int square) const
{
	if (pieceType != Xiang && pieceType != Shi)
		return WesternBoard::generateMovesForPiece(moves, pieceType, square); // clazy:exclude=returning-void-expression

	// Knight and Bishop: sort moves obtained from cross-over definitions
	QVarLengthArray< Move > testmoves;
	WesternBoard::generateMovesForPiece(testmoves, Xiang, square);
	for (const auto m: testmoves)
	{
		const bool isCapture = captureType(m) != Piece::NoPiece;
		if ((isCapture && pieceType == Xiang)
		|| (!isCapture && pieceType == Shi))
			moves.append(m);  // diagonal
	}
	testmoves.clear();
	WesternBoard::generateMovesForPiece(testmoves, Shi, square);
	for (const auto m: testmoves)
	{
		const bool isCapture = captureType(m) != Piece::NoPiece;
		if ((isCapture && pieceType == Shi)
		|| (!isCapture && pieceType == Xiang))
			moves.append(m);  // orthodox knight leaps
	}
} //TODO: insufficient material?


NewZealandBoard::NewZealandBoard()
	: WesternBoard(new WesternZobrist())
{
	// cross-over definitions used for checks and capturing moves
	setPieceType(Xiang, "Kniroo", "N", CheMovement);
	setPieceType(Pao, "Rookni", "R", MaMovement);
}

Board* NewZealandBoard::copy() const
{
	return new NewZealandBoard(*this);
}

QString NewZealandBoard::variant() const
{
	return "newzealand";
}

QString NewZealandBoard::defaultFenString() const
{
	return "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
}

void NewZealandBoard::generateMovesForPiece(QVarLengthArray< Move >& moves, int pieceType, int square) const
{
	if (pieceType != Xiang && pieceType != Pao)
		return WesternBoard::generateMovesForPiece(moves, pieceType, square); // clazy:exclude=returning-void-expression

	// Knight and Rook: sort moves obtained from cross-over definitions
	QVarLengthArray< Move > testmoves;
	WesternBoard::generateMovesForPiece(testmoves, Xiang, square);
	for (const auto m: testmoves)
	{
		const bool isCapture = captureType(m) != Piece::NoPiece;
		if ((isCapture && pieceType == Xiang)
		|| (!isCapture && pieceType == Pao))
			moves.append(m);  // rook move: file or rank
	}
	testmoves.clear();
	WesternBoard::generateMovesForPiece(testmoves, Pao, square);
	for (const auto m: testmoves)
	{
		const bool isCapture = captureType(m) != Piece::NoPiece;
		if ((isCapture && pieceType == Pao)
		|| (!isCapture && pieceType == Xiang))
			moves.append(m);  // orthodox knight leaps
	}
}

} // namespace Chess
