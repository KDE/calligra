/* This file is part of the KDE project
   Copyright (C) 2001,2002,2003,2004 Laurent Montel <montel@kde.org>
   Copyright (C) 2006 Thorsten Zachmann <zachmann@kde.org>

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


//#include <stdlib.h>
#include <iostream>
using std::cout;
using std::cerr;


#include <dcopobject.h>
#include <klocale.h>
#include <kdebug.h>

#include <KoXmlNS.h>
#include <KoXmlWriter.h>
#include <KoDom.h>
#include <KoOasisLoadingContext.h>

#include "kdchart/KDChartParams.h"
#include "kdchart/KDChartAxisParams.h"

#include "kchart_params.h"
#include "kchart_part.h"
#include "KChartParamsIface.h"


namespace KChart
{


KChartParams::KChartParams( KChartPart *_part )
    : KDChartParams(),
      m_part( _part )
{
    // Default values for subtypes.
    m_firstRowAsLabel = false;
    m_firstColAsLabel = false;

    // Default values for OpenDocument extensions.
    m_barNumLines     = 0;

    m_dcop = 0;
    //dcopObject(); // build it
}


KChartParams::~KChartParams()
{
    delete m_dcop;
}


// ----------------------------------------------------------------


QString KChartParams::chartTypeToString( ChartType _type) const
{
#if 0
    if (_type == BarLines )
	return "BarLines";
    else
#endif
	return KDChartParams::chartTypeToString( (KDChartParams::ChartType) _type );
}


KChartParams::ChartType
KChartParams::stringToChartType( const QString& _string )
{
#if 0
    if ( _string == "BarLines" )
	return BarLines;
    else
#endif
	return (ChartType) KDChartParams::stringToChartType( _string );
}


void KChartParams::setFirstRowAsLabel( bool _val )
{
    m_firstRowAsLabel = _val;

    m_part->doSetData( *m_part->data(),
		       m_firstRowAsLabel, m_firstColAsLabel );
}

void KChartParams::setFirstColAsLabel( bool _val )
{
    m_firstColAsLabel = _val;

    m_part->doSetData( *m_part->data(),
		       m_firstRowAsLabel, m_firstColAsLabel );
}


DCOPObject* KChartParams::dcopObject()
{
    if ( !m_dcop )
	m_dcop = new KChartParamsIface( this );
    return m_dcop;
}


// ================================================================
//                     Loading and Saving



static const struct {
    const char              *oasisClass;
    KChartParams::ChartType  chartType;
} oasisChartTypes[] = {
    { "chart:bar",    KChartParams::Bar },
    { "chart:line",   KChartParams::Line },
    { "chart:area",   KChartParams::Area },
    { "chart:circle", KChartParams::Pie },
    //{ "chart:xxx",    KChartParams::HiLo },
    //{ "chart:stock",  KChartParams::??? },
    //{ "chart:add-in", KChartParams::??? },
    //{ "chart:scatter",KChartParams::??? },
    { "chart:ring",   KChartParams::Ring },
    { "chart:radar",  KChartParams::Polar }
    //{ "chart:xxx",    KChartParams::BoxWhisker },

    //{ "chart:xxx",    KChartParams::BarLines },
    // FIXME: More?
};


static const unsigned int numOasisChartTypes
  = sizeof oasisChartTypes / sizeof *oasisChartTypes;


#if 0  // Example code!!
    KoOasisLoadingContext  loadingContext( this, oasisStyles, store );
    KoStyleStack          &styleStack = loadingContext.styleStack();

    styleStack.save();
    styleStack.setTypeProperties( "graphic" ); // load graphic-properties
    loadingContext.fillStyleStack( chartElem, KoXmlNS::chart, "style-name" );

    const QString fillColor = styleStack.attributeNS( KoXmlNS::draw, "fill-color" );
    kdDebug() << "fillColor=" << fillColor << endl;

    styleStack.restore();
#endif


// Load the data from an OpenDocument chart:chart element.

bool KChartParams::loadOasis( const QDomElement     &chartElem,
			      KoOasisLoadingContext &loadingContext,
                              QString               &errorMessage,
			      KoStore               */*store*/ )
{
    const QString chartClass = chartElem.attributeNS( KoXmlNS::chart,
						      "class", QString::null );
    bool          knownType = false;

    // Find out what KChart charttype the OASIS charttype corresponds to.
    for ( unsigned int i = 0 ; i < numOasisChartTypes ; ++i ) {
        if ( chartClass == oasisChartTypes[i].oasisClass ) {
            kdDebug(35001) << "found chart of type " << chartClass << endl;
	    //cerr << "found chart of type " << chartClass.latin1() << "\n";

            setChartType( oasisChartTypes[i].chartType );
            knownType = true;
            break;
        }
    }

    // If we can't find out what charttype it is, we might as well end here.
    if ( !knownType ) {
        errorMessage = i18n( "Unknown chart type %1" ).arg( chartClass );
        kdDebug(35001) << errorMessage << endl;
        return false;
    }

    // Title TODO (more details, e.g. font, placement etc)
    QDomElement  titleElem = KoDom::namedItemNS( chartElem,
						 KoXmlNS::chart, "title" );
    if ( !titleElem.isNull() ) {
	QDomElement  pElem = KoDom::namedItemNS( titleElem,
						 KoXmlNS::text, "p" );
	setHeader1Text( pElem.text() );
    }

    // Subtitle TODO (more details)
    QDomElement  subtitleElem = KoDom::namedItemNS( chartElem, KoXmlNS::chart,
						    "subtitle" );
    if ( !subtitleElem.isNull() ) {
	QDomElement  pElem = KoDom::namedItemNS( subtitleElem,
						 KoXmlNS::text, "p" );
	setHeader2Text( pElem.text() );
    }

    // TODO: Get legend settings


    // Get the plot-area.  This is where the action is.
    QDomElement  plotareaElem = KoDom::namedItemNS( chartElem,
						    KoXmlNS::chart, "plot-area" );
    if ( !plotareaElem.isNull() ) {
	return loadOasisPlotarea( plotareaElem, loadingContext, errorMessage );
    }

    return false;
}


