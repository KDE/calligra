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
#include "CellDataSet.h"
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
#include <KoOdfLoadingContext.h>
#include <KoOdfStylesReader.h>
#include <KoViewConverter.h>

// Qt
#include <QPointF>
#include <QSizeF>
#include <QList>
#include <QImage>
#include <QPainter>

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

const int MAX_PIXMAP_SIZE = 1000;

class PlotArea::Private
{
public:
    Private();
    ~Private();

    // The parent chart shape
    ChartShape *shape;

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
    
    QList<KoShape*> automaticallyHiddenAxisTitles;

    // We can rerender faster if we cache KDChart's output
    QImage   image;
    bool     paintPixmap;
    QPointF  lastZoomLevel;
    QSizeF   lastSize;
    mutable bool pixmapRepaintRequested;
};

PlotArea::Private::Private()
{
    shape = 0;
    kdChart = new KDChart::Chart();
    kdPlane = new KDChart::CartesianCoordinatePlane( kdChart );
    wall = 0;
    floor = 0;
    chartType = BarChartType;
    chartSubtype = NormalChartSubtype;
    threeD = false;
    threeDScene = 0;
    gapBetweenBars = 0;
    gapBetweenSets = 100;
    pixmapRepaintRequested = true;
    paintPixmap = true;
}

PlotArea::Private::~Private()
{
}


