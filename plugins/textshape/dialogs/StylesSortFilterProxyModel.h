/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef STYLESSORTFILTERPROXYMODEL_H
#define STYLESSORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

class StylesSortFilterProxyModel : public QSortFilterProxyModel
{
public:
    explicit StylesSortFilterProxyModel(QObject *parent = nullptr);

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
};

#endif /* STYLESSORTFILTERPROXYMODEL_H */
