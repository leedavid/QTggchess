
#pragma execution_character_set("utf-8")

#include <QAbstractItemView>
#include <QScrollBar>

#include "autoverticalscroller.h"

AutoVerticalScroller::AutoVerticalScroller(QAbstractItemView* view,
                                           QObject* parent)
	: QObject(parent), m_view(view), m_scrollToBottom(false)
{
	connect(m_view->model(),
		SIGNAL(rowsAboutToBeInserted(const QModelIndex&, int, int)),
		this, SLOT(onRowsAboutToBeInserted()));
	connect(m_view->model(), SIGNAL(rowsInserted(const QModelIndex&, int, int)),
		this, SLOT(onRowsInserted()));
}

void AutoVerticalScroller::onRowsAboutToBeInserted()
{
	m_scrollToBottom = m_view->verticalScrollBar()->value() ==
		m_view->verticalScrollBar()->maximum();
}

void AutoVerticalScroller::onRowsInserted()
{
	if (m_scrollToBottom)
		m_view->scrollToBottom();
}
