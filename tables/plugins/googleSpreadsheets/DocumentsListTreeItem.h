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

#ifndef DOCUMENTSLISTTREEITEM_H
#define DOCUMENTSLISTTREEITEM_H

#include <QVector>

class QVariant;

class DocumentsListTreeItem
{
public:
    DocumentsListTreeItem(const QVector<QVariant> &data, DocumentsListTreeItem *parent = 0);
    ~DocumentsListTreeItem();

    DocumentsListTreeItem *child(int number);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    bool insertChildren(int position, int count, int columns);
    bool insertColumns(int position, int columns);
    DocumentsListTreeItem *parent();
    bool removeChildren(int position, int count);
    bool removeColumns(int position, int columns);
    int childNumber() const;
    bool setData(int column, const QVariant &value);
    QList<DocumentsListTreeItem*> childs() { return childItems; }

private:
    QList<DocumentsListTreeItem*> childItems;
    QVector<QVariant> itemData;
    DocumentsListTreeItem *parentItem;
};

#endif //DOCUMENTSLISTTREEITEM_H
