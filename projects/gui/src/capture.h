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
		eMove,				// �߲�
		eSetFen,			// ����fen
		eText				// ��ʾ��Ϣ
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
			QString m_LX_name;                 // ���ߵ�����
			QString m_titleKeyword;            // ���ڹؼ���
			QString m_class;                   // ������
			QString m_PieceCatlog;             // ���ӵ�ͼƬĿ¼
			float offx;						   // ����ԭ��x
			float offy;                        // ����ԭ��y

			float m_dx;                         // ���̸��
			float m_dy;                         // ���̸��			
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

			int b90[90];			
		    Side side;
			QString fen;
		};


	public:

		explicit Capture(ChessGame* pGame, QObject* parent = nullptr);
		//Capture(float precision, bool UseAdb = false, int sleepMs = 200, float scX = 1.0f, float scY = 1.0f);
		~Capture();

		// �õ����ߵ���Ϣ
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
		bool isSolutionReady(); // �����ǲ���OK��

		int getB90(cv::Point p);
		bool fillB90(int b90[], QVector<cv::Point>& plist, int chess);

		bool getChessboardHwnd(bool onlyBChe = false);
		bool  SaveAllPiecePicture();  // �õ����е�������Ϣ
		bool  SaveOnePiecePic(int x, int y, QString chessName);

		bool captureOne(QString fname = nullptr, HWND hw = nullptr, bool disp = true, int sleepTimeMs = 0,
			QString path = nullptr);

		bool searchImage(QString findName, bool isCap = false, QString mainName = nullptr, HWND hw = nullptr);
		bool searchImage(HWND hw, QString findName, QVector<cv::Point>& res, QString mainName = nullptr, bool isCap = false,
			float threshold = 1.0f, bool isShow = false);

		bool char2key(WCHAR ch, LONG& vk_key);
		void AdbSendText(QString st, bool enter = true);

		cv::Mat QImage_to_cvMat(const QImage& image, bool inCloneImageData = false);   // ת����ʽ

		bool CaptureOneNotry(QString fname, HWND hw, int sleepTimeMS, QString path);



		bool check_window_valid(HWND window, int minW, int minH, window_search_mode mode = INCLUDE_MINIMIZED);
		inline HWND next_window(HWND window, window_search_mode mode = INCLUDE_MINIMIZED);
		inline HWND first_window(window_search_mode mode = INCLUDE_MINIMIZED);
		QString get_window_title(HWND hwnd);
		QString get_window_class(HWND hwnd);

		bool isChessBoardWindow(HWND hwnd, stLxBoard* pieceList, bool onlyBche);  // �ǲ������崰��

		QString getPicturePath();
		QString getFindPath();
		//QString getHashName(QString orgStr);

		void initBoard();

		bool SearchOnChessList(HWND hwnd, QString chess, QVector<cv::Point>& res, bool IsCap = false);

		//��QImageת��ΪMat
		cv::Mat QImageToCvMat(const QImage& inImage, bool inCloneImageData = true);
		cv::Mat QPixmapToCvMat(const QPixmap& inPixmap, bool inCloneImageData = true);


	private:


		cLXinfo m_LxInfo;
		stLxBoard m_LxBoard[2];
		Chess::Board* m_board;

		bool m_Ready_LXset;               // ��������������Ϣ��
		bool m_connectedBoard_OK;         // ��������

		bool bMustStop;
		bool bSendInitFen;                // �ѷ��ͳ�ʼ������


		int m_sleepTimeMs;   // ��ͼ��ʱ
		float m_precision;   // ƥ�侫��
		bool m_UseAdb;
		HWND m_hwnd;
		bool m_chessWinOK;  // ���״��ڹر���	
		float m_scaleX;
		float m_scaleY;
		float m_chessClip;   // �����ӵı߲ü�һЩ

		QHash<QString, cv::Mat> m_MatHash;
		QPixmap m_capPixmap;      // �������ʱץͼ
		cv::Mat m_image_source;   // ת���õ���ͼ

		
		//Chess::Board* m_board_second;

		const static int validWinW = 500;
		const static int validWinH = 500;

	};

};