#include "linkboard.h"

#include <QThread>
#include <QScreen>
#include <QPixmap>
#include <QRect>
#include <QDir>
#include <QGuiApplication>
#include <QMessagebox>
#include <board/boardfactory.h>
#include <QMutex>


#include "mainwindow.h"
#include "capture.h"

namespace Chess {


	QMutex LinkBoard::mutex;
	volatile bool LinkBoard::m_MayNewGame;

LinkBoard::LinkBoard(MainWindow* pMain, Capture* pCap, QString catName, bool isAuto)
	:m_pMain(pMain),
	m_pCap(pCap),
	m_catName(catName),
	m_isAutoClick(isAuto)
{
	this->initBoard();   
}

void LinkBoard::initBoard()
{
#if 0
	if (m_catName == "天天象棋") {        // 天天象棋
		m_precision_chess = 0.57f; // was 0.52
		m_precision_auto = 0.98f;
		m_UseAdb = false;
		m_sleepTimeMs = 50;
		m_scaleX = 1.0f;
		m_scaleY = 1.0f;

		//m_Ready_LXset = false;
		////m_chessWinOK = false;

		//m_chessClip = 0.25f;

		//this->m_board = BoardFactory::create("standard");
		//this->m_board_second = BoardFactory::create("standard");
		//m_board->reset();
		//m_board_second->reset();


		//this->m_LxInfo.m_LX_name = "兵河五四小棋盘";
		m_ParentKeyword = "中国象棋2017";           // 父窗口关键词
		m_Parentclass = "QQChess";             // 父窗口类
		m_titleKeyword = ""; // "BHGUI(test) - 新棋局";
		m_class = ""; // "Afx:00400000:b:00010003:00000006:0A1D05FB";

		//this->m_LxInfo.offx = 29.0f;
		//this->m_LxInfo.offy = 138.0f;
		//this->m_LxInfo.m_dx = 28.0f;
		//this->m_LxInfo.m_dy = 28.0f;

		m_offx_che = 73.0f;
		m_offy_che = 175.0f;
		m_dx = 68.0f;
		m_dy = 68.0f;

		this->m_Ready_LXset = false;

		//this->m_LxInfo.m_PieceCatlog = "0";
		//this->m_connectedBoard_OK = false;  // 

		m_side = Side::NoSide;

		m_flip = false;

		m_iLowHred = 0;
		m_iHighHred = 10;

		m_iLowSred = 77;
		m_iHighSred = 255;

		m_iLowVred = 95;
		m_iHighVred = 255;

		m_iLowHblack = 0;
		m_iHighHblack = 51;

		m_iLowSblack = 0;
		m_iHighSblack = 90;

		m_iLowVblack = 0;
		m_iHighVblack = 140;
	}
	else if (m_catName == "王者象棋") {   // 王者象棋
		m_precision_chess = 0.42f; // was 0.52
		m_precision_auto = 0.98f;
		m_UseAdb = false;
		m_sleepTimeMs = 50;
		m_scaleX = 1.0f;
		m_scaleY = 1.0f;

		//m_Ready_LXset = false;
		//m_chessWinOK = false;

		//m_chessClip = 0.25f;

		//this->m_board = BoardFactory::create("standard");
		//this->m_board_second = BoardFactory::create("standard");
		//m_board->reset();
		//m_board_second->reset();

		m_ParentKeyword = "王者象棋";         // 父窗口关键词
		m_Parentclass = "#32770";             // 父窗口类

		//this->m_LxInfo.m_LX_name = "兵河五四小棋盘";
		m_titleKeyword = "Chrome Legacy Window"; // "BHGUI(test) - 新棋局";
		m_class = "Chrome_RenderWidgetHostHWND"; // "Afx:00400000:b:00010003:00000006:0A1D05FB";

		//this->m_LxInfo.offx = 29.0f;
		//this->m_LxInfo.offy = 138.0f;
		//this->m_LxInfo.m_dx = 28.0f;
		//this->m_LxInfo.m_dy = 28.0f;

		m_offx_che = 43.0f;
		m_offy_che = 148.0f;
		m_dx = 50.5f;
		m_dy = 50.5f;
	
		this->m_Ready_LXset = false;

		//this->m_LxInfo.m_PieceCatlog = "1";
		//this->m_connectedBoard_OK = false;  // 

		m_side = Side::NoSide;

		m_flip = false;


		m_iLowHred = 0;
		m_iHighHred = 11;

		m_iLowSred = 85;
		m_iHighSred = 255;

		m_iLowVred = 95;
		m_iHighVred = 255;

		m_iLowHblack = 0;
		m_iHighHblack = 179;

		m_iLowSblack = 0;
		m_iHighSblack = 255;

		m_iLowVblack = 0;
		m_iHighVblack = 118;
	}
	this->saveToCatlog();

#endif		

	this->readFromCatlog();
	CalImageRect();
	m_Ready_LXset = false;
	m_side = Side::NoSide;
	m_flip = false;

}

void LinkBoard::run()
{
	if (m_isAutoClick) {
		this->runAutoClip();
	}
	else {
		this->runAutoChess();  
	}
}

bool LinkBoard::readFromCatlog(QString cat)
{
	
	if (cat == nullptr) {
		cat = m_catName;
	}
	try {
		QString fileName = QCoreApplication::applicationDirPath() + "/image/linkboard/" + cat + "/linkinfo.data";
		QFile file(fileName);
		if (!file.open(QIODevice::ReadOnly)) {
			return false;
		}
		QDataStream output(&file);
		output >> (*this);
		file.close();
	}
	catch (...) {
		return false;
	}
	return true;
}

bool LinkBoard::saveToCatlog(QString cat)
{
	if (cat == nullptr) {
		cat = m_catName;
	}
	try {
		QString fileName = QCoreApplication::applicationDirPath() + "/image/linkboard/" + cat + "/linkinfo.data";
		QFile file(fileName);
		if (!file.open(QIODevice::WriteOnly)) {
			return false;
		}
		QDataStream input(&file);
		input << (*this);
		file.close();
	}
	catch (...) {
		return false;
	}
	return true;
}

void LinkBoard::runAutoChess()
{
	
	this->bMustStop = false;
	bool bWeMustSendInitFen = false;
	this->m_hwnd = nullptr; 

	this->m_Ready_LXset = true;      // 连线信息OK

	if (this->m_Ready_LXset == false) {
		
		this->GetLxInfo(this->m_catName, false);   // 不保存棋子

		//this->GetLxInfo(this->m_catName, true);  // 保存棋子

		if (this->m_Ready_LXset == false) {
			m_pCap->SendMessageToMain("出错啦 01", "连线方案还没有准备好！");
			return;
		}
	}

	if (this->m_hwnd == nullptr) {  // 得到棋盘句柄
		if (!getChessboardHwnd(false, false)) {
			m_pCap->SendMessageToMain("出错啦 02", "连线方案还没有准备好！");
			return;
		}
	}

	QString MoveSendingFen;
	Chess::GenericMove MoveSendingMove;
	this->m_LxBoard[0].fen = "none";

	QElapsedTimer timeRun;  // 超时定时器
	timeRun.start();
	quint64 StartTime = timeRun.elapsed();

	while (true) {
		if (bMustStop) return;

		//try {
		//	cv::imshow("source", this->m_image_source);
		//	//cv::imshow("all", this->m_image_source_all);
		//	if (!m_isAutoClick) {
		//		cv::imshow("red", this->m_imgage_SHV[0]);
		//		cv::imshow("black", this->m_imgage_SHV[1]);
		//	}
		//	cv::waitKey();
		//}
		//catch (...) {

		//}

		//cv::imshow("source", this->m_image_source);
		//cv::waitKey();

		if (GetLxBoardChess(1)) {   // 读出第二个棋盘''

			QString fen = this->m_LxBoard[1].fen;
			if (fen == "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR b - - 0 1") { // 是初始局面，这个是对方在走第一步
				wait(m_sleepTimeMs);
				continue;
			}

			if (this->m_LxBoard[0].fen != fen) {
				if (fen.contains("rnbakabnr/9/1c5c1/p1p1p1p1p", Qt::CaseSensitive)  // 黑棋没有动过
					&& (fen.contains("b -", Qt::CaseSensitive))) {
					bWeMustSendInitFen = true;
				}

				if (fen == "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1") {
					bWeMustSendInitFen = true;
				}
			}
			if (this->m_LxBoard[0].fen == "none") {  // 才启动					
				bWeMustSendInitFen = true;
			}

			// 发送初始fen				
			if (bWeMustSendInitFen) {
				QString fen = this->m_LxBoard[1].fen;
				this->m_pCap->SendFenToMain(fen);

				StartTime = timeRun.elapsed();   // 发送棋盘后重置一下棋局开始时间

				//this->m_LxBoard[0].b90 = this->m_LxBoard[0].b90;
				int size = sizeof(this->m_LxBoard[0].b90);
				memcpy(this->m_LxBoard[0].b90, this->m_LxBoard[1].b90, size);
				this->m_LxBoard[0].fen = this->m_LxBoard[1].fen;

				bWeMustSendInitFen = false;
				for (int i = 0; i < 10; i++) {  // 延时2秒，等待主界面启动引擎
					wait(100);
					if (bMustStop) return;
				}
			}
			else {
				if (LinkBoard::m_MayNewGame) {
					if (fen == "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1") {
						if (((timeRun.elapsed() - StartTime) / 1000.0) > 25.0) {
							bWeMustSendInitFen = true;
						}
					}
				}
			}


			Chess::GenericMove m;
			if (this->Board2Move(m)) {
				//SendMoveToMain(m);
				MoveSendingFen = this->m_LxBoard[1].fen;
				MoveSendingMove = m;

				this->m_LxBoard[0].fen = MoveSendingFen;      // 保存走子后的fen	

				StartTime = timeRun.elapsed();   // 发送棋盘后重置一下棋局开始时间
			}

			if (this->m_LxBoard[1].fen == MoveSendingFen) {   // 棋盘没有改动
				if (MoveSendingMove.isNull() == false) {
					this->m_pCap->SendMoveToMain(m);
				}
			}

		}
		wait(m_sleepTimeMs);

		if (LinkBoard::m_MayNewGame) {
			LinkBoard::mutex.lock();
			LinkBoard::m_MayNewGame = false;;
			LinkBoard::mutex.unlock();
			this->m_MatHash.clear(); // 清空一下
		}
	}
}

void LinkBoard::runAutoClip()
{
	bMustStop = false;
	this->m_Ready_LXset = true;

	//if (!isSolutionReady()) {
	//	this->GetLxInfo("0");
	//	if (!isSolutionReady()) {
	//		SendMessageToMain("出错啦", "连线方案还没有准备好！");
	//		return;
	//	}
	//}

	if (getChessboardHwnd(false,false) == false) {
		m_pCap->SendMessageToMain("出错啦 20", "没有找到象棋界面！");
		return;
	}

	// 点击所有自动目录下的图
	QStringList nameFilters;
	nameFilters << "*.png";
	QString runPath = QCoreApplication::applicationDirPath() + "/image/linkboard/" + this->m_catName;

	while (true) {
		if (bMustStop) return;
		bool findNewGame = false;

		// 3. 可能是新开的棋局		
		QString dirpath = runPath + "/auto/";
		QDir dir = QDir(dirpath);
		QStringList files = dir.entryList(nameFilters, QDir::Files | QDir::Readable, QDir::Name);
		while (true) {
			bool isCap = true;   // 第一次要读一下图
			bool isFind = false;
			for (QString file : files) {
				if (this->SearchAndClick(file, isCap, "/auto/")) {  // 这个一直要读
					findNewGame = true;
					isFind = true;
				}
				else {
					isCap = false;
				}
				wait(100);				
				if (bMustStop) return;
			}
			if (!isFind) break;
		}

		// 1. 发现这些图就等待
		dirpath = runPath + "/not/";
		dir = QDir(dirpath);
		files = dir.entryList(nameFilters, QDir::Files | QDir::Readable, QDir::Name);

		while (true) {		
			bool isCap = true;   // 第一次要读一下图
			bool isFind = false;
			for (QString file : files) {
				if (searchImage(file, isCap, "/not/")) {  // 
					wait(100);
					isFind = true;
				}		
				else {
					isCap = false;
				}
				if (bMustStop) return;
			}
			if (!isFind) break;
		}

		// 2. 对局中发现的，对方求和，我方要说话等
		dirpath = runPath + "/gaming/";
		dir = QDir(dirpath);
		files = dir.entryList(nameFilters, QDir::Files | QDir::Readable, QDir::Name);
		while (true) {	
			bool isCap = true;   // 第一次要读一下图
			bool isFind = false;
			for (QString file : files) {
				if (SearchAndClick(file, isCap, "/gaming/")) {  // 
					wait(100);
					isFind = true;
				}		
				else {
					isCap = false;
				}
				if (bMustStop) return;
			}
			if (!isFind) break;
		}

		

		if (findNewGame == true) {
			LinkBoard::mutex.lock();
			LinkBoard::m_MayNewGame = true;  // 通知另外一个线程
			LinkBoard::mutex.unlock();
		}

		wait(1000);
	}
}

void LinkBoard::ProcessBoardMove(const Chess::GenericMove& move)
{
	int fx = move.sourceSquare().file();
	int fy = move.sourceSquare().rank();

	int tx = move.targetSquare().file();
	int ty = move.targetSquare().rank();

	int from = (9 - fy) * 9 + fx;
	int to = (9 - ty) * 9 + tx;
	ChinesePieceType piece = this->m_LxBoard[0].b90[from];
	if (piece <= ChinesePieceType::eBKing && piece >= ChinesePieceType::eBPawn) {
		m_side = Chess::Side::White;
	}
	else if (piece <= ChinesePieceType::eRKing && piece >= ChinesePieceType::eRPawn) {
		m_side = Chess::Side::Black;
	}
	else {
		//this->SendMessageToMain("error", "move error");
		return;
	}

	// 更新一下当前棋盘

	if (m_flip) {
		fx = 8 - fx;
		fy = 9 - fy;
		tx = 8 - tx;
		ty = 9 - ty;
	}


	//cLXinfo* pInfo = &this->m_LxInfo;

	int ffx = m_offx_che + fx * m_dx;
	int ffy = m_offy_che + (9 - fy) * m_dy;

	int ttx = m_offx_che + tx * m_dx;
	int tty = m_offy_che + (9 - ty) * m_dy;

	this->m_LxBoard[0].b90[from] = ChinesePieceType::eNoPice;
	this->m_LxBoard[0].b90[to] = piece;

	winLeftClick(m_hwnd, ffx, ffy);
	//wait(1);
	winLeftClick(m_hwnd, ttx, tty);
}

void LinkBoard::SetCatlogName(QString catName)
{
	this->m_catName = catName;
	this->initBoard();
}

bool LinkBoard::CalImageRect()
{
	
	//if (m_flip == false) {
		this->m_offx_board = m_offx_che - m_dx / 2 - 2;
		this->m_offy_board = m_offy_che - m_dy / 2 - 2;
		if (m_offx_board < 0) m_offx_board = 0;
		if (m_offy_board < 0) m_offy_board = 0;

		int w = m_offx_che + m_dx * 8.7;
		//if (w > m_image_source.cols) {
		//	w = m_image_source.cols;
		//}
		int h = m_offy_che + m_dy * 9.7;
		//if (h > m_image_source.rows) {
		//	h = m_image_source.rows;
		//}

		//cv::Rect crect(ox, oy, w - ox, h - oy);

		this->m_crect = cv::Rect(m_offx_board, m_offy_board, w - m_offx_board, h - m_offy_board);
	//}
	//else {
	//	this->m_offx_board = m_offx_che - m_dx / 2 - 2;
	//	this->m_offy_board = m_offy_che - m_dy / 2 - 2;
	//	if (m_offx_board < 0) m_offx_board = 0;
	//	if (m_offy_board < 0) m_offy_board = 0;

	//	int w = m_offx_che + m_dx * 8.7;
	//	//if (w > m_image_source.cols) {
	//	//	w = m_image_source.cols;
	//	//}
	//	int h = m_offy_che + m_dy * 9.7;

	//	this->m_crect = cv::Rect(m_offx_board, m_offy_board - m_dy * 9.0, w - m_offx_board, h - m_offy_board);
	//}

	//m_offx_che -= m_offx_board;
	//m_offy_che -= m_offy_board;

	return true;
}

bool LinkBoard::GetLxBoardChess(int index)
{
	//if (this->m_Ready_LXset == false)
	//	this->GetLxInfo(this->m_catName);

	if (this->m_Ready_LXset == false) return false;

	//if (this->m_connectedBoard_OK == false) {  // 还没有连接棋盘
	//	if (!getChessboardHwnd()) {
	//		qWarning("找不到棋盘！");
	//		return false;
	//	}
	//}
	stLxBoard* pList = &m_LxBoard[index];

	if (!searchChess(m_hwnd, "bk.png", pList->BKingList, Side::Black, true)) {    // 黑将
		return false;  // 找不到黑将了
	}

	//cv::imshow("ff", this->m_image_source);
	//cv::waitKey();

	if (!searchChess(m_hwnd, "rk.png", pList->RKingList, Side::White)) {    // 红将
		return false; // 找不到红将了
	}
	

	searchChess(m_hwnd, "br.png", pList->BCheList, Side::Black);     // 黑车
	searchChess(m_hwnd, "bn.png", pList->BMaList, Side::Black);      // 黑马
	searchChess(m_hwnd, "bc.png", pList->BPaoList, Side::Black);     // 黑炮
	searchChess(m_hwnd, "ba.png", pList->BShiList, Side::Black);     // 黑士
	searchChess(m_hwnd, "bb.png", pList->BXiangList, Side::Black);   // 黑象
	searchChess(m_hwnd, "bp.png", pList->BPawnList, Side::Black);    // 黑兵


	searchChess(m_hwnd, "rr.png", pList->RCheList, Side::White);     // 红车
	searchChess(m_hwnd, "rn.png", pList->RMaList, Side::White);      // 红马
	searchChess(m_hwnd, "rc.png", pList->RPaoList, Side::White);     // 红炮
	searchChess(m_hwnd, "ra.png", pList->RShiList, Side::White);     // 红士
	searchChess(m_hwnd, "rb.png", pList->RXiangList, Side::White);   // 红象
	searchChess(m_hwnd, "rp.png", pList->RPawnList, Side::White);    // 红兵
	
	return GetFen(pList);
}

bool LinkBoard::GetLxInfo(QString catlog, bool saveChess)
{
	
	if (this->m_Ready_LXset) return true;

	//stLxBoard* pPieceList = &m_LxBoard[0];

	if (!getChessboardHwnd(true, true)) {		
		//this->m_connectedBoard_OK = true;
		return false;
	}
	//this->m_Ready_LXset = true;

	cv::Point Bche0;
	cv::Point Bche1;

	stLxBoard* pPieceList = &m_LxBoard[0];
	if (pPieceList->BCheList[0].x < pPieceList->BCheList[1].x) {
		Bche0 = pPieceList->BCheList[0];
		Bche1 = pPieceList->BCheList[1];
	}
	else {
		Bche1 = pPieceList->BCheList[0];
		Bche0 = pPieceList->BCheList[1];
	}

	m_offx_che = Bche0.x;
	m_offy_che = Bche0.y;

	m_dx = (Bche1.x - Bche0.x) / 8.0f;
	m_dy = m_dx;

	if (m_dx < 10) return false;

	m_class = this->get_window_class(this->m_hwnd);
	m_titleKeyword = this->get_window_title(this->m_hwnd);	

	m_Ready_LXset = true;

	// 如果黑车的Y大于一半
	//int y = this->m_image_source.rows;
	//if (m_offy_che > y / 2) {
	//	this->m_flip = true;
	//}

	// 把棋盘搞小一些啊，
	//this->CalImageRect();

	this->CalImageRect();
	
	if (saveChess) {
		this->SaveAllPiecePicture();
	}	
	
	return true;
}


void LinkBoard::Find_window2(HWND parent, int level) {
	HWND child = NULL;
	TCHAR buf[MAX_PATH];
	//DWORD pid = 0, tid = 0;

	do {
		child = FindWindowEx(parent, child, NULL, NULL);
		int ret = GetWindowText(child, buf, MAX_PATH);
		buf[ret] = 0;
		//tid = GetWindowThreadProcessId(child, &pid);
		//for (int i = 0; i < level; ++i)
		//	_tprintf(L"\t");
		//_tprintf(L"%s ,  pid:%d, tid:%d\n", buf, pid, tid);

		isFindChild(child);
		if (this->m_hwnd) return;

		if (child)
			Find_window2(child, level + 1);
	} while (child);

}

// 根据车的图，得到棋盘句柄
bool LinkBoard::getChessboardHwnd(bool onlyBChe, bool getChess)
{
#if 1
	
	LPCWSTR pClass = NULL;
	LPCWSTR pCaption = NULL;	
	this->m_hwnd = nullptr;
	
	//if(m_Parentclass != "")
	if (m_Parentclass != "") {
		pClass = reinterpret_cast <LPCWSTR>(m_Parentclass.data());
	}
	if (m_ParentKeyword != "") {
		pCaption = reinterpret_cast <LPCWSTR>(m_ParentKeyword.data());
	}
	
	HWND hParent = ::FindWindow(pClass, pCaption);
	if (!hParent) {
		return false;
	}

	pClass = NULL;
	pCaption = NULL;

	if (m_class != "" || m_titleKeyword != "") {

		// 查找子窗口
		HWND child = NULL;
		do {
			//查找 Explore 下的一个窗口,如果能找到则根据 Explore 下的child 继续找
			child = FindWindowEx(hParent, child, NULL, NULL);
			//int ret = GetWindowText(child, buf, MAX_PATH);
			//buf[ret] = 0;
			//tid = GetWindowThreadProcessId(child, &pid);
			//_tprintf(L"%s ,  pid:%d, tid:%d\n", buf, pid, tid);
			
			isFindChild(child);
			if (this->m_hwnd) break;

			//遍历子窗口们
			if (child)
				Find_window2(child, 1);
		} while (child);


		/*

		if (m_class != "") {
			pClass = reinterpret_cast <LPCWSTR>(m_class.data());
		}
		if (m_titleKeyword != "") {
			pCaption = reinterpret_cast <LPCWSTR>(m_titleKeyword.data());
		}

		HWND hSub = ::FindWindowEx(hParent, NULL, pClass, pCaption);
		if (!hSub) {
			return false;
		}
		this->m_hwnd = hSub;
		*/
	}
	else {
		this->m_hwnd = hParent;
	}
#else	
	this->m_hwnd = (HWND)0x001A105E; // (HWND)0x0000000000970E0E;
#endif

	if (this->m_hwnd == nullptr) return false;


	if (!getChess) return true;

	

	// 再确认一下，是不是有二个黑车
	if (onlyBChe) {
		if (!searchChess(this->m_hwnd, "br.png", m_LxBoard[0].BCheList, Side::Black, true)) {
			return false;
		}
		if (m_LxBoard[0].BCheList.count() == 2) {
			return true;
		}
	}
	else {
		if (!searchChess(this->m_hwnd, "bk.png", m_LxBoard[0].BKingList, Side::Black, true)) {
			return false;
		}

		if (!searchChess(this->m_hwnd, "rk.png", m_LxBoard[0].RKingList, Side::White)) {
			return false;
		}

		if (m_LxBoard[0].RKingList.count() == 1 && m_LxBoard[0].BKingList.count() == 1) {
			return true;  // 兵河把将当时间方了！！
		}
	}	
	return false;
}

bool LinkBoard::isFindChild(HWND hw)
{
	QString wc = this->get_window_class(hw);
	QString wt = this->get_window_title(hw);

	if (wc == this->m_class && wt == this->m_titleKeyword) {
		this->m_hwnd = hw;		
		return true;
	}

	return false;
}

bool LinkBoard::Board2Move(GenericMove& m)
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

