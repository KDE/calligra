/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2003-2006 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXIDATATABLE_H
#define KEXIDATATABLE_H

#include <widget/dataviewcommon/kexidataawareview.h>

class KexiDataTableView;
class KexiTableView;
class KexiTableViewData;

namespace KexiDB
{
class Cursor;
}

/*! @short Provides a data-driven (record-based) tabular view.

 The KexiDataTable can display data provided "by hand"
 or from KexiDB-compatible database source.
 @see KexiFormView
*/
class KEXIDATATABLE_EXPORT KexiDataTable : public KexiDataAwareView
{
    Q_OBJECT

public:
    /*! CTOR1: Creates, empty table view that can be initialized later
     with setData().
     If \a dbAware is true, table will be db-aware,
      and KexiDataTableView is used internally.
     Otherwise, table will be not-db-aware,
      and KexiTableView is used internally. In the latter case,
      data can be set by calling tableView()->setData(KexiTableViewData* data). */
    KexiDataTable(QWidget *parent, bool dbAware = true);

    /*! CTOR2: Creates db-aware, table view initialized with \a cursor.
     KexiDataTableView is used internally. */
    KexiDataTable(QWidget *parent, KexiDB::Cursor *cursor);

    virtual ~KexiDataTable();

    KexiTableView* tableView() const;

public slots:
    /*! Sets data. Only works for db-aware table. */
    void setData(KexiDB::Cursor *cursor);

protected slots:
//! @todo
    void filter();

protected:
    void init();
};

#endif
