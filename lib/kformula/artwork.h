/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef ARTWORK_H
#define ARTWORK_H

#include "contextstyle.h"
#include "kformuladefs.h"

class QPainter;

KFORMULA_NAMESPACE_BEGIN

/**
 * A piece of art that may be used by any element.
 */
class Artwork {
public:

    Artwork(SymbolType type = EmptyBracket);
    ~Artwork() {}

    void calcSizes( const ContextStyle& style, ContextStyle::TextStyle tstyle, luPt parentSize );
    void calcSizes( const ContextStyle& style, ContextStyle::TextStyle tstyle );

    void draw( QPainter& painter, const LuPixelRect& r,
               const ContextStyle& style, ContextStyle::TextStyle tstyle,
               luPt parentSize, const LuPixelPoint& origin );
    void draw( QPainter& painter, const LuPixelRect& r,
               const ContextStyle& style, ContextStyle::TextStyle tstyle,
               const LuPixelPoint& parentOrigin );

    luPixel getWidth() const { return size.width(); }
    luPixel getHeight() const { return size.height(); }

    void setWidth( luPixel width ) { size.setWidth(width); }
    void setHeight( luPixel height ) { size.setHeight(height); }

    luPixel getBaseline() const { return baseline; }
    void setBaseline( luPixel line ) { baseline = line; }

    luPixel getX() const { return point.x(); }
    luPixel getY() const { return point.y(); }

    void setX( luPixel x ) { point.setX( x ); }
    void setY( luPixel y ) { point.setY( y ); }

    SymbolType getType() const { return type; }
    void setType(SymbolType t) { type = t; }

private:

    void drawCharacter( QPainter& painter, const ContextStyle& style, luPixel x, luPixel y, luPt height, QChar ch );

    void calcCharSize( const ContextStyle& style, luPt height, QChar ch );

    void calcRoundBracket(  const ContextStyle& style, const QChar chars[], luPt height, luPt charHeight );
    void calcSquareBracket( const ContextStyle& style, const QChar chars[], luPt height, luPt charHeight );
    void calcCurlyBracket(  const ContextStyle& style, const QChar chars[], luPt height, luPt charHeight );

    void drawLeftRoundBracket(   QPainter& p, const ContextStyle& style, luPixel x, luPixel y, luPt height, luPt charHeight );
    void drawRightRoundBracket(  QPainter& p, const ContextStyle& style, luPixel x, luPixel y, luPt height, luPt charHeight );

    void drawLeftSquareBracket(  QPainter& p, const ContextStyle& style, luPixel x, luPixel y, luPt height, luPt charHeight );
    void drawRightSquareBracket( QPainter& p, const ContextStyle& style, luPixel x, luPixel y, luPt height, luPt charHeight );

    void drawLeftLineBracket( QPainter& p, const ContextStyle& style, luPixel x, luPixel y, luPt height, luPt charHeight );
    void drawRightLineBracket( QPainter& p, const ContextStyle& style, luPixel x, luPixel y, luPt height, luPt charHeight );

    void drawLeftCurlyBracket(   QPainter& p, const ContextStyle& style, luPixel x, luPixel y, luPt height, luPt charHeight );
    void drawRightCurlyBracket(  QPainter& p, const ContextStyle& style, luPixel x, luPixel y, luPt height, luPt charHeight );

    void drawSmallRoundBracket( QPainter& p, const ContextStyle& style, const QChar chars[], luPixel x, luPixel y, luPt charHeight );
    void drawBigRoundBracket(   QPainter& p, const ContextStyle& style, const QChar chars[], luPixel x, luPixel y, luPt charHeight );
    void drawSmallCurlyBracket( QPainter& p, const ContextStyle& style, const QChar chars[], luPixel x, luPixel y, luPt charHeight );
    void drawBigCurlyBracket(   QPainter& p, const ContextStyle& style, const QChar chars[], luPixel x, luPixel y, luPt charHeight );

    LuPixelSize size;
    LuPixelPoint point;

    /**
     * Used if we are a character.
     */
    luPixel baseline;

    SymbolType type;
};

KFORMULA_NAMESPACE_END

#endif // ARTWORK_H
