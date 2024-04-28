/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "StylesSortFilterProxyModel.h"

#include <QDebug>

StylesSortFilterProxyModel::StylesSortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

bool StylesSortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    QVariant leftData = sourceModel()->data(left, Qt::DisplayRole);
    QVariant rightData = sourceModel()->data(right, Qt::DisplayRole);

    QString leftName = leftData.toString();
    QString rightName = rightData.toString();
    return QString::localeAwareCompare(leftName, rightName) < 0;
}
