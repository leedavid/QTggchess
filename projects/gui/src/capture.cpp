#include "capture.h"

#include <QThread>
#include <QScreen>
#include <QPixmap>
#include <QRect>
#include <QDir>
#include <QGuiApplication>
#include <QMessagebox>
#include <board/boardfactory.h>

#include "mainwindow.h"

namespace Chess {


	Capture::Capture(QObject* parent, bool isAuto)
		:QThread(parent),
		m_isAutoClick(isAuto),
		m_isRuning(false),
		m_bSendInitFen(false),
		m_bMainGetFenAlready(false),
		pMain((MainWindow*)(parent))
	{
		//MsgBoxThread(par);
		//this->m_msg.pGame = pGame;

		initBoard();

		connect(this, SIGNAL(CapSendSignal(stCaptureMsg)),
			parent, SLOT(processCapMsg(stCaptureMsg)));
	}

	void Capture::initBoard()
	{
		m_precision_chess = 0.52f;
		m_precision_auto = 0.98f;
		m_UseAdb = false;
		m_sleepTimeMs = 1;
		m_scaleX = 1.0f;
		m_scaleY = 1.0f;

		m_Ready_LXset = false;
		m_chessWinOK = false;

		//m_chessClip = 0.25f;

		this->m_board = Chess::BoardFactory::create("standard");
		//this->m_board_second = Chess::BoardFactory::create("standard");
		m_board->reset();
		//m_board_second->reset();


		this->m_LxInfo.m_LX_name = "��������С����";
		this->m_LxInfo.m_titleKeyword = "�й�����2017"; // "BHGUI(test) - �����";
		this->m_LxInfo.m_class = "QQChess"; // "Afx:00400000:b:00010003:00000006:0A1D05FB";

		//this->m_LxInfo.offx = 29.0f;
		//this->m_LxInfo.offy = 138.0f;
		//this->m_LxInfo.m_dx = 28.0f;
		//this->m_LxInfo.m_dy = 28.0f;

		this->m_LxInfo.offx = 73.0f;
		this->m_LxInfo.offy = 175.0f;
		this->m_LxInfo.m_dx = 68.0f;
		this->m_LxInfo.m_dy = 68.0f;

		this->m_Ready_LXset = false;

		this->m_LxInfo.m_PieceCatlog = "0";
		this->m_connectedBoard_OK = false;  // 

		m_side = Chess::Side::NoSide;

		m_flip = false;

	}

	cv::Mat Capture::QImageToCvMat(const QImage& inImage, bool inCloneImageData)
	{
		switch (inImage.format())
		{
			// 8-bit, 4 channel
		case QImage::Format_ARGB32:
		case QImage::Format_ARGB32_Premultiplied:
		{
			cv::Mat  mat(inImage.height(), inImage.width(),
				CV_8UC4,
				const_cast<uchar*>(inImage.bits()),
				static_cast<size_t>(inImage.bytesPerLine())
			);

			return (inCloneImageData ? mat.clone() : mat);
		}

		// 8-bit, 3 channel
		case QImage::Format_RGB32:
		case QImage::Format_RGB888:
		{
			if (!inCloneImageData)
			{
				qWarning() << "CVS::QImageToCvMat() - Conversion requires cloning because we use a temporary QImage";
			}

			QImage   swapped = inImage;

			if (inImage.format() == QImage::Format_RGB32)
			{
				swapped = swapped.convertToFormat(QImage::Format_RGB888);
			}

			swapped = swapped.rgbSwapped();

			return cv::Mat(swapped.height(), swapped.width(),
				CV_8UC3,
				const_cast<uchar*>(swapped.bits()),
				static_cast<size_t>(swapped.bytesPerLine())
			).clone();
		}

		// 8-bit, 1 channel
		case QImage::Format_Indexed8:
		{
			cv::Mat  mat(inImage.height(), inImage.width(),
				CV_8UC1,
				const_cast<uchar*>(inImage.bits()),
				static_cast<size_t>(inImage.bytesPerLine())
			);

			return (inCloneImageData ? mat.clone() : mat);
		}

		default:
			qWarning() << "CVS::QImageToCvMat() - QImage format not handled in switch:" << inImage.format();
			break;
		}

		return cv::Mat();
		// https ://blog.csdn.net/qq_17550379/article/details/78683153
	}

	cv::Mat Capture::QPixmapToCvMat(const QPixmap& inPixmap, bool inCloneImageData)
	{
		return QImageToCvMat(inPixmap.toImage(), inCloneImageData);
	}

	void Capture::winLeftClick(HWND hwnd, int x, int y)
	{
		/*	if (this->isUseAdb) {
				this->pAdb->LeftClick(x, y);
			}
			else {*/
		LONG temp = MAKELONG(x, y);
		// click
		//::PostMessage(hwnd, WM_ACTIVATE, WA_ACTIVE, 0);
		//::PostMessage(hwnd, WM_LBUTTONDOWN, MK_LBUTTON, temp);
		//wait(10);
		//::PostMessage(hwnd, WM_LBUTTONUP, 0, temp);

		//::SendMessage(hwnd, WM_ACTIVATE, WA_ACTIVE, 0);
		//::SendMessage(hwnd, WM_LBUTTONDOWN, MK_LBUTTON, temp);
		::SendMessage(hwnd, WM_LBUTTONDOWN, 0, temp);
		::SendMessage(hwnd, WM_LBUTTONUP, 0, temp);
		//}
	}

	// �������淢�����岽���������
	void Capture::ProcessBoardMove(const Chess::GenericMove& move) {
		int fx = move.sourceSquare().file();
		int fy = move.sourceSquare().rank();

		int tx = move.targetSquare().file();
		int ty = move.targetSquare().rank();

		// �����߲�
		//int fx = 7;
		//int fy = 0;
		//int tx = 6;
		//int ty = 2;



		//m_hwnd = (HWND)0x0052140E;

		//ShowWindow(m_hwnd, SW_RESTORE);
		//SetForegroundWindow(m_hwnd);

		//for (ffx = 5; ffx < 300; ffx += 5) {
		//	for (ffy = 5; ffy < 400; ffy += 5) {
		//		winLeftClick(m_hwnd, ffx, ffy);
		//		wait(1);
		//	}
		//}

		int from = (9 - fy) * 9 + fx;
		int to = (9 - ty) * 9 + tx;
		ChinesePieceType piece = this->m_LxBoard[0].b90[from];
		if (piece <= eBKing && piece >= eBPawn) {
			m_side = Chess::Side::White;
		}
		else if (piece <= eRKing && piece >= eRPawn) {
			m_side = Chess::Side::Black;
		}
		else {
			//this->SendMessageToMain("error", "move error");
			return;
		}

		// ����һ�µ�ǰ����

		if (m_flip) {
			fx = 8 - fx;
			fy = 9 - fy;
			tx = 8 - tx;
			ty = 9 - ty;
		}


		cLXinfo* pInfo = &this->m_LxInfo;

		int ffx = pInfo->offx + fx * pInfo->m_dx;
		int ffy = pInfo->offy + (9 - fy) * pInfo->m_dy;

		int ttx = pInfo->offx + tx * pInfo->m_dx;
		int tty = pInfo->offy + (9 - ty) * pInfo->m_dy;

		this->m_LxBoard[0].b90[from] = ChinesePieceType::eNoPice;
		this->m_LxBoard[0].b90[to] = piece;

		winLeftClick(m_hwnd, ffx, ffy);
		wait(5);
		winLeftClick(m_hwnd, ttx, tty);


		//m_side = Chess::Side::NoSide;


		//wait(20);

		//winLeftClick(m_hwnd, 0, 0);
		//int a = 0;
	}

