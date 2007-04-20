/* This file is part of the KDE project
   Copyright (C) 2003   Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003   Joseph Wenninger <jowenn@kde.org>
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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <qtimer.h>
#include <qapplication.h>

#include <kmessagebox.h>
#include <klocale.h>
#include <kdebug.h>
#include <kaction.h>

#include <kexidb/connection.h>
#include <kexidb/cursor.h>

#include "kexidatatableview.h"


KexiDataTableView::KexiDataTableView(QWidget *parent)
 : KexiTableView(0, parent)
{
	init();
}

KexiDataTableView::KexiDataTableView(QWidget *parent, KexiDB::Cursor *cursor)
 : KexiTableView(0, parent)
{
	init();
	setData(cursor);
}

KexiDataTableView::~KexiDataTableView()
{
}

void
KexiDataTableView::init()
{
	m_cursor = 0;

//	m_maxRecord = 0;
//	m_records = 0;
//	m_first = false;

//	connect(this, SIGNAL(contentsMoving(int, int)), this, SLOT(slotMoving(int)));
//	connect(verticalScrollBar(), SIGNAL(sliderMoved(int)), this, SLOT(slotMoving(int)));
}

/*void KexiDataTableView::initActions(KActionCollection *col)
{
	KexiTableView::initActions(col);
	new KAction(i18n("Filter"), "search-filter", 0, this, SLOT(filter()), col, "tablepart_filter");
}*/

bool KexiDataTableView::setData(KexiDB::Cursor *cursor)
{
//js	if (!m_first)
//js		clearColumns();
	if (!cursor) {
		clearColumns();
		m_cursor = 0;
		return true;
	}
	if (cursor!=m_cursor) {
		clearColumns();
	}
	m_cursor = cursor;

	if (!m_cursor->query()) {
		kDebug() << "KexiDataTableView::setData(): WARNING: cursor should have query schema defined!\n--aborting setData()." << endl;
		m_cursor->debug();
		clearColumns();
		return false;
	}

	if (m_cursor->fieldCount()<1) {
		clearColumns();
		return true;
	}

	if (!m_cursor->isOpened() && !m_cursor->open()) {
		kDebug() << "KexiDataTableView::setData(): WARNING: cannot open cursor\n--aborting setData(). \n" <<
			m_cursor->serverErrorMsg() << endl;
		m_cursor->debug();
		clearColumns();
		return false;
	}

	KexiTableViewData *tv_data = new KexiTableViewData(m_cursor);

	QString caption = m_cursor->query()->caption();
	if (caption.isEmpty())
		caption = m_cursor->query()->name();

	setCaption( caption );

	//PRIMITIVE!! data setting:
	tv_data->preloadAllRows();

	KexiTableView::setData(tv_data);
	return true;
}

#include "kexidatatableview.moc"
