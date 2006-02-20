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


bool KChartParams::loadOasis( const QDomElement& chartElem,
                              KoOasisStyles& /*oasisStyles*/,
                              QString& errorMessage )
{
    const QString  chartClass = chartElem.attributeNS( KoXmlNS::chart, 
						       "class",
						       QString::null );
    bool           knownType = false;

    for ( unsigned int i = 0 ; i < numOasisChartTypes ; ++i ) {
        if ( chartClass == oasisChartTypes[i].oasisClass ) {
            kdDebug(35001) << "found chart of type " << chartClass << endl;
	    //cerr << "found chart of type " << chartClass.latin1() << "\n";
            setChartType( oasisChartTypes[i].chartType );
            knownType = true;
            break;
        }
    }

    if ( !knownType ) {
        errorMessage = i18n( "Unknown chart type %1" ).arg( chartClass );
        kdDebug(35001) << errorMessage << endl;
        return false;
    }

    // TODO title (more details)
    QDomElement  titleElem = KoDom::namedItemNS( chartElem,
						 KoXmlNS::chart, "title" );
    if ( !titleElem.isNull() ) {
	QDomElement  pElem = KoDom::namedItemNS( titleElem,
						 KoXmlNS::text, "p" );
	//kdDebug(35001) << "Title: " << pElem.text() << endl;

	setHeader1Text( pElem.text() );
    }

    // TODO subtitle (more details)
    QDomElement  subtitleElem = KoDom::namedItemNS( chartElem, KoXmlNS::chart, 
						    "subtitle" );
    if ( !titleElem.isNull() ) {
	QDomElement  pElem = KoDom::namedItemNS( subtitleElem,
						 KoXmlNS::text, "p" );
	//kdDebug(35001) << "Subtitle: " << pElem.text() << endl;

	setHeader2Text( pElem.text() );
    }

    // TODO legend
    // TODO plot-area
    // TODO...

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
}


}  //KChart namespace
