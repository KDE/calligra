/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
                      
   Based on the other kformula implementation
   Copyright (C) 1999 Ilya Baran (ibaran@mit.edu)

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


Artwork::Artwork(SymbolType t)
    : type(t)
{
}

/**
 * Multiplies width and height with the factor.
 */
void Artwork::scale(double factor)
{
    double width = getWidth()*factor;
    double height = getHeight()*factor;
    setWidth((int)width);
    setHeight((int)height);
}


void Artwork::calcCharSize(const ContextStyle& style, int height, char ch)
{
    QFont f = style.getSymbolFont();
    f.setPointSize(height);
    fontSize = height;

    QFontMetrics fm(f);
    QRect bound = fm.boundingRect(ch);
    setWidth(bound.width());
    setHeight(bound.height());
}

bool Artwork::doSimpleRoundBracket(const ContextStyle& style, int height)
{
    return height < style.getBaseSize() + style.getSizeReduction();
}

bool Artwork::doSimpleCurlyBracket(const ContextStyle& style, int height)
{
    return height < 2*(style.getBaseSize() + style.getSizeReduction());
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
        calcCharSize(style, parentSize, '[');
        break;
    case RightSquareBracket:
        calcCharSize(style, parentSize, ']');
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
        calcCharSize(style, parentSize, static_cast<char>(183));
        break;
    case RightCornerBracket:
        calcCharSize(style, parentSize, static_cast<char>(210));
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
        //setHeight(parentSize);
        //setWidth(7.0/12/2*parentSize);
        calcCharSize(style, parentSize, static_cast<char>(218));
        break;
    case Sum:
        calcCharSize(style, parentSize, static_cast<char>(229));
        break;
    case Product:
        calcCharSize(style, parentSize, static_cast<char>(213));
        break;
    case Arrow:
        setWidth(2000);
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
        //drawLeftSquareBracket(painter, style, myX, myY, getHeight());
        drawCharacter(painter, style, myX, myY, '[');
        break;
    case RightSquareBracket:
        //drawRightSquareBracket(painter, style, myX, myY, getHeight());
        drawCharacter(painter, style, myX, myY, ']');
        break;
    case LeftCurlyBracket:
        //drawLeftCurlyBracket(painter, style, myX, myY, getHeight());
        if (!doSimpleCurlyBracket(style, parentSize)) {
            drawLeftCurlyBracket(painter, style, myX, myY, parentSize);
        }
        else {
            drawCharacter(painter, style, myX, myY, '{');
        }
        break;
    case RightCurlyBracket:
        //drawRightCurlyBracket(painter, style, myX, myY, getHeight());
        if (!doSimpleCurlyBracket(style, parentSize)) {
            drawRightCurlyBracket(painter, style, myX, myY, parentSize);
        }
        else {
            drawCharacter(painter, style, myX, myY, '}');
        }
        break;
    case LineBracket:
        //drawLineBracket(painter, style, myX, myY, getHeight());
        drawCharacter(painter, style, myX, myY, '|');
        break;
    case SlashBracket:
        drawCharacter(painter, style, myX, myY, '/');
        break;
    case BackSlashBracket:
        drawCharacter(painter, style, myX, myY, '\\');
        break;
    case LeftCornerBracket:
        //drawLeftCornerBracket(painter, style, myX, myY, getHeight());
        drawCharacter(painter, style, myX, myY, static_cast<char>(183));
        break;
    case RightCornerBracket:
        //drawRightCornerBracket(painter, style, myX, myY, getHeight());
        drawCharacter(painter, style, myX, myY, static_cast<char>(210));
        break;
    case LeftRoundBracket:
        //drawLeftRoundBracket(painter, style, myX, myY, getHeight());
        if (!doSimpleRoundBracket(style, parentSize)) {
            drawLeftRoundBracket(painter, style, myX, myY, parentSize);
        }
        else {
            drawCharacter(painter, style, myX, myY, '(');
        }
        break;
    case RightRoundBracket:
        //drawRightRoundBracket(painter, style, myX, myY, getHeight());
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
        drawCharacter(painter, style, myX, myY, static_cast<char>(218));
        break;
    case Sum:
        drawCharacter(painter, style, myX, myY, static_cast<char>(229));
        break;
    case Product:
        drawCharacter(painter, style, myX, myY, static_cast<char>(213));
        break;
    case Arrow:
        drawArrow(painter, myX, myY+getHeight()/2, getHeight());
        break;
    }

    // debug
    //painter.setBrush(Qt::NoBrush);
    //painter.setPen(Qt::green);
    //painter.drawRect(myX, myY, getWidth(), getHeight());
}