			if (chess2 != chess1) {     // 有棋子走动
				if (chess1 == ChinesePieceType::eNoPice) {       // 
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

		if (from != to) {

			ChinesePieceType piece = pre_board[from];
			if (piece <= ChinesePieceType::eBKing && piece >= ChinesePieceType::eBPawn) {
				if (m_side != Side::Black) {
					return false;
				}
			}
			else if (piece <= ChinesePieceType::eRKing && piece >= ChinesePieceType::eRPawn) {
				if (m_side != Side::White) {
					return false;
				}
			}
			else {
				//this->SendMessageToMain("error", "move error");
				return false;
			}

			Square from_square = Square(fx, 9 - fy);
			Square to_square = Square(tx, 9 - ty);

			m = GenericMove(from_square, to_square);

			if (m_pMain->isMoveValid(m) == true) {

				if (m == m_preMove) {   // 二次确认
					// 走子
					this->m_LxBoard[0].b90[from] = ChinesePieceType::eNoPice;
					this->m_LxBoard[0].b90[to] = piece;
					return true;
				}	
				m_preMove = m; 
			}

		}
	}

	return false;
}

bool LinkBoard::GetFen(stLxBoard* pList)
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

	fillB90(pList->b90, pList->RCheList, ChinesePieceType::eRChe);
	fillB90(pList->b90, pList->RMaList, ChinesePieceType::eRMa);
	fillB90(pList->b90, pList->RPaoList, ChinesePieceType::eRPao);
	fillB90(pList->b90, pList->RShiList, ChinesePieceType::eRShi);
	fillB90(pList->b90, pList->RXiangList, ChinesePieceType::eRXiang);
	fillB90(pList->b90, pList->RPawnList, ChinesePieceType::eRPawn);
	fillB90(pList->b90, pList->RKingList, ChinesePieceType::eRKing);

