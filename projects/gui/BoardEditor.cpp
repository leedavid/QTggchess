#include "BoardEditor.h"
#include "ui_boardeditor.h"
#include <qpushbutton.h>
#include <qdebug.h>

BoardEditorDlg::BoardEditorDlg(Chess::Board* board, QWidget* parrent) :
	QDialog(parrent),
	ui(new Ui::BoardEditorDialog) {
	ui->setupUi(this);
	boardscene = new BasicBoardScene(this);
	ui->boardView->setScene(boardscene);
	boardscene->setBoard(board);
	boardscene->populate(1);
	ui->comboBoxInitialStatus->setCurrentIndex(1);
	ui->boardView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui->boardView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui->boardView->setRenderHint(QPainter::Antialiasing);
	ui->boardView->setMouseTracking(true);
	ui->boardView->setMinimumSize(boardscene->sceneRect().size().toSize());
	ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("确定"));
	ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("取消"));

	connect(ui->comboBoxInitialStatus, SIGNAL(currentIndexChanged(int)), this, SLOT(initBoard(int)));
	connect(ui->comboBoxFlipSide, SIGNAL(currentIndexChanged(int)), this, SLOT(setFlip(int)));
	connect(ui->comboBoxTurnSide, SIGNAL(currentIndexChanged(int)), this, SLOT(setRedFirst(int)));
}

BoardEditorDlg::~BoardEditorDlg() {
	delete ui;
}

QString BoardEditorDlg::fenString() const
{
	return boardscene->fenString();
}

void BoardEditorDlg::initBoard(int idx) {
	boardscene->populate(idx);
}

void BoardEditorDlg::setFlip(int idx) {
	if (currentFlipIdx != idx) {
		boardscene->flip();
		currentFlipIdx = idx;
	}
}

void BoardEditorDlg::setRedFirst(int idx) {
	boardscene->setRedFirst(idx == 0);
}