	bool Capture::GetLxInfo(QString catlog)  // ֻҪ�鵽��û�г�������
	{

		m_Ready_LXset = false;

		cLXinfo* pInfo = &this->m_LxInfo;

		stLxBoard* pPieceList = &m_LxBoard[0];

		pInfo->m_PieceCatlog = catlog;


		if (!getChessboardHwnd(true))
			return false;

		// 
		cv::Point Bche0;
		cv::Point Bche1;

		if (pPieceList->BCheList[0].x < pPieceList->BCheList[1].x) {
			Bche0 = pPieceList->BCheList[0];
			Bche1 = pPieceList->BCheList[1];
		}
		else {
			Bche1 = pPieceList->BCheList[0];
			Bche0 = pPieceList->BCheList[1];
		}

		pInfo->offx = Bche0.x;
		pInfo->offy = Bche0.y;

		pInfo->m_dx = (Bche1.x - Bche0.x) / 8;
		pInfo->m_dy = pInfo->m_dx;

		if (pInfo->m_dx < 10) return false;

		pInfo->m_class = this->get_window_class(this->m_hwnd);
		pInfo->m_titleKeyword = this->get_window_title(this->m_hwnd);

		// �����̸�СһЩ����

		//
		m_Ready_LXset = true;
		return this->SaveAllPiecePicture();
	}


	bool Capture::SearchAndClick(QString findName, bool isCap, QString sub_catlog, HWND hw, float threshold)
	{
		if (sub_catlog == nullptr) {
			sub_catlog = "0/auto/";
		}
		if (hw == nullptr) {
			hw = m_hwnd;
		}

		int imgX, imgY;
		if (!searchImage(findName, isCap, sub_catlog, imgX, imgY, threshold)) {
			return false;
		}

		this->winLeftClick(hw, imgX, imgY);
		//wait(1000);

		return true;
	}

	bool Capture::searchImage(QString findName, bool isCap, QString sub_catlog, float threshold)
	{
		int imgX, imgY;
		if (sub_catlog == nullptr) {
			sub_catlog = "0/auto/";
		}
		return searchImage(findName, isCap, sub_catlog, imgX, imgY, threshold);
	}

	bool Capture::searchImage(QString findName, bool isCap, QString sub_catlog, int& imgX, int& imgY, float threshold)
	{
		if (isCap) {
			if (this->captureOne(m_hwnd, false) == false) {
				qWarning("searchImage 1 %s �����ˣ�", findName);
				return false;
			}
		}

		if (sub_catlog != nullptr) {
			findName = sub_catlog + findName;
		}

		cv::Mat image_template_main;
		try {

			if (this->m_MatHash.contains(findName)) {              // �����ڻ�����			
				image_template_main = this->m_MatHash.value(findName);
			}
			else {
				QString fFile = this->getFindPath() + findName;
				image_template_main = cv::imread(fFile.toStdString());   // ģ��ͼ
				// Ҫ��Ҫ����
				//if (this->m_scaleX != 1.0f) {
				//	cv::resize(image_template_main, image_template_main, cv::Size(), this->m_scaleX, this->m_scaleY);
				//}
				this->m_MatHash.insert(findName, image_template_main);
			}
		}
		catch (...) {
			//qWarning("searchImage 2 %s �����ˣ�", findName);
			return false;
		}


		//cv::imshow("templ", m_image_black);
		//cv::imshow("img", m_image_source);
		//cv::imshow("matched", image_template2);
		//cv::waitKey();	

		cv::Mat image_matched;
		try {
			cv::matchTemplate(m_image_source_all, image_template_main, image_matched, cv::TM_SQDIFF_NORMED); // cv::TM_CCORR); // cv::TM_SQDIFF);
		}
		catch (...) {
			qWarning("searchImage 3 %s �����ˣ�", findName);
			return false;
		}

#if 0
		cv::imshow("m_image_source", m_image_source);
		//cv::imshow("templ", image_template2);
		cv::imshow("m_image_black", m_image_red);
		cv::imshow("matched", image_matched);
		cv::waitKey();
#endif
		try {

			if (threshold == 1.0f) {
				threshold = this->m_precision_auto;
			}

			cv::Point minLoc, maxLoc;
			double minVal, maxVal;

			//Ѱ�����ƥ��λ��
			cv::minMaxLoc(image_matched, &minVal, &maxVal, &minLoc, &maxLoc);

			double matchThres = maxVal * (1 - threshold);

			if (minVal < matchThres) {
				imgX = minLoc.x + image_template_main.cols / 2;
				imgY = minLoc.y + image_template_main.rows / 2;
				return true;
			}
			else {
				return false;
			}
		}
		catch (...) {
			//qWarning("searchImage 5 %s �����ˣ�", findName);
			return false;
		}


		return false;
}

	

	void Capture::SendMessageToMain(const QString title, const QString msg)
	{
		m_msg.mType = stCaptureMsg::eText;
		m_msg.text = title;
		m_msg.title = msg;
		emit CapSendSignal(m_msg);
	}

	void Capture::SendMoveToMain(const Chess::GenericMove m)
	{
		m_msg.mType = stCaptureMsg::eMove;
		m_msg.m = m;
		emit CapSendSignal(m_msg);
	}

	void Capture::SendFenToMain(const QString fen)
	{
		m_msg.mType = stCaptureMsg::eSetFen;
		m_msg.text = fen;
		emit CapSendSignal(m_msg);
	}

	//QString Capture::GetFenLxBoard(bool isOrg)
	//{
	//	if (isOrg) {
	//		return this->m_LxBoard[0].pl
	//	}
	//}

	QChar Capture::Qpiece_to_char(int chess)
	{
		static const  QChar PieceString[]
			= { '0','p','b','a','c','n','r','k','P','B','A','C','N','R','K' };
		return PieceString[chess];
	}

	bool Capture::isSolutionReady()
	{
		return m_Ready_LXset;
	}

	int Capture::getB90(cv::Point p)
	{
		int dx = (int)((p.x - m_LxInfo.offx) / m_LxInfo.m_dx + 0.5f);
		int dy = (int)((p.y - m_LxInfo.offy) / m_LxInfo.m_dy + 0.5f);

		if (m_flip) {
			dy = 9 - dy;
			dx = 8 - dx;
		}

		int s90 = dx + 9 * dy;

		if (s90 > 89) s90 = 89;
		return s90;
	}

	bool Capture::fillB90(ChinesePieceType b90[], QVector<cv::Point>& plist, ChinesePieceType chess)
	{
		for (auto p : plist) {
			int s90 = getB90(p);
			b90[s90] = chess;
		}

		return true;
	}

	bool Capture::GetFen(stLxBoard* pList)
	{
		//int b90[90] = { 0 };
		//int a = sizeof(pList->b90);
		memset(pList->b90, 0, sizeof(pList->b90));


		//bool flip = false;
		if (pList->BKingList[0].y > pList->RKingList[0].y) {
			m_flip = true;
		}
		else {
			m_flip = false;
		}

		fillB90(pList->b90, pList->RCheList, eRChe);
		fillB90(pList->b90, pList->RMaList, eRMa);
		fillB90(pList->b90, pList->RPaoList, eRPao);
		fillB90(pList->b90, pList->RShiList, eRShi);
		fillB90(pList->b90, pList->RXiangList, eRXiang);
		fillB90(pList->b90, pList->RPawnList, eRPawn);
		fillB90(pList->b90, pList->RKingList, eRKing);
				
		fillB90(pList->b90, pList->BCheList, eBChe);
		fillB90(pList->b90, pList->BMaList, eBMa);
		fillB90(pList->b90, pList->BPaoList, eBPao);
		fillB90(pList->b90, pList->BShiList, eBShi);
		fillB90(pList->b90, pList->BXiangList, eBXiang);
		fillB90(pList->b90, pList->BPawnList, eBPawn);
		fillB90(pList->b90, pList->BKingList, eBKing);
		
		// getFen from B90


		QString fen;
		QChar c;

		for (int rank = 0; rank <= 9; rank++) {
			for (int file = 0; file <= 8; ) {
				int s90 = file + rank * 9;
				int chess = pList->b90[s90];

				if (chess == 0) {
					int len = 0;
					for (; file <= 8 && pList->b90[file + rank * 9] == 0; file++) {
						len++;
					}
					c = '0' + len;
				}
				else {
					c = Qpiece_to_char(chess);
					file++;
				}
				fen += c;
			}
			fen += (rank < 9 ? '/' : ' ');
		}
		fen += (m_flip == false ? "w " : "b ");   // ���fen ֻ��һ��
		fen += "- - 0 1";

		pList->fen = fen;

		return true;
	}

