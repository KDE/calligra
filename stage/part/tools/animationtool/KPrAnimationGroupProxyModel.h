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


#ifndef KPRANIMATIONGROUPPROXYMODEL_H
#define KPRANIMATIONGROUPPROXYMODEL_H

#include <QSortFilterProxyModel>

/** Filter animations model to display only groups of after preview / with preview
    animations that begin with an on click animation */
class KPrAnimationGroupProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit KPrAnimationGroupProxyModel(QObject *parent = 0);
    ~KPrAnimationGroupProxyModel() override;

    /**
     * @brief Takes a source model index to set the group to be displayed
     * Main model assigns a group number to each batch of with previous / after previous animations
     * that starts with an on click animation.
     * @param index of the parent animation
     * @return true if the animation group was changed
     */
    bool setCurrentIndex(const QModelIndex &index);

    /**
     * @brief Reset filter and restart model
     */
    void forceUpdateModel();

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
    int m_currentGroup;
};

#endif // KPRANIMATIONGROUPPROXYMODEL_H
