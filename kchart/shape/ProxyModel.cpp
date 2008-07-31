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
#include "Axis.h"
#include "DataSet.h"
#include "CellDataSet.h"
#include <interfaces/KoChartModel.h>

// Qt
#include <QRegion>

// KDE
#include <KDebug>

using namespace KChart;

class ProxyModel::Private {
public:
    Private();
    ~Private();
    
    void resetDefaultDataSetProperties();

    bool             firstRowIsLabel;
    bool             firstColumnIsLabel;
    Qt::Orientation  dataDirection;
    int dataDimensions;
    QMap<int, int> dataMap;
    
    QList<DataSet*> dataSets;
    QList<DataSet*> removedDataSets;
    
    QVector<QRect> selection;
    QAbstractItemModel *spreadSheetModel;
};

ProxyModel::Private::Private()
{
    firstRowIsLabel    = false;
    firstColumnIsLabel = false;
    dataDimensions = 1;

    dataDirection = Qt::Horizontal;
    spreadSheetModel = 0;
}

void ProxyModel::Private::resetDefaultDataSetProperties()
{
    for ( int i = 0; i < dataSets.size(); i++ )
    {
        dataSets[ i ]->setNumber( i );
        dataSets[ i ]->setColor( defaultDataSetColor( i ) );
    }
}

ProxyModel::ProxyModel()
    : QAbstractProxyModel( 0 ),
      d( new Private )
{
}

ProxyModel::~ProxyModel()
{
}

