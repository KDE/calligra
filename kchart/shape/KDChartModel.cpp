/* This file is part of the KDE project

   Copyright 2008 Johannes Simon <johannes.simon@gmail.com>

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

// Local
#include "KDChartModel.h"
#include "DataSet.h"

// KDE
#include <KDebug>

namespace KChart {

class KDChartModel::Private {
public:
    Private();

    int dataDimensions;
    QList<DataSet*> dataSets;
};

KDChartModel::Private::Private()
{
    dataDimensions = 1;
}

KDChartModel::KDChartModel( QObject *parent /* = 0 */ )
    : QAbstractItemModel( parent ),
      d( new Private )
{
}

KDChartModel::~KDChartModel()
{
}

QVariant KDChartModel::data( const QModelIndex &index,
                             int role /* = Qt::DisplayRole */ ) const
{
    if ( role != Qt::DisplayRole )
        return QVariant();
    
    int row = index.row();
    int column = index.column();
    int dataSetNumber = column / d->dataDimensions;
    DataSet *dataSet = d->dataSets[ dataSetNumber ];
    
    int dimension = column % d->dataDimensions;
    if ( dimension == 0 )
        return dataSet->yData( row );
    else if ( dimension == 1 )
        return dataSet->xData( row );
    else if ( dimension == 2 )
        return dataSet->customData( row );
    
    // Should never happen
    return QVariant();
}

void KDChartModel::dataChanged( const QModelIndex& topLeft, const QModelIndex& bottomRight )
{
    // TODO (Johannes): Emit dataChanged() for all datasets that have references to these rows
    emit QAbstractItemModel::dataChanged( topLeft, bottomRight );
}

QVariant KDChartModel::headerData( int section,
                                   Qt::Orientation orientation,
                                   int role /* = Qt::DisplayRole */ ) const
{
    if ( role != Qt::DisplayRole )
        return QVariant();
    
    if ( orientation == Qt::Vertical )
        return QVariant();
    
    if ( section >= columnCount() )
    {
        qDebug() << "Warning: Requesting header data from KDChartModel for non-existant data set!";
        return QVariant();
    }
    
    int dataSetNumber = section / d->dataDimensions;
    DataSet *dataSet = d->dataSets[ dataSetNumber ];
    
    return dataSet->labelData();
}

QModelIndex KDChartModel::index( int row, int column, const QModelIndex &parent ) const
{
    return createIndex( row, column, 0 );
}

QModelIndex KDChartModel::parent( const QModelIndex &index ) const
{
    return QModelIndex();
}

int KDChartModel::rowCount( const QModelIndex &parent /* = QModelIndex() */ ) const
{
    return d->dataSets.isEmpty() ? 0 : d->dataSets[0]->size();
}

int KDChartModel::columnCount( const QModelIndex &parent /* = QModelIndex() */ ) const
{
    return d->dataSets.size() * d->dataDimensions;
}

void KDChartModel::setDataDimensions( int dataDimensions )
{
    d->dataDimensions = dataDimensions;
}

void KDChartModel::addDataSet( DataSet *dataSet )
{
    if ( d->dataSets.contains( dataSet ) )
        return;
    
    int columnToBeInserted = columnCount();
    beginInsertColumns( QModelIndex(), columnToBeInserted, columnToBeInserted + d->dataDimensions - 1 );
    d->dataSets.append( dataSet );
    endInsertColumns();
}

void KDChartModel::removeDataSet( DataSet *dataSet )
{
    if ( !d->dataSets.contains( dataSet ) )
        return;
    
    int columnAboutToBeRemoved = d->dataSets.indexOf( dataSet ) * d->dataDimensions;
    beginRemoveColumns( QModelIndex(), columnAboutToBeRemoved, columnAboutToBeRemoved + d->dataDimensions - 1 );
    d->dataSets.removeAll( dataSet );
    endRemoveColumns();
}

} // namespace KChart
