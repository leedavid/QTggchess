#pragma once


#include <Windows.h>
#include <QString>
#include <QPixmap>
#include <QProcess>
#include <Qthread>
#include <QMessagebox>

#include <opencv2/opencv.hpp>
#include <board/board.h>
#include <board/boardfactory.h>
#include <chessgame.h>

struct stCaptureMsg {

	enum eCapMsg {
		eMove,				// 走步
		eSetFen,			// 设置fen
		eText				// 提示信息
	};

	eCapMsg mType;
	Chess::Move m;
	QString title;
	QString text;
	//ChessGame* pGame;
};

namespace Chess {

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
	
		
	private:

		void run() Q_DECL_OVERRIDE;

		struct cLXinfo {

		public:
			//cLXinfo();
			//~cLXinfo();
		public:
			QString m_LX_name;                 // 连线的名称
			QString m_titleKeyword;            // 窗口关键词
			QString m_class;                   // 窗口类
			QString m_PieceCatlog;             // 棋子的图片目录
			float offx;						   // 棋盘原点x
			float offy;                        // 棋盘原点y

			float m_dx;                         // 棋盘格宽
			float m_dy;                         // 棋盘格高			
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

			int b90[90];			
		    Side side;
			QString fen;
		};


	public:

		explicit Capture(ChessGame* pGame, QObject* parent = nullptr);
		//Capture(float precision, bool UseAdb = false, int sleepMs = 200, float scX = 1.0f, float scY = 1.0f);
		~Capture();

		// 得到连线的信息
		bool GetLxInfo(QString catlog);

		bool GetLxBoardChess(bool org = true);

		bool GetFen(stLxBoard* pList);

		Chess::Move GetMoveFromBoard();

		void on_start();
		void on_stop();
		void on_pause();

	private:

		stCaptureMsg m_msg;
		void SendMessageToMain(const QString title, const QString msg);
		void SendMoveToMain(const Chess::Move m);
		void SendFenToMain(const QString fen);

		//QString GetFenLxBoard(bool isOrg = true);

		QChar Qpiece_to_char(int chess);
		bool isSolutionReady(); // 方案是不是OK了

		int getB90(cv::Point p);
		bool fillB90(int b90[], QVector<cv::Point>& plist, int chess);

		bool getChessboardHwnd(bool onlyBChe = false);
		bool  SaveAllPiecePicture();  // 得到所有的棋子信息
		bool  SaveOnePiecePic(int x, int y, QString chessName);

		bool captureOne(QString fname = nullptr, HWND hw = nullptr, bool disp = true, int sleepTimeMs = 0,
			QString path = nullptr);

		bool searchImage(QString findName, bool isCap = false, QString mainName = nullptr, HWND hw = nullptr);
		bool searchImage(HWND hw, QString findName, QVector<cv::Point>& res, QString mainName = nullptr, bool isCap = false,
			float threshold = 1.0f, bool isShow = false);

		bool char2key(WCHAR ch, LONG& vk_key);
		void AdbSendText(QString st, bool enter = true);

		cv::Mat QImage_to_cvMat(const QImage& image, bool inCloneImageData = false);   // 转换格式

		bool CaptureOneNotry(QString fname, HWND hw, int sleepTimeMS, QString path);



		bool check_window_valid(HWND window, int minW, int minH, window_search_mode mode = INCLUDE_MINIMIZED);
		inline HWND next_window(HWND window, window_search_mode mode = INCLUDE_MINIMIZED);
		inline HWND first_window(window_search_mode mode = INCLUDE_MINIMIZED);
		QString get_window_title(HWND hwnd);
		QString get_window_class(HWND hwnd);

		bool isChessBoardWindow(HWND hwnd, stLxBoard* pieceList, bool onlyBche);  // 是不是象棋窗口

		QString getPicturePath();
		QString getFindPath();
		//QString getHashName(QString orgStr);

		void initBoard();

		bool SearchOnChessList(HWND hwnd, QString chess, QVector<cv::Point>& res, bool IsCap = false);

		//将QImage转化为Mat
		cv::Mat QImageToCvMat(const QImage& inImage, bool inCloneImageData = true);
		cv::Mat QPixmapToCvMat(const QPixmap& inPixmap, bool inCloneImageData = true);


	private:


		cLXinfo m_LxInfo;
		stLxBoard m_LxBoard[2];
		Chess::Board* m_board;

		bool m_Ready_LXset;               // 已有连线设置信息了
		bool m_connectedBoard_OK;         // 已连接了

		bool bMustStop;
		bool bSendInitFen;                // 已发送初始局面了


		int m_sleepTimeMs;   // 截图定时
		float m_precision;   // 匹配精度
		bool m_UseAdb;
		HWND m_hwnd;
		bool m_chessWinOK;  // 棋谱窗口关闭了	
		float m_scaleX;
		float m_scaleY;
		float m_chessClip;   // 把棋子的边裁剪一些

		QHash<QString, cv::Mat> m_MatHash;
		QPixmap m_capPixmap;      // 保存的临时抓图
		cv::Mat m_image_source;   // 转换好的主图

		
		//Chess::Board* m_board_second;

		const static int validWinW = 500;
		const static int validWinH = 500;

	};

};