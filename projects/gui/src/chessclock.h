

#ifndef CHESSCLOCK_H
#define CHESSCLOCK_H

#include <QWidget>
#include <QTime>

class QTimerEvent;
class QLabel;

class ChessClock: public QWidget
{
	Q_OBJECT
	
	public:
		ChessClock(QWidget* parent = nullptr);
	
	public slots:
		void setPlayerName(const QString& name);
		void setInfiniteTime(bool infinite);
		void setTime(int totalTime);
		void start(int totalTime);
		void stop();
	
	protected:
		virtual void timerEvent(QTimerEvent* event);
	
	private:
		void stopTimer();

		int m_totalTime;
		int m_timerId;
		bool m_infiniteTime;
		QTime m_time;
		QLabel* m_nameLabel;
		QLabel* m_timeLabel;
		QPalette m_defaultPalette;
};

#endif // CHESSCLOCK
