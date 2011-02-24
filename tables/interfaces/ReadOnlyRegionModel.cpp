/* This file is part of the KDE project
   Copyright 2008 Stefan Nikolaus stefan.nikolaus@kdemail.net

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
   Boston, MA 02110-1301, USA.
*/

#include "ReadOnlyRegionModel.h"

// KSpread
#include "Region.h"
#include "Sheet.h"

using namespace KSpread;

class ReadOnlyRegionModel::Private
{
public:
    Region region;
    ReadOnlyRegionModel test(Region());
};

ReadOnlyRegionModel::ReadOnlyRegionModel(const Region& region)
        : QAbstractProxyModel(region.firstSheet())
        , d(new Private)
{
    Q_ASSERT(region.isContiguous());
    d->region = region;
}

ReadOnlyRegionModel::~ReadOnlyRegionModel()
{
    delete d;
}

int ReadOnlyRegionModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return d->region.firstRange().width();
}

int ReadOnlyRegionModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return d->region.firstRange().height();
}

QModelIndex ReadOnlyRegionModel::index(int row, int column, const QModelIndex& parent) const
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    Q_UNUSED(parent);
    return QModelIndex();
}

QModelIndex ReadOnlyRegionModel::parent(const QModelIndex& index) const
{
    Q_UNUSED(index);
    return QModelIndex();
}

QModelIndex ReadOnlyRegionModel::mapFromSource(const QModelIndex& sourceIndex) const
{
    if (!d->region.contains(QPoint(sourceIndex.column() + 1, sourceIndex.row() + 1))) {
        return QModelIndex();
    }
    const QPoint offset = d->region.firstRange().topLeft();
    return createIndex(sourceIndex.column() - offset.x() + 1, sourceIndex.row() - offset.y() + 1);
}

QModelIndex ReadOnlyRegionModel::mapToSource(const QModelIndex& proxyIndex) const
{
    const QPoint offset = d->region.firstRange().topLeft();
//     return createIndex(proxyIndex.column() + offset.x() - 1, proxyIndex.row() + offset.y() - 1);
    return sourceModel()->index(proxyIndex.column() + offset.x() - 1, proxyIndex.row() + offset.y() - 1);
}
