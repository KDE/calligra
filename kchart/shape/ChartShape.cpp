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

// KDChart
#include "KDChartAbstractCoordinatePlane.h"
#include "KDChartBackgroundAttributes.h"
#include "KDChartBarDiagram.h"
#include "KDChartLineDiagram.h"
#include "KDChartCartesianAxis.h"
#include "KDChartCartesianCoordinatePlane.h"
#include "KDChartChart.h"
#include "KDChartPieDiagram.h"
#include "KDChartPolarDiagram.h"
#include "KDChartRingDiagram.h"
#include "KDChartFrameAttributes.h"
#include "KDChartGridAttributes.h"
#include "KDChartLegend.h"
#include "KDChartHeaderFooter.h"
#include "KDChartLineAttributes.h"
#include "KDChartDataValueAttributes.h"
#include "KDChartMarkerAttributes.h"
#include "KDChartTextAttributes.h"
#include "KDChartAttributesModel.h"

#include "KDChartBarAttributes.h"
#include "KDChartThreeDBarAttributes.h"
#include "KDChartLineAttributes.h"
#include "KDChartThreeDLineAttributes.h"
#include "KDChartPieAttributes.h"
#include "KDChartThreeDPieAttributes.h"

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

class ChartShape::Private
{
public:
    Private();
    ~Private();

    // We can rerender faster if we cache KDChart's output
    QPixmap pixmap;
    QPointF lastZoomLevel;
    QSizeF  lastSize;
    bool    pixmapRepaintRequested;

    // The chart and its contents
    OdfChartType        chartType;
    OdfChartSubtype     chartSubtype;

    // Whether we're in 2D or 3D mode
    bool threeDMode;

    // The underlying engine
    KDChart::Chart            *chart;
    KDChart::AbstractDiagram  *diagram;
    KDChart::Legend           *legend;

    // About the data
    bool                       firstRowIsLabel;
    bool                       firstColIsLabel;

    // Proxy model that holds the actual QAbstractItemModel
    ChartProxyModel           *chartModel;
    QAbstractItemModel        *internalModel;
    QAbstractItemModel        *externalModel;
    bool                       takeOwnershipOfModel;
    bool                       useExternalDataSource;
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
    useExternalDataSource  = false;
    chartModel             = new ChartProxyModel;
    internalModel          = 0;
    externalModel          = 0;
    pixmapRepaintRequested = true;
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
    d->chart     = new KDChart::Chart();
    d->diagram   = new KDChart::BarDiagram();

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
    modelChanged();
}

void ChartShape::setFirstColumnIsLabel( bool b )
{
    d->chartModel->setFirstColumnIsLabel( b );
    modelChanged();
}

