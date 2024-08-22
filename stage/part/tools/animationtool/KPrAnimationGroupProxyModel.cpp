/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Paul Mendez <paulestebanms@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KPrAnimationGroupProxyModel.h"

KPrAnimationGroupProxyModel::KPrAnimationGroupProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , m_currentGroup(-1)
{
}

KPrAnimationGroupProxyModel::~KPrAnimationGroupProxyModel() = default;

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
