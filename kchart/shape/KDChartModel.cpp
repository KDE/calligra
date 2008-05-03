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

using namespace KChart;

class KDChartModel::Private {
public:
    Private();

    int dataDimensions;
    QList<DataSet*> dataSets;
    QList<int> dataSetSizes;
    int biggestDataSetIndex;
};

KDChartModel::Private::Private()
{
	biggestDataSetIndex = -1;
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
    
    if ( d->dataDimensions == 1 )
        return dataSet->yData( row );
    else if ( d->dataDimensions == 2 )
    {
        if ( column % 2 == 0 )
            return dataSet->xData( row );
        else
            return dataSet->yData( row );
    }
    // TODO (Johannes): Support for third data dimension
    
    // Should never happen
    return QVariant();
}

void KDChartModel::dataChanged( const QModelIndex& topLeft, const QModelIndex& bottomRight )
{
    // TODO (Johannes): Emit dataChanged() for all datasets that have references to these rows
    emit QAbstractItemModel::dataChanged( topLeft, bottomRight );
}

void KDChartModel::dataSetSizeChanged( DataSet *dataSet, int newSize )
{
	int dataSetIndex = d->dataSets.indexOf( dataSet );
	
	if ( dataSetIndex < 0 )
	{
		qWarning() << "KDChartModel::dataSetSizeChanged(): The data set is not assigned to this model.";
		return;
	}
	
	if ( newSize > d->dataSetSizes[ d->biggestDataSetIndex ] )
	{
		qDebug() << "Inserting rows:" << d->dataSetSizes[ d->biggestDataSetIndex ] << newSize - 1;
		beginInsertRows( QModelIndex(), d->dataSetSizes[ d->biggestDataSetIndex ], newSize - 1 );
		
		d->dataSetSizes[ dataSetIndex ] = newSize;
		d->biggestDataSetIndex = dataSetIndex;
		
		endInsertRows();
	}
	else if ( newSize < d->dataSetSizes[ d->biggestDataSetIndex ] && dataSetIndex == d->biggestDataSetIndex )
	{
		const int oldMaxSize = d->dataSetSizes[ dataSetIndex ];
		
		int maxSize = 0;
		int biggestDataSetIndex = -1;
		for ( int i = 0; i < d->dataSets.size(); i++ )
		{
			const int size = d->dataSets[i]->size();
			if ( size > maxSize )
			{
				maxSize = size;
				biggestDataSetIndex = i;
			}
		}
		
		if ( maxSize < oldMaxSize )
		{
			beginRemoveRows( QModelIndex(), maxSize, oldMaxSize - 1 );
			d->biggestDataSetIndex = biggestDataSetIndex;
			d->dataSetSizes[ dataSetIndex ] = newSize;
			endRemoveRows();
		}
	}
	else
	{
		d->dataSetSizes[ dataSetIndex ] = newSize;
	}
}

void KDChartModel::slotColumnsInserted( const QModelIndex& parent, int start, int end )
{
    beginInsertRows( parent, start, end );
    endInsertRows();
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
        qWarning() << "KDChartModel::headerData(): Attempting to request header data for non-existant data set!";
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
	if ( d->biggestDataSetIndex < 0 )
		return 0;
	Q_ASSERT( d->biggestDataSetIndex < d->dataSets.count() );
	
    return d->dataSetSizes[ d->biggestDataSetIndex ];
}

int KDChartModel::columnCount( const QModelIndex &parent /* = QModelIndex() */ ) const
{
    return d->dataSets.size() * d->dataDimensions;
}

void KDChartModel::setDataDimensions( int dataDimensions )
{
    d->dataDimensions = dataDimensions;
}

