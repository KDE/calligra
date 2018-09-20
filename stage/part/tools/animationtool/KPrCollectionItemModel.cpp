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

// Heavily based in CollectionItemModel work of Peter Simonsson <peter.simonsson@gmail.com>

#include "KPrCollectionItemModel.h"

#include "StageDebug.h"

KPrCollectionItemModel::KPrCollectionItemModel(QObject* parent)
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

    switch(role)
    {
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
