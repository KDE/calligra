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
#include "KDChartModel.h"

// Qt
#include <QAbstractItemModel>
#include <QString>
#include <QPen>
#include <QColor>

// KDChart
#include <KDChartDataValueAttributes>
#include <KDChartAbstractDiagram>

using namespace KChart;

DataSet::DataSet( ProxyModel *proxyModel )
{
    m_model = proxyModel;
    
    m_chartType = LastChartType;
    m_chartSubType = NoChartSubtype;
    m_kdDataSetNumber = 0;
    m_showMeanValue = false;
    m_showValues = false;
    m_showLabels = false;
    m_showLowerErrorIndicator = false;
    m_showUpperErrorIndicator = false;
    m_errorPercentage = 0.0;
    m_errorMargin = 0.0;
    m_lowerErrorLimit = 0.0;
    m_upperErrorLimit = 0.0;
    m_pen = QPen( Qt::black );
    m_kdDiagram = 0;
    m_attachedAxis = 0;
    m_sourceIsSpreadSheet = false;
    m_spreadSheetModel = 0;
    m_size = 0;
    m_blockSignals = false;
}

DataSet::~DataSet()
{
    if ( m_attachedAxis )
        m_attachedAxis->detachDataSet( this, true );
}


ChartType DataSet::chartType() const
{
    return m_chartType;
}

ChartSubtype DataSet::chartSubType() const
{
    return m_chartSubType;
}

ChartType DataSet::globalChartType() const
{
    return m_globalChartType;
}

ChartSubtype DataSet::globalChartSubType() const
{
    return m_globalChartSubType;
}

Axis *DataSet::attachedAxis() const
{
    return m_attachedAxis;
}

ProxyModel *DataSet::model() const
{
    return m_model;
}

bool DataSet::showMeanValue() const
{
    return m_showMeanValue;
}

QPen DataSet::meanValuePen() const
{
    return m_meanValuePen;   
}

bool DataSet::showLowerErrorIndicator() const
{
    return m_showLowerErrorIndicator;
}

bool DataSet::showUpperErrorIndicator() const
{
    return m_showUpperErrorIndicator;
}

QPen DataSet::errorIndicatorPen() const
{
    return m_errorIndicatorPen;
}

ErrorCategory DataSet::errorCategory() const
{
    return m_errorCategory;
}

double DataSet::errorPercentage() const
{
    return m_errorPercentage;
}

double DataSet::errorMargin() const
{
    return m_errorMargin;   
}

double DataSet::lowerErrorLimit() const
{
    return m_lowerErrorLimit;
}

double DataSet::upperErrorLimit() const
{
    return m_upperErrorLimit;
}


void DataSet::setChartType( ChartType type )
{
    if ( type == m_chartType )
        return;
    
    Axis *axis = m_attachedAxis;
    axis->detachDataSet( this );
    
    m_chartType = type;
    
    axis->attachDataSet( this );
}

void DataSet::setChartSubType( ChartSubtype subType )
{
    if ( subType == m_chartSubType )
        return;
    
    Axis *axis = m_attachedAxis;
    axis->detachDataSet( this );
    
    m_chartSubType = subType;
    
    axis->attachDataSet( this );
}

void DataSet::setGlobalChartType( ChartType type )
{
	m_globalChartType = type;
}

void DataSet::setGlobalChartSubType( ChartSubtype type )
{
	m_globalChartSubType = type;
}


void DataSet::setAttachedAxis( Axis *axis )
{
    m_attachedAxis = axis;
}

bool DataSet::showValues() const
{
    return m_showValues;
}

bool DataSet::showLabels() const
{
    return m_showLabels;
}

void DataSet::setShowValues( bool showValues )
{
    if ( !m_kdDiagram )
        return;
    m_showValues = showValues;
    
    KDChart::DataValueAttributes attributes = m_kdDiagram->dataValueAttributes( m_kdDataSetNumber );
    attributes.setVisible( showValues );
    m_kdDiagram->setDataValueAttributes( m_kdDataSetNumber, attributes );
    
    if ( m_attachedAxis )
        m_attachedAxis->update();
}

void DataSet::setShowLabels( bool showLabels )
{
    m_showLabels = showLabels;
}

QPen DataSet::pen() const
{
    return m_pen;
}

QBrush DataSet::brush() const
{
    return m_brush;
}

void DataSet::setPen( const QPen &pen )
{
    m_pen = pen;
    m_kdDiagram->setPen( m_kdDataSetNumber, pen );
    if ( m_attachedAxis )
        m_attachedAxis->update();
}

