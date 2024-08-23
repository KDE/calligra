/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Peter Simonsson <peter.simonsson@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "CollectionItemModel.h"

#include "StencilBoxDebug.h"

#include <KoProperties.h>
#include <KoShapeFactoryBase.h>

#include <QMimeData>

CollectionItemModel::CollectionItemModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_viewMode(QListView::IconMode)
{
}

Qt::DropActions CollectionItemModel::supportedDragActions() const
{
    return Qt::CopyAction;
}

QVariant CollectionItemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() > m_shapeTemplateList.count())
        return QVariant();

    switch (role) {
    case Qt::ToolTipRole:
        return m_shapeTemplateList[index.row()].toolTip;

    case Qt::DecorationRole:
        return m_shapeTemplateList[index.row()].icon;

    case Qt::UserRole:
        return m_shapeTemplateList[index.row()].id;

    case Qt::DisplayRole:
        return m_viewMode == QListView::ListMode ? m_shapeTemplateList[index.row()].name : QString();

    case Qt::UserRole + 1:
        return m_shapeTemplateList[index.row()].name;

    default:
        return QVariant();
    }

    return QVariant();
}

int CollectionItemModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_shapeTemplateList.count();
}

void CollectionItemModel::setShapeTemplateList(const QList<KoCollectionItem> &newlist)
{
    m_shapeTemplateList = newlist;
    beginResetModel();
    endResetModel();
}

QListView::ViewMode CollectionItemModel::viewMode() const
{
    return m_viewMode;
}

void CollectionItemModel::setViewMode(QListView::ViewMode vm)
{
    if (m_viewMode == vm)
        return;
    m_viewMode = vm;
}

QMimeData *CollectionItemModel::mimeData(const QModelIndexList &indexes) const
{
    if (indexes.isEmpty())
        return nullptr;

    QModelIndex index = indexes.first();

    if (!index.isValid())
        return nullptr;

    if (m_shapeTemplateList.isEmpty())
        return nullptr;

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODeviceBase::WriteOnly);
    dataStream << m_shapeTemplateList[index.row()].id;
    const KoProperties *props = m_shapeTemplateList[index.row()].properties;

    if (props)
        dataStream << props->store("shapes");
    else
        dataStream << QString();

    QMimeData *mimeData = new QMimeData;
    mimeData->setData(SHAPETEMPLATE_MIMETYPE, itemData);

    return mimeData;
}

QStringList CollectionItemModel::mimeTypes() const
{
    QStringList mimetypes;
    mimetypes << SHAPETEMPLATE_MIMETYPE;

    return mimetypes;
}

Qt::ItemFlags CollectionItemModel::flags(const QModelIndex &index) const
{
    if (index.isValid())
        return QAbstractListModel::flags(index) | Qt::ItemIsDragEnabled;

    return QAbstractListModel::flags(index);
}

const KoProperties *CollectionItemModel::properties(const QModelIndex &index) const
{
    if (!index.isValid() || index.row() > m_shapeTemplateList.count())
        return nullptr;

    return m_shapeTemplateList[index.row()].properties;
}
