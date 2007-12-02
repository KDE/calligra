/* This file is part of the KDE project

   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2007 Inge Wallin     <inge@lysator.liu.se>

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
#include "ChartShape.h"

// Posix
#include <float.h> // For basic data types characteristics.

// Qt
#include <QAbstractItemModel>
#include <QStandardItemModel>
#include <QPainter>
#include <QPixmap>

// KDE
#include <kapplication.h>
#include <kdebug.h>
#include <klocale.h>

// KOffice
#include <KoViewConverter.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>
#include <KoShapeSavingContext.h>
#include <KoXmlNS.h>
#include <KoOasisLoadingContext.h>
#include <KoShapeLoadingContext.h>

// KDChart
#include "KDChartEnums"
#include "KDChartPosition"
#include "KDChartAbstractCoordinatePlane"
#include "KDChartBackgroundAttributes"
#include "KDChartBarDiagram"
#include "KDChartLineDiagram"
#include "KDChartCartesianAxis"
#include "KDChartCartesianCoordinatePlane"
#include "KDChartChart"
#include "KDChartPieDiagram"
#include "KDChartPolarDiagram"
#include "KDChartRingDiagram"
#include "KDChartFrameAttributes"
#include "KDChartGridAttributes"
#include "KDChartLegend"
#include "KDChartHeaderFooter"
#include "KDChartLineAttributes"
#include "KDChartDataValueAttributes"
#include "KDChartMarkerAttributes"
#include "KDChartTextAttributes"
#include "KDChartAttributesModel"

#include "KDChartBarAttributes"
#include "KDChartThreeDBarAttributes"
#include "KDChartLineAttributes"
#include "KDChartThreeDLineAttributes"
#include "KDChartPieAttributes"
#include "KDChartThreeDPieAttributes"

// KChart
#include "kchart_global.h"
#include "ChartProxyModel.h"


using namespace KChart;
//using namespace KDChart;


static bool isPolar( OdfChartType type )
{
    return ( type == CircleChartType
	     || type == RingChartType
	     || type == RadarChartType );
}

static bool isCartesian( OdfChartType type )
{
    return !isPolar( type );
}


// The private class

class ChartShape::Private
{
public:
    Private();
    ~Private();

    // Basic info about the chart
    OdfChartType        chartType;
    OdfChartSubtype     chartSubtype;
    bool                threeDMode; // Whether we're in 2D or 3D mode

    // The underlying engine
    KDChart::Chart            *chart;
    KDChart::AbstractDiagram  *diagram;
    KDChart::Legend           *legend;
    KDChart::HeaderFooter     *title;
    KDChart::HeaderFooter     *subTitle;
    KDChart::HeaderFooter     *footer;

    // About the data
    bool                       firstRowIsLabel;
    bool                       firstColIsLabel;

    // Proxy model that holds the actual QAbstractItemModel
    ChartProxyModel           *chartModel;
    QAbstractItemModel        *internalModel;
    QAbstractItemModel        *externalModel;
    bool                       takeOwnershipOfModel;

    // We can rerender faster if we cache KDChart's output
    QPixmap  pixmap;
    QPointF  lastZoomLevel;
    QSizeF   lastSize;
    bool     pixmapRepaintRequested;

    // ----------------------------------------------------------------
    // Data that are not immediately applicable to the chart itself.

    // The last subtype that each main type had when it was last used.
    ChartTypeOptions  chartTypeOptions[NUM_CHARTTYPES];
};


const OdfChartSubtype  defaultSubtypes[NUM_CHARTTYPES] = {
    NormalChartSubtype,     // Bar
    NormalChartSubtype,     // Line
    NormalChartSubtype,     // Area
    NoChartSubtype,         // Circle
    NoChartSubtype,         // Ring
    NoChartSubtype,         // Scatter
    NoChartSubtype,         // radar
    NoChartSubtype,         // Stock
    NoChartSubtype,         // Bubble
    NoChartSubtype,         // Surface
    NoChartSubtype          // Gantt
};

ChartShape::Private::Private()
{
    for ( int i = 0; i < NUM_CHARTTYPES; ++i )
        chartTypeOptions[i].subtype = defaultSubtypes[i];  
    threeDMode             = false;
    takeOwnershipOfModel   = false;
    internalModel          = 0;
    externalModel          = 0;
    pixmapRepaintRequested = true;
    chart                  = 0;
    diagram                = 0;
    legend                 = 0;
    title                  = 0;
    subTitle               = 0;
    footer                 = 0;
}


ChartShape::Private::~Private()
{
    if ( takeOwnershipOfModel && externalModel != 0 )
        delete externalModel;
    if ( internalModel != 0 )
        delete internalModel;
    delete chartModel;
    delete diagram;
    delete chart;
}

// ================================================================


ChartShape::ChartShape()
    : d( new Private )
{
    setShapeId( ChartShapeId );

    // Default type and subtype
    d->chartType    = BarChartType;
    d->chartSubtype = NormalChartSubtype;

    // Initialize a basic chart.
    d->chartModel = new ChartProxyModel( this );
    d->chart      = new KDChart::Chart();
    d->diagram    = new KDChart::BarDiagram();

    d->chart->coordinatePlane()->replaceDiagram( d->diagram );

    d->diagram->setModel( d->chartModel );

    d->firstRowIsLabel = false;
    d->firstColIsLabel = false;

    // Add axes to the diagram
    KDChart::AbstractCartesianDiagram  *diagram = static_cast<KDChart::AbstractCartesianDiagram*>( d->diagram );
    KDChart::CartesianAxis  *xAxis = new KDChart::CartesianAxis( diagram );
    KDChart::CartesianAxis  *yAxis = new KDChart::CartesianAxis( diagram );
    xAxis->setPosition( KDChart::CartesianAxis::Bottom );
    yAxis->setPosition( KDChart::CartesianAxis::Left );
    diagram->addAxis( xAxis );
    diagram->addAxis( yAxis );

    // Add a legend
    d->legend = new KDChart::Legend( d->diagram, d->chart ); 
    d->chart->addLegend( d->legend );

    setChartDefaults();

}

ChartShape::~ChartShape()
{
    delete d;
}

void ChartShape::refreshPixmap( QPainter &painter, const KoViewConverter &converter )
{
    // Adjust the size of the pixmap to the current zoom level
    d->pixmap = QPixmap( converter.documentToView( size() ).toSize() );
    const QRect paintRect = QRect( QPoint( 0, 0 ), size().toSize() );

    // Copy the painter's render hints, such as antialiasing
    QPainter pixmapPainter( &d->pixmap );
    pixmapPainter.setRenderHints( painter.renderHints() );

    // Adjust the pixmapPainter's coordinate system to the current zoom level
    applyConversion( pixmapPainter, converter );

    // Paint the background
    pixmapPainter.fillRect( paintRect, KApplication::palette().base() );
    d->chart->paint( &pixmapPainter, paintRect );
}

KDChart::Chart* ChartShape::chart() const
{
    return d->chart;
}

void ChartShape::setChartDefaults()
{
    d->legend->setPosition( KDChart::Position::East );
    d->legend->setAlignment( Qt::AlignRight );
    d->legend->setShowLines( false );
    d->legend->setTitleText( i18n( "Legend" ) );
    d->legend->setOrientation( Qt::Vertical );

    setDiagramDefaults( d->chartType );
}

void ChartShape::setDiagramDefaults( OdfChartType type  /* = LastChartType */ )
{
    if ( type != LineChartType && type != ScatterChartType )
        d->diagram->setPen( QPen( Qt::black, 0.4 ) );

    switch ( type )
    {
        case AreaChartType:
        {
            KDChart::LineAttributes attributes;
            attributes = ((KDChart::LineDiagram*) d->diagram)->lineAttributes();
            attributes.setDisplayArea( true );
            ((KDChart::LineDiagram*) d->diagram)->setLineAttributes( attributes );
        }
        break;

        case ScatterChartType:
        {
            KDChart::DataValueAttributes attributes = ((KDChart::LineDiagram*) d->diagram)->dataValueAttributes();
            KDChart::MarkerAttributes markerAttributes = attributes.markerAttributes();
            KDChart::TextAttributes   textAttributes   = attributes.textAttributes();
            markerAttributes.setVisible( true );
            textAttributes.setVisible( false );
            attributes.setTextAttributes( textAttributes );
            attributes.setMarkerAttributes( markerAttributes );
            attributes.setVisible( true );

            ((KDChart::LineDiagram*) d->diagram)->setDataValueAttributes( attributes );

            ((KDChart::LineDiagram*) d->diagram)->setPen( Qt::NoPen );
        }
        break;

        case BubbleChartType:
        {
            KDChart::DataValueAttributes attributes       = ((KDChart::LineDiagram*) d->diagram)->dataValueAttributes();
            KDChart::MarkerAttributes    markerAttributes = attributes.markerAttributes();
            KDChart::TextAttributes      textAttributes   = attributes.textAttributes();

            KDChart::LineAttributes      lineAttributes   = ((KDChart::LineDiagram*) d->diagram)->lineAttributes();

            markerAttributes.setMarkerStyle( KDChart::MarkerAttributes::MarkerCircle );
            markerAttributes.setVisible( true );
            textAttributes.setVisible( false );
            attributes.setTextAttributes( textAttributes );
            attributes.setMarkerAttributes( markerAttributes );
            attributes.setVisible( true );

            ((KDChart::LineDiagram*) d->diagram)->setDataValueAttributes( attributes );

            ((KDChart::LineDiagram*) d->diagram)->setPen( Qt::NoPen );
        }
        break;

        default:
        break;
    };
}

