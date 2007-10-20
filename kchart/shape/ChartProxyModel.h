/* This file is part of the KDE project

   Copyright 2007 Johannes Simon <johannes.simon@gmail.com>

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

#ifndef KCHART_CHART_PROXY_MODEL
#define KCHART_CHART_PROXY_MODEL

#include <QAbstractProxyModel>
#include <QAbstractTableModel>

class QAbstractItemModel;

namespace KChart {

class ChartProxyModel : public QAbstractProxyModel
{
    Q_OBJECT

public:
    ChartProxyModel( QObject *parent = 0 );
    ~ChartProxyModel();

public slots:
    virtual QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;
    virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

    virtual QMap<int, QVariant> itemData( const QModelIndex &index ) const;

    virtual QModelIndex index( int row, int column, const QModelIndex &parent = QModelIndex() ) const;
    virtual QModelIndex parent( const QModelIndex &index ) const;

    virtual QModelIndex mapFromSource( const QModelIndex &sourceIndex ) const;
    virtual QModelIndex mapToSource( const QModelIndex &proxyIndex ) const;

    Qt::Orientation mapFromSource( Qt::Orientation orientation ) const;
    Qt::Orientation mapToSource( Qt::Orientation orientation ) const;

    virtual int rowCount( const QModelIndex &parent = QModelIndex() ) const;
    virtual int columnCount( const QModelIndex &parent = QModelIndex() ) const;

    void setFirstRowIsLabel( bool b );
    void setFirstColumnIsLabel( bool b );
    void setDataDirection( Qt::Orientation orientation );

private:
    class Private;
    Private *const d;
};

} // namespace KChart

#endif
