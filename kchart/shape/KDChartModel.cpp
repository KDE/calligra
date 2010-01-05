/* This file is part of the KDE project

   Copyright 2008 Johannes Simon <johannes.simon@gmail.com>
   Copyright 2009 Inge Wallin    <inge@lysator.liu.se>

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


// Own
#include "KDChartModel.h"

// KDE
#include <KDebug>

// KChart
#include "DataSet.h"


using namespace KChart;

class KDChartModel::Private {
public:
    Private();
    ~Private();
    
    int   dataSetIndex( DataSet *dataSet ) const;
    int   maxDataSetSize() const;
    void  updateMaxDataSetSize();

    int             dataDimensions;
    QList<DataSet*> dataSets;
    int             biggestDataSetIndex;
    
    Qt::Orientation dataDirection;
};


KDChartModel::Private::Private()
{
    biggestDataSetIndex = -1;
    dataDimensions      = 1;
    dataDirection       = Qt::Vertical;
}

KDChartModel::Private::~Private()
{
}

int KDChartModel::Private::maxDataSetSize() const
{
    // There is no stored index yet. Default to 0, do not assert.
    if ( biggestDataSetIndex < 0 )
        return 0;

    // Check if stored index (of dataset with max size) is valid
    const bool indexIsValid = biggestDataSetIndex < dataSets.size();
    Q_ASSERT( indexIsValid );
    if ( !indexIsValid )
        return 0;

    Q_ASSERT( dataSets[ biggestDataSetIndex ] );
    return dataSets[ biggestDataSetIndex ]->size();
}

int KDChartModel::Private::dataSetIndex( DataSet *dataSet ) const
{
    for ( int i = 0; i < dataSets.size(); i++ ) {
        if ( dataSet->number() < dataSets[ i ]->number() )
            return i;
    }

    return dataSets.size();
}


// ================================================================
//                     class KDChartModel


KDChartModel::KDChartModel( QObject *parent /* = 0 */ )
    : QAbstractItemModel( parent ),
      d( new Private )
{
}

KDChartModel::~KDChartModel()
{
    delete d;
}


void KDChartModel::setDataDirection( Qt::Orientation direction )
{
    d->dataDirection = direction;
}

Qt::Orientation KDChartModel::dataDirection() const
{
    return d->dataDirection;
}

QVariant KDChartModel::data( const QModelIndex &index,
                             int role /* = Qt::DisplayRole */ ) const
{
    if ( role != Qt::DisplayRole )
        return QVariant();
    
    int row;
    int column;

    if ( d->dataDirection == Qt::Vertical ) {
        row = index.row();
        column = index.column();
    }
    else {
        row = index.column();
        column = index.row();
    }
    int       dataSetNumber = column / d->dataDimensions;
    DataSet  *dataSet       = d->dataSets[ dataSetNumber ];
    
    if ( d->dataDimensions == 1 )
        return dataSet->yData( row );
    else if ( d->dataDimensions == 2 ) {
        if ( column % 2 == 0 )
            return dataSet->xData( row );
        else
            return dataSet->yData( row );
    }
    else if ( d->dataDimensions == 3 ) {
        // TODO (Johannes): Support for third data dimension
        // We need to implement zData in Dataset first.
        return dataSet->xData( row );
    }
    
    // Should never happen
    return QVariant();
}

void KDChartModel::dataChanged( const QModelIndex& topLeft,
                                const QModelIndex& bottomRight )
{
    // TODO (Johannes): Emit dataChanged() for all datasets that have
    //                  references to these rows
    emit QAbstractItemModel::dataChanged( topLeft, bottomRight );
}

