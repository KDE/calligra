/* This file is part of the KDE project
 * Copyright (C) 2012 Paul Mendez <paulestebanms@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */


#include "KPrAnimationGroupProxyModel.h"

KPrAnimationGroupProxyModel::KPrAnimationGroupProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , m_currentGroup(-1)
{
}

KPrAnimationGroupProxyModel::~KPrAnimationGroupProxyModel()
{
}

bool KPrAnimationGroupProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    QModelIndex currentIndex = sourceModel()->index(source_row, 0, source_parent);
    int currentGroup = sourceModel()->data(currentIndex).toInt();
    return (currentGroup == m_currentGroup);
}

bool KPrAnimationGroupProxyModel::setCurrentIndex(const QModelIndex &index)
{
    QModelIndex currentIndex = sourceModel()->index(index.row(), 0);
    int currentGroup = sourceModel()->data(currentIndex).toInt();
    if (currentGroup != m_currentGroup) {
        m_currentGroup = currentGroup;
        invalidateFilter();
        revert();
        return true;
    }
    return false;
}

void KPrAnimationGroupProxyModel::forceUpdateModel()
{
    invalidateFilter();
    revert();
}
