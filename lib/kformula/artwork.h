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

#include <qpoint.h>
#include <qsize.h>

#include "contextstyle.h"
#include "formuladefs.h"

class QPainter;


/**
 * A piece of art that may be used by any element.
 */
class Artwork {
public:

    Artwork(SymbolType type = Empty);
    ~Artwork() {}

    void calcSizes(const ContextStyle& style, int parentSize);
    void calcSizes(const ContextStyle& style, ContextStyle::TextStyle tstyle);
    void draw(QPainter& painter, const QRect& r, const ContextStyle& style,
              int parentSize, const QPoint& origin);
    void draw(QPainter& painter, const QRect& r, const ContextStyle& style,
              ContextStyle::TextStyle tstyle, const QPoint& parentOrigin);

    int getWidth() const { return size.width(); }
    int getHeight() const { return size.height(); }

    void setWidth(int width) { size.setWidth(width); }
    void setHeight(int height) { size.setHeight(height); }

    int getBaseline() const { return baseline; }
    void setBaseline(int line) { baseline = line; }

    int getX() const { return point.x(); }
    int getY() const { return point.y(); }

    void setX(int x) { point.setX(x); }
    void setY(int y) { point.setY(y); }

    SymbolType getType() const { return type; }
    void setType(SymbolType t) { type = t; }

private:

    void drawCharacter(QPainter& painter, const ContextStyle& style, int x, int y, QChar ch);

    bool doSimpleRoundBracket(const ContextStyle& style, int height);
    bool doSimpleSquareBracket(const ContextStyle& style, int height);
    bool doSimpleCurlyBracket(const ContextStyle& style, int height);

    void calcCharSize(const ContextStyle& style, double height, QChar ch);

    void calcRoundBracket(const ContextStyle& style, int height);
    void calcSquareBracket(const ContextStyle& style, int height);
    void calcCurlyBracket(const ContextStyle& style, int height);

    void drawLeftRoundBracket(QPainter& p, const ContextStyle& style, int x, int y, int height);
    void drawRightRoundBracket(QPainter& p, const ContextStyle& style, int x, int y, int height);

    void drawLeftSmallRoundBracket(QPainter& p, const ContextStyle& style, const char chars[], int x, int y, int charHeight, bool left = true);
    void drawRightSmallRoundBracket(QPainter& p, const ContextStyle& style, const char chars[], int x, int y, int charHeight);
    void drawLeftBigRoundBracket(QPainter& p, const ContextStyle& style, const char chars[], int x, int y, int charHeight, int height, bool left = true);
    void drawRightBigRoundBracket(QPainter& p, const ContextStyle& style, const char chars[], int x, int y, int charHeight, int height);

    void drawLeftSquareBracket(QPainter& p, const ContextStyle& style, int x, int y, int height);
    void drawRightSquareBracket(QPainter& p, const ContextStyle& style, int x, int y, int height);

    void drawLeftCurlyBracket(QPainter& p, const ContextStyle& style, int x, int y, int height);
    void drawRightCurlyBracket(QPainter& p, const ContextStyle& style, int x, int y, int height);

    void drawLeftSmallCurlyBracket(QPainter& p, const ContextStyle& style, const char chars[], int x, int y, int charHeight, bool left = true);
    void drawRightSmallCurlyBracket(QPainter& p, const ContextStyle& style, const char chars[], int x, int y, int charHeight);
    void drawLeftBigCurlyBracket(QPainter& p, const ContextStyle& style, const char chars[], int x, int y, int charHeight, int height, bool left = true);
    void drawRightBigCurlyBracket(QPainter& p, const ContextStyle& style, const char chars[], int x, int y, int charHeight, int height);

    QSize size;
    QPoint point;

    /**
     * Used if we are a character.
     */
    double fontSize;
    int baseline;

    SymbolType type;

    static const char leftSmallRoundBracket[];
    static const char leftBigRoundBracket[];
    static const char leftSmallSquareBracket[];
    static const char leftBigSquareBracket[];
    static const char leftSmallCurlyBracket[];
    static const char leftBigCurlyBracket[];
};


#endif // ARTWORK_H
