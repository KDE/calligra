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

#include "kptflatproxymodel.h"

#include <KLocale>

#include <QModelIndex>
#include <QPersistentModelIndex>
#include <QItemSelection>

#include <qdebug.h>

namespace KPlato
{


FlatProxyModel::FlatProxyModel(QObject *parent)
    : QAbstractProxyModel( parent )
{
}

void FlatProxyModel::sourceModelDestroyed()
{
    m_sourceIndexList.clear();
}

void FlatProxyModel::sourceDataChanged(const QModelIndex &source_top_left, const QModelIndex &source_bottom_right)
{
    emit dataChanged( mapFromSource( source_top_left ), mapFromSource( source_bottom_right ) );
}

void FlatProxyModel::sourceHeaderDataChanged(Qt::Orientation orientation, int start, int end)
{
    emit headerDataChanged(orientation, start, end);
}

void FlatProxyModel::sourceReset()
{
    initiateMaps();
    reset();
}

void FlatProxyModel::sourceLayoutAboutToBeChanged()
{
    emit layoutAboutToBeChanged();
}

void FlatProxyModel::sourceLayoutChanged()
{
    initiateMaps();
    emit layoutChanged();
}

void FlatProxyModel::sourceRowsAboutToBeInserted(
    const QModelIndex &source_parent, int start, int end)
{
    m_sourceIndexList.clear();
    m_sourceIndexMap.clear();
}

void FlatProxyModel::sourceRowsInserted(const QModelIndex &source_parent, int start, int end)
{
    if ( sourceModel() == 0 ) {
        qDebug()<<"FlatProxyModel::sourceRowsInserted: No source model";
        return;
    }
    initiateMaps();
    reset();
}

void FlatProxyModel::sourceRowsAboutToBeRemoved(
    const QModelIndex &source_parent, int start, int end)
{
}

void FlatProxyModel::sourceRowsRemoved(
    const QModelIndex &source_parent, int start, int end)
{
    QList<QPersistentModelIndex> list = m_sourceIndexMap.values( QPersistentModelIndex( source_parent ) );
    int pstart = m_sourceIndexList.indexOf( list.at( start ) );
    int pend = m_sourceIndexList.indexOf( list.at( end ) );
    beginRemoveRows( QModelIndex(), pstart, pend );
    initiateMaps();
    endRemoveRows();
}

void FlatProxyModel::setSourceModel(QAbstractItemModel *model)
{
    if ( sourceModel() ) {
        disconnect(sourceModel(), SIGNAL(destroyed()),
                 this, SLOT(sourceModelDestroyed()));
        disconnect(sourceModel(), SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                this, SLOT(_q_sourceDataChanged(QModelIndex,QModelIndex)));

        disconnect(sourceModel(), SIGNAL(headerDataChanged(Qt::Orientation,int,int)),
                this, SLOT(_q_sourceHeaderDataChanged(Qt::Orientation,int,int)));

        disconnect(sourceModel(), SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)),
                this, SLOT(sourceRowsAboutToBeInserted(QModelIndex,int,int)));

        disconnect(sourceModel(), SIGNAL(rowsInserted(QModelIndex,int,int)),
                this, SLOT(sourceRowsInserted(QModelIndex,int,int)));

        disconnect(sourceModel(), SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
                this, SLOT(sourceRowsAboutToBeRemoved(QModelIndex,int,int)));

        disconnect(sourceModel(), SIGNAL(rowsRemoved(QModelIndex,int,int)),
                this, SLOT(sourceRowsRemoved(QModelIndex,int,int)));

        disconnect(sourceModel(), SIGNAL(layoutAboutToBeChanged()),
                this, SLOT(sourceLayoutAboutToBeChanged()));

        disconnect(sourceModel(), SIGNAL(layoutChanged()),
                this, SLOT(initiateMaps()));

