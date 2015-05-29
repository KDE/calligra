/*
 *  Copyright (c) 2010 Sebastian Sauer <sebsauer@kdab.com>
 *
 *  This library is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation; either version 2.1 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef XLSUTILS_H
#define XLSUTILS_H

#include <QDebug>
#include <math.h>

// translate the range-character to a number
int rangeCharToInt(char c)
{
    return (c >= 'A' && c <= 'Z') ? (c - 'A' + 1) : -1;
}

// translates the range-string into a number
int rangeStringToInt(const QString &string)
{
    int result = 0;
    const int size = string.size();
    for ( int i = 0; i < size; i++ )
        result += rangeCharToInt( string[i].toLatin1() ) * pow( 10.0, ( size - i - 1 ) );
    return result;
}

// splits a given cellrange like Sheet1.D2:Sheet1.F2, Sheet1.D2:F2, D2:F2 or D2 into its parts
QPair<QString,QRect> splitCellRange(QString range)
{
    range.remove( '$' ); // remove "fixed" character
    // remove []
    if(range.startsWith(QLatin1Char('[')) && range.endsWith(QLatin1Char(']'))) {
        range.remove(0, 1).chop(1);
    }
    QPair<QString,QRect> result;
    const bool isPoint = !range.contains( ':' );
    QRegExp regEx = isPoint ? QRegExp( "(.*)(\\.|\\!)([A-Z]+)([0-9]+)" ) : QRegExp ( "(.*)(\\.|\\!)([A-Z]+)([0-9]+)\\:(|.*\\.)([A-Z]+)([0-9]+)" );
    if ( regEx.indexIn( range ) >= 0 ) {
        const QString sheetName = regEx.cap( 1 );
        QPoint topLeft( rangeStringToInt( regEx.cap(3) ), regEx.cap(4).toInt() );
        if ( isPoint ) {
            result = QPair<QString,QRect>(sheetName, QRect(topLeft,QSize(1,1)));
        } else {
            QPoint bottomRight( rangeStringToInt( regEx.cap(6) ), regEx.cap(7).toInt() );
            result = QPair<QString,QRect>(sheetName, QRect(topLeft,bottomRight));
        }
    }
    return result;
}

#endif
