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

#include <iostream.h>

#include <qpainter.h>
#include <qpen.h>
#include <qstring.h>

#include "artwork.h"
#include "contextstyle.h"


// I'm not perfectly satisffied with the artwork in here. But at least
// you know... It's pretty cool already.


const char Artwork::leftSmallRoundBracket[] = {
    static_cast<char>(230), // uppercorner
    static_cast<char>(232)  // lowercorner
};
const char Artwork::leftBigRoundBracket[] = {
    static_cast<char>(236), // uppercorner
    static_cast<char>(238), // lowercorner
    static_cast<char>(234)  // line
};
const char Artwork::leftSmallSquareBracket[] = {
    static_cast<char>(233), // uppercorner
    static_cast<char>(235)  // lowercorner
};
const char Artwork::leftBigSquareBracket[] = {
    static_cast<char>(233), // uppercorner
    static_cast<char>(235), // lowercorner
    static_cast<char>(234)  // line
};
const char Artwork::leftSmallCurlyBracket[] = {
    static_cast<char>(230), // uppercorner
    static_cast<char>(232), // lowercorner
    static_cast<char>(234), // line
    static_cast<char>(237)  // middle
};
const char Artwork::leftBigCurlyBracket[] = {
    static_cast<char>(236), // uppercorner
    static_cast<char>(238), // lowercorner
    static_cast<char>(234), // line
    static_cast<char>(237)  // middle
};


Artwork::Artwork(SymbolType t)
    : type(t)
{
}


void Artwork::calcCharSize(const ContextStyle& style, double height, QChar ch)
{
    QFont f = style.getSymbolFont();
    f.setPointSizeFloat(height);
    fontSize = height;

    QFontMetrics fm(f);
    QRect bound = fm.boundingRect(ch);
    setWidth(fm.width(ch));
    setHeight(bound.height());
    setBaseline(-bound.top());
}

bool Artwork::doSimpleRoundBracket(const ContextStyle& style, int height)
{
    return height < style.getBaseSize();
}

bool Artwork::doSimpleSquareBracket(const ContextStyle& style, int height)
{
    return height < style.getBaseSize();
}

bool Artwork::doSimpleCurlyBracket(const ContextStyle& style, int height)
{
    return height < 2*(style.getBaseSize());
}

/**
 * We set our height to 1000. Our width depends on the symbol.
 * The user is expected to scale this down to the size he
 * needs.
 */
void Artwork::calcSizes(const ContextStyle& style, int parentSize)
{
    switch (type) {
    case LeftSquareBracket:
        if (!doSimpleSquareBracket(style, parentSize)) {
            calcSquareBracket(style, parentSize);
        }
        else {
            calcCharSize(style, parentSize, '[');
        }
        break;
    case RightSquareBracket:
        if (!doSimpleSquareBracket(style, parentSize)) {
            calcSquareBracket(style, parentSize);
        }
        else {
            calcCharSize(style, parentSize, ']');
        }
        break;
    case LineBracket:
        calcCharSize(style, parentSize, '|');
        break;
    case SlashBracket:
        calcCharSize(style, parentSize, '/');
        break;
    case BackSlashBracket:
        calcCharSize(style, parentSize, '\\');
        break;
    case LeftCornerBracket:
        calcCharSize(style, parentSize, static_cast<char>(225));
        break;
    case RightCornerBracket:
        calcCharSize(style, parentSize, static_cast<char>(241));
        break;
    case LeftRoundBracket:
        if (!doSimpleRoundBracket(style, parentSize)) {
            calcRoundBracket(style, parentSize);
        }
        else {
            calcCharSize(style, parentSize, '(');
        }
        break;
    case RightRoundBracket:
        if (!doSimpleRoundBracket(style, parentSize)) {
            calcRoundBracket(style, parentSize);
        }
        else {
            calcCharSize(style, parentSize, ')');
        }
        break;
    case Empty:
        setHeight(parentSize);
        setWidth(style.getEmptyRectWidth());
        break;
    case LeftCurlyBracket:
        if (!doSimpleCurlyBracket(style, parentSize)) {
            calcCurlyBracket(style, parentSize);
        }
        else {
            calcCharSize(style, parentSize, '{');
        }
        break;
    case RightCurlyBracket:
        if (!doSimpleCurlyBracket(style, parentSize)) {
            calcCurlyBracket(style, parentSize);
        }
        else {
            calcCharSize(style, parentSize, '}');
        }
        break;
    case Integral:
        calcCharSize(style, parentSize, static_cast<char>(242));
        break;
    case Sum:
        calcCharSize(style, parentSize, static_cast<char>(229));
        break;
    case Product:
        calcCharSize(style, parentSize, static_cast<char>(213));
        break;
    }
}


