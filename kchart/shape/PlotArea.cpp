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
#include <KoTextShapeData.h>

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
#include <KDChartFrameAttributes>
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
    
    QList<KDChart::AbstractDiagram*> kdDiagrams;
    
    KDChart::Chart *kdChart;
    KDChart::AbstractCoordinatePlane *kdPlane;
    
    Surface *wall;
    Surface *floor;
    
    ChartType chartType;
    ChartSubtype chartSubtype;
    bool threeD;
    
    ThreeDScene *threeDScene;
    
    int gapBetweenBars;
    int gapBetweenSets;
};

PlotArea::Private::Private()
{
    kdChart = new KDChart::Chart();
    kdPlane = new KDChart::CartesianCoordinatePlane( kdChart );
}

PlotArea::Private::~Private()
{
}


PlotArea::PlotArea( ChartShape *parent )
    : d( new Private )
{
    d->shape = parent;
}

PlotArea::~PlotArea()
{
}


void PlotArea::init()
{
    d->kdChart->resize( size().toSize() );
    d->kdChart->replaceCoordinatePlane( d->kdPlane );
    KDChart::FrameAttributes attr = d->kdChart->frameAttributes();
    attr.setVisible( false );
    d->kdChart->setFrameAttributes( attr );
    
    // There need to be at least these two axes. Do not delete, but hide them instead.
    Axis *xAxis = new Axis( this );
    xAxis->setPosition( BottomAxisPosition );
    Axis *yAxis = new Axis( this );
    yAxis->setPosition( LeftAxisPosition );
    yAxis->setShowGrid( true );
    d->axes.append( xAxis );
    d->axes.append( yAxis );
}

void PlotArea::modelReset()
{
    foreach( DataSet *dataSet, proxyModel()->dataSets() )
    {
        yAxis()->attachDataSet( dataSet );
    }
}

ProxyModel *PlotArea::proxyModel() const
{
    return d->shape->proxyModel();
}

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
    return proxyModel()->dataSets();
}

int PlotArea::dataSetCount() const
{
    return proxyModel()->dataSets().size();
}

Surface *PlotArea::wall() const
{
    return d->wall;
}

Surface *PlotArea::floor() const
{
    return d->floor;
}

Axis *PlotArea::xAxis() const
{
    foreach( Axis *axis, d->axes )
    {
        if ( axis->orientation() == Qt::Horizontal )
            return axis;
    }
    return 0;
}

Axis *PlotArea::yAxis() const
{
    foreach( Axis *axis, d->axes )
    {
        if ( axis->orientation() == Qt::Vertical )
            return axis;
    }
    return 0;
}

Axis *PlotArea::secondaryXAxis() const
{
    bool firstXAxisFound = false;
    foreach( Axis *axis, d->axes )
    {
        if ( axis->orientation() == Qt::Horizontal ) {
            if ( firstXAxisFound )
                return axis;
            else
                firstXAxisFound = true;
        }
    }
    return 0;
}

Axis *PlotArea::secondaryYAxis() const
{
    bool firstYAxisFound = false;
    foreach( Axis *axis, d->axes )
    {
        if ( axis->orientation() == Qt::Vertical ) {
            if ( firstYAxisFound )
                return axis;
            else
                firstYAxisFound = true;
        }
    }
    return 0;
}

ChartType PlotArea::chartType() const
{
    return d->chartType;
}

ChartSubtype PlotArea::chartSubType() const
{
    return d->chartSubtype;
}

bool PlotArea::isThreeD() const
{
    return d->threeD;
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
    
    requestRepaint();

    return true;
}

bool PlotArea::removeAxis( Axis *axis )
{
    if ( !d->axes.contains( axis ) )
        return false;
    d->axes.removeAll( axis );
    
    requestRepaint();
    
    return true;
}



void PlotArea::setChartType( ChartType type )
{
    if ( d->chartType != ScatterChartType && type == ScatterChartType )
    {
        d->shape->proxyModel()->setDataDimensions( 2 );
    }
    else if ( d->chartType == ScatterChartType && type != ScatterChartType )
    {
        d->shape->proxyModel()->setDataDimensions( 1 );
    }
    
    d->chartType = type;
    
    foreach ( Axis *axis, d->axes )
    {
        axis->plotAreaChartTypeChanged( type );
    }
    
    requestRepaint();
}

void PlotArea::setChartSubType( ChartSubtype subType )
{
    d->chartSubtype = subType;
    
    foreach ( Axis *axis, d->axes )
    {
        axis->plotAreaChartSubTypeChanged( subType );
    }
    
    requestRepaint();
}