bool KChartParams::loadOasisPlotarea( const QDomElement     &plotareaElem,
				      KoOasisLoadingContext &loadingContext,
				      QString               &errorMessage )
{
    QString  tmp;

    cerr << ">>> ==========================================================\n";

    // FIXME: attribute table:cell-range-address  - the cells in a spreadsheet

    // Get whether there is a label on the first row or column of the data.
    // This info is in the attribute chart:data-source-has-labels.
    //
    // NOTE: Only used in spreadsheets.
    tmp = plotareaElem.attributeNS( KoXmlNS::chart,
				    "data-source-has-labels", QString::null );
    m_firstRowAsLabel = false;
    m_firstColAsLabel = false;
    if ( tmp == "none" || tmp == "" )
	; // NOTHING
    else if ( tmp == "row" )
	m_firstRowAsLabel = true;
    else if ( tmp == "column" )
	m_firstColAsLabel = true;
    else if ( tmp == "both" ) {
	m_firstRowAsLabel = true;
	m_firstColAsLabel = true;
    }
    else {
	errorMessage = "Unknown value for chart:data-source-has-labels:"
	    + tmp;
	return false;
    }

    // ----------------------------------------------------------------
    // Now get the style and use it to get the contents.
    // This is hidden in the attribute chart:style-name.
    KoStyleStack          &styleStack = loadingContext.styleStack();


    tmp = plotareaElem.attributeNS( KoXmlNS::chart, "style-name",
				    QString::null );
    kdDebug(35001) << "Style name for the plot area: " << tmp << endl;
    styleStack.save();
    styleStack.setTypeProperties( "chart" ); // load chart properties
    loadingContext.fillStyleStack( plotareaElem, KoXmlNS::chart, "style-name", "chart" );

    switch ( m_chartType ) {
    case NoType:
	break;

    case Bar:
	// Find out subtype
	tmp = styleStack.attributeNS( KoXmlNS::chart, "vertical" );
	// FIXME: Vertical is ignored. At least store it so we can
	//        save it again even if we don't support it.

	//kdDebug(35001) << "  ======>  Vertical: " << tmp << "  <======" << endl;
	// Set the bar chart subtype.
	if ( styleStack.attributeNS( KoXmlNS::chart, "stacked" ) == "true" )
	    setBarChartSubType( BarStacked );
	else if ( styleStack.attributeNS( KoXmlNS::chart, "percentage" ) == "true" )
	    setBarChartSubType( BarPercent );
	else
	    setBarChartSubType( BarNormal );

	break;

	// chart:vertical      - true if vertical bars (only bar charts)
	// chart:stacked       - true for stacked bars
	// chart:percentage    - true for percentage  (mut. excl with stacked)
	// chart:connect-bars  - true if lines to connect bars should be drawn
	//                           only used for stacked and percentage bars.
	// FIXME: Support lines on bar charts.
	// chart:lines-used    - 0-n, number of lines on a bar chart. (def: 0)

    case Line:
	// Set the line chart subtype.
	if ( styleStack.attributeNS( KoXmlNS::chart, "stacked" ) == "true" )
	    setLineChartSubType( LineStacked );
	else if ( styleStack.attributeNS( KoXmlNS::chart, "percentage" ) == "true" )
	    setLineChartSubType( LinePercent );
	else
	    setLineChartSubType( LineNormal );

	break;

	// FIXME: Why is this even there?  Seems like an unnecessary attr.
	// chart:lines       - true for line charts, false otherwise

	// chart:stacked     - true for stacked bars
	// chart:percentage  - true for percentage  (mut. excl with stacked)

	// chart:symbol-type - used with line charts, should be "automatic"

    case Area:
	// Set the area chart subtype.
	if ( styleStack.attributeNS( KoXmlNS::chart, "stacked" ) == "true" )
	    setAreaChartSubType( AreaStacked );
	else if ( styleStack.attributeNS( KoXmlNS::chart, "percentage" ) == "true" )
	    setAreaChartSubType( AreaPercent );
	else
	    setAreaChartSubType( AreaNormal );

	break;

	// chart:stacked       - true for stacked bars
	// chart:percentage    - true for percentage  (mut. excl with stacked)

    case Pie:
	break;

    case HiLo:
	break;

    case Ring:
	break;

    case Polar:
	break;

    case BoxWhisker:
	break;
    }
    // TODO:
    // And get the info from the style.  Here is the contents:


    // TODO: These items are currently not used.  They should be at least
    //       be stored so that we can save them again.
    // chart:interpolation     - "cubic-spline" if using cubic splines
    // chart:splines           -
    // chart:spline-order      - "2" for cubic splines
    // chart:spline-resolution - how smooth (default: 20)

    // -- Used when chart:class == "stock:
    // chart:stock-updown-bars      - boolean
    // chart:stock-with-volume      - boolean
    // chart:japanese-candle-sticks - boolean

    // chart:series-source     - "row" or "columns

    // chart:data-label-number - "value" / "percentage" / "none" (def: none)

    // chart:data-label-text   - true if data hapoints have text labels
    // chart:data-label-symbol - true if data hapoints have legend symbol
    //                           (default: false for both)

    styleStack.restore();

    // ----------------------------------------------------------------
    // In the plot-area element there are two chart:axis elements

    QDomElement  xAxisElem;
    QDomElement  yAxisElem;

    QDomElement  axisElem;
    forEachElement( axisElem, plotareaElem ) {

	cerr << "plotarea element: " << axisElem.tagName().latin1() << "\n";
	if ( axisElem.tagName() != "axis" )
	    continue;

	tmp = axisElem.attributeNS( KoXmlNS::chart, "name",
				    QString::null);
	//kdDebug(35001) << "Got axis " << tmp << endl;
	cerr << "Got axis " << tmp.latin1() << "\n";
	if ( tmp == "primary-x" )
	    xAxisElem = axisElem;
	else if ( tmp == "primary-y" )
	    yAxisElem = axisElem;
	else
	    // Only supports two axes so far.
	    continue;
    }

    // Load the axes.
    if ( !loadOasisAxis( xAxisElem, loadingContext, errorMessage,
			 KDChartAxisParams::AxisPosBottom) )
	return false;
    if ( m_chartType != Pie
	 && !loadOasisAxis( yAxisElem, loadingContext, errorMessage,
			    KDChartAxisParams::AxisPosLeft) )
	return false;

    // Attributes for the axes:
    // chart:name       - either "primary-x" or "primary-y"

#if 0
    const QString fillColor = styleStack.attributeNS( KoXmlNS::draw,
						      "fill-color" );
    kdDebug() << "fillColor=" << fillColor << endl;
#endif

    cerr << "<<< ==========================================================\n";

    return true;
}


