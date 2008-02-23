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
   Boston, MA 02110-1301, USA.
*/

// Local
#include "PlotArea.h"
#include "Axis.h"
#include "DataSet.h"
#include "Surface.h"
#include "ThreeDScene.h"
#include "ProxyModel.h"

// KOffice
#include <KoXmlReader.h>
#include <KoXmlWriter.h>
#include <KoShapeLoadingContext.h>
#include <KoGenStyles.h>
#include <KoXmlNS.h>

// Qt
#include <QPointF>
#include <QSizeF>
#include <QList>

// KDChart
#include <KDChartChart>
#include <KDChartCartesianAxis>
#include <KDChartAbstractDiagram>
#include <KDChartAbstractCartesianDiagram>
#include <KDChartAbstractCoordinatePlane>
#include <KDChartBarAttributes>
#include <KDChartPolarCoordinatePlane>
// Attribute Classes
#include <KDChartDataValueAttributes>
#include <KDChartGridAttributes>
#include <KDChartTextAttributes>
#include <KDChartMarkerAttributes>
// Diagram Classes
#include <KDChartBarDiagram>
#include <KDChartPieDiagram>
#include <KDChartLineDiagram>
#include <KDChartRingDiagram>
#include <KDChartPolarDiagram>

using namespace KChart;

class PlotArea::Private
{
public:
    Private();
    ~Private();

    // The parent chart shape
    ChartShape *shape;

    // The position of the plot area in coordinates
    // relative to parent shape
    QPointF position;

    // The size of the plot area
    QSizeF size;

    // The list of axes
    QList<Axis*> axes;

    // The list of data sets in the diagram
    QList<DataSet*> dataSets;
    
    Surface *wall;
    Surface *floor;
    
    ChartType chartType;
    ChartSubType chartSubType;
    bool threeD;
    
    ThreeDScene *threeDScene;
    
    int gapBetweenBars;
    int gapBetweenSets;
    
    ProxyModel *model;
    
    KDChart::AbstractDiagram *kdDiagram;
    KDChart::AbstractCoordinatePlane *kdCoordinatePlane;
};

PlotArea::Private::Private()
{
}

PlotArea::Private::~Private()
{
}


PlotArea::PlotArea( ChartShape *parent )
    : d( new Private )
{
    d->shape = parent;
    d->model = parent->model();
    
    d->kdCoordinatePlane = new KDChart::CartesianCoordinatePlane( d->shape->kdChart() );
    d->kdDiagram = new KDChart::BarDiagram( 0, static_cast<KDChart::CartesianCoordinatePlane*>(d->kdCoordinatePlane) );
    d->kdDiagram->setModel( d->model );
    
    Axis *xAxis = new Axis( this );
    xAxis->setPosition( BottomAxisPosition );
    Axis *yAxis = new Axis( this );
    yAxis->setPosition( LeftAxisPosition );
    d->axes.append( xAxis );
    d->axes.append( yAxis );
    static_cast<KDChart::AbstractCartesianDiagram*>(d->kdDiagram)->addAxis( xAxis->kdAxis() );
    static_cast<KDChart::AbstractCartesianDiagram*>(d->kdDiagram)->addAxis( yAxis->kdAxis() );
    // TODO: Connect signals from the chart's proxy model
    // here to update the data set count
};


QPointF PlotArea::position() const
{
    return d->position;
}

void PlotArea::setPosition( const QPointF &position )
{
    d->position = position;
}

QSizeF PlotArea::size() const
{
    return d->size;
}

void PlotArea::setSize( const QSizeF &size )
{
    d->size = size;
}

QList<Axis*> PlotArea::axes() const
{
    return d->axes;
}

QList<DataSet*> PlotArea::dataSets() const
{
    return d->dataSets;
}

int PlotArea::dataSetCount() const
{
    return d->dataSets.size();
}

Surface *PlotArea::wall() const
{
    return d->wall;
}

Surface *PlotArea::floor() const
{
    return d->floor;
}

ThreeDScene *PlotArea::threeDScene() const
{
    return d->threeDScene;
}

