/* This file is part of the KDE project

   Copyright 2007-2008 Johannes Simon <johannes.simon@gmail.com>
   Copyright 2009      Inge Wallin <inge@lysator.liu.se>

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
#include "PlotArea.h"

// Qt
#include <QPointF>
#include <QSizeF>
#include <QList>
#include <QImage>
#include <QPainter>
#include <kdebug.h>

// KOffice
#include <KoXmlReader.h>
#include <KoXmlWriter.h>
#include <KoShapeLoadingContext.h>
#include <KoShapeSavingContext.h>
#include <KoGenStyles.h>
#include <KoXmlNS.h>
#include <KoTextShapeData.h>
#include <KoOdfLoadingContext.h>
#include <KoOdfStylesReader.h>
#include <KoUnit.h>
#include <KoViewConverter.h>
#include <KoShapeBackground.h>
#include <KoOdfGraphicStyles.h>

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

// KChart
#include "Legend.h"
#include "Surface.h"
#include "Axis.h"
#include "DataSet.h"
#include "ThreeDScene.h"
#include "ChartProxyModel.h"


using namespace KChart;

const int MAX_PIXMAP_SIZE = 1000;

class PlotArea::Private
{
public:
    Private( ChartShape *parent );
    ~Private();

    // The parent chart shape
    ChartShape *shape;

    // ----------------------------------------------------------------
    // Parts and properties of the chart

    ChartType     chartType;
    ChartSubtype  chartSubtype;
    
    Surface       *wall;
    Surface       *floor;       // Only used in 3D charts
    
    // The axes
    QList<Axis*>     axes;
    QList<KoShape*>  automaticallyHiddenAxisTitles;
    
    // 3D properties
    bool          threeD;
    ThreeDScene  *threeDScene;
    
    // ----------------------------------------------------------------
    // Data specific to each types

    // 1. Bar charts
    bool  vertical;
    int   gapBetweenBars;
    int   gapBetweenSets;
    
    // 2. Pie charts
    int   pieExplodeFactor;     // in percents

    // ----------------------------------------------------------------
    // The embedded KD Chart

    // The KD Chart parts
    KDChart::Chart                    *kdChart;
    KDChart::AbstractCoordinatePlane  *kdPlane;
    QList<KDChart::AbstractDiagram*>   kdDiagrams;
    
    // Caching: We can rerender faster if we cache KDChart's output
    QImage   image;
    bool     paintPixmap;
    QPointF  lastZoomLevel;
    QSizeF   lastSize;
    mutable bool pixmapRepaintRequested;
};

PlotArea::Private::Private( ChartShape *parent )
{
    shape = parent;

    // Default type: normal bar chart
    chartType    = BarChartType;
    chartSubtype = NormalChartSubtype;

    wall  = 0;
    floor = 0;

    threeD      = false;
    threeDScene = 0;

    // Data specific for bar charts
    vertical       = false;
    gapBetweenBars = 0;
    gapBetweenSets = 100;

    // KD Chart stuff
    kdChart = new KDChart::Chart();
    kdPlane = new KDChart::CartesianCoordinatePlane( kdChart );

    // Cache
    pixmapRepaintRequested = true;
    paintPixmap            = true;
}

PlotArea::Private::~Private()
{
    delete kdPlane;
    delete kdChart;
}


PlotArea::PlotArea( ChartShape *parent )
    : QObject()
    , KoShape()
    , d( new Private( parent ) )
{
    setShapeId( ChartShapeId );
    
    Q_ASSERT( d->shape );
    Q_ASSERT( d->shape->proxyModel() );
    
    connect( d->shape->proxyModel(), SIGNAL( modelReset() ),
             this,                   SLOT( dataSetCountChanged() ) );
    connect( d->shape->proxyModel(), SIGNAL( modelResetComplete() ),
             this,                   SLOT( update() ) );
    // FIXME: The following signatures don't match.  Bug?
    connect( d->shape->proxyModel(), SIGNAL( rowsInserted( const QModelIndex, int, int ) ),
             this,                   SLOT( dataSetCountChanged() ) );
    connect( d->shape->proxyModel(), SIGNAL( rowsRemoved( const QModelIndex, int, int ) ),
             this,                   SLOT( dataSetCountChanged() ) );
    connect( d->shape->proxyModel(), SIGNAL( columnsInserted( const QModelIndex, int, int ) ),
             this,                   SLOT( update() ) );
    connect( d->shape->proxyModel(), SIGNAL( columnsRemoved( const QModelIndex, int, int ) ),
             this,                   SLOT( update() ) );
    connect( d->shape->proxyModel(), SIGNAL( dataChanged() ),
             this,                   SLOT( update() ) );
}

PlotArea::~PlotArea()
{
    delete d;
}


void PlotArea::init()
{
    d->kdChart->resize( size().toSize() );
    d->kdChart->replaceCoordinatePlane( d->kdPlane );

    KDChart::FrameAttributes attr = d->kdChart->frameAttributes();
    attr.setVisible( false );
    d->kdChart->setFrameAttributes( attr );
    
    d->wall = new Surface( this );
    //d->floor = new Surface( this );
    
    // There need to be at least these two axes. Do not delete, but
    // hide them instead.
    Axis *xAxis = new Axis( this );
    Axis *yAxis = new Axis( this );
    xAxis->setPosition( BottomAxisPosition );
    yAxis->setPosition( LeftAxisPosition );
    yAxis->setShowMajorGrid( true );
    d->axes.append( xAxis );
    d->axes.append( yAxis );
}

void PlotArea::dataSetCountChanged()
{
    if ( !yAxis() )
        return;

    foreach( DataSet *dataSet, proxyModel()->dataSets() ) {
        if ( !dataSet->attachedAxis() ) {
            yAxis()->attachDataSet( dataSet );
        }
    }
}

ChartProxyModel *PlotArea::proxyModel() const
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

Axis *PlotArea::xAxis() const
{
    foreach( Axis *axis, d->axes ) {
        if ( axis->orientation() == Qt::Horizontal )
            return axis;
    }

    return 0;
}

Axis *PlotArea::yAxis() const
{
    foreach( Axis *axis, d->axes ) {
        if ( axis->orientation() == Qt::Vertical )
            return axis;
    }

    return 0;
}

Axis *PlotArea::secondaryXAxis() const
{
    bool firstXAxisFound = false;

    foreach( Axis *axis, d->axes ) {
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

    foreach( Axis *axis, d->axes ) {
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
    if ( d->axes.contains( axis ) ) {
    	qWarning() << "PlotArea::addAxis(): Trying to add already added axis.";
    	return false;
    }

    if ( !axis ) {
    	qWarning() << "PlotArea::addAxis(): Pointer to axis is NULL!";
    	return false;
    }
    d->axes.append( axis );
    
    if ( axis->dimension() == XAxisDimension ) {
        foreach ( Axis *_axis, d->axes )
            _axis->registerKdXAxis( axis->kdAxis() );
    }
    
    requestRepaint();

    return true;
}

bool PlotArea::removeAxis( Axis *axis )
{
    if ( !d->axes.contains( axis ) ) {
    	qWarning() << "PlotArea::removeAxis(): Trying to remove non-added axis.";
    	return false;
    }

    if ( !axis ) {
    	qWarning() << "PlotArea::removeAxis(): Pointer to axis is NULL!";
    	return false;
    }
    d->axes.removeAll( axis );
    
    if ( axis->dimension() == XAxisDimension ) {
        foreach ( Axis *_axis, d->axes )
            _axis->deregisterKdXAxis( axis->kdAxis() );
    }
    
    // This also removes the axis' title, which is a shape as well
    delete axis;
    
    requestRepaint();
    
    return true;
}



void PlotArea::setChartType( ChartType type )
{
    // Lots of things to do if the old and new types of coordinate
    // systems don't match.
    if ( !isPolar( d->chartType ) && isPolar( type ) ) {
        foreach ( Axis *axis, d->axes ) {
            if ( !axis->title()->isVisible() )
                continue;

            axis->title()->setVisible( false );
            d->automaticallyHiddenAxisTitles.append( axis->title() );
        }
    }
    else if ( isPolar( d->chartType ) && !isPolar( type ) ) {
        foreach ( KoShape *title, d->automaticallyHiddenAxisTitles ) {
            title->setVisible( true );
        }
        d->automaticallyHiddenAxisTitles.clear();
    }

    // FIXME BUG: This seems buggy.
    // FIXME NOW: Make this a switch (see comment below)
    // Set the dimensionality of the data points.
    if ( d->chartType != ScatterChartType && type == ScatterChartType ) {
        d->shape->proxyModel()->setDataDimensions( 2 );
    }
    // FIXME: This is not really true.  Could also be 3 (bubble charts)
    else if ( d->chartType == ScatterChartType && type != ScatterChartType ) {
        d->shape->proxyModel()->setDataDimensions( 1 );
    }
    
    d->chartType = type;
    
    foreach ( Axis *axis, d->axes ) {
        axis->plotAreaChartTypeChanged( type );
    }
    
    requestRepaint();
}

void PlotArea::setChartSubType( ChartSubtype subType )
{
    d->chartSubtype = subType;
    
    foreach ( Axis *axis, d->axes ) {
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


// ----------------------------------------------------------------
//                         loading and saving


bool PlotArea::loadOdf( const KoXmlElement &plotAreaElement,
                        KoShapeLoadingContext &context )
{
    KoStyleStack &styleStack = context.odfLoadingContext().styleStack();
    styleStack.save();

    styleStack.clear();

    // Find out about things that are in the plotarea style.
    // 
    // These things include chart subtype, special things for some
    // chart types like line charts, stock charts, etc.
    if ( plotAreaElement.hasAttributeNS( KoXmlNS::chart, "style-name" ) ) {
        context.odfLoadingContext().fillStyleStack( plotAreaElement, KoXmlNS::chart, "style-name", "chart" );

        styleStack.setTypeProperties( "graphic" );
        styleStack.setTypeProperties( "chart" );

        // Set subtypes stacked or percent.
        // These are valid for Bar, Line, Area and Radar types.
        if ( styleStack.hasProperty( KoXmlNS::chart, "percentage" )
             && styleStack.property( KoXmlNS::chart, "percentage" ) == "true" )
        {
            setChartSubType( PercentChartSubtype );
        }
        else if ( styleStack.hasProperty( KoXmlNS::chart, "stacked" )
                  && styleStack.property( KoXmlNS::chart, "stacked" ) == "true" )
        {
            setChartSubType( StackedChartSubtype );
        }

        // Data specific to bar charts
        if ( styleStack.hasProperty( KoXmlNS::chart, "vertical" )
             && styleStack.property( KoXmlNS::chart, "vertical" ) == "true" )
        {
            d->vertical = true;
        }

        // Data direction: It's in the plotarea style.
        if ( styleStack.hasProperty( KoXmlNS::chart, "series-source" ) ) {
            const QString  seriesSource
                = styleStack.property( KoXmlNS::chart, "series-source" );

            kDebug(35001) << "series-source=" << seriesSource;
            if ( seriesSource == "rows" )
                proxyModel()->setDataDirection( Qt::Horizontal );
            else if ( seriesSource == "columns" )
                proxyModel()->setDataDirection( Qt::Vertical );
            else
                // Use the default value for wrong values (not "rows" or "columns")
                proxyModel()->setDataDirection( Qt::Vertical );
        }

        // Special properties for various chart types
#if 0
        switch () {
        case BarChartType:
            if ( styleStack )
                ;
        }
#endif
    }

    loadOdfAttributes( plotAreaElement, context, OdfAllAttributes );
    
    //KoOdfStylesReader &stylesReader = context.odfLoadingContext().stylesReader();
    
    // Find out if the data table contains labels as first row and/or column.
    // This is in the plot-area element itself.
    if ( plotAreaElement.hasAttributeNS( KoXmlNS::chart,
                                         "data-source-has-labels" ) ) {

        // Yes, it does.  Now find out how.
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
    while( !d->axes.isEmpty() ) {
        Axis *axis = d->axes.takeLast();
        Q_ASSERT( axis );
        delete axis;
    }
    
    // A data set is always attached to an axis, so load them first.
    KoXmlElement n;
    forEachElement ( n, plotAreaElement ) {
        if ( n.namespaceURI() != KoXmlNS::chart )
            continue;

        if ( n.localName() == "axis" ) {
            Axis *axis = new Axis( this );
            axis->loadOdf( n, context );
            addAxis( axis );
        }
    }

    // Now, after the axes, load the datasets.
    // Note that this only contains properties of the datasets, the
    // actual data is not stored here.
    //
    // FIXME: Isn't the proxy model a strange place to store this data?
    d->shape->proxyModel()->loadOdf( plotAreaElement, context );

    // Now load the surfaces (wall and possibly floor)
    // FIXME: Use named tags instead of looping?
    forEachElement ( n, plotAreaElement ) {
        if ( n.namespaceURI() != KoXmlNS::chart )
            continue;

        if ( n.localName() == "wall" ) {
            d->wall->loadOdf( n, context );
        }
        else if ( n.localName() == "floor" ) {
            // The floor is not always present, so allocate it if needed.
            if ( !d->floor )
                d->floor = new Surface( this );
            d->floor->loadOdf( n, context );
        }
        else if ( n.localName() != "axis" && n.localName() != "series" ) {
            qWarning() << "PlotArea::loadOdf(): Unknown tag name " << n.localName();
        }
    }

    styleStack.restore();
    
    requestRepaint();
    
    return true;
}

void PlotArea::saveOdf( KoShapeSavingContext &context ) const
{
    KoXmlWriter &bodyWriter = context.xmlWriter();
    //KoGenStyles &mainStyles = context.mainStyles();
    bodyWriter.startElement( "chart:plot-area" );
    
    // FIXME: Somehow this style gets the name gr2 instead of ch2.
    //        Fix that as well.
    KoGenStyle plotAreaStyle( KoGenStyle::StyleChartAuto, "chart" );
    
    // Data direction
    const Qt::Orientation direction = proxyModel()->dataDirection();
    plotAreaStyle.addProperty( "chart:series-source",  
                               ( direction == Qt::Horizontal )
                               ? "rows" : "columns" );
    // Save chart subtype
    saveOdfSubType( bodyWriter, plotAreaStyle );
    
    bodyWriter.addAttribute( "chart:style-name",
                             saveStyle( plotAreaStyle, context ) );
    
    const QSizeF s( size() );
    const QPointF p( position() );
    bodyWriter.addAttributePt( "svg:width",  s.width() );
    bodyWriter.addAttributePt( "svg:height", s.height() );
    bodyWriter.addAttributePt( "svg:x", p.x() );
    bodyWriter.addAttributePt( "svg:y", p.y() );

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

    // Save the axes.
    if ( isCartesian( d->chartType ) ) {
        foreach( Axis *axis, d->axes ) {
            axis->saveOdf( context );
        }
    }
    
    // Save data series
    d->shape->proxyModel()->saveOdf( context );

    // Save the floor and wall of the plotarea.
    d->wall->saveOdf( context, "chart:wall" );
    if ( d->floor )
        d->floor->saveOdf( context, "chart:floor" );

    // TODO chart:series
    // TODO chart:grid
    
    bodyWriter.endElement(); // chart:plot-area
}

void PlotArea::saveOdfSubType( KoXmlWriter& xmlWriter,
                               KoGenStyle& plotAreaStyle ) const
{
    Q_UNUSED( xmlWriter );

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
        // Data specific to bar charts
        if ( d->vertical )
            plotAreaStyle.addProperty( "chart:vertical", "true" );
        // Don't save this if zero, because that's the default.
        //plotAreaStyle.addProperty( "chart:lines-used", 0 ); // FIXME: for now
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

        if ( d->threeD ) {
            plotAreaStyle.addProperty( "chart:three-dimensional", "true" );
            // FIXME: Save all 3D attributes too.
        }

    break;

    case CircleChartType:
        // FIXME
        break;

    case RingChartType:
        // FIXME
        break;

    case ScatterChartType:
        // FIXME
        break;
    case RadarChartType:
        // Save subtype of the Radar chart.
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
        break;

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
    d->gapBetweenBars = percent;

    emit gapBetweenBarsChanged( percent );
}

void PlotArea::setGapBetweenSets( int percent )
{
    d->gapBetweenSets = percent;

    emit gapBetweenSetsChanged( percent );
}

void PlotArea::setPieExplodeFactor( DataSet *dataSet, int percent )
{
    d->pieExplodeFactor = percent;

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
    parent()->legend()->update();
    requestRepaint();
    foreach( Axis* axis, d->axes )
        axis->update();

    KoShape::update();
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
    d->paintPixmap = false;//paintRectSize.width() < MAX_PIXMAP_SIZE || paintRectSize.height() < MAX_PIXMAP_SIZE;
    
    if ( d->paintPixmap ) {
        d->image = QImage( paintRectSize, QImage::Format_RGB32 );
    
        // Copy the painter's render hints, such as antialiasing
        QPainter pixmapPainter( &d->image );
        pixmapPainter.setRenderHints( painter.renderHints() );
        pixmapPainter.setRenderHint( QPainter::Antialiasing, false );
    
        // scale the painter's coordinate system to fit the current zoom level
        applyConversion( pixmapPainter, converter );

        d->kdChart->paint( &pixmapPainter, QRect( QPoint( borderX, borderY ), QSize( plotAreaSize.width() - 2 * borderX, plotAreaSize.height() - 2 * borderY ) ) );
    } else {

        d->kdChart->paint( &painter, QRect( QPoint( borderX, borderY ), QSize( plotAreaSize.width() - 2 * borderX, plotAreaSize.height() - 2 * borderY ) ) );
    }
}

void PlotArea::paint( QPainter& painter, const KoViewConverter& converter )
{
    //painter.save();

    // First of all, scale the painter's coordinate system to fit the current zoom level
    applyConversion( painter, converter );
    
    // Calculate the clipping rect
    QRectF paintRect = QRectF( QPointF( 0, 0 ), size() );
    //clipRect.intersect( paintRect );
    painter.setClipRect( paintRect );
    
    // Paint the background
    if ( background() ) {
        QPainterPath p;
        p.addRect( paintRect );
        background()->paint( painter, p );
    }

    // Get the current zoom level
    QPointF zoomLevel;
    converter.zoom( &zoomLevel.rx(), &zoomLevel.ry() );

    // Only repaint the pixmap if it is scheduled, the zoom level
    // changed or the shape was resized.
    /*if (    d->pixmapRepaintRequested
         || d->lastZoomLevel != zoomLevel
         || d->lastSize      != size()
         || !d->paintPixmap ) {
        // TODO (js): What if two zoom levels are constantly being
        //            requested?  At the moment, this *is* the case,
        //            due to the fact that the shape is also rendered
        //            in the page overview in KPresenter Every time
        //            the window is hidden and shown again, a repaint
        //            is requested --> laggy performance, especially
        //            when quickly switching through windows.
        //
        // ANSWER (iw): what about having a small mapping between size
        //              in pixels and pixmaps?  The size could be 2 or
        //              at most 3.  We could manage the replacing
        //              using LRU.
        paintPixmap( painter, converter );
        d->pixmapRepaintRequested = false;
        d->lastZoomLevel = zoomLevel;
        d->lastSize      = size();
    }*/
    painter.setRenderHint( QPainter::Antialiasing, false );
    d->kdChart->paint( &painter, paintRect.toRect() );
    //painter.restore();

    // Paint the cached pixmap if we got a GO from paintPixmap()
    //if ( d->paintPixmap )
    //    painter.drawImage( 0, 0, d->image );
}

void PlotArea::relayout() const
{
    Q_ASSERT( d->kdPlane );
    d->kdPlane->relayout();
    update();
}

#include "PlotArea.moc"

