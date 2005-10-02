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


#include "kchart_params.h"
#include "KChartParamsIface.h"
#include <dcopobject.h>
#include "kdchart/KDChartParams.h"
#include "kdchart/KDChartAxisParams.h"
#include <koxmlns.h>
#include <klocale.h>
#include <kdebug.h>

namespace KChart
{

KChartParams::KChartParams()
    : KDChartParams()
{
    m_dcop = 0;
    //dcopObject(); // build it
}


KChartParams::~KChartParams()
{
    delete m_dcop;
}


DCOPObject* KChartParams::dcopObject()
{
    if ( !m_dcop )
	m_dcop = new KChartParamsIface( this );
    return m_dcop;
}

static const struct {
    const char* oasisClass;
    KDChartParams::ChartType chartType;
} oasisChartTypes[] = {
    { "chart:bar", KDChartParams::Bar }
// TODO...
};

static const unsigned int numOasisChartTypes
  = sizeof oasisChartTypes / sizeof *oasisChartTypes;


bool KChartParams::loadOasis( const QDomElement& chartElem,
                              KoOasisStyles& /*oasisStyles*/,
                              QString& errorMessage )
{
    const QString chartClass = chartElem.attributeNS( KoXmlNS::chart, "class", QString::null );
    bool knownType = false;
    for ( unsigned int i = 0 ; i < numOasisChartTypes ; ++i ) {
        if ( chartClass == oasisChartTypes[i].oasisClass ) {
            kdDebug(35001) << "found chart of type " << chartClass << endl;
            setChartType( oasisChartTypes[i].chartType );
            knownType = true;
        }
    }
    if ( !knownType ) {
        errorMessage = i18n( "Unknown chart type %1" ).arg( chartClass );
        kdDebug(35001) << errorMessage << endl;
        return false;
    }

    // TODO title
    // TODO legend
    // TODO plot-area
    // TODO...

    return true;
}

}  //KChart namespace
