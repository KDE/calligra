/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>, Torben Weis <weis@kde.org>

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

#ifndef kwunit_h
#define kwunit_h

/**
 * KWord stores everthing in pt (using "double") internally.
 * When displaying a value to the user, the value is converted to the user's unit
 * of choice, and rounded to a reasonable precision to avoid 0.999999
 */
class KWUnit
{
public:
    enum Unit {
        U_MM,
        U_PT,
        U_INCH
    };

    // Prepare ptValue to be displayed in pt
    static double toPoint( double ptValue ) {
        // No conversion, only rounding (to 0.001 precision)
        return qRound( ptValue * 1000.0 ) / 1000.0;
    }

    // Prepare ptValue to be displayed in mm
    static double toMM( double ptValue ) {
        // "mm" values are rounded to 0.0001 millimeters
        return qRound( POINT_TO_MM( ptValue ) * 10000.0 ) / 10000.0;
    }

    // Prepare ptValue to be displayed in inch
    static double toInch( double ptValue ) {
        // "in" values are rounded to 0.00001 inches
        return qRound( POINT_TO_INCH( ptValue ) * 100000.0 ) / 100000.0;
    }

    // This method is the one to use to display a value in a dialog
    // Return the value @ptValue converted to @p unit and rounded, ready to be displayed
    static double userValue( double ptValue, Unit unit ) {
        switch ( unit ) {
        case U_MM:
            return toMM( ptValue );
        case U_INCH:
            return toInch( ptValue );
        case U_PT:
        default:
            return toPoint( ptValue );
        }
    }

    // This method is the one to use to read a value from a dialog
    // Return the value in @p unit, converted to points for internal use
    static double fromUserValue( double value, Unit unit ) {
        switch ( unit ) {
        case U_MM:
            return MM_TO_POINT( value );
        case U_INCH:
            return INCH_TO_POINT( value );
        case U_PT:
        default:
            return value;
        }
    }

    // Convert a unit name into a Unit enum
    static Unit unit( const QString &_unitName ) {
        if ( _unitName == "mm" ) return U_MM;
        if ( _unitName == "inch" ) return U_INCH;
        return U_PT;
    }
    // Get the name of a unit
    static QString unitName( Unit _unit ) {
        if ( _unit == U_MM ) return "mm";
        if ( _unit == U_INCH ) return "inch";
        return "pt";
    }
    // Get the full (translated) description of a unit
    static QString unitDescription( Unit _unit );
};

#endif
