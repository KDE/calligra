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

// Local
#include "ProxyModel.h"
#include "DataSet.h"

// KDE
#include <KDebug>

using namespace KChart;

class ProxyModel::Private {
public:
    Private();

    bool             firstRowIsLabel;
    bool             firstColumnIsLabel;
    Qt::Orientation  dataDirection;
    int dataDimensions;
    QMap<int, int> dataMap;
    QList<DataSet*> dataSets;
};

ProxyModel::Private::Private()
{
    firstRowIsLabel    = false;
    firstColumnIsLabel = false;
    dataDimensions = 1;

    dataDirection = Qt::Vertical;
}

ProxyModel::ProxyModel()
    : QAbstractProxyModel( 0 ),
      d( new Private )
{
    connect( this, SIGNAL( modelReset() ), this, SLOT( rebuildDataMap() ) );
}

ProxyModel::~ProxyModel()
{
}

void ProxyModel::rebuildDataMap()
{
    int numRows = rowCount();
    int dataSetCount = 0;
    
    // If we have 2 dimensions or more, the x data comes from
    // the first row by default
    int xDataRowInSourceModel = 0;
    int yDataRowInSourceModel = 0;
    
    if ( d->dataDimensions > 1 )
    {
        // Since first row is for x values,
        // move the first y value one row down
        yDataRowInSourceModel++;
    }
    
    d->dataMap.clear();
    
    for ( int i = 0; i < numRows; i += d->dataDimensions )
    {
        dataSetCount++;
        // Only insert a new data set if we don't already have it
        if ( dataSetCount > d->dataSets.count() )
        {
            d->dataSets.append( new DataSet( this ) );
        }
        
        if ( d->dataDimensions == 1 )
        {
            d->dataMap.insert( i, yDataRowInSourceModel );
        }
        else if ( d->dataDimensions == 2 )
        {
            d->dataMap.insert( i    , yDataRowInSourceModel );
            d->dataMap.insert( i + 1, xDataRowInSourceModel );
        }
        else if ( d->dataDimensions == 3 )
        {
            // TODO (Johannes): Handle third data dimension
        }
        yDataRowInSourceModel++;
    }
    
    // Some rows have apparently been removed from the model.
    // Remove remaining data sets from the list.
    while ( d->dataSets.count() > dataSetCount )
    {
        DataSet *dataSet = d->dataSets.takeLast();
        // TODO (Johannes): Keep the data sets to re-add them later
        // when the row count is increased again
        delete dataSet;
    }
}

void ProxyModel::setSourceModel( QAbstractItemModel *sourceModel )
{
    connect( sourceModel, SIGNAL( dataChanged( const QModelIndex&, const QModelIndex& ) ),
             this,        SLOT( dataChanged( const QModelIndex&, const QModelIndex& ) ) );

    QAbstractProxyModel::setSourceModel( sourceModel );

    // Update the entire data set
    reset();
}

QVariant ProxyModel::data( const QModelIndex &index,
                           int role /* = Qt::DisplayRole */ ) const
{
    if ( sourceModel() == 0 )
        return QVariant();
    
    QModelIndex sourceIndex = mapToSource( index );
    if ( sourceIndex == QModelIndex() )
    {
        qWarning() << "ProxyModel::data(): Attempting to request data for invalid source index";
        qWarning() << "ProxyModel::data(): Mapping resulted in:";
        qWarning() << index << "-->" << sourceIndex;
        return QVariant();
    }
    QVariant value = sourceModel()->data( sourceIndex, role );
    return value;
}

void ProxyModel::dataChanged( const QModelIndex& topLeft, const QModelIndex& bottomRight )
{
    int firstRow, lastRow;
    int firstCol, lastCol;
    int numRows;
    if ( d->dataDirection == Qt::Vertical )
    {
        numRows = rowCount();
        firstRow = qMin( topLeft.row(), bottomRight.row() );
        lastRow  = qMax( topLeft.row(), bottomRight.row() );
        
        firstCol = qMin( topLeft.column(), bottomRight.column() );
        lastCol  = qMax( topLeft.column(), bottomRight.column() );
        
        if ( d->firstRowIsLabel )
        {
            firstRow--;
            lastRow--;
        }
        if ( d->firstColumnIsLabel )
        {
            firstCol--;
            lastCol--;
        }
    }
    else
    {
        numRows = columnCount();
        firstRow = qMin( topLeft.column(), bottomRight.column() );
        lastRow  = qMax( topLeft.column(), bottomRight.column() );
        
        firstCol = qMin( topLeft.row(), bottomRight.row() );
        lastCol  = qMax( topLeft.row(), bottomRight.row() );

        if ( d->firstRowIsLabel )
        {
            firstCol--;
            lastCol--;
        }
        if ( d->firstColumnIsLabel )
        {
            firstRow--;
            lastRow--;
        }
    }
    
    for ( int i = 0; i < numRows; i++ )
    {
        if ( d->dataMap[ i ] >= firstRow && d->dataMap[ i ] <= lastRow )
        {
            int dataSet = int( i / d->dataDimensions );
            Q_ASSERT( dataSet < d->dataSets.size() );
            d->dataSets[ dataSet ]->dataChanged( firstCol, lastCol );
        }
    }
    
    emit dataChanged();
}

