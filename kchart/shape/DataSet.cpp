/* This file is part of the KDE project

   Copyright 2007-2008 Johannes Simon <johannes.simon@gmail.com>

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
 * Boston, MA 02110-1301, USA.
*/

// Local
#include "DataSet.h"
#include "CellRegion.h"
#include "ProxyModel.h"
#include "Axis.h"
#include "PlotArea.h"
#include "KDChartModel.h"
#include <interfaces/KoChartModel.h>

// Qt
#include <QAbstractItemModel>
#include <QString>
#include <QPen>
#include <QColor>

// KDChart
#include <KDChartDataValueAttributes>
#include <KDChartAbstractDiagram>

using namespace KChart;

class DataSet::Private
{
public:
    Private();
    ~Private();
    
    ChartType chartType;
    ChartSubtype chartSubType;
    Axis *attachedAxis;
    bool showMeanValue;
    QPen meanValuePen;
    bool showValues;
    bool showLabels;
    bool showLowerErrorIndicator;
    bool showUpperErrorIndicator;
    QPen errorIndicatorPen;
    ErrorCategory errorCategory;
    double errorPercentage;
    double errorMargin;
    double lowerErrorLimit;
    double upperErrorLimit;
    QPen pen;
    QBrush brush;
    
    CellRegion xDataRegion;
    CellRegion yDataRegion;
    CellRegion customDataRegion;
    CellRegion labelDataRegion;
    CellRegion categoryDataRegion;
    
    Qt::Orientation xDataDirection;
    Qt::Orientation yDataDirection;
    Qt::Orientation customDataDirection;
    Qt::Orientation labelDataDirection;
    Qt::Orientation categoryDataDirection;
    
    PlotArea *plotArea;
    
    ProxyModel *model;
    KoChart::ChartModel *spreadSheetModel;
    KDChart::AbstractDiagram *kdDiagram;
    int kdDataSetNumber;
    
    bool sourceIsSpreadSheet;
    
    QList<KDChartModel*> kdChartModels;
    
    int size;
    bool blockSignals;
};

DataSet::Private::Private()
{
    chartType = LastChartType;
    chartSubType = NoChartSubtype;
    kdDataSetNumber = 0;
    showMeanValue = false;
    showValues = false;
    showLabels = false;
    showLowerErrorIndicator = false;
    showUpperErrorIndicator = false;
    errorPercentage = 0.0;
    errorMargin = 0.0;
    lowerErrorLimit = 0.0;
    upperErrorLimit = 0.0;
    pen = QPen( Qt::black );
    plotArea = 0;
    model = 0;
    kdDiagram = 0;
    attachedAxis = 0;
    sourceIsSpreadSheet = false;
    spreadSheetModel = 0;
    size = 0;
    blockSignals = false;
}

DataSet::Private::~Private()
{
}


DataSet::DataSet( ProxyModel *proxyModel )
    : d( new Private )
{
    d->model = proxyModel;
}

DataSet::~DataSet()
{
    if ( d->attachedAxis )
        d->attachedAxis->detachDataSet( this, true );
}


ChartType DataSet::chartType() const
{
    return d->chartType;
}


ChartSubtype DataSet::chartSubType() const
{
    return d->chartSubType;
}

Axis *DataSet::attachedAxis() const
{
    return d->attachedAxis;
}

ProxyModel *DataSet::model() const
{
    return d->model;
}

bool DataSet::showMeanValue() const
{
    return d->showMeanValue;
}

QPen DataSet::meanValuePen() const
{
    return d->meanValuePen;   
}

bool DataSet::showLowerErrorIndicator() const
{
    return d->showLowerErrorIndicator;
}

bool DataSet::showUpperErrorIndicator() const
{
    return d->showUpperErrorIndicator;
}

QPen DataSet::errorIndicatorPen() const
{
    return d->errorIndicatorPen;
}

ErrorCategory DataSet::errorCategory() const
{
    return d->errorCategory;
}

double DataSet::errorPercentage() const
{
    return d->errorPercentage;
}

double DataSet::errorMargin() const
{
    return d->errorMargin;   
}

double DataSet::lowerErrorLimit() const
{
    return d->lowerErrorLimit;
}

double DataSet::upperErrorLimit() const
{
    return d->upperErrorLimit;
}


