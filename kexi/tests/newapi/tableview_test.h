/* This file is part of the KDE project
   Copyright (C) 2003 Jarosław Staniek <staniek@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef TABLEVIEW_TEST_H
#define TABLEVIEW_TEST_H

#include <widget/tableview/kexidatatableview.h>
#include <kexidb/cursor.h>

#include <QDesktopWidget>

int tableViewTest()
{
    if (!conn->useDatabase(db_name)) {
        conn->debugError();
        return 1;
    }

    KexiDB::TableSchema *persons = conn->tableSchema("persons");
    if (!persons) {
        conn->debugError();
        kDebug() << "tableViewTest(): !persons";
        return 1;
    }

// KexiTableView *tv = new KexiTableView(0, "tv", /*KexiTableList *contents=*/0);
// KexiDB::Cursor *cursor = conn->executeQuery( "select * from persons", KexiDB::Cursor::Buffered );
    KexiDB::Cursor *cursor = conn->prepareQuery(*persons , cursor_options);  //KexiDB::Cursor::Buffered );
    if (!cursor) {
        conn->debugError();
        kDebug() << "tableViewTest(): !cursor";
        return 1;
    }

    KexiDataTableView *tv = new KexiDataTableView(0, cursor);

    app->setMainWidget(tv);
    tv->move((qApp->desktop()->width() - tv->width()) / 2, (qApp->desktop()->height() - tv->height()) / 2);
    tv->show();
    tv->setFocus();

    return 0;
}

#endif

