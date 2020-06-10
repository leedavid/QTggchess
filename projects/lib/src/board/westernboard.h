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

#ifndef WESTERNBOARD_H
#define WESTERNBOARD_H

#include "board.h"

namespace Chess {

class WesternZobrist;


/*!
 * \brief A board for western chess variants
 *
 * WesternBoard serves as the overclass for all western variants.
 * In addition to possibly unique pieces, a western variant has
 * the same pieces as standard chess, the same rules for castling,
 * en-passant capture, promotion, etc.
 *
 * WesternBoard implements the rules of standard chess, including
 * check, checkmate, stalemate, promotion, 3-fold repetition,
 * 50 move rule and draws by insufficient material.
 */
class LIB_EXPORT WesternBoard : public Board
{
	public:
		/*! Basic piece types for western variants. */
		enum WesternPieceType
		{
			Pawn = 1,	//!< Pawn
			Xiang,		//!< Knight
			Shi,		//!< Bishop
			Pao,		//!< Rook
			Ma,		    //!< Ma
			Che,		//!< Queen
			King		//!< King
		};

		/*! Creates a new WesternBoard object. */
		WesternBoard(WesternZobrist* zobrist);

		// Inherited from Board
		virtual int width() const;
		virtual int height() const;
		virtual Result result();
		virtual int reversibleMoveCount() const;

		//bool getIsLink(return m_)
		

	protected:
		/*! The king's castling side. */
		//enum CastlingSide
		//{
		//	QueenSide,	//!< Queen side (O-O-O)
		//	KingSide,	//!< King side (O-O)
		//	NoCastlingSide	//!< No castling side
		//};
		/*! Movement mask for Pawn moves. */
		//static const unsigned PawnMovement = 1;
		///*! Movement mask for Knight moves. */
		//static const unsigned MaMovement = 2;
		///*! Movement mask for Xiang moves. */
		//static const unsigned XiangMovement = 4;
		///*! Movement mask for Rook moves. */
		//static const unsigned CheMovement = 8;
		///*! Movement mask for Shi moves. */
		//static const unsigned ShiMovement = 16;
		///*! Movement mask for Pao moves. */
		//static const unsigned PaoMovement = 32;
		///*! Movement mask for King moves. */
		//static const unsigned KingMovement = 64;

		/*! Types of Pawn moves. */
		enum StepType
		{
			 NoStep = 0,      //!< Cannot move here
			 FreeStep =  1,   //!< May move if target is empty
			 CaptureStep = 2  //!< Capture opposing piece only
			 /* FreeOrCaptureStep = FreeStep|CaptureStep //!< like King or Sergeant*/
		};
		/*! Stores a move \a type and a move direction \a file for a Pawn move */
		//struct PawnStep { StepType type; int file; };
		/*!
		 * Movement mask for Pawn moves.
		 * Lists pawn move types and relative files
		 * \sa BerolinaBoard
		 *
		 * Default: A Pawn can step straight ahead onto a free square or
		 * capture diagonally forward. So initialise this
		 * as { {CaptureStep, -1}, {FreeStep, 0}, {CaptureStep, 1} }
		 */
		//QVarLengthArray<PawnStep, 8> m_pawnSteps;
		/*!
		 * Helper function for Pawn moves. Returns the count of moves
		 * of the given \a type that are specified in pawnSteps.
		 */
		
		/*!
		 * Returns true if both counts of kings given by
		 * \a whiteKings and \a blackKings are correct.
		 * WesternBoard expects exactly one king per side.
		 * \sa AntiBoard
		 * \sa HordeBoard
		 */
		virtual bool kingsCountAssertion(int whiteKings,
						 int blackKings) const;
		/*!
		 * Returns true if the king can capture opposing pieces.
		 * The default value is true.
		 * \sa AtomicBoard
		 */
		//virtual bool kingCanCapture() const;
		/*!
		* Returns true if castling is allowed.
		* The default value is true.
		* \sa ShatranjBoard
		*/
		//virtual bool hasCastling() const;
		/*!
		 * Returns true if pawns have an initial double step option.
		 * The default value is true.
		 * \sa ShatranjBoard
		 */
		//virtual bool pawnHasDoubleStep() const;
		/*!
		 * Returns true if a pawn can be captured en passant after
		 * an initial double step.
		 * The default value is the value of pawnHasDoubleStep().
		 */
		//virtual bool hasEnPassantCaptures() const;
		/*!
		 * Returns true if a rule provides \a side to insert a reserve
		 * piece at a vacated source \a square immediately after a move.
		 * The default value is false.
		 *
		 * \sa SeirawanBoard
		 */
		//virtual bool variantHasChanneling(Side side, int square) const;
		/*!
		 * Adds pawn promotions to a move list.
		 *
		 * This function is called when a pawn can promote by
		 * moving from \a sourceSquare to \a targetSquare.
		 * This function generates all the possible promotions
		 * and adds them to \a moves.
		 */