	//���¾���������ı仯����ҵ���һ������״̬
	bool Capture::Board2Move(Chess::GenericMove& m)
	{
		ChinesePieceType* pre_board = this->m_LxBoard[0].b90;
		ChinesePieceType* now_board = this->m_LxBoard[1].b90;

		int fx = -1;
		int fy = -1;
		int tx = -1;
		int ty = -1;
	

		for (int y = 0; y < 10; y++) {
			for (int x = 0; x < 9; x++) {
				ChinesePieceType chess2 = pre_board[y * 9 + x];
				ChinesePieceType chess1 = now_board[y * 9 + x];

				if (chess2 != chess1) {     // �������߶�
					if (chess1 == eNoPice) {       // 
						fx = x;
						fy = y;						
					}
					else {
						tx = x;
						ty = y;						
					}					
				}
			}
		}

		if (fx != -1 && fy != -1 && tx != -1 && ty != -1) {
			int from = fy * 9 + fx;
			int to = ty * 9 + tx;

			
			//if (m_flip) {
			//	from = (9 - fy) * 9 + (8-fx);
			//	to = (9 - ty) * 9 + (8-tx);
			//}
			

			if (from != to) {

				ChinesePieceType piece = pre_board[from];
				if (piece <= eBKing && piece >= eBPawn) {
					if (m_side != Chess::Side::Black) {
						return false;
					}
				}
				else if (piece <= eRKing && piece >= eRPawn) {
					if (m_side != Chess::Side::White) {
						return false;
					}
				}
				else {
					//this->SendMessageToMain("error", "move error");
					return false;
				}

				//if (m_flip) {
				//	fx = 8 - fx;
				//	fy = 9 - fy;
				//	tx = 8 - tx;
				//	ty = 9 - ty;
				//}

				Square from_square = Square(fx, 9 - fy);
				Square to_square = Square(tx, 9 - ty);


				m = Chess::GenericMove(from_square, to_square);

				if (pMain->isMoveValid(m) == true) {
					// ����
					this->m_LxBoard[0].b90[from] = eNoPice;
					this->m_LxBoard[0].b90[to] = piece;



					//m = 0x331e;

					//int source = Chess::sq

					//Square(int file, int rank);
					//
					//Chess::GenericMove gmove;
					//gmove.
					//
					//m = Chess::Move(from, to);
					return true;

				}
				
			}
		}
		
		return false;
	}

	// �߳�����
	void Capture::run() {

		m_isRuning = true;

		this->m_MatHash.clear(); // ���һ��

		if (m_isAutoClick) {    // ȫ�Զ�����	

			bMustStop = false;
			m_bSendInitFen = false;

			
			this->m_Ready_LXset = true;
			//this->GetMoveFromBoard();
			if (!isSolutionReady()) {

				this->GetLxInfo("0");

				if (!isSolutionReady()) {
					SendMessageToMain("������", "���߷�����û��׼���ã�");
					return;
				}
			}	

			if (isFindAutoWin() == false) {
				return;
			}

			//this->m_hwnd = HWND(0x00030806);
			QString last_fen; // = "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR b - - 0 1";
			QString last_send_fen; 
			bool MayBeEnd; 
			MayBeEnd = false;
			

			while (true) {
				if (bMustStop) break;		

				//------------------------------------------------------------------------
				// ��������Զ�Ŀ¼�µ�ͼ
				QStringList nameFilters;
				nameFilters << "*.png";

				QString runPath = QCoreApplication::applicationDirPath();
				QString dirpath = runPath + "/image/findchess/0/auto/";

				QDir dir(dirpath);

				bool find = true;
				
				while (find) {
					find = false;
					QStringList files = dir.entryList(nameFilters, QDir::Files | QDir::Readable, QDir::Name);
					for (QString file : files) {						

						while (true) {   // �����治���ٷ�����
							if (searchImage("gate.png", true, "0/not/")) {
								wait(500);
							}
							else {
								break;
							}
							if (bMustStop) break;
						}

						if (this->SearchAndClick(file, true)) {
							find = true;
							MayBeEnd = true;
							wait(200);
						}
						if (bMustStop) break;
					}
				}
				//----------------------------------------------------------------------------


				// 1. ���ֳ�ʼFEN�� �ͷ���ʼ��Ϣ

				if (GetLxBoardChess(true)) {    // ������������Ϣ
					QString fen = this->m_LxBoard[0].fen;

					//if (m_bSendInitFen == true) {
						// ���fen�ǳ�ʼ����fen�� ��Ҫ�ȴ��Է�����
						// rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR b - - 0 1
					//if (fen == "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR b - - 0 1") {
					//	m_bSendInitFen = false;						
					//}
					//if (fen == "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1") {
					//	m_bSendInitFen = false;				
					//}
					if (MayBeEnd) {
						last_send_fen = "";
						MayBeEnd = false;
					}

					if (last_send_fen != fen) {

						if (fen == "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR b - - 0 1") {
							m_bSendInitFen = false;
							// ����ǶԷ��ߺ죬�����ǣ�Ҫ��һ��
							last_send_fen = fen;
						}
						if (fen == "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1") {
							m_bSendInitFen = false;
						}
						if (last_send_fen != fen) {
							if (m_bSendInitFen == false) {
								wait(300);
								SendFenToMain(fen);
								
								m_bSendInitFen = true;
								last_send_fen = fen;
							}
						}
					}

					last_fen = fen;
				}
				else {
					//m_bSendInitFen = false;
				}
				wait(500);  
			}
		}
		else {

			bMustStop = false;
			m_bSendInitFen = false;

			this->m_Ready_LXset = true;
			//this->GetMoveFromBoard();
			if (!isSolutionReady()) {

				this->GetLxInfo("0");

				if (!isSolutionReady()) {

					SendMessageToMain("������", "���߷�����û��׼���ã�");
					return;
				}
			}

			QString MoveSendingFen;
			Chess::GenericMove MoveSendingMove;

			while (true) {   // ����

				if (bMustStop) break;

				/*
				//HWND m_hWndProg = (HWND)0x00711024;
				////HWND m_hWndBoard = (HWND)0x0052140E;

				//HWND m_hWndBoard = (HWND)0x00711024;

				////m_hwnd = (HWND)0x016A09DC;

				//ShowWindow(m_hWndProg, SW_RESTORE);
				//SetForegroundWindow(m_hWndProg);

				//for (int ffx = 5; ffx < 300; ffx += 5) {
				//	for (int ffy = 5; ffy < 400; ffy += 5) {


				//

				//		BringWindowToTop(m_hWndProg);       //������һ�²�ͬ�ĺ���Ч��
				//		SetForegroundWindow(m_hWndProg);

				//		winLeftClick(m_hWndBoard, ffx, ffy);
				//		wait(1);
				//	}
				//}

				*/

				if (m_bSendInitFen == false) {
					if (GetLxBoardChess(true)) {
						QString fen = this->m_LxBoard[0].fen;

						// ���fen�ǳ�ʼ����fen�� ��Ҫ�ȴ��Է�����
						// rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR b - - 0 1
						if (fen == "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR b - - 0 1") {
						}
						else {
							if (m_bMainGetFenAlready == false) {
								SendFenToMain(fen);
							}
							m_bSendInitFen = true;
						}
					}
				}
				else {
					// ��ȡ�Է����߲������͵����߳���
					if (GetLxBoardChess(false)) {   // �����ڶ�������''

						Chess::GenericMove m;
						if (this->Board2Move(m)) {
							//SendMoveToMain(m);
							MoveSendingFen = this->m_LxBoard[1].fen;
							MoveSendingMove = m;
						}

						if (this->m_LxBoard[1].fen == MoveSendingFen) {   // ����û�иĶ�
							if (MoveSendingMove.isNull() == false) {
								SendMoveToMain(m);
							}
						}						
					}
				}
				wait(m_sleepTimeMs);
			}
			//SendMessageToMain("OK", "�����˳����ߣ�");
		}

		m_isRuning = false;
	}