void ProxyModel::rebuildDataMap()
{
	if ( d->spreadSheetModel )
	{
        int k = 0;
	    if ( d->dataDirection == Qt::Horizontal )
	    {
	        QMap<int, QVector<QRect> > rows;
	        QMap<int, QVector<QRect> > sortedRows;
	        // Split up region in horizontal rectangles
	        // that are sorted from top to bottom
	        foreach ( const QRect &rect, d->selection )
	        {
	            int x = rect.topLeft().x();
	            for ( int y = rect.topLeft().y(); y <= rect.bottomLeft().y(); y++ )
	            {
	                QRect dataRect = QRect( QPoint( x, y ), QSize( rect.width(), 1 ) );
	                if ( !rows.contains( y ) )
	                    rows.insert( y, QVector<QRect>() );
	                rows[y].append( dataRect );
	            }
	        }
	        
	        // Sort rectangles in each row from left to right
	        QMapIterator<int, QVector<QRect> > i( rows );
	        while ( i.hasNext() )
	        {
	            i.next();
	            int row = i.key();
	            QVector<QRect> unsortedRects = i.value();
	            QVector<QRect> sortedRects;
	            
	            foreach ( const QRect &rect, unsortedRects )
	            {
	                int index;
	                
	                for ( index = 0; index < sortedRects.size(); index++ )
	                    if ( rect.topLeft().x() <= sortedRects[ index ].topLeft().x() )
	                        break;
	                
	                sortedRects.insert( index, rect );
	            }
	            
	            sortedRows.insert( row, sortedRects );
	        }
	        
	        QMapIterator<int, QVector<QRect> > j( sortedRows );
	        
	        CellRegion categoryRegion;
	        
	        if ( d->firstRowIsLabel && j.hasNext() )
	        {
	            j.next();
	            
	            categoryRegion = CellRegion( j.value() );
	            if ( d->firstColumnIsLabel )
	                categoryRegion.subtract( categoryRegion.pointAtIndex( 0 ) );
	        }
	        
	        while ( j.hasNext() )
	        {
	            j.next();
	            
	            DataSet *dataSet;
	            if ( k >= d->dataSets.size() )
	            {
	                if ( !d->removedDataSets.isEmpty() )
	                    dataSet = d->removedDataSets.takeLast();
	                else
	                    dataSet = new CellDataSet( this );
	                d->dataSets.append( dataSet );
	            }
	            else
	                dataSet = d->dataSets[k];
	            dataSet->blockSignals( true );
	            
	            CellRegion yDataRegion( j.value() );
	    
	            qDebug() << "Creating data set with region " << j.value();
	            if ( d->firstColumnIsLabel )
	            {
	                QPoint labelDataPoint = yDataRegion.pointAtIndex( 0 );
	                dataSet->setLabelDataRegion( CellRegion( labelDataPoint ) );
	                dataSet->setCategoryDataRegion( categoryRegion );
	                
	                yDataRegion.subtract( labelDataPoint );
	            }
	            
	            dataSet->setYDataRegion( yDataRegion );
	            k++;
	            dataSet->blockSignals( false );
	        }
	    }
	    else
	    {
            QMap<int, QVector<QRect> > columns;
            QMap<int, QVector<QRect> > sortedColumns;
            // Split up region in horizontal rectangles
            // that are sorted from top to bottom
            foreach ( const QRect &rect, d->selection )
            {
                int y = rect.topLeft().y();
                for ( int x = rect.topLeft().x(); x <= rect.topRight().x(); x++ )
                {
                    QRect dataRect = QRect( QPoint( x, y ), QSize( 1, rect.height() ) );
                    if ( !columns.contains( x ) )
                        columns.insert( x, QVector<QRect>() );
                    columns[x].append( dataRect );
                }
            }
            
            // Sort rectangles in each column from top to bottom
            QMapIterator<int, QVector<QRect> > i( columns );
            while ( i.hasNext() )
            {
                i.next();
                int row = i.key();
                QVector<QRect> unsortedRects = i.value();
                QVector<QRect> sortedRects;
                
                foreach ( const QRect &rect, unsortedRects )
                {
                    int index;
                    
                    for ( index = 0; index < sortedRects.size(); index++ )
                        if ( rect.topLeft().y() <= sortedRects[ index ].topLeft().y() )
                            break;
                    
                    sortedRects.insert( index, rect );
                }
                
                sortedColumns.insert( row, sortedRects );
            }
            
            QMapIterator<int, QVector<QRect> > j( sortedColumns );
            
            CellRegion categoryRegion;
            
            if ( d->firstColumnIsLabel && j.hasNext() )
            {
                j.next();
                
                categoryRegion = CellRegion( j.value() );
                if ( d->firstRowIsLabel )
                    categoryRegion.subtract( categoryRegion.pointAtIndex( 0 ) );
            }
            
            while ( j.hasNext() )
            {
                j.next();
                
                DataSet *dataSet;
                if ( k >= d->dataSets.size() )
                {
                    if ( !d->removedDataSets.isEmpty() )
                        dataSet = d->removedDataSets.takeLast();
                    else
                        dataSet = new CellDataSet( this );
                    d->dataSets.append( dataSet );
                }
                else
                    dataSet = d->dataSets[k];
                dataSet->blockSignals( true );
                
                CellRegion yDataRegion( j.value() );
        
                qDebug() << "Creating data set with region " << j.value();
                if ( d->firstRowIsLabel )
                {
                    QPoint labelDataPoint = yDataRegion.pointAtIndex( 0 );
                    dataSet->setLabelDataRegion( CellRegion( labelDataPoint ) );
                    dataSet->setCategoryDataRegion( categoryRegion );
                    
                    yDataRegion.subtract( labelDataPoint );
                }
                
                dataSet->setYDataRegion( yDataRegion );
                k++;
                dataSet->blockSignals( false );
            }
	    }
	    
	    while ( d->dataSets.size() > k + 1 )
	    {
	    	DataSet *dataSet = d->dataSets.takeLast();
	    	dataSet->setKdChartModel( 0 );
	        d->removedDataSets.append( dataSet );
	    }
	}
	else
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
	        	if ( !d->removedDataSets.isEmpty() )
	        		d->dataSets.append( d->removedDataSets.takeLast() );
	        	else
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
	    
	    while ( d->dataSets.size() > dataSetCount )
	    {
	    	DataSet *dataSet = d->dataSets.takeLast();
	    	if ( dataSet->attachedAxis() )
	    		dataSet->attachedAxis()->detachDataSet( dataSet );
	        d->removedDataSets.append( dataSet );
	    }
	}
	
	d->resetDefaultDataSetProperties();
}

void ProxyModel::setSourceModel( QAbstractItemModel *sourceModel )
{
    connect( sourceModel, SIGNAL( dataChanged( const QModelIndex&, const QModelIndex& ) ),
             this,        SLOT( dataChanged( const QModelIndex&, const QModelIndex& ) ) );
    
    // We now need DataSets instead of CellDataSets
    if ( d->spreadSheetModel )
    {
    	while ( !d->dataSets.isEmpty() )
    	{
    		DataSet *dataSet = d->dataSets.takeLast();
    		if ( dataSet )
    			delete dataSet;
    	}
    	while ( !d->removedDataSets.isEmpty() )
    	{
    		DataSet *dataSet = d->removedDataSets.takeLast();
    		if ( dataSet )
    			delete dataSet;
    	}
    }
    
    d->spreadSheetModel = 0;

    QAbstractProxyModel::setSourceModel( sourceModel );
    
    rebuildDataMap();

    // Update the entire data set
    reset();
}