void Artwork::calcSizes(const ContextStyle& style,
                        ContextStyle::TextStyle tstyle)
{
    double mySize = style.getAdjustedSize( tstyle );
    switch (type) {
    case LeftSquareBracket:
        calcCharSize(style, mySize, '[');
        break;
    case RightSquareBracket:
        calcCharSize(style, mySize, ']');
        break;
    case LineBracket:
        calcCharSize(style, mySize, '|');
        break;
    case SlashBracket:
        calcCharSize(style, mySize, '/');
        break;
    case BackSlashBracket:
        calcCharSize(style, mySize, '\\');
        break;
    case LeftCornerBracket:
        calcCharSize(style, mySize, static_cast<char>(225));
        break;
    case RightCornerBracket:
        calcCharSize(style, mySize, static_cast<char>(241));
        break;
    case LeftRoundBracket:
        calcCharSize(style, mySize, '(');
        break;
    case RightRoundBracket:
        calcCharSize(style, mySize, ')');
        break;
    case Empty:
        calcCharSize(style, mySize, ' ');
        break;
    case LeftCurlyBracket:
        calcCharSize(style, mySize, '{');
        break;
    case RightCurlyBracket:
        calcCharSize(style, mySize, '}');
        break;
    case Integral:
    case Sum:
    case Product:
        break;
    }
}


void Artwork::draw(QPainter& painter, const QRect& r, const ContextStyle& style,
                   int parentSize, const QPoint& origin)
{
    int myX = origin.x() + getX();
    int myY = origin.y() + getY();
    if (!QRect(myX, myY, getWidth(), getHeight()).intersects(r))
        return;

    painter.setPen(style.getDefaultColor());

    switch (type) {
    case LeftSquareBracket:
        if (!doSimpleSquareBracket(style, parentSize)) {
            drawLeftSquareBracket(painter, style, myX, myY, parentSize);
        }
        else {
            drawCharacter(painter, style, myX, myY, '[');
        }
        break;
    case RightSquareBracket:
        if (!doSimpleSquareBracket(style, parentSize)) {
            drawRightSquareBracket(painter, style, myX, myY, parentSize);
        }
        else {
            drawCharacter(painter, style, myX, myY, ']');
        }
        break;
    case LeftCurlyBracket:
        if (!doSimpleCurlyBracket(style, parentSize)) {
            drawLeftCurlyBracket(painter, style, myX, myY, parentSize);
        }
        else {
            drawCharacter(painter, style, myX, myY, '{');
        }
        break;
    case RightCurlyBracket:
        if (!doSimpleCurlyBracket(style, parentSize)) {
            drawRightCurlyBracket(painter, style, myX, myY, parentSize);
        }
        else {
            drawCharacter(painter, style, myX, myY, '}');
        }
        break;
    case LineBracket:
        drawCharacter(painter, style, myX, myY, '|');
        break;
    case SlashBracket:
        drawCharacter(painter, style, myX, myY, '/');
        break;
    case BackSlashBracket:
        drawCharacter(painter, style, myX, myY, '\\');
        break;
    case LeftCornerBracket:
        drawCharacter(painter, style, myX, myY, static_cast<char>(225));
        break;
    case RightCornerBracket:
        drawCharacter(painter, style, myX, myY, static_cast<char>(241));
        break;
    case LeftRoundBracket:
        if (!doSimpleRoundBracket(style, parentSize)) {
            drawLeftRoundBracket(painter, style, myX, myY, parentSize);
        }
        else {
            drawCharacter(painter, style, myX, myY, '(');
        }
        break;
    case RightRoundBracket:
        if (!doSimpleRoundBracket(style, parentSize)) {
            drawRightRoundBracket(painter, style, myX, myY, parentSize);
        }
        else {
            drawCharacter(painter, style, myX, myY, ')');
        }
        break;
    case Empty:
        //drawEmpty(painter, myX, myY, getHeight());
        break;
    case Integral:
        //drawIntegral(painter, style, myX, myY+getHeight()/2, getHeight()/2);
        drawCharacter(painter, style, myX, myY, static_cast<char>(242));
        break;
    case Sum:
        drawCharacter(painter, style, myX, myY, static_cast<char>(229));
        break;
    case Product:
        drawCharacter(painter, style, myX, myY, static_cast<char>(213));
        break;
    }

    // debug
    //painter.setBrush(Qt::NoBrush);
    //painter.setPen(Qt::green);
    //painter.drawRect(myX, myY, getWidth(), getHeight());
}


