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

enum KWUnits {
    U_MM,
    U_PT,
    U_INCH
};

class KWUnit
{
public:
    KWUnit() { _pt = 0.0; _mm = 0.0; _inch = 0.0; }
    KWUnit( double __pt, double ___mm, double __inch )
    { _pt = __pt; _mm = ___mm; _inch = __inch; }
    KWUnit( double ___mm )
    { setMM( ___mm ); }
    KWUnit( const KWUnit &unit ) {
        _pt = unit._pt;
        _mm = unit._mm;
        _inch = unit._inch;
    }

    inline void setPT( double __pt )
    { _pt = __pt; _mm = POINT_TO_MM( _pt ); _inch = POINT_TO_INCH( _pt ); }
    inline void setMM( double ___mm )
    { _mm = ___mm; _pt = MM_TO_POINT( _mm ); _inch = MM_TO_INCH( _mm ); }
    inline void setINCH( double __inch )
    { _inch = __inch; _mm = INCH_TO_MM( _inch ); _pt = INCH_TO_POINT( _inch ); }
    inline void setPT_MM( double __pt, double ___mm )
    { _pt = __pt; _mm = ___mm; _inch = MM_TO_INCH( _mm ); }
    inline void setPT_INCH( double __pt, double __inch )
    { _pt = __pt; _inch = __inch; _mm = INCH_TO_MM( _inch ); }
    inline void setMM_INCH( double ___mm, double __inch )
    { _mm = ___mm; _inch = __inch; _pt = MM_TO_POINT( _mm ); }
    inline void setPT_MM_INCH( double __pt, double ___mm, double __inch )
    { _pt = __pt; _mm = ___mm; _inch = __inch; }

    inline double pt() const { return _pt; }
    inline double mm() const { return _mm; }
    inline double inch() const { return _inch; }

    // Return the value for this measure, converted to @p unitType
    inline double value( KWUnits _unitType );

    KWUnit &operator=( const KWUnit &unit );

    // Convert a unit name into a KWUnits enum
    static KWUnits unitType( const QString &_unit );

    // Create a measure using a given @p value in a given @p unitType
    static KWUnit createUnit( double value, KWUnits unitType );

protected:
    double _pt;
    double _mm;
    double _inch;

};

inline KWUnit &KWUnit::operator=( const KWUnit &unit )
{
    _pt = unit._pt;
    _mm = unit._mm;
    _inch = unit._inch;
    return *this;
}

inline QTextStream& operator<<( QTextStream&out, const KWUnit &unit )
{
    out << "pt=\"" << unit.pt() << "\" mm=\"" << unit.mm() << "\" inch=\"" << unit.inch() << "\"";
    return out;
}

inline double KWUnit::value( KWUnits _unitType )
{
    switch ( _unitType ) {
        case U_MM:
            return _mm;
        case U_INCH:
            return _inch;
        case U_PT:
        default:
            return _pt;
    }
}

inline KWUnit KWUnit::createUnit( double value, KWUnits unitType )
{
    KWUnit u;
    switch ( unitType ) {
        case U_MM:
            u.setMM(value);
            break;
        case U_INCH:
            u.setINCH(value);
            break;
        case U_PT:
        default:
            u.setPT(value);
    }
    return u;
}

inline KWUnits KWUnit::unitType( const QString & _unit )
{
    if ( _unit == "mm" ) return U_MM;
    if ( _unit == "inch" ) return U_INCH;
    return U_PT;
}

#endif