int PlotArea::gapBetweenBars() const
{
    return d->gapBetweenBars;
}

int PlotArea::gapBetweenSets() const
{
    return d->gapBetweenSets;
}

bool PlotArea::addAxis( Axis *axis )
{
    if ( d->axes.contains( axis ) )
        return false;
    d->axes.append( axis );
    
    if ( isCartesian( d->chartType ) )
        ((KDChart::AbstractCartesianDiagram*)d->kdDiagram)->addAxis( axis->kdAxis() );
    return true;
}

bool PlotArea::removeAxis( Axis *axis )
{
    if ( !d->axes.contains( axis ) )
        return false;
    d->axes.removeAll( axis );
    
    if ( isCartesian( d->chartType ) )
            ((KDChart::AbstractCartesianDiagram*)d->kdDiagram)->takeAxis( axis->kdAxis() );
    return true;
}

bool PlotArea::loadOdf( const KoXmlElement &plotAreaElement, KoShapeLoadingContext &context )
{
    KoXmlElement dataHasLabelsElem = KoXml::namedItemNS( plotAreaElement, 
                                                         KoXmlNS::chart, "data-source-has-labels" );
    if ( plotAreaElement.hasAttributeNS( KoXmlNS::chart,
                                         "data-source-has-labels" ) ) {

        const QString  dataSourceHasLabels
            = plotAreaElement.attributeNS( KoXmlNS::chart,
                                           "data-source-has-labels" );
        if ( dataSourceHasLabels == "both" ) {
            d->model->setFirstRowIsLabel( true );
            d->model->setFirstColumnIsLabel( true );
        } else if ( dataSourceHasLabels == "row" ) {
            d->model->setFirstRowIsLabel( true );
            d->model->setFirstColumnIsLabel( false );
        } else if ( dataSourceHasLabels == "column" ) {
            d->model->setFirstRowIsLabel( false );
            d->model->setFirstColumnIsLabel( true );
        } else {
            // dataSourceHasLabels == "none" or wrong value
            d->model->setFirstRowIsLabel( false );
            d->model->setFirstColumnIsLabel( false );
        }
    }
    else {
        // No info about if first row / column contains labels.
        d->model->setFirstRowIsLabel( false );
        d->model->setFirstColumnIsLabel( false );
    }

    // 1. Load Axes
    if ( isCartesian( d->chartType ) ) {
        // TODO: Remove all axes before loading new ones
        KoXmlElement e;
        forEachElement( e, plotAreaElement ) {
            const KoXmlElement axisElement = e;
            Axis *axis = new Axis( this );
            if ( e.localName() == "axis" && e.namespaceURI() == KoXmlNS::chart ) {
                axis->loadOdf( axisElement, context );
            }
            addAxis( axis );
        }
    }
    return true;
}

void PlotArea::saveOdf( KoXmlWriter &bodyWriter, KoGenStyles &mainStyles ) const
{
    // Prepare the style for the plot area
    KoGenStyle plotAreaStyle( KoGenStyle::StyleAuto, "chart" );

    // Save chart subtype
    saveOdfSubType( bodyWriter, plotAreaStyle );

    // About the data:
    //   Save if the first row / column contain headers.
    QString  dataSourceHasLabels;
    if ( d->model->firstRowIsLabel() )
        if ( d->model->firstColumnIsLabel() )
            dataSourceHasLabels = "both";
        else
            dataSourceHasLabels = "row";
    else
        if ( d->model->firstColumnIsLabel() )
            dataSourceHasLabels = "column";
        else
            dataSourceHasLabels = "none";
    // Note: this is saved in the plotarea attributes and not the style.
    bodyWriter.addAttribute( "chart:data-source-has-labels",
                            dataSourceHasLabels );

    // Data direction
    {
        Qt::Orientation  direction = d->model->dataDirection();
        plotAreaStyle.addProperty( "chart:series-source",  
                                   ( direction == Qt::Horizontal )
                                   ? "rows" : "columns" );
    }

    // Register the style, and get back its auto-generated name
    const QString  styleName = mainStyles.lookup( plotAreaStyle, "ch" );
    bodyWriter.addAttribute( "chart:style-name", styleName );

    if ( isCartesian( d->chartType ) ) {
        foreach( Axis *axis, d->axes ) {
            axis->saveOdf( bodyWriter, mainStyles );
        }
    }

    // TODO chart:series
    // TODO chart:wall
    // TODO chart:grid
    // TODO chart:floor
}