void DataSet::setChartType( ChartType type )
{
    Axis *axis = d->attachedAxis;
    axis->detachDataSet( this );
    
    d->chartType = type;
    
    axis->attachDataSet( this );
}

void DataSet::setChartSubType( ChartSubtype subType )
{
    Axis *axis = d->attachedAxis;
    axis->detachDataSet( this );
    
    d->chartSubType = subType;
    
    axis->attachDataSet( this );
}


void DataSet::setAttachedAxis( Axis *axis )
{
    d->attachedAxis = axis;
}

bool DataSet::showValues() const
{
    return d->showValues;
}

bool DataSet::showLabels() const
{
    return d->showLabels;
}

void DataSet::setShowValues( bool showValues )
{
    if ( !d->kdDiagram )
        return;
    d->showValues = showValues;
    
    KDChart::DataValueAttributes attributes = d->kdDiagram->dataValueAttributes( d->kdDataSetNumber );
    attributes.setVisible( showValues );
    d->kdDiagram->setDataValueAttributes( d->kdDataSetNumber, attributes );
    
    if ( d->attachedAxis )
        d->attachedAxis->update();
}

void DataSet::setShowLabels( bool showLabels )
{
    d->showLabels = showLabels;
}

QPen DataSet::pen() const
{
    return d->pen;
}

QBrush DataSet::brush() const
{
    return d->brush;
}

void DataSet::setPen( const QPen &pen )
{
    d->pen = pen;
    d->kdDiagram->setPen( d->kdDataSetNumber, pen );
    if ( d->attachedAxis )
        d->attachedAxis->update();
}

void DataSet::setBrush( const QBrush &brush )
{
    d->brush = brush;
    d->kdDiagram->setBrush( d->kdDataSetNumber, brush );
    if ( d->attachedAxis )
        d->attachedAxis->update();
}

QColor DataSet::color() const
{
    return d->brush.color();
}

void DataSet::setColor( const QColor &color )
{
    QBrush brush = d->brush;
    brush.setColor( color );
    setBrush( brush );
}

void DataSet::setShowMeanValue( bool show )
{
    d->showMeanValue = show;
}

void DataSet::setMeanValuePen( const QPen &pen )
{
    d->meanValuePen = pen;
}

void DataSet::setShowLowerErrorIndicator( bool show )
{
    d->showLowerErrorIndicator = show;
}

void DataSet::setShowUpperErrorIndicator( bool show )
{
    d->showUpperErrorIndicator = show;
}

void DataSet::setShowErrorIndicators( bool lower, bool upper )
{
    setShowLowerErrorIndicator( lower );
    setShowUpperErrorIndicator( upper );
}

void DataSet::setErrorIndicatorPen( const QPen &pen )
{
    d->errorIndicatorPen = pen;
}

void DataSet::setErrorCategory( ErrorCategory category )
{
    d->errorCategory = category;
}

void DataSet::setErrorPercentage( double percentage )
{
    d->errorPercentage = percentage;
}

void DataSet::setErrorMargin( double margin )
{
    d->errorMargin = margin;
}

void DataSet::setLowerErrorLimit( double limit )
{
    d->lowerErrorLimit = limit;
}

void DataSet::setUpperErrorLimit( double limit )
{
    d->upperErrorLimit = limit;
}

QVariant DataSet::xData( int index ) const
{
    if ( !d->xDataRegion.isValid() )
            return QVariant();
        
    QPoint dataPoint = d->xDataRegion.pointAtIndex( index );
    return d->model->data( d->model->index( dataPoint.y(), dataPoint.x() ) );
}

QVariant DataSet::yData( int index ) const
{
    if ( !d->yDataRegion.isValid() )
            return QVariant();
        
    QPoint dataPoint = d->yDataRegion.pointAtIndex( index );
    qDebug() << dataPoint;
    return d->model->data( d->model->index( dataPoint.y(), dataPoint.x() ) );
}

QVariant DataSet::customData( int index ) const
{
    if ( !d->customDataRegion.isValid() )
            return QVariant();
        
    QPoint dataPoint = d->customDataRegion.pointAtIndex( index );
    return d->model->data( d->model->index( dataPoint.y(), dataPoint.x() ) );
}

