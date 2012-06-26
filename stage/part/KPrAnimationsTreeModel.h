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
#ifndef KPRANIMATIONSTREEMODEL_H
#define KPRANIMATIONSTREEMODEL_H

#include <QAbstractItemModel>
#include "stage_export.h"

class KPrPage;
class KPrView;
class KPrCustomAnimationItem;
class KoShape;


/** Data model for the animations tree view (see animation tool)*/
class STAGE_EXPORT KPrAnimationsTreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit KPrAnimationsTreeModel(QObject *parent=0);
    ~KPrAnimationsTreeModel();
    void clear();

    Qt::ItemFlags flags(const QModelIndex &index) const;
     QVariant data(const QModelIndex &index,
                   int role=Qt::DisplayRole) const;
     QVariant headerData(int section, Qt::Orientation orientation,
                         int role=Qt::DisplayRole) const;
     int rowCount(const QModelIndex &parent=QModelIndex()) const;
     int columnCount(const QModelIndex &parent=QModelIndex()) const;
     QModelIndex index(int row, int column,
                       const QModelIndex &parent=QModelIndex()) const;
     QModelIndex parent(const QModelIndex &index) const;

     bool setHeaderData(int, Qt::Orientation, const QVariant&,
                        int=Qt::EditRole) { return false; }
     bool setData(const QModelIndex &index, const QVariant &value,
                  int role=Qt::EditRole);
     bool insertRows(int row, int count,
                     const QModelIndex &parent=QModelIndex());
     bool removeRows(int row, int count,
                     const QModelIndex &parent=QModelIndex());
 /*
     Qt::DropActions supportedDragActions() const
         { return Qt::MoveAction; }
     Qt::DropActions supportedDropActions() const
         { return Qt::MoveAction; }
     QStringList mimeTypes() const;
     QMimeData *mimeData(const QModelIndexList &indexes) const;
     bool dropMimeData(const QMimeData *mimeData,
             Qt::DropAction action, int row, int column,
             const QModelIndex &parent);

     bool hasCutItem() const { return cutItem; }
 */
     QModelIndex moveUp(const QModelIndex &index);
     QModelIndex moveDown(const QModelIndex &index);
     QModelIndex cut(const QModelIndex &index);
     QModelIndex removeItemByIndex(const QModelIndex &index);
     QModelIndex paste(const QModelIndex &index);
     QModelIndex promote(const QModelIndex &index);
     QModelIndex demote(const QModelIndex &index);


    /// An active page is required before use the model
    void setActivePage(KPrPage *activePage);

    /// requiere to update model if a shape is removed (or added with undo)
    void setDocumentView(KPrView *view);

    /// Return the first animation index for the given shape
    QModelIndex indexByShape(KoShape* shape);
    /// Return the index of a given item or an invalid index if it isn't on the model
    QModelIndex indexByItem(KPrCustomAnimationItem* item);
    /// Return the parent item
    KPrCustomAnimationItem* rootItem() const;

signals:
    void rootChanged();

public slots:
    /// Triggers an update of the complete model
    void updateData();

private:
    /// Return an item for the given index
    KPrCustomAnimationItem* itemForIndex(const QModelIndex &index) const;

    /// Emit standard signals when a group of items is changed
    void announceItemChanged(KPrCustomAnimationItem *item);
    QModelIndex moveItem(KPrCustomAnimationItem *parent, int oldRow, int newRow);

    KPrPage *m_activePage;
    KPrView *m_view;
    KPrCustomAnimationItem *m_rootItem;
    KPrCustomAnimationItem *cutItem;
};

#endif // KPRANIMATIONSTREEMODEL_H
