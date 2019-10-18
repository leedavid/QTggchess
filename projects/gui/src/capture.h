#pragma once

#include <QString>
#include <QPixmap>

#include <opencv2/opencv.hpp>


class Capture : public QObject
{
	Q_OBJECT

		enum window_search_mode {
		INCLUDE_MINIMIZED,
		EXCLUDE_MINIMIZED
	};

public:

	Capture(float precision, bool UseAdb);
	~Capture();


private:

	float m_precision; // = 0.9f;   // Æ¥Åä¾«¶È
	bool m_UseAdb;
	HWND m_hwnd;


};