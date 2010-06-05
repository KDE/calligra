/* This file is part of the KDE project
   Copyright 2009 Stefan Nikolaus stefan.nikolaus@kdemail.net

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

#include "RegionModel.h"

// KSpread
#include "Region.h"
#include "Sheet.h"

using namespace KSpread;

class RegionModel::Private
{
public:
    Sheet* sheet;
    QRect range;
    bool overwriteMode;
};


RegionModel::RegionModel(const Region& region)
    : SheetModel(region.lastSheet())
    , d(new Private)
{
    Q_ASSERT(region.isContiguous());
    Q_ASSERT(!region.isEmpty());
    Q_ASSERT(region.lastSheet());
    d->sheet = region.lastSheet();
    d->range = region.lastRange();
    d->overwriteMode = true;
}

RegionModel::~RegionModel()
{
    delete d;
}

int RegionModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid() && parent.internalPointer() != d->sheet->map()) {
        return false;
    }
    if (d->overwriteMode) {
        return SheetModel::columnCount(parent) - d->range.left() + 1;
    }
    return d->range.width();
}

QModelIndex RegionModel::index(int row, int column, const QModelIndex &parent) const
{
    return SheetModel::index(row + d->range.top() - 1, column + d->range.left() - 1, parent);
}

int RegionModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid() && parent.internalPointer() != d->sheet->map()) {
        return false;
    }
    if (d->overwriteMode) {
        return SheetModel::rowCount(parent) - d->range.top() + 1;
    }
    return d->range.height();
}

#include "RegionModel.moc"
