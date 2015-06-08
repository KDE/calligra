/* This file is part of the KDE project
   Copyright (C) 2003   Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003   Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2003-2014 Jarosław Staniek <staniek@kde.org>

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

#include "KexiDataTableScrollArea.h"
#include "KexiDataTableScrollArea.h"
#include "KexiDataTableView.h"

#include <KDbConnection>
#include <KDbCursor>

#include <QDebug>

KexiDataTableScrollArea::KexiDataTableScrollArea(QWidget *parent)
        : KexiTableScrollArea(0, parent)
{
    init();
}

KexiDataTableScrollArea::KexiDataTableScrollArea(QWidget *parent, KexiDB::Cursor *cursor)
        : KexiTableScrollArea(0, parent)
{
    init();
    setData(cursor);
}

KexiDataTableScrollArea::~KexiDataTableScrollArea()
{
}

void
KexiDataTableScrollArea::init()
{
    m_cursor = 0;
}

bool KexiDataTableScrollArea::setData(KexiDB::Cursor *cursor)
{
    if (!cursor) {
        clearColumns();
        m_cursor = 0;
        return true;
    }
    if (cursor != m_cursor) {
        clearColumns();
    }
    m_cursor = cursor;

    if (!m_cursor->query()) {
        qWarning() << "Cursor should have query schema defined!\n--aborting setData().\n";
        m_cursor->debug();
        clearColumns();
        return false;
    }

    if (m_cursor->fieldCount() < 1) {
        clearColumns();
        return true;
    }

    if (!m_cursor->isOpened() && !m_cursor->open()) {
        qWarning() << "Cannot open cursor\n--aborting setData(). \n" << m_cursor->serverErrorMsg();
        m_cursor->debug();
        clearColumns();
        return false;
    }

    KexiDB::TableViewData *tv_data = new KexiDB::TableViewData(m_cursor);
    KexiDataTableView* dataTable = qobject_cast<KexiDataTableView*>(parentWidget());
    if (dataTable) {
        dataTable->loadTableViewSettings(tv_data);
    }

    QString windowTitle(m_cursor->query()->caption());
    if (windowTitle.isEmpty())
        windowTitle = m_cursor->query()->name();

    setWindowTitle(windowTitle);

    //PRIMITIVE!! data setting:
    tv_data->preloadAllRows();

    KexiTableScrollArea::setData(tv_data);
    return true;
}

