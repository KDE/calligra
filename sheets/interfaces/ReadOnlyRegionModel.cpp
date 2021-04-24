/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus stefan.nikolaus @kdemail.net

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ReadOnlyRegionModel.h"

// Sheets
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
