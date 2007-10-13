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

// KChart
#include "kchart_global.h"


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
    // The chart and its contents
    OdfChartType        chartType;
    OdfChartSubtype     chartSubtype;

    // The underlying engine
    KDChart::Chart            *chart;
    KDChart::AbstractDiagram  *diagram;

    // About the data
    bool                       firstRowAsLabel;
    bool                       firstColAsLabel;
    QAbstractItemModel        *chartData;

    // Default data to be used until the first call to setModel().
    // After that, it is never used again.
    QStandardItemModel         defaultData;
};


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
    d->chart->coordinatePlane()->replaceDiagram(d->diagram);
    setModel( &d->defaultData );

    d->firstRowAsLabel = false;
    d->firstColAsLabel = false;

    // Add axes to the diagram
    KDChart::AbstractCartesianDiagram  *diagram = static_cast<KDChart::AbstractCartesianDiagram*>(d->diagram);
    KDChart::CartesianAxis  *xAxis = new KDChart::CartesianAxis( diagram );
    KDChart::CartesianAxis  *yAxis = new KDChart::CartesianAxis( diagram );
    xAxis->setPosition( KDChart::CartesianAxis::Bottom );
    yAxis->setPosition( KDChart::CartesianAxis::Left );
    diagram->addAxis( xAxis );
    diagram->addAxis( yAxis );

    // Add a legend
    KDChart::Legend  *legend = new KDChart::Legend( d->diagram, d->chart );
    legend->setPosition( KDChart::Position::East );
    legend->setAlignment( Qt::AlignRight );
    legend->setShowLines( false );
    legend->setTitleText( i18n( "Legend" ) );
    legend->setOrientation( Qt::Vertical );
    d->chart->addLegend( legend );

    setChartDefaults();
    //createDefaultData();
#if 0
    // diagram->coordinatePlane returns an abstract plane one.
    // if we want to specify the orientation we need to cast
    // as follow
    CartesianCoordinatePlane* plane = static_cast <CartesianCoordinatePlane*>
		    ( d->diagram->coordinatePlane() );

    /* Configure grid steps and pen */
    // Vertical
    GridAttributes gv ( plane->gridAttributes( Qt::Vertical ) );

    // Configure a grid pen
    // I know it is horrible
    // just for demo'ing
    QPen gridPen(  Qt::gray );
    gridPen.setWidth( 0 );
    gv.setGridPen(  gridPen );

    // Configure a sub-grid pen
    QPen subGridPen( Qt::darkGray );
    subGridPen.setWidth( 0 );
    subGridPen.setStyle( Qt::DotLine );
    gv.setSubGridPen(  subGridPen );

    // Display a blue zero line
    gv.setZeroLinePen( QPen( Qt::blue ) );

    // change step and substep width
    // or any of those.
    gv.setGridStepWidth( 1.0 );
    gv.setGridSubStepWidth( 0.5 );
    gv.setGridVisible(  true );
    gv.setSubGridVisible( true );

    // Horizontal
    GridAttributes gh = plane->gridAttributes( Qt::Horizontal );
    gh.setGridPen( gridPen );
    gh.setGridStepWidth(  0.5 );
    gh.setSubGridPen(  subGridPen );
    gh.setGridSubStepWidth( 0.1 );

    plane->setGridAttributes( Qt::Vertical,  gv );
    plane->setGridAttributes( Qt::Horizontal,  gh );
#endif
}

ChartShape::~ChartShape()
{
    delete d->chartData;
    delete d->diagram;
    delete d->chart;
    delete d;
}

KDChart::Chart* ChartShape::chart() const
{
    return d->chart;
}


void ChartShape::setChartDefaults()
{
}

void ChartShape::setChartSubtype( OdfChartSubtype newSubtype )
{
    if( d->chartType != LineChartType && d->chartType != BarChartType && d->chartType != AreaChartType )
        return;

    switch ( newSubtype ) {
        case KChart::StackedChartSubtype:
            ( ( KDChart::LineDiagram* ) d->diagram )->setType( KDChart::LineDiagram::Stacked );
            break;
        case KChart::PercentChartSubtype:
            ( ( KDChart::LineDiagram* ) d->diagram )->setType( KDChart::LineDiagram::Percent );
            break;
        default:
            ( ( KDChart::LineDiagram* ) d->diagram )->setType( KDChart::LineDiagram::Normal );
    }
    d->chartSubtype = newSubtype;
    d->diagram->update();
    d->chart->update();
    repaint();
}