void ChartShape::setChartType( OdfChartType    newType,
                               OdfChartSubtype newSubtype )
{
    KDChart::AbstractDiagram           *new_diagram = 0;
    KDChart::CartesianCoordinatePlane  *cartPlane = 0;
    KDChart::PolarCoordinatePlane      *polPlane  = 0;

    if (d->chartType == newType && d->chartSubtype == newSubtype)
        return;

    saveChartTypeOptions();

    if ( d->chartType != newType ) {
        switch ( newType ) {
        case BarChartType:
            new_diagram = new KDChart::BarDiagram( d->chart, cartPlane );
            break;

        case LineChartType:
            new_diagram = new KDChart::LineDiagram( d->chart, cartPlane );
            break;

        case AreaChartType:
            new_diagram = new KDChart::LineDiagram( d->chart, cartPlane );
            break;

        case CircleChartType:
            new_diagram = new KDChart::PieDiagram( d->chart, polPlane );
            break;

        case RingChartType:
            new_diagram = new KDChart::RingDiagram( d->chart, polPlane );
            break;

        case ScatterChartType:
            new_diagram = new KDChart::LineDiagram( d->chart, cartPlane );
            break;

        case RadarChartType:
            new_diagram = new KDChart::PolarDiagram( d->chart, polPlane );
            break;

        case StockChartType:
            return;
            break;

        case BubbleChartType:
            new_diagram = new KDChart::LineDiagram( d->chart, cartPlane );
            break;

        case SurfaceChartType:
            // FIXME
            return;
            break;

        case GanttChartType:
            // FIXME
            return;
            break;

        case LastChartType:
        default:
            return;
            break;

        }
    }

    // Check if we need another type of coordinate plane than we
    // already have before.
    if ( new_diagram != NULL ) {
        if ( isPolar( d->chartType ) && isCartesian( newType ) ) {
            cartPlane = new KDChart::CartesianCoordinatePlane( d->chart );
            d->chart->replaceCoordinatePlane( cartPlane );
        }
        else if ( isCartesian( d->chartType ) && isPolar( newType ) ) {
            polPlane = new KDChart::PolarCoordinatePlane( d->chart );
            d->chart->replaceCoordinatePlane( polPlane );
        }
        else if ( isCartesian( d->chartType ) && isCartesian( newType ) ) {
            KDChart::AbstractCartesianDiagram *old =
                    ( KDChart::AbstractCartesianDiagram* )( d->chart->coordinatePlane()->diagram() );
            foreach ( KDChart::CartesianAxis* axis, old->axes() ) {
                old->takeAxis( axis );
                ( ( KDChart::AbstractCartesianDiagram* ) new_diagram )->addAxis( axis );
            }
        }

        new_diagram->setModel( d->chartModel );

        // This will crash if the new model for new_diagram is not set.
        // Thus, put it after new_diagram->setModel().
        KDChart::LegendList legends = d->chart->legends();
        foreach ( KDChart::Legend* legend, legends )
            legend->setDiagram( new_diagram );

        // FIXME: Aren't we leaking memory by not doing this?
        // KDChartAbstractDiagram::~KDChartAbstractDiagram() will try to delete
        // its KDChartAttributesModel instance, which would cause a crash.
        if( new_diagram != 0 ) {
            d->chart->coordinatePlane()->replaceDiagram( new_diagram );
            d->diagram = new_diagram;
            setDiagramDefaults( newType );
            //delete new_diagram;
        }

        update();

        // Update local data
        d->chartType = newType;
        // Reset the chart subtype to make sure setChartSubtype() will
        // set the subtype of the new chart to the old one. Otherwise,
        // it'll return because there apparently is nothing to change.
        d->chartSubtype = NoChartSubtype;
    }

    restoreChartTypeOptions( d->chartType );

    // Only set the new subtype if it's valid, and if the argument
    // was provided ( that is, if the argument is not at it's default defined in ChartShape.h )
    if( newSubtype != NoChartSubtype )
        setChartSubtype( newSubtype );
}

