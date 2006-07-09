/* This file is part of the KDE project
 Copyright (C) 2003 Alexander Dymo <adymo@mksat.net>

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

#include <klocale.h>

#include <qsettings.h>

#include "commdefs.h"

namespace Kudesigner
{

QString rttiName( int rtti )
{
    switch ( rtti )
    {
    case Rtti_Label:
        return i18n( "Label" );
    case Rtti_Field:
        return i18n( "Field" );
    case Rtti_Special:
        return i18n( "Special" );
    case Rtti_Calculated:
        return i18n( "Calculated Field" );
    case Rtti_Line:
        return i18n( "Line" );
    case Rtti_ReportItem:
    default:
        return i18n( "Report Item" );
    }
}

int Config::m_holderSize = 6;
int Config::m_gridSize = 10;

void Config::setGridSize( int gridSize )
{
    QSettings settings;
    settings.writeEntry( "/kudesigner/gridSize", gridSize );
    m_gridSize = gridSize;
}

int Config::gridSize()
{
    static bool confRead = false;
    if ( !confRead )
    {
        QSettings settings;
        m_gridSize = settings.readNumEntry( "/kudesigner/gridSize", 10 );
        confRead = true;
    }
    return m_gridSize;
}

}