	//Chess::Move Capture::GetMoveFromBoard()
	//{
	//	//while (true)
	//	//{
	//	//	this->GetLxBoardChess();
	//	//}
	//	
	//	return Chess::Move();
	//}

	// �����߳�
	void Capture::on_start()
	{
		this->start();	
	}

	void Capture::on_stop()
	{
		bMustStop = true;
	}

	// �õ����е������б�
	bool Capture::GetLxBoardChess(bool org)
	{

		if (this->m_Ready_LXset == false)
			this->GetLxInfo("0");

		if (this->m_Ready_LXset == false) return false;

		if (this->m_connectedBoard_OK == false) {  // ��û����������
			if (!getChessboardHwnd()) {
				qWarning("�Ҳ������̣�");
				return false;
			}			
		}

		if (this->m_connectedBoard_OK == false) return false;

		// 
		stLxBoard* pList = &m_LxBoard[1];
		if (org) {
			pList = &m_LxBoard[0];
		}

		if (!SearchOnChessList(m_hwnd, "bk.png", pList->BKingList, SearchWhichCap::eBlack, true)) {    // �ڽ�
			return false;  // �Ҳ����ڽ���
		}

		if (!SearchOnChessList(m_hwnd, "rk.png", pList->RKingList, SearchWhichCap::eRed)) {    // �콫
			return false; // �Ҳ����콫��
		}


		SearchOnChessList(m_hwnd, "br.png", pList->BCheList, SearchWhichCap::eBlack);     // �ڳ�
		SearchOnChessList(m_hwnd, "bn.png", pList->BMaList, SearchWhichCap::eBlack);      // ����
		SearchOnChessList(m_hwnd, "bc.png", pList->BPaoList, SearchWhichCap::eBlack);     // ����
		SearchOnChessList(m_hwnd, "ba.png", pList->BShiList, SearchWhichCap::eBlack);     // ��ʿ
		SearchOnChessList(m_hwnd, "bb.png", pList->BXiangList, SearchWhichCap::eBlack);   // ����
		SearchOnChessList(m_hwnd, "bp.png", pList->BPawnList, SearchWhichCap::eBlack);    // �ڱ�


		SearchOnChessList(m_hwnd, "rr.png", pList->RCheList, SearchWhichCap::eRed);     // �쳵
		SearchOnChessList(m_hwnd, "rn.png", pList->RMaList, SearchWhichCap::eRed);      // ����
		SearchOnChessList(m_hwnd, "rc.png", pList->RPaoList, SearchWhichCap::eRed);     // ����
		SearchOnChessList(m_hwnd, "ra.png", pList->RShiList, SearchWhichCap::eRed);     // ��ʿ
		SearchOnChessList(m_hwnd, "rb.png", pList->RXiangList, SearchWhichCap::eRed);   // ����
		SearchOnChessList(m_hwnd, "rp.png", pList->RPawnList, SearchWhichCap::eRed);    // ���
		

		return GetFen(pList);
	}

	// �õ����е����ӵ�ͼƬ
	bool Capture::SaveAllPiecePicture()
	{
		// Bche
		SaveOnePiecePic(0, 0, "br.png");
		SaveOnePiecePic(1, 0, "bn.png");
		SaveOnePiecePic(2, 0, "bb.png");
		SaveOnePiecePic(3, 0, "ba.png");
		SaveOnePiecePic(4, 0, "bk.png");

		SaveOnePiecePic(1, 2, "bc.png");
		SaveOnePiecePic(0, 3, "bp.png");

		// �쳵
		SaveOnePiecePic(0, 9, "rr.png");
		SaveOnePiecePic(1, 9, "rn.png");
		SaveOnePiecePic(2, 9, "rb.png");
		SaveOnePiecePic(3, 9, "ra.png");
		SaveOnePiecePic(4, 9, "rk.png");

		SaveOnePiecePic(1, 7, "rc.png");  // ����
		SaveOnePiecePic(0, 6, "rp.png");

		return true;
	}

	bool Capture::SaveOnePiecePic(int x, int y, QString chessName)
	{
		int pieceSize = int(m_LxInfo.m_dx * 0.8f);
		QString picPath = getFindPath() + m_LxInfo.m_PieceCatlog + "/" + chessName;

		QRect rect(
			m_LxInfo.offx - pieceSize / 2 + x * m_LxInfo.m_dx,
			m_LxInfo.offy - pieceSize / 2 + y * m_LxInfo.m_dx,
			pieceSize, pieceSize);
		QPixmap cropped = this->m_capPixmap.copy(rect);
		cropped.save(picPath, "PNG");   // �ڳ�

		//cv::FileStorage

		return true;
	}

	Capture::~Capture()
	{
		delete this->m_board;
		//delete this->m_board_second;
	}

	bool Capture::captureOne(HWND hw, bool isTransHSV, int sleepTimeMs)	{
		if (this->m_UseAdb) {
			//return this->pAdb->one_screenshot(this->capPixmap, disp, fname, sleepTimeMs, path);
			return false;
		}
		else {
			return this->CaptureOneNotry(hw, sleepTimeMs, isTransHSV);
		}
	}

	bool Capture::searchChess(QString findName, bool isCap, SearchWhichCap sWhich, HWND hw)
	{
		QVector<cv::Point> res;
		if (hw == nullptr) {
			hw = this->m_hwnd;
		}
		return this->searchChess(hw, findName, res, sWhich, isCap);
	}

	bool Capture::searchCountours(HWND hw, QString findName,bool isCap) {
		if (isCap) {
			if (this->captureOne(hw) == false) {
				qWarning("searchImage 1 %s �����ˣ�", findName);
				return false;
			}
		}

		cv::namedWindow("Control", cv::WINDOW_AUTOSIZE); //create a window called "Control"
		int iLowH = 100;
		int iHighH = 140;

		int iLowS = 90;
		int iHighS = 255;

		int iLowV = 90;
		int iHighV = 255;

		//Create trackbars in "Control" window		

		cv::createTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
		cv::createTrackbar("HighH", "Control", &iHighH, 179);

		cv::createTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
		cv::createTrackbar("HighS", "Control", &iHighS, 255);

		cv::createTrackbar("LowV", "Control", &iLowV, 255); //Value (0 - 255)
		cv::createTrackbar("HighV", "Control", &iHighV, 255);


		while (true) {

			cv::Mat imgHSV;
			std::vector<cv::Mat> hsvSplit;
			cv::cvtColor(m_image_source, imgHSV, cv::COLOR_BGR2HSV);

			//cv::split(imgHSV, hsvSplit);
			//cv::equalizeHist(hsvSplit[2], hsvSplit[2]);
			//cv::merge(hsvSplit, imgHSV);

			cv::Mat imgThresholded;

			cv::inRange(imgHSV, cv::Scalar(iLowH, iLowS, iLowV), cv::Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image

			// ������ (ȥ��һЩ���)
			cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, 1));
			cv::morphologyEx(imgThresholded, imgThresholded, cv::MORPH_OPEN, element);

			// �ղ��� (����һЩ��ͨ��)
			cv::morphologyEx(imgThresholded, imgThresholded, cv::MORPH_CLOSE, element);
	


			cv::imshow("Thresholded Image", imgThresholded); //show the thresholded image
			cv::imshow("Original", m_image_source); //show the original image

			char key = (char)cv::waitKey(300);
			if (key == 27)
				break;		
		}

		// opencv ʵ���ض���ɫ������ȡ�붨λ
		// https://blog.csdn.net/chenghaoy/article/details/86509950


		//CV_EXPORTS_W void findContours(InputArray image, OutputArrayOfArrays contours,
		//	OutputArray hierarchy, int mode,
		//	int method, Point offset = Point());

		///** @overload */
		//CV_EXPORTS void findContours(InputArray image, OutputArrayOfArrays contours,
		//	int mode, int method, Point offset = Point());

		//cv::Mat m_image_find = m_image_source;   // ת���õ���ͼ
		cv::Mat grayImage;
		cv::Mat out_Canny;