        disconnect(sourceModel(), SIGNAL(modelReset()), this, SLOT(sourceReset()));
    }
    QAbstractProxyModel::setSourceModel(model ? model : &m_privatemodel);
    if ( model != &m_privatemodel ) {
        connect(model, SIGNAL(destroyed()),
                 this, SLOT(sourceModelDestroyed()));
    }
    connect(sourceModel(), SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            this, SLOT(sourceDataChanged(QModelIndex,QModelIndex)));

    connect(sourceModel(), SIGNAL(headerDataChanged(Qt::Orientation,int,int)),
            this, SLOT(sourceHeaderDataChanged(Qt::Orientation,int,int)));

    connect(sourceModel(), SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)),
            this, SLOT(sourceRowsAboutToBeInserted(QModelIndex,int,int)));

    connect(sourceModel(), SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SLOT(sourceRowsInserted(QModelIndex,int,int)));

    connect(sourceModel(), SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
            this, SLOT(sourceRowsAboutToBeRemoved(QModelIndex,int,int)));

    connect(sourceModel(), SIGNAL(rowsRemoved(QModelIndex,int,int)),
            this, SLOT(sourceRowsRemoved(QModelIndex,int,int)));

    connect(sourceModel(), SIGNAL(layoutAboutToBeChanged()),
            this, SLOT(sourceLayoutAboutToBeChanged()));

    connect(sourceModel(), SIGNAL(layoutChanged()),
            this, SLOT(initiateMaps()));

    connect(sourceModel(), SIGNAL(modelReset()), this, SLOT(initiateMaps()));

    initiateMaps();
    reset();
}

QModelIndex FlatProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    if ( parent.isValid() ) {
        return QModelIndex();
    }
    return createIndex( row, column );
}

QModelIndex FlatProxyModel::parent(const QModelIndex &child) const
{
    return QModelIndex();
}

int FlatProxyModel::rowCount(const QModelIndex &parent) const
{
    return m_sourceIndexList.count();
}

int FlatProxyModel::columnCount(const QModelIndex &parent) const
{
    if ( sourceModel() == 0 ) {
        return 0;
    }
    return sourceModel()->columnCount() + 1;
}

bool FlatProxyModel::hasChildren(const QModelIndex &parent) const
{
    return false;
}

QVariant FlatProxyModel::data(const QModelIndex &index, int role) const
{
    if ( sourceModel() == 0 || !index.isValid()) {
        qDebug()<<"FlatProxyModel::data: No source model || invalid index";
        return QVariant();
    }
    QModelIndex source_index;
    int col = index.column() - sourceModel()->columnCount();
    if ( col < 0 ) {
        source_index = mapToSource(index);
        qDebug()<<"FlatProxyModel::data: source column"<<col<<sourceModel()->columnCount();
    } else {
        source_index = mapToSource( this->index( index.row(), 0 ) );
        qDebug()<<"FlatProxyModel::data: proxy column"<<col<<sourceModel()->columnCount();
    }
    if ( !source_index.isValid() ) {
        qDebug()<<"FlatProxyModel::data: index valid but source index not valid";
        return QVariant();
    }
    QVariant r;
    if ( col < 0 ) {
        r = sourceModel()->data(source_index, role);
    } else if ( col == 0 ) {
        source_index = source_index.parent();
        if ( source_index.isValid() ) {
            r = sourceModel()->data(source_index, role);
        }
    }
    qDebug()<<"FlatProxyModel::data:"<<index<<r;
    return r;
}

bool FlatProxyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if ( sourceModel() == 0 ) {
        return false;
    }
    QModelIndex source_index = mapToSource(index);
    if (index.isValid() && !source_index.isValid()) {
        return false;
    }
    return sourceModel()->setData(source_index, value, role);
}

QVariant FlatProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ( sourceModel() == 0 ) {
        return QVariant();
    }
    int sec = section - sourceModel()->columnCount();
    if ( sec < 0 ) {
        return sourceModel()->headerData(section, orientation, role);
    }
    if ( sec == 0 ) {
        return i18n( "Parent" );
    }
    return QVariant();
}

bool FlatProxyModel::setHeaderData(int section, Qt::Orientation orientation,
                                          const QVariant &value, int role)
{
    if ( sourceModel() == 0 ) {
        return false;
    }
    //TODO
    return sourceModel()->setHeaderData(section, orientation, value, role);
}

QMimeData *FlatProxyModel::mimeData(const QModelIndexList &indexes) const
{
    if ( sourceModel() == 0 ) {
        return 0;
    }
    QModelIndexList source_indexes;
    for (int i = 0; i < indexes.count(); ++i) {
        source_indexes << mapToSource(indexes.at(i));
    }
    return sourceModel()->mimeData(source_indexes);
}

