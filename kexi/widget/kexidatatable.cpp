/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2003-2005 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/


#include <qlayout.h>
#include <qlabel.h>
//Added by qt3to4:
#include <Q3VBoxLayout>

#include <kiconloader.h>
#include <klocale.h>
#include <kaction.h>
#include <kmenu.h>

#include <kexidb/cursor.h>

#include "kexidatatableview.h"
#include "kexidatatable.h"
#include "kexidialogbase.h"

KexiDataTable::KexiDataTable(KexiMainWindow *mainWin, QWidget *parent, 
	const char *name, bool dbAware)
// : KexiViewBase(mainWin, parent, name)
 : KexiDataAwareView( mainWin, parent, name )
{
	KexiTableView *view;
	if (dbAware)
		view = new KexiDataTableView(this, 
			QString("%1_datatableview").arg(name ? name : "KexiDataTableView").latin1());
	else
		view = new KexiTableView(0, this, 
			QString("%1_tableview").arg(name ? name : "KexiTableView").latin1());

	KexiDataAwareView::init( view, view, view );
//	init();
}

//KexiDataTable::KexiDataTable(KexiMainWindow *win, KexiDB::Cursor *cursor, 
//	const QString &caption)
KexiDataTable::KexiDataTable(KexiMainWindow *mainWin, QWidget *parent, 
	KexiDB::Cursor *cursor, const char *name)
 : KexiDataAwareView( mainWin, parent, name )
// : KexiViewBase(mainWin, parent, name)
// , m_view(0)
{
//	m_view = new KexiDataTableView(this, "view", cursor);
	KexiTableView *view = new KexiDataTableView(this, "view", cursor);
	KexiDataAwareView::init( view, view, view );
//	init();
}

KexiDataTable::~KexiDataTable()
{
}

#if 0 //moved

void KexiDataTable::init()
{
	connect(m_view, SIGNAL(cellSelected(int,int)), this, SLOT(slotCellSelected(int,int)));
	//! before closing - we'are accepting editing
	connect(this,SIGNAL(closing()),m_view,SLOT(acceptRowEdit()));

	//! updating actions on start/stop editing
	connect(m_view, SIGNAL(rowEditStarted(int)), this, SLOT(slotUpdateRowActions(int)));
	connect(m_view, SIGNAL(rowEditTerminated(int)), this, SLOT(slotUpdateRowActions(int)));
	connect(m_view, SIGNAL(reloadActions()), this, SLOT(reloadActions()));

	Q3VBoxLayout *box = new Q3VBoxLayout(this);
	box->addWidget(m_view);

	//resize to preferred size
/*	QSize preferredSize = m_view->sizeHint();
	KexiDialogBase* dlg = parentDialog();
	if (dlg && dlg->mdiParent()) {
		QRect r = dlg->mdiParent()->mdiAreaContentsRect();
		preferredSize = preferredSize.boundedTo( QSize(
			r.width() - 10,
			r.height() - dlg->mdiParent()->captionHeight() - 10
		) );
	}*/

	setMinimumSize(m_view->minimumSizeHint().width(),m_view->minimumSizeHint().height());
	resize( preferredSizeHint( m_view->sizeHint() ) );
//js	m_view->show();
	setFocusProxy(m_view);
//	m_view->setFocus();
//not needed	setIcon(SmallIcon("table"));
	
	initActions();
//js already done in keximainwindow:	registerDialog();
	reloadActions();
}

void
KexiDataTable::initActions()
{
	plugSharedAction("edit_delete_row", m_view, SLOT(deleteCurrentRow()));
	m_view->plugSharedAction(sharedAction("edit_delete_row")); //for proper shortcut

	plugSharedAction("edit_delete",m_view, SLOT(deleteAndStartEditCurrentCell()));
	m_view->plugSharedAction(sharedAction("edit_delete")); //for proper shortcut

	plugSharedAction("edit_edititem",m_view, SLOT(startEditOrToggleValue()));
	m_view->plugSharedAction(sharedAction("edit_edititem")); //for proper shortcut

	plugSharedAction("data_save_row",m_view, SLOT(acceptRowEdit()));
	m_view->plugSharedAction(sharedAction("data_save_row")); //for proper shortcut

	plugSharedAction("data_cancel_row_changes",m_view, SLOT(cancelRowEdit()));
	m_view->plugSharedAction(sharedAction("data_cancel_row_changes")); //for proper shortcut

	if (m_view->isSortingEnabled()) {
		plugSharedAction("data_sort_az", m_view, SLOT(sortAscending()));
		plugSharedAction("data_sort_za", m_view, SLOT(sortDescending()));
	}

	m_view->plugSharedAction(sharedAction("edit_insert_empty_row")); //for proper shortcut

	setAvailable("data_sort_az", m_view->isSortingEnabled());
	setAvailable("data_sort_za", m_view->isSortingEnabled());
//! \todo 	plugSharedAction("data_filter", this, SLOT(???()));
}