void KDChartModel::dataSetSizeChanged( DataSet *dataSet, int newSize )
{
    Q_UNUSED( newSize );

    int dataSetIndex = d->dataSets.indexOf( dataSet );
    if ( dataSetIndex < 0 ) {
        qWarning() << "KDChartModel::dataSetSizeChanged(): The data set is not assigned to this model.";
        return;
    }

    // Check if stored max size (i.e. the index of the dataset having
    // that max size) is valid.
    Q_ASSERT( d->biggestDataSetIndex >= 0
              && d->biggestDataSetIndex < d->dataSets.size() );
    const int oldMaxSize = d->dataSets[ d->biggestDataSetIndex ]->size();

    // Determine new max data set size.
    //
    // Do not use updateMaxDataSetSize() here. d->biggestDataSetIndex
    // must not be changed before begin<Insert/Remove><Rows/Columns>()
    // signal is emitted.
    int maxSize = 0;
    int biggestDataSetIndex = -1;
    for ( int i = 0; i < d->dataSets.size(); i++ ) {
        const int size = d->dataSets[i]->size();
        if ( size > maxSize ) {
            maxSize = size;
            biggestDataSetIndex = i;
        }
    }

    // Columns/rows have been added
    if ( maxSize > oldMaxSize ) {
        if ( d->dataDirection == Qt::Vertical )
            beginInsertRows( QModelIndex(), oldMaxSize, newSize - 1 );
        else
            beginInsertColumns( QModelIndex(), oldMaxSize, newSize - 1 );

        d->biggestDataSetIndex = biggestDataSetIndex;

        if ( d->dataDirection == Qt::Vertical )
            endInsertRows();
        else
            endInsertColumns();
        // Columns/rows have been removed
    } else if ( maxSize < oldMaxSize ) {
        if ( d->dataDirection == Qt::Vertical )
            beginRemoveRows( QModelIndex(), maxSize, oldMaxSize - 1 );
        else
            beginRemoveColumns( QModelIndex(), maxSize, oldMaxSize - 1 );

        d->biggestDataSetIndex = biggestDataSetIndex;

        if ( d->dataDirection == Qt::Vertical )
            endRemoveRows();
        else
            endRemoveColumns();
    }
}

void KDChartModel::slotColumnsInserted( const QModelIndex& parent, 
                                        int start, int end )
{
    if ( d->dataDirection == Qt::Vertical ) {
        beginInsertRows( parent, start, end );
        endInsertRows();
    }
    else {
        beginInsertColumns( parent, start, end );
        endInsertColumns();
    }
}

QVariant KDChartModel::headerData( int section,
                                   Qt::Orientation orientation,
                                   int role /* = Qt::DisplayRole */ ) const
{
    if ( role != Qt::DisplayRole ) {
        return QVariant();
    }
    
    if ( d->dataSets.isEmpty() ) {
        qWarning() << "KDChartModel::headerData(): Attempting to request header, but model has no datasets assigned to it.";
        return QVariant();
    }
    
    if ( orientation != d->dataDirection ) {
        int dataSetNumber = section / d->dataDimensions;
        Q_ASSERT( dataSetNumber < d->dataSets.count() );

        DataSet *dataSet  = d->dataSets[ dataSetNumber ];
        return dataSet->labelData();
    }
    
    DataSet *dataSet = d->dataSets[ 0 ];
    return dataSet->categoryData( section );
}

QModelIndex KDChartModel::index( int row, int column,
				 const QModelIndex &parent ) const
{
    Q_UNUSED( parent );

    return createIndex( row, column, 0 );
}

QModelIndex KDChartModel::parent( const QModelIndex &index ) const
{
    Q_UNUSED( index );

    return QModelIndex();
}

int KDChartModel::rowCount( const QModelIndex &parent /* = QModelIndex() */ ) const
{
    Q_UNUSED( parent );

    if ( d->dataDirection == Qt::Vertical )
        return d->maxDataSetSize();

    return d->dataSets.size() * d->dataDimensions;
}

int KDChartModel::columnCount( const QModelIndex &parent /* = QModelIndex() */ ) const
{
    Q_UNUSED( parent );

    if ( d->dataDirection == Qt::Vertical )
        return d->dataSets.size() * d->dataDimensions;

    return d->maxDataSetSize();    
}

void KDChartModel::setDataDimensions( int dataDimensions )
{
    d->dataDimensions = dataDimensions;
}

int KDChartModel::dataDimensions() const
{
    return d->dataDimensions;
}