PlotArea::PlotArea( ChartShape *parent )
    : d( new Private )
    , KoShape()
{
    d->shape = parent;
    
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

PlotArea::~PlotArea()
{
}


void PlotArea::dataSetCountChanged()
{
    foreach( DataSet *dataSet, proxyModel()->dataSets() )
    {
        if ( !dataSet->attachedAxis() )
        {
            yAxis()->attachDataSet( dataSet );
        }
    }
}

ProxyModel *PlotArea::proxyModel() const
{
    return d->shape->proxyModel();
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
    {
    	qWarning() << "PlotArea::addAxis(): This axis has already been added.";
    	return false;
    }
    if ( !axis )
    {
    	qWarning() << "PlotArea::addAxis(): Pointer to axis is NULL!";
    	return false;
    }
    d->axes.append( axis );
    
    foreach ( Axis *axis, d->axes )
    {
        if ( axis->dimension() == XAxisDimension )
            axis->registerKdXAxis( axis->kdAxis() );
    }
    
    requestRepaint();

    return true;
}

bool PlotArea::removeAxis( Axis *axis )
{
    if ( !d->axes.contains( axis ) )
    {
    	qWarning() << "PlotArea::removeAxis(): This axis has not been added previously.";
    	return false;
    }
    if ( !axis )
    {
    	qWarning() << "PlotArea::removeAxis(): Pointer to axis is NULL!";
    	return false;
    }
    d->axes.removeAll( axis );
    
    foreach ( Axis *axis, d->axes )
    {
        axis->deregisterKdXAxis( axis->kdAxis() );
    }
    
    requestRepaint();
    
    return true;
}



void PlotArea::setChartType( ChartType type )
{
    if ( !isPolar( d->chartType ) && isPolar( type ) )
    {
        foreach ( Axis *axis, d->axes )
        {
            if ( !axis->title()->isVisible() )
                continue;
            axis->title()->setVisible( false );
            d->automaticallyHiddenAxisTitles.append( axis->title() );
        }
    }
    else if ( isPolar( d->chartType ) && !isPolar( type ) )
    {
        foreach ( KoShape *title, d->automaticallyHiddenAxisTitles )
        {
            title->setVisible( true );
        }
        d->automaticallyHiddenAxisTitles.clear();
    }
    
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
    qWarning() << "Error: The plot area cannot be loaded from ODF directly, but only as a child of the chart shape.";
}

bool PlotArea::loadOdf( const KoXmlElement &plotAreaElement, const KoOdfStylesReader &stylesReader )
{
    if ( plotAreaElement.hasAttributeNS( KoXmlNS::svg, "x" ) && plotAreaElement.hasAttributeNS( KoXmlNS::svg, "y" ) )
    {
        const qreal x = KoUnit::parseValue( plotAreaElement.attributeNS( KoXmlNS::svg, "x" ) );
        const qreal y = KoUnit::parseValue( plotAreaElement.attributeNS( KoXmlNS::svg, "y" ) );
        setPosition( QPointF( x, y ) );
    }
    
    if ( plotAreaElement.hasAttributeNS( KoXmlNS::svg, "width" ) && plotAreaElement.hasAttributeNS( KoXmlNS::svg, "height" ) )
    {
        const qreal width = KoUnit::parseValue( plotAreaElement.attributeNS( KoXmlNS::svg, "width" ) );
        const qreal height = KoUnit::parseValue( plotAreaElement.attributeNS( KoXmlNS::svg, "height" ) );
        setSize( QSizeF( width, height ) );
    }
    
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

    // Remove all axes before loading new ones
    KoXmlElement e;
    while( !d->axes.isEmpty() )
    {
        Axis *axis = d->axes.takeLast();
        Q_ASSERT( axis );
        delete axis;
    }
    
    qDebug() << "AXIS COUNT1=" << d->axes.size();
    
    // Things to load first
    KoXmlElement n;
    for ( n = plotAreaElement.firstChild().toElement(); !n.isNull(); n = n.nextSibling().toElement() )
    {
        if ( n.namespaceURI() != KoXmlNS::chart )
            continue;
        if ( n.localName() == "axis" ) {
            Axis *axis = new Axis( this );
            axis->loadOdf( n, stylesReader );
            addAxis( axis );
        }
    }

    qDebug() << "AXIS COUNT2=" << d->axes.size();
    // Things to load second
    for ( n = plotAreaElement.firstChild().toElement(); !n.isNull(); n = n.nextSibling().toElement() )
    {
        if ( n.namespaceURI() != KoXmlNS::chart )
            continue;
        if ( n.localName() == "series" )
            loadOdfSeries( n, stylesReader );
        else if ( n.localName() == "wall" )
            d->wall->loadOdf( n, stylesReader );
        else if ( n.localName() == "floor" )
            d->floor->loadOdf( n, stylesReader );
        else if ( n.localName() != "axis" )
        {
            qWarning() << "PlotArea::loadOdf(): Unknown tag name \"" << n.localName() << "\"";
        }
    }
    
    if ( plotAreaElement.hasAttributeNS( KoXmlNS::chart, "style-name" ) ) {
        QString styleName = plotAreaElement.attributeNS( KoXmlNS::chart, "style-name", QString() );
        const KoXmlElement *styleElement = stylesReader.findStyle( styleName, "chart" );
        if ( styleElement && !styleElement->isNull() ) {
            KoXmlElement chartPropertiesElement = styleElement->namedItemNS( KoXmlNS::style, "chart-properties" ).toElement();
            if ( !chartPropertiesElement.isNull() ) {
                if ( chartPropertiesElement.hasAttributeNS( KoXmlNS::chart, "percentage" ) ) {
                    QString percentage = chartPropertiesElement.attributeNS( KoXmlNS::chart, "percentage", QString() );
                    if ( percentage == "true" )
                    {
                        setChartSubType( PercentChartSubtype );
                    }
                }
                else if ( chartPropertiesElement.hasAttributeNS( KoXmlNS::chart, "stacked" ) ) {
                    QString stacked = chartPropertiesElement.attributeNS( KoXmlNS::chart, "stacked", QString() );
                    if ( stacked == "true" )
                    {
                        setChartSubType( StackedChartSubtype );
                    }
                }
            }
        }
    }
    
    requestRepaint();
    
    return true;
}

bool PlotArea::loadOdfSeries( const KoXmlElement &seriesElement, const KoOdfStylesReader &stylesReader )
{
    DataSet *dataSet = d->shape->proxyModel()->createDataSet();
    Q_ASSERT( dataSet );
    if ( !dataSet )
        return true;

    if ( seriesElement.hasAttributeNS( KoXmlNS::chart, "values-cell-range-address" ) )
    {
        const QString region = seriesElement.attributeNS( KoXmlNS::chart, "values-cell-range-address", QString() );
        dataSet->setYDataRegionString( region );
    }
    if ( seriesElement.hasAttributeNS( KoXmlNS::chart, "label-cell-address" ) )
    {
        const QString region = seriesElement.attributeNS( KoXmlNS::chart, "label-cell-address", QString() );
        dataSet->setLabelDataRegionString( region );
    }
    if ( seriesElement.hasAttributeNS( KoXmlNS::chart, "style-name" ) )
    {
        QString styleName = seriesElement.attributeNS( KoXmlNS::chart, "style-name", QString() );
        const KoXmlElement *styleElement = stylesReader.findStyle( styleName, "chart" );
        if ( styleElement && !styleElement->isNull() )
        {
            KoXmlElement n = styleElement->firstChild().toElement();
            for ( ; !n.isNull(); n = n.nextSibling().toElement() )
            {
                
                if ( n.namespaceURI() != KoXmlNS::style )
                    continue;
                if ( n.localName() == "chart-properties" )
                {
                    
                    if ( n.hasAttributeNS( KoXmlNS::chart, "pie-offset" ) )
                    {
                        int pieOffset = n.attributeNS( KoXmlNS::chart, "pie-offset", QString() ).toInt();
                        
                        setPieExplodeFactor( dataSet, pieOffset );
                    }
                }
                else if ( n.localName() == "graphic-properties" )
                {
                    if ( n.hasAttributeNS( KoXmlNS::draw, "fill-color" ) )
                    {
                        QColor fillColor( n.attributeNS( KoXmlNS::draw, "fill-color", QString() ) );

                        dataSet->setColor( fillColor );
                    }
                    if ( n.hasAttributeNS( KoXmlNS::svg, "stroke-color" ) )
                    {
                        QColor strokeColor( n.attributeNS( KoXmlNS::svg, "stroke-color", QString() ) );
                        
                        dataSet->setPen( strokeColor );
                    }
                }
                else
                {
                    qWarning() << "PlotArea::loadOdfSeries(): Unknown tag name \"" << n.localName() << "\"";
                }
            }
        }
    }
        
    
    KoXmlElement n = seriesElement.firstChild().toElement();
    for ( ; !n.isNull(); n = n.nextSibling().toElement() )
    {
        if ( n.namespaceURI() != KoXmlNS::chart )
            continue;
        // FIXME: Load data points
    }

    return true;
}

void PlotArea::saveOdf( KoShapeSavingContext &context ) const
{
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
    emit gapBetweenBarsChanged( percent );
}

void PlotArea::setGapBetweenSets( int percent )
{
    emit gapBetweenSetsChanged( percent );
}

void PlotArea::setPieExplodeFactor( DataSet *dataSet, int percent )
{
    emit pieExplodeFactorChanged( dataSet, percent );
}

ChartShape *PlotArea::parent() const
{
    // There has to be a valid parent
    Q_ASSERT( d->shape );
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
    relayout();
}

void PlotArea::requestRepaint() const
{
    d->pixmapRepaintRequested = true;
}

void PlotArea::paintPixmap( QPainter &painter, const KoViewConverter &converter )
{
    // Adjust the size of the painting area to the current zoom level
    const QSize paintRectSize = converter.documentToView( size() ).toSize();
    const QRect paintRect = QRect( QPoint( 0, 0 ), paintRectSize );
    const QSize plotAreaSize = size().toSize();
    const int borderX = 4;
    const int borderY = 4;

    // Only use a pixmap with sane sizes
    d->paintPixmap = paintRectSize.width() < MAX_PIXMAP_SIZE || paintRectSize.height() < MAX_PIXMAP_SIZE;
    
    if ( d->paintPixmap ) {
        d->image = QImage( paintRectSize, QImage::Format_ARGB32 );
    
        // Copy the painter's render hints, such as antialiasing
        QPainter pixmapPainter( &d->image );
        pixmapPainter.setRenderHints( painter.renderHints() );
        pixmapPainter.setRenderHint( QPainter::Antialiasing, false );
    
        // Paint the background
        QBrush bgBrush( Qt::white );
        if ( d->chartType == CircleChartType || d->chartType == RingChartType )
            bgBrush = QBrush( QColor( 255, 255, 255, 0 ) );
        pixmapPainter.fillRect( paintRect, bgBrush );
    
        // scale the painter's coordinate system to fit the current zoom level
        applyConversion( pixmapPainter, converter );

        d->kdChart->paint( &pixmapPainter, QRect( QPoint( borderX, borderY ), QSize( plotAreaSize.width() - 2 * borderX, plotAreaSize.height() - 2 * borderY ) ) );
    } else {
        // Paint the background
        painter.fillRect( paintRect, QColor( 255, 255, 255, 0 ) );
    
        // scale the painter's coordinate system to fit the current zoom level
        applyConversion( painter, converter );

        d->kdChart->paint( &painter, QRect( QPoint( borderX, borderY ), QSize( plotAreaSize.width() - 2 * borderX, plotAreaSize.height() - 2 * borderY ) ) );
    }
}

void PlotArea::paint( QPainter& painter, const KoViewConverter& converter )
{
    // Calculate the clipping rect
    QRectF paintRect = QRectF( QPointF( 0, 0 ), size() );
    //clipRect.intersect( paintRect );
    painter.setClipRect( converter.documentToView( paintRect ) );

    // Get the current zoom level
    QPointF zoomLevel;
    converter.zoom( &zoomLevel.rx(), &zoomLevel.ry() );

    // Only repaint the pixmap if it is scheduled, the zoom level changed or the shape was resized
    if (    d->pixmapRepaintRequested
         || d->lastZoomLevel != zoomLevel
         || d->lastSize      != size()
         || !d->paintPixmap ) {
        // TODO: What if two zoom levels are constantly being requested?
        // At the moment, this *is* the case, due to the fact
        // that the shape is also rendered in the page overview
        // in KPresenter
        // Everytime the window is hidden and shown again, a repaint is
        // requested --> laggy performance, especially when quickly
        // switching through windows
        paintPixmap( painter, converter );
        d->pixmapRepaintRequested = false;
        d->lastZoomLevel = zoomLevel;
        d->lastSize      = size();
    }

    // Paint the cached pixmap if we got a GO from paintPixmap()
    if ( d->paintPixmap )
        painter.drawImage( 0, 0, d->image );
}

void PlotArea::relayout() const
{
    d->pixmapRepaintRequested = true;
    KoShape::update();
}

#include "PlotArea.moc"

