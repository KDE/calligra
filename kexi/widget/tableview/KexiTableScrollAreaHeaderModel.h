/* This file is part of the KDE project
   Copyright (C) 2014-2015 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and,or
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

#ifndef KEXITABLESCROLLAREAHEADERMODEL_H
#define KEXITABLESCROLLAREAHEADERMODEL_H

#include "kexidatatable_export.h"
#include <kexi_global.h>

#include <QAbstractTableModel>

class KexiTableScrollArea;

//! @short A model for use in headers of table scroll area.
/*! It's a QAbstractTableModel-compliant proxy to data that is provided by KexiTableScrollArea.
*/
class KEXIDATATABLE_EXPORT KexiTableScrollAreaHeaderModel : public QAbstractTableModel
{
public:
    explicit KexiTableScrollAreaHeaderModel(KexiTableScrollArea* parent = 0);

    virtual ~KexiTableScrollAreaHeaderModel();

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    void beginInsertRows(const QModelIndex &parent, int first, int last);

    void endInsertRows();

    void beginRemoveRows(const QModelIndex &parent, int first, int last);

    void endRemoveRows();

private:
    class Private;
    Private * const d;
};

#endif
