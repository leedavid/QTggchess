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

#ifndef IMPORT_PROGRESS_DIALOG_H
#define IMPORT_PROGRESS_DIALOG_H

#include <QDialog>

class PgnImporter;

namespace Ui {
	class ImportProgressDialog;
}

/*!
 * \brief Dialog for PGN database import progress.
 *
 * \sa PgnImporter
 */
class ImportProgressDialog : public QDialog
{
	Q_OBJECT

	public:
		/*! Constructs a new ImportProgressDialog with \a importer. */
		ImportProgressDialog(PgnImporter* pgnImporter,
				     QWidget* parent = nullptr);
		/*! Destroys the dialog. */
		virtual ~ImportProgressDialog();

	private slots:
		void onImporterFinished();
		void onImportError(int error);
		void updateImportStatus(const QTime& startTime, int numReadGames, qint64 numReadBytes);

	private:
		QString humaniseTime(int sec);
		qint64 m_totalFileSize;
		int m_lastUpdateSecs;
		bool m_importError;
		Ui::ImportProgressDialog* ui;
};

#endif // IMPORT_PROGRESS_DIALOG_H