		/*! Returns the king square of \a side. */
		int kingSquare(Side side) const;
		/*! Returns the current en-passant square. */
		//int enpassantSquare() const;
		/*!
		 * Parse castling rights given by character \a c of the FEN
		 * token. Returns true if successful.
		 */
		//virtual bool parseCastlingRights(QChar c);
		/*!
		 * Returns true if \a side has a right to castle on \a castlingSide;
		 * otherwise returns false.
		 *
		 * \note Even if this function returns true, castling may not be
		 * a legal move in the current position.
		 */
		//bool hasCastlingRight(Side side, CastlingSide castlingSide) const;
		/*!
		 * Removes castling rights at \a square.
		 *
		 * If one of the players has a rook at \a square, the rook can't
		 * be used for castling. This function should be called when a
		 * capture happens at \a square.
		 */
		//void removeCastlingRights(int square);
		/*!
		 * Removes all castling rights of \a side.
		 */
		//void removeCastlingRights(Side side);
		/*!
		 * Defines the file a king may castle to on \a castlingSide.
		 * Defaults: 2 (c-file) and width() - 2 (normally g-file)
		 */
		//virtual int castlingFile(CastlingSide castlingSide) const;
		/*!
		 * Returns true if \a side is under attack at \a square.
		 * If \a square is 0, then the king square is used.
		 */
		virtual bool inCheck(Side side /*, int square = 0*/) const;

		/*!
		 * Returns FEN extensions. The default is an empty string.
		 *
		 * This function is called by fenString() via vFenString().
		 * Returns additional parts of the current position's (extended)
		 * FEN string which succeed the en passant field.
		 */
		virtual QString vFenIncludeString(FenNotation notation) const;

		// Inherited from Board
		virtual void vInitialize();
		virtual QString vFenString(FenNotation notation) const;
		virtual bool vSetFenString(const QStringList& fen);
		virtual QString lanMoveString(const Move& move);
		virtual QString ChineseMoveString(const Move& move);
		virtual Move moveFromLanString(const QString& str);
		//virtual Move moveFromSanString(const QString& str);
		virtual void vMakeMove(const Move& move,
				       BoardTransition* transition);
		virtual void vUndoMove(const Move& move);
		virtual void generateMovesForPiece(QVarLengthArray<Move>& moves,
						   int pieceType,
						   int square) const;
		virtual bool vIsLegalMove(const Move& move);
		virtual bool isLegalPosition();
		virtual int captureType(const Move& move) const;

		virtual Move moveFromStringCN(const QString& str);

	private:
		//bool m_bIsAutoLink;   // 是不是自动连线

		// Data for reversing/unmaking a move
		struct MoveData
		{
			Piece capture;
			//bool isIncheck;       // 是否被将军
			//bool isRecap;         // 是否常捉
			int reversibleMoveCount;
		};

		//bool m_bIsLinkChess;     // 是否连线象棋
		
		int m_arwidth;
		int m_sign;
		int m_kingSquare[2];
		int m_plyOffset;
		int m_reversibleMoveCount;
		//bool m_kingCanCapture;
	
		//bool m_multiDigitNotation;
		QVector<MoveData> m_history;

		const WesternZobrist* m_zobrist;

		QVarLengthArray<int> m_BPawnOffsets;	    // 黑卒
		QVarLengthArray<int> m_RPawnOffsets;	    // 红兵
		QVarLengthArray<int> m_MaOffsets;
		QVarLengthArray<int> m_MaLegOffsets;             // 马腿
		QVarLengthArray<int> m_MaCheckLegOffsets;        // 别人的马将军的马腿

		QVarLengthArray<int> m_XiangOffsets;        // 相
		QVarLengthArray<int> m_XiangEyeOffsets;     // 象眼
	
		QVarLengthArray<int> m_CheOffsets;			// 车
		QVarLengthArray<int> m_ShiOffsets;			// 仕

		QVarLengthArray<QString> strnumCn;
		QVarLengthArray<QString> strnumEn;
		QVarLengthArray<QString> strnumName;

};


} // namespace Chess
#endif // WESTERNBOARD_H
