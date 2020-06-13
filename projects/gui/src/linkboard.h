#pragma once

#include <opencv2/opencv.hpp>
#include <board/board.h>
#include <board/boardfactory.h>

#include <QProcess>
#include <Qthread>
#include <Windows.h>
#include <QPixmap>

#include <chessgame.h>

class MainWindow;

namespace Chess {

	enum class LinkWhich {
		TianTian = 0,     // 天天平台
		KingChess,        // 王者象棋
		OtherChess
	};

	enum class ChinesePieceType
	{
		eNoPice = 0,
		eBPawn,	//!< Pawn
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

	struct stLxBoard {
		QVector<cv::Point> RCheList;		// 红车
		QVector<cv::Point> RMaList;			// 红马
		QVector<cv::Point> RPaoList;		// 红炮
		QVector<cv::Point> RShiList;		// 红仕
		QVector<cv::Point> RXiangList;		// 红相
		QVector<cv::Point> RKingList;		// 红将
		QVector<cv::Point> RPawnList;		// 红兵

		QVector<cv::Point> BCheList;		// 黑车
		QVector<cv::Point> BMaList;			// 黑马
		QVector<cv::Point> BPaoList;		// 黑炮
		QVector<cv::Point> BShiList;		// 黑仕
		QVector<cv::Point> BXiangList;		// 黑相
		QVector<cv::Point> BKingList;		// 黑将
		QVector<cv::Point> BPawnList;		// 黑兵

		ChinesePieceType b90[90];
		Side side;
		QString fen;
	};



	class LinkBoard
	{

	public:
		LinkBoard(MainWindow* pMain, QString catName);
		void setStop(bool s) {
			this->bMustStop = s;
		};
		// 读取棋盘
		void runAutoChess();

	private:

		volatile bool bMustStop;           // 马上停止

		MainWindow* m_pMain;
		HWND m_parentHwnd;                 // 棋盘父窗口
		HWND m_hwnd;                       // 可响应鼠标走棋的窗口

		QString m_catName;                 // 这个就是连线的名称

		//QString m_LX_name;                 // 连线的名称
		QString m_ParentKeyword;           // 父窗口关键词
		QString m_Parentclass;             // 父窗口类
		QString m_titleKeyword;            // 窗口关键词
		QString m_class;                   // 窗口类
		QString m_PieceCatlog;             // 棋子的图片目录
		float m_offx;					   // 棋盘原点x
		float m_offy;                      // 棋盘原点y

		float m_dx;                        // 棋盘格宽
		float m_dy;                        // 棋盘格高	

		bool m_UseAdb;                      // 是否手机adb连接

		float m_precision_chess;           // 棋子识别精度
		float m_precision_auto;            // 图片识别精度

		int m_sleepTimeMs;                 // 棋子识别间隔
		int m_scaleX = 1.0f;               // 棋子比例
		int m_scaleY = 1.0f;               // 

		bool m_Ready_LXset = false;
		bool m_chessWinOK = false;
		bool m_connectedBoard_OK = false; 

		QHash<QString, cv::Mat> m_MatHash;    // 棋子模板
		QPixmap m_capPixmap;                  // 保存的临时抓图
		cv::Mat m_image_source;               // 转换好的主图，只有棋盘
		cv::Mat m_image_source_all;           // 整个图，方便按钮
		//cv::Mat m_image_red;                  // 红方棋子
		//cv::Mat m_image_black;                // 黑方棋子
		cv::Mat m_imgage_SHV[2];              // 红黑
		bool m_isAutoClick;                   // 是不是自动连线处理

		bool m_flip;                       // 棋盘翻转
		Chess::Side m_side;                // 走子方
		stLxBoard m_LxBoard[2];

		int m_iLowHred = 0;                // HSV 区分红黑棋子的参数
		int m_iHighHred = 10;

		int m_iLowSred = 77;
		int m_iHighSred = 255;

		int m_iLowVred = 95;
		int m_iHighVred = 255;

		int m_iLowHblack = 0;
		int m_iHighHblack = 179;

		int m_iLowSblack = 0;
		int m_iHighSblack = 139;

		int m_iLowVblack = 0;
		int m_iHighVblack = 131;

	private:

		bool GetLxInfo(QString catlog);
		bool getChessboardHwnd(bool onlyBChe);        // 得到棋盘句柄

		bool Board2Move(Chess::GenericMove& m);
		bool GetFen(stLxBoard* pList);
		bool fillB90(ChinesePieceType b90[], QVector<cv::Point>& plist, ChinesePieceType chess);
		int getB90(cv::Point p);
		QChar Qpiece_to_char(ChinesePieceType chess);

		bool  SaveAllPiecePicture(QString subCat = nullptr);  // 得到所有的棋子的图片
		bool  SaveOnePiecePic(int x, int y, QString chessName, QString subCat);

		void initBoard();
		void winLeftClick(HWND hwnd, int x, int y, int off_x = 0, int off_y = 0);
		void wait(int msec) {
			QTime dieTime = QTime::currentTime().addMSecs(msec);
			while (QTime::currentTime() < dieTime)
				QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
		}

		bool SearchAndClick(QString findName, bool isCap, QString sub_catlog = nullptr, HWND hw = nullptr, float threshold = 1.0f);

		bool searchCountours(HWND hw, QString findName, bool isCap);

		bool searchImage(QString findName, bool isCap, QString sub_catlog, float threshold = 1.0f);
		bool searchImage(QString findName, bool isCap, QString sub_catlog, int& imgX, int& imgY, float threshold);

		bool captureOne(HWND hw = nullptr, bool isTransHSV = true, int sleepTimeMs = 0);
		bool CaptureOneNotry(HWND hw, int sleepTimeMS, bool isTransHSV = true);

		bool searchChess(HWND hw, QString findName, QVector<cv::Point>& res, Chess::Side side = Chess::Side::White, bool isCap = false,
			float threshold = 1.0f, bool isShow = false);
		//bool SearchOnChessList(HWND hwnd, QString chess, QVector<cv::Point>& res, bool IsCap = false);

		//将QImage转化为Mat
		cv::Mat QImageToCvMat(const QImage& inImage, bool inCloneImageData = true);
		cv::Mat QPixmapToCvMat(const QPixmap& inPixmap, bool inCloneImageData = true);


		QString get_window_title(HWND hwnd);
		QString get_window_class(HWND hwnd);

	};

}