QStringList FlatProxyModel::mimeTypes() const
{
    if ( sourceModel() == 0 ) {
        return QStringList();
    }
    return sourceModel()->mimeTypes();
}

Qt::DropActions FlatProxyModel::supportedDropActions() const
{
    if ( sourceModel() == 0 ) {
        return 0;
    }
    return sourceModel()->supportedDropActions();
}

bool FlatProxyModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
                                         int row, int column, const QModelIndex &parent)
{
    if ( sourceModel() == 0 ) {
        return false;
    }
    if ((row == -1) && (column == -1))
        return sourceModel()->dropMimeData(data, action, -1, -1, mapToSource(parent));
    int source_destination_row = -1;
    int source_destination_column = -1;
    QModelIndex source_parent;
    if (row == rowCount(parent)) {
        source_parent = mapToSource(parent);
        source_destination_row = sourceModel()->rowCount(source_parent);
    } else {
        QModelIndex proxy_index = index(row, column, parent);
        QModelIndex source_index = mapToSource(proxy_index);
        source_destination_row = source_index.row();
        source_destination_column = source_index.column();
        source_parent = source_index.parent();
    }
    return sourceModel()->dropMimeData(data, action, source_destination_row,
                                  source_destination_column, source_parent);
}

bool FlatProxyModel::insertRows(int row, int count, const QModelIndex &parent)
{
    return false;
}

bool FlatProxyModel::removeRows(int row, int count, const QModelIndex &parent)
{
    //TODO
    return false;
}


/*!
   Returns the source model index corresponding to the given \a
   proxyIndex from the sorting filter model.

   \sa mapFromSource()
*/
QModelIndex FlatProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if ( ! proxyIndex.isValid() ) {
        return QModelIndex();
    }
    QModelIndex source_index = m_sourceIndexList.value( proxyIndex.row() );
    if ( proxyIndex.column() != 0 ) {
        source_index = sourceModel()->index( source_index.row(), proxyIndex.column(), source_index.parent() );
    }
    //qDebug()<<"FlatProxyModel::mapToSource:"<<proxyIndex<<"->"<<source_index;
    return source_index;
}

/*!
    Returns the model index in the FlatProxyModel given the \a
    sourceIndex from the source model.

    \sa mapToSource()
*/
QModelIndex FlatProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    QPersistentModelIndex idx = sourceIndex;
    if ( idx.column() != 0 ) {
        // we only map indeces with colun 0
        idx = sourceModel()->index( idx.row(), 0, idx.parent() );
    }
    QModelIndex proxy_index = index( m_sourceIndexList.indexOf( QPersistentModelIndex( sourceIndex ) ), idx.column() );
    //qDebug()<<"FlatProxyModel::mapFromSource:"<<sourceIndex<<"->"<<proxy_index;
    return proxy_index;
}

QItemSelection FlatProxyModel::mapSelectionToSource(const QItemSelection &proxySelection) const
{
    return QAbstractProxyModel::mapSelectionToSource(proxySelection);
}

QItemSelection FlatProxyModel::mapSelectionFromSource(const QItemSelection &sourceSelection) const
{
    return QAbstractProxyModel::mapSelectionFromSource(sourceSelection);
}

void FlatProxyModel::initiateMaps(  const QModelIndex &sourceParent )
{
    if ( ! sourceParent.isValid() ) {
        m_sourceIndexList.clear();
        m_sourceIndexMap.clear();
    }
    QAbstractItemModel *m = sourceModel();
    if ( m == 0 ) {
        qDebug()<<"FlatProxyModel::initiateMaps: No source model";
        return;
    }
    int count = m->rowCount( sourceParent );
    for ( int row = 0; row < count; ++row ) {
        QPersistentModelIndex idx = m->index( row, 0, sourceParent );

        m_sourceIndexList.append( idx );
        m_sourceIndexMap.insert( idx.parent(), idx );

        initiateMaps( idx );
    }
    //qDebug()<<"FlatProxyModel::initiateMaps: source index list="<<m_sourceIndexList;
}


} // namespace KPlato

#include "kptflatproxymodel.moc"
