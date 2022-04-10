/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Stefan Nikolaus stefan.nikolaus @kdemail.net

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "RegionModel.h"

// Sheets
#include "engine/Region.h"
#include "core/Sheet.h"

using namespace Calligra::Sheets;

class RegionModel::Private
{
public:
    SheetBase* sheet;
    QRect range;
    bool overwriteMode;
};


RegionModel::RegionModel(const Region& region)
        : SheetModel(dynamic_cast<Sheet*>(region.lastSheet()))
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
