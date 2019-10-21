#include "capture.h"

#include <QThread>
#include <QScreen>
#include <QPixmap>
#include <QRect>
#include <QGuiApplication>
#include <QMessagebox>
#include <board/boardfactory.h>

namespace Chess {

	class MainWindow;

	enum ChinesePieceType
	{
		eBPawn = 1,	//!< Pawn
		eBXiang,		//!< Knight
		eBShi,		//!< Bishop
		eBPao,		//!< Rook
		eBMa,		    //!< Ma
		eBChe,		//!< Queen
		eBKing,		//!< King

		eRPawn,	    //!< Pawn
		eRXiang,		//!< Knight
		eRShi,		//!< Bishop
		eRPao,		//!< Rook
		eRMa,		    //!< Ma
		eRChe,		//!< Queen
		eRKing		//!< King
	};


	Capture::Capture(ChessGame* pGame, QObject* parent)
		:QThread(parent)
	{		
		//MsgBoxThread(par);
		this->m_msg.pGame = pGame;

		initBoard();

		connect(this, SIGNAL(CapSendSignal(stCaptureMsg)),
			parent, SLOT(processCapMsg(stCaptureMsg)));
	}
	   	  
	void Capture::initBoard()
	{
		m_precision = 0.96f;
		m_UseAdb = false;
		m_sleepTimeMs = 350;
		m_scaleX = 1.0f;
		m_scaleY = 1.0f;

		m_Ready_LXset = false;
		m_chessWinOK = false;
		
		m_chessClip = 0.2f;
		
		this->m_board = Chess::BoardFactory::create("standard");
		//this->m_board_second = Chess::BoardFactory::create("standard");
		m_board->reset();
		//m_board_second->reset();


		this->m_LxInfo.m_LX_name = "兵河五四小棋盘";
		this->m_LxInfo.m_titleKeyword = "BHGUI(test) - 新棋局";
		this->m_LxInfo.m_class = "Afx:00400000:b:00010003:00000006:0A1D05FB";

		this->m_LxInfo.offx = 29.0f;
		this->m_LxInfo.offy = 138.0f;
		this->m_LxInfo.m_dx = 28.0f;
		this->m_LxInfo.m_dy = 28.0f;
		this->m_Ready_LXset = true;

		this->m_LxInfo.m_PieceCatlog = "0";

		this->m_connectedBoard_OK = false;  // 

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


	bool Capture::GetLxInfo(QString catlog)  // 只要查到有没有车就行了
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

		// 把棋盘搞小一些啊，

		//
		m_Ready_LXset = true;
		return this->SaveAllPiecePicture();
	}


	void Capture::SendMessageToMain(QString title, QString msg)
	{
	}

	void Capture::SendMoveToMain(Chess::Move m)
	{
	}

	void Capture::SendFenToMain(QString fen)
	{
	}

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
		int s90 = dx + 9 * dy;