void KDChartModel::addDataSet( DataSet *dataSet, bool silent )
{
    if ( d->dataSets.contains( dataSet ) ) {
        qWarning() << "KDChartModel::addDataSet(): Attempting to insert already-contained data set";
        return;
    }
    
    int dataSetIndex = d->dataSetIndex( dataSet );
    
    if ( silent ) {
    	d->dataSets.insert( dataSetIndex, dataSet );
    	const int dataSetSize = dataSet->size();
        if ( d->biggestDataSetIndex < 0 || dataSetSize > d->maxDataSetSize() )
        	d->biggestDataSetIndex = dataSetIndex;
    }
    else if ( !d->dataSets.isEmpty() ) {
        const int columnAboutToBeInserted = dataSetIndex * d->dataDimensions;
        if ( d->dataDirection == Qt::Vertical )
            beginInsertColumns( QModelIndex(), columnAboutToBeInserted,
                                columnAboutToBeInserted + d->dataDimensions - 1 );
        else
            beginInsertRows( QModelIndex(), columnAboutToBeInserted, 
                             columnAboutToBeInserted + d->dataDimensions - 1 );
        d->dataSets.insert( dataSetIndex, dataSet );
        if ( d->dataDirection == Qt::Vertical )
            endInsertColumns();
        else
            endInsertRows();
		    
        const int dataSetSize = dataSet->size();
        if ( dataSetSize > d->maxDataSetSize() ) {
            if ( d->dataDirection == Qt::Vertical )
                beginInsertRows( QModelIndex(),
                                 d->maxDataSetSize(), dataSetSize - 1 );
            else
                beginInsertColumns( QModelIndex(),
                                    d->maxDataSetSize(), dataSetSize - 1 );
            d->biggestDataSetIndex = dataSetIndex;
            if ( d->dataDirection == Qt::Vertical )
                endInsertRows();
            else
                endInsertColumns();
        }
        else {
            // Update index if we inserted another data set *before* it.
            if ( dataSetIndex <= d->biggestDataSetIndex )
                d->biggestDataSetIndex++;
        }
    }
    else {
        // If we had no datasets before, we haven't had a valid
        // structure yet.  Thus, emit the modelReset() signal.
        d->dataSets.append( dataSet );
        d->biggestDataSetIndex = 0;
		
        reset();
    }
    
    // Regenerate the numbers for all the datasets.
    for ( int i = dataSetIndex; i < d->dataSets.size(); i++ ) {
        d->dataSets[ i ]->setKdDataSetNumber( i );
    }
    
    dataSet->setKdChartModel( this );
}

void KDChartModel::removeDataSet( DataSet *dataSet, bool silent )
{
    const int dataSetIndex = d->dataSets.indexOf( dataSet );
    if ( dataSetIndex < 0 )
        return;
    
    if ( silent ) {
    	d->dataSets.removeAll( dataSet );
    	
    	const int dataSetSize = dataSet->size();
    	if ( dataSetIndex == d->biggestDataSetIndex ) {
            int maxSize = 0;
            for ( int i = 0; i < d->dataSets.size(); i++ ) {
                const int size = d->dataSets[i]->size();
                if ( size > maxSize ) {
                    maxSize = size;
                    d->biggestDataSetIndex = i;
                }
            }
        }

        if ( dataSetIndex < d->biggestDataSetIndex )
            d->biggestDataSetIndex--;
    }
    else {
    	const int dataSetSize = dataSet->size();
    	if ( dataSetIndex == d->biggestDataSetIndex ) {
            int maxSize = 0;
            int biggestDataSetIndex = -1;
            for ( int i = 0; i < d->dataSets.size(); i++ ) {
                if ( i == dataSetIndex )
                    continue;

                const int size = d->dataSets[i]->size();
                if ( size > maxSize ) {
                    maxSize = size;
                    biggestDataSetIndex = i;
                }
            }
    		
            if ( maxSize < dataSetSize ) {
                if ( d->dataDirection == Qt::Vertical )
                    beginRemoveRows( QModelIndex(), maxSize, dataSetSize - 1 );
                else
                    beginRemoveColumns( QModelIndex(), maxSize, dataSetSize - 1 );
                d->biggestDataSetIndex = biggestDataSetIndex;
                if ( d->dataDirection == Qt::Vertical )
                    endRemoveRows();
                else
                    endRemoveColumns();
            }
            else {
                d->biggestDataSetIndex = biggestDataSetIndex;
            }
        }
    	
    	if ( dataSetIndex < d->biggestDataSetIndex )
    	    d->biggestDataSetIndex--;
    	
    	int columnAboutToBeRemoved = dataSetIndex * d->dataDimensions;
    	if ( d->dataDirection == Qt::Vertical )
    	    beginRemoveColumns( QModelIndex(), columnAboutToBeRemoved,
                                columnAboutToBeRemoved + d->dataDimensions - 1 );
    	else
            beginRemoveRows( QModelIndex(), columnAboutToBeRemoved, 
                             columnAboutToBeRemoved + d->dataDimensions - 1 );
    	d->dataSets.removeAt( dataSetIndex );
        if ( d->dataDirection == Qt::Vertical )
            endRemoveColumns();
        else
            endRemoveRows();
    }
    
    for ( int i = dataSetIndex; i < d->dataSets.size(); i++ ) {
        d->dataSets[ i ]->setKdDataSetNumber( i );
    }
}

QList<DataSet*> KDChartModel::dataSets() const
{
    return d->dataSets;
}

void KDChartModel::emitReset()
{
    d->biggestDataSetIndex = -1;

    // Update max data set sizes
    int maxSize = 0;
    for ( int i = 0; i < d->dataSets.size(); i++ ) {
        if ( d->dataSets[i]->size() > maxSize ) {
            maxSize = d->dataSets[i]->size();
            d->biggestDataSetIndex = i;
        }
    }

    emit reset();
}

#include "KDChartModel.moc"