void Artwork::drawIntegral(QPainter& painter, const ContextStyle& style, int x, int y, int size)
{
    QPointArray a;
    //size = size * 4 / 3;
    a.setPoints(12,
		size / 4 - size / 16 + size / 24, 0,                     // 1
		size / 4 + size / 16 + size / 24, -size + size / 16,     // 2
		size / 2 - size / 16 + size / 24, -size,                 // 3
		size / 2 + size / 24 + size / 24, -size + size / 8,      // 4
		size / 2 - size / 16 + size / 24, -size + size / 16,     // 5
		size / 4 + size / 12 + size / 24, -size + size / 8,      // 6
		size / 4 + size / 16 + size / 24, 0,                     // 7
		size / 4 - size / 16 + size / 24, size - size / 16,      // 8
		size / 16 + size / 24, size,                             // 9
		0, size - size / 8,                                      // 10
		size / 16 + size / 24, size - size / 16,                 // 11
		size / 4 - size / 12 + size / 24, size - size / 8);      // 12
    a.translate(x, y);
    painter.setBrush(style.getDefaultColor());
    painter.drawPolygon(a);
}

void Artwork::drawCharacter(QPainter& painter, const ContextStyle& style, int x, int y, QChar ch)
{
    QFont f = style.getSymbolFont();
    f.setPointSize(fontSize);

    QFontMetrics fm(f);
    QRect bound = fm.boundingRect(ch);
    painter.setFont(f);
    painter.drawText(x-bound.x(), y-bound.top(), QString(ch));
    //painter.drawRect(bound);
    //painter.drawRect(x, y, getWidth(), getHeight());
    //cerr << bound.x() << " " << bound.y() << " " << bound.width() << " " << bound.height() << endl;
}

void Artwork::drawArrow(QPainter& painter, int x, int y, int size)
{
    painter.setPen(QPen(Qt::black, size / 8));
    painter.drawLine(x, y, x + size, y);
    painter.drawLine(x + size - size / 16, y,
                     x + size - size / 16 - size / 4, y - size / 4);
    painter.drawLine(x + size - size / 16, y,
                     x + size - size / 16 - size / 4, y + size / 4);
    painter.setPen(QPen(Qt::black, 1));
}


void Artwork::calcRoundBracket(const ContextStyle& style, int height)
{
    int charHeight;
    char uppercorner;
    char lowercorner;
    
    if (height <= style.zoomItY(80)) {
        charHeight = height/2;
        uppercorner = static_cast<char>(230);
        lowercorner = static_cast<char>(232);
    }
    else {
        charHeight = style.zoomItY(40);
        uppercorner = static_cast<char>(236);
        lowercorner = static_cast<char>(238);
        //line = static_cast<char>(134);
    }
    
    QFont f = style.getSymbolFont();
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
        drawLeftSmallRoundBracket(p, style, x, y, height/2);
    }
    else {
        drawLeftBigRoundBracket(p, style, x, y, style.zoomItY(40), height);
    }
}

void Artwork::drawRightRoundBracket(QPainter& p, const ContextStyle& style, int x, int y, int height)
{
    if (height <= style.zoomItY(80)) {
        drawRightSmallRoundBracket(p, style, x, y, height/2);
    }
    else {
        drawRightBigRoundBracket(p, style, x, y, style.zoomItY(40), height);
    }
}

