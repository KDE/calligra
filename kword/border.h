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
class KWDocument;
class QPainter;

/**
 * This class represents a border - for anything, like paragraph, or frame
 */
class Border : public Qt
{
public:
    enum BorderStyle {SOLID = 0, DASH = 1, DOT = 2, DASH_DOT = 3, DASH_DOT_DOT = 4};
    Border();
    Border( const QColor & c, BorderStyle s, double width );
    QColor color;
    BorderStyle style;
    double ptWidth;

    bool operator==( const Border _brd ) const;
    bool operator!=( const Border _brd ) const;

    // Load from XML
    static Border loadBorder( const QDomElement & elem );
    // Save to XML
    void save( QDomElement & elem );

    // String to style enum, and vice versa, for UI.
    static BorderStyle getStyle( const QString &style );
    static QString getStyle( const BorderStyle &style );

    // Zoom the border width. If ptWidth is 0, minborder is returned.
    static int zoomWidthX( double ptWidth, KWDocument * doc, int minborder ); // For left/right borders
    static int zoomWidthY( double ptWidth, KWDocument * doc, int minborder ); // For top/bottom borders

    // Get a ready-to-use QPen for this border.
    // width is usually QMAX(doc->zoomIt[XY]( brd.ptWidth ), 1)
    // @internal
    static QPen borderPen( const Border & brd, int width );

    // The do-it-all method :)
    // Draws in @p painter the 4 borders on the _outside_ of @p rect.
    // If a border is of size 0, minborder will be applied (no border if 0, defaultPen otherwise)
    static void drawBorders( QPainter& painter, KWDocument * doc, QRect rect,
                             Border left, Border right, Border top, Border bottom,
                             int minborder, QPen defaultPen );
};

#endif