void Artwork::draw(QPainter& painter, const QRect& r, const ContextStyle& style,
                   ContextStyle::TextStyle, const QPoint& parentOrigin)
{
    int myX = parentOrigin.x() + getX();
    int myY = parentOrigin.y() + getY();
    if (!QRect(myX, myY, getWidth(), getHeight()).intersects(r))
        return;

    painter.setPen(style.getDefaultColor());

    switch (type) {
    case LeftSquareBracket:
        drawCharacter(painter, style, myX, myY, '[');
        break;
    case RightSquareBracket:
        drawCharacter(painter, style, myX, myY, ']');
        break;
    case LeftCurlyBracket:
        drawCharacter(painter, style, myX, myY, '{');
        break;
    case RightCurlyBracket:
        drawCharacter(painter, style, myX, myY, '}');
        break;
    case LineBracket:
        drawCharacter(painter, style, myX, myY, '|');
        break;
    case SlashBracket:
        drawCharacter(painter, style, myX, myY, '/');
        break;
    case BackSlashBracket:
        drawCharacter(painter, style, myX, myY, '\\');
        break;
    case LeftCornerBracket:
        drawCharacter(painter, style, myX, myY, static_cast<char>(225));
        break;
    case RightCornerBracket:
        drawCharacter(painter, style, myX, myY, static_cast<char>(241));
        break;
    case LeftRoundBracket:
        drawCharacter(painter, style, myX, myY, '(');
        break;
    case RightRoundBracket:
        drawCharacter(painter, style, myX, myY, ')');
        break;
    case Empty:
        break;
    case Integral:
    case Sum:
    case Product:
        break;
    }
}


void Artwork::drawCharacter(QPainter& painter, const ContextStyle& style, int x, int y, QChar ch)
{
    QFont f = style.getSymbolFont();
    //f.setRawName("-urw-symbol-medium-r-normal-*-*-720-*-*-p-*-adobe-fontspecific");

    f.setPointSizeFloat(fontSize);

    //QFontMetrics fm(f);
    //QRect bound = fm.boundingRect(ch);
    painter.setFont(f);
    //painter.drawText(x-bound.x(), y-bound.top(), QString(ch));
    painter.drawText(x, y+getBaseline(), QString(ch));

    //painter.drawRect(bound);
    //painter.drawRect(x, y, getWidth(), getHeight());
    //cerr << bound.x() << " " << bound.y() << " " << bound.width() << " " << bound.height() << endl;
}


void Artwork::calcRoundBracket(const ContextStyle& style, int height)
{
    int charHeight;
    char uppercorner;
    char lowercorner;

    if (height <= style.zoomItY(80)) {
        charHeight = height/2;
        uppercorner = leftSmallRoundBracket[0];
        lowercorner = leftSmallRoundBracket[1];
    }
    else {
        charHeight = style.zoomItY(40);
        uppercorner = leftBigRoundBracket[0];
        lowercorner = leftBigRoundBracket[1];
        //line = leftBigRoundBracket[2];
    }

    QFont f = style.getSymbolFont();
    //f.setRawName("-urw-symbol-medium-r-normal-*-*-720-*-*-p-*-adobe-fontspecific");
    f.setPointSize(charHeight);
    QFontMetrics fm(f);
    QRect upperBound = fm.boundingRect(uppercorner);
    QRect lowerBound = fm.boundingRect(lowercorner);

    setWidth(upperBound.width());
    if (height <= style.zoomItY(80)) {
        setHeight(upperBound.height() + lowerBound.height());
    }
    else {
        setHeight(height);
    }
}

