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

#include "DocumentsListTreeItem.h"

#include <QVariant>
#include <QList>


DocumentsListTreeItem::DocumentsListTreeItem(const QVector<QVariant> &data, DocumentsListTreeItem *parent)
{
    parentItem = parent;
    itemData = data;
}

DocumentsListTreeItem::~DocumentsListTreeItem()
{
    qDeleteAll(childItems);
}

DocumentsListTreeItem *DocumentsListTreeItem::child(int number)
{
    return childItems.value(number);
}

int DocumentsListTreeItem::childCount() const
{
    return childItems.count();
}

int DocumentsListTreeItem::childNumber() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<DocumentsListTreeItem*>(this));
    
    return 0;
}

int DocumentsListTreeItem::columnCount() const
{
    return itemData.count();
}

bool DocumentsListTreeItem::insertChildren(int position, int count, int columns)
{
    if (position < 0 || position > childItems.size())
        return false;

    for (int row = 0; row < count; ++row) {
        QVector<QVariant> data(columns);
        DocumentsListTreeItem *item = new DocumentsListTreeItem(data, this);
        childItems.insert(position, item);
    }

    return true;
}

bool DocumentsListTreeItem::insertColumns(int position, int columns)
{
    if (position < 0 || position > itemData.size())
        return false;

    for (int column = 0; column < columns; ++column)
        itemData.insert(position, QVariant());

    foreach (DocumentsListTreeItem *child, childItems)
        child->insertColumns(position, columns);

    return true;
}

QVariant DocumentsListTreeItem::data(int column) const
{
    return itemData.value(column);
}

bool DocumentsListTreeItem::setData(int column, const QVariant& value)
{
    if (column < 0 || column >= itemData.size())
        return false;
    
    itemData[column] = value;
    
    return true;
}

bool DocumentsListTreeItem::removeChildren(int position, int count)
{
    if (position < 0 || position + count > childItems.size())
        return false;

    for (int row = 0; row < count; ++row)
        delete childItems.takeAt(position);

    return true;
}

bool DocumentsListTreeItem::removeColumns(int position, int columns)
{
    if (position < 0 || position + columns > itemData.size())
        return false;

    for (int column = 0; column < columns; ++column)
        itemData.remove(position);

    foreach (DocumentsListTreeItem *child, childItems)
        child->removeColumns(position, columns);

    return true;
}

DocumentsListTreeItem *DocumentsListTreeItem::parent()
{
    return parentItem;
}
