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


// I'm not happy with the artwork in here. But at least
// there is some...


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
        calcCharSize(style, parentSize, '<');
        break;
    case RightCornerBracket:
        calcCharSize(style, parentSize, '>');
        break;
    case LeftRoundBracket:
        calcCharSize(style, parentSize, '(');
        break;
    case RightRoundBracket:
        calcCharSize(style, parentSize, ')');
        break;
    case Empty:
        setHeight(parentSize);
        setWidth(style.getEmptyRectWidth());
        break;
    case LeftCurlyBracket:
        calcCharSize(style, parentSize, '{');
        break;
    case RightCurlyBracket:
        calcCharSize(style, parentSize, '}');
        break;
    case Integral:
        //setHeight(parentSize);
        //setWidth(7.0/12/2*parentSize);
        calcCharSize(style, parentSize, static_cast<char>(218));
        break;
    case Sum:
        calcCharSize(style, parentSize, 'S');
        break;
    case Product:
        calcCharSize(style, parentSize, 'P');
        break;
    case Arrow:
        setWidth(2000);
        break;
    }
}

void Artwork::draw(QPainter& painter, const ContextStyle& style,
                   int parentSize, const QPoint& origin)
{
    int myX = origin.x() + getX();
    int myY = origin.y() + getY();

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
        drawCharacter(painter, style, myX, myY, '{');
        break;
    case RightCurlyBracket:
        //drawRightCurlyBracket(painter, style, myX, myY, getHeight());
        drawCharacter(painter, style, myX, myY, '}');
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
        drawCharacter(painter, style, myX, myY, '<');
        break;
    case RightCornerBracket:
        //drawRightCornerBracket(painter, style, myX, myY, getHeight());
        drawCharacter(painter, style, myX, myY, '>');
        break;
    case LeftRoundBracket:
        //drawLeftRoundBracket(painter, style, myX, myY, getHeight());
        drawCharacter(painter, style, myX, myY, '(');
        break;
    case RightRoundBracket:
        //drawRightRoundBracket(painter, style, myX, myY, getHeight());
        drawCharacter(painter, style, myX, myY, ')');
        break;
    case Empty:
        //drawEmpty(painter, myX, myY, getHeight());
        break;
    case Integral:
        //drawIntegral(painter, style, myX, myY+getHeight()/2, getHeight()/2);
        drawCharacter(painter, style, myX, myY, static_cast<char>(218));
        break;
    case Sum:
        drawCharacter(painter, style, myX, myY, 'S');
        break;
    case Product:
        drawCharacter(painter, style, myX, myY, 'P');
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
