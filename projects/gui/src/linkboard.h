#pragma once
#include <chessgame.h>

#include <opencv2/opencv.hpp>
#include <board/board.h>
#include <board/boardfactory.h>

#include <QProcess>
#include <Qthread>
#include <Windows.h>
#include <QPixmap>
#include <QMutex>



class MainWindow;


namespace Chess {

	class Capture;

	struct stCaptureMsg {

		enum eCapMsg {
			eMove,				// �߲�
			eSetFen,			// ����fen
			eText				// ��ʾ��Ϣ
		};

		eCapMsg mType;
		//Chess::Move m;
		Chess::GenericMove m;
		QString title;
		QString text;
		//ChessGame* pGame;
	};
	Q_DECLARE_METATYPE(stCaptureMsg)

	enum class LinkWhich {
		TianTian = 0,     // ����ƽ̨
		KingChess,        // ��������
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
		QVector<cv::Point> RCheList;		// �쳵
		QVector<cv::Point> RMaList;			// ����
		QVector<cv::Point> RPaoList;		// ����
		QVector<cv::Point> RShiList;		// ����
		QVector<cv::Point> RXiangList;		// ����
		QVector<cv::Point> RKingList;		// �콫
		QVector<cv::Point> RPawnList;		// ���

		QVector<cv::Point> BCheList;		// �ڳ�
		QVector<cv::Point> BMaList;			// ����
		QVector<cv::Point> BPaoList;		// ����
		QVector<cv::Point> BShiList;		// ����
		QVector<cv::Point> BXiangList;		// ����
		QVector<cv::Point> BKingList;		// �ڽ�
		QVector<cv::Point> BPawnList;		// �ڱ�

		ChinesePieceType b90[90];
		Side side;
		QString fen;
	};



	class LinkBoard
	{

		static QMutex mutex;
		static volatile bool m_MayNewGame;

	public:
		LinkBoard(MainWindow* pMain, Capture* pCap, QString catName, bool isAuto);
		void setStop(bool s) {
			this->bMustStop = s;
		};
		// ��ȡ����
		void run();
		
		void ProcessBoardMove(const Chess::GenericMove& move);

		void SetCatlogName(QString catName);

	private:

		void runAutoChess();
		void runAutoClip();

		volatile bool bMustStop;           // ����ֹͣ

		MainWindow* m_pMain;
		Capture* m_pCap; 



		HWND m_parentHwnd;                 // ���̸�����
		HWND m_hwnd;                       // ����Ӧ�������Ĵ���

		QString m_catName;                 // ����������ߵ�����
		bool m_isAutoClick;                // 

		//QString m_LX_name;                 // ���ߵ�����
		QString m_ParentKeyword;           // �����ڹؼ���
		QString m_Parentclass;             // ��������
		QString m_titleKeyword;            // ���ڹؼ���
		QString m_class;                   // ������
		QString m_PieceCatlog;             // ���ӵ�ͼƬĿ¼

		
		float m_offx_che;					   // ���� �� ԭ��x
		float m_offy_che;                      // ���� �� ԭ��y
		float m_offx_board;                    // 
		float m_offy_board;                    // ����ԭ�㣬��Ϊͼ����С��
		cv::Rect m_crect;

		//int moveTime = 0;
		GenericMove m_preMove; 

		float m_dx;                        // ���̸��
		float m_dy;                        // ���̸��	

		bool m_UseAdb;                      // �Ƿ��ֻ�adb����

		float m_precision_chess;           // ����ʶ�𾫶�
		float m_precision_auto;            // ͼƬʶ�𾫶�

		int m_sleepTimeMs;                 // ����ʶ����
		int m_scaleX = 1.0f;               // ���ӱ���
		int m_scaleY = 1.0f;               // 

		bool m_Ready_LXset = false;
		bool m_chessWinOK = false;
		//bool m_connectedBoard_OK = false; 

		QHash<QString, cv::Mat> m_MatHash;    // ����ģ��
		QPixmap m_capPixmap;                  // �������ʱץͼ
		cv::Mat m_image_source;               // ת���õ���ͼ��ֻ������
		cv::Mat m_image_source_all;           // ����ͼ�����㰴ť
		//cv::Mat m_image_red;                  // �췽����
		//cv::Mat m_image_black;                // �ڷ�����
		cv::Mat m_imgage_SHV[2];              // ���
		//bool m_isAutoClick;                   // �ǲ����Զ����ߴ���

		bool m_flip;                       // ���̷�ת
		Chess::Side m_side;                // ���ӷ�
		stLxBoard m_LxBoard[2];

		int m_iLowHred = 0;                // HSV ���ֺ�����ӵĲ���
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

		//bool m_findChild = false;

	private:

		bool CalImageRect();

		bool GetLxBoardChess(int index);
		bool GetLxInfo(QString catlog, bool saveChess = false);
		bool getChessboardHwnd(bool onlyBChe = false, bool getChess = true);        // �õ����̾��

		bool isFindChild(HWND hw);
		void Find_window2(HWND parent, int level);

		bool Board2Move(Chess::GenericMove& m);
		bool GetFen(stLxBoard* pList);
		bool fillB90(ChinesePieceType b90[], QVector<cv::Point>& plist, ChinesePieceType chess);
		int getB90(cv::Point p);
		QChar Qpiece_to_char(ChinesePieceType chess);

		bool  SaveAllPiecePicture(QString subCat = nullptr);  // �õ����е����ӵ�ͼƬ
		bool  SaveOnePiecePic(int x, int y, QString chessName, QString subCat);

		void initBoard();
		void winLeftClick(HWND hwnd, int x, int y, int off_x = 0, int off_y = 0);
		void wait(int msec);

		bool SearchAndClick(QString findName, bool isCap, QString sub_catlog = nullptr, HWND hw = nullptr, float threshold = 1.0f);

		bool searchCountours(HWND hw, QString findName, bool isCap);

		bool searchImage(QString findName, bool isCap, QString sub_catlog, float threshold = 1.0f);
		bool searchImage(QString findName, bool isCap, QString sub_catlog, int& imgX, int& imgY, float threshold);

		bool captureOne(HWND hw = nullptr, bool isTransHSV = true, int sleepTimeMs = 0);
		bool CaptureOneNotry(HWND hw, int sleepTimeMS, bool isTransHSV = true);

		bool searchChess(HWND hw, QString findName, QVector<cv::Point>& res, Chess::Side side = Chess::Side::White, bool isCap = false,
			float threshold = 1.0f, bool isShow = false);
		//bool SearchOnChessList(HWND hwnd, QString chess, QVector<cv::Point>& res, bool IsCap = false);

		//��QImageת��ΪMat
		cv::Mat QImageToCvMat(const QImage& inImage, bool inCloneImageData = true);
		cv::Mat QPixmapToCvMat(const QPixmap& inPixmap, bool inCloneImageData = true);


		QString get_window_title(HWND hwnd);
		QString get_window_class(HWND hwnd);

	};

}



// https://blog.csdn.net/x356982611/article/details/54292930
// �������ռ䣬 Q_DECLARE_METATYPE() ��Ӧ�÷��������ռ�����