void DataSet::setBrush( const QBrush &brush )
{
    m_brush = brush;
    m_kdDiagram->setBrush( m_kdDataSetNumber, brush );
    if ( m_attachedAxis )
        m_attachedAxis->update();
}

QColor DataSet::color() const
{
    return m_brush.color();
}

void DataSet::setColor( const QColor &color )
{
    QBrush brush = m_brush;
    brush.setColor( color );
    setBrush( brush );
}

void DataSet::setShowMeanValue( bool show )
{
    m_showMeanValue = show;
}

void DataSet::setMeanValuePen( const QPen &pen )
{
    m_meanValuePen = pen;
}

void DataSet::setShowLowerErrorIndicator( bool show )
{
    m_showLowerErrorIndicator = show;
}

void DataSet::setShowUpperErrorIndicator( bool show )
{
    m_showUpperErrorIndicator = show;
}

void DataSet::setShowErrorIndicators( bool lower, bool upper )
{
    setShowLowerErrorIndicator( lower );
    setShowUpperErrorIndicator( upper );
}

void DataSet::setErrorIndicatorPen( const QPen &pen )
{
    m_errorIndicatorPen = pen;
}

void DataSet::setErrorCategory( ErrorCategory category )
{
    m_errorCategory = category;
}

void DataSet::setErrorPercentage( double percentage )
{
    m_errorPercentage = percentage;
}

void DataSet::setErrorMargin( double margin )
{
    m_errorMargin = margin;
}

void DataSet::setLowerErrorLimit( double limit )
{
    m_lowerErrorLimit = limit;
}

void DataSet::setUpperErrorLimit( double limit )
{
    m_upperErrorLimit = limit;
}

QVariant DataSet::xData( int index ) const
{
    return m_model->xData( this, index );
}

QVariant DataSet::yData( int index ) const
{
    return m_model->yData( this, index );
}

QVariant DataSet::customData( int index ) const
{
    return m_model->customData( this, index );
}

QVariant DataSet::categoryData( int index ) const
{
    return m_model->categoryData( this, index );
}

QVariant DataSet::labelData() const
{
    return m_model->labelData( this );
}

int DataSet::size() const
{
    return m_model->columnCount();
}

int DataSet::dimension() const
{
	// Return "1" by default
	const ChartType chartType = m_chartType != LastChartType ? m_chartType : m_globalChartType;
	switch ( chartType )
	{
	case BarChartType:
	case AreaChartType:
	case LineChartType:
	case CircleChartType:
	case RadarChartType:
	case SurfaceChartType:
		return 1;
	case ScatterChartType:
	case GanttChartType:
	case RingChartType:
		return 2;
	case BubbleChartType:
		return 3;
	}
	
	return 0;
}

void DataSet::setKdDiagram( KDChart::AbstractDiagram *diagram )
{
    m_kdDiagram = diagram;
}

KDChart::AbstractDiagram *DataSet::kdDiagram() const
{
    return m_kdDiagram;
}

int DataSet::kdDataSetNumber() const
{
    return m_kdDataSetNumber;
}

void DataSet::setKdDataSetNumber( int number )
{
    m_kdDataSetNumber = number;
    if ( m_kdDiagram )
    {
        m_brush = m_kdDiagram->brush( number );
        m_pen = m_kdDiagram->pen( number );
    }
}

void DataSet::setKdChartModel( KDChartModel *model )
{
    m_kdChartModel = model;
}

KDChartModel *DataSet::kdChartModel() const
{
	return m_kdChartModel;
}

void DataSet::xDataChanged( int start, int end ) const
{
    if ( m_kdChartModel )
    {
        const int column = m_kdDataSetNumber * 2;
    	m_kdChartModel->dataChanged( m_kdChartModel->index( start, column ),
    		                         m_kdChartModel->index( end,   column ) );
    }
}

void DataSet::yDataChanged( int start, int end ) const
{
    if ( m_kdChartModel )
    {
        const int column = dimension() == 2 ? m_kdDataSetNumber * 2 + 1 : m_kdDataSetNumber;
    	m_kdChartModel->dataChanged( m_kdChartModel->index( start, column ),
    			                     m_kdChartModel->index( end,   column ) );
    }
}

void DataSet::customDataChanged( int start, int end ) const
{
}

void DataSet::labelDataChanged() const
{
}

void DataSet::categoryDataChanged( int start, int end ) const
{
}

void DataSet::blockSignals( bool block )
{
	m_blockSignals = block;
}
