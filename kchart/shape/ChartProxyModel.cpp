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

#include "ChartProxyModel.h"

#include <KDebug>

namespace KChart {

class ChartProxyModel::Private {
public:
    Private();
};

ChartProxyModel::Private::Private()
{
}

ChartProxyModel::ChartProxyModel( QObject *parent /* = 0 */ )
    : QAbstractProxyModel( parent ),
      d( new Private )
{
}

ChartProxyModel::~ChartProxyModel()
{
}

QVariant ChartProxyModel::data( const QModelIndex &index,
                                int role /* = Qt::DisplayRole */ ) const
{
    if ( sourceModel() == 0 )
        return QVariant();

    return sourceModel()->data( sourceModel()->index( index.row(), index.column() ), role );
}

QVariant ChartProxyModel::headerData( int section,
                                      Qt::Orientation orientation,
                                      int role /* = Qt::DisplayRole */ ) const
{
    if ( sourceModel() == 0 )
        return QVariant();

    return sourceModel()->headerData( section, orientation, role );
}

QMap<int, QVariant> ChartProxyModel::itemData( const QModelIndex &index ) const
{
    return sourceModel()->itemData( index );
}

QModelIndex ChartProxyModel::index( int row,
                                    int column,
                                    const QModelIndex &parent /* = QModelIndex() */ ) const
{
    return QAbstractItemModel::createIndex( row, column, 0 );
}

QModelIndex ChartProxyModel::parent( const QModelIndex &index ) const
{
    return QModelIndex();
}

QModelIndex ChartProxyModel::mapFromSource( const QModelIndex &sourceIndex ) const
{
    return QModelIndex();
}

QModelIndex ChartProxyModel::mapToSource( const QModelIndex &proxyIndex ) const
{
    return QModelIndex();
}

int ChartProxyModel::rowCount( const QModelIndex &parent /* = QModelIndex() */ ) const
{
    if ( sourceModel() == 0 )
        return 0;

    return sourceModel()->rowCount( parent );
}

int ChartProxyModel::columnCount( const QModelIndex &parent /* = QModelIndex() */ ) const
{
    if ( sourceModel() == 0 )
        return 0;

    return sourceModel()->columnCount( parent );
}

} // namespace KChart