		if (s90 > 89) s90 = 89;
		return s90;
	}

	bool Capture::fillB90(int b90[], QVector<cv::Point>& plist, int chess)
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

		bool flip = false;
		if (pList->BKingList[0].y > pList->RKingList[0].y) {
			flip = true;
		}

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
		fen += (flip == false ? "w " : "b ");
		fen += "- - 0 1";

		pList->fen = fen;

		return true;
	}

	// 线程运行
	void Capture::run() {

		// 发送一个走步
		//{
		//	m_msg.mType = stCaptureMsg::eMove;
		//	m_msg.m = m_board->moveFromString("h2e2");
		//	emit CapSendSignal(m_msg);	
		//	return;
		//}

		//this->GetMoveFromBoard();
		if (!isSolutionReady()) {			
			m_msg.mType = stCaptureMsg::eText;
			m_msg.text = "连线方案还没有准备好！";
			m_msg.title = "出错啦";
			emit CapSendSignal(m_msg);
			return;
		}

		//
	}

	Chess::Move Capture::GetMoveFromBoard()
	{
		//while (true)
		//{
		//	this->GetLxBoardChess();
		//}
		
		return Chess::Move();
	}

	// 启动线程
	void Capture::on_start()
	{
		this->start();
	}

	// 得到所有的棋子列表
	bool Capture::GetLxBoardChess(bool org)
	{

		if (this->m_Ready_LXset == false)
			this->GetLxInfo("0");

		if (this->m_Ready_LXset == false) return false;

		if (this->m_connectedBoard_OK == false) {  // 还没有连接棋盘
			if (!getChessboardHwnd()) {
				qWarning("找不到棋盘！");
				return false;
			}
		}

		if (this->m_connectedBoard_OK == false) return false;

		// 
		stLxBoard* pList = &m_LxBoard[1];
		if (org) {
			pList = &m_LxBoard[0];
		}

		if (!SearchOnChessList(m_hwnd, "bk.png", pList->BKingList, true)) {    // 黑将
			return false;  // 找不到对方的将了
		}

		SearchOnChessList(m_hwnd, "br.png", pList->BCheList);     // 黑车
		SearchOnChessList(m_hwnd, "bn.png", pList->BMaList);      // 黑马
		SearchOnChessList(m_hwnd, "bc.png", pList->BPaoList);     // 黑炮
		SearchOnChessList(m_hwnd, "ba.png", pList->BShiList);     // 黑士
		SearchOnChessList(m_hwnd, "bb.png", pList->BXiangList);   // 黑象
		SearchOnChessList(m_hwnd, "bp.png", pList->BPawnList);    // 黑兵


		SearchOnChessList(m_hwnd, "rr.png", pList->RCheList);     // 红车
		SearchOnChessList(m_hwnd, "rn.png", pList->RMaList);      // 红马
		SearchOnChessList(m_hwnd, "rc.png", pList->RPaoList);     // 红炮
		SearchOnChessList(m_hwnd, "ra.png", pList->RShiList);     // 红士
		SearchOnChessList(m_hwnd, "rb.png", pList->RXiangList);   // 红象
		SearchOnChessList(m_hwnd, "rp.png", pList->RPawnList);    // 红兵
		SearchOnChessList(m_hwnd, "rk.png", pList->RKingList);    // 红将

		return GetFen(pList);
	}

	// 得到所有的棋子的图片
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

		// 红车
		SaveOnePiecePic(0, 9, "rr.png");
		SaveOnePiecePic(1, 9, "rn.png");
		SaveOnePiecePic(2, 9, "rb.png");
		SaveOnePiecePic(3, 9, "ra.png");
		SaveOnePiecePic(4, 9, "rk.png");

		SaveOnePiecePic(1, 7, "rc.png");  // 红炮
		SaveOnePiecePic(0, 6, "rp.png");

		return true;
	}

	bool Capture::SaveOnePiecePic(int x, int y, QString chessName)
	{
		int pieceSize = int(m_LxInfo.m_dx * 0.65f);
		QString picPath = getFindPath() + m_LxInfo.m_PieceCatlog + "/" + chessName;

		QRect rect(
			m_LxInfo.offx - pieceSize / 2 + x * m_LxInfo.m_dx,
			m_LxInfo.offy - pieceSize / 2 + y * m_LxInfo.m_dx,
			pieceSize, pieceSize);
		QPixmap cropped = this->m_capPixmap.copy(rect);
		cropped.save(picPath, "PNG");   // 黑车

		return true;
	}

	Capture::~Capture()
	{
		delete this->m_board;
		//delete this->m_board_second;
	}

	bool Capture::captureOne(QString fname, HWND hw, bool disp, int sleepTimeMs, QString path)
	{
		Q_UNUSED(disp);

		if (path == nullptr) {
			path = this->getPicturePath();
		}
		if (this->m_UseAdb) {
			//return this->pAdb->one_screenshot(this->capPixmap, disp, fname, sleepTimeMs, path);
			return false;
		}
		else {
			return this->CaptureOneNotry(fname, hw, sleepTimeMs, path);
		}
	}

	bool Capture::searchImage(QString findName, bool isCap, QString mainName, HWND hw)
	{
		QVector<cv::Point> res;
		if (hw == nullptr) {
			hw = this->m_hwnd;
		}
		return this->searchImage(hw, findName, res, mainName, isCap);
	}

	bool Capture::searchImage(HWND hw, QString findName, QVector<cv::Point>& res, QString mainName, bool isCap, float threshold, bool isShow)
	{
		if (isCap) {
			if (this->captureOne(mainName, hw) == false) {
				qWarning("searchImage 1 %s 出错了！", findName);
				return false;
			}
		}

		cv::Mat image_template_scaled;
		//cv::Mat image_source;

		try {
			if (mainName == nullptr) {

				//cv::Mat mat = QImage_to_cvMat(this->capPixmap.toImage(), false);
				//image_source = cv::Mat(mat.rows, mat.cols, CV_8UC3);
				//int from_to[] = { 0,0,  1,1,  2,2 };
				//cv::mixChannels(&mat, 1, &image_source, 1, from_to, 3);

				//image_source = mat2;
			}
			else {
				QString mFile = this->getPicturePath() + mainName;
				m_image_source = cv::imread(mFile.toStdString()); // 主图
			}

			//QString hashName = "aff"; // getHashName(findName);

			if (this->m_MatHash.contains(findName)) {              // 保存在缓存中
				image_template_scaled = this->m_MatHash.value(findName);
			}
			else {
				QString fFile = this->getFindPath() + findName;
				cv::Mat image_template = cv::imread(fFile.toStdString());   // 模板图


				cv::resize(image_template, image_template_scaled, cv::Size(), this->m_scaleX, this->m_scaleY);


				// 在这儿再裁一下，因为有边框干扰		

				int w = image_template_scaled.rows;
				int h = image_template_scaled.cols;
				float s = this->m_chessClip;
				cv::Rect crect(w * s, h * s, w * (1 - 2 * s), h * (1 - 2 * s));
				image_template_scaled = image_template_scaled(crect);

				this->m_MatHash.insert(findName, image_template_scaled);
			}


			//QString fFile = this->getFindPath() + findName;
			//cv::Mat image_template = cv::imread(fFile.toStdString());   // 模板图
			//cv::resize(image_template, image_template_scaled, cv::Size(), this->scaleX, this->scaleY);

		}
		catch (...) {
			qWarning("searchImage 2 %s 出错了！", findName);
			return false;
		}


		//cv::imshow("img", image_source);
		//cv::imshow("templ", image_template_scaled);
		//cv::waitKey();

		cv::Mat image_matched;
		try {
			cv::matchTemplate(m_image_source, image_template_scaled, image_matched, cv::TM_CCOEFF_NORMED);
		}
		catch (...) {
			qWarning("searchImage 3 %s 出错了！", findName);
			return false;
		}

		//cv::imshow("templ", image_template_scaled);
		//cv::imshow("img", image_source);
		//cv::imshow("matched", image_matched);
		//cv::GOB::WaitKey();

		if (threshold == 1.0f) {
			threshold = this->m_precision;
		}
		res.clear();  // 清空数组

		bool Isfind = false;

		while (true) {
			cv::Point minLoc, maxLoc;
			double minVal, maxVal;

			//寻找最佳匹配位置
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

		return Isfind;
	}

	cv::Mat Capture::QImage_to_cvMat(const QImage& image, bool inCloneImageData)
	{
		switch (image.format())
		{
			// 对应Mat中8位4通道图像
		case QImage::Format_RGB32:
		{
			cv::Mat mat(image.height(), image.width(), CV_8UC4, const_cast<uchar*>(image.bits()), image.bytesPerLine());
			return (inCloneImageData ? mat.clone() : mat);
		}

		// 对应Mat中8位3通道图像
		case QImage::Format_RGB888:
		{
			if (!inCloneImageData) {
				//qWarning() << "ASM::QImageToCvMat() - Conversion requires cloning since we use a temporary QImage";
			}

			//rgbSwapped函数用于翻转RB通道，并返回一个新QImage，并不会改变原有图像
			QImage swapped = image.rgbSwapped();
			return cv::Mat(swapped.height(), swapped.width(), CV_8UC3, const_cast<uchar*>(swapped.bits()), swapped.bytesPerLine()).clone();
		}

		// 8位单通道
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

	bool Capture::CaptureOneNotry(QString fname, HWND hw, int sleepTimeMS, QString path)
	{

		try {

			QThread::msleep(sleepTimeMS);

			QScreen* screen = QGuiApplication::primaryScreen();

			this->m_capPixmap = screen->grabWindow(WId(hw));

			//fname = "tmp2.png";
			if (fname != nullptr) {
				this->m_capPixmap.save(path + fname, "PNG");
			}

			//QString cname = this->get_window_class(hw);

			// 转换成 openCV 格式
			/*
			cv::Mat mat = QImage_to_cvMat(this->m_capPixmap.toImage(), false);
			m_image_source = cv::Mat(mat.rows, mat.cols, CV_8UC3);
			int from_to[] = { 0,0,  1,1,  2,2 };
			cv::mixChannels(&mat, 1, &m_image_source, 1, from_to, 3);
			*/

			this->m_image_source = QPixmapToCvMat(this->m_capPixmap, true);

			if (this->m_Ready_LXset) {  // 当前的联线信息OK了

				//this->m_LxInfo.m_dx

				cv::Rect crect(0, 0, m_LxInfo.offx + m_LxInfo.m_dx * 8.8, m_LxInfo.offy + m_LxInfo.m_dx * 9.8);
				m_image_source = m_image_source(crect);

				//cv::imshow("templ", m_image_source);
				//cv::waitKey();
			}

			// isDisplay	
			//if (this->displayCapture) {
			//	GOB::DisplayImage(this->capPixmap, this->isZoom);
			//}

		}
		catch (...) {
			qWarning("截图出错了， %s", path + fname);
			return false;
		}
		return  true;
	}

	bool Capture::getChessboardHwnd(bool onlyBChe)
	{
		HWND hw = first_window();
		while (hw != nullptr)
		{
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

	bool Capture::SearchOnChessList(HWND hwnd, QString chess, QVector<cv::Point>& res, bool isCap)
	{
		QString chessFile = m_LxInfo.m_PieceCatlog + "/" + chess;  // 黑车

		return searchImage(hwnd, chessFile, res, nullptr, isCap);
	}

	bool Capture::isChessBoardWindow(HWND hwnd, stLxBoard* pieceList, bool onlyBche)
	{
		// +cname	Qt5QWindowIcon	QString
		QString wc = this->get_window_class(hwnd);
		if (wc == "Qt5QWindowIcon") {   // 去了自己的窗口
			return false;
		}

		if (onlyBche) {
			//QString rk = m_LxInfo.m_PieceCatlog + "/br.png";  // 黑车
			//if (!searchImage(hwnd, rk, pieceList->BCheList, nullptr, true)) {

			if (!SearchOnChessList(hwnd, "br.png", pieceList->BCheList, true)) {
				return false;
			}
			if (pieceList->BCheList.count() == 2) {
				return true;
			}
		}
		else {  // 得判断双方有将

			if (!SearchOnChessList(hwnd, "bk.png", pieceList->BKingList, true)) {
				return false;
			}

			if (!SearchOnChessList(hwnd, "rk.png", pieceList->RKingList)) {
				return false;
			}

			if (pieceList->RKingList.count() >= 1 && pieceList->BKingList.count() >= 1) {
				return true;  // 兵河把将当时间方了！！
			}
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