bool KChartParams::loadOasisAxis( const QDomElement      &axisElem,
				  KoOasisLoadingContext  &loadingContext,
				  QString                &errorMessage,
				  KDChartAxisParams::AxisPos  axisPos )
{
    Q_UNUSED( errorMessage );

    QString        tmp;
    QDomElement    tmpElem;
    KoStyleStack  &styleStack = loadingContext.styleStack();

    // Get the axis to manipulate.
    // TODO

    // Get the axis title if any.
    QDomElement  titleElem = KoDom::namedItemNS( axisElem,
						 KoXmlNS::chart, "title" );
    if ( !titleElem.isNull() ) {
	tmpElem = KoDom::namedItemNS( titleElem, KoXmlNS::text, "p" );
	setAxisTitle( axisPos, tmpElem.text() );
    }



    cerr << ">>> ----------------------------------------------------------\n";
    cerr << "Loading axis " << axisElem.attributeNS( KoXmlNS::chart, "name",
						     QString::null).latin1()
	 << "\n";

    tmp = axisElem.attributeNS( KoXmlNS::chart, "style-name", QString::null );
    //kdDebug(35001) << "Style name for the axis: " << tmp << endl;
    cerr << "Style name for the axis: " << tmp.latin1() << "\n";
    styleStack.save();
    styleStack.setTypeProperties( "chart" ); // load chart properties
    loadingContext.fillStyleStack( axisElem, KoXmlNS::chart, "style-name", "chart" );

    // chart:class      - "category" / "value" / "domain" (domain for scatter)
    //    child: chart:categories

    // child:  chart:title   - Name of title if any.
    // child:  chart:grid
    //           attr: chart:class  - "major" / "minor"

    // chart:style-name - Associated style with the following info:
    // --------------------------------
    // chart:display-label          - true if an axis label should be shown.

    // chart:tick-marks-major-inner - true if display tickmarks at major
    // chart:tick-marks-major-outer   or minor intervals outside / inside
    // chart:tick-minor-major-inner   the chart area.
    // chart:tick-minor-major-outer

    // chart:logarithmic            - true if logarithmic scale

    // text:line-break              - true if categories can be broken

    // chart:text-overlap           - true if labels can overlap

    // chart:label-arrangement      - "side-by-side" / "stagger-even" /
    //                                "stagger-odd"  (def: side-by-side)

    // chart:visible                - true if labels + ticks should be shown.

    // children:
    // chart:
    // chart:
    // chart:
    // chart:
    // chart:
    styleStack.restore();

    cerr << "<<< ----------------------------------------------------------\n";
    return true;
}


