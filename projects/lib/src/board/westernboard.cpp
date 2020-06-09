

#include "westernboard.h"
#include <QStringList>
#include "westernzobrist.h"
#include "boardtransition.h"


#pragma execution_character_set("utf-8")

namespace Chess {

WesternBoard::WesternBoard(WesternZobrist* zobrist)
	: Board(zobrist),
	  m_arwidth(0),
	  m_sign(1),
	  m_plyOffset(0),
	  m_reversibleMoveCount(0),
	  //m_kingCanCapture(true),
	  //m_multiDigitNotation(false),
	  m_zobrist(zobrist)	
{
	setPieceType(Pawn, tr("pawn"), "P"); // , PawnMovement);                    // ��
	setPieceType(Ma, tr("knight"), "N"); // , MaMovement);                      // ��
	setPieceType(Xiang, tr("bishop"), "B"); // , XiangMovement);                // ��
	setPieceType(Shi, tr("shi"), "A"); // , XiangMovement);                     // ��
	setPieceType(Che, tr("rook"), "R"); // , CheMovement);                      // ��
	setPieceType(Pao, tr("pao"), "C"); // , PaoMovement);                       // ��
	setPieceType(King, tr("king"), "K"); // , KingMovement);                    // ��
}

int WesternBoard::width() const
{
	return 9;
}

int WesternBoard::height() const
{
	return 10;
}

bool WesternBoard::kingsCountAssertion(int whiteKings, int blackKings) const
{
	return whiteKings == 1 && blackKings == 1;
}

//bool WesternBoard::kingCanCapture() const
//{
//	return true;
//}


void WesternBoard::vInitialize()
{
	//m_kingCanCapture = kingCanCapture();
	m_arwidth = width() + 2;

	m_kingSquare[Side::White] = 0;
	m_kingSquare[Side::Black] = 0;

	

	m_BPawnOffsets.resize(3);
	m_BPawnOffsets[0] = m_arwidth;
	m_BPawnOffsets[1] = -1;
	m_BPawnOffsets[2] = 1;

	m_RPawnOffsets.resize(3);
	m_RPawnOffsets[0] = -m_arwidth;
	m_RPawnOffsets[1] = -1;
	m_RPawnOffsets[2] = 1;

	m_MaLegOffsets.resize(8);   // ����
	m_MaLegOffsets[0] = -m_arwidth;
	m_MaLegOffsets[1] = -m_arwidth;
	m_MaLegOffsets[2] = -1;
	m_MaLegOffsets[3] = +1;
	m_MaLegOffsets[4] = -1;
	m_MaLegOffsets[5] = +1;
	m_MaLegOffsets[6] = +m_arwidth;
	m_MaLegOffsets[7] = +m_arwidth;

	m_MaOffsets.resize(8);
	m_MaOffsets[0] = -2 * m_arwidth - 1;
	m_MaOffsets[1] = -2 * m_arwidth + 1;
	m_MaOffsets[2] = -m_arwidth - 2;
	m_MaOffsets[3] = -m_arwidth + 2;
	m_MaOffsets[4] = m_arwidth - 2;
	m_MaOffsets[5] = m_arwidth + 2;
	m_MaOffsets[6] = 2 * m_arwidth - 1;
	m_MaOffsets[7] = 2 * m_arwidth + 1;

	m_MaCheckLegOffsets.resize(8);   // ���˵�����������
	m_MaCheckLegOffsets[0] = -m_arwidth-1;
	m_MaCheckLegOffsets[1] = -m_arwidth+1;
	m_MaCheckLegOffsets[2] = -m_arwidth -1;
	m_MaCheckLegOffsets[3] = -m_arwidth +1;
	m_MaCheckLegOffsets[4] = m_arwidth -1;
	m_MaCheckLegOffsets[5] = m_arwidth +1;
	m_MaCheckLegOffsets[6] = +m_arwidth-1;
	m_MaCheckLegOffsets[7] = +m_arwidth+1;


	//QVarLengthArray<int> m_MaCheckLegOffsets;        // ���˵�����������

	
	m_XiangOffsets.resize(4);
	m_XiangOffsets[0] = -2 * m_arwidth - 2;
	m_XiangOffsets[1] = -2 * m_arwidth + 2;
	m_XiangOffsets[2] = 2 * m_arwidth - 2;
	m_XiangOffsets[3] = 2 * m_arwidth + 2;

	m_XiangEyeOffsets.resize(4);
	m_XiangEyeOffsets[0] = -m_arwidth - 1;
	m_XiangEyeOffsets[1] = -m_arwidth + 1;
	m_XiangEyeOffsets[2] = m_arwidth - 1;
	m_XiangEyeOffsets[3] = m_arwidth + 1;

	m_CheOffsets.resize(4);
	m_CheOffsets[0] = -m_arwidth;
	m_CheOffsets[1] = -1;
	m_CheOffsets[2] = 1;
	m_CheOffsets[3] = m_arwidth;

	m_ShiOffsets.resize(4);
	m_ShiOffsets[0] = -m_arwidth - 1;
	m_ShiOffsets[1] = -m_arwidth + 1;
	m_ShiOffsets[2] = m_arwidth - 1;
	m_ShiOffsets[3] = m_arwidth + 1;

	strnumCn.resize(10);
	strnumCn[0] = "��";
	strnumCn[1] = "һ";
	strnumCn[2] = "��";
	strnumCn[3] = "��";
	strnumCn[4] = "��";
	strnumCn[5] = "��";
	strnumCn[6] = "��";
	strnumCn[7] = "��";
	strnumCn[8] = "��";
	strnumCn[9] = "��";

	strnumEn.resize(10);
	strnumEn[0] = "��";
	strnumEn[1] = "��";
	strnumEn[2] = "��";
	strnumEn[3] = "��";
	strnumEn[4] = "��";
	strnumEn[5] = "��";
	strnumEn[6] = "��";
	strnumEn[7] = "��";
	strnumEn[8] = "��";
	strnumEn[9] = "��";

	strnumName.resize(16);
	strnumName[0] = "��";
	strnumName[1] = "��";
	strnumName[2] = "��";
	strnumName[3] = "��";
	strnumName[4] = "��";
	strnumName[5] = "��";
	strnumName[6] = "��";
	strnumName[7] = "˧";
	strnumName[8]  = "��";
	strnumName[9]  = "��";
	strnumName[10] = "ʿ";
	strnumName[11] = "��";
	strnumName[12] = "��";
	strnumName[13] = "��";
	strnumName[14] = "��";
	strnumName[15] = "��";

}



int WesternBoard::captureType(const Move& move) const
{
	//if (pieceAt(move.sourceSquare()).type() == Pawn
	//&&  move.targetSquare() == m_enpa ssant Square)
	//	return Pawn;

	return Board::captureType(move);
}



Move WesternBoard::moveFromStringCN(const QString& str)
{

	QVarLengthArray<Move> moves;
	generateMoves(moves);

	//QString s4 = str.split("x")[0];   // ȥ��x

	for (int i = 0; i < moves.size(); i++)
	{
		const auto m = moves[i];
		// �õ�����߲������ƣ�
		QString cn = ChineseMoveString(m);

		if (str == cn) {
			if (vIsLegalMove(m)) {
				return m;
			}
		}
	}
	
	
	return Move();
}



QString WesternBoard::lanMoveString(const Move& move)
{
	return Board::lanMoveString(move);
}



QString WesternBoard::ChineseMoveString(const Move& move)
{
	QString str;
	int source = move.sourceSquare();
	int target = move.targetSquare();
	Piece piece = pieceAt(source);
	Piece capture = pieceAt(target);
	Square FromSquare = chessSquare(source);
	Square ToSqure = chessSquare(target);

	Side side = sideToMove();

	int fx = FromSquare.file();
	int tx = ToSqure.file();
	int fy = FromSquare.rank();
	int ty = ToSqure.rank();

	int chessType = piece.type();
	bool isQH = false;             // ���û��ǰ����
	QString stQH = "";

	if (chessType == Xiang || chessType == Shi || chessType == King) {
	}
	else {
		// ���Ͽ�û������
		int targetSquare = source;
		while(true){
			targetSquare -= 11;
			if (!isValidSquare(chessSquare(targetSquare)))
				break;
			Piece mpiece = pieceAt(targetSquare);
			if (mpiece.side() == side) {
				if (mpiece.type() == chessType) {
					if (side == Side::White) {
						stQH = "��";
					}
					else {
						stQH = "ǰ";
					}
					isQH = true;
					goto QH_BRANCH;
				}
			}
		}
		// ���¿�û������
		targetSquare = source;
		while (true) {
			targetSquare += 11;
			if (!isValidSquare(chessSquare(targetSquare)))
				break;
			Piece mpiece = pieceAt(targetSquare);	
			if (mpiece.side() == side) {
				if (mpiece.type() == chessType) {
					if (side == Side::White) {
						stQH = "ǰ";
					}
					else {
						stQH = "��";
					}
					isQH = true;
					goto QH_BRANCH;
				}
			}
		}
	}


QH_BRANCH:
	if (isQH == true) {
		str = stQH;
		if (side == Side::White) {
			str += strnumName[chessType]; // ����������	
			if (ty == fy) {
				str += "ƽ";
				str += strnumCn[10 - (tx + 1)];
			}
			else {
				if (target < source) {
					str += "��";
				}
				else {
					str += "��";
				}
				if (chessType == Xiang || chessType == Shi || chessType == Ma) {
					str += strnumCn[10 - (tx + 1)];
				}
				else {
					str += strnumCn[abs(fy - ty)];
				}
			}
		}
		else {
			str += strnumName[chessType + 7];   // ����������
			if (ty == fy) {
				str += "ƽ";
				str += strnumEn[(tx + 1)];
			}
			else {
				if (target < source) {
					str += "��";
				}
				else {
					str += "��";
				}
				if (chessType == Xiang || chessType == Shi || chessType == Ma) {
					str += strnumEn[(tx + 1)];
				}
				else {
					str += strnumEn[abs(fy - ty)];
				}
			}
		}
	}
	else {  // �岽����ǰ��
		if (side == Side::White) {
			if (ty == fy) {
				str = strnumName[chessType]; // ����������			
				str += strnumCn[10 - (fx + 1)];  //�õ��߲���FROM����
				str += "ƽ";
				str += strnumCn[10 - (tx + 1)];
			}
			else {  //y���겻��ͬ 
				str = strnumName[chessType]; // ����������	
				str += strnumCn[10 - (fx + 1)];  //�õ��߲���FROM����
				if (target < source) {
					str += "��";
				}
				else {
					str += "��";
				}
				if (chessType == Xiang || chessType == Shi || chessType == Ma) {
					str += strnumCn[10 - (tx + 1)];
				}
				else {
					str += strnumCn[abs(fy-ty)];
				}
			}
		}
		else {
			if (ty == fy) {
				str = strnumName[chessType + 7];   // ����������
				str += strnumEn[(fx + 1)];    // �õ��߲���FROM����
				str += "ƽ";
				str += strnumEn[(tx + 1)];
			}
			else {  //y���겻��ͬ 
				str = strnumName[chessType + 7];  // ����������	
				str += strnumEn[(fx + 1)];   // �õ��߲���FROM����
				if (target < source) {
					str += "��";
				}
				else {
					str += "��";
				}
				if (chessType == Xiang || chessType == Shi || chessType == Ma) {
					str += strnumEn[(tx + 1)];
				}
				else {
					str += strnumEn[abs(fy - ty)];
				}
			}
		}
	}

	
	//if (capture.isValid())   // ���Ӳ�
	//	str += 'x';

	//if (move.promotion() != Piece::NoPiece)
	//	str += "=" + pieceSymbol(move.promotion()).toUpper();

	//if (checkOrMate != 0)
	//	str += checkOrMate;

	return str;
}

Move WesternBoard::moveFromLanString(const QString& str)
{
	Move move(Board::moveFromEnglishString(str));

	//Side side = sideToMove();
	//int source = move.sourceSquare();
	//int target = move.targetSquare();

	//if (source == m_kingSquare[side]
	//&&  qAbs(source - target) != 1)
	//{
	//	const int* rookSq = m_castlingRights.rookSquare[side];
	//	if (target == m_castleTarget[side][QueenSide])
	//		target = rookSq[QueenSide];
	//	else if (target == m_castleTarget[side][KingSide])
	//		target = rookSq[KingSide];

	//	if (target != 0)
	//		return Move(source, target);
	//}

	return move;
}

//Move WesternBoard::moveFromSanString(const QString& str)
//{
//	if (str.length() < 2)
//		return Move();
//
//	QString mstr = str;
//	Side side = sideToMove();
//
//	// Ignore check/mate/strong move/blunder notation
//	while (mstr.endsWith('+') || mstr.endsWith('#')
//	||     mstr.endsWith('!') || mstr.endsWith('?'))
//	{
//		mstr.chop(1);
//	}
//
//	if (mstr.length() < 2)
//		return Move();
//
//	// Castling
//	//if (mstr.startsWith("O-O"))
//	//{
//	//	CastlingSide cside;
//	//	if (mstr == "O-O")
//	//		cside = KingSide;
//	//	else if (mstr == "O-O-O")
//	//		cside = QueenSide;
//	//	else
//	//		return Move();
//
//	//	int source = m_kingSquare[side];
//	//	int target = m_castlingRights.rookSquare[side][cside];
//
//	//	Move move(source, target);
//	//	if (isLegalMove(move))
//	//		return move;
//	//	else
//	//		return Move();
//	//}
//
//	// number of digits in notation of squares
//	int digits = 1;
//
//	// only for tall boards: find maximum number of sequential digits
//	if (m_multiDigitNotation)
//	{
//		int count = 0;
//		for (const QChar& ch : qAsConst(mstr))
//		{
//			if (ch.isDigit())
//			{
//				count++;
//				if (count > digits)
//					digits = count;
//			}
//			else
//				count = 0;
//		}
//	}
//
//	Square sourceSq;
//	Square targetSq;
//	QString::const_iterator it = mstr.cbegin();
//
//	// A SAN move can't start with the capture mark, and
//	if (*it == 'x')
//		return Move();
//	// a pawn move should not specify the piece type
//	if (pieceFromSymbol(*it) == Pawn)
//		it++; // ignore character
//	// Piece type
//	Piece piece = pieceFromSymbol(*it);
//	if (piece.side() != Side::White)
//		piece = Piece::NoPiece;
//	else
//		piece.setSide(side);
//	if (piece.isEmpty())
//	{
//		piece = Piece(side, Pawn);
//		targetSq = chessSquare(mstr.mid(0, 1 + digits));
//		if (isValidSquare(targetSq))
//			it += 1 + digits;
//	}
//	else
//	{
//		++it;
//
//		// Drop moves
//		//if (*it == '@')
//		//{
//		//	targetSq = chessSquare(mstr.right(1 + digits));
//		//	if (!isValidSquare(targetSq))
//		//		return Move();
//
//		//	Move move(0, squareIndex(targetSq), piece.type());
//		//	if (isLegalMove(move))
//		//		return move;
//		//	return Move();
//		//}
//	}
//
//	bool stringIsCapture = false;
//
//	if (!isValidSquare(targetSq))
//	{
//		// Source square's file
//		sourceSq.setFile(it->toLatin1() - 'a');
//		if (sourceSq.file() < 0 || sourceSq.file() >= width())
//			sourceSq.setFile(-1);
//		else if (++it == mstr.cend())
//			return Move();
//
//		// Source square's rank
//		if (it->isDigit())
//		{
//			const QString tmp(mstr.mid(it - mstr.constBegin(),
//						   digits));
//			sourceSq.setRank(-1 + tmp.toInt());
//			if (sourceSq.rank() < 0 || sourceSq.rank() >= height())
//				return Move();
//			it += digits;
//		}
//		if (it == mstr.cend())
//		{
//			// What we thought was the source square, was
//			// actually the target square.
//			if (isValidSquare(sourceSq))
//			{
//				targetSq = sourceSq;
//				sourceSq.setRank(-1);
//				sourceSq.setFile(-1);
//			}
//			else
//				return Move();
//		}
//		// Capture
//		else if (*it == 'x')
//		{
//			if(++it == mstr.cend())
//				return Move();
//			stringIsCapture = true;
//		}
//
//		// Target square
//		if (!isValidSquare(targetSq))
//		{
//			if (it + 1 >= mstr.cend())
//				return Move();
//			QString tmp(mstr.mid(it - mstr.cbegin(), 1 + digits));
//			targetSq = chessSquare(tmp);
//			it += tmp.size();
//		}
//	}
//	if (!isValidSquare(targetSq))
//		return Move();
//	int target = squareIndex(targetSq);
//
//	// Make sure that the move string is right about whether
//	// or not the move is a capture.
//	bool isCapture = false;
//	if (pieceAt(target).side() == side.opposite())
//		isCapture = true;
//	if (isCapture != stringIsCapture)
//		return Move();
//
//	// Promotion
//	int promotion = Piece::NoPiece;
//	if (it != mstr.cend())
//	{
//		if ((*it == '=' || *it == '(') && ++it == mstr.cend())
//			return Move();
//
//		promotion = pieceFromSymbol(*it).type();
//		if (promotion == Piece::NoPiece)
//			return Move();
//	}
//
//	QVarLengthArray<Move> moves;
//	generateMoves(moves, piece.type());
//	const Move* match = nullptr;
//
//	// Loop through all legal moves to find a move that matches
//	// the data we got from the move string.
//	for (int i = 0; i < moves.size(); i++)
//	{
//		const Move& move = moves[i];
//		if (move.sourceSquare() == 0 || move.targetSquare() != target)
//			continue;
//		Square sourceSq2 = chessSquare(move.sourceSquare());
//		if (sourceSq.rank() != -1 && sourceSq2.rank() != sourceSq.rank())
//			continue;
//		if (sourceSq.file() != -1 && sourceSq2.file() != sourceSq.file())
//			continue;
//		// Castling moves were handled earlier
//		if (pieceAt(target) == Piece(side, Pao))
//			continue;
//		//if (move.promotion() != promotion)
//		//	continue;
//
//		if (!vIsLegalMove(move))
//			continue;
//
//		// Return an empty move if there are multiple moves that
//		// match the move string.
//		if (match != nullptr)
//			return Move();
//		match = &move;
//	}
//
//	if (match != nullptr)
//		return *match;
//
//	return Move();
//}

//QString WesternBoard::castlingRightsString(FenNotation notation) const
//{
//	QString str;
//
//	for (int side = Side::White; side <= Side::Black; side++)
//	{
//		for (int cside = KingSide; cside >= QueenSide; cside--)
//		{
//			int rs = m_castlingRights.rookSquare[side][cside];
//			if (rs == 0)
//				continue;
//
//			int offset = (cside == QueenSide) ? -1: 1;
//			Piece piece;
//			int i = rs + offset;
//			bool ambiguous = false;
//
//			// If the castling rook is not the outernmost rook,
//			// the castling square is ambiguous
//			while (!(piece = pieceAt(i)).isWall())
//			{
//				if (piece == Piece(Side::Type(side), Pao))
//				{
//					ambiguous = true;
//					break;
//				}
//				i += offset;
//			}
//
//			QChar c;
//			// If the castling square is ambiguous, then we can't
//			// use 'K' or 'Q'. Instead we'll use the square's file.
//			if (ambiguous || notation == ShredderFen)
//				c = QChar('a' + chessSquare(rs).file());
//			else
//			{
//				if (cside == 0)
//					c = 'q';
//				else
//					c = 'k';
//			}
//			if (side == upperCaseSide())
//				c = c.toUpper();
//			str += c;
//		}
//	}
//
//	if (str.length() == 0)
//		str = "-";
//	return str;
//}



QString WesternBoard::vFenIncludeString(FenNotation notation) const
{
	Q_UNUSED(notation);
	return "";
}

QString WesternBoard::vFenString(FenNotation notation) const
{
	// Castling rights
	//QString fen = castlingRightsString(notation) + ' ';

	// En-passant square
	//if (m_enpas santSquare != 0)
	//{
	//	fen += squareString(m_en passantSquare);
	//	if (m_pawnAm biguous)
	//		fen += squareString(m_enp assantTarget);
	//}
	//else
	//	fen += '-';

	QString fen  =vFenIncludeString(notation);

	// Reversible halfmove count
	fen += "- - ";
	fen += QString::number(m_reversibleMoveCount);

	// Full move number
	fen += ' ';
	fen += QString::number((m_history.size() + m_plyOffset) / 2 + 1);

	return fen;
}

//bool WesternBoard::parseCastlingRights(QChar c)
//{
//	if (!m_hasCastling)
//		return false;
//
//	int offset = 0;
//	CastlingSide cside = NoCastlingSide;
//	Side side = (c.isUpper()) ? upperCaseSide() : upperCaseSide().opposite();
//	c = c.toLower();
//
//	if (c == 'q')
//	{
//		cside = QueenSide;
//		offset = -1;
//	}
//	else if (c == 'k')
//	{
//		cside = KingSide;
//		offset = 1;
//	}
//
//	int kingSq = m_kingSquare[side];
//
//	if (offset != 0)
//	{
//		Piece piece;
//		int i = kingSq + offset;
//		int rookSq = 0;
//
//		// Locate the outernmost rook on the castling side
//		while (!(piece = pieceAt(i)).isWall())
//		{
//			if (piece == Piece(side, Pao))
//				rookSq = i;
//			i += offset;
//		}
//		if (rookSq != 0)
//		{
//			setCastlingSquare(side, cside, rookSq);
//			return true;
//		}
//	}
//	else	// Shredder FEN or X-FEN
//	{
//		int file = c.toLatin1() - 'a';
//		if (file < 0 || file >= width())
//			return false;
//
//		// Get the rook's source square
//		int rookSq;
//		if (side == Side::White)
//			rookSq = (height() + 1) * m_arwidth + 1 + file;
//		else
//			rookSq = 2 * m_arwidth + 1 + file;
//
//		// Make sure the king and the rook are on the same rank
//		if (abs(kingSq - rookSq) >= width())
//			return false;
//
//		// Update castling rights in the FenData object
//		if (pieceAt(rookSq) == Piece(side, Pao))
//		{
//			if (rookSq > kingSq)
//				cside = KingSide;
//			else
//				cside = QueenSide;
//			setCastlingSquare(side, cside, rookSq);
//			return true;
//		}
//	}
//
//	return false;
//}

bool WesternBoard::vSetFenString(const QStringList& fen)
{
	if (fen.size() < 2)
		return false;
	QStringList::const_iterator token = fen.begin();

	// Find the king squares
	int kingCount[2] = {0, 0};
	for (int sq = 0; sq < arraySize(); sq++)
	{
		Piece tmp = pieceAt(sq);
		if (tmp.type() == King)
		{
			m_kingSquare[tmp.side()] = sq;
			kingCount[tmp.side()]++;
		}
	}
	if (!kingsCountAssertion(kingCount[Side::White],
				 kingCount[Side::Black]))
		return false;

	// short non-standard format without castling and ep fields?
	bool isShortFormat = false;
	if (fen.size() < 3)
		token->toInt(&isShortFormat);

	// allowed only for variants without castling and en passant captures
	//if (isShortFormat && (m_hasCastling || m_hasEnPassantCaptures))
	//	return false;

	// Castling rights
	//m_castlingRights.rookSquare[Side::White][QueenSide] = 0;
	//m_castlingRights.rookSquare[Side::White][KingSide] = 0;
	//m_castlingRights.rookSquare[Side::Black][QueenSide] = 0;
	//m_castlingRights.rookSquare[Side::Black][KingSide] = 0;

	if (!isShortFormat)
	{
		if (*token != "-")
		{
			QString::const_iterator c;
			for (c = token->begin(); c != token->end(); ++c)
			{
				//if (!parseCastlingRights(*c))
				//	return false;
			}
		}
		++token;
	}
	// En-passant square
	
	Side side(sideToMove());
	m_sign = (side == Side::White) ? 1 : -1;

	//if (m_hasEnPassantCaptures && *token != "-")
	//{
	//	int epSq = squareIndex(*token);
	//	int fenEpTgt = 0;
	//	// ambiguous ep variants: read ep square [and target]
	//	if (m_pawnAmb iguous)
	//	{
	//		for (int i = 2; i <= token->length(); i++)
	//		{
	//			epSq = squareIndex(token->left(i));
	//			fenEpTgt = squareIndex(token->mid(i));
	//			if (epSq != 0 && fenEpTgt != 0)
	//				break;
	//		}
	//	}
	//	setEnpassantSquare(epSq);
	//	if (m_enpa ssa ntSquare == 0)
	//		return false;

	//	Piece ownPawn(side, Pawn);
	//	Piece opPawn(side.opposite(), Pawn);
	//	int matchesOwn = 0;
	//	int epTgt = 0;

	//	for (const PawnStep& pStep: m_pawnSteps)
	//	{
	//		int sq = epSq - pawnPushOffset(pStep, m_sign);
	//		Piece piece = pieceAt(sq);
	//		if (pStep.type == CaptureStep && piece == ownPawn)
	//			matchesOwn++;
	//		else if (pStep.type == FreeStep && piece == opPawn
	//		&&      (fenEpTgt == 0 || fenEpTgt == sq))
	//			epTgt = sq;
	//	}
	//	// Ignore the en-passant square if an en-passant
	//	// capture isn't possible.
	//	if (!matchesOwn)
	//		setEnpassantSquare(0);
	//	else
	//		// set ep square and target
	//		setEnpassantSquare(epSq, epTgt);
	//}
	if (!isShortFormat)
		++token;

	// Reversible halfmove count
	if (token != fen.end())
	{
		bool ok;
		int tmp = token->toInt(&ok);
		if (!ok || tmp < 0)
			return false;
		m_reversibleMoveCount = tmp;
		++token;
	}
	else
		m_reversibleMoveCount = 0;

	// Read the full move number and calculate m_plyOffset
	if (token != fen.end())
	{
		bool ok;
		int tmp = token->toInt(&ok);
		if (!ok || tmp < 1)
			return false;
		m_plyOffset = 2 * (tmp - 1);
	}
	else
		m_plyOffset = 0;

	if (m_sign != 1)
		m_plyOffset++;

	m_history.clear();
	return true;
}


void WesternBoard::vMakeMove(const Move& move, BoardTransition* transition)
{
	Side side = sideToMove();
	int source = move.sourceSquare();
	int target = move.targetSquare();
	Piece capture = pieceAt(target);
	//int promotionType = move.promotion();
	int pieceType = pieceAt(source).type();

	//int* rookSq = m_castlingRights.rookSquare[side];
	bool clearSource = true;
	bool isReversible = true;

	Q_ASSERT(target != 0);

	//MoveData md = { capture, epSq, epTgt, m_castlingRights,
	//		NoCastlingSide, m_reversibleMoveCount };

	MoveData md = { capture, m_reversibleMoveCount };


	if (source == target)
		clearSource = 0;

	//setEnpassantSquare(0);

	if (pieceType == King)
	{
		m_kingSquare[side] = target;

	}


	if (captureType(move) != Piece::NoPiece)
	{
		//removeCastlingRights(target);
		isReversible = false;
	}

	//if (promotionType != Piece::NoPiece)
	//	isReversible = false;

	if (transition != nullptr)
	{
		if (source != 0)
			transition->addMove(chessSquare(source),
					    chessSquare(target));
		//else
		//	transition->addDrop(Piece(side, pieceType),
		//			    chessSquare(target));
	}

	setSquare(target, Piece(side, pieceType));
	if (clearSource)
		setSquare(source, Piece::NoPiece);

	if (isReversible)
		m_reversibleMoveCount++;
	else
		m_reversibleMoveCount = 0;

	m_history.append(md);
	m_sign *= -1;
}

void WesternBoard::vUndoMove(const Move& move)
{
	const MoveData& md = m_history.last();
	int source = move.sourceSquare();
	int target = move.targetSquare();

	m_sign *= -1;
	Side side = sideToMove();

	//setEnpassantSquare(md.enpassantSquare, md.enpassantTarget);
	m_reversibleMoveCount = md.reversibleMoveCount;
	//m_castlingRights = md.castlingRights;

	//CastlingSide cside = md.castlingSide;
	//if (cside != NoCastlingSide)
	//{
	//	m_kingSquare[side] = source;
	//	// Move the rook back after castling
	//	int tmp = m_castleTarget[side][cside];
	//	setSquare(tmp, Piece::NoPiece);
	//	tmp = (cside == QueenSide) ? tmp + 1 : tmp - 1;
	//	setSquare(tmp, Piece::NoPiece);

	//	setSquare(target, Piece(side, Pao));
	//	setSquare(source, Piece(side, King));
	//	m_history.pop_back();
	//	return;
	//}
	//else 
		
	if (target == m_kingSquare[side])
	{
		m_kingSquare[side] = source;
	}


	//if (move.promotion() != Piece::NoPiece)
	//{
	//	if (source != 0)
	//	{
	//		if (variantHasChanneling(side, source))
	//			setSquare(source, pieceAt(target));
	//		else
	//			setSquare(source, Piece(side, Pawn));
	//	}
	//}
	//else
		setSquare(source, pieceAt(target));

	setSquare(target, md.capture);
	m_history.pop_back();
}

void WesternBoard::generateMovesForPiece(QVarLengthArray<Move>& moves,
					 int pieceType,
					 int sourceSquare) const
{		
	switch (pieceType)
	{
	case Pawn:     // �����߲�
	{
		//int p = sourceSquare;
		Piece piece = pieceAt(sourceSquare);
		if (piece.side() == Side::White) {
			// ���
			Side opSide = sideToMove().opposite();
			for (int i = 0; i < m_RPawnOffsets.size(); i++)
			{
				int targetSquare = sourceSquare + m_RPawnOffsets[i];
				if (!isValidSquare(chessSquare(targetSquare)))
					continue;

				if (sourceSquare > 75) { // ��û�й���
					if (i != 0)  continue;
				}

				Piece capture = pieceAt(targetSquare);
				if (capture.isEmpty() || capture.side() == opSide)
					moves.append(Move(sourceSquare, targetSquare));
			}
		}
		else {
			// �ڱ�
			Side opSide = sideToMove().opposite();
			for (int i = 0; i < m_BPawnOffsets.size(); i++)
			{
				int targetSquare = sourceSquare + m_BPawnOffsets[i];
				if (!isValidSquare(chessSquare(targetSquare)))
					continue;

				if (sourceSquare < 78) { // ��û�й���
					if (i != 0)  continue;
				}

				Piece capture = pieceAt(targetSquare);
				if (capture.isEmpty() || capture.side() == opSide)
					moves.append(Move(sourceSquare, targetSquare));
			}
		}
	}
	break;
	case King:
	{
		Side opSide = sideToMove().opposite();
		for (int i = 0; i < m_CheOffsets.size(); i++)
		{
			int targetSquare = sourceSquare + m_CheOffsets[i];
			if (!isValidSquare(chessSquare(targetSquare)))
				continue;

			if (!isInPalace(chessSquare(targetSquare))) {
				continue;
			}

			Piece capture = pieceAt(targetSquare);
			if (capture.isEmpty() || capture.side() == opSide)
				moves.append(Move(sourceSquare, targetSquare));
		}
	}
	break;
	case Shi:
	{
		Side opSide = sideToMove().opposite();
		for (int i = 0; i < m_ShiOffsets.size(); i++)
		{
			int targetSquare = sourceSquare + m_ShiOffsets[i];
			if (!isValidSquare(chessSquare(targetSquare)))
				continue;

			if (!isInPalace(chessSquare(targetSquare))) {
				continue;
			}

			Piece capture = pieceAt(targetSquare);
			if (capture.isEmpty() || capture.side() == opSide)
				moves.append(Move(sourceSquare, targetSquare));
		}
	}
	break;
	case Che:     // ��
	{
		//break;
		Side side = sideToMove();
		for (int i = 0; i < m_CheOffsets.size(); i++)
		{
			int offset = m_CheOffsets[i];
			int targetSquare = sourceSquare + offset;
			Piece capture;
			while (!(capture = pieceAt(targetSquare)).isWall()
				&& capture.side() != side)
			{
				moves.append(Move(sourceSquare, targetSquare));
				if (!capture.isEmpty())
					break;
				targetSquare += offset;
			}
		}
	}
	break;
	case Pao:     // ��
	{
		Side side = sideToMove();

		for (int i = 0; i < m_CheOffsets.size(); i++)
		{
			int offset = m_CheOffsets[i];
			int targetSquare = sourceSquare + offset;
			Piece capture;
			// �����Ӳ�
			while (!(capture = pieceAt(targetSquare)).isWall() && capture.isEmpty())
			{
				moves.append(Move(sourceSquare, targetSquare));
				if (!capture.isEmpty())
					break;
				targetSquare += offset;
			}
			// ���Ӳ�
			if (!capture.isEmpty()) {
				while (true) // && capture.isEmpty())
				{
					targetSquare += offset;
					capture = pieceAt(targetSquare);
					if (capture.isEmpty()) {
						continue;
					}
					if (capture.isWall()) {
						break;
					}
					if (capture.side() != side) {
						moves.append(Move(sourceSquare, targetSquare));
						break;
					}
				}
			}
		}
		break;
	}
	case Ma:      // ����߲�		
	{
		//break;
		Side opSide = sideToMove().opposite();
		for (int i = 0; i < m_MaOffsets.size(); i++)
		{
			int targetSquare = sourceSquare + m_MaOffsets[i];
			if (!isValidSquare(chessSquare(targetSquare)))
				continue;
			int leg = sourceSquare + m_MaLegOffsets[i];
			if (!pieceAt(leg).isEmpty())
				continue;     // ������
			Piece capture = pieceAt(targetSquare);
			if (capture.isEmpty() || capture.side() == opSide)
				moves.append(Move(sourceSquare, targetSquare));
		}
	}
	break;
	case Xiang:
	{
		//QVarLengthArray<int> m_XiangOffsets;       // ��
		//QVarLengthArray<int> m_XiangEyeOffsets;    // ����
		Side opSide = sideToMove().opposite();
		for (int i = 0; i < m_XiangOffsets.size(); i++)
		{
			int targetSquare = sourceSquare + m_XiangOffsets[i];
			if (!isValidSquare(chessSquare(targetSquare)))
				continue;
			int leg = sourceSquare + m_XiangEyeOffsets[i];
			if (!pieceAt(leg).isEmpty())
				continue;     // ������

			Piece capture = pieceAt(targetSquare);
			if (capture.isEmpty() || capture.side() == opSide) {

				// ���ܹ���	
				if (sourceSquare > 78 && targetSquare < 78) {
					continue;
				}
				if (sourceSquare < 78 && targetSquare > 78) {
					continue;
				}
				moves.append(Move(sourceSquare, targetSquare));
			}
		}
	}
	break;
	default:
		return;
	}	
	return;
}



bool WesternBoard::inCheck(Side side /*, int square*/) const
{
	Side opSide = side.opposite();
	//Piece opKing(opSide, King);
	Piece piece;

	int ksquare = m_kingSquare[side];

	//if (ksquare != 27 && ksquare != 126) {
	//	int a = 0;
	//}

	// �Ƿ�������ڣ���, �Է��Ľ� ����
	for (int i = 0; i < m_CheOffsets.size(); i++)
	{
		int offset = m_CheOffsets[i];
		int targetSquare = ksquare + offset;

		int count = 0;    // һ������ֻ����һ���ھ�
		while (true)
		{
			if (!isValidSquare(chessSquare(targetSquare)))
				break;

			piece = pieceAt(targetSquare);
			if (!piece.isEmpty())
			{
				count++;
				if (count == 1) {        // �ǲ����г��������Է��Ľ��ڽ���
					if (piece.side() == opSide) {  // �ǶԷ�������
						if (piece.type() == Che || piece.type() == King) {
							return true;
						}
						else if (piece.type() == Pawn) {
							if (abs(targetSquare - ksquare) == 1) {
								return true;
							}
							if (side == Side::White) {
								if (targetSquare == ksquare - 11) {
									return true;
								}
							}
							else {
								if (targetSquare == ksquare + 11) {
									return true;
								}
							}
						}
					}
				}
				else if (count == 2) {   // �ǲ��������ڽ�
					if (piece.side() == opSide) {  // �ǶԷ�������
						if (piece.type() == Pao) {
							return true;
						}
					}
					break;
				}
			}
			targetSquare += offset;
		}
	}

	//�Ƿ���� ����

	// Knight, archbishop, chancellor attacks
	for (int i = 0; i < m_MaOffsets.size(); i++)
	{
		piece = pieceAt(ksquare + m_MaOffsets[i]);
		if (piece.side() == opSide && piece.type() == Ma) {
			// ��Ҫ��һ���������ǲ���������
			Piece leg = pieceAt(ksquare + m_MaCheckLegOffsets[i]);

			//int a = ksquare + m_MaCheckLegOffsets[i];
			//int b = m_MaCheckLegOffsets[i];

			if (leg.isEmpty()) {
				return true;
			}
		}

	}

	//Side opSide = sideToMove().opposite();
	//for (int i = 0; i < m_MaOffsets.size(); i++)
	//{
	//	int targetSquare = sourceSquare + m_MaOffsets[i];
	//	if (!isValidSquare(chessSquare(targetSquare)))
	//		continue;
	//	int leg = sourceSquare + m_MaLegOffsets[i];
	//	if (!pieceAt(leg).isEmpty())
	//		continue;     // ������
	//	Piece capture = pieceAt(targetSquare);
	//	if (capture.isEmpty() || capture.side() == opSide)
	//		moves.append(Move(sourceSquare, targetSquare));
	//}

	

	
	return false;
}

bool WesternBoard::isLegalPosition()
{
	Side side = sideToMove().opposite();
	if (inCheck(side))
		return false;

	if (m_history.isEmpty())
		return true;

	return true;
}



bool WesternBoard::vIsLegalMove(const Move& move)
{
	Q_ASSERT(!move.isNull());

	//if (!m_kingCanCapture
	//&&  move.sourceSquare() == m_kingSquare[sideToMove()]
	//&&  captureType(move) != Piece::NoPiece)
	//	return false;

	return Board::vIsLegalMove(move);
}



int WesternBoard::kingSquare(Side side) const
{
	Q_ASSERT(!side.isNull());
	return m_kingSquare[side];
}



int WesternBoard::reversibleMoveCount() const
{
	return m_reversibleMoveCount;
}

Result WesternBoard::result()
{
	QString str;

	// Checkmate/Stalemate
	if (!canMove())
	{
		//if (inCheck(sideToMove()))
		//{
			Side winner = sideToMove().opposite();
			str = tr("%1 ȡ��ʤ����").arg(winner.toString());

			return Result(Result::Win, winner, str);
		//}
		//else
		//{
		//	str = tr("Draw by stalemate");
		//	return Result(Result::Draw, Side::NoSide, str);
		//}
	}

	// Insufficient mating material
	int material = 0;
	bool bishops[] = { false, false };
	for (int i = 0; i < arraySize(); i++)
	{
		const Piece& piece = pieceAt(i);
		if (!piece.isValid())
			continue;

		switch (piece.type())
		{
		case King:
			break;
		case Shi:
		{
			//auto color = chessSquare(i).color();
			//if (color != Square::NoColor && !bishops[color])
			//{
			//	material++;
			//	bishops[color] = true;
			//}
			break;
		}
		case Xiang:
			//material++;
			break;
		default:
			material += 2;
			break;
		}
	}
	if (material <= 0)
	{
		str = tr("˫���޽����������к�");
		return Result(Result::Draw, Side::NoSide, str);
	}

	// ���������״̬������ʱ�����ж�
	//if(this->m_)

	if (Board::GetIsAutoLinkStat()) {
		// 50 move rule
		if (m_reversibleMoveCount >= 160)
		{
			str = tr("60 ��δ���ӣ��кͣ�");
			return Result(Result::Draw, Side::NoSide, str);
		}

		// 3-fold repetition
		if (repeatCount() >= 10)
		{
			str = tr("ѭ���������кͣ�");
			return Result(Result::Draw, Side::NoSide, str);
		}
	}
	else {
		// 50 move rule
		if (m_reversibleMoveCount >= 120)
		{
			str = tr("60 ��δ���ӣ��кͣ�");
			return Result(Result::Draw, Side::NoSide, str);
		}

		// 3-fold repetition
		if (repeatCount() >= 5)
		{
			str = tr("ѭ���������кͣ�");
			return Result(Result::Draw, Side::NoSide, str);
		}
	}

	

	return Result();
}

} // namespace Chess