QVariant ProxyModel::headerData( int section,
                                 Qt::Orientation orientation,
                                 int role /* = Qt::DisplayRole */ ) const
{
    if ( sourceModel() == 0 )
        return QVariant();

    orientation = mapToSource( orientation );

    int row    = 0;
    int column = 0;

    if ( orientation == Qt::Vertical ) {
        if ( !d->firstColumnIsLabel )
            return QVariant();

        // Return the first column in the section-th row
        row = section;
        if ( d->firstRowIsLabel )
            row++;
    }
    else if ( orientation == Qt::Horizontal ) {
        if( !d->firstRowIsLabel )
            return QVariant();

        // Return the section-th column in the first row
        column = section;
        if ( d->firstColumnIsLabel )
            column++;
    }
    
    if ( row >= sourceModel()->rowCount() )
    {
        qWarning() << "ProxyModel::headerData(): Attempting to request header data for row >= rowCount";
        return QVariant();
    }
    if ( column >= sourceModel()->columnCount() )
    {
        qWarning() << "ProxyModel::headerData(): Attempting to request header data for column >= columnCount";
        return QVariant();
    }

    return sourceModel()->data( sourceModel()->index( row, column ), role );
}

QMap<int, QVariant> ProxyModel::itemData( const QModelIndex &index ) const
{
    return sourceModel()->itemData( mapToSource( index ) );
}

QModelIndex ProxyModel::index( int row,
                                    int column,
                                    const QModelIndex &parent /* = QModelIndex() */ ) const
{
    Q_UNUSED( parent );

    return QAbstractItemModel::createIndex( row, column, 0 );
}

QModelIndex ProxyModel::parent( const QModelIndex &index ) const
{
    Q_UNUSED( index );

    return QModelIndex();
}

QModelIndex ProxyModel::mapFromSource( const QModelIndex &sourceIndex ) const
{
    int row, column;
    if ( d->dataDirection == Qt::Vertical ) {
        row = sourceIndex.row();
        column = sourceIndex.column();

        if ( d->firstRowIsLabel )
            row--;
        if ( d->firstColumnIsLabel )
            column--;
        
        // Find the first occurance of row in the map
        for ( int i = 0; i < d->dataMap.size(); i++ )
        {
            if ( d->dataMap[i] == row ) {
                row = i;
                break;
            }
        }
    }
    else {
        row = sourceIndex.column();
        column = sourceIndex.row();

        if ( d->firstRowIsLabel )
            row--;
        if ( d->firstColumnIsLabel )
            column--;
        
        // Find the first occurance of column in the map
        for ( int i = 0; i < d->dataMap.size(); i++ )
        {
            if ( d->dataMap[i] == column ) {
                column = i;
                break;
            }
        }
    }
    
    return sourceModel()->index( row, column );
}

QModelIndex ProxyModel::mapToSource( const QModelIndex &proxyIndex ) const
{
    int row, column;
    if ( d->dataDirection == Qt::Vertical ) {
        row = d->dataMap[ proxyIndex.row() ];
        column = proxyIndex.column();
    }
    else {
        row = proxyIndex.column();
        column = d->dataMap[ proxyIndex.row() ];
    }

    if ( d->firstRowIsLabel )
        row++;
    if ( d->firstColumnIsLabel )
        column++;

    return sourceModel()->index( row, column );
}

Qt::Orientation ProxyModel::mapFromSource( Qt::Orientation orientation ) const
{
    // In fact, this method does exactly the same thing as mapToSource( Qt::Orientation ),
    // but replacing the code with a call to mapToSource() would just confuse at this point.
    if ( d->dataDirection == Qt::Vertical )
        return orientation;

    // orientation is Qt::Horizontal
    // Thus, we need to return the opposite of orientation.
    if ( orientation == Qt::Vertical )
        return Qt::Horizontal;
    return Qt::Vertical;
}

Qt::Orientation ProxyModel::mapToSource( Qt::Orientation orientation ) const
{
    if ( d->dataDirection == Qt::Vertical )
        return orientation;

    // orientation is Qt::Horizontal
    // Thus, we need to return the opposite of orientation.
    if ( orientation == Qt::Vertical )
        return Qt::Horizontal;
    return Qt::Vertical;
}