		cv::cvtColor(m_image_source,grayImage, cv::COLOR_BGR2GRAY);

		cv::Mat hsvImage;
		cv::cvtColor(m_image_source, hsvImage, cv::COLOR_BGR2HSV);

		int hueValue = 0; // red color
		int hueRange = 15; // how much difference from the desired color we want to include to the result If you increase this value, for example a red color would detect some orange values, too.

		//int minSaturation = 50; // I'm not sure which value is good here...
		//int minValue = 50; // not sure whether 50 is a good min value here...

		cv::Mat hueMask;
		cv::inRange(hsvImage, hueValue - hueRange, hueValue + hueRange, hueMask);


		int min_Thresh = 240;
		int max_Thresh = 255;
		Canny(grayImage, out_Canny, min_Thresh, max_Thresh * 2, 3);

		//cv::imshow("source", m_image_source);
		cv::imshow("canny", out_Canny);
		//cv::waitKey();

		//CV_RETR_EXTERNALֻ�������Χ��������������Χ�����ڵ���Χ����������
		std::vector<std::vector<cv::Point>> g_vContours;
		std::vector<cv::Vec4i> g_vHierarchy;		

		cv::RNG G_RNG(1234);

		cv::findContours(out_Canny, g_vContours, g_vHierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

		cv::Mat Drawing = cv::Mat::zeros(out_Canny.size(), CV_8UC3);

		for (int i = 0; i < g_vContours.size(); i++) {
			cv::Scalar color = cv::Scalar(G_RNG.uniform(0, 255), G_RNG.uniform(0, 255), G_RNG.uniform(0, 255));
			cv::drawContours(Drawing, g_vContours, i, color, 2, 8, g_vHierarchy, 0);
		}

		cv::imshow("gray", grayImage);

		cv::imshow("drawing", Drawing);

		//cv::imshow("contours", grayImage);
		cv::waitKey();

		return true;

	}

	bool Capture::getKingInfo(HWND hw, QString findName, bool isCap)
	{
		if (isCap) {
			if (this->captureOne(hw) == false) {
				qWarning("searchImage 1 %s �����ˣ�", findName);
				return false;
			}
		}

		cv::namedWindow("Control", cv::WINDOW_AUTOSIZE); //create a window called "Control"
		int iLowH = 100;
		int iHighH = 140;

		int iLowS = 90;
		int iHighS = 255;

		int iLowV = 90;
		int iHighV = 255;

		//Create trackbars in "Control" window		

		cv::createTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
		cv::createTrackbar("HighH", "Control", &iHighH, 179);

		cv::createTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
		cv::createTrackbar("HighS", "Control", &iHighS, 255);

		cv::createTrackbar("LowV", "Control", &iLowV, 255); //Value (0 - 255)
		cv::createTrackbar("HighV", "Control", &iHighV, 255);


		while (true) {

			cv::Mat imgHSV;
			std::vector<cv::Mat> hsvSplit;
			cv::cvtColor(m_image_source, imgHSV, cv::COLOR_BGR2HSV);

			//cv::split(imgHSV, hsvSplit);
			//cv::equalizeHist(hsvSplit[2], hsvSplit[2]);
			//cv::merge(hsvSplit, imgHSV);

			cv::Mat imgThresholded;

			cv::inRange(imgHSV, cv::Scalar(iLowH, iLowS, iLowV), cv::Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image

			// ������ (ȥ��һЩ���)
			cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, 1));
			cv::morphologyEx(imgThresholded, imgThresholded, cv::MORPH_OPEN, element);

			// �ղ��� (����һЩ��ͨ��)
			cv::morphologyEx(imgThresholded, imgThresholded, cv::MORPH_CLOSE, element);



			cv::imshow("Thresholded Image", imgThresholded); //show the thresholded image
			cv::imshow("Original", m_image_source); //show the original image

			char key = (char)cv::waitKey(300);
			if (key == 27)
				break;
		}

		// opencv ʵ���ض���ɫ������ȡ�붨λ
		// https://blog.csdn.net/chenghaoy/article/details/86509950


		//CV_EXPORTS_W void findContours(InputArray image, OutputArrayOfArrays contours,
		//	OutputArray hierarchy, int mode,
		//	int method, Point offset = Point());

		///** @overload */
		//CV_EXPORTS void findContours(InputArray image, OutputArrayOfArrays contours,
		//	int mode, int method, Point offset = Point());

		//cv::Mat m_image_find = m_image_source;   // ת���õ���ͼ
		cv::Mat grayImage;
		cv::Mat out_Canny;

		cv::cvtColor(m_image_source, grayImage, cv::COLOR_BGR2GRAY);

		cv::Mat hsvImage;
		cv::cvtColor(m_image_source, hsvImage, cv::COLOR_BGR2HSV);

		int hueValue = 0; // red color
		int hueRange = 15; // how much difference from the desired color we want to include to the result If you increase this value, for example a red color would detect some orange values, too.

		//int minSaturation = 50; // I'm not sure which value is good here...
		//int minValue = 50; // not sure whether 50 is a good min value here...

		cv::Mat hueMask;
		cv::inRange(hsvImage, hueValue - hueRange, hueValue + hueRange, hueMask);


		int min_Thresh = 240;
		int max_Thresh = 255;
		Canny(grayImage, out_Canny, min_Thresh, max_Thresh * 2, 3);

		//cv::imshow("source", m_image_source);
		cv::imshow("canny", out_Canny);
		//cv::waitKey();

		//CV_RETR_EXTERNALֻ�������Χ��������������Χ�����ڵ���Χ����������
		std::vector<std::vector<cv::Point>> g_vContours;
		std::vector<cv::Vec4i> g_vHierarchy;

		cv::RNG G_RNG(1234);

		cv::findContours(out_Canny, g_vContours, g_vHierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

		cv::Mat Drawing = cv::Mat::zeros(out_Canny.size(), CV_8UC3);

		for (int i = 0; i < g_vContours.size(); i++) {
			cv::Scalar color = cv::Scalar(G_RNG.uniform(0, 255), G_RNG.uniform(0, 255), G_RNG.uniform(0, 255));
			cv::drawContours(Drawing, g_vContours, i, color, 2, 8, g_vHierarchy, 0);
		}

		cv::imshow("gray", grayImage);

		cv::imshow("drawing", Drawing);

		//cv::imshow("contours", grayImage);
		cv::waitKey();

		return true;
	}