QVariant DataSet::categoryData( int index ) const
{
    if ( !d->categoryDataRegion.isValid() )
            return QVariant();
        
    QPoint dataPoint = d->categoryDataRegion.pointAtIndex( index );
    return d->model->data( d->model->index( dataPoint.y(), dataPoint.x() ) );
}

QVariant DataSet::labelData() const
{
    if ( !d->labelDataRegion.isValid() )
            return QVariant();
    
    QString label;
    
    int cellCount = d->labelDataRegion.cellCount();
    for ( int i = 0; i < cellCount; i++ )
    {
        QPoint dataPoint = d->labelDataRegion.pointAtIndex( i );
        label += d->model->data( d->model->index( dataPoint.y(), dataPoint.x() ) ).toString();
    }
    
    return QVariant( label );
}


CellRegion DataSet::xDataRegion() const
{
    return d->xDataRegion;
}

CellRegion DataSet::yDataRegion() const
{
    return d->yDataRegion;
}

CellRegion DataSet::customDataRegion() const
{
    return d->customDataRegion;
}

CellRegion DataSet::categoryDataRegion() const
{
    return d->categoryDataRegion;
}

CellRegion DataSet::labelDataRegion() const
{
    return d->labelDataRegion;
}

QString DataSet::xDataRegionString() const
{
    if ( !d->model->spreadSheetModel() )
        return QString();
    
    return d->model->spreadSheetModel()->regionToString( d->xDataRegion.rects() );
}

QString DataSet::yDataRegionString() const
{
    if ( !d->model->spreadSheetModel() )
        return QString();
    
    return d->model->spreadSheetModel()->regionToString( d->yDataRegion.rects() );
}

QString DataSet::customDataRegionString() const
{
    if ( !d->model->spreadSheetModel() )
        return QString();
    
    return d->model->spreadSheetModel()->regionToString( d->customDataRegion.rects() );
}

QString DataSet::categoryDataRegionString() const
{
    if ( !d->model->spreadSheetModel() )
        return QString();
    
    return d->model->spreadSheetModel()->regionToString( d->categoryDataRegion.rects() );
}

QString DataSet::labelDataRegionString() const
{
    if ( !d->model->spreadSheetModel() )
        return QString();
    
    return d->model->spreadSheetModel()->regionToString( d->labelDataRegion.rects() );
}


void DataSet::setXDataRegion( const CellRegion &region )
{
    d->xDataRegion = region;
    int newSize = 0;
    
    QRect boundingRect;
    foreach ( QRect rect, region.rects() )
    {
        newSize += qMax( rect.width(), rect.height() );
        boundingRect |= rect;
    }
    
    if ( boundingRect.height() > 1 )
        d->xDataDirection = Qt::Vertical;
    else
        d->xDataDirection = Qt::Horizontal;

    d->size = qMax( d->size, newSize );
}

void DataSet::setYDataRegion( const CellRegion &region )
{
    d->yDataRegion = region;
    d->yDataDirection = region.orientation();
    const int oldSize = d->size;
    d->size = region.cellCount();
    qDebug() << "cell count:" << region.cellCount();
    
    if ( !d->blockSignals )
    {
	    foreach( KDChartModel *model, d->kdChartModels )
	    {
	    	if ( oldSize != d->size )
	    		model->dataSetSizeChanged( this, d->size );
	        model->dataChanged( model->index( 0,           d->kdDataSetNumber ),
	                            model->index( d->size - 1, d->kdDataSetNumber ) );
	    }
    }
}

void DataSet::setCustomDataRegion( const CellRegion &region )
{
    d->customDataRegion = region;
    int newSize = 0;
    
    QRect boundingRect;
    foreach ( QRect rect, region.rects() )
    {
        newSize += qMax( rect.width(), rect.height() );
        boundingRect |= rect;
    }
    
    if ( boundingRect.height() > 1 )
        d->customDataDirection = Qt::Vertical;
    else
        d->customDataDirection = Qt::Horizontal;

    d->size = qMax( d->size, newSize );
}

void DataSet::setCategoryDataRegion( const CellRegion &region )
{
    d->categoryDataRegion = region;
    int newSize = 0;
    
    QRect boundingRect;
    foreach ( QRect rect, region.rects() )
    {
        newSize += qMax( rect.width(), rect.height() );
        boundingRect |= rect;
    }
    
    if ( boundingRect.height() > 1 )
        d->categoryDataDirection = Qt::Vertical;
    else
        d->categoryDataDirection = Qt::Horizontal;
    
    d->size = qMax( d->size, newSize );
}