void ChartShape::setChartSubtype( OdfChartSubtype newSubtype )
{
    // Nothing to do if no change.
    if ( d->chartSubtype == newSubtype )
        return;

    // Convert between ODF subtypes and KDChart subtypes..
    
    switch ( d->chartType ) {
    case BarChartType:
        switch ( newSubtype ) {
        case NoChartSubtype:
            break;
        case NormalChartSubtype:
            ( ( KDChart::BarDiagram* ) d->diagram )->setType( KDChart::BarDiagram::Normal );
            break;
        case StackedChartSubtype:
            ( ( KDChart::BarDiagram* ) d->diagram )->setType( KDChart::BarDiagram::Stacked );
            break;
        case PercentChartSubtype:
            ( ( KDChart::BarDiagram* ) d->diagram )->setType( KDChart::BarDiagram::Percent );
            break;
        }

    case LineChartType:
    case AreaChartType:
        switch ( newSubtype ) {
        case NoChartSubtype:
            break;
        case NormalChartSubtype:
            ( ( KDChart::LineDiagram* ) d->diagram )->setType( KDChart::LineDiagram::Normal );
            break;
        case StackedChartSubtype:
            ( ( KDChart::LineDiagram* ) d->diagram )->setType( KDChart::LineDiagram::Stacked );
            break;
        case PercentChartSubtype:
            ( ( KDChart::LineDiagram* ) d->diagram )->setType( KDChart::LineDiagram::Percent );
            break;
        }

    case CircleChartType:
        break;
    case RingChartType:
        break;
    case ScatterChartType:
        break;
    case RadarChartType:
        break;
    case StockChartType:
        break;
    case BubbleChartType:
        break;
    case SurfaceChartType:
        break;
    case GanttChartType:
        break;
    default:
        break;
    }

    d->chartSubtype = newSubtype;

    update();
}

void ChartShape::setThreeDMode( bool threeD )
{
    switch ( d->chartType ) {
        case BarChartType:
            {
                KDChart::ThreeDBarAttributes attributes( ( ( KDChart::BarDiagram* )d->diagram )->threeDBarAttributes() );
                attributes.setEnabled( threeD );
                ( ( KDChart::BarDiagram* )d->diagram )->setThreeDBarAttributes( attributes );
            }
            break;
        case LineChartType:
            {
                KDChart::ThreeDLineAttributes attributes( ( ( KDChart::LineDiagram* )d->diagram )->threeDLineAttributes() );
                attributes.setEnabled( threeD );
                ( ( KDChart::LineDiagram* )d->diagram )->setThreeDLineAttributes( attributes );
            }
            break;
        case CircleChartType:
            {
                KDChart::ThreeDPieAttributes attributes( ( ( KDChart::PieDiagram* )d->diagram )->threeDPieAttributes() );
                attributes.setEnabled( threeD );
                ( ( KDChart::PieDiagram* )d->diagram )->setThreeDPieAttributes( attributes );
            }
            break;
        default:
            return;
    }
    d->threeDMode = threeD;

    update();
}

void ChartShape::setFirstRowIsLabel( bool b )
{
    d->chartModel->setFirstRowIsLabel( b );
}

void ChartShape::setFirstColumnIsLabel( bool b )
{
    d->chartModel->setFirstColumnIsLabel( b );
}

void ChartShape::setDataDirection( Qt::Orientation orientation )
{
    d->chartModel->setDataDirection( orientation );
}

void ChartShape::setLegendTitle( const QString &title )
{
    d->legend->setTitleText( title );
    update();
}

void ChartShape::setLegendTitleFont( const QFont& font )
{
    KDChart::TextAttributes attributes = d->legend->titleTextAttributes();
    attributes.setFont( font );
    d->legend->setTitleTextAttributes( attributes );
    update();
}

void ChartShape::setLegendFont( const QFont& font )
{
    KDChart::TextAttributes attributes = d->legend->textAttributes();
    attributes.setFont( font );
    d->legend->setTextAttributes( attributes );
    update();
}

void ChartShape::setLegendFontSize( int size )
{
    KDChart::TextAttributes attributes = d->legend->textAttributes();
    attributes.setFontSize( size );
    d->legend->setTextAttributes( attributes );
    update();
}

void ChartShape::setLegendSpacing( int spacing )
{
    d->legend->setSpacing( ( uint )spacing );
    update();
}

void ChartShape::setLegendShowLines( bool b )
{
    d->legend->setShowLines( b );
    update();
}

void ChartShape::setLegendOrientation( Qt::Orientation orientation )
{
    d->legend->setOrientation( orientation );
    update();
}

void ChartShape::setLegendAlignment( Qt::Alignment alignment )
{
    d->legend->setAlignment( alignment );
    update();
}

void ChartShape::setLegendFixedPosition( KDChart::Position position )
{
    d->legend->setPosition( position );
    update();
}