void PlotArea::saveOdfSubType( KoXmlWriter& xmlWriter, KoGenStyle& plotAreaStyle ) const
{
    switch ( d->chartType ) {
    case BarChartType:
        switch( d->chartSubType ) {
        case NoChartSubType:
        case NormalChartSubType:
            break;
        case StackedChartSubType:
            plotAreaStyle.addProperty( "chart:stacked", "true" );
            break;
        case PercentChartSubType:
            plotAreaStyle.addProperty( "chart:percentage", "true" );
            break;
        }
    if ( d->threeD ) {
        plotAreaStyle.addProperty( "chart:three-dimensional", "true" );
        // FIXME: Save all 3D attributes too.
    }
        plotAreaStyle.addProperty( "chart:vertical", "false" ); // FIXME
        plotAreaStyle.addProperty( "chart:lines-used", 0 ); // FIXME: for now
        break;

    case LineChartType:
        switch( d->chartSubType ) {
        case NoChartSubType:
        case NormalChartSubType:
            break;
        case StackedChartSubType:
            plotAreaStyle.addProperty( "chart:stacked", "true" );
            break;
        case PercentChartSubType:
            plotAreaStyle.addProperty( "chart:percentage", "true" );
            break;
        }
    if ( d->threeD ) {
        plotAreaStyle.addProperty( "chart:three-dimensional", "true" );
        // FIXME: Save all 3D attributes too.
    }
        // FIXME: What does this mean?
        plotAreaStyle.addProperty( "chart:symbol-type", "automatic" );
        break;

    case AreaChartType:
        switch( d->chartSubType ) {
        case NoChartSubType:
        case NormalChartSubType:
        break;
    case StackedChartSubType:
            plotAreaStyle.addProperty( "chart:stacked", "true" );
            break;
        case PercentChartSubType:
            plotAreaStyle.addProperty( "chart:percentage", "true" );
            break;
        }
        //plotAreaStyle.addProperty( "chart:lines-used", 0 ); // #### for now
    if ( d->threeD ) {
        plotAreaStyle.addProperty( "chart:three-dimensional", "true" );
        // FIXME: Save all 3D attributes too.
    }

    case CircleChartType:
        // FIXME
        break;
    break;

    case RingChartType:
    case ScatterChartType:
    case RadarChartType:
    case StockChartType:
    case BubbleChartType:
    case SurfaceChartType:
    case GanttChartType:
        // FIXME
        break;

        // This is not a valid type, but needs to be handled to avoid
        // a warning from gcc.
    case LastChartType:
    default:
        // FIXME
        break;
    }
}

KDChart::AbstractDiagram *PlotArea::kdDiagram() const
{
    return d->kdDiagram;
}

void PlotArea::setGapBetweenBars( int percent )
{
    if ( d->chartType != BarChartType )
        return;
    
    KDChart::BarAttributes attributes = ((KDChart::BarDiagram*) d->kdDiagram)->barAttributes();
    attributes.setBarGapFactor( (float)percent / 100.0 );
    ((KDChart::BarDiagram*) d->kdDiagram)->setBarAttributes( attributes );
        
    d->shape->update();
}

void PlotArea::setGapBetweenSets( int percent )
{
    if ( d->chartType != BarChartType )
        return;
    
    KDChart::BarAttributes attributes = ((KDChart::BarDiagram*) d->kdDiagram)->barAttributes();
    attributes.setGroupGapFactor( (float)percent / 100.0 );
    ((KDChart::BarDiagram*) d->kdDiagram)->setBarAttributes( attributes );
        
    d->shape->update();
}
