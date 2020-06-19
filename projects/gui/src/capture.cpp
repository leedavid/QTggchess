#include "capture.h"

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

namespace Chess {

	// 初始化静态成员
	//QMutex Capture::mutex;
	//bool Capture::m_MayNewGame = false;

	Capture::Capture(QObject* parent, QString catName, bool isAuto)
		:QThread(parent),
		pMain((MainWindow*)(parent))
	{
		//m_linkBoard = new LinkBoard(pMain, this, "天天象棋", isAuto);
		m_linkBoard = new LinkBoard(pMain, this, catName, isAuto);

		connect(this, SIGNAL(CapSendSignal(stCaptureMsg)),
			parent, SLOT(slotProcessCapMsg(stCaptureMsg)));
	}



	//stCaptureMsg m_msg;      // 是不是可能放在
	//stCaptureMsg m_msg;      // 是不是可能放在
	void Capture::SendMessageToMain(const QString title, const QString msg)
	{
		stCaptureMsg m_msg;      // 是不是可能放在
		m_msg.mType = stCaptureMsg::eText;
		m_msg.text = title;
		m_msg.title = msg;
		emit CapSendSignal(m_msg);
	}

	void Capture::SendMoveToMain(const Chess::GenericMove m)
	{
		stCaptureMsg m_msg;      // 是不是可能放在
		m_msg.mType = stCaptureMsg::eMove;
		m_msg.m = m;
		emit CapSendSignal(m_msg);
	}

	void Capture::SendFenToMain(const QString fen)
	{
		stCaptureMsg m_msg;      // 是不是可能放在
		m_msg.mType = stCaptureMsg::eSetFen;
		m_msg.text = fen;
		emit CapSendSignal(m_msg);
	}

	// 根据引擎发来的棋步，点击棋盘
	void Capture::ProcessBoardMove(const Chess::GenericMove& move) {

		this->m_linkBoard->ProcessBoardMove(move);
	}



	// 线程运行
	void Capture::run() {	

		this->m_linkBoard->run();
	}

	// 启动线程
	void Capture::on_start()
	{
		this->start();	
	}

	void Capture::on_stop()
	{
		//bMustStop = true;
		this->m_linkBoard->setStop(true);
	}

	void Capture::on_pause()
	{
	}