void Artwork::drawLeftRoundBracket(QPainter& p, const ContextStyle& style, int x, int y, int height)
{
    if (height <= style.zoomItY(80)) {
        drawLeftSmallRoundBracket(p, style, leftSmallRoundBracket, x, y, height/2);
	//cout << "drawLeftSmallRoundBracket" << endl;
    }
    else {
        drawLeftBigRoundBracket(p, style, leftBigRoundBracket, x, y, style.zoomItY(40), height);
	//cout << "drawLeftBigRoundBracket" << endl;
    }
}

void Artwork::drawRightRoundBracket(QPainter& p, const ContextStyle& style, int x, int y, int height)
{
    if (height <= style.zoomItY(80)) {
        drawRightSmallRoundBracket(p, style, leftSmallRoundBracket, x, y, height/2);
    }
    else {
        drawRightBigRoundBracket(p, style, leftBigRoundBracket, x, y, style.zoomItY(40), height);
    }
}

void Artwork::drawLeftSmallRoundBracket(QPainter& p, const ContextStyle& style, const char chars[], int x, int y, int charHeight, bool left)
{
    QFont f = style.getSymbolFont();
    //f.setRawName("-urw-symbol-medium-r-normal-*-*-720-*-*-p-*-adobe-fontspecific");
    f.setPointSize(charHeight);
    p.setFont(f);

    char uppercorner = chars[0];
    char lowercorner = chars[1];

    QFontMetrics fm(p.fontMetrics());
    QRect upperBound = fm.boundingRect(uppercorner);
    QRect lowerBound = fm.boundingRect(lowercorner);

    if (!left) {
        x -= upperBound.width() - 1;
        y -= upperBound.height() + lowerBound.height() - 1;
    }

    //p.setPen(Qt::gray);
    //p.drawRect(x, y, upperBound.width(), upperBound.height() + lowerBound.height());

    //p.setPen(Qt::black);
    p.drawText(x-upperBound.left(), y-upperBound.top(), QString(QChar(uppercorner)));
    p.drawText(x-lowerBound.left(), y+upperBound.height()-lowerBound.top(), QString(QChar(lowercorner)));
}

void Artwork::drawRightSmallRoundBracket(QPainter& p, const ContextStyle& style, const char chars[], int x, int y, int charHeight)
{
    p.save();
    p.setWorldMatrix(QWMatrix(-1, 0, 0, -1, 2*x, 2*y), true);
    drawLeftSmallRoundBracket(p, style, chars, x, y, charHeight, false);
    p.restore();
}

void Artwork::drawLeftBigRoundBracket(QPainter& p, const ContextStyle& style, const char chars[], int x, int y, int charHeight, int height, bool left)
{
    QFont f = style.getSymbolFont();
    //f.setRawName("-urw-symbol-medium-r-normal-*-*-720-*-*-p-*-adobe-fontspecific");
    f.setPointSize(charHeight);
    p.setFont(f);

    char uppercorner = chars[0];
    char lowercorner = chars[1];
    char line = chars[2];

    QFontMetrics fm(p.fontMetrics());
    QRect upperBound = fm.boundingRect(uppercorner);
    QRect lowerBound = fm.boundingRect(lowercorner);
    QRect lineBound = fm.boundingRect(line);

    if (!left) {
        x -= upperBound.width() - 1;
        y -= height - 1;
    }

    //p.setPen(Qt::gray);
    //p.drawRect(x, y, upperBound.width(), height);

    p.drawText(x-upperBound.left(), y-upperBound.top(), QString(QChar(uppercorner)));
    p.drawText(x-lowerBound.left(), y+height-lowerBound.top()-lowerBound.height(), QString(QChar(lowercorner)));

    int gap = height - upperBound.height() - lowerBound.height();
    p.fillRect(x, y+upperBound.height(), lineBound.width(), gap, style.getDefaultColor());
}

void Artwork::drawRightBigRoundBracket(QPainter& p, const ContextStyle& style, const char chars[], int x, int y, int charHeight, int height)
{
    p.save();
    p.setWorldMatrix(QWMatrix(-1, 0, 0, -1, 2*x, 2*y), true);
    drawLeftBigRoundBracket(p, style, chars, x, y, charHeight, height, false);
    p.restore();
}


