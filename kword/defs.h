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

#ifndef defs_h
#define defs_h

#define KWORD_VERSION "pre-Beta1"

#include <qtextstream.h>
#include <qstring.h>
#include <koGlobal.h>
#include "kword_factory.h"

#define MIME_TYPE "application/x-kword"
#define STANDARD_COLUMN_SPACING 3

enum MouseMode {
    MM_EDIT = 0,
    MM_EDIT_FRAME = 1,
    MM_CREATE_TEXT = 2,
    MM_CREATE_PIX = 3,
    MM_CREATE_CLIPART = 4,
    MM_CREATE_TABLE = 5,
    MM_CREATE_FORMULA = 6,
    MM_CREATE_PART = 7,
    MM_CREATE_KSPREAD_TABLE = 8
};

enum EditMode {
    EM_INSERT,
    EM_DELETE,
    EM_BACKSPACE,
    EM_CMOVE,
    EM_NONE,
    EM_RETURN
};

enum InsertPos {
    I_BEFORE,
    I_AFTER
};

enum KWUnits {
    U_MM, U_PT,
    U_INCH
};

enum KWTblCellSize {
    TblAuto = 0,
    TblManual
};

const QChar KWSpecialChar( static_cast<char>( 0 ) );

#define KWBarIcon( x ) BarIcon( x, KWordFactory::global() )

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

    inline const double pt() { return _pt; }
    inline const double mm() { return _mm; }
    inline const double inch() { return _inch; }

    KWUnit &operator=( const KWUnit &unit );

    static const KWUnits unitType( const QString _unit );

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

inline QTextStream& operator<<( QTextStream&out, KWUnit &unit )
{
    out << "pt=\"" << unit.pt() << "\" mm=\"" << unit.mm() << "\" inch=\"" << unit.inch() << "\"";
    return out;
}

inline const KWUnits KWUnit::unitType( const QString _unit )
{
    if ( _unit == "mm" ) return U_MM;
    if ( _unit == "inch" ) return U_INCH;
    return U_PT;
}

#endif