	Capture::~Capture()
	{
		while (isRunning()) {
			on_stop();
			wait(1);
		}
		delete m_linkBoard;
		//delete this->m_board;		
	}
	};  // namespace Chess 
	/*
	// 自动开始点击图片
	void Capture::runAutoClip()
	{
		//this->m_MatHash.clear(); // 清空一下
		bMustStop = false;
		this->m_Ready_LXset = true;

		//if (!isSolutionReady()) {
		//	this->GetLxInfo("0");
		//	if (!isSolutionReady()) {
		//		SendMessageToMain("出错啦", "连线方案还没有准备好！");
		//		return;
		//	}
		//}

		if (isFindAutoWin() == false) {
			SendMessageToMain("出错啦", "没有找到天天象棋界面！");
			return;
		}

		// 点击所有自动目录下的图
		QStringList nameFilters;
		nameFilters << "*.png";
		QString runPath = QCoreApplication::applicationDirPath();

		while (true) {
			if (bMustStop) break;
			bool findNewGame = false;

			// 1. 发现这些图就等待
			QString dirpath = runPath + "/image/findchess/0/not/";
			QDir dir(dirpath);
			QStringList files = dir.entryList(nameFilters, QDir::Files | QDir::Readable, QDir::Name);
			
			while (true) {
				bool isCap = true;   // 第一次要读一下图
				bool isFind = false;
				for (QString file : files) {
					if (searchImage(file, isCap, "0/not/")) {  // 
						wait(100);
						isFind = true;
					}
					isCap = false;
					if (bMustStop) break;
				}
				if (!isFind) break;				
			}
			
			// 2. 对局中发现的，对方求和，我方要说话等
			dirpath = runPath + "/image/findchess/0/gaming/";
			dir = QDir(dirpath);
			files = dir.entryList(nameFilters, QDir::Files | QDir::Readable, QDir::Name);			
			while (true) {
				bool isCap = true;   // 第一次要读一下图
				bool isFind = false;
				for (QString file : files) {
					if (SearchAndClick(file, isCap, "0/gaming/")) {  // 
						wait(100);
						isFind = true;
					}
					isCap = false;
					if (bMustStop) break;
				}
				if (!isFind) break;
			}			

			// 3. 可能是新开的棋局		
			dirpath = runPath + "/image/findchess/0/auto/";
			dir = QDir(dirpath);
			files = dir.entryList(nameFilters, QDir::Files | QDir::Readable, QDir::Name);	
			while (true) {		
				bool isFind = false;
				for (QString file : files) {					
					if (this->SearchAndClick(file, true)) {  // 这个一直要读
						findNewGame = true;
						isFind = true;
					}
					wait(300);
					if (bMustStop) break;
				}
				if (!isFind) break;
			}

			if (findNewGame == true) {
				Capture::mutex.lock();
				Capture::m_MayNewGame = true;  // 通知另外一个线程
				Capture::mutex.unlock();
			}

			wait(500);
		}

		bMustStop = false;
	}

	// 连线下棋
	void Capture::runAutoChess()
	{
		

		bMustStop = false;
		m_bWeMustSendInitFen = false;

		if (m_Chess == LinkWhich::TianTian) {

			this->m_Ready_LXset = true;

			if (!isSolutionReady()) {

				this->GetLxInfo("0");

				if (!isSolutionReady()) {

					SendMessageToMain("出错啦", "连线方案还没有准备好！");
					return;
				}
			}
		}
		else if (m_Chess == LinkWhich::KingChess) {

			this->m_Ready_LXset = false;
			if (!isSolutionReady()) {

				this->GetLxInfo("1");

				if (!isSolutionReady()) {

					SendMessageToMain("出错啦", "连线方案还没有准备好！");
					return;
				}
			}
		}
	


		QString MoveSendingFen;
		Chess::GenericMove MoveSendingMove;
		this->m_LxBoard[0].fen = "none";

		//this->m_hwnd = HWND(0x004B09F6);
		//if (!SearchOnChessList(m_hwnd, "br.png", this->m_LxBoard[0].BCheList, SearchWhichCap::eBlack, true)) {
		//
		//	int a = 0;
		//}


		QElapsedTimer timeRun;  // 超时定时器
		timeRun.start();
		quint64 StartTime = timeRun.elapsed();
		while (true) {   // 走棋

			if (bMustStop) break;
			
			if (GetLxBoardChess(1)) {   // 读出第二个棋盘''

				QString fen = this->m_LxBoard[1].fen;
				if (fen == "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR b - - 0 1") { // 是初始局面，这个是对方在走第一步
					wait(m_sleepTimeMs);
					continue;
				}
				
				if (this->m_LxBoard[0].fen != fen) {
					if (fen.contains("rnbakabnr/9/1c5c1/p1p1p1p1p", Qt::CaseSensitive)  // 黑棋没有动过
						&& (fen.contains("b -", Qt::CaseSensitive))){ 
						m_bWeMustSendInitFen = true;
					}

					if (fen == "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1") {
						m_bWeMustSendInitFen = true;
					}
				}
				if (this->m_LxBoard[0].fen == "none") {  // 才启动					
					m_bWeMustSendInitFen = true;
				}

				// 发送初始fen				
				if (m_bWeMustSendInitFen) {
					QString fen = this->m_LxBoard[1].fen;
					SendFenToMain(fen);

					StartTime = timeRun.elapsed();   // 发送棋盘后重置一下棋局开始时间

					//this->m_LxBoard[0].b90 = this->m_LxBoard[0].b90;
					int size = sizeof(this->m_LxBoard[0].b90);
					memcpy(this->m_LxBoard[0].b90, this->m_LxBoard[1].b90, size);
					this->m_LxBoard[0].fen = this->m_LxBoard[1].fen;

					m_bWeMustSendInitFen = false;
					for (int i = 0; i < 10; i++) {  // 延时2秒，等待主界面启动引擎
						wait(100);
						if (bMustStop) break;
					}
				}
				else {
					if (Capture::m_MayNewGame) {
						if (fen == "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1") {
							if (((timeRun.elapsed() - StartTime) / 1000.0) > 25.0) {
								m_bWeMustSendInitFen = true;
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
						SendMoveToMain(m);
					}
				}
			}
			wait(m_sleepTimeMs);


			if (Capture::m_MayNewGame) {
				Capture::mutex.lock();
				Capture::m_MayNewGame = false;;
				Capture::mutex.unlock();
				this->m_MatHash.clear(); // 清空一下
			}			
			
		}
		//SendMessageToMain("OK", "你已退出连线！");
	}

	// 得到所有的棋子列表
	bool Capture::GetLxBoardChess(int index)
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
		stLxBoard* pList = &m_LxBoard[index];

		if (!SearchOnChessList(m_hwnd, "bk.png", pList->BKingList, SearchWhichCap::eBlack, true)) {    // 黑将
			return false;  // 找不到黑将了
		}

		if (!SearchOnChessList(m_hwnd, "rk.png", pList->RKingList, SearchWhichCap::eRed)) {    // 红将
			return false; // 找不到红将了
		}


		SearchOnChessList(m_hwnd, "br.png", pList->BCheList, SearchWhichCap::eBlack);     // 黑车
		SearchOnChessList(m_hwnd, "bn.png", pList->BMaList, SearchWhichCap::eBlack);      // 黑马
		SearchOnChessList(m_hwnd, "bc.png", pList->BPaoList, SearchWhichCap::eBlack);     // 黑炮
		SearchOnChessList(m_hwnd, "ba.png", pList->BShiList, SearchWhichCap::eBlack);     // 黑士
		SearchOnChessList(m_hwnd, "bb.png", pList->BXiangList, SearchWhichCap::eBlack);   // 黑象
		SearchOnChessList(m_hwnd, "bp.png", pList->BPawnList, SearchWhichCap::eBlack);    // 黑兵


		SearchOnChessList(m_hwnd, "rr.png", pList->RCheList, SearchWhichCap::eRed);     // 红车
		SearchOnChessList(m_hwnd, "rn.png", pList->RMaList, SearchWhichCap::eRed);      // 红马
		SearchOnChessList(m_hwnd, "rc.png", pList->RPaoList, SearchWhichCap::eRed);     // 红炮
		SearchOnChessList(m_hwnd, "ra.png", pList->RShiList, SearchWhichCap::eRed);     // 红士
		SearchOnChessList(m_hwnd, "rb.png", pList->RXiangList, SearchWhichCap::eRed);   // 红象
		SearchOnChessList(m_hwnd, "rp.png", pList->RPawnList, SearchWhichCap::eRed);    // 红兵
		

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
		int pieceSize = int(m_LxInfo.m_dx * 0.8f);
		QString picPath = getFindPath() + m_LxInfo.m_PieceCatlog + "/" + chessName;

		QRect rect(
			m_LxInfo.m_offx - pieceSize / 2 + x * m_LxInfo.m_dx,
			m_LxInfo.m_offy - pieceSize / 2 + y * m_LxInfo.m_dx,
			pieceSize, pieceSize);
		QPixmap cropped = this->m_capPixmap.copy(rect);
		cropped.save(picPath, "PNG");   // 黑车

		//cv::FileStorage

		return true;
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
		
		//CV_EXPORTS void findContours(InputArray image, OutputArrayOfArrays contours,
		//	int mode, int method, Point offset = Point());

		//cv::Mat m_image_find = m_image_source;   // 转换好的主图
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

	bool Capture::getKingInfo(HWND hw, QString findName, bool isCap)
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

	bool Capture::searchChess(HWND hw, QString findName, QVector<cv::Point>& res, SearchWhichCap sWhich, bool isCap, float threshold, bool isShow)
	{
		if (isCap) {
			if (this->captureOne(hw) == false) {
				qWarning("searchImage 1 %s 出错了！", qUtf8Printable(findName));
				return false;
			}
		}

		//searchCountours(hw, findName, isCap);

		bool Isfind = false;
		int findNumber = 0;

		
		//cv::Mat m_bin;
		//cv::threshold(this->m_image_black, m_bin, 125, 255, cv::THRESH_BINARY);
		
		cv::imshow("red", this->m_image_red);
		cv::imshow("black", this->m_image_black);
		cv::imshow("img", m_image_source);

		//cv::imshow("bin", m_bin);

		cv::waitKey();
		
		


		// threshold(imag, result, 30, 200.0, CV_THRESH_BINARY);
		

		//cv::imshow("matched", image_template2);
		//cv::waitKey();

		cv::Mat image_template_main;

		cv::Mat image_template_chess; // 棋子有可能有几种
		bool bIsInHash = false;

		cv::Mat image_template_chess_pre;  // 棋子有可能有几种
		cv::Mat image_template_chess_gold; // 棋子有可能有几种
		//cv::Mat image_template_chess_light; // 棋子有可能有几种

		try {

			if (this->m_MatHash.contains(findName)) {              // 保存在缓存中
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
					image_template_main = cv::imread(fFile.toLocal8Bit().toStdString());   // 模板图

					// 要不要缩放
					//if (this->m_scaleX != 1.0f) {
					//	cv::resize(image_template_main, image_template_main, cv::Size(), this->m_scaleX, this->m_scaleY);
					//}
					this->m_MatHash.insert(findName, image_template_main);
				}
				else {



					// 在这儿再裁一下，因为有边框干扰		

					//int w = image_template_scaled.rows;
					//int h = image_template_scaled.cols;
					//float s = this->m_chessClip;
					//cv::Rect crect(w * s, h * s, w * (1 - 2 * s), h * (1 - 2 * s));
					//image_template_scaled = image_template_scaled(crect);


					QString fFile = this->getFindPath() + findName;
					cv::Mat image_template = cv::imread(fFile.toLocal8Bit().toStdString());   // 模板图

					// 要处理成HSV mask 格式

					cv::Mat imgHSV;
					cv::cvtColor(image_template, imgHSV, cv::COLOR_BGR2HSV);


					if (sWhich == SearchWhichCap::eBlack) {
						cv::inRange(imgHSV, cv::Scalar(iLowHblack, iLowSblack, iLowVblack), cv::Scalar(iHighHblack, iHighSblack, iHighVblack), image_template_chess_pre); //Threshold the image
						//cv::threshold(image_template_chess_pre, image_template_chess_pre, 125, 255, cv::THRESH_BINARY);
					}
					else {
						cv::inRange(imgHSV, cv::Scalar(iLowHred, iLowSred, iLowVred), cv::Scalar(iHighHred, iHighSred, iHighVred), image_template_chess_pre); //Threshold the image
						//cv::threshold(image_template_chess_pre, image_template_chess_pre, 125, 255, cv::THRESH_BINARY);
					}
					// this->m_MatHash.insert(findName, image_template2); 后面正确的插入
					//tmpSize = image_template2.cols * image_template2.rows;

					// 金棋子
					if (m_Chess == LinkWhich::TianTian) {
						fFile = this->getFindPath() + "/0/golden/" + findName;
					}
					else {
						fFile = this->getFindPath() + "/1/golden/" + findName;
					}
					image_template = cv::imread(fFile.toLocal8Bit().toStdString());   // 模板图
					cv::cvtColor(image_template, imgHSV, cv::COLOR_BGR2HSV);

					if (sWhich == SearchWhichCap::eBlack) {
						cv::inRange(imgHSV, cv::Scalar(iLowHblack, iLowSblack, iLowVblack), cv::Scalar(iHighHblack, iHighSblack, iHighVblack), image_template_chess_gold); //Threshold the image
						//cv::threshold(image_template_chess_gold, image_template_chess_gold, 125, 255, cv::THRESH_BINARY);
					}
					else {
						cv::inRange(imgHSV, cv::Scalar(iLowHred, iLowSred, iLowVred), cv::Scalar(iHighHred, iHighSred, iHighVred), image_template_chess_gold); //Threshold the image
						//cv::threshold(image_template_chess_gold, image_template_chess_gold, 125, 255, cv::THRESH_BINARY);
					}

					/
					// 发光棋子 image_template_chess_light					
					fFile = this->getFindPath() + "/0/light/" + findName;
					image_template = cv::imread(fFile.toLocal8Bit().toStdString());   // 模板图
					cv::cvtColor(image_template, imgHSV, cv::COLOR_BGR2HSV);

					if (sWhich == SearchWhichCap::eBlack) {
						cv::inRange(imgHSV, cv::Scalar(iLowHblack, iLowSblack, iLowVblack), cv::Scalar(iHighHblack, iHighSblack, iHighVblack), image_template_chess_light); //Threshold the image
						//cv::threshold(image_template_chess_light, image_template_chess_light, 125, 255, cv::THRESH_BINARY);
					}
					else {
						cv::inRange(imgHSV, cv::Scalar(iLowHred, iLowSred, iLowVred), cv::Scalar(iHighHred, iHighSred, iHighVred), image_template_chess_light); //Threshold the image
						//cv::threshold(image_template_chess_light, image_template_chess_light, 125, 255, cv::THRESH_BINARY);
					}
					/

					//cv::imshow("pre", image_template_chess_pre);
					//cv::imshow("gold", image_template_chess_gold);
					//cv::waitKey();

				}
			}
		}
		catch (...) {
			//qWarning("searchImage 2 %s 出错了！", findName);
			return false;
		}


		//cv::imshow("templ", m_image_black);
		//cv::imshow("templ", m_image_red);
		//cv::imshow("img", m_image_source);
		//cv::imshow("matched", image_template2);
		//cv::waitKey();



		//cv::imshow("img", image_source);
		//cv::imshow("templ", image_template_scaled);
		//cv::waitKey();

		cv::TemplateMatchModes method = cv::TM_SQDIFF_NORMED;  // cv::TM_CCOEFF_NORMED; // cv::TM_SQDIFF_NORMED;

		for (int i = 0; i < 2; i++) {

			cv::Mat image_matched;
			try {
				if (sWhich == SearchWhichCap::eMain) {

					if (i == 1) {
						return false;
					}
					cv::matchTemplate(m_image_source, image_template_main, image_matched, method); // cv::TM_CCORR); // cv::TM_SQDIFF);

				}
				else if (bIsInHash == true) {
					if (i == 1) {
						return false;
					}
					if (sWhich == SearchWhichCap::eBlack) {
						cv::matchTemplate(m_image_black, image_template_chess, image_matched, method); // cv::TM_CCORR); // cv::TM_SQDIFF);
					}
					else {
						cv::matchTemplate(m_image_red, image_template_chess, image_matched, method); // cv::TM_CCORR); // cv::TM_SQDIFF);
					}
				}
				else {
					if (sWhich == SearchWhichCap::eBlack) {
						if (i == 0) {
							cv::matchTemplate(m_image_black, image_template_chess_pre, image_matched, method); // cv::TM_CCORR); // cv::TM_SQDIFF);
						}
						else if(i== 1){
							cv::matchTemplate(m_image_black, image_template_chess_gold, image_matched, method); // cv::TM_CCORR); // cv::TM_SQDIFF);
						}
						//else if (i == 2) {
						//	cv::matchTemplate(m_image_black, image_template_chess_light, image_matched, method); // cv::TM_CCORR); // cv::TM_SQDIFF);
						//}
					}
					else {
						if (i == 0) {
							cv::matchTemplate(m_image_red, image_template_chess_pre, image_matched, method); // cv::TM_CCORR); // cv::TM_SQDIFF);
						}
						else if (i == 1) {
							cv::matchTemplate(m_image_red, image_template_chess_gold, image_matched, method); // cv::TM_CCORR); // cv::TM_SQDIFF);
						}
						//else if (i == 2) {
						//	cv::matchTemplate(m_image_red, image_template_chess_light, image_matched, method); // cv::TM_CCORR); // cv::TM_SQDIFF);
						//}
					}
				}				
			}
			catch (...) {
				qWarning("searchImage 3 %s 出错了！", qUtf8Printable(findName));
				return false;
			}

			//if (i == 1) {
			//	cv::imshow("gold_temp", image_template_chess_gold);
			//	cv::waitKey();
			//}

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
						findNumber = i;

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
							else if (i == 1) {
								chessP.x = minLoc.x + image_template_chess_gold.cols / 2;
								chessP.y = minLoc.y + image_template_chess_gold.rows / 2;
							}
							//else if (i == 2) {
							//	chessP.x = minLoc.x + image_template_chess_light.cols / 2;
							//	chessP.y = minLoc.y + image_template_chess_light.rows / 2;
							//}
						}

	/*					if (sWhich == SearchWhichCap::eMain) {
							chessP.x = maxLoc.x + image_template_main.cols / 2;
							chessP.y = maxLoc.y + image_template_main.rows / 2;
						}
						else {
							if (bIsInHash == true) {
								chessP.x = maxLoc.x + image_template_chess.cols / 2;
								chessP.y = maxLoc.y + image_template_chess.rows / 2;
							}
							else if (i == 0) {
								chessP.x = maxLoc.x + image_template_chess_pre.cols / 2;
								chessP.y = maxLoc.y + image_template_chess_pre.rows / 2;
							}
							else if (i == 1) {
								chessP.x = maxLoc.x + image_template_chess_gold.cols / 2;
								chessP.y = maxLoc.y + image_template_chess_gold.rows / 2;
							}
							else if (i == 2) {
								chessP.x = maxLoc.x + image_template_chess_light.cols / 2;
								chessP.y = maxLoc.y + image_template_chess_light.rows / 2;
							}
						}/

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
			}
			catch (...) {
				qWarning("searchImage 5 %s 出错了！", qUtf8Printable(findName));
				return false;
			}

			if (Isfind) {
				if (bIsInHash == false) {
					if (findNumber == 0) {
						this->m_MatHash.insert(findName, image_template_chess_pre);
					}
					else if (findNumber == 1) {
						this->m_MatHash.insert(findName, image_template_chess_gold);
					}
					//else if (findNumber == 2) {
					//	this->m_MatHash.insert(findName, image_template_chess_light);
					//}
				}
				return true;
			}
		}		

		return false;

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

	// 截图并拆分成红黑二个
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

			// 转换成 openCV 格式
			/*
			cv::Mat mat = QImage_to_cvMat(this->m_capPixmap.toImage(), false);
			m_image_source = cv::Mat(mat.rows, mat.cols, CV_8UC3);
			int from_to[] = { 0,0,  1,1,  2,2 };
			cv::mixChannels(&mat, 1, &m_image_source, 1, from_to, 3);
			/	

			if (this->m_capPixmap.width() < 100) {
				return false;
			}

			if (m_isAutoClick) {
				m_image_source_all = QPixmapToCvMat(this->m_capPixmap, true);
				m_image_source = m_image_source_all;
			}
			else {
				this->m_image_source = QPixmapToCvMat(this->m_capPixmap, true);
			}

			
			if (this->m_Ready_LXset) {  // 当前的联线信息OK了

				//this->m_LxInfo.m_dx

				cv::Rect crect(0, 0, m_LxInfo.m_offx + m_LxInfo.m_dx * 8.8, m_LxInfo.m_offy + m_LxInfo.m_dx * 9.8);

				m_image_source = m_image_source(crect);

				//cv::imshow("templ", m_image_source);
				//cv::waitKey();
			}

			// 拆分红黑二个图
			//cv::Mat m_image_red;      // 红方棋子
			//cv::Mat m_image_black;    // 黑方棋子
			if (isHSV) {
				cv::Mat imgHSV;
				cv::cvtColor(m_image_source, imgHSV, cv::COLOR_BGR2HSV);

				cv::inRange(imgHSV, cv::Scalar(iLowHred, iLowSred, iLowVred), cv::Scalar(iHighHred, iHighSred, iHighVred), m_image_red); //Threshold the image
				cv::inRange(imgHSV, cv::Scalar(iLowHblack, iLowSblack, iLowVblack), cv::Scalar(iHighHblack, iHighSblack, iHighVblack), m_image_black); //Threshold the image

				// 二值化处理
				//cv::threshold(m_image_red, m_image_red, 125, 255, cv::THRESH_BINARY);
				//cv::threshold(m_image_black, m_image_black, 125, 255, cv::THRESH_BINARY);
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
			/
					
			
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

		//if (window == HWND(0x000114F2)) {  // Chrome Legacy Window
		//	int a = 0;
		//}

		if (
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
		QString chessFile = m_LxInfo.m_PieceCatlog + "/" + chess;  // 

		return searchChess(hwnd, chessFile, res, sWhich, isCap);
	}

	bool Capture::isChessBoardWindow(HWND hwnd, stLxBoard* pieceList, bool onlyBche)
	{
		// +cname	Qt5QWindowIcon	QString
		QString wc = this->get_window_class(hwnd);
		if (wc == "Qt5QWindowIcon") {   // 去了自己的窗口
			return false;
		}


		// 
		if (m_Chess == LinkWhich::TianTian) {

			//QString title = this->get_window_title(hwnd);

			if (wc == this->m_LxInfo.m_class) {

				// 得找到32个圆				
				if (onlyBche) {
					//QString rk = m_LxInfo.m_PieceCatlog + "/br.png";  // 黑车
					//if (!searchImage(hwnd, rk, pieceList->BCheList, nullptr, true)) {

					if (!SearchOnChessList(hwnd, "br.png", pieceList->BCheList, SearchWhichCap::eBlack, true)) {
						return false;
					}
					//if (pieceList->BCheList.count() >= 1) {
					//	int a = 0;
					//}
					if (pieceList->BCheList.count() == 2) {
						return true;
					}
				}
				else {  // 得判断双方有将

					if (!SearchOnChessList(hwnd, "bk.png", pieceList->BKingList, SearchWhichCap::eBlack, true)) {
						return false;
					}

					if (!SearchOnChessList(hwnd, "rk.png", pieceList->RKingList, SearchWhichCap::eRed)) {
						return false;
					}

					if (pieceList->RKingList.count() == 1 && pieceList->BKingList.count() == 1) {
						return true;  // 兵河把将当时间方了！！
					}
				}
			}
		}
		else if (m_Chess == LinkWhich::KingChess) {
			QString wtitle = this->get_window_title(hwnd);

			//if (wtitle.contains("王")) {
			//	int a = 0;
			//}

			if (wtitle == "Chrome Legacy Window"){ // "王者象棋") {

			//if(hwnd == HWND(0x000514CA)){
				if (onlyBche) {
					//QString rk = m_LxInfo.m_PieceCatlog + "/br.png";  // 黑车
					//if (!searchImage(hwnd, rk, pieceList->BCheList, nullptr, true)) {

					if (!SearchOnChessList(hwnd, "br.png", pieceList->BCheList, SearchWhichCap::eBlack, true)) {
						return false;
					}
					//if (pieceList->BCheList.count() >= 1) {
					//	int a = 0;
					//}
					if (pieceList->BCheList.count() == 2) {
						return true;
					}
				}
				else {  // 得判断双方有将

					if (!SearchOnChessList(hwnd, "bk.png", pieceList->BKingList, SearchWhichCap::eBlack, true)) {
						return false;
					}

					if (!SearchOnChessList(hwnd, "rk.png", pieceList->RKingList, SearchWhichCap::eRed)) {
						return false;
					}

					if (pieceList->RKingList.count() == 1 && pieceList->BKingList.count() == 1) {
						return true;  // 兵河把将当时间方了！！
					}
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
			//if (wc == "Qt5QWindowIcon") {   // 去了自己的窗口
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
	   
	*/


