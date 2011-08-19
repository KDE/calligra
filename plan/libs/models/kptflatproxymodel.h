/* This file is part of the KDE project
  Copyright (C) 2010 Dag Andersen <danders@get2net.dk>

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

#ifndef KPLATO_FLATPROXYMODEL_H
#define KPLATO_FLATPROXYMODEL_H

#include "kplatomodels_export.h"

#include <QAbstractProxyModel>
#include <QStandardItemModel>

class PersistentModelIndex;
class QModelIndex;
class QItemSelection;

/// The main namespace
namespace KPlato
{

/**
    FlatProxyModel is a proxy model that makes a tree source model flat.
    
    This might be usefull to present data from a tree model in e.g. a table view or a report.
    
    Note that the source model should have the same number of columns for all parent indeces,
    since a flat model obviously have the same number of columns for all indeces.
    If this is not the case, the behavior is undefined.
    
    The row sequence of the flat model is the same as if the source model was fully expanded.

    The flat model adds a Parent column at the end of the source model columns,
    to make it possible to access the parent index's data at column 0.
*/
class KPLATOMODELS_EXPORT FlatProxyModel : public QAbstractProxyModel
{
    Q_OBJECT
public:
    FlatProxyModel ( QObject * parent = 0 );

    virtual QModelIndex mapFromSource ( const QModelIndex & sourceIndex ) const;
    virtual QItemSelection mapSelectionFromSource ( const QItemSelection & sourceSelection ) const;
    virtual QItemSelection mapSelectionToSource ( const QItemSelection & proxySelection ) const;
    virtual QModelIndex mapToSource ( const QModelIndex & proxyIndex ) const;
    virtual void setSourceModel ( QAbstractItemModel * sourceModel );

    QModelIndex index( int row, int column, const QModelIndex &parent = QModelIndex() ) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount( const QModelIndex &parent = QModelIndex() ) const;
    int columnCount(const QModelIndex &parent  = QModelIndex() ) const;
    bool hasChildren(const QModelIndex &parent = QModelIndex() ) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole ) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole );

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole);

    QMimeData *mimeData(const QModelIndexList &indexes) const;
    QStringList mimeTypes() const;
    Qt::DropActions supportedDropActions() const;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent = QModelIndex());
    
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());


public slots:
    void sourceDataChanged(const QModelIndex &source_top_left,
                           const QModelIndex &source_bottom_right);
    void sourceHeaderDataChanged(Qt::Orientation orientation, int start, int end);

    void sourceReset();

    void sourceLayoutAboutToBeChanged();
    void sourceLayoutChanged();

    void sourceRowsAboutToBeInserted(const QModelIndex &source_parent,
                                        int start, int end);
    void sourceRowsInserted(const QModelIndex &source_parent,
                               int start, int end);
    void sourceRowsAboutToBeRemoved(const QModelIndex &source_parent,
                                       int start, int end);
    void sourceRowsRemoved(const QModelIndex &source_parent,
                              int start, int end);

protected:
    int mapFromSourceRow( const QModelIndex & sourceIndex ) const;
    int mapToSourceRow( const  QModelIndex & sourceIndex ) const;

private slots:
    void initiateMaps( const QModelIndex &sourceParent = QModelIndex() );
    void sourceModelDestroyed();
    
private:
    /// List of sourceIndexes
    QList<QPersistentModelIndex> m_sourceIndexList;
    /// Map of sourceIndexes (parent, index)
    QMultiMap<QPersistentModelIndex, QPersistentModelIndex> m_sourceIndexMap;

    QStandardItemModel m_privatemodel;
};

} //namespace KPlato


#endif