void ChartShape::saveChartTypeOptions()
{
    // Check if the int value is in range of the OdfChartType enumeration
    if( d->chartType < BarChartType && d->chartType >= LastChartType )
        return;

    d->chartTypeOptions[( int )d->chartType].subtype = d->chartSubtype;
}

void ChartShape::restoreChartTypeOptions( OdfChartType type )
{
    // Check if the int value is in range of the OdfChartType enumeration
    if( type < BarChartType && type >= LastChartType )
        return;

    setChartSubtype( d->chartTypeOptions[( int )type].subtype );
    setThreeDMode( d->threeDMode );
}

void ChartShape::setModel( QAbstractItemModel *model, bool takeOwnershipOfModel /* = false */ )
{
    d->externalModel = model;
    d->chartModel->setSourceModel( model );
    d->takeOwnershipOfModel = takeOwnershipOfModel;

    delete d->internalModel;
    d->internalModel = 0;
}

void ChartShape::setInternalModel( QAbstractItemModel *model )
{
    d->internalModel = model;
    d->chartModel->setSourceModel( model );
}

bool ChartShape::hasInternalModel()
{
    if ( d->internalModel == 0 )
        return false;
    return true;
}

QAbstractItemModel *ChartShape::model()
{
    return d->chartModel;
}

OdfChartType ChartShape::chartType() const
{
    return d->chartType;
}

OdfChartSubtype ChartShape::chartSubtype() const
{
    return d->chartSubtype;
}

bool ChartShape::threeDMode() const
{
    return d->threeDMode;
}

ChartTypeOptions ChartShape::chartTypeOptions( OdfChartType type ) const
{
    if ( type >= BarChartType && type < LastChartType)
        return d->chartTypeOptions[( int )type];

    return ChartTypeOptions();
}

void ChartShape::paint( QPainter& painter, const KoViewConverter& converter )
{
    // Calculate the clipping rect
    QRectF clipRect  = painter.clipRegion().boundingRect();
    QRectF paintRect = converter.documentToView( QRectF( position(), size() ) );
    clipRect.intersect( paintRect );
    painter.setClipRect( clipRect );

    // Get the current zoom level
    QPointF zoomLevel;
    converter.zoom( &zoomLevel.rx(), &zoomLevel.ry() );

    // Only repaint the pixmap if it is scheduled, the zoom level changed or the shape was resized
    if (    d->pixmapRepaintRequested
         || d->lastZoomLevel != zoomLevel
         || d->lastSize      != size() ) {
        refreshPixmap( painter, converter );
        d->pixmapRepaintRequested = false;
        d->lastZoomLevel = zoomLevel;
        d->lastSize      = size();
    }

    // Paint the cached pixmap
    painter.drawPixmap( 0, 0, d->pixmap );
}


// ================================================================
//               OpenDocument loading and saving


static const struct {
    OdfChartType   chartType;
    const char    *odfName;
} odfChartTypes[] = {
    { BarChartType ,    "chart:bar"     },
    { LineChartType,    "chart:line"    },
    { AreaChartType ,   "chart:area"    },
    { CircleChartType,  "chart:circle"  },
    { RingChartType,    "chart:ring"    },
    { ScatterChartType, "chart:scatter" },
    { RadarChartType,   "chart:radar"   },
    { StockChartType,   "chart:stock"   },
    { BubbleChartType,  "chart:bubble"  },
    { SurfaceChartType, "chart:surface" },
    { GanttChartType,   "chart:gantt"   },
};

static const unsigned int  numOdfChartTypes = ( sizeof odfChartTypes
                                                / sizeof *odfChartTypes );


// ----------------------------------------------------------------
//                             Loading


