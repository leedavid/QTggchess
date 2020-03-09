#pragma once
#include <qdialog.h>
#include "src/boardview/BasicBoardScene.h"
#include "pgngame.h"

namespace Ui {
	class BoardEditorDialog;
}

class BoardEditorDlg :
	public QDialog
{
	Q_OBJECT

public:
	explicit BoardEditorDlg(Chess::Board* board, QWidget* parrent = nullptr);
	~BoardEditorDlg();
	Ui::BoardEditorDialog* ui;

	QString fenString() const;

public slots:
	void initBoard(int idx);
	void setFlip(int idx);
	void setRedFirst(int idx);

private:
	BasicBoardScene* boardscene;
	PgnGame* pgnGame;
	int currentFlipIdx = 0;
};

