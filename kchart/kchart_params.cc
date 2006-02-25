/* This file is part of the KDE project
   Copyright (C) 2001,2002,2003,2004 Laurent Montel <montel@kde.org>

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
    m_barlinesChartSubType = BarlinesNormal;
    m_barlinesNumLines     = 1;

    m_firstRowAsLabel = false;
    m_firstColAsLabel = false;

    m_dcop = 0;
    //dcopObject(); // build it
}


KChartParams::~KChartParams()
{
    delete m_dcop;
}


QString KChartParams::chartTypeToString( ChartType _type) const
{
    if (_type == BarLines )
	return "BarLines";
    else
	return KDChartParams::chartTypeToString( (KDChartParams::ChartType) _type );
}


KChartParams::ChartType
KChartParams::stringToChartType( const QString& _string )
{
    if ( _string == "BarLines" )
	return BarLines;
    else
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


// Load an the data from an OpenDocument chart:chart element.

bool KChartParams::loadOasis( const QDomElement &chartElem,
                              KoOasisStyles     &oasisStyles,
                              QString           &errorMessage,
			      KoStore           *store )
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

    // TODO title (more details, e.g. font, placement etc)
    QDomElement  titleElem = KoDom::namedItemNS( chartElem,
						 KoXmlNS::chart, "title" );
    if ( !titleElem.isNull() ) {
	QDomElement  pElem = KoDom::namedItemNS( titleElem,
						 KoXmlNS::text, "p" );
	setHeader1Text( pElem.text() );
    }

    // TODO subtitle (more details)
    QDomElement  subtitleElem = KoDom::namedItemNS( chartElem, KoXmlNS::chart, 
						    "subtitle" );
    if ( !titleElem.isNull() ) {
	QDomElement  pElem = KoDom::namedItemNS( subtitleElem,
						 KoXmlNS::text, "p" );
	setHeader2Text( pElem.text() );
    }

    // TODO: Get legend settings

    
    // Get the plot-area.  This is where the action is.
    QDomElement  plotareaElem = KoDom::namedItemNS( chartElem,
						    KoXmlNS::chart, "plot-area" );
    if ( !plotareaElem.isNull() ) {
	return loadOasisPlotarea( plotareaElem, oasisStyles, errorMessage,
				  store );
    }

    return false;
}


bool KChartParams::loadOasisPlotarea( const QDomElement &plotareaElem,
				      KoOasisStyles     &oasisStyles,
				      QString           &errorMessage,
				      KoStore           *store)
{
    QString  tmp;

    // FIXME: attribute table:cell-range-address  - the cells in a spreadsheet

    // Get whether there is a label on first row or column.
    // This info is in the attribute chart:data-source-has-labels.
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
    KoOasisLoadingContext  loadingContext( m_part, oasisStyles, store );
    KoStyleStack          &styleStack = loadingContext.styleStack();

    styleStack.save();
    styleStack.setTypeProperties( "chart" ); // load chart properties
    loadingContext.fillStyleStack( plotareaElem, KoXmlNS::chart, "style-name" );

    // TODO:
    // And get the info from the style.  Here is the contents:

    // chart:lines       - true for line charts, false otherwise
    // chart:symbol-type - used with line charts, should be "automatic"

    // chart:interpolation     - "cubic-spline" if using cubic splines
    // chart:splines           - 
    // chart:spline-order      - "2" for cubic splines
    // chart:spline-resolution - how smooth (default: 20)

    // chart:vertical          - true if vertical bars (only bar charts)
    // chart:stacked           - true for stacked bars
    // chart:percentage        - true for percentage  (mut. excl with stacked)
    // chart:connect-bars      - true if lines to connect bars should be drawn
    //                           only used for stacked and percentage bars.
    // chart:lines-used        - 0-n, number of lines on a bar chart. (def: 0)

    // -- Used when chart:class == "stock:
    // chart:stock-updown-bars      - boolean 
    // chart:stock-with-volume      - boolean 
    // chart:japanese-candle-sticks - boolean 

    // chart:series-source     - "row" or "columns

    // chart:data-label-number - "value" / "percentage" / "none" (def: none)

    // chart:data-label-text   - true if data hapoints have text labels
    // chart:data-label-symbol - true if data hapoints have legend symbol
    //                           (default: false for both)


    // ----------------------------------------------------------------
    // In the plot-area element there are two chart:axis elements

    // Get the chart:axis elements.
    QDomNode     node      = plotareaElem.firstChild();
    QDomElement  xAxisElem = node.toElement();

    node = node.nextSibling();
    QDomElement  yAxisElem = node.toElement();

    if ( xAxisElem.tagName() != "chart:axis"
	 || yAxisElem.tagName() != "chart:axis" )
    {
#if 0
	// FIXME: Check that there is only these two children of plot-area.
	// FIXME: Error handling.
	errorMessage = "Error in axis loading";
	return false;
#endif
    }

    // Attributes for the axes:
    // chart:name       - either "primary-x" or "primary-y"
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

#if 0
    const QString fillColor = styleStack.attributeNS( KoXmlNS::draw,
						      "fill-color" );
    kdDebug() << "fillColor=" << fillColor << endl;
#endif

    styleStack.restore();

    return true;
}



void KChartParams::saveOasis( KoXmlWriter* bodyWriter, KoGenStyles& mainStyles )
{
    Q_UNUSED( mainStyles );
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

    // TODO title
    // TODO subtitle
    // TODO legend
    // TODO plot-area
    // TODO...
}


}  //KChart namespace
