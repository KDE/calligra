/* This file is part of the KDE project
   Copyright (C) 2000 Thomas Zander <zander@earthling.net>

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

#ifndef KWBorder_h
#define KWBorder_h

#include <qcolor.h>
#include <qpen.h>
class QDomElement;

class Border : public Qt {

public:
    enum BorderStyle {SOLID = 0, DASH = 1, DOT = 2, DASH_DOT = 3, DASH_DOT_DOT = 4};
    Border();
    Border( const QColor & c, BorderStyle s, unsigned int width );
    QColor color;
    BorderStyle style;
    double ptWidth;

    bool operator==( const Border _brd ) const;
    bool operator!=( const Border _brd ) const;

    static Border loadBorder( const QDomElement & elem );
    void save( QDomElement & elem );
    static QPen borderPen( const Border & _brd );
    static BorderStyle getStyle( const QString &style );
    static QString getStyle( const BorderStyle &style );
};

#endif
