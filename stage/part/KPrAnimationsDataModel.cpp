/* This file is part of the KDE project
 * Copyright (C) 2012 Paul Mendez <paulestebanms@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (  at your option ) any later version.
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

#include "KPrAnimationsDataModel.h"

#include <animations/KPrAnimationStep.h>
#include "animations/KPrAnimationSubStep.h"
#include <KPrPage.h>
#include <KPrView.h>
#include <KPrCustomAnimationItem.h>

//KDE HEADERS
#include <KIconLoader>
#include <KLocale>
#include "KoShape.h"
#include "KoShapeContainer.h"
#include "KoShapePainter.h"
#include <KoPADocument.h>
#include <KoShapeManager.h>

//QT HEADERS
#include <QImage>
#include <QPainter>
#include <QDebug>
#include <QAbstractAnimation>

const int REAL_COLUMN_COUNT = 3;

enum ColumnNames {
    ShapeThumbnail = 0,
    AnimationIcon = 1,
    StartTime = 2,
    EndTime = 3,
    AnimationClass = 4
};

KPrAnimationsDataModel::KPrAnimationsDataModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_rootItem(0)
{
}

QModelIndex KPrAnimationsDataModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!m_rootItem || row < 0 || column < 0 || column > AnimationClass) {
        return QModelIndex();
    }
    KPrCustomAnimationItem *parentItem = itemForIndex(parent);
    Q_ASSERT(parentItem);
    if ((row == 0) && !m_rootItem->isDefaulAnimation()) {
        return createIndex(row, column, m_rootItem);
    }
    //Make sure of not display default init event
    if (KPrCustomAnimationItem *item = parentItem->childAt(
                parentItem->isDefaulAnimation() ? row : row - 1)) {
        return createIndex(row, column, item);
    }
    return QModelIndex();
}

QModelIndex KPrAnimationsDataModel::indexByShape(KoShape *shape)
{
    QModelIndex parent = QModelIndex();
    if (!shape) {
        return QModelIndex();
    }
    for (int row = 0; row < rowCount(parent); ++row) {
        QModelIndex thisIndex = index(row, 0, parent);
        KPrCustomAnimationItem *item = itemForIndex(thisIndex);
        if (item->shape() == shape) {
            return thisIndex;
        }
    }
    return QModelIndex();
}

QModelIndex KPrAnimationsDataModel::indexByItem(KPrCustomAnimationItem *item)
{
    QModelIndex parent = QModelIndex();
    if (!item) {
        return QModelIndex();
    }
    if (item->isDefaulAnimation()) {
        return QModelIndex();
    }
    for (int row = 0; row < rowCount(parent); ++row) {
        QModelIndex thisIndex = index(row, 0, parent);
        KPrCustomAnimationItem *thisItem = itemForIndex(thisIndex);
        if (item == thisItem) {
            return thisIndex;
        }
    }
    return QModelIndex();
}

int KPrAnimationsDataModel::rowCount(const QModelIndex &parent) const
{   
    if (parent.isValid()) {
        return 0;
    }
    KPrCustomAnimationItem *parentItem = itemForIndex(parent);
    if (parentItem) {
        return parentItem->isDefaulAnimation() ? parentItem->childCount() : parentItem->childCount() + 1;
    }
    else {
        return 0;
    }
}

int KPrAnimationsDataModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return REAL_COLUMN_COUNT;
}

QVariant KPrAnimationsDataModel::data(const QModelIndex &index, int role) const
{
    if (!m_rootItem || !index.isValid() || index.column() < 0 ||
        index.column() > AnimationClass) {
        return QVariant();
    }
    if (KPrCustomAnimationItem *item = itemForIndex(index)) {
        if (role == Qt::TextAlignmentRole) {
            return int(Qt::AlignRight | Qt::AlignVCenter);
        } else if (role == Qt::DisplayRole) {
            switch (index.column()) {
            case ShapeThumbnail:
                return QVariant();
            case AnimationIcon:
                return QVariant();
            case StartTime:
                return item->startTime();
            case EndTime:
                return item->duration();
            case AnimationClass:
                return item->type();
            default:
                return QVariant();

            }
        } else if (role == Qt::DecorationRole) {
            switch (index.column()) {
            case ShapeThumbnail:
                return item->thumbnail();
            case AnimationIcon:
                return item->animationIcon();
            default:
                return QVariant();
            }
        } else if (role == Qt::TextAlignmentRole) {
                return Qt::AlignCenter;
        } else if (role == Qt::ToolTipRole) {
            switch (index.column()) {
            case ShapeThumbnail:
                return QVariant();
            case AnimationIcon:
                return item->animationName();
            case StartTime:
                return i18n("Start after %1 seconds. Duration of %2 seconds").
                        arg(item->startTime()).arg(item->duration());
            case EndTime:
            case AnimationClass:
            default:
                return QVariant();

            }
        }
    }
    return QVariant();
}

QVariant KPrAnimationsDataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }
    if (orientation == Qt::Horizontal) {
        switch (section) {
        case ShapeThumbnail:
            return i18n("Seconds");
        default:
            return QVariant();
        }

    }
    return QVariant();
}

bool KPrAnimationsDataModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    //TODO: Edition features are not yet implemented
    if (index.isValid() && role == Qt::EditRole) {
        switch (index.column()) {
        case ShapeThumbnail:
            return false;
        case AnimationIcon:
            return false;
        case StartTime:
            //TODO: save new value in animation step.
            /*
            m_data[index.row()].startTime = value.toDouble();
            emit dataChanged(index, index);
            return true;
        case EndTime:
            m_data[index.row()].duration = value.toDouble();
            emit dataChanged(index, index);
            return true;
            */
        case AnimationClass:
            return false;
        default:
            return false;

        }
    }
    return false;
}

Qt::ItemFlags KPrAnimationsDataModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    return flags;
}

void KPrAnimationsDataModel::setParentItem(KPrCustomAnimationItem *item, KPrCustomAnimationItem *rootItem)
{
    if (item) {
        if (item->parent() == rootItem) {
            if (m_rootItem != item) {
                m_rootItem = item;
                reset();
            }
        }
    }
    else{
        m_rootItem = 0;
        reset();
    }
    emit layoutAboutToBeChanged();
    emit layoutChanged();
}

void KPrAnimationsDataModel::setDocumentView(KPrView *view)
{
    m_view = view;
    if (m_view->kopaDocument())
    {
        connect(m_view->kopaDocument(), SIGNAL(shapeRemoved(KoShape*)), this, SLOT(removeModel()));
        connect(m_view->kopaDocument(), SIGNAL(shapeAdded(KoShape*)), this, SLOT(removeModel()));
    }
    reset();
}

void KPrAnimationsDataModel::update()
{
    if (!m_rootItem->shape()) {
        m_rootItem = 0;
        emit layoutAboutToBeChanged();
        emit layoutChanged();
    }
}

void KPrAnimationsDataModel::removeModel()
{
    m_rootItem = 0;
    emit layoutAboutToBeChanged();
    emit layoutChanged();
}

KPrCustomAnimationItem *KPrAnimationsDataModel::itemForIndex(const QModelIndex &index) const
{
    if (index.isValid()) {
        if (KPrCustomAnimationItem *item = static_cast<KPrCustomAnimationItem*>(
                index.internalPointer()))
            return item;
    }
    return m_rootItem;
}


