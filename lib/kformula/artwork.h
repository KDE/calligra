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

    Artwork(SymbolType type = Empty);
    ~Artwork() {}

    void calcSizes( const ContextStyle& style, lu parentSize );
    void calcSizes( const ContextStyle& style, ContextStyle::TextStyle tstyle );

    void draw( QPainter& painter, const LuRect& r, const ContextStyle& style,
               lu parentSize, const LuPoint& origin );
    void draw( QPainter& painter, const LuRect& r, const ContextStyle& style,
               ContextStyle::TextStyle tstyle, const LuPoint& parentOrigin );

    lu getWidth() const { return size.width(); }
    lu getHeight() const { return size.height(); }

    void setWidth( lu width ) { size.setWidth(width); }
    void setHeight( lu height ) { size.setHeight(height); }

    lu getBaseline() const { return baseline; }
    void setBaseline( lu line ) { baseline = line; }

    lu getX() const { return point.x(); }
    lu getY() const { return point.y(); }

    void setX( lu x ) { point.setX( x ); }
    void setY( lu y ) { point.setY( y ); }

    SymbolType getType() const { return type; }
    void setType(SymbolType t) { type = t; }

private:

    void drawCharacter( QPainter& painter, const ContextStyle& style, lu x, lu y, QChar ch );

    bool doSimpleRoundBracket( const ContextStyle& style, lu height );
    bool doSimpleSquareBracket( const ContextStyle& style, lu height );
    bool doSimpleCurlyBracket( const ContextStyle& style, lu height );

    void calcCharSize( const ContextStyle& style, lu height, QChar ch );

    void calcRoundBracket(const ContextStyle& style, const char chars[], lu height);
    void calcSquareBracket(const ContextStyle& style, const char chars[], lu height);
    void calcCurlyBracket(const ContextStyle& style, const char chars[], lu height);

    void drawLeftRoundBracket(QPainter& p, const ContextStyle& style, lu x, lu y, lu height);
    void drawRightRoundBracket(QPainter& p, const ContextStyle& style, lu x, lu y, lu height);

    void drawLeftSquareBracket(QPainter& p, const ContextStyle& style, lu x, lu y, lu height);
    void drawRightSquareBracket(QPainter& p, const ContextStyle& style, lu x, lu y, lu height);

    void drawLeftCurlyBracket(QPainter& p, const ContextStyle& style, lu x, lu y, lu height);
    void drawRightCurlyBracket(QPainter& p, const ContextStyle& style, lu x, lu y, lu height);

    void drawSmallRoundBracket(QPainter& p, const ContextStyle& style, const char chars[], lu x, lu y, lu charHeight);
    void drawBigRoundBracket(QPainter& p, const ContextStyle& style, const char chars[], lu x, lu y, lu charHeight, lu height);
    void drawSmallCurlyBracket(QPainter& p, const ContextStyle& style, const char chars[], lu x, lu y, lu charHeight);
    void drawBigCurlyBracket(QPainter& p, const ContextStyle& style, const char chars[], lu x, lu y, lu charHeight, lu height);

    LuSize size;
    LuPoint point;

    /**
     * Used if we are a character.
     */
    lu fontSize;
    lu baseline;

    SymbolType type;

    static const char leftRoundBracket[];
    static const char leftSquareBracket[];
    static const char leftCurlyBracket[];

    static const char rightRoundBracket[];
    static const char rightSquareBracket[];
    static const char rightCurlyBracket[];
};

KFORMULA_NAMESPACE_END

#endif // ARTWORK_H