void Artwork::calcSquareBracket(const ContextStyle& style, int height)
{
    int charHeight;
    char uppercorner;
    char lowercorner;

    if (height <= style.zoomItY(80)) {
        charHeight = height/2;
        uppercorner = leftSmallSquareBracket[0];
        lowercorner = leftSmallSquareBracket[1];
    }
    else {
        charHeight = style.zoomItY(40);
        uppercorner = leftBigSquareBracket[0];
        lowercorner = leftBigSquareBracket[1];
        //line = leftBigRoundBracket[2];
    }

    QFont f = style.getSymbolFont();
    //f.setRawName("-urw-symbol-medium-r-normal-*-*-720-*-*-p-*-adobe-fontspecific");
    f.setPointSize(charHeight);
    QFontMetrics fm(f);
    QRect upperBound = fm.boundingRect(uppercorner);
    QRect lowerBound = fm.boundingRect(lowercorner);

    setWidth(upperBound.width());
    if (height <= style.zoomItY(80)) {
        setHeight(upperBound.height() + lowerBound.height());
    }
    else {
        setHeight(height);
    }
}

void Artwork::drawLeftSquareBracket(QPainter& p, const ContextStyle& style, int x, int y, int height)
{
    if (height <= style.zoomItY(80)) {
        drawLeftSmallRoundBracket(p, style, leftSmallSquareBracket, x, y, height/2);
    }
    else {
        drawLeftBigRoundBracket(p, style, leftBigSquareBracket, x, y, style.zoomItY(40), height);
    }
}

void Artwork::drawRightSquareBracket(QPainter& p, const ContextStyle& style, int x, int y, int height)
{
    if (height <= style.zoomItY(80)) {
        drawRightSmallRoundBracket(p, style, leftSmallSquareBracket, x, y, height/2);
    }
    else {
        drawRightBigRoundBracket(p, style, leftBigSquareBracket, x, y, style.zoomItY(40), height);
    }
}


void Artwork::calcCurlyBracket(const ContextStyle& style, int height)
{
    int charHeight;
    char uppercorner;
    char lowercorner;
    char line;
    char middle;

    if (height <= style.zoomItY(120)) {
        charHeight = height/3;
        uppercorner = leftSmallCurlyBracket[0];
        lowercorner = leftSmallCurlyBracket[1];
        line = leftSmallCurlyBracket[2];
        middle = leftSmallCurlyBracket[3];
    }
    else {
        charHeight = style.zoomItY(40);
        uppercorner = leftBigCurlyBracket[0];
        lowercorner = leftBigCurlyBracket[1];
        line = leftBigCurlyBracket[2];
        middle = leftBigCurlyBracket[3];
    }

    QFont f = style.getSymbolFont();
    //f.setRawName("-urw-symbol-medium-r-normal-*-*-720-*-*-p-*-adobe-fontspecific");
    f.setPointSize(charHeight);
    QFontMetrics fm(f);
    QRect upperBound = fm.boundingRect(uppercorner);
    QRect lowerBound = fm.boundingRect(lowercorner);
    QRect lineBound = fm.boundingRect(line);
    QRect middleBound = fm.boundingRect(middle);
    int offset = middleBound.width() - lineBound.width();

    setWidth(upperBound.width() + offset);
    if (height <= style.zoomItY(120)) {
        setHeight(upperBound.height() + lowerBound.height() + middleBound.height());
    }
    else {
        setHeight(height);
    }
}

void Artwork::drawLeftCurlyBracket(QPainter& p, const ContextStyle& style, int x, int y, int height)
{
    if (height <= style.zoomItY(120)) {
        drawLeftSmallCurlyBracket(p, style, leftSmallCurlyBracket, x, y, height/3);
    }
    else {
        drawLeftBigCurlyBracket(p, style, leftBigCurlyBracket, x, y, style.zoomItY(40), height);
    }
}

void Artwork::drawRightCurlyBracket(QPainter& p, const ContextStyle& style, int x, int y, int height)
{
    if (height <= style.zoomItY(120)) {
        drawRightSmallCurlyBracket(p, style, leftSmallCurlyBracket, x, y, height/3);
    }
    else {
        drawRightBigCurlyBracket(p, style, leftBigCurlyBracket, x, y, style.zoomItY(40), height);
    }
}

