/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Paul Mendez <paulestebanms@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
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
    explicit KPrAnimationGroupProxyModel(QObject *parent = nullptr);
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