void Artwork::drawLeftSmallRoundBracket(QPainter& p, const ContextStyle& style, int x, int y, int charHeight, bool left)
{
    QFont f = style.getSymbolFont();
    f.setPointSize(charHeight);
    p.setFont(f);

    char uppercorner = static_cast<char>(230);
    char lowercorner = static_cast<char>(232);
    
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

void Artwork::drawRightSmallRoundBracket(QPainter& p, const ContextStyle& style, int x, int y, int charHeight)
{
    p.save();
    p.setWorldMatrix(QWMatrix(-1, 0, 0, -1, 2*x, 2*y), true);
    drawLeftSmallRoundBracket(p, style, x, y, charHeight, false);
    p.restore();
}

void Artwork::drawLeftBigRoundBracket(QPainter& p, const ContextStyle& style, int x, int y, int charHeight, int height, bool left)
{
    QFont f = style.getSymbolFont();
    f.setPointSize(charHeight);
    p.setFont(f);

    char uppercorner = static_cast<char>(236);
    char lowercorner = static_cast<char>(238);
    char line = static_cast<char>(134);
    
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

    //p.setPen(Qt::black);
    p.drawText(x-upperBound.left(), y-upperBound.top(), QString(QChar(uppercorner)));
    p.drawText(x-lowerBound.left(), y+height-lowerBound.top()-lowerBound.height(), QString(QChar(lowercorner)));

    int gap = height - upperBound.height() - lowerBound.height();
    int lineCount = gap / lineBound.height() + 1;
    int start = (height - lineCount*lineBound.height()) / 2;
    
    for (int i = 0; i < lineCount; i++) {
        p.drawText(x-lineBound.left(), y-lineBound.top()+start+i*lineBound.height(), QString(QChar(line)));
    }
}

void Artwork::drawRightBigRoundBracket(QPainter& p, const ContextStyle& style, int x, int y, int charHeight, int height)
{
    p.save();
    p.setWorldMatrix(QWMatrix(-1, 0, 0, -1, 2*x, 2*y), true);
    drawLeftBigRoundBracket(p, style, x, y, charHeight, height, false);
    p.restore();
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
        uppercorner = static_cast<char>(230);
        lowercorner = static_cast<char>(232);
        line = static_cast<char>(234);
        middle = static_cast<char>(237);
    }
    else {
        charHeight = style.zoomItY(40);
        uppercorner = static_cast<char>(236);
        lowercorner = static_cast<char>(238);
        line = static_cast<char>(134);
        middle = static_cast<char>(237);
    }
    
    QFont f = style.getSymbolFont();
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
        drawLeftSmallCurlyBracket(p, style, x, y, height/3);
    }
    else {
        drawLeftBigCurlyBracket(p, style, x, y, style.zoomItY(40), height);
    }
}

void Artwork::drawRightCurlyBracket(QPainter& p, const ContextStyle& style, int x, int y, int height)
{
    if (height <= style.zoomItY(120)) {
        drawRightSmallCurlyBracket(p, style, x, y, height/3);
    }
    else {
        drawRightBigCurlyBracket(p, style, x, y, style.zoomItY(40), height);
    }
}

void Artwork::drawLeftSmallCurlyBracket(QPainter& p, const ContextStyle& style, int x, int y, int charHeight, bool left)
{
    QFont f = style.getSymbolFont();
    f.setPointSize(charHeight);
    p.setFont(f);
    //p.setPen(Qt::black);

    char uppercorner = static_cast<char>(230);
    char lowercorner = static_cast<char>(232);
    char line = static_cast<char>(234);
    char middle = static_cast<char>(237);
    
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

void Artwork::drawRightSmallCurlyBracket(QPainter& p, const ContextStyle& style, int x, int y, int charHeight)
{
    p.save();
    p.setWorldMatrix(QWMatrix(-1, 0, 0, -1, 2*x, 2*y), true);
    drawLeftSmallCurlyBracket(p, style, x, y, charHeight, false);
    p.restore();
}

void Artwork::drawLeftBigCurlyBracket(QPainter& p, const ContextStyle& style, int x, int y, int charHeight, int height, bool left)
{
    QFont f = style.getSymbolFont();
    f.setPointSize(charHeight);
    p.setFont(f);
    //p.setPen(Qt::black);

    char uppercorner = static_cast<char>(236);
    char lowercorner = static_cast<char>(238);
    char line = static_cast<char>(134);
    char middle = static_cast<char>(237);
    
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

    //p.setPen(Qt::black);
    p.drawText(x-upperBound.left()+offset, y-upperBound.top(), QString(QChar(uppercorner)));
    p.drawText(x-middleBound.left(), y+(height-middleBound.height())/2-middleBound.top(), QString(QChar(middle)));
    p.drawText(x-lowerBound.left()+offset, y+height-lowerBound.top()-lowerBound.height(), QString(QChar(lowercorner)));

    int gap = height/2 - upperBound.height() - middleBound.height() / 2;
    if (gap > 0) {
        int lineCount = gap / lineBound.height() + 1;

        int start = (height - middleBound.height()) / 2;
        for (int i = 0; i < lineCount; i++) {
            p.drawText(x-lineBound.left()+offset,
                       y-lineBound.top()+QMAX(start-(i+1)*lineBound.height(), upperBound.width()),
                       QString(QChar(line)));
        }
        
        start = (height + middleBound.height()) / 2;
        for (int i = 0; i < lineCount; i++) {
            p.drawText(x-lineBound.left()+offset,
                       y-lineBound.top()+QMIN(start+i*lineBound.height(), height-upperBound.width()-lineBound.height()),
                       QString(QChar(line)));
        }
    }
}

void Artwork::drawRightBigCurlyBracket(QPainter& p, const ContextStyle& style, int x, int y, int charHeight, int height)
{
    p.save();
    p.setWorldMatrix(QWMatrix(-1, 0, 0, -1, 2*x, 2*y), true);
    drawLeftBigCurlyBracket(p, style, x, y, charHeight, height, false);
    p.restore();
}
