/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <david@mandrakesoft.com>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

//#include <koGlobal.h>
#include "koUnit.h"
#include <klocale.h>
#include <kglobal.h>

QString KoUnit::unitDescription( Unit _unit )
{
    switch ( _unit )
    {
    case KoUnit::U_MM:
        return i18n("Millimeters (mm)");
    case KoUnit::U_CM:
        return i18n("Centimeters (cm)");
    case KoUnit::U_DM:
        return i18n("Decimeters (dm)");
    case KoUnit::U_INCH:
        return i18n("Inches (in)");
    case KoUnit::U_PT:
        return i18n("Points (pt)" );
    default:
        return i18n("Error!");
    }
}

double KoUnit::ptToUnit( double ptValue, Unit unit )
{
    switch ( unit ) {
    case U_MM:
        return toMM( ptValue );
    case U_CM:
        return toCM( ptValue );
    case U_DM:
        return toDM( ptValue );
    case U_INCH:
        return toInch( ptValue );
    case U_PT:
    default:
        return toPoint( ptValue );
    }
}

QString KoUnit::userValue( double ptValue, Unit unit )
{
    return KGlobal::locale()->formatNumber( ptToUnit( ptValue, unit ) );
}

double KoUnit::ptFromUnit( double value, Unit unit )
{
    switch ( unit ) {
    case U_MM:
        return MM_TO_POINT( value );
    case U_CM:
        return CM_TO_POINT( value );
    case U_DM:
        return DM_TO_POINT( value );
    case U_INCH:
        return INCH_TO_POINT( value );
    case U_PT:
    default:
        return value;
    }
}

double KoUnit::fromUserValue( const QString& value, Unit unit )
{
    bool ok; // TODO pass as parameter
    return ptFromUnit( KGlobal::locale()->readNumber( value, &ok ), unit );
}
