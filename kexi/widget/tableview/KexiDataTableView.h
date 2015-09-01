/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>
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

#ifndef KEXIDATATABLE_H
#define KEXIDATATABLE_H

#include "kexidatatable_export.h"
#include <widget/dataviewcommon/kexidataawareview.h>

class KDbCursor;
class KDbTableViewData;
class KexiTableScrollArea;

/*! @short Provides a data-driven (record-based) tabular view.

 The KexiDataTableView can display data provided "by hand"
 or from KexiDB-compatible database source.
 @see KexiFormView
*/
class KEXIDATATABLE_EXPORT KexiDataTableView : public KexiDataAwareView
{
    Q_OBJECT

public:
    /*! CTOR1: Creates, empty table view that can be initialized later
     with setData().
     If \a dbAware is true, table will be db-aware,
      and KexiDataTableView is used internally.
     Otherwise, table will be not-db-aware,
      and KexiTableView is used internally. In the latter case,
      data can be set by calling tableView()->setData(KDbTableViewData* data). */
    explicit KexiDataTableView(QWidget *parent, bool dbAware = true);

    /*! CTOR2: Creates db-aware, table view initialized with \a cursor.
     KexiDataTableView is used internally. */
    KexiDataTableView(QWidget *parent, KDbCursor *cursor);

    virtual ~KexiDataTableView();

    KexiTableScrollArea* tableView() const;

    //! Loads settings for table into @a data model.
    //! Used after loading data model in KexiDataTableView::setData(KDbCursor*), before calling KexiTableView::setData().
    //! @return true on success
    bool loadTableViewSettings(KDbTableViewData* data);

public Q_SLOTS:
    /*! Sets data. Only works for db-aware table. */
    void setData(KDbCursor *cursor);

    /*! Saves settings for the view. Implemented for KexiView. */
    virtual bool saveSettings();

protected Q_SLOTS:
//! @todo
    void filter();

protected:
    void init();

    class Private;
    Private * const d;
};

#endif