void PlotArea::setThreeD( bool threeD )
{
    d->threeD = threeD;
    
    foreach( Axis *axis, d->axes )
        axis->setThreeD( threeD );
    
    requestRepaint();
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
            proxyModel()->setFirstRowIsLabel( true );
            proxyModel()->setFirstColumnIsLabel( true );
        } else if ( dataSourceHasLabels == "row" ) {
            proxyModel()->setFirstRowIsLabel( true );
            proxyModel()->setFirstColumnIsLabel( false );
        } else if ( dataSourceHasLabels == "column" ) {
            proxyModel()->setFirstRowIsLabel( false );
            proxyModel()->setFirstColumnIsLabel( true );
        } else {
            // dataSourceHasLabels == "none" or wrong value
            proxyModel()->setFirstRowIsLabel( false );
            proxyModel()->setFirstColumnIsLabel( false );
        }
    }
    else {
        // No info about if first row / column contains labels.
        proxyModel()->setFirstRowIsLabel( false );
        proxyModel()->setFirstColumnIsLabel( false );
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
    
    requestRepaint();
    
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
    if ( proxyModel()->firstRowIsLabel() )
        if ( proxyModel()->firstColumnIsLabel() )
            dataSourceHasLabels = "both";
        else
            dataSourceHasLabels = "row";
    else
        if ( proxyModel()->firstColumnIsLabel() )
            dataSourceHasLabels = "column";
        else
            dataSourceHasLabels = "none";
    // Note: this is saved in the plotarea attributes and not the style.
    bodyWriter.addAttribute( "chart:data-source-has-labels",
                            dataSourceHasLabels );

    // Data direction
    {
        Qt::Orientation  direction = proxyModel()->dataDirection();
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
        switch( d->chartSubtype ) {
        case NoChartSubtype:
        case NormalChartSubtype:
            break;
        case StackedChartSubtype:
            plotAreaStyle.addProperty( "chart:stacked", "true" );
            break;
        case PercentChartSubtype:
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
        switch( d->chartSubtype ) {
        case NoChartSubtype:
        case NormalChartSubtype:
            break;
        case StackedChartSubtype:
            plotAreaStyle.addProperty( "chart:stacked", "true" );
            break;
        case PercentChartSubtype:
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
        switch( d->chartSubtype ) {
        case NoChartSubtype:
        case NormalChartSubtype:
        break;
    case StackedChartSubtype:
            plotAreaStyle.addProperty( "chart:stacked", "true" );
            break;
        case PercentChartSubtype:
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

void PlotArea::setGapBetweenBars( int percent )
{
    if ( d->chartType != BarChartType )
        return;
    
    //KDChart::BarAttributes attributes = ((KDChart::BarDiagram*) d->kdDiagram)->barAttributes();
    //attributes.setBarGapFactor( (float)percent / 100.0 );
    //((KDChart::BarDiagram*) d->kdDiagram)->setBarAttributes( attributes );
    
    requestRepaint();
}

void PlotArea::setGapBetweenSets( int percent )
{
    if ( d->chartType != BarChartType )
        return;
    
    //KDChart::BarAttributes attributes = ((KDChart::BarDiagram*) d->kdDiagram)->barAttributes();
    //attributes.setGroupGapFactor( (float)percent / 100.0 );
    //((KDChart::BarDiagram*) d->kdDiagram)->setBarAttributes( attributes );
    
    requestRepaint();
}

ChartShape *PlotArea::parent() const
{
    return d->shape;
}

KDChart::AbstractCoordinatePlane *PlotArea::kdPlane() const
{
    return d->kdPlane;
}

KDChart::Chart *PlotArea::kdChart() const
{
    return d->kdChart;
}

bool PlotArea::registerKdDiagram( KDChart::AbstractDiagram *diagram )
{
    if ( d->kdDiagrams.contains( diagram ) )
        return false;
    d->kdDiagrams.append( diagram );
    return true;
}

bool PlotArea::deregisterKdDiagram( KDChart::AbstractDiagram *diagram )
{
    if ( !d->kdDiagrams.contains( diagram ) )
        return false;
    d->kdDiagrams.removeAll( diagram );
    return true;
}

void PlotArea::update() const
{
    foreach( Axis* axis, d->axes )
        axis->update();
    d->shape->relayout();
}

void PlotArea::requestRepaint() const
{
    if ( !d->shape )
        return;
    
    d->shape->requestRepaint();
}

void PlotArea::paint( QPainter &painter )
{
    d->kdChart->paint( &painter, QRect( QPoint( 0, 0 ), d->shape->size().toSize() ) );
}

#include "PlotArea.moc"

