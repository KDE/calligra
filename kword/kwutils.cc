/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#include "kwutils.h"

const QCString RNUnits[] = {"", "i", "ii", "iii", "iv", "v", "vi", "vii", "viii", "ix"};
const QCString RNTens[] = {"", "x", "xx", "xxx", "xl", "l", "lx", "lxx", "lxxx", "xc"};
const QCString RNHundreds[] = {"", "c", "cc", "ccc", "cd", "d", "dc", "dcc", "dccc", "cm"};
const QCString RNThousands[] = {"", "m", "mm", "mmm"};

QString makeRomanNumber( int n )
{
    return QString::fromLatin1( RNThousands[ ( n / 1000 ) ] +
                                RNHundreds[ ( n / 100 ) % 10 ] +
                                RNTens[ ( n / 10 ) % 10 ] +
                                RNUnits[ ( n ) % 10 ] );
}

/*================================================================*/
QString correctQString( const QString &str )
{
    if ( str.lower() == "(null)" )
        return QString::null;

    return str;
}

/*================================================================*/
QString correctQString( const char *str )
{
    return correctQString( QString( str ) );
}