void KDChartModel::addDataSet( DataSet *dataSet, bool silent )
{
    if ( d->dataSets.contains( dataSet ) )
    {
        qWarning() << "KDChartModel::addDataSet(): Attempting to insert already-contained data set";
        return;
    }
    
    if ( silent )
    {
    	d->dataSets.append( dataSet );
    	const int dataSetSize = dataSet->size();
        d->dataSetSizes.append( dataSetSize );
        if ( d->biggestDataSetIndex < 0 || dataSetSize > d->dataSetSizes[ d->biggestDataSetIndex ] )
        	d->biggestDataSetIndex = d->dataSets.count() - 1;
    }
    else
    {
		if ( !d->dataSets.isEmpty() )
		{
		    int columnToBeInserted = columnCount();
		    beginInsertColumns( QModelIndex(), columnToBeInserted, columnToBeInserted + d->dataDimensions - 1 );
		    d->dataSets.append( dataSet );
		    endInsertColumns();
		    
		    const int dataSetSize = dataSet->size();
		    d->dataSetSizes.append( dataSetSize );
		    if ( dataSetSize > d->dataSetSizes[ d->biggestDataSetIndex ] )
		    {
		    	beginInsertRows( QModelIndex(), d->dataSetSizes[ d->biggestDataSetIndex ], dataSetSize - 1 );
		    	d->biggestDataSetIndex = d->dataSets.count() - 1;
		    	endInsertRows();
		    }
		}
		else
		{
		    // If we had no datasets before, we haven't had a valid structure yet
		    // Thus, emit the modelReset() signal
		    d->dataSets.append( dataSet );
		    d->dataSetSizes.append( dataSet->size() );
		    d->biggestDataSetIndex = 0;
		
		    reset();
		}
    }
    
    dataSet->setKdChartModel( this );
}

void KDChartModel::removeDataSet( DataSet *dataSet, bool silent )
{
	const int dataSetIndex = d->dataSets.indexOf( dataSet );
	if ( dataSetIndex < 0 )
		return;
    
    if ( silent )
    {
    	d->dataSets.removeAll( dataSet );
    	
    	const int dataSetSize = dataSet->size();
    	if ( dataSetIndex == d->biggestDataSetIndex )
    	{
    		int maxSize = 0;
    		for ( int i = 0; i < d->dataSets.size(); i++ )
    		{
    			const int size = d->dataSets[i]->size();
    			if ( size > maxSize )
    			{
    				maxSize = size;
    				d->biggestDataSetIndex = i;
    			}
    		}
    	}
    }
    else
    {
    	const int dataSetSize = dataSet->size();
    	if ( dataSetIndex == d->biggestDataSetIndex )
    	{
    		int maxSize = 0;
    		int biggestDataSetIndex = -1;
    		for ( int i = 0; i < d->dataSets.size(); i++ )
    		{
    			if ( d->dataSets[i] == dataSet )
    				continue;
    			const int size = d->dataSets[i]->size();
    			if ( size > maxSize )
    			{
    				maxSize = size;
    				biggestDataSetIndex = i;
    			}
    		}
    		
    		if ( maxSize < dataSetSize )
    		{
    			beginRemoveRows( QModelIndex(), maxSize, dataSetSize - 1 );
    			d->biggestDataSetIndex = biggestDataSetIndex;
    			endRemoveRows();
    		}
    	}
    	
    	int columnAboutToBeRemoved = d->dataSets.indexOf( dataSet ) * d->dataDimensions;
    	beginRemoveColumns( QModelIndex(), columnAboutToBeRemoved, columnAboutToBeRemoved + d->dataDimensions - 1 );
    	d->dataSets.removeAt( dataSetIndex );
        d->dataSetSizes.removeAt( dataSetIndex );
    	endRemoveColumns();
    }
    
    dataSet->setKdChartModel( 0 );
}

QList<DataSet*> KDChartModel::dataSets() const
{
    return d->dataSets;
}

void KDChartModel::emitReset()
{
	d->biggestDataSetIndex = -1;
	// Update data set sizes
	int maxSize = 0;
	for ( int i = 0; i < d->dataSets.size(); i++ )
	{
		d->dataSetSizes[i] = d->dataSets[i]->size();
		if ( d->dataSetSizes[i] > maxSize )
		{
			maxSize = d->dataSetSizes[i];
			d->biggestDataSetIndex = i;
		}
	}
	reset();
}

#include "KDChartModel.moc"
