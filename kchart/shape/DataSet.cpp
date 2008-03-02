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
#include "ProxyModel.h"
#include "Axis.h"
#include "PlotArea.h"

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
    
    PlotArea *plotArea;
    
    ProxyModel *model;
    KDChart::AbstractDiagram *kdDiagram;
    int kdDataSetNumber;
};

DataSet::Private::Private()
{
    chartType = LastChartType;
    kdDataSetNumber = 0;
}

DataSet::Private::~Private()
{
}


DataSet::DataSet( PlotArea *plotArea )
    : d( new Private )
{
    d->plotArea = plotArea;
    d->model = d->plotArea->proxyModel();
}

DataSet::~DataSet()
{
    if ( d->attachedAxis )
        d->attachedAxis->detachDataSet( this );
}


ChartType DataSet::chartType() const
{
    // Return the global chart type of the plot area
    // if no custom type was specified for this data set
    return ( d->chartType == LastChartType ) ? d->plotArea->chartType() : d->chartType;
}

Axis *DataSet::attachedAxis() const
{
    return d->attachedAxis;
}

QString DataSet::xValueCellRange() const
{
    return QString();
}

QString DataSet::yValueCellRange() const
{
    return QString();
}

QString DataSet::widthCellRange() const
{
    return QString();
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
    // TODO (Johannes): Set the data set's chart sub type
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
    d->showValues = showValues;
    
    KDChart::DataValueAttributes attributes = d->kdDiagram->dataValueAttributes( d->kdDataSetNumber );
    attributes.setVisible( showValues );
    d->kdDiagram->setDataValueAttributes( d->kdDataSetNumber, attributes );
}

void DataSet::setShowLabels( bool showLabels )
{
    d->showLabels = showLabels;
}

QPen DataSet::pen() const
{
    return d->pen;
}

void DataSet::setPen( const QPen &pen )
{
    d->kdDiagram->setPen( d->kdDataSetNumber, pen );
}

QColor DataSet::color() const
{
    return d->pen.color();
}

void DataSet::setColor( const QColor &color )
{
    QPen pen = d->kdDiagram->pen( d->kdDataSetNumber );
    pen.setColor( color );
    d->kdDiagram->setPen( pen );
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

QVariant DataSet::xData( int index )
{
    return d->model->xData( this, index );
}

QVariant DataSet::yData( int index )
{
    return d->model->yData( this, index );
}

QVariant DataSet::customData( int index )
{
    return d->model->customData( this, index );
}

QVariant DataSet::labelData()
{
    return d->model->labelData( this );
}

int DataSet::size() const
{
    return d->model->columnCount();
}

void DataSet::setKdDiagram( KDChart::AbstractDiagram *diagram )
{
    d->kdDiagram = diagram;
}

void DataSet::setKdDataSetNumber( int number )
{
    d->kdDataSetNumber = number;
}