bool ChartShape::loadOdf( const KoXmlElement    &chartElement, 
			  KoShapeLoadingContext &context )
{
    if ( chartElement.hasAttributeNS( KoXmlNS::chart, "class" ) ) {
        kDebug() << " ---------------------------------------------------------------- " ;
        kDebug() << " Chart class: " 
                 <<  chartElement.attributeNS( KoXmlNS::chart, "class" );
    }
    else
        return false;


    // 1. Load the chart type.
    const QString chartClass = chartElement.attributeNS( KoXmlNS::chart,
                                                         "class", QString() );

    // Find out what charttype the chart class corresponds to.
    bool  knownType = false;
    for ( unsigned int i = 0 ; i < numOdfChartTypes ; ++i ) {
        if ( chartClass == odfChartTypes[i].odfName ) {
            kDebug(35001) <<"found chart of type" << chartClass;

            setChartType( odfChartTypes[i].chartType );
            knownType = true;
            break;  
        }
    }

    // If we can't find out what charttype it is, we might as well end here.
    if ( !knownType ) {
        // FIXME: Find out what the equivalent of
        //        KoDocument::setErrorMessage() is for KoShape.
        //setErrorMessage( i18n( "Unknown chart type %1" ,chartClass ) );
        return false;
    }

    // 2. Load the title.
    KoXmlElement titleElem = KoXml::namedItemNS( chartElement, 
                                                    KoXmlNS::chart, "title" );
    if( !titleElem.isNull() ) {
        if( !loadOdfTitle( titleElem, context) )
            return false;
    }

    // 3. Load the subtitle.
    KoXmlElement subTitleElem = KoXml::namedItemNS( chartElement, 
                                                    KoXmlNS::chart, "subtitle" );
    if( !subTitleElem.isNull() ) {
        if( !loadOdfSubTitle( subTitleElem, context) )
            return false;
    }

    // 4. Load the footer.
    KoXmlElement footerElem = KoXml::namedItemNS( chartElement, 
                                                  KoXmlNS::chart, "footer" );
    if( !footerElem.isNull() ) {
        if( !loadOdfFooter( footerElem, context) )
            return false;
    }

    // 5. Load the legend.
    KoXmlElement legendElem = KoXml::namedItemNS( chartElement, KoXmlNS::chart,
						  "legend" );
    if ( !legendElem.isNull() ) {
	if ( !loadOdfLegend( legendElem, context ) )
	    return false;
    }

    // 6. Load the plot area (this is where the real action is!).
    KoXmlElement  plotareaElem = KoXml::namedItemNS( chartElement,
						     KoXmlNS::chart, "plot-area" );
    if ( !plotareaElem.isNull() ) {
	if ( !loadOdfPlotarea( plotareaElem, context ) )
	    return false;
    }

    // 7. Load the data
    KoXmlElement  dataElem = KoXml::namedItemNS( chartElement,
						 KoXmlNS::table, "table" );
    if ( !dataElem.isNull() ) {
	if ( !loadOdfData( dataElem, context ) )
	    return false;
    }

#if 0  // Taken from old kchart_params.cpp: Use what we can from here
       // and throw away the rest.



    // Subtitle TODO (more details)
    KoXmlElement subtitleElem = KoXml::namedItemNS( chartElem, KoXmlNS::chart,
						   "subtitle" );
    if ( !subtitleElem.isNull() ) {
        loadingContext.styleStack().save();
        loadingContext.fillStyleStack( subtitleElem, KoXmlNS::chart, "style-name", "chart" );
        QFont font;
        QColor color;
        loadOasisFont( loadingContext, font, color );
        setHeaderFooterFont( KDChartParams::HdFtPosHeader2, font, true, font.pointSize() );
        setHeaderFooterColor( KDChartParams::HdFtPosHeader2, color );
        loadingContext.styleStack().restore();

	KoXmlElement  pElem = KoXml::namedItemNS( subtitleElem,
						 KoXmlNS::text, "p" );
	setHeader2Text( pElem.text() );
    }

    // Footer TODO (more details)
    KoXmlElement footerElem = KoXml::namedItemNS( chartElem, KoXmlNS::chart,
						 "footer" );
    if ( !footerElem.isNull() ) {
        loadingContext.styleStack().save();
        loadingContext.fillStyleStack( footerElem, KoXmlNS::chart, "style-name", "chart" );
        QFont font;
        QColor color;
        loadOasisFont( loadingContext, font, color );
        setHeaderFooterFont( KDChartParams::HdFtPosFooter, font, true, font.pointSize() );
        setHeaderFooterColor( KDChartParams::HdFtPosFooter, color );
        loadingContext.styleStack().restore();

	KoXmlElement  pElem = KoXml::namedItemNS( footerElem,
						 KoXmlNS::text, "p" );
	setFooterText( pElem.text() );
    }

    // TODO: Get legend settings
    KoXmlElement legendElem = KoXml::namedItemNS( chartElem, KoXmlNS::chart,
						 "legend" );
    if ( !legendElem.isNull() ) {
        loadingContext.styleStack().save();
        loadingContext.fillStyleStack( legendElem, KoXmlNS::chart, "style-name", "chart" );
        QFont font;
        QColor color;
        loadOasisFont( loadingContext, font, color );
        //tz I didn't find that Oasis support separate font/colors for the title and the rest of the legend
        setLegendFont( font, true );
        setLegendTitleFont( font, true );
        setLegendTextColor( color );
        setLegendTitleTextColor( color );
        loadingContext.styleStack().restore();
        QString lp;
        if ( legendElem.hasAttributeNS( KoXmlNS::chart, "legend-position" ) )
        {
            lp = legendElem.attributeNS( KoXmlNS::chart, "legend-position", QString() );
        }
        QString lalign;
        if ( legendElem.hasAttributeNS( KoXmlNS::chart, "legend-align" ) )
        {
            lalign = legendElem.attributeNS( KoXmlNS::chart, "legend-align", QString() );
        }

        LegendPosition lpos = NoLegend;
        int align = 1;
        if ( lalign == "start" )
        {
            align = 0;
        }
        else if ( lalign == "end" )
        {
            align = 2;
        }

        if ( lp == "start" )
        {
            lpos = LegendLeft;
            if ( align == 0 )
                lpos = LegendTopLeftLeft;
            else if ( align == 2 )    
                lpos = LegendBottomLeftLeft;
        }
        else if ( lp == "end" )
        {
            lpos = LegendRight;
            if ( align == 0 )
                lpos = LegendTopRightRight;
            else if ( align == 2 )    
                lpos = LegendBottomRightRight;
        }
        else if ( lp == "top" )
        {
            lpos = LegendTop;
            if ( align == 0 )
                lpos = LegendTopLeftTop;
            else if ( align == 2 )    
                lpos = LegendTopRightTop;
        }
        else if ( lp == "bottom" )
        {
            lpos = LegendBottom;
            if ( align == 0 )
                lpos = LegendBottomLeftBottom;
            else if ( align == 2 )    
                lpos = LegendBottomRightBottom;
        }
        else if ( lp == "top-start" )
        {
            lpos = LegendTopLeft;
        }
        else if ( lp == "bottom-start" )
        {
            lpos = LegendBottomLeft;
        }
        else if ( lp == "top-end" )
        {
            lpos = LegendTopRight;
        }
        else if ( lp == "bottom-end" )
        {
            lpos = LegendBottomRight;
        }

        setLegendPosition( lpos );
        //bodyWriter->addAttribute( "koffice:title", legendTitleText() );
        if ( legendElem.hasAttributeNS( KoXmlNS::koffice, "title" ) )
        {
            setLegendTitleText( legendElem.attributeNS( KoXmlNS::koffice, "title", QString() ) );
        }
    }
    else
    {
        setLegendPosition( NoLegend );
    }

    return false;
#endif

    return true;
}


bool ChartShape::loadOdfTitle ( const KoXmlElement &titleElement,
                                    KoShapeLoadingContext &context ) 
{
    // TODO: Read optional attributes
    // 1. Table range
    // 2. Position and size
    // 3. Style name
    KoXmlElement  pElement = KoXml::namedItemNS( titleElement,
                                            KoXmlNS::text, "p" );
    KDChart::HeaderFooter *old = d->title;
    d->title = new KDChart::HeaderFooter();
    d->title->setType( KDChart::HeaderFooter::Header );
    d->title->setPosition( KDChart::Position::North );
    d->title->setText( pElement.text() );
    d->chart->replaceHeaderFooter( d->title, old );
    return true;
}

