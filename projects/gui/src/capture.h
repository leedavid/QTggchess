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
	//	QString m_LX_name;                 // ���ߵ�����
	//	QString m_ParentKeyword;           // �����ڹؼ���
	//	QString m_Parentclass;             // ��������
	//	QString m_titleKeyword;            // ���ڹؼ���
	//	QString m_class;                   // ������
	//	QString m_PieceCatlog;             // ���ӵ�ͼƬĿ¼
	//	float m_offx;					   // ����ԭ��x
	//	float m_offy;                      // ����ԭ��y

	//	float m_dx;                        // ���̸��
	//	float m_dy;                        // ���̸��	

	//	bool m_isAdb;                      // �Ƿ��ֻ�adb����
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
		//bool m_bMainGetFenAlready;                // �ѷ��ͳ�ʼ������
		

		//Chess::Move GetMoveFromBoard();

		void SetCatlogName(QString catName) { this->m_linkBoard->SetCatlogName(catName); };

		void on_start();
		void on_stop();
		void on_pause();

		//stCaptureMsg m_msg;      // �ǲ��ǿ��ܷ���
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

		//// �õ����ߵ���Ϣ
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
		//bool isSolutionReady(); // �����ǲ���OK��

		//int getB90(cv::Point p);
		//bool fillB90(ChinesePieceType b90[], QVector<cv::Point>& plist, ChinesePieceType chess);

		//bool getChessboardHwnd(bool onlyBChe = false);
		//bool  SaveAllPiecePicture();  // �õ����е�������Ϣ
		//bool  SaveOnePiecePic(int x, int y, QString chessName);

		//bool captureOne(HWND hw = nullptr, bool isTransHSV = true, int sleepTimeMs = 0);

		//bool searchChess(QString findName, bool isCap = false, SearchWhichCap sWhich = SearchWhichCap::eMain, HWND hw = nullptr);
		//bool searchChess(HWND hw, QString findName, QVector<cv::Point>& res, SearchWhichCap sWhich, bool isCap = false,
		//	float threshold = 1.0f, bool isShow = false);

		//bool char2key(WCHAR ch, LONG& vk_key);
		//void AdbSendText(QString st, bool enter = true);

		//bool searchCountours(HWND hw, QString findName, bool isCap);

		//bool getKingInfo(HWND hw, QString findName, bool isCap);

		//cv::Mat QImage_to_cvMat(const QImage& image, bool inCloneImageData = false);   // ת����ʽ

		//bool CaptureOneNotry(HWND hw, int sleepTimeMS, bool isTransHSV = true);



		//bool check_window_valid(HWND window, int minW, int minH, window_search_mode mode = INCLUDE_MINIMIZED);
		//inline HWND next_window(HWND window, window_search_mode mode = INCLUDE_MINIMIZED);
		//inline HWND first_window(window_search_mode mode = INCLUDE_MINIMIZED);
		//QString get_window_title(HWND hwnd);
		//QString get_window_class(HWND hwnd);

		//bool isChessBoardWindow(HWND hwnd, stLxBoard* pieceList, bool onlyBche);  // �ǲ������崰��
		//bool isFindAutoWin();

		//QString getPicturePath();
		//QString getFindPath();
		////QString getHashName(QString orgStr);

		//void initBoard();

		//bool SearchOnChessList(HWND hwnd, QString chess, QVector<cv::Point>& res, SearchWhichCap sWhich, bool IsCap = false);

		////��QImageת��ΪMat
		//cv::Mat QImageToCvMat(const QImage& inImage, bool inCloneImageData = true);
		//cv::Mat QPixmapToCvMat(const QPixmap& inPixmap, bool inCloneImageData = true);

		//void winLeftClick(HWND hwnd, int x, int y);

		


	private:

		//volatile bool bMustStop;

		MainWindow* pMain;

		//Chess::Side m_side;

		//bool m_flip;                      // �����ǲ��Ƿ�ת��
		//bool m_isAutoClick;               // �ǲ����Զ����ߴ���

		//cLXinfo m_LxInfo;
		//stLxBoard m_LxBoard[2];
		//Chess::Board *m_board;

		//bool m_Ready_LXset;               // ��������������Ϣ��
		//bool m_connectedBoard_OK;         // ��������

		//
		//bool m_bWeMustSendInitFen;                // �ѷ��ͳ�ʼ������


		//int m_sleepTimeMs;   // ��ͼ��ʱ
		//float m_precision_auto;   // ƥ�侫��-ͼƬʶ��
		//float m_precision_chess;  // ƥ�侫��-����ʶ��
		//bool m_UseAdb;
		//HWND m_hwnd;
		//bool m_chessWinOK;  // ���״��ڹر���	
		//float m_scaleX;
		//float m_scaleY;
		//float m_chessClip;   // �����ӵı߲ü�һЩ

		//QHash<QString, cv::Mat> m_MatHash;    // ����ģ��
		//QPixmap m_capPixmap;                  // �������ʱץͼ
		//cv::Mat m_image_source;               // ת���õ���ͼ��ֻ������
		//cv::Mat m_image_source_all;           // ����ͼ�����㰴ť
		//cv::Mat m_image_red;                  // �췽����
		//cv::Mat m_image_black;                // �ڷ�����
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