void ChartShape::setChartType( OdfChartType    newType,
                               OdfChartSubtype newSubtype )
{
    KDChart::AbstractDiagram           *new_diagram;
    KDChart::CartesianCoordinatePlane  *cartPlane = 0;
    KDChart::PolarCoordinatePlane      *polPlane  = 0;

    if (d->chartType == newType && d->chartSubtype == newSubtype)
        return;

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

	    // This is the way that KDChart represents an area chart:
	    //   a line chart with the displayArea attribute set to true.
	    {
		KDChart::LineAttributes attributes;
		attributes = ((KDChart::LineDiagram*) new_diagram)->lineAttributes();
		attributes.setDisplayArea( true );
		((KDChart::LineDiagram*) new_diagram)->setLineAttributes( attributes );
	    }
	    break;

	case CircleChartType:
	    new_diagram = new KDChart::PieDiagram(d->chart, polPlane);
	    break;

	case RingChartType:
	    new_diagram = new KDChart::RingDiagram(d->chart, polPlane);
	    break;

	case ScatterChartType:
	    // FIXME
	    return;
	    break;

	case RadarChartType:
	    new_diagram = new KDChart::PolarDiagram(d->chart, polPlane);
	    break;

	case StockChartType:
	    return;
	    break;

	case BubbleChartType:
	    // FIXME
	    return;
	    break;

	case SurfaceChartType:
	    // FIXME
	    return;
	    break;

	case GanttChartType:
	    // FIXME
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
                    qobject_cast<KDChart::AbstractCartesianDiagram*>( d->chart->coordinatePlane()->diagram() );
            Q_FOREACH( KDChart::CartesianAxis* axis, old->axes() ) {
                old->takeAxis( axis );
                qobject_cast<KDChart::AbstractCartesianDiagram*>(new_diagram)->addAxis( axis );
            }
        }

        //FIXME:No clue why this doesnt work but it makes a crash sometimes
//         LegendList legends = d->chart->legends();
//         Q_FOREACH(Legend* l, legends)
//             l->setDiagram( new_diagram );

        new_diagram->setModel( d->chartData );

        //FIXME:Aren't we leaking memory bot doing this?, 
        //although causes a crash
//         delete d->diagram;
        d->diagram = new_diagram;
        d->diagram->update();

        d->chart->coordinatePlane()->replaceDiagram( new_diagram ); // FIXME
        d->chart->update();
        //repaint(); Not needed

        // Update local data
        d->chartType = newType;
    }

    setChartSubtype( newSubtype );
}


void ChartShape::setModel( QAbstractItemModel* model )
{
    kDebug() << "BEGIN";
    d->chartData = model;
    d->chart->coordinatePlane()->takeDiagram( d->diagram );
    d->diagram->setModel( model );
    d->diagram->update();
    d->chart->coordinatePlane()->replaceDiagram( d->diagram );
    d->chart->update();

#if 0
    for ( int col = 0; col < d->diagram->model()->columnCount(); ++col ) {
        QPen pen(d->diagram->pen( col ));
        pen.setColor( Qt::black );
        pen.setWidth(4);
        d->diagram->setPen( iColumn, pen );
    }
#endif
/*
    KDChart::FrameAttributes faChart( d->chart->frameAttributes() );
    faChart.setPen( QPen(QColor(0x60,0x60,0xb0), 8) );
    d->chart->setFrameAttributes( faChart );

    BackgroundAttributes baChart( d->chart->backgroundAttributes() );
    baChart.setVisible( true );
    baChart.setBrush( QColor(0xd0,0xd0,0xff) );
    d->chart->setBackgroundAttributes( baChart );
*/
    kDebug() <<" END";
}

QAbstractItemModel *ChartShape::model()
{
    return d->chartData;
}

OdfChartType ChartShape::chartType() const
{
    return d->chartType;
}

OdfChartSubtype ChartShape::chartSubtype() const
{
    return d->chartSubtype;
}