void DataSet::setLabelDataRegion( const CellRegion &region )
{
    d->labelDataRegion = region;
    
    QRect boundingRect;
    foreach ( QRect rect, region.rects() )
        boundingRect |= rect;
    
    if ( boundingRect.height() > 1 )
        d->labelDataDirection = Qt::Vertical;
    else
        d->labelDataDirection = Qt::Horizontal;
}

void DataSet::setXDataRegionString( const QString &string )
{
    if ( !d->model->spreadSheetModel() )
        return;
    
    setXDataRegion( d->model->spreadSheetModel()->stringToRegion( string ) );
}

void DataSet::setYDataRegionString( const QString &string )
{
    if ( !d->model->spreadSheetModel() )
        return;
    
    setYDataRegion( d->model->spreadSheetModel()->stringToRegion( string ) );
}

void DataSet::setCustomDataRegionString( const QString &string )
{
    if ( !d->model->spreadSheetModel() )
        return;
    
    setCustomDataRegion( d->model->spreadSheetModel()->stringToRegion( string ) );
}

void DataSet::setCategoryDataRegionString( const QString &string )
{
    if ( !d->model->spreadSheetModel() )
        return;
    
    setCategoryDataRegion( d->model->spreadSheetModel()->stringToRegion( string ) );
}

void DataSet::setLabelDataRegionString( const QString &string )
{
    if ( !d->model->spreadSheetModel() )
        return;
    
    setLabelDataRegion( d->model->spreadSheetModel()->stringToRegion( string ) );
}


int DataSet::size() const
{
    qDebug() << "DataSet::size(): Returning" << d->size;
    return d->size;
}

void DataSet::setKdDiagram( KDChart::AbstractDiagram *diagram )
{
    d->kdDiagram = diagram;
}

KDChart::AbstractDiagram *DataSet::kdDiagram() const
{
    return d->kdDiagram;
}

void DataSet::setKdDataSetNumber( int number )
{
    d->kdDataSetNumber = number;
    if ( d->kdDiagram )
    {
        d->brush = d->kdDiagram->brush( number );
        d->pen = d->kdDiagram->pen( number );
    }
}

bool DataSet::registerKdChartModel( KDChartModel *model )
{
    if ( d->kdChartModels.contains( model ) )
        return false;
    d->kdChartModels.append( model );
    return true;
}

bool DataSet::deregisterKdChartModel( KDChartModel *model )
{
    if ( !d->kdChartModels.contains( model ) )
        return false;
    d->kdChartModels.removeAll( model );
    return true;
}

void DataSet::deregisterAllKdChartModels()
{
	d->kdChartModels.clear();
}

void DataSet::yDataChanged( const QRect &rect ) const
{    
    int start, end;
    
    QVector<QRect> yDataRegionRects = d->yDataRegion.rects();
    
    if ( d->yDataDirection == Qt::Horizontal )
    {
        QPoint topLeft = rect.topLeft();
        QPoint topRight = rect.topRight();
        
        int totalWidth = 0;
        int i;
        
        for ( i = 0; i < yDataRegionRects.size(); i++ )
        {
            if ( yDataRegionRects[i].contains( topLeft ) )
            {
                start = totalWidth + topLeft.x() - yDataRegionRects[i].topLeft().x();
                break;
            }
            totalWidth += yDataRegionRects[i].width();
        }
        
        for ( i; i < yDataRegionRects.size(); i++ )
        {
            if ( yDataRegionRects[i].contains( topRight ) )
            {
                end = totalWidth + topRight.x() - yDataRegionRects[i].topLeft().x();
                break;
            }
            totalWidth += yDataRegionRects[i].width();
        }
    }
    else
    {
    }
    
    qDebug() << "y data changed:" << start << end << "[this=" << this << "]";
    
    if ( !d->blockSignals )
    {
	    foreach( KDChartModel *model, d->kdChartModels )
	    {
	        model->dataChanged( model->index( start, d->kdDataSetNumber ),
	                            model->index( end,   d->kdDataSetNumber ) );
	    }
    }
}

void DataSet::blockSignals( bool block )
{
	d->blockSignals = block;
}
