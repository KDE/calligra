/* This file is part of the KDE project
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexialtertabledialog.h"

#include <qlayout.h>
#include <qlabel.h>

#include <kiconloader.h>
#include <klocale.h>
#include <kaction.h>
#include <kpopupmenu.h>

#include <kexidb/cursor.h>
#include <kexidb/tableschema.h>
#include <kexidb/connection.h>

#include "kexidatatableview.h"

KexiAlterTableDialog::KexiAlterTableDialog(KexiMainWindow *win, QWidget *parent, 
	KexiDB::TableSchema &table, const char *name)
 : KexiViewBase(win, parent, name)
{
	m_table = &table;
	m_view = new KexiDataTableView(this, "view");
	init();
}

void KexiAlterTableDialog::init()
{
/*
	connect(m_view, SIGNAL(cellSelected(int,int)), this, SLOT(slotCellSelected(int,int)));
	//! before closing - we'are accepting editing
	connect(this,SIGNAL(closing()),m_view,SLOT(acceptRowEdit()));

	//! updating actions on start/stop editing
	connect(m_view, SIGNAL(rowEditStarted(int)), this, SLOT(slotUpdateRowActions(int)));
	connect(m_view, SIGNAL(rowEditTerminated(int)), this, SLOT(slotUpdateRowActions(int)));

*/
	QVBoxLayout *box = new QVBoxLayout(this);
	box->addWidget(m_view);
	setMinimumSize(m_view->minimumSizeHint().width(),m_view->minimumSizeHint().height());
	resize(m_view->sizeHint());
	m_view->setFocus();
	initActions();
}

KexiAlterTableDialog::~KexiAlterTableDialog()
{
}

void
KexiAlterTableDialog::initActions()
{
/*
	plugSharedAction("edit_delete_row", m_view, SLOT(deleteCurrentRow()));
	plugSharedAction("edit_delete_row", m_view->popup());
	m_view->plugSharedAction(sharedAction("edit_delete_row")); //for proper shortcut

	plugSharedAction("edit_delete",m_view, SLOT(deleteAndStartEditCurrentCell()));
	m_view->plugSharedAction(sharedAction("edit_delete")); //for proper shortcut

	plugSharedAction("data_save_row",m_view, SLOT(acceptRowEdit()));
	m_view->plugSharedAction(sharedAction("data_save_row")); //for proper shortcut

	slotCellSelected( m_view->currentColumn(), m_view->currentRow() );
	*/
}

QWidget* KexiAlterTableDialog::mainWidget() 
{
	return m_view;
}

QSize KexiAlterTableDialog::minimumSizeHint() const
{
//	QWidget*const w= (QWidget*const)mainWidget();
	return m_view->minimumSizeHint();
//	return mainWidget() ? mainWidget()->minimumSizeHint() : KMdiChildView::minimumSizeHint();
}

QSize KexiAlterTableDialog::sizeHint() const
{
	return m_view->sizeHint();
}

// update actions --------------

/*
void KexiAlterTableDialog::slotCellSelected(int col, int row)
{
	slotUpdateRowActions(row);
}

void KexiAlterTableDialog::slotUpdateRowActions(int row)
{
	setAvailable("edit_delete_row", !m_view->isReadOnly() && !(m_view->isInsertingEnabled() && row==m_view->rows()) );
	setAvailable("data_save_row", m_view->rowEditing());
}*/


#include "kexialtertabledialog.moc"

