/* This file is part of the KDE project
   Copyright (C) 2002 Laurent Montel <lmontel@mandrakesoft.com>
   Copyright (c) 2003 Lukas Tinkl <lukas@kde.org>
   Copyright (c) 2003 David Faure <faure@kde.org>

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

#include "ooutils.h"
#include <qcolor.h>
#include <koUnit.h>
#include <qregexp.h>

double OoUtils::toPoint( QString value, double defaultVal )
{
    value.simplifyWhiteSpace();
    value.remove( ' ' );

    int index = value.find( QRegExp( "[a-z]{1,2}$" ), -1 );
    if ( index == -1 )
        return defaultVal;

    QString unit = value.mid( index - 1 );
    value.truncate ( index - 1 );

    if ( unit == "cm" )
        return CM_TO_POINT( value.toDouble() );
    else if ( unit == "mm" )
        return MM_TO_POINT( value.toDouble() );
    else if ( unit == "pt" )
        return value.toDouble();
    else
        return defaultVal;
}

bool OoUtils::parseBorder(const QString & tag, double * width, int * style, QColor * color)
{
    //string like "0.088cm solid #800000"

    if (tag.isEmpty() || tag=="none" || tag=="hidden") // in fact no border
        return false;

    QString _width = tag.section(' ', 0, 0);
    QString _style = tag.section(' ', 1, 1);
    QString _color = tag.section(' ', 2, 2);

    *width = toPoint(_width, 1.0);

    if (_style=="double")
        *style = 5;
    else
        *style = 0;

    if (_color.isEmpty())
        *color = QColor();
    else
        *color = QColor(_color);

    return true;
}