bool ChartShape::loadOdfSubTitle ( const KoXmlElement &titleElement,
                                    KoShapeLoadingContext &context ) 
{
    // TODO: Read optional attributes
    // 1. Table range
    // 2. Position and size
    // 3. Style name
    KoXmlElement  pElement = KoXml::namedItemNS( titleElement,
                                            KoXmlNS::text, "p" );
    KDChart::HeaderFooter *old = d->subTitle;
    d->subTitle = new KDChart::HeaderFooter();
    d->subTitle->setType( KDChart::HeaderFooter::Header );
    d->subTitle->setPosition( KDChart::Position::North );
    d->subTitle->setText( pElement.text() );
    d->chart->replaceHeaderFooter( d->subTitle, old );
    return true;
}

bool ChartShape::loadOdfFooter ( const KoXmlElement &footerElement,
                                 KoShapeLoadingContext &context ) 
{
    // TODO: Read optional attributes
    // 1. Table range
    // 2. Position and size
    // 3. Style name
    KoXmlElement  pElement = KoXml::namedItemNS( footerElement,
                                                 KoXmlNS::text, "p" );
    KDChart::HeaderFooter *old = d->footer;
    d->footer = new KDChart::HeaderFooter();
    d->footer->setType( KDChart::HeaderFooter::Header );
    d->footer->setPosition( KDChart::Position::North );
    d->footer->setText( pElement.text() );
    d->chart->replaceHeaderFooter( d->footer, old );
    return true;
}

bool ChartShape::loadOdfLegend( const KoXmlElement    &legendElement, 
				KoShapeLoadingContext &context )
{
    // TODO: Read optional attributes
    // 1. Legend placement
    // 2. Legend expansion
    // 3. Legend styling
    // 4. KOffice specific title name
    KDChart::Legend *old = d->legend;
    d->legend = new KDChart::Legend( d->diagram, d->chart );
    d->chart->replaceLegend( d->legend, old );
    return true;
}

bool ChartShape::loadOdfPlotarea( const KoXmlElement    &plotareaElement, 
				  KoShapeLoadingContext &context )
{
    return true;
}

bool ChartShape::loadOdfData( const KoXmlElement    &dataElement, 
			      KoShapeLoadingContext &context )
{
    return true;
}

// ----------------------------------------------------------------
//                             Saving




void ChartShape::saveOdf( KoShapeSavingContext & context ) const
{
    KoXmlWriter&  bodyWriter = context.xmlWriter();
    KoGenStyles&  mainStyles( context.mainStyles() );

    kError(32001) << "----------------------------------------------------------------";

    bodyWriter.startElement( "chart:chart" );

    // 1. Write the chart type.
    bool knownType = false;
    for ( unsigned int i = 0 ; i < numOdfChartTypes ; ++i ) {
        if ( d->chartType == odfChartTypes[i].chartType ) {
            bodyWriter.addAttribute( "chart:class", odfChartTypes[i].odfName );
            knownType = true;
            break;
        }
    }
    if ( !knownType ) {
        kError(32001) << "Unknown chart type in ChartShape::saveOdf:"
                      << (int) d->chartType << endl;
    }

    // 2. Write the title.
    saveOdfTitle( bodyWriter, mainStyles );

    // 3. Write the subtitle.
    saveOdfSubTitle( bodyWriter, mainStyles );

    // 4. Write the footer.
    saveOdfFooter( bodyWriter, mainStyles );

    // 5. Write the legend.
    saveOdfLegend( bodyWriter, mainStyles );

    // 6. Write the plot area (this is where the real action is!).
    bodyWriter.startElement( "chart:plot-area" );
    saveOdfPlotarea( bodyWriter, mainStyles );
    bodyWriter.endElement();

    // 7. Save the data
    saveOdfData( bodyWriter, mainStyles );

    bodyWriter.endElement(); // chart:chart
}


void ChartShape::saveOdfTitle( KoXmlWriter& bodyWriter,
                               KoGenStyles& mainStyles ) const
{
    // Optional element
    if ( d->title ) {
        bodyWriter.startElement( "chart:title" );
        QRect rect( d->title->geometry() );
        bodyWriter.addAttributePt( "svg:x", rect.x() );
        bodyWriter.addAttributePt( "svg:y", rect.y() );
        KDChart::TextAttributes ta = d->title->textAttributes();
        bodyWriter.addAttribute( "chart:style-name", saveOdfFont( mainStyles, ta.font(), ta.pen().color() ) );
        bodyWriter.startElement( "text:p" );
        bodyWriter.addTextNode( d->title->text() );
        bodyWriter.endElement(); // text:p
        bodyWriter.endElement(); // chart:title
    }
}

void ChartShape::saveOdfSubTitle( KoXmlWriter& bodyWriter,
                                  KoGenStyles& mainStyles ) const
{
    // Optional element
    if ( d->subTitle ) {
        bodyWriter.startElement( "chart:subtitle" );
        QRect rect( d->subTitle->geometry() );
        bodyWriter.addAttributePt( "svg:x", rect.x() );
        bodyWriter.addAttributePt( "svg:y", rect.y() );
        KDChart::TextAttributes ta = d->subTitle->textAttributes();
        bodyWriter.addAttribute( "chart:style-name", saveOdfFont( mainStyles, ta.font(), ta.pen().color() ) );
        bodyWriter.startElement( "text:p" );
        bodyWriter.addTextNode( d->subTitle->text() );
        bodyWriter.endElement(); // text:p
        bodyWriter.endElement(); // chart:subtitle
    }
}

void ChartShape::saveOdfFooter( KoXmlWriter& bodyWriter,
                                KoGenStyles& mainStyles ) const
{
    // Optional element
    if ( d->footer ) {
        bodyWriter.startElement( "chart:footer" );
        QRect rect( d->footer->geometry() );
        bodyWriter.addAttributePt( "svg:x", rect.x() );
        bodyWriter.addAttributePt( "svg:y", rect.y() );
        KDChart::TextAttributes ta = d->footer->textAttributes();
        bodyWriter.addAttribute( "chart:style-name", saveOdfFont( mainStyles, ta.font(), ta.pen().color() ) );
        bodyWriter.startElement( "text:p" );
        bodyWriter.addTextNode( d->footer->text() );
        bodyWriter.endElement(); // text:p
        bodyWriter.endElement(); // chart:footer
    }
}

