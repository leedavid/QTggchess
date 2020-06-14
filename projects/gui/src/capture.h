#pragma once

#include "linkboard.h"

#include <Windows.h>
#include <QString>
#include <QPixmap>
#include <QProcess>
#include <Qthread>
#include <QMessagebox>
#include <QMutex>


#include <chessgame.h>

//#include "mainwindow.h"
//class MainWindow;



namespace Chess {

	//enum SearchWhichCap {
	//	eMain = 1,
	//	eRed,
	//	eBlack
	//};


	//struct cLXinfo {
	//	QString m_LX_name;                 // 连线的名称
	//	QString m_ParentKeyword;           // 父窗口关键词
	//	QString m_Parentclass;             // 父窗口类
	//	QString m_titleKeyword;            // 窗口关键词
	//	QString m_class;                   // 窗口类
	//	QString m_PieceCatlog;             // 棋子的图片目录
	//	float m_offx;					   // 棋盘原点x
	//	float m_offy;                      // 棋盘原点y

	//	float m_dx;                        // 棋盘格宽
	//	float m_dy;                        // 棋盘格高	

	//	bool m_isAdb;                      // 是否手机adb连接
	//};
	

	class Capture : public QThread
	{
		Q_OBJECT

		enum window_search_mode {
			INCLUDE_MINIMIZED,
			EXCLUDE_MINIMIZED
		};

	public:	


	signals:
		void CapSendSignal(stCaptureMsg msg);

	public slots:
		void ProcessBoardMove(const Chess::GenericMove& move);
	
		
	private:

		void run() Q_DECL_OVERRIDE;

		

		


	public:

		//static QMutex mutex;
		//static bool m_MayNewGame;

		explicit Capture(QObject* parent, QString catName, bool isAuto = false);
		//Capture(float precision, bool UseAdb = false, int sleepMs = 200, float scX = 1.0f, float scY = 1.0f);
		~Capture();

		//bool m_isRuning;
		//bool m_bMainGetFenAlready;                // 已发送初始局面了
		

		//Chess::Move GetMoveFromBoard();

		void SetCatlogName(QString catName) { this->m_linkBoard->SetCatlogName(catName); };

		void on_start();
		void on_stop();
		void on_pause();

		//stCaptureMsg m_msg;      // 是不是可能放在
		void SendMessageToMain(const QString title, const QString msg);
		void SendMoveToMain(const Chess::GenericMove m);
		void SendFenToMain(const QString fen);

	private:

		LinkBoard* m_linkBoard;

		//LinkWhich m_Chess;

		//void runAutoClip();
		//void runAutoChess();

		////void mywait(int msec) {
		////	QTime dieTime = QTime::currentTime().addMSecs(msec);
		////	while (QTime::currentTime() < dieTime)
		////		QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
		////}

		//// 得到连线的信息
		//bool GetLxInfo(QString catlog);

		//bool GetLxBoardChess(int index);

		//bool GetFen(stLxBoard* pList);

		//bool Board2Move(Chess::GenericMove& m);

		//bool SearchAndClick(QString findName, bool isCap, QString sub_catlog = nullptr, HWND hw = nullptr, float threshold = 1.0f);

		//bool searchImage(QString findName, bool isCap, QString sub_catlog, float threshold = 1.0f);
		//bool searchImage(QString findName, bool isCap, QString sub_catlog, int& imgX, int& imgY, float threshold);

		//

		////QString GetFenLxBoard(bool isOrg = true);

		//QChar Qpiece_to_char(ChinesePieceType chess);
		//bool isSolutionReady(); // 方案是不是OK了

		//int getB90(cv::Point p);
		//bool fillB90(ChinesePieceType b90[], QVector<cv::Point>& plist, ChinesePieceType chess);

		//bool getChessboardHwnd(bool onlyBChe = false);
		//bool  SaveAllPiecePicture();  // 得到所有的棋子信息
		//bool  SaveOnePiecePic(int x, int y, QString chessName);

		//bool captureOne(HWND hw = nullptr, bool isTransHSV = true, int sleepTimeMs = 0);