void KexiDataTable::reloadActions()
{
//	m_view->initActions(guiClient()->actionCollection());
//warning FIXME Move this to the table part
/*
	kDebug()<<"INIT ACTIONS***********************************************************************"<<endl;
	new KAction(i18n("Filter"), "search-filter", 0, this, SLOT(filter()), actionCollection(), "tablepart_filter");
	setXMLFile("kexidatatableui.rc");
*/
	m_view->popup()->clear();

	unplugSharedAction("edit_clear_table");
	plugSharedAction("edit_clear_table", this, SLOT(deleteAllRows()));

	if (m_view->isEmptyRowInsertingEnabled()) {
		unplugSharedAction("edit_insert_empty_row");
		plugSharedAction("edit_insert_empty_row", m_view, SLOT(insertEmptyRow()));
		plugSharedAction("edit_insert_empty_row", m_view->popup());
	}
	else {
		unplugSharedAction("edit_insert_empty_row");
		unplugSharedAction("edit_insert_empty_row", m_view->popup());
	}

	if (m_view->isDeleteEnabled())
		plugSharedAction("edit_delete_row", m_view->popup());
	else
		unplugSharedAction("edit_delete_row", m_view->popup());

	//if (!m_view->isSortingEnabled()) {
//		unplugSharedAction("data_sort_az");
//		unplugSharedAction("data_sort_za");
	//}
	setAvailable("data_sort_az", m_view->isSortingEnabled());
	setAvailable("data_sort_za", m_view->isSortingEnabled());

	slotCellSelected( m_view->currentColumn(), m_view->currentRow() );
}
#endif

/*moved
void
KexiDataTable::updateActions(bool activated)
{
	setAvailable("data_sort_az", m_view->isSortingEnabled());
	setAvailable("data_sort_za", m_view->isSortingEnabled());
	KexiViewBase::updateActions(activated);
}*/

void
KexiDataTable::setData(KexiDB::Cursor *c)
{
	if (!dynamic_cast<KexiDataTableView*>(mainWidget()))
		return;
	dynamic_cast<KexiDataTableView*>(mainWidget())->setData(c);
}

void KexiDataTable::filter()
{
}

/*moved
QSize KexiDataTable::minimumSizeHint() const
{
//	QWidget*const w= (QWidget*const)mainWidget();
	return m_view ? m_view->minimumSizeHint() : KexiViewBase::minimumSizeHint();
//	return mainWidget() ? mainWidget()->minimumSizeHint() : KMdiChildView::minimumSizeHint();
}

QSize KexiDataTable::sizeHint() const
{
	return m_view ? m_view->sizeHint() : KexiViewBase::sizeHint();
}
*/

// update actions --------------
#if 0
void KexiDataTable::slotCellSelected(int /*col*/, int row)
{
	slotUpdateRowActions(row);
}

void KexiDataTable::deleteAllRows()
{
	m_view->deleteAllRows(true/*ask*/, true/*repaint*/);
}

#endif

/*moved
void KexiDataTable::slotUpdateRowActions(int row)
{
	setAvailable("edit_delete", !m_view->isReadOnly() && !(m_view->isInsertingEnabled() && row==m_view->rows()));
	setAvailable("edit_delete_row", !m_view->isReadOnly() && !(m_view->isDeleteEnabled() && row==m_view->rows()) );
	setAvailable("edit_insert_empty_row", !m_view->isReadOnly() && m_view->isEmptyRowInsertingEnabled());
	setAvailable("edit_clear_table", !m_view->isReadOnly() && m_view->isDeleteEnabled() && m_view->rows()>0);
	setAvailable("data_save_row", m_view->rowEditing());
	setAvailable("data_cancel_row_changes", m_view->rowEditing());
	setAvailable("data_sort_az", m_view->isSortingEnabled());
	setAvailable("data_sort_za", m_view->isSortingEnabled());
}*.
*/

KexiTableView* KexiDataTable::tableView() const
{
	return dynamic_cast<KexiTableView*>(m_internalView);
}

#include "kexidatatable.moc"
