/*
 * Copyright 2012  Yue Liu <yue.liu@mail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

#include "StencilListModel.h"

#include <KoProperties.h>

#include <QList>
#include <QString>
#include <QIcon>
#include <QMimeData>

StencilListModel::StencilListModel(QString stencilName, QObject *parent) :
    QAbstractStencilListModel(parent), m_stencilName(stencilName)
{
}

StencilListModel::~StencilListModel() {
    qDeleteAll(m_list);
    m_list.clear();
}

QVariant StencilListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() > m_list.count ())
        return QVariant();

    switch(role)
    {
    case Qt::DisplayRole:
        return m_list[index.row()].name;
    case Qt::DecorationRole:
        return m_list[index.row()].icon;

    case Qt::UserRole:
        return m_list[index.row()].id;

    case Qt::UserRole+1:
        return m_list[index.row()].properties;

    default:
        return QVariant();
    }
}

int StencilListModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_list.count();
}

QMimeData* StencilListModel::mimeData(const QModelIndexList& indexes) const
{
    if(indexes.isEmpty())
        return 0;

    QModelIndex index = indexes.first();

    if(!index.isValid())
        return 0;

    if(m_list.isEmpty())
        return 0;

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << m_list[index.row()].id;
    KoProperties *props = m_list[index.row()].properties;

    if(props)
        dataStream << props->store("shapes");
    else
        dataStream << QString();

    QMimeData* mimeData = new QMimeData;
    mimeData->setData(SHAPETEMPLATE_MIMETYPE, itemData);

    return mimeData;
}

QStringList StencilListModel::mimeTypes() const
{
    QStringList mimetypes;
    mimetypes << SHAPETEMPLATE_MIMETYPE;

    return mimetypes;
}

void StencilListModel::appendRow(ShapeItem *item)
{
    appendRows(QList<ShapeItem*>() << item);
}

void StencilListModel::appendRows(const QList<ShapeItem *> &items)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount()+items.size()-1);
    foreach(ShapeItem *item, items) {
        connect(item, SIGNAL(dataChanged()), SLOT(handleItemChange()));
        m_list.append(item);
    }
    endInsertRows();
}

void StencilListModel::insertRow(int row, ShapeItem *item)
{
    beginInsertRows(QModelIndex(), row, row);
    connect(item, SIGNAL(dataChanged()), SLOT(handleItemChange()));
    m_list.insert(row, item);
    endInsertRows();
}

ShapeItem * StencilListModel::find(const QString &id) const
{
    foreach(ShapeItem* item, m_list) {
        if(item->id() == id) return item;
    }
    return 0;
}

QModelIndex StencilListModel::indexFromItem(const ShapeItem *item) const
{
    Q_ASSERT(item);
    for(int row=0; row<m_list.size(); ++row) {
        if(m_list.at(row) == item) return index(row);
    }
    return QModelIndex();
}

bool StencilListModel::removeRow(int row, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if(row < 0 || row >= m_list.size()) return false;
    beginRemoveRows(QModelIndex(), row, row);
    delete m_list.takeAt(row);
    endRemoveRows();
    return true;
}

bool StencilListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if(row < 0 || (row+count) >= m_list.size()) return false;
    beginRemoveRows(QModelIndex(), row, row+count-1);
    for(int i=0; i<count; ++i) {
        delete m_list.takeAt(row);
    }
    endRemoveRows();
    return true;
}

ShapeItem * StencilListModel::takeRow(int row)
{
    beginRemoveRows(QModelIndex(), row, row);
    ShapeItem* item = m_list.takeAt(row);
    endRemoveRows();
    return item;
}

void StencilListModel::handleItemChange()
{
    ShapeItem* item = static_cast<ShapeItem*>(sender());
    QModelIndex index = indexFromItem(item);
    if(index.isValid())
        emit dataChanged(index, index);
}
