/*
    This file is part of GGzero Chess.
    Copyright (C) 2008-2018 GGzero Chess authors

    GGzero Chess is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    GGzero Chess is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with GGzero Chess.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef GAMESETTINGSWIDGET_H
#define GAMESETTINGSWIDGET_H

#include <QWidget>
#include <timecontrol.h>
#include <gameadjudicator.h>

namespace Ui {
	class GameSettingsWidget;
}
namespace Chess {
	class Board;
}
class EngineConfiguration;
class OpeningSuite;
class OpeningBook;

class GameSettingsWidget : public QWidget
{
	Q_OBJECT

	public:
		explicit GameSettingsWidget(QWidget *parent = nullptr);
		virtual ~GameSettingsWidget();

		QString chessVariant() const;
		TimeControl timeControl() const;
		bool pondering() const;
		GameAdjudicator adjudicator() const;
		OpeningSuite* openingSuite() const;
		int openingSuiteDepth() const;
		OpeningBook* openingBook() const;
		int bookDepth() const;
		bool isValid() const;

		void applyEngineConfiguration(EngineConfiguration* config);
		void enableSettingsUpdates();

	public slots:
		void onHumanCountChanged(int count);

	signals:
		void variantChanged(const QString& variant);
		void timeControlChanged();
		void statusChanged(bool ok);

	private slots:
		void validateFen(const QString& fen);
		void showTimeControlDialog();

	private:
		void readSettings();

		Ui::GameSettingsWidget *ui;
		TimeControl m_timeControl;
		Chess::Board* m_board;
		QPalette m_defaultPalette;
		bool m_isValid;
};

#endif // GAMESETTINGSWIDGET_H
