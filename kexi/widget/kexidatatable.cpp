/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qlayout.h>
#include <qlabel.h>

#include <kiconloader.h>
#include <klocale.h>
#include <kaction.h>
#include <kpopupmenu.h>

#include <kexidb/cursor.h>

#include "kexidatatableview.h"
#include "kexidatatable.h"
#include "kexidialogbase.h"

//KexiDataTable::KexiDataTable(KexiMainWindow *win, const QString &caption)
KexiDataTable::KexiDataTable(KexiMainWindow *mainWin, QWidget *parent, 
	const char *name, bool dbAware)
 : KexiViewBase(mainWin, parent, name)
 , m_view(0)
{
	if (dbAware)
		m_view = new KexiDataTableView(this, "view");
	else
		m_view = new KexiTableView(0, this, "view");
	init();
}

//KexiDataTable::KexiDataTable(KexiMainWindow *win, KexiDB::Cursor *cursor, 
//	const QString &caption)
KexiDataTable::KexiDataTable(KexiMainWindow *mainWin, QWidget *parent, 
	KexiDB::Cursor *cursor, const char *name)
 : KexiViewBase(mainWin, parent, name)
 , m_view(0)
{
	m_view = new KexiDataTableView(this, "view", cursor);
	init();
}

void KexiDataTable::init()
{
	connect(m_view, SIGNAL(cellSelected(int,int)), this, SLOT(slotCellSelected(int,int)));
	//! before closing - we'are accepting editing
	connect(this,SIGNAL(closing()),m_view,SLOT(acceptRowEdit()));

	//! updating actions on start/stop editing
	connect(m_view, SIGNAL(rowEditStarted(int)), this, SLOT(slotUpdateRowActions(int)));
	connect(m_view, SIGNAL(rowEditTerminated(int)), this, SLOT(slotUpdateRowActions(int)));
	connect(m_view, SIGNAL(reloadActions()), this, SLOT(reloadActions()));

	QVBoxLayout *box = new QVBoxLayout(this);
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
//	setFocusProxy(m_view);
	m_view->setFocus();
//not needed	setIcon(SmallIcon("table"));
	
	initActions();
//js already done in keximainwindow:	registerDialog();
	reloadActions();
}

KexiDataTable::~KexiDataTable()
{
}

void
KexiDataTable::initActions()
{
	plugSharedAction("edit_delete_row", m_view, SLOT(deleteCurrentRow()));
	m_view->plugSharedAction(sharedAction("edit_delete_row")); //for proper shortcut

	plugSharedAction("edit_delete",m_view, SLOT(deleteAndStartEditCurrentCell()));
	m_view->plugSharedAction(sharedAction("edit_delete")); //for proper shortcut

	plugSharedAction("data_save_row",m_view, SLOT(acceptRowEdit()));
	m_view->plugSharedAction(sharedAction("data_save_row")); //for proper shortcut
}

void KexiDataTable::reloadActions()
{
//	m_view->initActions(guiClient()->actionCollection());
//warning FIXME Move this to the table part
/*
	kdDebug()<<"INIT ACTIONS***********************************************************************"<<endl;
	new KAction(i18n("Filter"), "filter", 0, this, SLOT(filter()), actionCollection(), "tablepart_filter");
	setXMLFile("kexidatatableui.rc");
*/
	m_view->popup()->clear();


	if (m_view->isEmptyRowInsertingEnabled()) {
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

	slotCellSelected( m_view->currentColumn(), m_view->currentRow() );
}

void
KexiDataTable::setData(KexiDB::Cursor *c)
{
	if (!m_view->isA("KexiDataTableView"))
		return;
	static_cast<KexiDataTableView*>(m_view)->setData(c);
}

void KexiDataTable::filter()
{
}

QWidget* KexiDataTable::mainWidget() 
{
	return m_view;
}

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

// update actions --------------

void KexiDataTable::slotCellSelected(int col, int row)
{
	slotUpdateRowActions(row);
}

void KexiDataTable::slotUpdateRowActions(int row)
{
	setAvailable("edit_delete_row", !m_view->isReadOnly() && !(m_view->isInsertingEnabled() && row==m_view->rows()) );
	setAvailable("edit_insert_empty_row", m_view->isEmptyRowInsertingEnabled());
	setAvailable("data_save_row", m_view->rowEditing());
}

#include "kexidatatable.moc"