	bool Capture::searchChess(HWND hw, QString findName, QVector<cv::Point>& res, SearchWhichCap sWhich, bool isCap, float threshold, bool isShow)
	{

		//searchCountours(hw, findName, mainName, isCap);

		if (isCap) {
			if (this->captureOne(hw) == false) {
				qWarning("searchImage 1 %s �����ˣ�", findName);
				return false;
			}
		}

		cv::Mat image_template_main;
		//cv::Mat image_template2;

		cv::Mat image_template_chess; // �����п����м���
		bool bIsInHash = false;
		//cv::Mat image_source;
		//int tmpSize = 1;

		cv::Mat image_template_chess_pre;  // �����п����м���
		cv::Mat image_template_chess_gold; // �����п����м���

		try {

			//QString hashName = "aff"; // getHashName(findName);

			if (this->m_MatHash.contains(findName)) {              // �����ڻ�����
				if (sWhich == SearchWhichCap::eMain) {
					image_template_main = this->m_MatHash.value(findName);
				}
				else {
					image_template_chess = this->m_MatHash.value(findName);
				}
				bIsInHash = true;
			}
			else {

				if (sWhich == SearchWhichCap::eMain) {

					QString fFile = this->getFindPath() + findName;
					image_template_main = cv::imread(fFile.toStdString());   // ģ��ͼ

					// Ҫ��Ҫ����
					//if (this->m_scaleX != 1.0f) {
					//	cv::resize(image_template_main, image_template_main, cv::Size(), this->m_scaleX, this->m_scaleY);
					//}
					this->m_MatHash.insert(findName, image_template_main);
				}
				else {



					// ������ٲ�һ�£���Ϊ�б߿����		

					/*int w = image_template_scaled.rows;
					int h = image_template_scaled.cols;
					float s = this->m_chessClip;
					cv::Rect crect(w * s, h * s, w * (1 - 2 * s), h * (1 - 2 * s));
					image_template_scaled = image_template_scaled(crect);*/


					QString fFile = this->getFindPath() + findName;
					cv::Mat image_template = cv::imread(fFile.toStdString());   // ģ��ͼ

					// Ҫ�����HSV mask ��ʽ

					cv::Mat imgHSV;
					cv::cvtColor(image_template, imgHSV, cv::COLOR_BGR2HSV);


					if (sWhich == SearchWhichCap::eBlack) {
						cv::inRange(imgHSV, cv::Scalar(iLowHblack, iLowSblack, iLowVblack), cv::Scalar(iHighHblack, iHighSblack, iHighVblack), image_template_chess_pre); //Threshold the image
					}
					else {
						cv::inRange(imgHSV, cv::Scalar(iLowHred, iLowSred, iLowVred), cv::Scalar(iHighHred, iHighSred, iHighVred), image_template_chess_pre); //Threshold the image
					}
					// this->m_MatHash.insert(findName, image_template2); ������ȷ�Ĳ���
					//tmpSize = image_template2.cols * image_template2.rows;

					// ������
					fFile = this->getFindPath() + "/0/golden/" + findName;
					image_template = cv::imread(fFile.toStdString());   // ģ��ͼ
					cv::cvtColor(image_template, imgHSV, cv::COLOR_BGR2HSV);

					if (sWhich == SearchWhichCap::eBlack) {
						cv::inRange(imgHSV, cv::Scalar(iLowHblack, iLowSblack, iLowVblack), cv::Scalar(iHighHblack, iHighSblack, iHighVblack), image_template_chess_gold); //Threshold the image
					}
					else {
						cv::inRange(imgHSV, cv::Scalar(iLowHred, iLowSred, iLowVred), cv::Scalar(iHighHred, iHighSred, iHighVred), image_template_chess_gold); //Threshold the image
					}

				}
			}
		}
		catch (...) {
			//qWarning("searchImage 2 %s �����ˣ�", findName);
			return false;
		}

		
		//cv::imshow("templ", m_image_black);
		//cv::imshow("img", m_image_source);
		//cv::imshow("matched", image_template2);
		//cv::waitKey();
		

//#define TM_CCOEFF
#ifdef TM_CCOEFF
		//cv::imshow("img", image_source);
		//cv::imshow("templ", image_template_scaled);
		//cv::waitKey();

//ͼ��ģ��ƥ��
//һ����ԣ�Դͼ����ģ��ͼ��patch�ߴ�һ���Ļ�������ֱ��ʹ��������ܵ�ͼ�����ƶȲ����ķ�����
//���Դͼ����ģ��ͼ��ߴ粻һ����ͨ����Ҫ���л���ƥ�䴰�ڣ�ɨ�������ͼ������õ�ƥ��patch��
//��OpenCV�ж�Ӧ�ĺ���Ϊ��matchTemplate()������������������ͼ���л�������Ѱ�Ҹ���λ����ģ��ͼ��patch�����ƶȡ����ƶȵ����۱�׼��ƥ�䷽�����У�
//CV_TM_SQDIFF ƽ����ƥ�䷨�����ƶ�Խ�ߣ�ֵԽС����
//CV_TM_CCORR ���ƥ�䷨�����ó˷����������ƶ�Խ��ֵԽ�󣩣�
//CV_TM_CCOEFF ���ϵ��ƥ�䷨��1��ʾ��õ�ƥ�䣬-1��ʾ����ƥ�䣩��
//ͨ��,���ŴӼ򵥵Ĳ���(ƽ����)�������ӵĲ���(���ϵ��),���ǿɻ��Խ��Խ׼ȷ��ƥ��(ͬʱҲ��ζ��Խ��Խ��ļ������). /
//��õİ취�Ƕ�������Щ���ö���һЩ����ʵ��,�Ա�Ϊ�Լ���Ӧ��ѡ��ͬʱ����ٶȺ;��ȵ���ѷ���.//

		cv::Mat image_matched;
		try {
			// CV_TM_SQDIFF
			//cv::matchTemplate(m_image_source, image_template_scaled, image_matched, cv::TM_SQDIFF);
			cv::matchTemplate(m_image_source, image_template_scaled, image_matched, cv::TM_CCOEFF_NORMED);
		}
		catch (...) {
			qWarning("searchImage 3 %s �����ˣ�", findName);
			return false;
		}

#if 0
		cv::imshow("templ", image_template_scaled);
		cv::imshow("img", m_image_source);
		cv::imshow("matched", image_matched);
		cv::waitKey();
#endif


		if (threshold == 1.0f) {
			threshold = this->m_precision;
		}
		res.clear();  // �������

		bool Isfind = false;

		while (true) {
			cv::Point minLoc, maxLoc;
			double minVal, maxVal;

			//Ѱ�����ƥ��λ��
			cv::minMaxLoc(image_matched, &minVal, &maxVal, &minLoc, &maxLoc);

			if (maxVal > threshold) {

				Isfind = true;

				cv::Point chessP;

				chessP.x = maxLoc.x + image_template_scaled.cols / 2;
				chessP.y = maxLoc.y + image_template_scaled.rows / 2;

				if (isShow) {
					//cv::Mat image_color;
					//cv::cvtColor(image_source, image_color, cv::CV_BGR2GRAY);
					cv::circle(m_image_source,
						chessP, //cv::Point(imgX, imgY),
						image_template_scaled.rows,
						cv::Scalar(0, 255, 255),
						2,
						8,
						0);

					cv::imshow("target", m_image_source);
					cv::imshow("templ", image_template_scaled);
				}

				res.append(chessP);

				//cv::imshow("m1", image_matched);
				cv::floodFill(image_matched, maxLoc, cv::Scalar(0));
				//cv::imshow("m2", image_matched);
			}
			else {
				break;
			}

		}

#else
		//cv::imshow("img", image_source);
		//cv::imshow("templ", image_template_scaled);
		//cv::waitKey();

//ͼ��ģ��ƥ��
//һ����ԣ�Դͼ����ģ��ͼ��patch�ߴ�һ���Ļ�������ֱ��ʹ��������ܵ�ͼ�����ƶȲ����ķ�����
//���Դͼ����ģ��ͼ��ߴ粻һ����ͨ����Ҫ���л���ƥ�䴰�ڣ�ɨ�������ͼ������õ�ƥ��patch��
//��OpenCV�ж�Ӧ�ĺ���Ϊ��matchTemplate()������������������ͼ���л�������Ѱ�Ҹ���λ����ģ��ͼ��patch�����ƶȡ����ƶȵ����۱�׼��ƥ�䷽�����У�
//CV_TM_SQDIFF ƽ����ƥ�䷨�����ƶ�Խ�ߣ�ֵԽС����
//CV_TM_CCORR ���ƥ�䷨�����ó˷����������ƶ�Խ��ֵԽ�󣩣�
//CV_TM_CCOEFF ���ϵ��ƥ�䷨��1��ʾ��õ�ƥ�䣬-1��ʾ����ƥ�䣩��
//ͨ��,���ŴӼ򵥵Ĳ���(ƽ����)�������ӵĲ���(���ϵ��),���ǿɻ��Խ��Խ׼ȷ��ƥ��(ͬʱҲ��ζ��Խ��Խ��ļ������). /
//��õİ취�Ƕ�������Щ���ö���һЩ����ʵ��,�Ա�Ϊ�Լ���Ӧ��ѡ��ͬʱ����ٶȺ;��ȵ���ѷ���.//

		for (int i = 0; i < 2; i++) {

			cv::Mat image_matched;
			try {
				if (sWhich == SearchWhichCap::eMain) {

					if (i == 1) {
						return false;
					}

					// CV_TM_SQDIFF
					cv::matchTemplate(m_image_source, image_template_main, image_matched, cv::TM_SQDIFF_NORMED); // cv::TM_CCORR); // cv::TM_SQDIFF);

					
				}
				else if (bIsInHash == true) {
					if (i == 1) {
						return false;
					}
					if (sWhich == SearchWhichCap::eBlack) {
						cv::matchTemplate(m_image_black, image_template_chess, image_matched, cv::TM_SQDIFF_NORMED); // cv::TM_CCORR); // cv::TM_SQDIFF);
					}
					else {
						cv::matchTemplate(m_image_red, image_template_chess, image_matched, cv::TM_SQDIFF_NORMED); // cv::TM_CCORR); // cv::TM_SQDIFF);
					}
				}
				else {
					if (sWhich == SearchWhichCap::eBlack) {
						if (i == 0) {
							cv::matchTemplate(m_image_black, image_template_chess_pre, image_matched, cv::TM_SQDIFF_NORMED); // cv::TM_CCORR); // cv::TM_SQDIFF);
						}
						else {
							cv::matchTemplate(m_image_black, image_template_chess_gold, image_matched, cv::TM_SQDIFF_NORMED); // cv::TM_CCORR); // cv::TM_SQDIFF);
						}
					}
					else {
						if (i == 0) {
							cv::matchTemplate(m_image_red, image_template_chess_pre, image_matched, cv::TM_SQDIFF_NORMED); // cv::TM_CCORR); // cv::TM_SQDIFF);
						}
						else {
							cv::matchTemplate(m_image_red, image_template_chess_gold, image_matched, cv::TM_SQDIFF_NORMED); // cv::TM_CCORR); // cv::TM_SQDIFF);
						}
					}
				}
				//cv::matchTemplate(m_image_source, image_template_scaled, image_matched, cv::TM_CCOEFF_NORMED);  image_template_chess
			}
			catch (...) {
				qWarning("searchImage 3 %s �����ˣ�", findName);
				return false;
			}

#if 0
			cv::imshow("m_image_source", m_image_source);
			//cv::imshow("templ", image_template2);
			cv::imshow("m_image_black", m_image_red);
			cv::imshow("matched", image_matched);
			cv::waitKey();
#endif
			try {

				if (threshold == 1.0f) {
					threshold = this->m_precision_chess;
				}
				res.clear();  // �������

				bool Isfind = false;

				while (true) {
					cv::Point minLoc, maxLoc;
					double minVal, maxVal;

					//Ѱ�����ƥ��λ��
					cv::minMaxLoc(image_matched, &minVal, &maxVal, &minLoc, &maxLoc);


					//double matchVal = minVal / tmpSize;  // ȥ��ģ���С��ƥ�侫�ȵ�Ӱ��
					//threshold = 0.90;
					double matchThres = maxVal * (1 - threshold);

					if (minVal < matchThres) {

						Isfind = true;

						cv::Point chessP;

						if (sWhich == SearchWhichCap::eMain) {
							chessP.x = minLoc.x + image_template_main.cols / 2;
							chessP.y = minLoc.y + image_template_main.rows / 2;
						}
						else {							
							if (bIsInHash == true) {
								chessP.x = minLoc.x + image_template_chess.cols / 2;
								chessP.y = minLoc.y + image_template_chess.rows / 2;
							}
							else if (i == 0) {
								chessP.x = minLoc.x + image_template_chess_pre.cols / 2;
								chessP.y = minLoc.y + image_template_chess_pre.rows / 2;
							}
							else {
								chessP.x = minLoc.x + image_template_chess_gold.cols / 2;
								chessP.y = minLoc.y + image_template_chess_gold.rows / 2;
							}
						}

						if (isShow) {
							//cv::Mat image_color;
							//cv::cvtColor(image_source, image_color, cv::CV_BGR2GRAY);
							cv::circle(m_image_source,
								chessP, //cv::Point(imgX, imgY),
								40,
								cv::Scalar(0, 255, 255),
								2,
								8,
								0);

							cv::imshow("target", m_image_source);
							//cv::imshow("templ", image_template);
						}

						res.append(chessP);

						//cv::Rect rc; // = cv::Rect(minLoc, cv::Size(10, 10));
						//cv::imshow("m1", image_matched);
						//cv::floodFill(image_matched, minLoc, cv::Scalar(maxVal), &rc, cv::Scalar(minVal * 0.5), cv::Scalar(minVal * 0.5));



						cv::circle(image_matched, minLoc, 10, cv::Scalar(maxVal), -1);

						//cv::imshow("m2", image_matched);
						//cv::waitKey();
					}
					else {
						break;
					}

				}

#endif
				//cv::waitKey();

				if (bIsInHash == false) {
					if (i == 0) {
						this->m_MatHash.insert(findName, image_template_chess_pre);
					}
					else {
						this->m_MatHash.insert(findName, image_template_chess_gold);
					}
				}

				return Isfind;
			}
			catch (...) {
				//qWarning("searchImage 5 %s �����ˣ�", findName);
				return false;
			}
		}

		return false;
		
	}