void ChartShape::setDataDirection( Qt::Orientation orientation )
{
    d->chartModel->setDataDirection( orientation );
    modelChanged();
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

void ChartShape::setUseExternalDatasource( bool b )
{
    if ( b ) {
        if ( d->externalModel != 0 ) {
            d-> chartModel->setSourceModel( d->externalModel );
            d->useExternalDataSource = true;
        }
    } else {
        if ( d->internalModel != 0 ) {
            d->chartModel->setSourceModel( d->internalModel );
            d->useExternalDataSource = false;
        }
    }
}

void ChartShape::modelChanged()
{
    // Tell the diagram that the entire set of data changed
    d->diagram->dataChanged( d->chartModel->index( 0, 0 ),
                             d->chartModel->index( d->chartModel->rowCount() - 1,
                                                  d->chartModel->columnCount() - 1 ) );
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
    modelChanged();
}

void ChartShape::setInternalModel( QAbstractItemModel *model )
{
    d->internalModel = model;
    d->chartModel->setSourceModel( model );
    modelChanged();
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
    // Get the current zoom level
    QPointF zoomLevel;
    converter.zoom( &zoomLevel.rx(), &zoomLevel.ry() );

    // Only repaint the pixmap if it is scheduled, the zoom level changed or the shape was resized
    if (    d->pixmapRepaintRequested
         || d->lastZoomLevel != zoomLevel
         || d->lastSize      != size() ) {
        refreshPixmap( painter, converter );
        d->lastZoomLevel = zoomLevel;
        d->lastSize      = size();
        d->pixmapRepaintRequested = false;
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
    { RingChartType,    "chart:circle"  },
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
    // FIXME

    // 3. Load the subtitle.
    // FIXME

    // 4. Load the footer.
    // FIXME

    // 5. Load the legend.
     //loadLegend( ... );

    // 6. Load the plot area (this is where the real action is!).

    // 7. Load the data
    //loadOdfData(  );

#if 0  // Taken from old kchart_params.cpp: Use what we can from here
       // and throw away the rest.

    // Title TODO (more details, e.g. font, placement etc)
    KoXmlElement titleElem = KoXml::namedItemNS( chartElem,
						 KoXmlNS::chart, "title" );
    if ( !titleElem.isNull() ) {
        loadingContext.styleStack().save();
        loadingContext.fillStyleStack( titleElem, KoXmlNS::chart, "style-name", "chart" );
        QFont font;
        QColor color;
        loadOasisFont( loadingContext, font, color );
        setHeaderFooterFont( KDChartParams::HdFtPosHeader, font, true, font.pointSize() );
        setHeaderFooterColor( KDChartParams::HdFtPosHeader, color );
        loadingContext.styleStack().restore();

	KoXmlElement  pElem = KoXml::namedItemNS( titleElem,
						 KoXmlNS::text, "p" );
	setHeader1Text( pElem.text() );
    }

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
    if ( !legendElem.isNull() )
    {
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

    // Get the plot-area.  This is where the action is.
    KoXmlElement  plotareaElem = KoXml::namedItemNS( chartElem,
						    KoXmlNS::chart, "plot-area" );
    if ( !plotareaElem.isNull() ) {
	return loadOasisPlotarea( plotareaElem, loadingContext, errorMessage );
    }

    return false;
#endif

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
    // FIXME

    // 3. Write the subtitle.
    // FIXME

    // 4. Write the footer.
    // FIXME

    // 5. Write the legend.
    saveLegend( bodyWriter, mainStyles );

    // 6. Write the plot area (this is where the real action is!).
    bodyWriter.startElement( "chart:plot-area" );
    saveOdfPlotArea( bodyWriter, mainStyles );
    bodyWriter.endElement();

    // 7. Save the data
    saveOdfData( bodyWriter, mainStyles );

    bodyWriter.endElement(); // chart:chart
}


void ChartShape::saveLegend( KoXmlWriter &bodyWriter,
			     KoGenStyles& mainStyles ) const
{
}


#if 0
void KChartParams::saveOasis( KoXmlWriter* bodyWriter, 
                              KoGenStyles& mainStyles ) const
{
    bodyWriter->startElement( "chart:title" );
    QRect rect( headerFooterRect( KDChartParams::HdFtPosHeader ) );
    bodyWriter->addAttributePt( "svg:x", rect.x() );
    bodyWriter->addAttributePt( "svg:y", rect.y() );
    bodyWriter->addAttribute( "chart:style-name", saveOasisFont( mainStyles, header1Font(), headerFooterColor( KDChartParams::HdFtPosHeader ) ) );
    bodyWriter->startElement( "text:p" );
    bodyWriter->addTextNode( header1Text() );
    bodyWriter->endElement(); // text:p
    bodyWriter->endElement(); // chart:title

    QString subTitle( header2Text() );
    if ( !subTitle.isEmpty() ) {

        kDebug(32001) <<"header rect:" << headerFooterRect( KDChartParams::HdFtPosHeader2 );
        QRect rect( headerFooterRect( KDChartParams::HdFtPosHeader2 ) );
        bodyWriter->startElement( "chart:subtitle" );
        bodyWriter->addAttributePt( "svg:x", rect.x() );
        bodyWriter->addAttributePt( "svg:y", rect.y() );
        bodyWriter->addAttribute( "chart:style-name", 
				  saveOasisFont( mainStyles, 
						 header2Font(), 
						 headerFooterColor( KDChartParams::HdFtPosHeader2 ) ) );

        bodyWriter->startElement( "text:p" );
        bodyWriter->addTextNode( subTitle );
        bodyWriter->endElement(); // text:p
        bodyWriter->endElement(); // chart:subtitle
    }


    QString footer( footerText() );
    if ( !footer.isEmpty() ) {
        QRect rect( headerFooterRect( KDChartParams::HdFtPosFooter ) );
        bodyWriter->startElement( "chart:footer" );
        bodyWriter->addAttributePt( "svg:x", rect.x() );
        bodyWriter->addAttributePt( "svg:y", rect.y() );
        bodyWriter->addAttribute( "chart:style-name",
				  saveOasisFont( mainStyles, 
						 footerFont(), 
						 headerFooterColor( KDChartParams::HdFtPosFooter ) ) );

        bodyWriter->startElement( "text:p" );
        bodyWriter->addTextNode( footer );
        bodyWriter->endElement(); // text:p
        bodyWriter->endElement(); // chart:footer
    }

    // TODO legend
    LegendPosition lpos = legendPosition();
    if ( lpos != NoLegend ) {
        bodyWriter->startElement( "chart:legend" );
        QString lp;
        QString lalign;
        switch ( lpos ) {
	case LegendTop: 
	    lp = "top";
	    lalign = "center";
	    break;
	case LegendBottom:
	    lp = "bottom";
	    lalign = "center";
	    break;
	case LegendLeft: 
	    lp = "start";
	    lalign = "center";
	    break;
	case LegendRight:
	    lp = "end";
	    lalign = "center";
	    break;
	case LegendTopLeft:
	    lp = "top-start";
	    break;
	case LegendTopLeftTop:
	    lp = "top";
	    lalign = "start";
	    break;
	case LegendTopLeftLeft:
	    lp = "start";
	    lalign = "start";
	    break;
	case LegendTopRight:
	    lp = "top-end";
	    break;
	case LegendTopRightTop:
	    lp = "top";
	    lalign = "end";
	    break;
	case LegendTopRightRight:
	    lp = "end";
	    lalign = "start";
	    break;
	case LegendBottomLeft:
	    lp = "bottom-start";
	    break;
	case LegendBottomLeftBottom:
	    lp = "bottom";
	    lalign = "start";
	    break;
	case LegendBottomLeftLeft:
	    lp = "start";
	    lalign = "end";
	    break;
	case LegendBottomRight:
	    lp = "bottom-end";
	    break;
	case LegendBottomRightBottom:
	    lp = "bottom";
	    lalign = "end";
	    break;
	case LegendBottomRightRight:
	    lp = "end";
	    lalign = "end";
	    break;
	default:
	    lp = "end";
	    lalign = "center";
	    break;
        }

        bodyWriter->addAttribute( "chart:legend-position", lp );
        bodyWriter->addAttribute( "chart:legend-align", lalign );
        bodyWriter->addAttribute( "chart:style-name",
				  saveOasisFont( mainStyles, 
						 legendFont(), 
						 legendTextColor() ) );
        bodyWriter->addAttribute( "koffice:title", legendTitleText() );
        bodyWriter->endElement(); // chart:legend
    }

    bodyWriter->startElement( "chart:plot-area" );
    saveOasisPlotArea( bodyWriter, mainStyles );
    bodyWriter->endElement();
}
#endif

void ChartShape::saveOdfPlotArea( KoXmlWriter& xmlWriter,
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
        plotAreaStyle.addProperty( "chart:vertical", "false" ); // FIXME
        plotAreaStyle.addProperty( "chart:lines-used", 0 ); // FIXME: for now

#if 0
	if ( threeDBars() )
	    plotAreaStyle.addProperty( "chart:three-dimensional", "true" );
        break;
#endif

    case LineChartType:
        // FIXME
        break;

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
        // FIXME: What does this mean?
        plotAreaStyle.addProperty( "chart:symbol-type", "automatic" );

#if 0
	if ( threeDLines() )
	    plotAreaStyle.addProperty( "chart:three-dimensional", "true" );
#endif

        break;

    case AreaChartType:
        // FIXME
        break;

#if 0
        switch( areaChartSubType() ) {
        case AreaStacked:
            plotAreaStyle.addProperty( "chart:stacked", "true" );
            break;
        case AreaPercent:
            plotAreaStyle.addProperty( "chart:percentage", "true" );
            break;
        case AreaNormal:
            break;
        }
#endif
        //plotAreaStyle.addProperty( "chart:lines-used", 0 ); // #### for now


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


#if 0
void ChartShape::initNullChart()
{
} 
#endif


void ChartShape::update()
{
    d->diagram->doItemsLayout();
    d->legend->update();
    d->diagram->update();
    d->chart->update();
    d->pixmapRepaintRequested = true;
    KoShape::update();
}
