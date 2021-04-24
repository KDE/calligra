/*
 *  SPDX-FileCopyrightText: 2010 Sebastian Sauer <sebsauer@kdab.com>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
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
