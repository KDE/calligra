/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

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

#include <kexidb/cursor.h>

#include "kexidatatableview.h"
#include "kexidatatable.h"

KexiDataTable::KexiDataTable(KexiMainWindow *win, const QString &caption)
 : KexiDialogBase(win, caption)
{
	m_view = new KexiDataTableView(this, "view");
	init();
}

KexiDataTable::KexiDataTable(KexiMainWindow *win, const QString &caption, KexiDB::Cursor *cursor)
 : KexiDialogBase(win, caption)
{
	m_view = new KexiDataTableView(this, "view", cursor);
	init();
}

void KexiDataTable::init()
{
	QVBoxLayout *box = new QVBoxLayout(this);
	box->addWidget(m_view);
	setMinimumSize(m_view->minimumSizeHint().width(),m_view->minimumSizeHint().height());
	resize(m_view->sizeHint());
//js	m_view->show();
	setFocusProxy(m_view);
	m_view->setFocus();
	setIcon(SmallIcon("table"));
	
	initActions();
//js already done in keximainwindow:	registerDialog();
}


KexiDataTable::~KexiDataTable()
{
}

void
KexiDataTable::initActions()
{
	kdDebug()<<"INIT ACTIONS***********************************************************************"<<endl;
	new KAction(i18n("Filter"), "filter", 0, this, SLOT(filter()), actionCollection(), "tablepart_filter");
	setXMLFile("kexidatatableui.rc");

}

void
KexiDataTable::setData(KexiDB::Cursor *c)
{
	m_view->setData(c);
}

void KexiDataTable::filter()
{
}

QWidget* KexiDataTable::mainWidget() 
{ return m_view; }

QSize KexiDataTable::minimumSizeHint() const
{
//	QWidget*const w= (QWidget*const)mainWidget();
	return m_view->minimumSizeHint();
//	return mainWidget() ? mainWidget()->minimumSizeHint() : KMdiChildView::minimumSizeHint();
}

QSize KexiDataTable::sizeHint() const
{
	return m_view->sizeHint();
}

#include "kexidatatable.moc"