void ProxyModel::setSourceModel( QAbstractItemModel *sourceModel, const QVector<QRect> &selection )
{
    connect( sourceModel, SIGNAL( dataChanged( const QModelIndex&, const QModelIndex& ) ),
                            this,  SLOT( dataChanged( const QModelIndex&, const QModelIndex& ) ) );
    
    d->selection = selection;
    
    // We now need CellDataSets instead of DataSets
    if ( !d->spreadSheetModel )
    {
    	while ( !d->dataSets.isEmpty() )
    	{
    		DataSet *dataSet = d->dataSets.takeLast();
    		if ( dataSet )
    			delete dataSet;
    	}
    	while ( !d->removedDataSets.isEmpty() )
    	{
    		DataSet *dataSet = d->removedDataSets.takeLast();
    		if ( dataSet )
    			delete dataSet;
    	}
    }

    QAbstractProxyModel::setSourceModel( sourceModel );
    
    d->spreadSheetModel = sourceModel;
    
    rebuildDataMap();

    // Update the entire data set
    reset();
}

QAbstractItemModel *ProxyModel::spreadSheetModel() const
{
    return d->spreadSheetModel;
}

void ProxyModel::setSelection( const QVector<QRect> &selection )
{
    d->selection = selection;
    //needReset();
}

