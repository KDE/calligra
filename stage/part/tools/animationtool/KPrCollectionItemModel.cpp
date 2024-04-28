/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Paul Mendez <paulestebanms@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

// Heavily based in CollectionItemModel work of Peter Simonsson <peter.simonsson@gmail.com>

#include "KPrCollectionItemModel.h"

#include "StageDebug.h"

KPrCollectionItemModel::KPrCollectionItemModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

Qt::DropActions KPrCollectionItemModel::supportedDragActions() const
{
    return Qt::IgnoreAction;
}

QVariant KPrCollectionItemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() > m_animationClassList.count())
        return QVariant();

    switch (role) {
    case Qt::ToolTipRole:
        return m_animationClassList[index.row()].toolTip;

    case Qt::DecorationRole:
        return m_animationClassList[index.row()].icon;

    case Qt::UserRole:
        return m_animationClassList[index.row()].id;

    case Qt::DisplayRole:
        return m_animationClassList[index.row()].name;

    default:
        return QVariant();
    }

    return QVariant();
}

int KPrCollectionItemModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_animationClassList.count();
}

void KPrCollectionItemModel::setAnimationClassList(const QVector<KPrCollectionItem> &newlist)
{
    beginResetModel();
    m_animationClassList = newlist;
    endResetModel();
}

KoXmlElement KPrCollectionItemModel::animationContext(const QModelIndex &index) const
{
    return m_animationClassList.value(index.row()).animationContext;
}

Qt::ItemFlags KPrCollectionItemModel::flags(const QModelIndex &index) const
{
    return QAbstractListModel::flags(index);
}