	cv::Mat Capture::QImage_to_cvMat(const QImage& image, bool inCloneImageData)
	{
		switch (image.format())
		{
			// ��ӦMat��8λ4ͨ��ͼ��
		case QImage::Format_RGB32:
		{
			cv::Mat mat(image.height(), image.width(), CV_8UC4, const_cast<uchar*>(image.bits()), image.bytesPerLine());
			return (inCloneImageData ? mat.clone() : mat);
		}

		// ��ӦMat��8λ3ͨ��ͼ��
		case QImage::Format_RGB888:
		{
			if (!inCloneImageData) {
				//qWarning() << "ASM::QImageToCvMat() - Conversion requires cloning since we use a temporary QImage";
			}

			//rgbSwapped�������ڷ�תRBͨ����������һ����QImage��������ı�ԭ��ͼ��
			QImage swapped = image.rgbSwapped();
			return cv::Mat(swapped.height(), swapped.width(), CV_8UC3, const_cast<uchar*>(swapped.bits()), swapped.bytesPerLine()).clone();
		}

		// 8λ��ͨ��
		case QImage::Format_Indexed8:
		{
			cv::Mat  mat(image.height(), image.width(), CV_8UC1, const_cast<uchar*>(image.bits()), image.bytesPerLine());

			return (inCloneImageData ? mat.clone() : mat);
		}

		default:

			//qDebug() << "Image format is not supported: depth=%d and %d format\n", image.depth(), image.format();
			break;
		}

		return cv::Mat();

		// https ://blog.csdn.net/wingWC/article/details/78724435 

	}