	fillB90(pList->b90, pList->BCheList, ChinesePieceType::eBChe);
	fillB90(pList->b90, pList->BMaList, ChinesePieceType::eBMa);
	fillB90(pList->b90, pList->BPaoList, ChinesePieceType::eBPao);
	fillB90(pList->b90, pList->BShiList, ChinesePieceType::eBShi);
	fillB90(pList->b90, pList->BXiangList, ChinesePieceType::eBXiang);
	fillB90(pList->b90, pList->BPawnList, ChinesePieceType::eBPawn);
	fillB90(pList->b90, pList->BKingList, ChinesePieceType::eBKing);

	// getFen from B90


	QString fen;
	QChar c;

	for (int rank = 0; rank <= 9; rank++) {
		for (int file = 0; file <= 8; ) {
			int s90 = file + rank * 9;
			ChinesePieceType chess = pList->b90[s90];

			if (chess == ChinesePieceType::eNoPice) {
				int len = 0;
				for (; file <= 8 && pList->b90[file + rank * 9] == ChinesePieceType::eNoPice; file++) {
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
	fen += (m_flip == false ? "w " : "b ");   // 这个fen 只用一次
	fen += "- - 0 1";

	pList->fen = fen;

	return true;
}

bool LinkBoard::fillB90(ChinesePieceType b90[], QVector<cv::Point>& plist, ChinesePieceType chess)
{
	for (auto p : plist) {
		int s90 = getB90(p);
		b90[s90] = chess;
	}

	return true;
}

int LinkBoard::getB90(cv::Point p)
{
	assert(this->m_Ready_LXset == true);
	
	int dx = (int)((p.x - (m_offx_che - m_offx_board)) / m_dx + 0.5f);
	int dy = (int)((p.y - (m_offy_che - m_offy_board)) / m_dy + 0.5f);

	if (m_flip) {
		dy = 9 - dy;
		dx = 8 - dx;
	}

	int s90 = dx + 9 * dy;

	if (s90 > 89) s90 = 89;
	return s90;
}

QChar LinkBoard::Qpiece_to_char(ChinesePieceType chess)
{
	static const  QChar PieceString[]
		= { '0','p','b','a','c','n','r','k','P','B','A','C','N','R','K' };
	return PieceString[(int)chess];
}

bool LinkBoard::SaveAllPiecePicture(QString subCat)
{
	if (subCat == nullptr) {
		subCat = "0";                     // 第一种棋子
	}

	// 再读一下
	if (!searchChess(this->m_hwnd, "br.png", m_LxBoard[0].BCheList, Side::Black, true)) {

		//cv::imshow("dffff", this->m_image_source);
		//cv::waitKey();

		return false;
	}

	//cv::imshow("dffff", this->m_image_source);
	//cv::waitKey();

	if (this->m_flip == false) {

		// Bche
		SaveOnePiecePic(0, 0, "br.png", subCat);
		SaveOnePiecePic(1, 0, "bn.png", subCat);
		SaveOnePiecePic(2, 0, "bb.png", subCat);
		SaveOnePiecePic(3, 0, "ba.png", subCat);
		SaveOnePiecePic(4, 0, "bk.png", subCat);

		SaveOnePiecePic(1, 2, "bc.png", subCat);
		SaveOnePiecePic(0, 3, "bp.png", subCat);

		// 红车						
		SaveOnePiecePic(0, 9, "rr.png", subCat);
		SaveOnePiecePic(1, 9, "rn.png", subCat);
		SaveOnePiecePic(2, 9, "rb.png", subCat);
		SaveOnePiecePic(3, 9, "ra.png", subCat);
		SaveOnePiecePic(4, 9, "rk.png", subCat);

		SaveOnePiecePic(1, 7, "rc.png", subCat);  // 红炮
		SaveOnePiecePic(0, 6, "rp.png", subCat);
	}
	//else {
	//	SaveOnePiecePic(0, 0, "rr.png", subCat);
	//	SaveOnePiecePic(1, 0, "rn.png", subCat);
	//	SaveOnePiecePic(2, 0, "rb.png", subCat);
	//	SaveOnePiecePic(3, 0, "ra.png", subCat);
	//	SaveOnePiecePic(4, 0, "rk.png", subCat);

	//	SaveOnePiecePic(1, 2, "rc.png", subCat);
	//	SaveOnePiecePic(0, 3, "rp.png", subCat);

	//	// 红车						
	//	SaveOnePiecePic(0, 9, "br.png", subCat);
	//	SaveOnePiecePic(1, 9, "bn.png", subCat);
	//	SaveOnePiecePic(2, 9, "bb.png", subCat);
	//	SaveOnePiecePic(3, 9, "ba.png", subCat);
	//	SaveOnePiecePic(4, 9, "bk.png", subCat);

	//	SaveOnePiecePic(1, 7, "bc.png", subCat);  // 红炮
	//	SaveOnePiecePic(0, 6, "bp.png", subCat);
	//}

	return true;
}

bool LinkBoard::SaveOnePiecePic(int x, int y, QString chessName, QString subCat)
{
	
	assert(m_Ready_LXset == true);	
	// 得到全目录
	try {
		QString runPath = QCoreApplication::applicationDirPath();
		QString picFile = runPath + "/image/linkboard/" + this->m_catName + "/chess/" + subCat + "/" + chessName;

		int pieceSize = int(m_dx * 0.50f);
		QRect rect(
			(m_offx_che) - pieceSize / 2 + x * m_dx,
			(m_offy_che) - pieceSize / 2 + y * m_dx,
			pieceSize, pieceSize);
		QPixmap cropped = this->m_capPixmap.copy(rect);
		cropped.save(picFile, "PNG");   // 黑车
	}
	catch (...) {
		return false;
	}
	return true;
}


void LinkBoard::winLeftClick(HWND hwnd, int x, int y, int off_x, int off_y)
{
	LONG temp = MAKELONG(x+off_x, y+off_y);
	::SendMessage(hwnd, WM_LBUTTONDOWN, 0, temp);
	//wait(1);
	::SendMessage(hwnd, WM_LBUTTONUP, 0, temp);
}

void LinkBoard::wait(int msec)
{
	
		//QTime dieTime = QTime::currentTime().addMSecs(msec);
		//while (QTime::currentTime() < dieTime)
		//	QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
		this->m_pCap->msleep(msec);
	
}

bool LinkBoard::SearchAndClick(QString findName, bool isCap, QString sub_catlog, HWND hw, float threshold)
{
	if (sub_catlog == nullptr) {
		sub_catlog = "/auto/";
	}
	if (hw == nullptr) {
		hw = m_hwnd;
	}
	int imgX, imgY;
	if (!searchImage(findName, isCap, sub_catlog, imgX, imgY, threshold)) {
		return false;
	}

	this->winLeftClick(hw, imgX, imgY);

	return true;
}

bool LinkBoard::searchCountours(HWND hw, QString findName, bool isCap)
{
	if (isCap) {
		if (this->captureOne(hw) == false) {
			qWarning("searchImage 1 %s 出错了！", findName);
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

		// 开操作 (去除一些噪点)
		cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, 1));
		cv::morphologyEx(imgThresholded, imgThresholded, cv::MORPH_OPEN, element);

		// 闭操作 (连接一些连通域)
		cv::morphologyEx(imgThresholded, imgThresholded, cv::MORPH_CLOSE, element);



		cv::imshow("Thresholded Image", imgThresholded); //show the thresholded image
		cv::imshow("Original", m_image_source); //show the original image

		char key = (char)cv::waitKey(300);
		if (key == 27)
			break;
	}

	// opencv 实现特定颜色线条提取与定位
	// https://blog.csdn.net/chenghaoy/article/details/86509950


	//CV_EXPORTS_W void findContours(InputArray image, OutputArrayOfArrays contours,
	//	OutputArray hierarchy, int mode,
	//	int method, Point offset = Point());

	///** @overload */
	//CV_EXPORTS void findContours(InputArray image, OutputArrayOfArrays contours,
	//	int mode, int method, Point offset = Point());

	//cv::Mat m_image_find = m_image_source;   // 转换好的主图
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

	//CV_RETR_EXTERNAL只检测最外围轮廓，包含在外围轮廓内的内围轮廓被忽略
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

bool LinkBoard::searchImage(QString findName, bool isCap, QString sub_catlog, float threshold)
{
	int imgX, imgY;
	if (sub_catlog == nullptr) {
		sub_catlog = "/auto/";
	}
	return searchImage(findName, isCap, sub_catlog, imgX, imgY, threshold);
}

bool LinkBoard::searchImage(QString findName, bool isCap, QString sub_catlog, int& imgX, int& imgY, float threshold)
{
	if (isCap) {
		if (this->captureOne(m_hwnd, false) == false) {
			qWarning("searchImage 11 %s 出错了！", qUtf8Printable(findName));
			return false;
		}
	}

	if (sub_catlog != nullptr) {
		findName = sub_catlog + findName;
	}

	//QString runPath = QCoreApplication::applicationDirPath();
	//QString picFile = runPath + "/" + this->m_catName + "/chess/" + subCat + "/" + chessName;

	cv::Mat image_template_main;
	try {

		if (this->m_MatHash.contains(findName)) {              // 保存在缓存中			
			image_template_main = this->m_MatHash.value(findName);
		}
		else {
			QString runPath = QCoreApplication::applicationDirPath() + "/image/linkboard/" + this->m_catName;
			QString fFile = runPath + findName;
			image_template_main = cv::imread(fFile.toLocal8Bit().toStdString());   // 模板图
			// 要不要缩放
			//if (this->m_scaleX != 1.0f) {
			//	cv::resize(image_template_main, image_template_main, cv::Size(), this->m_scaleX, this->m_scaleY);
			//}
			this->m_MatHash.insert(findName, image_template_main);
		}
	}
	catch (...) {
		//qWarning("searchImage 2 %s 出错了！", findName);
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
		qWarning("searchImage 3 %s 出错了！", qUtf8Printable(findName));
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

		//寻找最佳匹配位置
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
		//qWarning("searchImage 5 %s 出错了！", findName);
		return false;
	}


	return false;
}

bool LinkBoard::captureOne(HWND hw, bool isTransHSV, int sleepTimeMs)
{
	if (this->m_UseAdb) {
		//return this->pAdb->one_screenshot(this->capPixmap, disp, fname, sleepTimeMs, path);
		return false;
	}
	else {
		return this->CaptureOneNotry(hw, sleepTimeMs, isTransHSV);
	}
}

bool LinkBoard::CaptureOneNotry(HWND hw, int sleepTimeMS, bool isTransHSV)
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

		// 转换成 openCV 格式
		/*
		cv::Mat mat = QImage_to_cvMat(this->m_capPixmap.toImage(), false);
		m_image_source = cv::Mat(mat.rows, mat.cols, CV_8UC3);
		int from_to[] = { 0,0,  1,1,  2,2 };
		cv::mixChannels(&mat, 1, &m_image_source, 1, from_to, 3);
		*/

		if (this->m_capPixmap.width() < 100) {   // 图太小了
			return false;
		}

		if (m_isAutoClick) {
			m_image_source_all = QPixmapToCvMat(this->m_capPixmap, true);
			//m_image_source = m_image_source_all;
		}
		else {
			this->m_image_source = QPixmapToCvMat(this->m_capPixmap, true);

			if (this->m_Ready_LXset) {  // 当前的联线信息OK了

				//this->m_LxInfo.m_dx

#if 0
				cv::imshow("templ2", m_image_source);

				

				//int ox = m_offx_che - m_dx/2 - 2;
				//int oy = m_offy_che - m_dy/2 - 2;
				//if (ox < 0) ox = 0;
				//if (oy < 0) oy = 0;

				//int w = m_offx_che + m_dx * 8.7;
				//if (w > m_image_source.cols) {
				//	w = m_image_source.cols;
				//}
				//int h = m_offy_che + m_dy * 9.7;
				//if (h > m_image_source.rows) {
				//	h = m_image_source.rows;
				//}

				//cv::Rect crect(ox, oy, w-ox, h-oy);

				m_image_source = m_image_source(m_crect);

				//cv::imshow("templ", m_image_source);
				//cv::waitKey();
				cv::imshow("source", this->m_image_source);
				cv::waitKey();
#else
				m_image_source = m_image_source(m_crect);
#endif
			}

			// 拆分红黑二个图
			//cv::Mat m_image_red;      // 红方棋子
			//cv::Mat m_image_black;    // 黑方棋子
			if (isTransHSV) {
				cv::Mat imgHSV;
				cv::cvtColor(m_image_source, imgHSV, cv::COLOR_BGR2HSV);

				cv::inRange(imgHSV, cv::Scalar(m_iLowHred, m_iLowSred, m_iLowVred),
					cv::Scalar(m_iHighHred, m_iHighSred, m_iHighVred), m_imgage_SHV[Side::White]); //Threshold the image
				cv::inRange(imgHSV, cv::Scalar(m_iLowHblack, m_iLowSblack, m_iLowVblack),
					cv::Scalar(m_iHighHblack, m_iHighSblack, m_iHighVblack), m_imgage_SHV[Side::Black]); //Threshold the image

				// 二值化处理
				//cv::threshold(m_image_red, m_image_red, 125, 255, cv::THRESH_BINARY);
				//cv::threshold(m_image_black, m_image_black, 125, 255, cv::THRESH_BINARY);
				//cv::imshow("white", m_imgage_SHV[Side::White]);
				//cv::imshow("black", m_imgage_SHV[Side::Black]);
				//cv::waitKey();
			}
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
		//qWarning("截图出错了， %s", path + fname);
		return false;
	}
	return  true;
}

bool LinkBoard::searchChess(HWND hw, QString findName, QVector<cv::Point>& res, Side side, bool isCap, float threshold, bool isShow)
{
	if (isCap) {
		if (this->captureOne(hw) == false) {
			qWarning("searchImage 1 %s 出错了！", qUtf8Printable(findName));
			return false;
		}
	}

	//searchCountours(hw, "ff", true);

	bool Isfind = false;
	bool bIsInHash = false;

	QVector<cv::Mat> template_chess;

	try {
		if (this->m_MatHash.contains(findName)) {              // 保存在缓存中
			template_chess << this->m_MatHash.value(findName);
			bIsInHash = true;
		}
		else {  // 把所有的各种棋子放入
			QString filePath = QCoreApplication::applicationDirPath() + "/image/linkboard/" + this->m_catName + "/chess/";
			QDir dir(filePath);
			if (!dir.exists()) {
				return false;
			}
			dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot); // | QDir::Files | QDir::NoDotAndDotDot);
			//文件夹优先
			dir.setSorting(QDir::DirsFirst);
			//QFileInfoList list = dir.entryInfoList();
			QStringList files = dir.entryList();
			if (files.size() < 1) {
				return false;
			}
			for (QString file : files) {
				QString chessFile = filePath + file + "/" + findName;
				//cv::Mat mtemp = cv::imread(chessFile.toLocal8Bit().toStdString());   // 模板图
				cv::Mat imgHSV;
				cv::cvtColor(cv::imread(chessFile.toLocal8Bit().toStdString()), imgHSV, cv::COLOR_BGR2HSV);

				cv::Mat mtemp;
				if (side == Side::Black) {
					cv::inRange(imgHSV, cv::Scalar(m_iLowHblack, m_iLowSblack, m_iLowVblack),
						cv::Scalar(m_iHighHblack, m_iHighSblack, m_iHighVblack), mtemp);  //Threshold the image
				}
				else {
					cv::inRange(imgHSV, cv::Scalar(m_iLowHred, m_iLowSred, m_iLowVred),
						cv::Scalar(m_iHighHred, m_iHighSred, m_iHighVred), mtemp);        //Threshold the image
				}
				template_chess << mtemp;
			}

		}
	}
	catch (...) {
		//qWarning("searchImage 2 %s 出错了！", findName);
		return false;
	}

	cv::TemplateMatchModes method = cv::TM_SQDIFF_NORMED;  // cv::TM_CCOEFF_NORMED; // cv::TM_SQDIFF_NORMED;
	try {
		for (cv::Mat m : template_chess) {
			cv::Mat image_matched;
			cv::matchTemplate(this->m_imgage_SHV[side], m, image_matched, method);


#if 0
			cv::imshow("m_image_source", m_image_source);
			cv::imshow("black", m_imgage_SHV[1]);
			cv::imshow("red", m_imgage_SHV[0]);
			cv::imshow("m", m);
			//cv::imshow("templ", image_template2);
			cv::waitKey();  //m_imgage_SHV[side]
#endif


#if 0
			cv::imshow("m_image_source", m_image_source);
			//cv::imshow("templ", image_template2);
			cv::imshow("m_image_black", m_image_red);
			cv::imshow("matched", image_matched);
			cv::waitKey();
#endif
			if (threshold == 1.0f) {
				threshold = this->m_precision_chess;
			}
			res.clear();  // 清空数组

			while (true) {
				cv::Point minLoc, maxLoc;
				double minVal, maxVal;

				//寻找最佳匹配位置
				cv::minMaxLoc(image_matched, &minVal, &maxVal, &minLoc, &maxLoc);


				//double matchVal = minVal / tmpSize;  // 去掉模板大小对匹配精度的影响
				//threshold = 0.90;
				double matchThres = maxVal * (1.0 - threshold);
				if (minVal < matchThres) {

					//if(maxVal > 0.75){

					Isfind = true;
					

					cv::Point chessP;

					chessP.x = minLoc.x + m.cols / 2;
					chessP.y = minLoc.y + m.rows / 2;


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

					//cv::circle(image_matched, maxLoc, 10, cv::Scalar(minVal), -1);

					//cv::imshow("m2", image_matched);
					//cv::waitKey();
				}
				else {
					break;
				}

			}

			if (Isfind) {
				if (bIsInHash == false) {					
					this->m_MatHash.insert(findName, m);		
				}
				return true;
			}
		}
	}
	catch (...) {
		//qWarning("searchImage 2 %s 出错了！", findName);
		return false;
	}
	return false;
}



cv::Mat LinkBoard::QImageToCvMat(const QImage& inImage, bool inCloneImageData)
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

cv::Mat LinkBoard::QPixmapToCvMat(const QPixmap& inPixmap, bool inCloneImageData)
{
	return QImageToCvMat(inPixmap.toImage(), inCloneImageData);
}

QString LinkBoard::get_window_title(HWND hwnd)
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

QString LinkBoard::get_window_class(HWND hwnd)
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

// https://www.wandouip.com/t5i217503/	
QDataStream& operator<<(QDataStream& input, const LinkBoard& board)
{
	// TODO: 在此处插入 return 语句
	input 
		//<< board.m_catName
		<< board.m_ParentKeyword
		<< board.m_Parentclass
		<< board.m_titleKeyword
		<< board.m_class
		<< board.m_offx_che
		<< board.m_offy_che
		<< board.m_dx
		<< board.m_dy
		<< board.m_UseAdb
		<< board.m_precision_chess
		<< board.m_precision_auto
		<< board.m_sleepTimeMs
		<< board.m_scaleX
		<< board.m_scaleY
		<< board.m_iLowHred
		<< board.m_iHighHred
		<< board.m_iLowSred
		<< board.m_iHighSred
		<< board.m_iLowVred
		<< board.m_iHighVred
		<< board.m_iLowHblack
		<< board.m_iHighHblack
		<< board.m_iLowSblack
		<< board.m_iHighSblack
		<< board.m_iLowVblack
		<< board.m_iHighVblack;

	return input;
}

QDataStream& operator>>(QDataStream& output, LinkBoard& board)
{
	// TODO: 在此处插入 return 语句
	output 
		//>> board.m_catName
		>> board.m_ParentKeyword
		>> board.m_Parentclass
		>> board.m_titleKeyword
		>> board.m_class
		>> board.m_offx_che
		>> board.m_offy_che
		>> board.m_dx
		>> board.m_dy
		>> board.m_UseAdb
		>> board.m_precision_chess
		>> board.m_precision_auto
		>> board.m_sleepTimeMs
		>> board.m_scaleX
		>> board.m_scaleY
		>> board.m_iLowHred
		>> board.m_iHighHred
		>> board.m_iLowSred
		>> board.m_iHighSred
		>> board.m_iLowVred
		>> board.m_iHighVred
		>> board.m_iLowHblack
		>> board.m_iHighHblack
		>> board.m_iLowSblack
		>> board.m_iHighSblack
		>> board.m_iLowVblack
		>> board.m_iHighVblack;

	return output;
}

}