void ChartShape::paint( QPainter& painter, const KoViewConverter& converter )
{
    const QRectF paintRect = QRectF( QPointF( 0.0, 0.0 ), size() );

    applyConversion( painter, converter );
    painter.setClipRect( paintRect, Qt::IntersectClip );

    //kDebug() << "Painting chart into " << paintRect << endl;
    // painting chart contents
    painter.fillRect(paintRect, KApplication::palette().base());
    d->chart->paint( &painter, paintRect.toRect() );
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


bool ChartShape::loadOdf( const KoXmlElement    &element, 
			  KoShapeLoadingContext &context )
{

//     if( element.hasAttributeNS( KoXmlNS::chart, "title" ) ) {
//         HeaderFooter Header;
//         KoXmlElement title = element.attributeNS( KoXmlNS::chart, "title" );
//         QPointF pos( KoUnit::parseValue( title.attributeNS( KoXmlNS::svg, "x", QString() ) ),
//                      KoUnit::parseValue( title.attributeNS( KoXmlNS::svg, "y", QString() ) ) );
//     }

    return false;
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

    // Save the data
    saveOdfData( bodyWriter, mainStyles );

    bodyWriter.endElement(); // chart:chart
}


void ChartShape::saveLegend( KoXmlWriter &bodyWriter,
			     KoGenStyles& mainStyles ) const
{
}


#if 0
void KChartParams::saveOasis( KoXmlWriter* bodyWriter, KoGenStyles& mainStyles ) const
{
    bool knownType = false;
    for ( unsigned int i = 0 ; i < numOasisChartTypes ; ++i ) {
        if ( m_chartType == oasisChartTypes[i].chartType ) {
            bodyWriter->addAttribute( "chart:class", oasisChartTypes[i].oasisClass );
            knownType = true;
            break;
        }
    }

    if ( !knownType ) {
        kError(32001) << "Unknown chart type in KChartParams::saveOasis, extend oasisChartTypes!" << endl;
    }

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
    QString dataSourceHasLabels;
    if ( m_firstRowAsLabel )
        if ( m_firstColAsLabel )
            dataSourceHasLabels = "both";
        else
            dataSourceHasLabels = "row";
    else
        if ( m_firstColAsLabel )
            dataSourceHasLabels = "column";
        else
            dataSourceHasLabels = "none";
    bodyWriter->addAttribute( "chart:data-source-has-labels", dataSourceHasLabels );
#endif
    // Prepare the style for the plot area
    KoGenStyle plotAreaStyle( KoGenStyle::StyleAuto, "chart" );

    switch ( d->chartType ) {
    case BarChartType:
        switch( d->chartSubtype ) {
        case StackedChartSubtype:
            plotAreaStyle.addProperty( "chart:stacked", "true" );
            break;
        case PercentChartSubtype:
            plotAreaStyle.addProperty( "chart:percentage", "true" );
            break;
        case NormalChartSubtype:
            break;
#if 0
	case BarMultiRows:
	    break;
#endif
        }
        plotAreaStyle.addProperty( "chart:vertical", "false" ); // #### always?
        plotAreaStyle.addProperty( "chart:lines-used", 0 ); // FIXME: for now

#if 0
	if ( threeDBars() )
	    plotAreaStyle.addProperty( "chart:three-dimensional", "true" );
    case Line:
        switch( lineChartSubType() ) {
        case LineStacked:
            plotAreaStyle.addProperty( "chart:stacked", "true" );
            break;
        case LinePercent:
            plotAreaStyle.addProperty( "chart:percentage", "true" );
            break;
        case LineNormal:
            break;
        }
        plotAreaStyle.addProperty( "chart:symbol-type", "automatic" );

	if ( threeDLines() )
	    plotAreaStyle.addProperty( "chart:three-dimensional", "true" );

        break;

    case Area:
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
        //plotAreaStyle.addProperty( "chart:lines-used", 0 ); // #### for now

        // TODO - very similar

    case Pie:
	if ( threeDPies() )
	    plotAreaStyle.addProperty( "chart:three-dimensional", "true" );

	break;

    case HiLo:
	break;

    case Ring:
	break;

    case Polar:
	break;

    case BoxWhisker:
	break;
#endif
    }

#if 0
    // chart:series-source
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


QString KChartParams::saveOasisFont( KoGenStyles& mainStyles, const QFont& font, const QColor& color ) const
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
    const int cols = d->chartData->columnCount();
    const int rows = d->chartData->rowCount();

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
            //QVariant value( d->chartData.cellVal( row, col ) );
            QModelIndex  index = d->chartData->index( row, col );
            QVariant     value = d->chartData->data( index );

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