DataSet *ProxyModel::createDataSet()
{
    if ( !d->spreadSheetModel )
        return 0;
    
    DataSet *dataSet = new CellDataSet( this );
    d->dataSets.append( dataSet );
    
    dataSet->setColor( defaultDataSetColor( d->dataSets.size() - 1 ) );
    dataSet->setNumber( d->dataSets.size() - 1 );
    
    return dataSet;
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
	if ( d->spreadSheetModel )
	{
	    QPoint topLeftPoint( topLeft.column(), topLeft.row() );
	    // Excerpt from the Qt reference for QRect::bottomRight() which is used for calculating bottomRight
	    // Note that for historical reasons this function returns QPoint(left() + width() -1, top() + height() - 1).
	    QPoint bottomRightPoint( bottomRight.column() + 1, bottomRight.row() + 1 );
	    QRect dataChangedRect = QRect( topLeftPoint, QSize( bottomRightPoint.x() - topLeftPoint.x(), bottomRightPoint.y() - topLeftPoint.y() ) );
	    
	    foreach ( DataSet *dataSet, d->dataSets )
	    {
	        bool intersects = false;
	        QRect changedRect;
	        foreach ( const QRect &rect, dataSet->yDataRegion().rects() )
	        {
	            if ( rect.intersects( dataChangedRect ) )
	            {
	                changedRect |= rect.intersected( dataChangedRect );
	                intersects = true;
	                break;
	            }
	        }
	        if ( intersects )
	        {
	            dataSet->yDataChanged( changedRect );
	        }
	    }
	}
	else
	{
	    int firstRow, lastRow;
	    int firstCol, lastCol;
	    int numRows;
	    if ( d->dataDirection == Qt::Horizontal )
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
	            if ( d->dataDimensions == 1 )
	            {
    	            Q_ASSERT( i < d->dataSets.size() );
    	            d->dataSets[ i ]->yDataChanged( firstCol, lastCol );
	            }
	            else if ( d->dataDimensions == 2 )
                {
                    int dataSet = int( i / 2 );
                    Q_ASSERT( dataSet < d->dataSets.size() );
                    if ( i % 2 == 0 )
                        d->dataSets[ dataSet ]->yDataChanged( firstCol, lastCol );
                    else
                        d->dataSets[ dataSet ]->xDataChanged( firstCol, lastCol );
                }
	            else
	            {
	                Q_ASSERT_X( false, "ProxyModel::dataChanged", "No more than one data direction supported" );
	            }
	        }
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

    if ( orientation == Qt::Horizontal ) {
        if ( !d->firstColumnIsLabel )
            return QVariant();

        // Return the first column in the section-th row
        row = section;
        if ( d->firstRowIsLabel )
            row++;
        // first source row is used for x values
        if ( d->dataDimensions == 2 )
            row++;
    }
    else {
        if( !d->firstRowIsLabel )
            return QVariant();

        // Return the section-th column in the first row
        column = section;
        if ( d->firstColumnIsLabel )
            column++;
        // first source column is used for x values
        if ( d->dataDimensions == 2 )
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
    if ( d->dataDirection == Qt::Horizontal ) {
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
    if ( d->dataDirection == Qt::Horizontal ) {
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
    if ( d->dataDirection == Qt::Horizontal )
        return orientation;

    // orientation is Qt::Horizontal
    // Thus, we need to return the opposite of orientation.
    if ( orientation == Qt::Vertical )
        return Qt::Horizontal;
    return Qt::Vertical;
}

Qt::Orientation ProxyModel::mapToSource( Qt::Orientation orientation ) const
{
    if ( d->dataDirection == Qt::Horizontal )
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
    if ( d->dataDirection == Qt::Horizontal )
        rowCount = sourceModel()->rowCount( parent );
    else
        rowCount = sourceModel()->columnCount( parent );
    // Even if the first row is a header - if the data table is empty,
    // we still have 0 rows, not -1

    bool firstRowIsLabel = d->firstRowIsLabel;
    if ( d->dataDirection == Qt::Vertical )
        firstRowIsLabel = d->firstColumnIsLabel;

    if ( rowCount > 0 && firstRowIsLabel )
        rowCount--;
    
    // One row is used for x values
    if ( d->dataDimensions == 2 )
        rowCount--;
    
    rowCount *= d->dataDimensions;

    return rowCount;
}

int ProxyModel::columnCount( const QModelIndex &parent /* = QModelIndex() */ ) const
{
    if ( sourceModel() == 0 )
        return 0;
    

    int columnCount;
    if ( d->dataDirection == Qt::Horizontal )
        columnCount = sourceModel()->columnCount( parent );
    else
        columnCount = sourceModel()->rowCount( parent );
    // Even if the first column is a header - if the data table is empty,
    // we still have 0 columns, not -1

    bool firstColumnIsLabel = d->firstColumnIsLabel;
    if ( d->dataDirection == Qt::Vertical )
        firstColumnIsLabel = d->firstRowIsLabel;

    if ( columnCount > 0 && firstColumnIsLabel )
        columnCount--;
    
    return columnCount;
}

void ProxyModel::setFirstRowIsLabel( bool b )
{
    if ( b == d->firstRowIsLabel )
        return;
    
    /*if ( b ) {
        if ( d->dataDirection == Qt::Horizontal )
            beginRemoveColumns( QModelIndex(), 0, 0 );
        else
            beginRemoveRows( QModelIndex(), 0, 0 );
    } else {
        if ( d->dataDirection == Qt::Horizontal )
            beginInsertColumns( QModelIndex(), 0, 0 );
        else
            beginInsertRows( QModelIndex(), 0, 0 );
    }*/
    
    d->firstRowIsLabel = b;
    
    if ( !sourceModel() )
        return;
    
    rebuildDataMap();
    reset();
    
    /*if ( b ) {
        if ( d->dataDirection == Qt::Horizontal )
            endRemoveColumns();
        else
            endRemoveRows();
    } else {
        if ( d->dataDirection == Qt::Horizontal )
            endInsertColumns();
        else
            endInsertRows();
    }*/
}
 
void ProxyModel::setFirstColumnIsLabel( bool b )
{
    /*if ( b == d->firstColumnIsLabel )
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
    }*/
    
    d->firstColumnIsLabel = b;
    
    if ( !sourceModel() )
        return;
    
    rebuildDataMap();
    reset();
    
    /*if ( b ) {
        if ( d->dataDirection == Qt::Vertical )
            endRemoveColumns();
        else
            endRemoveRows();
    } else {
        if ( d->dataDirection == Qt::Vertical )
            endInsertColumns();
        else
            endInsertRows();
    }*/
}

Qt::Orientation ProxyModel::dataDirection()
{
    return d->dataDirection;
}

void ProxyModel::setDataDirection( Qt::Orientation orientation )
{
    d->dataDirection = orientation;
    rebuildDataMap();
    reset();
}

void ProxyModel::setDataDimensions( int dimensions )
{
    d->dataDimensions = dimensions;
    rebuildDataMap();
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

QList<DataSet*> ProxyModel::dataSets() const
{
    return d->dataSets;
}

QVariant ProxyModel::xData( const DataSet *dataSet, int column ) const
{
    int dataSetNumber = d->dataSets.indexOf( const_cast<DataSet*>( dataSet ) );
    return data( index( dataSetNumber * d->dataDimensions + 1, column ) );
}

QVariant ProxyModel::yData( const DataSet *dataSet, int column ) const
{
    int dataSetNumber = d->dataSets.indexOf( const_cast<DataSet*>( dataSet ) );
    return data( index( dataSetNumber * d->dataDimensions, column ) );
}

QVariant ProxyModel::customData( const DataSet *dataSet, int column ) const
{
    int dataSetNumber = d->dataSets.indexOf( const_cast<DataSet*>( dataSet ) );
    return data( index( dataSetNumber * d->dataDimensions + 2, column ) );
}

QVariant ProxyModel::labelData( const DataSet *dataSet ) const
{
    int dataSetNumber = d->dataSets.indexOf( const_cast<DataSet*>( dataSet ) );
    return headerData( dataSetNumber, d->dataDirection );
}

QVariant ProxyModel::categoryData( const DataSet *dataSet, int column ) const
{
    Q_UNUSED( dataSet );
    return headerData( column, d->dataDirection == Qt::Vertical ? Qt::Horizontal : Qt::Vertical );
}

#include "ProxyModel.moc"