void Artwork::drawLeftSmallCurlyBracket(QPainter& p, const ContextStyle& style, const char chars[], int x, int y, int charHeight, bool left)
{
    QFont f = style.getSymbolFont();
    //f.setRawName("-urw-symbol-medium-r-normal-*-*-720-*-*-p-*-adobe-fontspecific");
    f.setPointSize(charHeight);
    p.setFont(f);

    char uppercorner = chars[0];
    char lowercorner = chars[1];
    char line = chars[2];
    char middle = chars[3];

    QFontMetrics fm(p.fontMetrics());
    QRect upperBound = fm.boundingRect(uppercorner);
    QRect lowerBound = fm.boundingRect(lowercorner);
    QRect lineBound = fm.boundingRect(line);
    QRect middleBound = fm.boundingRect(middle);
    int offset = middleBound.width() - lineBound.width();

    if (!left) {
        x -= upperBound.width() + offset - 1;
        y -= upperBound.height() + lowerBound.height() + middleBound.height() - 1;
    }

    //p.setPen(Qt::gray);
    //p.drawRect(x, y, upperBound.width() + offset, upperBound.height() + lowerBound.height() + middleBound.height());

    //p.setPen(Qt::black);
    p.drawText(x-upperBound.left()+offset, y-upperBound.top(), QString(QChar(uppercorner)));
    p.drawText(x-middleBound.left(), y+upperBound.height()-middleBound.top(), QString(QChar(middle)));
    p.drawText(x-lowerBound.left()+offset, y+upperBound.height()+middleBound.height()-lowerBound.top(), QString(QChar(lowercorner)));
}

void Artwork::drawRightSmallCurlyBracket(QPainter& p, const ContextStyle& style, const char chars[], int x, int y, int charHeight)
{
    p.save();
    p.setWorldMatrix(QWMatrix(-1, 0, 0, -1, 2*x, 2*y), true);
    drawLeftSmallCurlyBracket(p, style, chars, x, y, charHeight, false);
    p.restore();
}

void Artwork::drawLeftBigCurlyBracket(QPainter& p, const ContextStyle& style, const char chars[], int x, int y, int charHeight, int height, bool left)
{
    QFont f = style.getSymbolFont();
    //f.setRawName("-urw-symbol-medium-r-normal-*-*-720-*-*-p-*-adobe-fontspecific");
    f.setPointSize(charHeight);
    p.setFont(f);

    char uppercorner = chars[0];
    char lowercorner = chars[1];
    char line = chars[2];
    char middle = chars[3];

    QFontMetrics fm(p.fontMetrics());
    QRect upperBound = fm.boundingRect(uppercorner);
    QRect lowerBound = fm.boundingRect(lowercorner);
    QRect lineBound = fm.boundingRect(line);
    QRect middleBound = fm.boundingRect(middle);
    int offset = middleBound.width() - lineBound.width();

    if (!left) {
        x -= upperBound.width() + offset - 1;
        y -= height - 1;
    }

    //p.setPen(Qt::gray);
    //p.drawRect(x, y, upperBound.width() + offset, height);

    p.drawText(x-upperBound.left()+offset, y-upperBound.top(), QString(QChar(uppercorner)));
    p.drawText(x-middleBound.left(), y+(height-middleBound.height())/2-middleBound.top(), QString(QChar(middle)));
    p.drawText(x-lowerBound.left()+offset, y+height-lowerBound.top()-lowerBound.height(), QString(QChar(lowercorner)));

    int gap = height/2 - upperBound.height() - middleBound.height() / 2;
    p.fillRect(x+offset, y+upperBound.height(), lineBound.width(), gap, style.getDefaultColor());
    p.fillRect(x+offset, y+(height+middleBound.height())/2, lineBound.width(), gap, style.getDefaultColor());
}

void Artwork::drawRightBigCurlyBracket(QPainter& p, const ContextStyle& style, const char chars[], int x, int y, int charHeight, int height)
{
    p.save();
    p.setWorldMatrix(QWMatrix(-1, 0, 0, -1, 2*x, 2*y), true);
    drawLeftBigCurlyBracket(p, style, chars, x, y, charHeight, height, false);
    p.restore();
}
