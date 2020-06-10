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

#ifndef GAMEDATABASESEARCHDIALOG_H
#define GAMEDATABASESEARCHDIALOG_H

#include <QDialog>
#include <pgngamefilter.h>

namespace Ui {
	class GameDatabaseSearchDialog;
}

/*!
 * \brief Dialog for searching and filtering game databases.
 *
 * \sa GameDatabaseDialog
 */
class GameDatabaseSearchDialog : public QDialog
{
	Q_OBJECT

	public:
		/*! Constructs a new GameDatabaseSearchDialog. */
		GameDatabaseSearchDialog(QWidget* parent = nullptr);
		/*! Destroys the dialog. */
		virtual ~GameDatabaseSearchDialog();

		/*! Returns the PGN filter. */
		PgnGameFilter filter() const;

	private slots:
		void onResultChanged(int index);

	private:
		Ui::GameDatabaseSearchDialog* ui;
};

#endif // GAMEDATABASESEARCHDIALOG_H