//void Capture::initBoard()
//{
//
//	if (m_Chess == LinkWhich::TianTian) {
//
//		m_precision_chess = 0.57f; // was 0.52
//		m_precision_auto = 0.98f;
//		m_UseAdb = false;
//		m_sleepTimeMs = 20;
//		m_scaleX = 1.0f;
//		m_scaleY = 1.0f;
//
//		m_Ready_LXset = false;
//		m_chessWinOK = false;
//
//		//m_chessClip = 0.25f;
//
//		//this->m_board = Chess::BoardFactory::create("standard");
//		//this->m_board_second = Chess::BoardFactory::create("standard");
//		//m_board->reset();
//		//m_board_second->reset();
//
//
//		this->m_LxInfo.m_LX_name = "兵河五四小棋盘";
//		this->m_LxInfo.m_titleKeyword = "中国象棋2017"; // "BHGUI(test) - 新棋局";
//		this->m_LxInfo.m_class = "QQChess"; // "Afx:00400000:b:00010003:00000006:0A1D05FB";
//
//		//this->m_LxInfo.offx = 29.0f;
//		//this->m_LxInfo.offy = 138.0f;
//		//this->m_LxInfo.m_dx = 28.0f;
//		//this->m_LxInfo.m_dy = 28.0f;
//
//		this->m_LxInfo.m_offx = 73.0f;
//		this->m_LxInfo.m_offy = 175.0f;
//		this->m_LxInfo.m_dx = 68.0f;
//		this->m_LxInfo.m_dy = 68.0f;
//
//		this->m_Ready_LXset = false;
//
//		this->m_LxInfo.m_PieceCatlog = "0";
//		this->m_connectedBoard_OK = false;  // 
//
//		m_side = Chess::Side::NoSide;
//
//		m_flip = false;
//	}
//	else if (m_Chess == LinkWhich::KingChess) {
//		m_precision_chess = 0.57f; // was 0.52
//		m_precision_auto = 0.98f;
//		m_UseAdb = false;
//		m_sleepTimeMs = 20;
//		m_scaleX = 1.0f;
//		m_scaleY = 1.0f;
//
//		m_Ready_LXset = false;
//		m_chessWinOK = false;
//
//		//m_chessClip = 0.25f;
//
//		//this->m_board = Chess::BoardFactory::create("standard");
//		//this->m_board_second = Chess::BoardFactory::create("standard");
//		//m_board->reset();
//		//m_board_second->reset();
//
//
//		this->m_LxInfo.m_LX_name = "兵河五四小棋盘";
//		this->m_LxInfo.m_titleKeyword = "中国象棋2017"; // "BHGUI(test) - 新棋局";
//		this->m_LxInfo.m_class = "QQChess"; // "Afx:00400000:b:00010003:00000006:0A1D05FB";
//
//		//this->m_LxInfo.offx = 29.0f;
//		//this->m_LxInfo.offy = 138.0f;
//		//this->m_LxInfo.m_dx = 28.0f;
//		//this->m_LxInfo.m_dy = 28.0f;
//
//		this->m_LxInfo.m_offx = 37.0f;
//		this->m_LxInfo.m_offy = 41.0f;
//		this->m_LxInfo.m_dx = 50.0f;
//		this->m_LxInfo.m_dy = 50.0f;
//
//		this->m_Ready_LXset = false;
//
//		this->m_LxInfo.m_PieceCatlog = "1";
//		this->m_connectedBoard_OK = false;  // 
//
//		m_side = Chess::Side::NoSide;
//
//		m_flip = false;
//
//		iLowHred = 0;
//		iHighHred = 10;
//
//		iLowSred = 77;
//		iHighSred = 255;
//
//		iLowVred = 95;
//		iHighVred = 255;
//
//		iLowHblack = 0;
//		iHighHblack = 179;
//
//		iLowSblack = 0;
//		iHighSblack = 139;
//
//		iLowVblack = 0;
//		iHighVblack = 131;
//	}
//
//}
//
//cv::Mat Capture::QImageToCvMat(const QImage& inImage, bool inCloneImageData)
//{
//	switch (inImage.format())
//	{
//		// 8-bit, 4 channel
//	case QImage::Format_ARGB32:
//	case QImage::Format_ARGB32_Premultiplied:
//	{
//		cv::Mat  mat(inImage.height(), inImage.width(),
//			CV_8UC4,
//			const_cast<uchar*>(inImage.bits()),
//			static_cast<size_t>(inImage.bytesPerLine())
//		);
//
//		return (inCloneImageData ? mat.clone() : mat);
//	}
//
//	// 8-bit, 3 channel
//	case QImage::Format_RGB32:
//	case QImage::Format_RGB888:
//	{
//		if (!inCloneImageData)
//		{
//			qWarning() << "CVS::QImageToCvMat() - Conversion requires cloning because we use a temporary QImage";
//		}
//
//		QImage   swapped = inImage;
//
//		if (inImage.format() == QImage::Format_RGB32)
//		{
//			swapped = swapped.convertToFormat(QImage::Format_RGB888);
//		}
//
//		swapped = swapped.rgbSwapped();
//
//		return cv::Mat(swapped.height(), swapped.width(),
//			CV_8UC3,
//			const_cast<uchar*>(swapped.bits()),
//			static_cast<size_t>(swapped.bytesPerLine())
//		).clone();
//	}
//
//	// 8-bit, 1 channel
//	case QImage::Format_Indexed8:
//	{
//		cv::Mat  mat(inImage.height(), inImage.width(),
//			CV_8UC1,
//			const_cast<uchar*>(inImage.bits()),
//			static_cast<size_t>(inImage.bytesPerLine())
//		);
//
//		return (inCloneImageData ? mat.clone() : mat);
//	}
//
//	default:
//		qWarning() << "CVS::QImageToCvMat() - QImage format not handled in switch:" << inImage.format();
//		break;
//	}
//
//	return cv::Mat();
//	// https ://blog.csdn.net/qq_17550379/article/details/78683153
//}
//
//cv::Mat Capture::QPixmapToCvMat(const QPixmap& inPixmap, bool inCloneImageData)
//{
//	return QImageToCvMat(inPixmap.toImage(), inCloneImageData);
//}
//
//void Capture::winLeftClick(HWND hwnd, int x, int y)
//{
//	/*	if (this->isUseAdb) {
//			this->pAdb->LeftClick(x, y);
//		}
//		else {*/
//	LONG temp = MAKELONG(x, y);
//	// click
//	//::PostMessage(hwnd, WM_ACTIVATE, WA_ACTIVE, 0);
//	//::PostMessage(hwnd, WM_LBUTTONDOWN, MK_LBUTTON, temp);
//	//wait(10);
//	//::PostMessage(hwnd, WM_LBUTTONUP, 0, temp);
//
//	//::SendMessage(hwnd, WM_ACTIVATE, WA_ACTIVE, 0);
//	//::SendMessage(hwnd, WM_LBUTTONDOWN, MK_LBUTTON, temp);
//	::SendMessage(hwnd, WM_LBUTTONDOWN, 0, temp);
//	::SendMessage(hwnd, WM_LBUTTONUP, 0, temp);
//	//}
//}
//
//// 根据引擎发来的棋步，点击棋盘
//void Capture::ProcessBoardMove(const Chess::GenericMove& move) {
//
//	this->m_linkBoard->ProcessBoardMove(move);
//	/*
//	int fx = move.sourceSquare().file();
//	int fy = move.sourceSquare().rank();
//
//	int tx = move.targetSquare().file();
//	int ty = move.targetSquare().rank();
//
//	int from = (9 - fy) * 9 + fx;
//	int to = (9 - ty) * 9 + tx;
//	ChinesePieceType piece = this->m_LxBoard[0].b90[from];
//	if (piece <= ChinesePieceType::eBKing && piece >= ChinesePieceType::eBPawn) {
//		m_side = Chess::Side::White;
//	}
//	else if (piece <= ChinesePieceType::eRKing && piece >= ChinesePieceType::eRPawn) {
//		m_side = Chess::Side::Black;
//	}
//	else {
//		//this->SendMessageToMain("error", "move error");
//		return;
//	}
//
//	// 更新一下当前棋盘
//
//	if (m_flip) {
//		fx = 8 - fx;
//		fy = 9 - fy;
//		tx = 8 - tx;
//		ty = 9 - ty;
//	}
//
//
//	cLXinfo* pInfo = &this->m_LxInfo;
//
//	int ffx = pInfo->m_offx + fx * pInfo->m_dx;
//	int ffy = pInfo->m_offy + (9 - fy) * pInfo->m_dy;
//
//	int ttx = pInfo->m_offx + tx * pInfo->m_dx;
//	int tty = pInfo->m_offy + (9 - ty) * pInfo->m_dy;
//
//	this->m_LxBoard[0].b90[from] = ChinesePieceType::eNoPice;
//	this->m_LxBoard[0].b90[to] = piece;
//
//	winLeftClick(m_hwnd, ffx, ffy);
//	wait(5);
//	winLeftClick(m_hwnd, ttx, tty);
//	*/
//
//}
//
//bool Capture::GetLxInfo(QString catlog)  // 只要查到有没有车就行了
//{
//
//	m_Ready_LXset = false;
//
//	cLXinfo* pInfo = &this->m_LxInfo;
//
//	stLxBoard* pPieceList = &m_LxBoard[0];
//
//	pInfo->m_PieceCatlog = catlog;
//
//
//	if (!getChessboardHwnd(true))
//		return false;
//
//	// 
//	cv::Point Bche0;
//	cv::Point Bche1;
//
//	if (pPieceList->BCheList[0].x < pPieceList->BCheList[1].x) {
//		Bche0 = pPieceList->BCheList[0];
//		Bche1 = pPieceList->BCheList[1];
//	}
//	else {
//		Bche1 = pPieceList->BCheList[0];
//		Bche0 = pPieceList->BCheList[1];
//	}
//
//	pInfo->m_offx = Bche0.x;
//	pInfo->m_offy = Bche0.y;
//
//	pInfo->m_dx = (Bche1.x - Bche0.x) / 8;
//	pInfo->m_dy = pInfo->m_dx;
//
//	if (pInfo->m_dx < 10) return false;
//
//	pInfo->m_class = this->get_window_class(this->m_hwnd);
//	pInfo->m_titleKeyword = this->get_window_title(this->m_hwnd);
//
//	// 把棋盘搞小一些啊，
//
//	//
//	m_Ready_LXset = true;
//	return this->SaveAllPiecePicture();
//}
//
//
//bool Capture::SearchAndClick(QString findName, bool isCap, QString sub_catlog, HWND hw, float threshold)
//{
//	if (m_Chess == LinkWhich::TianTian) {
//		if (sub_catlog == nullptr) {
//			sub_catlog = "0/auto/";
//		}
//	}
//	else if (m_Chess == LinkWhich::KingChess) {
//		if (sub_catlog == nullptr) {
//			sub_catlog = "1/auto/";
//		}
//	}
//	if (hw == nullptr) {
//		hw = m_hwnd;
//	}
//
//	int imgX, imgY;
//	if (!searchImage(findName, isCap, sub_catlog, imgX, imgY, threshold)) {
//		return false;
//	}
//
//	this->winLeftClick(hw, imgX, imgY);
//	//wait(1000);
//
//	return true;
//}
//
//bool Capture::searchImage(QString findName, bool isCap, QString sub_catlog, float threshold)
//{
//	int imgX, imgY;
//	if (sub_catlog == nullptr) {
//		sub_catlog = "0/auto/";
//	}
//	return searchImage(findName, isCap, sub_catlog, imgX, imgY, threshold);
//}
//
//bool Capture::searchImage(QString findName, bool isCap, QString sub_catlog, int& imgX, int& imgY, float threshold)
//{
//	if (isCap) {
//		if (this->captureOne(m_hwnd, false) == false) {
//			qWarning("searchImage 11 %s 出错了！", qUtf8Printable(findName));
//			return false;
//		}
//	}
//
//	if (sub_catlog != nullptr) {
//		findName = sub_catlog + findName;
//	}
//
//	cv::Mat image_template_main;
//	try {
//
//		if (this->m_MatHash.contains(findName)) {              // 保存在缓存中			
//			image_template_main = this->m_MatHash.value(findName);
//		}
//		else {
//			QString fFile = this->getFindPath() + findName;
//			image_template_main = cv::imread(fFile.toLocal8Bit().toStdString());   // 模板图
//			// 要不要缩放
//			//if (this->m_scaleX != 1.0f) {
//			//	cv::resize(image_template_main, image_template_main, cv::Size(), this->m_scaleX, this->m_scaleY);
//			//}
//			this->m_MatHash.insert(findName, image_template_main);
//		}
//	}
//	catch (...) {
//		//qWarning("searchImage 2 %s 出错了！", findName);
//		return false;
//	}
//
//
//	//cv::imshow("templ", m_image_black);
//	//cv::imshow("img", m_image_source);
//	//cv::imshow("matched", image_template2);
//	//cv::waitKey();	
//
//	cv::Mat image_matched;
//	try {
//		cv::matchTemplate(m_image_source_all, image_template_main, image_matched, cv::TM_SQDIFF_NORMED); // cv::TM_CCORR); // cv::TM_SQDIFF);
//	}
//	catch (...) {
//		qWarning("searchImage 3 %s 出错了！", qUtf8Printable(findName));
//		return false;
//	}
//
//#if 0
//	cv::imshow("m_image_source", m_image_source);
//	//cv::imshow("templ", image_template2);
//	cv::imshow("m_image_black", m_image_red);
//	cv::imshow("matched", image_matched);
//	cv::waitKey();
//#endif
//	try {
//
//		if (threshold == 1.0f) {
//			threshold = this->m_precision_auto;
//		}
//
//		cv::Point minLoc, maxLoc;
//		double minVal, maxVal;
//
//		//寻找最佳匹配位置
//		cv::minMaxLoc(image_matched, &minVal, &maxVal, &minLoc, &maxLoc);
//
//		double matchThres = maxVal * (1 - threshold);
//
//		if (minVal < matchThres) {
//			imgX = minLoc.x + image_template_main.cols / 2;
//			imgY = minLoc.y + image_template_main.rows / 2;
//			return true;
//		}
//		else {
//			return false;
//		}
//	}
//	catch (...) {
//		//qWarning("searchImage 5 %s 出错了！", findName);
//		return false;
//	}
//
//
//	return false;
//}

	//QString Capture::GetFenLxBoard(bool isOrg)
	//{
	//	if (isOrg) {
	//		return this->m_LxBoard[0].pl
	//	}
	//}

	//QChar Capture::Qpiece_to_char(ChinesePieceType chess)
	//{
	//	static const  QChar PieceString[]
	//		= { '0','p','b','a','c','n','r','k','P','B','A','C','N','R','K' };
	//	return PieceString[(int)chess];
	//}

	//bool Capture::isSolutionReady()
	//{
	//	return m_Ready_LXset;
	//}

	//int Capture::getB90(cv::Point p)
	//{
	//	int dx = (int)((p.x - m_LxInfo.m_offx) / m_LxInfo.m_dx + 0.5f);
	//	int dy = (int)((p.y - m_LxInfo.m_offy) / m_LxInfo.m_dy + 0.5f);

	//	if (m_flip) {
	//		dy = 9 - dy;
	//		dx = 8 - dx;
	//	}

	//	int s90 = dx + 9 * dy;

	//	if (s90 > 89) s90 = 89;
	//	return s90;
	//}

	//bool Capture::fillB90(ChinesePieceType b90[], QVector<cv::Point>& plist, ChinesePieceType chess)
	//{
	//	for (auto p : plist) {
	//		int s90 = getB90(p);
	//		b90[s90] = chess;
	//	}

	//	return true;
	//}

	//bool Capture::GetFen(stLxBoard* pList)
	//{
	//	//int b90[90] = { 0 };
	//	//int a = sizeof(pList->b90);
	//	memset(pList->b90, 0, sizeof(pList->b90));


	//	//bool flip = false;
	//	if (pList->BKingList[0].y > pList->RKingList[0].y) {
	//		m_flip = true;
	//	}
	//	else {
	//		m_flip = false;
	//	}

	//	fillB90(pList->b90, pList->RCheList, ChinesePieceType::eRChe);
	//	fillB90(pList->b90, pList->RMaList, ChinesePieceType::eRMa);
	//	fillB90(pList->b90, pList->RPaoList, ChinesePieceType::eRPao);
	//	fillB90(pList->b90, pList->RShiList, ChinesePieceType::eRShi);
	//	fillB90(pList->b90, pList->RXiangList, ChinesePieceType::eRXiang);
	//	fillB90(pList->b90, pList->RPawnList, ChinesePieceType::eRPawn);
	//	fillB90(pList->b90, pList->RKingList, ChinesePieceType::eRKing);
	//			
	//	fillB90(pList->b90, pList->BCheList, ChinesePieceType::eBChe);
	//	fillB90(pList->b90, pList->BMaList, ChinesePieceType::eBMa);
	//	fillB90(pList->b90, pList->BPaoList, ChinesePieceType::eBPao);
	//	fillB90(pList->b90, pList->BShiList, ChinesePieceType::eBShi);
	//	fillB90(pList->b90, pList->BXiangList, ChinesePieceType::eBXiang);
	//	fillB90(pList->b90, pList->BPawnList, ChinesePieceType::eBPawn);
	//	fillB90(pList->b90, pList->BKingList, ChinesePieceType::eBKing);
	//	
	//	// getFen from B90


	//	QString fen;
	//	QChar c;

	//	for (int rank = 0; rank <= 9; rank++) {
	//		for (int file = 0; file <= 8; ) {
	//			int s90 = file + rank * 9;
	//			ChinesePieceType chess = pList->b90[s90];

	//			if (chess == ChinesePieceType::eNoPice) {
	//				int len = 0;
	//				for (; file <= 8 && pList->b90[file + rank * 9] == ChinesePieceType::eNoPice; file++) {
	//					len++;
	//				}
	//				c = '0' + len;
	//			}
	//			else {
	//				c = Qpiece_to_char(chess);
	//				file++;
	//			}
	//			fen += c;
	//		}
	//		fen += (rank < 9 ? '/' : ' ');
	//	}
	//	fen += (m_flip == false ? "w " : "b ");   // 这个fen 只用一次
	//	fen += "- - 0 1";

	//	pList->fen = fen;

	//	return true;
	//}

	////由新旧两个盘面的变化情况找到上一步走子状态
	//bool Capture::Board2Move(Chess::GenericMove& m)
	//{
	//	ChinesePieceType* pre_board = this->m_LxBoard[0].b90;
	//	ChinesePieceType* now_board = this->m_LxBoard[1].b90;

	//	int fx = -1;
	//	int fy = -1;
	//	int tx = -1;
	//	int ty = -1;
	//

	//	for (int y = 0; y < 10; y++) {
	//		for (int x = 0; x < 9; x++) {
	//			ChinesePieceType chess2 = pre_board[y * 9 + x];
	//			ChinesePieceType chess1 = now_board[y * 9 + x];

	//			if (chess2 != chess1) {     // 有棋子走动
	//				if (chess1 == ChinesePieceType::eNoPice) {       // 
	//					fx = x;
	//					fy = y;						
	//				}
	//				else {
	//					tx = x;
	//					ty = y;						
	//				}					
	//			}
	//		}
	//	}

	//	if (fx != -1 && fy != -1 && tx != -1 && ty != -1) {
	//		int from = fy * 9 + fx;
	//		int to = ty * 9 + tx;

	//		
	//		//if (m_flip) {
	//		//	from = (9 - fy) * 9 + (8-fx);
	//		//	to = (9 - ty) * 9 + (8-tx);
	//		//}
	//		

	//		if (from != to) {

	//			ChinesePieceType piece = pre_board[from];
	//			if (piece <= ChinesePieceType::eBKing && piece >= ChinesePieceType::eBPawn) {
	//				if (m_side != Chess::Side::Black) {
	//					return false;
	//				}
	//			}
	//			else if (piece <= ChinesePieceType::eRKing && piece >= ChinesePieceType::eRPawn) {
	//				if (m_side != Chess::Side::White) {
	//					return false;
	//				}
	//			}
	//			else {
	//				//this->SendMessageToMain("error", "move error");
	//				return false;
	//			}

	//			//if (m_flip) {
	//			//	fx = 8 - fx;
	//			//	fy = 9 - fy;
	//			//	tx = 8 - tx;
	//			//	ty = 9 - ty;
	//			//}

	//			Square from_square = Square(fx, 9 - fy);
	//			Square to_square = Square(tx, 9 - ty);


	//			m = Chess::GenericMove(from_square, to_square);

	//			if (pMain->isMoveValid(m) == true) {
	//				// 走子
	//				this->m_LxBoard[0].b90[from] = ChinesePieceType::eNoPice;
	//				this->m_LxBoard[0].b90[to] = piece;



	//				//m = 0x331e;

	//				//int source = Chess::sq

	//				//Square(int file, int rank);
	//				//
	//				//Chess::GenericMove gmove;
	//				//gmove.
	//				//
	//				//m = Chess::Move(from, to);
	//				return true;

	//			}
	//			
	//		}
	//	}
	//	
	//	return false;
	//}
