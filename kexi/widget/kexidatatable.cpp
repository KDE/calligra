/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2003-2006 Jaroslaw Staniek <js@iidea.pl>

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
}

KexiDataTable::KexiDataTable(KexiMainWindow *mainWin, QWidget *parent, 
	KexiDB::Cursor *cursor, const char *name)
 : KexiDataAwareView( mainWin, parent, name )
{
	KexiTableView *view = new KexiDataTableView(this, "view", cursor);
	KexiDataAwareView::init( view, view, view );
}

KexiDataTable::~KexiDataTable()
{
}

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

KexiTableView* KexiDataTable::tableView() const
{
	return dynamic_cast<KexiTableView*>(m_internalView);
}

#include "kexidatatable.moc"