void ChartShape::saveOdfLegend( KoXmlWriter &bodyWriter,
                                KoGenStyles& mainStyles ) const
{
    // Optional element
    if ( d->legend ) {
        bodyWriter.startElement( "chart:legend" );

        QString lp;
        QString lalign;
        switch ( d->legend->position().value() ) {
        case KDChartEnums::PositionNorthWest:
            lp = "top-start";
            break;
        case KDChartEnums::PositionNorth:
            lp = "top";
            lalign = "center";
            break;
        case KDChartEnums::PositionNorthEast:
            lp = "top-end";
            break;
        case KDChartEnums::PositionEast:
            lp = "bottom";
            lalign = "center";
            break;
        case KDChartEnums::PositionSouthEast:
            lp = "bottom-end";
            break;
        case KDChartEnums::PositionSouth:
            lp = "bottom";
            lalign = "center";
            break;
        case KDChartEnums::PositionSouthWest:
            lp = "bottom-start";
            break;
        case KDChartEnums::PositionWest:
            lp = "start";
            lalign = "center";
            break;

        case KDChartEnums::PositionCenter:
        case KDChartEnums::PositionFloating:
        case KDChartEnums::PositionUnknown:
            // TODO: Unhandled
            // There's no direct ODF equivalent for PositionCenter.
            // Absolute coordiantes could be used for Floating and Unknown
            break;
        }

        if ( !lp.isEmpty() ) {
            bodyWriter.addAttribute( "chart:legend-position", lp );
        }
        if ( !lalign.isEmpty() ) {
            bodyWriter.addAttribute( "chart:legend-align", lalign );
        }
            
        KDChart::TextAttributes ta = d->legend->titleTextAttributes();
        bodyWriter.addAttribute( "chart:style-name", saveOdfFont( mainStyles, ta.font(), ta.pen().color() ) );
        bodyWriter.addAttribute( "koffice:title", d->legend->titleText() );
        bodyWriter.endElement(); // chart:legend
    }
}

QString ChartShape::saveOdfFont( KoGenStyles& mainStyles, 
                                 const QFont& font,
                                 const QColor& color ) const
{
    KoGenStyle::PropertyType tt = KoGenStyle::TextType;
    KoGenStyle autoStyle( KoGenStyle::StyleAuto, "chart", 0 );
    autoStyle.addProperty( "fo:font-family", font.family(), tt );
    autoStyle.addPropertyPt( "fo:font-size", font.pointSize(), tt );
    autoStyle.addProperty( "fo:color", color.isValid() ? color.name() : "#000000", tt );
    int w = font.weight();
    autoStyle.addProperty( "fo:font-weight", w == 50 ? "normal" : w == 75 ? "bold" : QString::number( qRound(  w / 10 ) * 100 ), tt );
    autoStyle.addProperty( "fo:font-style", font.italic() ? "italic" : "normal", tt );

    return mainStyles.lookup( autoStyle, "ch", KoGenStyles::ForceNumbering );
}

