/* This file is part of the KDE project
   Copyright (C) 2010-2011 Christoph Goerlich <chgoerlich@gmx.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "DocumentsListTreeModel.h"
#include "DocumentsListTreeItem.h"
#include "GoogleDocument.h"

#include <QStringList>

#include <QDebug>

DocumentsListTreeModel::DocumentsListTreeModel(const QStringList& headers, const QString& data, QObject* parent)
    : QAbstractItemModel(parent)
{
    QVector<QVariant> rootData;
    foreach (QString header, headers)
        rootData << header;
        
    rootItem = new DocumentsListTreeItem(rootData);

    setupModelData(data.split(QString("\n")), rootItem);
}

DocumentsListTreeModel::DocumentsListTreeModel(/*const QStringList &headers,*/
        const QList<GoogleDocument*> &data, QObject* parent)
    : QAbstractItemModel(parent)
{
     QVector<QVariant> rootData;
//     foreach (QString header, headers)
//         rootData << header;
    
    rootData << "Title" << "DocType" << "Author" << "Updated";
    rootItem = new DocumentsListTreeItem(rootData);

    QString strData;
    foreach (const GoogleDocument *doc, data) {
        strData += doc->title() +"\t" + doc->docType() + "\t" + doc->author() + "\t" + doc->updated() + "\n";
    }

    setupModelData(strData.split(QString("\n")), rootItem);
}

DocumentsListTreeModel::~DocumentsListTreeModel()
{
    delete rootItem;
}

int DocumentsListTreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return rootItem->columnCount();
}

QVariant DocumentsListTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    DocumentsListTreeItem *item = getItem(index);

    return item->data(index.column());
}


Qt::ItemFlags DocumentsListTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

DocumentsListTreeItem* DocumentsListTreeModel::getItem(const QModelIndex& index) const
{
    if (index.isValid()) {
        DocumentsListTreeItem *item = static_cast<DocumentsListTreeItem*>(index.internalPointer());
        if (item)
            return item;
    }
    return rootItem;
}

QVariant DocumentsListTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}

QModelIndex DocumentsListTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();

    DocumentsListTreeItem *parentItem = getItem(parent);
    
    DocumentsListTreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

bool DocumentsListTreeModel::insertColumns(int position, int columns, const QModelIndex& parent)
{
    bool success;

    beginInsertColumns(parent, position, position + columns - 1);
    success = rootItem->insertColumns(position, columns);
    endInsertColumns();

    return success;
}

bool DocumentsListTreeModel::removeColumns(int position, int columns, const QModelIndex& parent)
{
    bool success;

    beginRemoveColumns(parent, position, position + columns - 1);
    success = rootItem->removeColumns(position, columns);
    endRemoveColumns();

    if (rootItem->columnCount() == 0)
        removeRows(0, rowCount());

    return success;
}

bool DocumentsListTreeModel::insertRows(int position, int rows, const QModelIndex& parent)
{
    DocumentsListTreeItem *parentItem = getItem(parent);
    bool success;

    beginInsertRows(parent, position, position + rows - 1);
    success = parentItem->insertChildren(position, rows, rootItem->columnCount());
    endInsertRows();

    return success;
}

bool DocumentsListTreeModel::removeRows(int position, int rows, const QModelIndex& parent)
{
    DocumentsListTreeItem *parentItem = getItem(parent);
    bool success = true;

    beginRemoveRows(parent, position, position + rows - 1);
    success = parentItem->removeChildren(position, rows);
    endRemoveRows();

    return success;
}

QModelIndex DocumentsListTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    DocumentsListTreeItem *childItem = getItem(index);
    DocumentsListTreeItem *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, parentItem);
}

int DocumentsListTreeModel::rowCount(const QModelIndex &parent) const
{
    DocumentsListTreeItem *parentItem = getItem(parent);

    return parentItem->childCount();
}

bool DocumentsListTreeModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
     if (role != Qt::EditRole)
         return false;

     DocumentsListTreeItem *item = getItem(index);
     bool result = item->setData(index.column(), value);

     if (result)
         emit dataChanged(index, index);

     return result;
}

bool DocumentsListTreeModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
     if (role != Qt::EditRole || orientation != Qt::Horizontal)
         return false;

     bool result = rootItem->setData(section, value);

     if (result)
         emit headerDataChanged(orientation, section, section);

     return result;
}

void DocumentsListTreeModel::setupModelData(const QStringList &lines, DocumentsListTreeItem *parent)
{
    QList<DocumentsListTreeItem*> parents;
    QList<int> indentations;
    parents << parent;
    indentations << 0;

    int number = 0;

    while (number < lines.count()) {
        int position = 0;
        while (position < lines[number].length()) {
            if (lines[number].mid(position, 1) != " ")
                break;
            position++;
        }

        QString lineData = lines[number].mid(position).trimmed();

        if (!lineData.isEmpty()) {
            // Read the column data from the rest of the line.
            QStringList columnStrings = lineData.split("\t", QString::SkipEmptyParts);
            QVector<QVariant> columnData;
            for (int column = 0; column < columnStrings.count(); ++column)
                columnData << columnStrings[column];

            if (position > indentations.last()) {
                // The last child of the current parent is now the new parent
                // unless the current parent has no children.

                if (parents.last()->childCount() > 0) {
                    parents << parents.last()->child(parents.last()->childCount()-1);
                    indentations << position;
                }
            } 
            else {
                while (position < indentations.last() && parents.count() > 0) {
                    parents.pop_back();
                    indentations.pop_back();
                }
            }

            // Append a new item to the current parent's list of children.
            DocumentsListTreeItem *parent = parents.last();
            parent->insertChildren(parent->childCount(), 1, rootItem->columnCount());
            for (int column = 0; column < columnData.size(); ++column)
                parent->child(parent->childCount() - 1)->setData(column, columnData[column]);
        }

        number++;
    }
}

#include "DocumentsListTreeModel.moc"
