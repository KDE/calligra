/* This file is part of the KDE project

   Copyright 1999-2007  Kalle Dalheimer <kalle@kde.org>
   Copyright 2005-2007  Inge Wallin <inge@lysator.liu.se>
   Copyright 2007 Thorsten Zachmann <zachmann@kde.org>

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
#include "KChartPart.h"

// Posix
#include <float.h> // For basic data types characteristics.

// C++, for debugging
#include <iostream>

using std::cout;
using std::cerr;

// Qt
#include <QList>
#include <QStandardItemModel>
#include <qdom.h>
#include <qtextstream.h>
#include <qbuffer.h>
#include <qpainter.h>

// KDE
#include <klocale.h>
#include <kstandarddirs.h>
#include <kglobal.h>
#include <kdebug.h>
#include <ktemporaryfile.h>

// KOffice
#include <KoZoomHandler.h>
#include <KoXmlNS.h>
#include <KoXmlWriter.h>
#include <KoXmlReader.h>
#include <KoShape.h>
#include <KoShapeCreateCommand.h>
#include <KoShapeManager.h>
#include <KoStore.h>
#include <KoStoreDevice.h>
#include <KoShapeSavingContext.h>
#include <KoToolManager.h>
#include <KoOdfLoadingContext.h>
#include <KoOdfStylesReader.h>
#include <KoOdfReadStore.h>
#include <KoOdfWriteStore.h>
#include <KoShapeLoadingContext.h>
#include <KoShapeRegistry.h>

// KDChart
#include "KDChartChart"
#include "KDChartAbstractDiagram" // Base class for the diagrams
#include "KDChartAbstractCoordinatePlane"
#include "KDChartBarDiagram"

// KChart
#include "KChartCanvas.h"
#include "KChartView.h"
#include "KChartFactory.h"
#include "ChartShape.h"
#include "ChartShapeFactory.h"

#include "prefs.h"


using namespace std;



namespace KChart
{

KChartPart::KChartPart( QWidget *parentWidget,
			QObject* parent,
			bool singleViewMode )
  : KoChart::Part( parentWidget, parent, singleViewMode ),
    m_chartData( 0 )
{
    // A KChart part contains a single chart Flake shape. 
    m_chartShape = (ChartShape*)(KoShapeRegistry::instance()->value("ChartShape") )->createDefaultShape(resourceManager());
    setComponentData( KChartFactory::global(), false );
    setTemplateType( "kchart_template" );

    // Init some members that need it.
    m_bCanChangeValue = true;

    // Set the size to minimal.
    initEmpty();
}


KChartPart::~KChartPart()
{
    delete m_chartData;
    //delete m_chart;
}


void KChartPart::initEmpty()
{
    initNullChart();

    resetURL();
    setEmpty();
}


// This method creates the simplest chart imaginable:
// Data size 1x1, empty, no headers
//
void KChartPart::initNullChart()
{
    // Fill cells with data if there is none.
    //kDebug(35001) <<"Initialize null chart.";

#if 0
    m_currentData->setDataHasVerticalHeaders( false );
    m_currentData->setDataHasHorizontalHeaders( false );
#endif

    setChartDefaults();

#if 0
    m_params->setDrawSolidExcessArrows(true);
#endif
}


KoView* KChartPart::createViewInstance( QWidget* parent )
{
    return new KChartView( this, parent );
}


// ================================================================
//                              Painting


void KChartPart::paintContent( QPainter& painter, const QRect& rect)
{
    Q_UNUSED(painter);
    Q_UNUSED(rect);
#if 0
    int  numDatasets;

    // If params is 0, initDoc() has not been called.
    Q_ASSERT( m_params != 0 );

    KDChartAxisParams  xAxisParms;
    xAxisParms = m_params->axisParams( KDChartAxisParams::AxisPosBottom );

    // Handle data in rows or columns.
    //
    // This means getting row or column headers from the document and
    // set them as X axis labels or legend according to the current
    // setting.  Also, transpose the data if it should be displayed in
    // columns instead of in rows.


    // Create the displayData table.
    numDatasets = createDisplayData();

    // Create and set the axis labels and legend.
    QStringList  longLabels;
    QStringList  shortLabels;
    createLabelsAndLegend(longLabels, shortLabels);

    // Set the x axis labels.
    xAxisParms.setAxisLabelStringLists( &longLabels, &shortLabels );
    m_params->setAxisParams(KDChartAxisParams::AxisPosBottom, xAxisParms);

    // Handle some types or subtypes of charts specially, notably:
    //  - Bar charts with lines in them

    if ( chartType() == BarChartType ) {
	if ( m_params->barNumLines() > 0 ) {

	    // If this is a bar chart and the user wants a few lines in
	    // it, we need to create an additional chart in the same
	    // drawing area.

	    // Specify that we want to have an additional chart.
	    m_params->setAdditionalChartType( KDChartParams::Line );

	    const int numBarDatasets = numDatasets - m_params->barNumLines();

	    // Assign the datasets to the charts: DataEntry, from, to, chart#
	    m_params->setChartSourceMode( KDChartParams::DataEntry,
					  0, numBarDatasets - 1,
					  0 ); // The bar chart
	    m_params->setChartSourceMode( KDChartParams::DataEntry,
					  numBarDatasets, numDatasets - 1,
					  1 ); // The line chart
	}
	else {
	    // Otherwise we don't want any extra chart.
	    m_params->setAdditionalChartType( KDChartParams::NoType );
	}
    }

    // Ok, we have now created a data set for display, and params with
    // suitable legends and axis labels.  Now start the real painting.

    // ## TODO: support zooming

    // We only need to draw the document rectangle "rect".
    KDChart::paint( &painter, m_params, &m_displayData, 0, &rect );

#else
    // Paint the shape that is the real chart.
    kDebug(35001) << "################## Painting!";
    //KoZoomHandler  zoomHandler;
    //m_chartShape->paint( painter, zoomHandler );
    return;

    // Ok, we have now created a data set for display, and params with
    // suitable legends and axis labels.  Now start the real painting.

    // Make the chart use our model.
    //kDebug(35001) <<"Painting!!";

   // ## TODO: support zooming

    // We only need to draw the document rectangle "rect".
    //m_chart->paint( &painter, rect );
#endif
}


// ================================================================


// This function sets the data from an external source.  It is called,
// for instance, when the chart is initialized from a spreadsheet in
// KSpread.
//
void KChartPart::analyzeHeaders( const QStandardItemModel &data )
{
    Q_UNUSED( data );
#if 0
    // FIXME(khz): replace this when automatic string detection works in KDChart
    // Does the top/left cell contain a string?
    bool isStringTopLeft = (data.cellVal( 0, 0 ).type() == QVariant::String);

    // Does the first row (without first cell) contain only strings?
    bool isStringFirstRow = true;
    for ( uint col = 1; isStringFirstRow && col < data.cols(); col++ ) {
        isStringFirstRow = (data.cellVal( 0, col ).type() == QVariant::String);
    }

    // Just in case, we only have 1 row, we never use it for label text.
    // This prevents a crash.
    //
    // FIXME: Wonder if this is still true for KDChart 1.1.3 / iw
    //        Disabling...
#if 1
    if ( data.rows() == 1 )
        isStringFirstRow = false;
#endif

    // Does the first column (without first cell) contain only strings?
    bool isStringFirstCol = true;
    for ( uint row = 1; isStringFirstCol && row < data.rows(); row++ ) {
        isStringFirstCol = (data.cellVal( row, 0 ).type() == QVariant::String);
    }

    // Just in case, we only have 1 column, we never use it for axis
    // label text => prevents crash.
#if 1
    if ( data.cols() == 1 )
        isStringFirstRow = false;
#endif

    bool hasColHeader = false;
    bool hasRowHeader = false;

    // Let's check if we have a full axis label text column
    if ( isStringFirstCol && isStringTopLeft
	 || isStringFirstCol && isStringFirstRow )
        hasColHeader = true;

    // Let's check if we have a full label text row.
    if ( isStringFirstRow && isStringTopLeft
	 || isStringFirstCol && isStringFirstRow )
        hasRowHeader = true;

    setFirstRowAsLabel( hasRowHeader );
    setFirstColAsLabel( hasColHeader );

    doSetData(data, hasRowHeader, hasColHeader);
#endif
}


void KChartPart::addShape( KoShape *shape )
{
    foreach( KoView* view, views() ) {
        KChartCanvas  *canvas = ( ( KChartView* )view )->canvasWidget();
        canvas->shapeManager()->addShape( shape );
    }
}

void KChartPart::removeShape( KoShape* )
{
}

// ----------------------------------------------------------------


// Set up some values for the chart Axis, that are not well chosen by
// default by KDChart.
//

void KChartPart::setChartDefaults()
{
#if 0
  //
  // Settings for the Y axis.
  //
  KDChartAxisParams  yAxis;
  yAxis = m_params->axisParams( KDChartAxisParams::AxisPosLeft );

  // decimal symbol and thousands separator
  yAxis.setAxisLabelsRadix( KGlobal::locale()->decimalSymbol(),
			    KGlobal::locale()->thousandsSeparator() );

  m_params->setAxisParams( KDChartAxisParams::AxisPosLeft, yAxis );

  //
  // Settings for the X axis.
  //
  KDChartAxisParams  xAxis;
  xAxis = m_params->axisParams( KDChartAxisParams::AxisPosBottom );

  // These two shouldn't be necessary to set.
  xAxis.setAxisFirstLabelText();
  xAxis.setAxisLastLabelText();

  m_params->setAxisParams( KDChartAxisParams::AxisPosBottom, xAxis );

  // Other parameters for various things.
  m_params->setLineColor();

  // Setting the background layer.
  KDFrame frame;
  frame.setBackground( QBrush( QColor( 230, 222, 222 ) ) );
  m_params->setFrame( KDChartEnums::AreaInnermost, frame, 0, 0, 0, 0 );
#endif
}


// ================================================================
//                     Loading and Storing


// ----------------------------------------------------------------
//                 Save and Load program configuration



void KChartPart::loadConfig( KConfig *config )
{
    Q_UNUSED( config );
#if 0
    KConfigGroup conf = config->group("ChartParameters");

    // TODO: the fonts
    // PENDING(kalle) Put the applicable ones of these back in
    //   QFont tempfont;
    //   tempfont = conf.readFontEntry("titlefont", &titlefont);
    //   setTitleFont(tempfont);
    //   tempfont = conf.readFontEntry("ytitlefont", &ytitlefont);
    //   setYTitleFont(tempfont);
    //   tempfont = conf.readFontEntry("xtitlefont", &xtitlefont);
    //   setXTitleFont(tempfont);
    //   tempfont = conf.readFontEntry("yaxisfont", &yaxisfont);
    //   setYAxisFont(tempfont);
    //   tempfont = conf.readFontEntry("xaxisfont", &xaxisfont);
    //   setXAxisFont(tempfont);
    //   tempfont = conf.readFontEntry("labelfont", &labelfont);
    //   setLabelFont(tempfont);
    //   tempfont = conf.readFontEntry("annotationfont", &annotationfont);
    //   setAnnotationFont(tempfont);

    //   ylabel_fmt = conf.readEntry("ylabel_fmt", ylabel_fmt );
    //   ylabel2_fmt = conf.readEntry("ylabel2_fmt", ylabel2_fmt);
    //   xlabel_spacing = conf.readEntry("xlabel_spacing");
    //   ylabel_density = conf.readEntry("ylabel_density", ylabel_density);
    //   requested_ymin = conf.readEntry("requested_ymin", requested_ymin);
    //   requested_ymax = conf.readEntry("requested_ymax", requested_ymax );
    //   requested_yinterval = conf.readEntry("requested_yinterval",
    // 					   requested_yinterval);
    //   shelf = conf.readEntry("shelf", shelf);
    //   grid = conf.readEntry("grid", grid);
    //   xaxis = conf.readEntry("xaxis", xaxis);
    //   yaxis = conf.readEntry("yaxis", yaxis);
    //   yaxis2 = conf.readEntry("yaxis2", yaxis);
    //   llabel = conf.readEntry("llabel", llabel);
    //   yval_style = conf.readEntry("yval_style", yval_style);
    //   stack_type = (KChartStackType)conf.readEntry("stack_type", stack_type);
    m_params->setLineMarker(conf.readEntry("lineMarker",
						m_params->lineMarker()));
    m_params->setThreeDBarDepth( conf.readEntry("_3d_depth",
							  m_params->threeDBarDepth() ) );
    m_params->setThreeDBarAngle( conf.readEntry( "_3d_angle",
						     m_params->threeDBarAngle() ) );

    KDChartAxisParams  leftparams;
    leftparams = m_params->axisParams( KDChartAxisParams::AxisPosLeft );
    KDChartAxisParams  rightparams;
    rightparams = m_params->axisParams( KDChartAxisParams::AxisPosRight );
    KDChartAxisParams  bottomparams;
    bottomparams = m_params->axisParams( KDChartAxisParams::AxisPosBottom );

    bottomparams.setAxisLineColor( conf.readEntry( "XTitleColor", QColor(Qt::black) ) );
    leftparams.setAxisLineColor( conf.readEntry( "YTitleColor", QColor(Qt::black) ) );
    rightparams.setAxisLineColor( conf.readEntry( "YTitle2Color", QColor(Qt::black) ) );
    bottomparams.setAxisLabelsColor( conf.readEntry( "XLabelColor", QColor(Qt::black) ) );
    leftparams.setAxisLabelsColor( conf.readEntry( "YLabelColor", QColor(Qt::black) ) );
    rightparams.setAxisLabelsColor( conf.readEntry( "YLabel2Color", QColor(Qt::black) ) );
    leftparams.setAxisGridColor( conf.readEntry( "GridColor", QColor(Qt::black) ) );
    m_params->setOutlineDataColor( conf.readEntry( "LineColor", QColor(Qt::black) ) );
    m_params->setAxisParams( KDChartAxisParams::AxisPosLeft,
                            leftparams );
    m_params->setAxisParams( KDChartAxisParams::AxisPosRight,
                            rightparams );
    m_params->setAxisParams( KDChartAxisParams::AxisPosBottom,
                            bottomparams );

    //   hlc_style = (KChartHLCStyle)conf.readEntry("hlc_style", hlc_style);
    //   hlc_cap_width = conf.readEntry("hlc_cap_width", hlc_cap_width);
    //   // TODO: Annotation font
    //   num_scatter_pts = conf.readEntry("num_scatter_pts", num_scatter_pts);
    //   // TODO: Scatter type
    //   thumbnail = conf.readEntry("thumbnail", thumbnail);
    //   thumblabel = conf.readEntry("thumblabel", thumblabel);
    //   border = conf.readEntry("border", border);
    //   BGColor = conf.readColorEntry("BGColor", &BGColor);
    //   PlotColor = conf.readColorEntry("PlotColor", &PlotColor);
    //   VolColor = conf.readColorEntry("VolColor", &VolColor);
    //   EdgeColor = conf.readColorEntry("EdgeColor", &EdgeColor);
    //   loadColorArray(conf, &SetColor, "SetColor");
    //   loadColorArray(conf, &ExtColor, "ExtColor");
    //   loadColorArray(conf, &ExtVolColor, "ExtVolColor");
    //   transparent_bg = conf.readEntry("transparent_bg", transparent_bg);
    //   // TODO: explode, missing
    //   percent_labels = (KChartPercentType)conf.readEntry("percent_labels",
    // 							 percent_labels);
    //   label_dist = conf.readEntry("label_dist", label_dist);
    //   label_line = conf.readEntry("label_line", label_line);
    setChartType( (ChartType)conf.readEntry( "type", int(chartType() )) );
    //   other_threshold = conf.readEntry("other_threshold", other_threshold);

    //   backgroundPixmapName = conf.readPathEntry( "backgroundPixmapName", QString() );
    //   if( !backgroundPixmapName.isNull() ) {
    //     backgroundPixmap.load( KStandardDirs::locate( "wallpaper", backgroundPixmapName ));
    //     backgroundPixmapIsDirty = true;
    //   } else
    //     backgroundPixmapIsDirty = false;
    //   backgroundPixmapScaled = conf.readEntry( "backgroundPixmapScaled", true );
    //   backgroundPixmapCentered = conf.readEntry( "backgroundPixmapCentered", true );
    //   backgroundPixmapIntensity = conf.readEntry( "backgroundPixmapIntensity", 0.25 );
#endif
}


void KChartPart::defaultConfig(  )
{
    //FIXME
#if 0
    delete m_params;
    m_params = new KChartParams( this );
#endif
    setChartDefaults();
}


void KChartPart::saveConfig( KConfig *config )
{
    Q_UNUSED( config );
#if 0
    KConfigGroup conf = config->group("ChartParameters");

    // PENDING(kalle) Put some of these back in
    // the fonts
    //   conf.writeEntry("titlefont", titlefont);
    //   conf.writeEntry("ytitlefont", ytitlefont);
    //   conf.writeEntry("xtitlefont", xtitlefont);
    //   conf.writeEntry("yaxisfont", yaxisfont);
    //   conf.writeEntry("xaxisfont", xaxisfont);
    //   conf.writeEntry("labelfont", labelfont);

    //   conf.writeEntry("ylabel_fmt", ylabel_fmt);
    //   conf.writeEntry("ylabel2_fmt", ylabel2_fmt);
    //   conf.writeEntry("xlabel_spacing", xlabel_spacing);
    //   conf.writeEntry("ylabel_density", ylabel_density);
    //   conf.writeEntry("requested_ymin", requested_ymin);
    //   conf.writeEntry("requested_ymax", requested_ymax);
    //   conf.writeEntry("requested_yinterval", requested_yinterval);

    //   conf.writeEntry("shelf", shelf);
    //   conf.writeEntry("grid", grid );
    //   conf.writeEntry("xaxis", xaxis);
    //   conf.writeEntry("yaxis", yaxis);
    //   conf.writeEntry("yaxis2", yaxis2);
    //   conf.writeEntry("llabel", llabel);
    //   conf.writeEntry("yval_style", yval_style );
    //   conf.writeEntry("stack_type", (int)stack_type);

    conf.writeEntry( "_3d_depth", m_params->threeDBarDepth() );
    conf.writeEntry( "_3d_angle", m_params->threeDBarAngle() );

    KDChartAxisParams leftparams;
    leftparams   = m_params->axisParams( KDChartAxisParams::AxisPosLeft );
    KDChartAxisParams rightparams;
    rightparams  = m_params->axisParams( KDChartAxisParams::AxisPosRight );
    KDChartAxisParams bottomparams;
    bottomparams = m_params->axisParams( KDChartAxisParams::AxisPosBottom );
    conf.writeEntry( "LineColor",    m_params->outlineDataColor() );
    conf.writeEntry( "XTitleColor",  bottomparams.axisLineColor() );
    conf.writeEntry( "YTitleColor",  leftparams.axisLineColor() );
    conf.writeEntry( "YTitle2Color", rightparams.axisLineColor() );
    conf.writeEntry( "XLabelColor",  bottomparams.axisLabelsColor() );
    conf.writeEntry( "YLabelColor",  leftparams.axisLabelsColor() );
    conf.writeEntry( "YLabel2Color", rightparams.axisLabelsColor() );
    conf.writeEntry( "GridColor",    leftparams.axisGridColor() );

    //   conf.writeEntry("hlc_style", (int)hlc_style);
    //   conf.writeEntry("hlc_cap_width", hlc_cap_width );
    //   // TODO: Annotation type!!!
    //   conf.writeEntry("annotationfont", annotationfont);
    //   conf.writeEntry("num_scatter_pts", num_scatter_pts);
    //   // TODO: Scatter type!!!
    //   conf.writeEntry("thumbnail", thumbnail);
    //   conf.writeEntry("thumblabel", thumblabel);
    //   conf.writeEntry("thumbval", thumbval);
    //   conf.writeEntry("border", border);
    //   conf.writeEntry("BGColor", BGColor);
    //   conf.writeEntry("PlotColor", PlotColor);
    //   conf.writeEntry("VolColor", VolColor);
    //   conf.writeEntry("EdgeColor", EdgeColor);
    //   saveColorArray(conf, &SetColor, "SetColor");
    //   saveColorArray(conf, &ExtColor, "ExtColor");
    //   saveColorArray(conf, &ExtVolColor, "ExtVolColor");


    //   conf.writeEntry("transparent_bg", transparent_bg);
    //   // TODO: explode, missing
    //   conf.writeEntry("percent_labels",(int) percent_labels );
    //   conf.writeEntry("label_dist", label_dist);
    //   conf.writeEntry("label_line", label_line);
    conf.writeEntry( "type", (int) m_params->chartType() );
    //   conf.writeEntry("other_threshold", other_threshold);

    // background pixmap stuff
    //   if( !backgroundPixmapName.isNull() )
    // 	conf.writePathEntry( "backgroundPixmapName", backgroundPixmapName );
    //   conf.writeEntry( "backgroundPixmapIsDirty", backgroundPixmapIsDirty );
    //   conf.writeEntry( "backgroundPixmapScaled", backgroundPixmapScaled );
    //   conf.writeEntry( "backgroundPixmapCentered", backgroundPixmapCentered );
    //   conf.writeEntry( "backgroundPixmapIntensity", backgroundPixmapIntensity );
    conf.writeEntry( "lineMarker", (int) m_params->lineMarker());
#endif
}


// ----------------------------------------------------------------
//              Save and Load OpenDocument file format


bool KChartPart::loadOdf( KoOdfReadStore & odfStore )
{
    KoXmlElement  content = odfStore.contentDoc().documentElement();

    // Find office:body
    KoXmlElement  realBody ( KoXml::namedItemNS( content, 
                                                 KoXmlNS::office, "body" ) );
    if ( realBody.isNull() ) {
        setErrorMessage( i18n( "Invalid OpenDocument file. No office:body tag found." ));
        return false;
    }

    // Find office:chart
    KoXmlElement bodyElement = KoXml::namedItemNS( realBody,
                                                   KoXmlNS::office, "chart" );
    if ( bodyElement.isNull() ) {
        kError(32001) << "No office:chart found!";

        KoXmlElement  childElem;
        QString       localName;
        forEachElement( childElem, realBody ) {
            localName = childElem.localName();
        }
        if ( localName.isEmpty() )
            setErrorMessage( i18n( "Invalid OpenDocument file. No tag found inside office:body." ) );
        else
            setErrorMessage( i18n( "This document is not a chart, but %1. Please try opening it with the appropriate application.",
                                   KoDocument::tagNameToDocumentType( localName ) ) );
        return false;
    }

    // Find chart:chart
    KoXmlElement chartElement = KoXml::namedItemNS( bodyElement, 
                                                    KoXmlNS::chart, "chart" );
    if ( chartElement.isNull() ) {
        setErrorMessage( i18n( "No chart found in the file" ) );
        return false;
    }

    // Load styles first
#if 0
    loadAllStyles( context );
    styleManager()->loadOasisStyleTemplate( stylesReader, this );
#endif

    KoOdfLoadingContext  context( odfStore.styles(), odfStore.store() );
    KoShapeLoadingContext shapeContext(context, resourceManager());

    // ----------------------------------------------------------------
    // FIXME: Load chart here.
    // ----------------------------------------------------------------
    if ( !m_chartShape->loadOdf( chartElement, shapeContext ) )
        return false;

#if 0     // Load settings later.
    if ( !odfStore.settingsDoc().isNull() ) {
        loadOasisSettings( odfStore.settingsDoc() );
    }
#endif

    return true;

    // ----------------------------------------------------------------

#if 0
    kDebug(35001) <<"kchart loadOasis called";

    // Set some sensible defaults.
    setChartDefaults();

    KoXmlElement  content = doc.documentElement();
    KoXmlElement  bodyElem ( KoXml::namedItemNS( content,
						KoXmlNS::office, "body" ) );
    if ( bodyElem.isNull() ) {
        kError(32001) << "No office:body found!";
        setErrorMessage( i18n( "Invalid OASIS OpenDocument file. No office:body tag found." ) );
        return false;
    }

    // Get the office:chart element.
    KoXmlElement officeChartElem = KoXml::namedItemNS( bodyElem,
						      KoXmlNS::office, "chart" );
    if ( officeChartElem.isNull() ) {
        kError(32001) << "No office:chart found!";
        KoXmlElement  childElem;
        QString      localName;
        forEachElement( childElem, bodyElem ) {
            localName = childElem.localName();
        }

        if ( localName.isEmpty() )
            setErrorMessage( i18n( "Invalid OASIS OpenDocument file. No tag found inside office:body." ) );
        else
            setErrorMessage( i18n( "This document is not a chart, but %1. Please try opening it with the appropriate application." , KoDocument::tagNameToDocumentType( localName ) ) );

        return false;
    }

    KoXmlElement chartElem = KoXml::namedItemNS( officeChartElem,
						KoXmlNS::chart, "chart" );
    if ( chartElem.isNull() ) {
        setErrorMessage( i18n( "Invalid OASIS OpenDocument file. No chart:chart tag found." ) );
        return false;
    }

    // Get the loading context and stylestack from the styles.
    KoOdfLoadingContext  loadingContext( stylesReader, store );
    //KoStyleStack          &styleStack = loadingContext.styleStack();

#if 0  // Example code!!
    // load chart properties into the stylestack.
    styleStack.save();
    styleStack.setTypeProperties( "chart" ); // load chart properties
    loadingContext.fillStyleStack( chartElem, KoXmlNS::chart, "style-name" );

    const QString fillColor = styleStack.property( KoXmlNS::draw, "fill-color" );
    kDebug(35001) <<"fillColor=" << fillColor;

    styleStack.restore();
#endif

    // Load chart parameters, most of these are stored in the
    // chart:plot-area element within chart:chart.
    QString  errorMessage;
    bool     ok = m_params->loadOasis( chartElem, loadingContext, errorMessage,
				       store);
    if ( !ok ) {
        setErrorMessage( errorMessage );
        return false;
    }

    // TODO Load data direction (see loadAuxiliary)

    // Load the data table.
    KoXmlElement tableElem = KoXml::namedItemNS( chartElem,
						KoXmlNS::table, "table" );
    if ( !tableElem.isNull() ) {
        ok = loadOasisData( tableElem );
        if ( !ok )
            return false; // TODO setErrorMessage
    }

    return true;
#endif
}


bool KChartPart::loadOasisData( const KoXmlElement& tableElem )
{
    Q_UNUSED( tableElem );
#if 0
    int          numberHeaderColumns = 0;
    KoXmlElement  tableHeaderColumns = KoXml::namedItemNS( tableElem,
							  KoXmlNS::table,
							  "table-header-columns" );

    KoXmlElement  elem;
    forEachElement( elem, tableHeaderColumns ) {
        if ( elem.localName() == "table-column" ) {
            int repeated = elem.attributeNS( KoXmlNS::table, "number-columns-repeated", QString() ).toInt();
            numberHeaderColumns += qMax( 1, repeated );
        }
    }

    // With 0 you get no titles, and with more than 1 we ignore the others.
    Q_ASSERT( numberHeaderColumns == 1 );

    int numberDataColumns = 0;
    KoXmlElement tableColumns = KoXml::namedItemNS( tableElem, KoXmlNS::table, "table-columns" );
    forEachElement( elem, tableColumns ) {
        if ( elem.localName() == "table-column" ) {
            int repeated = elem.attributeNS( KoXmlNS::table, "number-columns-repeated", QString() ).toInt();
            numberDataColumns += qMax( 1, repeated );
        }
    }

    // Parse table-header-rows for the column names.
    m_colLabels.clear();
    KoXmlElement tableHeaderRows = KoXml::namedItemNS( tableElem, KoXmlNS::table, "table-header-rows" );
    if ( tableHeaderRows.isNull() )
        kWarning(35001) << "No table-header-rows element found!";
    KoXmlElement tableHeaderRow = KoXml::namedItemNS( tableHeaderRows, KoXmlNS::table, "table-row" );
    if ( tableHeaderRow.isNull() )
        kWarning(35001) << "No table-row inside table-header-rows!";

    int cellNum = 0;
    forEachElement( elem, tableHeaderRow ) {
        if ( elem.localName() == "table-cell" ) {
            ++cellNum;
            if ( cellNum > numberHeaderColumns ) {
                KoXmlElement pElem = KoXml::namedItemNS( elem, KoXmlNS::text, "p" );
                m_colLabels.append( pElem.text() );
            }
        }
    }
    numberDataColumns = qMax( numberDataColumns, cellNum - numberHeaderColumns );
    if ( (int)m_colLabels.count() != numberDataColumns )
        kWarning(35001) << "Got " << m_colLabels.count()
			 << " column titles, expected " << numberDataColumns
			 << endl;

    // Get the number of rows, and read row labels
    int          numberDataRows = 0;
    KoXmlElement  tableRows = KoXml::namedItemNS( tableElem, KoXmlNS::table, "table-rows" );

    m_rowLabels.clear();
    forEachElement( elem, tableRows ) {
        if ( elem.localName() == "table-row" ) {
            int repeated = elem.attributeNS( KoXmlNS::table, "number-rows-repeated", QString() ).toInt();
            Q_ASSERT( repeated <= 1 ); // we don't handle yet the case where data rows are repeated (can this really happen?)
            numberDataRows += qMax( 1, repeated );
            if ( numberHeaderColumns > 0 ) {
                KoXmlElement firstCell = KoXml::namedItemNS( elem, KoXmlNS::table, "table-cell" );
                KoXmlElement pElem = KoXml::namedItemNS( firstCell, KoXmlNS::text, "p" );
                m_rowLabels.append( pElem.text() );
            }
        }
    }

    kDebug(35001) <<"numberHeaderColumns=" << numberHeaderColumns
		   << " numberDataColumns=" << numberDataColumns
                   << " numberDataRows=" << numberDataRows;

    if ( (int)m_rowLabels.count() != numberDataRows)
        kWarning(35001) << "Got " << m_rowLabels.count()
			 << " row labels, expected " << numberDataRows;

    m_currentData.expand( numberDataRows, numberDataColumns );
    m_currentData.setUsedCols( numberDataColumns );
    m_currentData.setUsedRows( numberDataRows );

    // Now really load the cells.
    int row = 0;
    KoXmlElement rowElem;
    forEachElement( rowElem, tableRows ) {
        if ( rowElem.localName() == "table-row" ) {
            int col = 0;
            int cellNum = 0;
            KoXmlElement cellElem;
            forEachElement( cellElem, rowElem ) {
                if ( cellElem.localName() == "table-cell" ) {
                    ++cellNum;
                    if ( cellNum > numberHeaderColumns ) {
                        QString valueType = cellElem.attributeNS( KoXmlNS::office, "value-type", QString() );
                        if ( valueType != "float" )
                            kWarning(35001) << "Don't know how to handle value-type " << valueType;
                        else {
                            QString  value = cellElem.attributeNS( KoXmlNS::office, "value", QString() );
                            qreal    val = value.toDouble();

                            m_currentData.setCell( row, col, val );
                        }
                        ++col;
                    }
                }
            }
            ++row;
        }
    }
#endif
    return true;
}


bool KChartPart::saveOdf( SavingContext & documentContext )
{
    KoStore * store = documentContext.odfStore.store();
    KoXmlWriter * manifestWriter = documentContext.odfStore.manifestWriter();
    // Check if we can create the content.xml file inside the store.
    if ( !store->open( "content.xml" ) )
        return false;

    // The saving is done in three steps:
    //  1. Write the contents of the document to a temporary 'file',
    //     while at the same time creating the generated styles, also 
    //     called 'automatic' styles.
    //  2. Write the contents of the automatic styles to the main file.
    //  3. Copy the contents in the temp file to the main file.
    //

    // The main file.
    KoStoreDevice  contentDev( store );
    KoXmlWriter   *contentWriter = KoOdfWriteStore::createOasisXmlWriter( &contentDev,
                                                         "office:document-content" );

    // Create a temporary file for the document contents.
    // Also check that it was successfully created.
    KTemporaryFile contentTmpFile;
    if ( !contentTmpFile.open() ) {
        qWarning("Creation of temporary file to store document content failed.");
        return false;
    }

    // Start of phase 1: write contents to the temp file.
    KoXmlWriter  contentTmpWriter( &contentTmpFile, 1 );

    contentTmpWriter.startElement( "office:body" );
    contentTmpWriter.startElement( "office:chart" );

    // Write the chart itself.
    KoGenStyles           mainStyles;
    KoShapeSavingContext  shapeSavingContext( contentTmpWriter, mainStyles, 
                                              documentContext.embeddedSaver );
    m_chartShape->saveOdf( shapeSavingContext );

    contentTmpWriter.endElement(); // office:chart
    contentTmpWriter.endElement(); // office:body

    // End of phase 1.
    contentTmpFile.close();


    // Start of phase 2: write out the automatic styles
    contentWriter->startElement( "office:automatic-styles" );

    // Actually write the automatic styles.
    writeAutomaticStyles( *contentWriter, mainStyles );

    // End of phase 2: write automatic styles
    contentWriter->endElement(); // office:automatic-styles

    // Phase 3: Add the contents to the file
    contentWriter->addCompleteElement( &contentTmpFile );

    contentWriter->endElement(); // office:automatic-styles
    contentWriter->endDocument();
    delete contentWriter;

    if ( !store->close() )
        return false;

    // Now we need to write the manifest, i.e. the list of files in
    // the store.

    // Add manifest line for content.xml
    manifestWriter->addManifestEntry( "content.xml",  "text/xml" );

    // Add manifest line for styles.xml
    if ( !store->open( "styles.xml" ) )
        return false;
    manifestWriter->addManifestEntry( "styles.xml",  "text/xml" );

    // FIXME: Do we need this?
    //saveOasisDocumentStyles( store, mainStyles );

    if ( !store->close() ) // done with styles.xml
        return false;


    if( !store->open("settings.xml") )
        return false;

    // Write settings
    // FIXME: Should we save any settings in KChart?
    KoXmlWriter* settingsWriter = KoOdfWriteStore::createOasisXmlWriter(&contentDev,
                                                       "office:document-settings");
    settingsWriter->startElement("office:settings");

    settingsWriter->startElement("config:config-item-set");
    settingsWriter->addAttribute("config:name", "view-settings");

    saveUnitOdf(settingsWriter);

    //saveOasisSettings( *settingsWriter );

    settingsWriter->endElement(); // config:config-item-set

#if 0
    settingsWriter->startElement("config:config-item-set");
    settingsWriter->addAttribute("config:name", "configuration-settings");
    settingsWriter->addConfigItem("SpellCheckerIgnoreList", d->spellListIgnoreAll.join( "," ) );
    settingsWriter->endElement(); // config:config-item-set
#endif

    settingsWriter->endElement(); // office:settings

    settingsWriter->endElement(); // Root:element
    settingsWriter->endDocument();
    delete settingsWriter;

    if(!store->close())
        return false;

    manifestWriter->addManifestEntry("settings.xml", "text/xml");

    setModified( false );

    return true;
}


void KChartPart::writeAutomaticStyles( KoXmlWriter& contentWriter, 
                                       KoGenStyles& mainStyles ) const
{
    QList<KoGenStyles::NamedStyle>  styles = mainStyles.styles( KoGenStyle::ParagraphAutoStyle );
    QList<KoGenStyles::NamedStyle>::const_iterator  it;
    for ( it = styles.constBegin(); it != styles.constEnd(); ++it ) {
        kDebug(35001) << "Style: " << (*it).name;
        (*it).style->writeStyle( &contentWriter, mainStyles, "style:style",
                                 (*it).name, "style:chart-properties" );
    }
}


// ----------------------------------------------------------------
//             Save and Load old KChart file format


QDomDocument KChartPart::saveXML()
{
#if 0
    QDomElement  tmpElem;

    //kDebug(35001) <<"kchart saveXML called";

    // The biggest part of the saving is done by KDChart itself, so we
    // don't have to do it.
    QDomDocument doc = m_params->saveXML( false );

    // ----------------------------------------------------------------
    // The rest of the saving has to be done by us.

    QDomElement docRoot = doc.documentElement();

    // Save auxiliary data.
    QDomElement aux = doc.createElement( "KChartAuxiliary" );
    docRoot.appendChild( aux );

    // The data direction (rows/columns).
    tmpElem = doc.createElement( "direction" );
    tmpElem.setAttribute( "value", (int) m_params->dataDirection() );
    aux.appendChild( tmpElem );

    tmpElem = doc.createElement( "dataaslabel" );
    tmpElem.setAttribute( "firstrow",
			  m_params->firstRowAsLabel() ? "true" : "false" );
    tmpElem.setAttribute( "firstcol",
			  m_params->firstColAsLabel() ? "true" : "false" );
    aux.appendChild( tmpElem );

    tmpElem = doc.createElement( "barnumlines" );
    tmpElem.setAttribute( "value", (int) m_params->barNumLines() );
    aux.appendChild( tmpElem );

    // Save the data values.
    QDomElement data = doc.createElement( "data" );
    docRoot.appendChild( data );

    int cols = m_currentData.usedCols()
             ? qMin(m_currentData.usedCols(), m_currentData.cols())
             : m_currentData.cols();
    int rows = m_currentData.usedRows()
             ? qMin(m_currentData.usedRows(), m_currentData.rows())
             : m_currentData.rows();
    data.setAttribute( "cols", cols );
    data.setAttribute( "rows", rows );
    kDebug(35001) <<"      writing" << cols <<"," << rows <<"  (cols,rows).";

    for (int i=0; i!=rows; ++i) {
        for (int j=0; j!=cols; ++j) {
            QDomElement e = doc.createElement( "cell" );
            data.appendChild( e );
            QString valType;
            QVariant value( m_currentData.cellVal( i,j ) );
            switch ( value.type() ) {
                case QVariant::Invalid:  valType = "NoValue";   break;
                case QVariant::String:   valType = "String";    break;
                case QVariant::Double:   valType = "Double";    break;
                case QVariant::DateTime: valType = "DateTime";  break;
                default: {
                    valType = "(unknown)";
                    kDebug(35001) <<"ERROR: cell" << i <<"," << j
				   << " has unknown type.";
                }
            }

            e.setAttribute( "valType", valType );
            //kDebug(35001) <<"      cell" << i <<"," << j
	    //	   << " saved with type '" << valType << "'." ;
            switch ( value.type() ) {
                case QVariant::String:  e.setAttribute( "value", value.toString() );
                              break;
                case QVariant::Double:  e.setAttribute( "value", QString::number( value.toDouble() ) );
                              break;
                case QVariant::DateTime:e.setAttribute( "value", "" );
                              break;
                default: {
                    e.setAttribute( "value", "" );
                    if( QVariant::Invalid != value.type() )
                        kDebug(35001) <<"ERROR: cell" << i <<"," << j
				       << " has unknown type." ;
                }
            }
        }
    }

    return doc;
#endif
    return QDomDocument();
}


bool KChartPart::loadXML( const KoXmlDocument& doc, KoStore* )
{
    Q_UNUSED( doc );

#if 0
    kDebug(35001) <<"kchart loadXML called";

    // First try to load the KDChart parameters.
    bool  result = m_params->loadXML( doc );

    // If went well, try to load the auxiliary data and the data...
    if (result) {
        result = loadAuxiliary(doc) && loadData( doc, m_currentData );
    }
    else {
	// ...but if it did, try to load the old XML format.
        result = loadOldXML( doc );
    }

    // If everything is OK, then get the headers from the KDChart parameters.
    if (result) {
        QStringList        legendLabels;
        KDChartAxisParams  params;
        params = m_params->axisParams( KDChartAxisParams::AxisPosBottom );

	// Get the legend.
	QString  str;
	uint     index = 0;
	while ( !(str = m_params->legendText(index++)).isNull() )
	    legendLabels << str;

	if (m_params->dataDirection() == KChartParams::DataRows) {
	    m_colLabels = params.axisLabelStringList();
	    m_rowLabels = legendLabels;
	}
	else {
	    m_colLabels = legendLabels;
	    m_rowLabels = params.axisLabelStringList();
	}

	setChartDefaults();
    }

    m_params->setDrawSolidExcessArrows(true);

    return result;
#endif
    return true;
}


#if 0
// Load the auxiliary data.
//
// Currently, that means the data direction.
//
bool KChartPart::loadAuxiliary( const KoXmlDocument& doc )
{
    KoXmlElement  chart = doc.documentElement();
    KoXmlElement  aux   = chart.namedItem("KChartAuxiliary").toElement();

    // Older XML files might be missing this section.  That is OK; the
    // defaults will be used.
    if (aux.isNull())
	return true;

    KoXmlNode node = aux.firstChild();

    // If the aux section exists, it should contain data.
    while (!node.isNull()) {

	KoXmlElement e = node.toElement();
	if (e.isNull()) {
	    // FIXME: Should this be regarded as an error?
	    node = node.nextSibling();
	    continue;
	}

	// Check for direction
	if ( e.tagName() == "direction" ) {
	    if ( e.hasAttribute("value") ) {
		bool  ok;

		// Read the direction. On failure, use the default.
		int   dir = e.attribute("value").toInt(&ok);
		if ( !ok )
		    dir = (int) KChartParams::DataColumns;

		//kDebug(35001) <<"Got aux value \"direction\":" << dir;
		m_params->setDataDirection( (KChartParams::DataDirection) dir );
	    }
	    else {
		kDebug(35001) <<"Error in direction tag.";
	    }
	}

	// Check for first row / col as label
	else if ( e.tagName() == "dataaslabel" ) {
	    QString  val;

	    if ( e.hasAttribute("firstrow") ) {
		// Read the direction. On failure, use the default.
		val = e.attribute("firstrow");
		if ( val == "true" )
		    m_params->setFirstRowAsLabel( true );
		else
		    m_params->setFirstRowAsLabel( false );
	    }
	    else {
		kDebug(35001) <<"Error in barnumlines tag.";
		m_params->setFirstRowAsLabel( false );
	    }

	    if ( e.hasAttribute("firstcol") ) {
		// Read the direction. On failure, use the default.
		val = e.attribute("firstcol");
		if ( val == "true" )
		    m_params->setFirstColAsLabel( true );
		else
		    m_params->setFirstColAsLabel( false );
	    }
	    else {
		kDebug(35001) <<"Error in barnumlines tag.";
		m_params->setFirstColAsLabel( false );
	    }
	}

	// Check for number of lines in a bar chart.
	else if ( e.tagName() == "barnumlines" ) {
	    if ( e.hasAttribute("value") ) {
		bool  ok;

		// Read the number of lines. On failure, use the default.
		int   barNumLines = e.attribute("value").toInt(&ok);
		if ( !ok )
		    barNumLines = 0;

		//kDebug(35001) <<"Got aux value \"barnumlines\":"
		//	       << barNumLines << endl;
		m_params->setBarNumLines( barNumLines );
	    }
	    else {
		kDebug(35001) <<"Error in barnumlines tag.";
	    }
	}
#if 0
	// Expand with more auxiliary types when needed.
	else if ( e.tagName() == "..." ) {
	}
	and so on...
#endif

	node = node.nextSibling();
    }

    return true;
}
#endif


#if 0
bool KChartPart::loadData( const KoXmlDocument& doc )
{
    kDebug(35001) <<"kchart loadData called";

    KoXmlElement chart = doc.documentElement();
    KoXmlElement data = chart.namedItem("data").toElement();
    bool ok;
    int cols = data.attribute("cols").toInt(&ok);
    kDebug(35001) <<"cols readed as:" << cols;
    if ( !ok ){
         return false;
    }

    int rows = data.attribute("rows").toInt(&ok);
    if ( !ok ){
         return false;
    }

    kDebug(35001) << rows <<" x" << cols;
    m_currentData.expand(rows, cols);
    m_currentData.setUsedCols( cols );
    m_currentData.setUsedRows( rows );
    kDebug(35001) <<"Expanded!";
    KoXmlNode n = data.firstChild();
    //QArray<int> tmpExp(rows*cols);
    //QArray<bool> tmpMissing(rows*cols);
    for (int i=0; i!=rows; i++) {
        for (int j=0; j!=cols; j++) {
            if (n.isNull()) {
                kDebug(35001) <<"Some problems, there is less data than it should be!";
                break;
            }
            KoXmlElement e = n.toElement();
            if ( !e.isNull() && e.tagName() == "cell" ) {
                // add the cell to the corresponding place...
                QVariant t;
                if ( e.hasAttribute("value") && e.hasAttribute("valType") ) {
                    QString valueType = e.attribute("valType").toLower();
                    if ( "string" == valueType ) {
                        t = e.attribute("value");
                    }
                    else if ( "double" == valueType ) {
                        bool bOk;
                        qreal val = e.attribute("value").toDouble(&bOk);
                        if ( !bOk )
                            val = 0.0;
                        t = val;
                    /*
                    } else if ( "datetime" == valueType ) {
                        t = . . .
                    */
                    } else {
                        t.clear();
                        if ( "novalue" != valueType )
                            kDebug(35001) <<"ERROR: cell" << i <<"," << j <<" has unknown type '" << valueType <<"'.";
                    }
                } else
                    t.clear();

                m_currentData.setCell(i,j, t );

		/*
                if ( e.hasAttribute( "hide" ) ) {
                    tmpMissing[cols*j+i] = (bool)e.attribute("hide").toInt( &ok );
                    if ( !ok )
                        return false;
                } else {
                    tmpMissing[cols*j+i] = false;
                }
                if ( e.hasAttribute( "dist" ) ) {
                    tmpExp[cols*j+i] = e.attribute("dist").toInt( &ok );
                    if ( !ok )
                        return false;
                } else {
                    tmpExp[cols*j+i] = 0;
                }
		*/

                n = n.nextSibling();
            }
        }
    }
    /*
    m_params->missing=tmpMissing;
    m_params->explode=tmpExp;
    */

    return true;
}
#endif



void  KChartPart::slotModified()
{
    kDebug(35001) <<"slotModified called!";

    setModified(true);
}


bool KChartPart::showEmbedInitDialog( QWidget* /*parent*/ )
{
  // Don't show an embed dialog
  return true;
}


}  //KChart namespace

#include "KChartPart.moc"