	// ��ͼ����ֳɺ�ڶ���
	bool Capture::CaptureOneNotry(HWND hw, int sleepTimeMS, bool isHSV)
	{

		try {

			QThread::msleep(sleepTimeMS);

			QScreen* screen = QGuiApplication::primaryScreen();

			this->m_capPixmap = screen->grabWindow(WId(hw));

			//fname = "tmp2.png";
			//if (fname != nullptr) {
			//	this->m_capPixmap.save(path + fname, "PNG");
			//}

			//QString cname = this->get_window_class(hw);

			// ת���� openCV ��ʽ
			/*
			cv::Mat mat = QImage_to_cvMat(this->m_capPixmap.toImage(), false);
			m_image_source = cv::Mat(mat.rows, mat.cols, CV_8UC3);
			int from_to[] = { 0,0,  1,1,  2,2 };
			cv::mixChannels(&mat, 1, &m_image_source, 1, from_to, 3);
			*/			

			if (m_isAutoClick) {
				m_image_source_all = QPixmapToCvMat(this->m_capPixmap, true);
				m_image_source = m_image_source_all;
			}
			else {
				this->m_image_source = QPixmapToCvMat(this->m_capPixmap, true);
			}

			if (this->m_Ready_LXset) {  // ��ǰ��������ϢOK��

				//this->m_LxInfo.m_dx

				cv::Rect crect(0, 0, m_LxInfo.offx + m_LxInfo.m_dx * 8.8, m_LxInfo.offy + m_LxInfo.m_dx * 9.8);			

				m_image_source = m_image_source(crect);

				//cv::imshow("templ", m_image_source);
				//cv::waitKey();
			}

			// ��ֺ�ڶ���ͼ
			//cv::Mat m_image_red;      // �췽����
			//cv::Mat m_image_black;    // �ڷ�����
			if (isHSV) {
				cv::Mat imgHSV;
				cv::cvtColor(m_image_source, imgHSV, cv::COLOR_BGR2HSV);

				cv::inRange(imgHSV, cv::Scalar(iLowHred, iLowSred, iLowVred), cv::Scalar(iHighHred, iHighSred, iHighVred), m_image_red); //Threshold the image
				cv::inRange(imgHSV, cv::Scalar(iLowHblack, iLowSblack, iLowVblack), cv::Scalar(iHighHblack, iHighSblack, iHighVblack), m_image_black); //Threshold the image
			}

			//cv::Mat hsv_channels_red[3];
			//cv::Mat hsv_red;
			//cv::inRange(imgHSV, cv::Scalar(iLowHred, iLowSred, iLowVred), cv::Scalar(iHighHred, iHighSred, iHighVred), hsv_red); //Threshold the image
			//cv::split(hsv_red, hsv_channels_red);
			//m_image_red = hsv_channels_red[2];


			//cv::split(hsv_red, hsv_channels_red);
			//m_image_red = hsv_channels_red[2];

			//cv::inRange(imgHSV, cv::Scalar(iLowHblack, iLowSblack, iLowVblack), cv::Scalar(iHighHblack, iHighSblack, iHighVblack), m_image_black); //Threshold the image

			/*
			cv::imshow("m_image_red", m_image_red); //show the thresholded image
			cv::imshow("m_image_black", m_image_black); //show the original image
			cv::waitKey();
			*/
					
			
			// isDisplay	
			//if (this->displayCapture) {
			//	GOB::DisplayImage(this->capPixmap, this->isZoom);
			//}

		}
		catch (...) {
			//qWarning("��ͼ�����ˣ� %s", path + fname);
			return false;
		}
		return  true;
	}

	bool Capture::getChessboardHwnd(bool onlyBChe)
	{
		HWND hw = first_window();
		while (hw != nullptr)
		{
			//if (hw == (HWND)0x00791730) {
			//	//this->m_hwnd = hw;

			//	//this->m_connectedBoard_OK = true;

			//	//return true;
			//	int a = 0;
			//}
			//else {
			//	hw = next_window(hw);
			//	continue;
			//}

			
			if (isChessBoardWindow(hw, &m_LxBoard[0], onlyBChe)) {
				//if (isMoveOrg) {
				//	RECT  rect;
				//	GetClientRect(hw, &rect);
				//	::MoveWindow(hw, 0, 0, rect.right, rect.top, false);
				//}


				

				this->m_hwnd = hw;

				this->m_connectedBoard_OK = true;

				return true;
			}

			hw = next_window(hw);
		}
		return false;
	}

	bool Capture::check_window_valid(HWND window, int minW, int minH, window_search_mode mode)
	{
		DWORD styles, ex_styles;
		RECT  rect;

		if (/*!IsWindowVisible(window) ||*/
			(mode == EXCLUDE_MINIMIZED && IsIconic(window)))
			return false;

		GetClientRect(window, &rect);
		styles = DWORD(GetWindowLongPtr(window, GWL_STYLE));
		ex_styles = DWORD(GetWindowLongPtr(window, GWL_EXSTYLE));

		if (ex_styles & WS_EX_TOOLWINDOW)
			return false;
		if (styles & WS_CHILD)
			return false;
		//if (mode == EXCLUDE_MINIMIZED && (rect.bottom == 0 || rect.right == 0))
		//if (mode == EXCLUDE_MINIMIZED && (rect.bottom <= minH || rect.right <= minW))

		if (rect.bottom <= minH || rect.right <= minW)
			return false;

		return true;
	}

	inline HWND Capture::next_window(HWND window, window_search_mode mode)
	{
		while (true) {
			window = GetNextWindow(window, GW_HWNDNEXT);
			if (!window || check_window_valid(window, validWinW, validWinH, mode))
				break;
		}
		return window;
	}

	inline HWND Capture::first_window(window_search_mode mode)
	{
		HWND window = GetWindow(GetDesktopWindow(), GW_CHILD);
		if (!check_window_valid(window, validWinW, validWinH, mode))
			window = next_window(window, mode);
		return window;
	}

	QString Capture::get_window_title(HWND hwnd)
	{
		QString retStr;
		wchar_t temp[512];
		int len;

		len = GetWindowTextLengthW(hwnd);
		if (!len)
			return retStr;

		//temp = reinterpret_cast<wchar_t *>(malloc(sizeof(wchar_t) * (quint32(len)+1)));
		if (GetWindowTextW(hwnd, temp, len + 1))
		{
			retStr = QString::fromWCharArray(temp);
		}
		//free(temp);
		return retStr;
	}

	QString Capture::get_window_class(HWND hwnd)
	{
		QString retStr;
		wchar_t temp[256];

		temp[0] = 0;
		if (GetClassNameW(hwnd, temp, sizeof(temp) / sizeof(wchar_t)))
		{
			retStr = QString::fromWCharArray(temp);
		}
		return retStr;
	}

	bool Capture::SearchOnChessList(HWND hwnd, QString chess, QVector<cv::Point>& res, SearchWhichCap sWhich, bool isCap)
	{
		QString chessFile = m_LxInfo.m_PieceCatlog + "/" + chess;  // �ڳ�

		return searchChess(hwnd, chessFile, res, sWhich, isCap);
	}

	bool Capture::isChessBoardWindow(HWND hwnd, stLxBoard* pieceList, bool onlyBche)
	{
		// +cname	Qt5QWindowIcon	QString
		QString wc = this->get_window_class(hwnd);
		if (wc == "Qt5QWindowIcon") {   // ȥ���Լ��Ĵ���
			return false;
		}

		//QString title = this->get_window_title(hwnd);
		if (wc == this->m_LxInfo.m_class) {

			// ���ҵ�32��Բ				
			if (onlyBche) {
				//QString rk = m_LxInfo.m_PieceCatlog + "/br.png";  // �ڳ�
				//if (!searchImage(hwnd, rk, pieceList->BCheList, nullptr, true)) {

				if (!SearchOnChessList(hwnd, "br.png", pieceList->BCheList, SearchWhichCap::eBlack,true)) {
					return false;
				}
				//if (pieceList->BCheList.count() >= 1) {
				//	int a = 0;
				//}
				if (pieceList->BCheList.count() == 2) {
					return true;
				}
			}
			else {  // ���ж�˫���н�

				if (!SearchOnChessList(hwnd, "bk.png", pieceList->BKingList, SearchWhichCap::eBlack, true)) {
					return false;
				}

				if (!SearchOnChessList(hwnd, "rk.png", pieceList->RKingList,SearchWhichCap::eRed)) {
					return false;
				}

				if (pieceList->RKingList.count() >= 1 && pieceList->BKingList.count() >= 1) {
					return true;  // ���Ӱѽ���ʱ�䷽�ˣ���
				}
			}
		}

		return false;
	}

	bool Capture::isFindAutoWin()
	{
		HWND hw = first_window();
		while (hw != nullptr)
		{
			//if (hw == (HWND)0x00791730) {
			//	//this->m_hwnd = hw;

			//	//this->m_connectedBoard_OK = true;

			//	//return true;
			//	int a = 0;
			//}
			//else {
			//	hw = next_window(hw);
			//	continue;
			//}

			QString wc = this->get_window_class(hw);
			//if (wc == "Qt5QWindowIcon") {   // ȥ���Լ��Ĵ���
			//	continue;
			//}

			//QString title = this->get_window_title(hwnd);
			if (wc == this->m_LxInfo.m_class) {

				this->m_hwnd = hw;		

				return true;
			}

			hw = next_window(hw);
		}
		return false;
	}

	QString Capture::getPicturePath()
	{
		QString runPath = QCoreApplication::applicationDirPath();
		QString str = runPath + "/image/";
		return str;
	}

	QString Capture::getFindPath()
	{
		QString runPath = QCoreApplication::applicationDirPath();
		QString str = runPath + "/image/findchess/";
		return str;
	}
	   
};  // namespace Chess 