void ChartShape::saveOdfPlotarea( KoXmlWriter& xmlWriter,
                                  KoGenStyles& mainStyles ) const
{
#if 0
    QString  dataSourceHasLabels;
    if ( m_firstRowIsLabel )
        if ( m_firstColIsLabel )
            dataSourceHasLabels = "both";
        else
            dataSourceHasLabels = "row";
    else
        if ( m_firstColIsLabel )
            dataSourceHasLabels = "column";
        else
            dataSourceHasLabels = "none";
    bodyWriter->addAttribute( "chart:data-source-has-labels", dataSourceHasLabels );
#endif
    // Prepare the style for the plot area
    KoGenStyle plotAreaStyle( KoGenStyle::StyleAuto, "chart" );

    // Save chart subtype
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
	if ( threeDMode() ) {
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
	if ( threeDMode() ) {
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
	if ( threeDMode() ) {
	    plotAreaStyle.addProperty( "chart:three-dimensional", "true" );
	    // FIXME: Save all 3D attributes too.
	}

    case CircleChartType:
        // FIXME
        break;

#if 0
	if ( threeDPies() )
	    plotAreaStyle.addProperty( "chart:three-dimensional", "true" );
#endif
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

#if 0
    // Data direction
    plotAreaStyle.addProperty( "chart:series-source",
			       ( dataDirection() == DataRows ) ? "rows" : "columns" );
#endif

    // Register the style, and get back its auto-generated name
    const QString  styleName = mainStyles.lookup( plotAreaStyle, "ch" );
    xmlWriter.addAttribute( "chart:style-name", styleName );

#if 0
    saveOasisAxis( xmlWriter, mainStyles, KDChartAxisParams::AxisPosBottom, "x" );
    saveOasisAxis( xmlWriter, mainStyles, KDChartAxisParams::AxisPosLeft, "y" );
#endif

    // TODO chart:series
    // TODO chart:wall
    // TODO chart:floor
}


#if 0
void KChartParams::saveOasisAxis( KoXmlWriter* bodyWriter, 
				  KoGenStyles& mainStyles,
                                  KDChartAxisParams::AxisPos axisPos, 
				  const char* axisName ) const
{
    bodyWriter->startElement( "chart:axis" );

    bodyWriter->addAttribute( "chart:dimension", axisName );
    bodyWriter->addAttribute( "chart:name", QByteArray( "primary-" ) + axisName );

    KoGenStyle axisStyle( KoGenStyle::StyleAuto, "chart" );

    // TODO: Save axis style properties, like
    axisStyle.addProperty( "chart:display-label", "true" ); // ###


    const QString styleName = mainStyles.lookup( axisStyle, "ch" );
    bodyWriter->addAttribute( "chart:style-name", styleName );

    // Write axis titles if any.
    QString tmpStr = axisTitle( axisPos );
    if ( !tmpStr.isEmpty() ) {
	bodyWriter->startElement( "chart:title" );
	// TODO: Save style, svg:x, svg:y

	// Write the text in the axis title.
	bodyWriter->startElement( "text:p" );
	bodyWriter->addTextNode( tmpStr
				 .remove( QRegExp( "^<qt><center>" ) )
				 .remove( QRegExp( "</center></qt>$" ) ) );
	bodyWriter->endElement(); // text:p

	bodyWriter->endElement(); // chart:title
    }

    // TODO x axis has chart:categories, y axis has chart:grid ?
    // Maybe that part should be done by the caller of saveOasisAxis then
    // including the opening/closing of the chart:axis element...

    bodyWriter->endElement(); // chart:axis
}


QString KChartParams::saveOasisFont( KoGenStyles& mainStyles, 
                                     const QFont& font,
                                     const QColor& color ) const
{
    KoGenStyle::PropertyType tt = KoGenStyle::TextType;
    KoGenStyle autoStyle( KoGenStyle::StyleAuto, "chart", 0 );
    autoStyle.addProperty( "fo:font-family", font.family(), tt );
    autoStyle.addPropertyPt( "fo:font-size", font.pointSize(), tt );
    autoStyle.addProperty( "fo:color", color.isValid() ? color.name() : "#000000", tt );
    int w = font.weight();
    autoStyle.addProperty( "fo:font-weight", w == 50 ? "normal" : w == 75 ? "bold" : QString::number( qRound(  w / 10 ) * 100 ), tt );
    autoStyle.addProperty( "fo:font-style", font.italic() ? "italic" : "normal", tt );

    return mainStyles.lookup( autoStyle, "ch", KoGenStyles::ForceNumbering );
}
#endif


void ChartShape::saveOdfData( KoXmlWriter& bodyWriter,
                              KoGenStyles& mainStyles ) const
{
    Q_UNUSED( mainStyles );

    const int cols = d->chartModel->columnCount();
    const int rows = d->chartModel->rowCount();

    bodyWriter.startElement( "table:table" );
    bodyWriter.addAttribute( "table:name", "local-table" );

    // Exactly one header column, always.
    bodyWriter.startElement( "table:table-header-columns" );
    bodyWriter.startElement( "table:table-column" );
    bodyWriter.endElement(); // table:table-column
    bodyWriter.endElement(); // table:table-header-columns

    // Then "cols" columns
    bodyWriter.startElement( "table:table-columns" );
    bodyWriter.startElement( "table:table-column" );
    bodyWriter.addAttribute( "table:number-columns-repeated", cols );
    bodyWriter.endElement(); // table:table-column
    bodyWriter.endElement(); // table:table-columns

    // Exactly one header row, always.
    bodyWriter.startElement( "table:table-header-rows" );
    bodyWriter.startElement( "table:table-row" );

    // The first column in header row is just the header column - no title needed
    bodyWriter.startElement( "table:table-cell" );
    bodyWriter.addAttribute( "office:value-type", "string" );
    bodyWriter.startElement( "text:p" );
    bodyWriter.endElement(); // text:p
    bodyWriter.endElement(); // table:table-cell

#if 0
    // Save column labels in the first header row, for instance:
    //          <table:table-cell office:value-type="string">
    //            <text:p>Column 1 </text:p>
    //          </table:table-cell>
    QStringList::const_iterator colLabelIt = m_colLabels.begin();
    for ( int col = 0; col < cols ; ++col ) {
        if ( colLabelIt != m_colLabels.end() ) {
            bodyWriter.startElement( "table:table-cell" );
            bodyWriter.addAttribute( "office:value-type", "string" );
            bodyWriter.startElement( "text:p" );
            bodyWriter.addTextNode( *colLabelIt );
            bodyWriter.endElement(); // text:p
            bodyWriter.endElement(); // table:table-cell
            ++colLabelIt;
        }
    }
#endif
    bodyWriter.endElement(); // table:table-row
    bodyWriter.endElement(); // table:table-header-rows

    bodyWriter.startElement( "table:table-rows" );
    //QStringList::const_iterator rowLabelIt = m_rowLabels.begin();
    for ( int row = 0; row < rows ; ++row ) {
        bodyWriter.startElement( "table:table-row" );
#if 0
        if ( rowLabelIt != m_rowLabels.end() ) {
            // Save row labels, similar to column labels
            bodyWriter.startElement( "table:table-cell" );
            bodyWriter.addAttribute( "office:value-type", "string" );

            bodyWriter.startElement( "text:p" );
            bodyWriter.addTextNode( *rowLabelIt );
            bodyWriter.endElement(); // text:p

            bodyWriter.endElement(); // table:table-cell
            ++rowLabelIt;
        }
#endif
        for ( int col = 0; col < cols; ++col ) {
            //QVariant value( d->chartModel.cellVal( row, col ) );
            QModelIndex  index = d->chartModel->index( row, col );
            QVariant     value = d->chartModel->data( index );

            QString  valType;
            QString  valStr;

            switch ( value.type() ) {
            case QVariant::Invalid:
		break;
            case QVariant::String:
		valType = "string";
		valStr  = value.toString();
		break;
            case QVariant::Double:
		valType = "float";
		valStr  = QString::number( value.toDouble(), 'g', DBL_DIG );
		break;
            case QVariant::DateTime:
		valType = "date";
		valStr  = ""; /* like in saveXML, but why? */
		break;
            default: {
                kDebug(35001) <<"ERROR: cell" << row <<"," << col
                               << " has unknown type." << endl;
                }
            }

	    // Add the value type and the string to the XML tree.
            bodyWriter.startElement( "table:table-cell" );
            if ( !valType.isEmpty() ) {
                bodyWriter.addAttribute( "office:value-type", valType );
                if ( value.type() == QVariant::Double )
                    bodyWriter.addAttribute( "office:value", valStr );

                bodyWriter.startElement( "text:p" );
                bodyWriter.addTextNode( valStr );
                bodyWriter.endElement(); // text:p
            }
	    bodyWriter.endElement(); // table:table-cell
        }
        bodyWriter.endElement(); // table:table-row
    }

    bodyWriter.endElement(); // table:table-rows
    bodyWriter.endElement(); // table:table
}


// ================================================================
//                         Private methods


void ChartShape::dataChanged( const QModelIndex &topLeft, const QModelIndex &bottomRight )
{
    d->diagram->dataChanged( topLeft, bottomRight );
    update();
}

void ChartShape::update()
{
    d->diagram->doItemsLayout();
    d->chart->update();
    d->pixmapRepaintRequested = true;
    KoShape::update();
}