QString KChartParams::saveOasisFont( KoGenStyles& mainStyles, const QFont& font, const QColor& color ) const
{
    KoGenStyle::PropertyType tt = KoGenStyle::TextType;
    KoGenStyle autoStyle( KoGenStyle::STYLE_AUTO, "chart", 0 );
    autoStyle.addProperty( "fo:font-family", font.family(), tt );
    autoStyle.addPropertyPt( "fo:font-size", font.pointSize(), tt );
    autoStyle.addProperty( "fo:color", color.isValid() ? color.name() : "#000000", tt );
    int w = font.weight();
    autoStyle.addProperty( "fo:font-weight", w == 50 ? "normal" : w == 75 ? "bold" : QString::number( qRound(  w / 10 ) * 100 ), tt );
    autoStyle.addProperty( "fo:font-style", font.italic() ? "italic" : "normal", tt );

    return mainStyles.lookup( autoStyle, "ch", KoGenStyles::ForceNumbering );
}

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
        kdError(32001) << "Unknown chart type in KChartParams::saveOasis, extend oasisChartTypes!" << endl;
    }

    bodyWriter->startElement( "chart:title" );
    QRect rect( headerFooterRect( KDChartParams::HdFtPosHeader ) );
    bodyWriter->addAttribute( "svg:x", rect.x() );
    bodyWriter->addAttribute( "svg:y", rect.y() );
    bodyWriter->addAttribute( "chart:style-name", saveOasisFont( mainStyles, header1Font(), headerFooterColor( KDChartParams::HdFtPosHeader ) ) );
    bodyWriter->startElement( "text:p" );
    bodyWriter->addTextNode( header1Text() );
    bodyWriter->endElement(); // text:p
    bodyWriter->endElement(); // chart:title

    QString subTitle( header2Text() );
    if ( !subTitle.isEmpty() )
    {

        kdDebug(32001) << "header rect: " << headerFooterRect( KDChartParams::HdFtPosHeader2 ) << endl;
        QRect rect( headerFooterRect( KDChartParams::HdFtPosHeader2 ) );
        bodyWriter->startElement( "chart:subtitle" );
        bodyWriter->addAttribute( "svg:x", rect.x() );
        bodyWriter->addAttribute( "svg:y", rect.y() );
        bodyWriter->addAttribute( "chart:style-name", saveOasisFont( mainStyles, 
                                                                     header2Font(), 
                                                                     headerFooterColor( KDChartParams::HdFtPosHeader2 ) ) );

        bodyWriter->startElement( "text:p" );
        bodyWriter->addTextNode( subTitle );
        bodyWriter->endElement(); // text:p
        bodyWriter->endElement(); // chart:title
    }


    // TODO legend

    bodyWriter->startElement( "chart:plot-area" );
    saveOasisPlotArea( bodyWriter, mainStyles );
    bodyWriter->endElement();

    // TODO...
}

