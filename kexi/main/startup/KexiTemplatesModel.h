/* This file is part of the KDE project
   Copyright (C) 2011 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXITEMPLATESMODEL_H
#define KEXITEMPLATESMODEL_H

#include <QAbstractListModel>

#include <KCategorizedSortFilterProxyModel>
#include <core/kexitemplateloader.h>

class KexiTemplatesModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum AdditionalRoles {
        NameRole = 0x9D419D49,
        CategoryRole = 0xA2BEF158
    };

    KexiTemplatesModel(const KexiTemplateCategoryInfoList& templateCategories,
                       QObject *parent = 0);
    ~KexiTemplatesModel();

    QModelIndex  index(int row, int column = 0,
                       const QModelIndex& parent = QModelIndex() ) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role) const;
    Qt::ItemFlags flags( const QModelIndex& index ) const;

private:
    class Private;
    Private* const d;
};
 
class KexiTemplatesProxyModel : public KCategorizedSortFilterProxyModel
{
public:
    KexiTemplatesProxyModel(QObject *parent = 0);
};

#endif