int ProxyModel::rowCount( const QModelIndex &parent /* = QModelIndex() */ ) const
{
    if ( sourceModel() == 0 )
        return 0;

    int rowCount;
    if ( d->dataDirection == Qt::Vertical )
        rowCount = sourceModel()->rowCount( parent );
    else
        rowCount = sourceModel()->columnCount( parent );
    // Even if the first row is a header - if the data table is empty,
    // we still have 0 rows, not -1

    bool firstRowIsLabel = d->firstRowIsLabel;
    if ( d->dataDirection == Qt::Horizontal )
        firstRowIsLabel = d->firstColumnIsLabel;

    if ( rowCount > 0 && firstRowIsLabel )
        rowCount--;
    
    rowCount *= d->dataDimensions;

    return rowCount;
}

int ProxyModel::columnCount( const QModelIndex &parent /* = QModelIndex() */ ) const
{
    if ( sourceModel() == 0 )
        return 0;

    int columnCount;
    if ( d->dataDirection == Qt::Vertical )
        columnCount = sourceModel()->columnCount( parent );
    else
        columnCount = sourceModel()->rowCount( parent );
    // Even if the first column is a header - if the data table is empty,
    // we still have 0 columns, not -1

    bool firstColumnIsLabel = d->firstColumnIsLabel;
    if ( d->dataDirection == Qt::Horizontal )
        firstColumnIsLabel = d->firstRowIsLabel;

    if ( columnCount > 0 && firstColumnIsLabel )
        columnCount--;
    
    return columnCount;
}

void ProxyModel::setFirstRowIsLabel( bool b )
{
    if ( b == d->firstRowIsLabel )
        return;
    
    if ( b ) {
        if ( d->dataDirection == Qt::Horizontal )
            beginRemoveColumns( QModelIndex(), 0, 0 );
        else
            beginRemoveRows( QModelIndex(), 0, 0 );
    } else {
        if ( d->dataDirection == Qt::Horizontal )
            beginInsertColumns( QModelIndex(), 0, 0 );
        else
            beginInsertRows( QModelIndex(), 0, 0 );
    }
    
    d->firstRowIsLabel = b;
    rebuildDataMap();
    
    if ( b ) {
        if ( d->dataDirection == Qt::Horizontal )
            endRemoveColumns();
        else
            endRemoveRows();
    } else {
        if ( d->dataDirection == Qt::Horizontal )
            endInsertColumns();
        else
            endInsertRows();
    }
}
 
void ProxyModel::setFirstColumnIsLabel( bool b )
{
    if ( b == d->firstColumnIsLabel )
        return;
    
    if ( b ) {
        if ( d->dataDirection == Qt::Vertical )
            beginRemoveColumns( QModelIndex(), 0, 0 );
        else
            beginRemoveRows( QModelIndex(), 0, 0 );
    } else {
        if ( d->dataDirection == Qt::Vertical )
            beginInsertColumns( QModelIndex(), 0, 0 );
        else
            beginInsertRows( QModelIndex(), 0, 0 );
    }
    
    d->firstColumnIsLabel = b;
    rebuildDataMap();
    
    if ( b ) {
        if ( d->dataDirection == Qt::Vertical )
            endRemoveColumns();
        else
            endRemoveRows();
    } else {
        if ( d->dataDirection == Qt::Vertical )
            endInsertColumns();
        else
            endInsertRows();
    }
}

Qt::Orientation ProxyModel::dataDirection()
{
    return d->dataDirection;
}

void ProxyModel::setDataDirection( Qt::Orientation orientation )
{
    d->dataDirection = orientation;
    reset();
}

void ProxyModel::setDataDimensions( int dimensions )
{
    d->dataDimensions = dimensions;
    reset();
}

bool ProxyModel::firstRowIsLabel() const
{
    return d->firstRowIsLabel;
}

bool ProxyModel::firstColumnIsLabel() const
{
    return d->firstColumnIsLabel;
}

QVariant ProxyModel::xData( DataSet *dataSet, int column ) const
{
    int dataSetNumber = d->dataSets.indexOf( dataSet );
    return data( index( dataSetNumber * d->dataDimensions + 1, column ) );
}

QVariant ProxyModel::yData( DataSet *dataSet, int column ) const
{
    int dataSetNumber = d->dataSets.indexOf( dataSet );
    return data( index( dataSetNumber * d->dataDimensions, column ) );
}

QVariant ProxyModel::customData( DataSet *dataSet, int column ) const
{
    int dataSetNumber = d->dataSets.indexOf( dataSet );
    return data( index( dataSetNumber * d->dataDimensions + 2, column ) );
}

QVariant ProxyModel::labelData( DataSet *dataSet ) const
{
    int dataSetNumber = d->dataSets.indexOf( dataSet );
    return headerData( dataSetNumber, d->dataDirection );
}

QList<DataSet*> ProxyModel::dataSets() const
{
    return d->dataSets;
}

#include "ProxyModel.moc"