void KChartParams::saveOasisPlotArea( KoXmlWriter* bodyWriter, KoGenStyles& mainStyles ) const
{
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

    // Prepare the style for the plot area
    KoGenStyle plotAreaStyle( KoGenStyle::STYLE_AUTO, "chart" );

    switch ( m_chartType ) {
    case NoType:
	break;

    case Bar:
        switch( barChartSubType() ) {
        case BarStacked:
            plotAreaStyle.addProperty( "chart:stacked", "true" );
            break;
        case BarPercent:
            plotAreaStyle.addProperty( "chart:percentage", "true" );
            break;
        case BarNormal:
            break;
	case BarMultiRows:
	    break;
        }
        plotAreaStyle.addProperty( "chart:vertical", "false" ); // #### always?
        plotAreaStyle.addProperty( "chart:lines-used", 0 ); // #### for now

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

        break;

    case Area:
        // TODO - very similar

    case Pie:
	break;

    case HiLo:
	break;

    case Ring:
	break;

    case Polar:
	break;

    case BoxWhisker:
	break;
    }

    // Register the style, and get back its auto-generated name
    const QString styleName = mainStyles.lookup( plotAreaStyle, "ch" );

    bodyWriter->addAttribute( "chart:style-name", styleName );

    saveOasisAxis( bodyWriter, mainStyles, KDChartAxisParams::AxisPosBottom, "x" );
    saveOasisAxis( bodyWriter, mainStyles, KDChartAxisParams::AxisPosLeft, "y" );

    // TODO chart:series
    // TODO chart:wall
    // TODO chart:floor
}

void KChartParams::saveOasisAxis( KoXmlWriter* bodyWriter, KoGenStyles& mainStyles,
                                  KDChartAxisParams::AxisPos /*axisPos*/, const char* axisName ) const
{
    bodyWriter->startElement( "chart:axis" );

    bodyWriter->addAttribute( "chart:dimension", axisName );
    bodyWriter->addAttribute( "chart:name", QCString( "primary-" ) + axisName );

    KoGenStyle axisStyle( KoGenStyle::STYLE_AUTO, "chart" );

    // TODO save axis style properties, like
    axisStyle.addProperty( "chart:display-label", "true" ); // ###


    const QString styleName = mainStyles.lookup( axisStyle, "ch" );
    bodyWriter->addAttribute( "chart:style-name", styleName );

    // TODO x axis has chart:categories, y axis has chart:grid ?
    // Maybe that part should be done by the caller of saveOasisAxis then
    // including the opening/closing of the chart:axis element...

    bodyWriter->endElement(); // chart:axis
}

}  //KChart namespace