		//bool searchChess(QString findName, bool isCap = false, SearchWhichCap sWhich = SearchWhichCap::eMain, HWND hw = nullptr);
		//bool searchChess(HWND hw, QString findName, QVector<cv::Point>& res, SearchWhichCap sWhich, bool isCap = false,
		//	float threshold = 1.0f, bool isShow = false);

		//bool char2key(WCHAR ch, LONG& vk_key);
		//void AdbSendText(QString st, bool enter = true);

		//bool searchCountours(HWND hw, QString findName, bool isCap);

		//bool getKingInfo(HWND hw, QString findName, bool isCap);

		//cv::Mat QImage_to_cvMat(const QImage& image, bool inCloneImageData = false);   // 转换格式

		//bool CaptureOneNotry(HWND hw, int sleepTimeMS, bool isTransHSV = true);



		//bool check_window_valid(HWND window, int minW, int minH, window_search_mode mode = INCLUDE_MINIMIZED);
		//inline HWND next_window(HWND window, window_search_mode mode = INCLUDE_MINIMIZED);
		//inline HWND first_window(window_search_mode mode = INCLUDE_MINIMIZED);
		//QString get_window_title(HWND hwnd);
		//QString get_window_class(HWND hwnd);

		//bool isChessBoardWindow(HWND hwnd, stLxBoard* pieceList, bool onlyBche);  // 是不是象棋窗口
		//bool isFindAutoWin();

		//QString getPicturePath();
		//QString getFindPath();
		////QString getHashName(QString orgStr);

		//void initBoard();

		//bool SearchOnChessList(HWND hwnd, QString chess, QVector<cv::Point>& res, SearchWhichCap sWhich, bool IsCap = false);

		////将QImage转化为Mat
		//cv::Mat QImageToCvMat(const QImage& inImage, bool inCloneImageData = true);
		//cv::Mat QPixmapToCvMat(const QPixmap& inPixmap, bool inCloneImageData = true);

		//void winLeftClick(HWND hwnd, int x, int y);

		


	private:

		//volatile bool bMustStop;

		MainWindow* pMain;

		//Chess::Side m_side;

		//bool m_flip;                      // 棋盘是不是翻转了
		//bool m_isAutoClick;               // 是不是自动连线处理

		//cLXinfo m_LxInfo;
		//stLxBoard m_LxBoard[2];
		//Chess::Board *m_board;

		//bool m_Ready_LXset;               // 已有连线设置信息了
		//bool m_connectedBoard_OK;         // 已连接了

		//
		//bool m_bWeMustSendInitFen;                // 已发送初始局面了


		//int m_sleepTimeMs;   // 截图定时
		//float m_precision_auto;   // 匹配精度-图片识别
		//float m_precision_chess;  // 匹配精度-棋子识别
		//bool m_UseAdb;
		//HWND m_hwnd;
		//bool m_chessWinOK;  // 棋谱窗口关闭了	
		//float m_scaleX;
		//float m_scaleY;
		//float m_chessClip;   // 把棋子的边裁剪一些

		//QHash<QString, cv::Mat> m_MatHash;    // 棋子模板
		//QPixmap m_capPixmap;                  // 保存的临时抓图
		//cv::Mat m_image_source;               // 转换好的主图，只有棋盘
		//cv::Mat m_image_source_all;           // 整个图，方便按钮
		//cv::Mat m_image_red;                  // 红方棋子
		//cv::Mat m_image_black;                // 黑方棋子
		//
		//int iLowHred = 0;
		//int iHighHred = 10;

		//int iLowSred = 77;
		//int iHighSred = 255;

		//int iLowVred = 95;
		//int iHighVred = 255;

		//int iLowHblack = 0;
		//int iHighHblack = 51;

		//int iLowSblack = 0;
		//int iHighSblack = 90;

		//int iLowVblack = 0;
		//int iHighVblack = 140;

		//
		//Chess::Board* m_board_second;

		//const static int validWinW = 300;
		//const static int validWinH = 400;

	};

};