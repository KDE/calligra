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
    setHeight(parentSize);
    switch (type) {
    case LeftSquareBracket:
    case RightSquareBracket:
    case LeftCurlyBracket:
    case RightCurlyBracket:
    case LineBracket:
    case LeftCornerBracket:
    case RightCornerBracket:
    case LeftRoundBracket:
    case RightRoundBracket:
    case Empty:
        setWidth(8);
        break;
    case Integral:
        setWidth(7.0/12/2*parentSize);
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
        drawLeftSquareBracket(painter, style, myX, myY, getHeight());
        break;
    case RightSquareBracket:
        drawRightSquareBracket(painter, style, myX, myY, getHeight());
        break;
    case LeftCurlyBracket:
        drawLeftCurlyBracket(painter, style, myX, myY, getHeight());
        break;
    case RightCurlyBracket:
        drawRightCurlyBracket(painter, style, myX, myY, getHeight());
        break;
    case LineBracket:
        drawLineBracket(painter, style, myX, myY, getHeight());
        break;
    case LeftCornerBracket:
        drawLeftCornerBracket(painter, style, myX, myY, getHeight());
        break;
    case RightCornerBracket:
        drawRightCornerBracket(painter, style, myX, myY, getHeight());
        break;
    case LeftRoundBracket:
        drawLeftRoundBracket(painter, style, myX, myY, getHeight());
        break;
    case RightRoundBracket:
        drawRightRoundBracket(painter, style, myX, myY, getHeight());
        break;
    case Empty:
        drawEmpty(painter, myX, myY, getHeight());
        break;
    case Integral:
        drawIntegral(painter, style, myX, myY+getHeight()/2, getHeight()/2);
        break;
    case Sum:
        drawSum(painter, style, myX, myY);
        break;
    case Product:
        drawProduct(painter, style, myX, myY);
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

void Artwork::drawSum(QPainter& painter, const ContextStyle& style, int x, int y)
{
    QFont f = style.getSymbolFont();
    f.setPointSize(fontSize);

    QFontMetrics fm(f);
    QRect bound = fm.boundingRect('S');
    painter.setFont(f);
    painter.drawText(x-bound.x(), y+getHeight(), QString(QChar('S')));
    //painter.drawRect(bound);
    //cerr << bound.x() << " " << bound.y() << " " << bound.width() << " " << bound.height() << endl;
}

void Artwork::drawProduct(QPainter& painter, const ContextStyle& style, int x, int y)
{
    QFont f = style.getSymbolFont();
    f.setPointSize(fontSize);

    QFontMetrics fm(f);
    QRect bound = fm.boundingRect('P');
    painter.setFont(f);
    painter.drawText(x-bound.x(), y+getHeight(), QString(QChar('P')));
    //painter.drawRect(bound);
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


void Artwork::drawLeftSquareBracket(QPainter& painter, const ContextStyle& style, int x, int y, int size)
{
    int width = getWidth()-2;
    int thickness = width/4+1;
    //int unit = width - thickness;
        
    painter.setBrush(style.getDefaultColor());
    painter.setPen(Qt::NoPen);
    QPointArray points(10);
    
    points.setPoint(1, x, y);
    points.setPoint(2, x + width, y);
    points.setPoint(3, x + width, y + thickness);
    points.setPoint(4, x + thickness, y + thickness);
    points.setPoint(5, x + thickness, y + getHeight() - thickness);
    points.setPoint(6, x + width, y + getHeight() - thickness);
    points.setPoint(7, x + width, y+getHeight());
    points.setPoint(8, x, y+getHeight());

    painter.drawPolygon(points, false, 1, 8);
}

void Artwork::drawRightSquareBracket(QPainter& painter, const ContextStyle& style, int x, int y, int size)
{
    int width = getWidth()-2;
    int thickness = width/4+1;
    int unit = width - thickness;
        
    painter.setBrush(style.getDefaultColor());
    painter.setPen(Qt::NoPen);
    QPointArray points(10);
    
    points.setPoint(1, x, y);
    points.setPoint(2, x + width, y);
    points.setPoint(3, x + width, y + getHeight());
    points.setPoint(4, x, y + getHeight());
    points.setPoint(5, x, y + getHeight() - thickness);
    points.setPoint(6, x + unit, y + getHeight() - thickness);
    points.setPoint(7, x + unit, y + thickness);
    points.setPoint(8, x, y + thickness);

    painter.drawPolygon(points, false, 1, 8);
}
    
void Artwork::drawLeftCurlyBracket(QPainter& painter, const ContextStyle& style, int x, int y, int size)
{
}

void Artwork::drawRightCurlyBracket(QPainter& painter, const ContextStyle& style, int x, int y, int size)
{
}

void Artwork::drawLineBracket(QPainter& painter, const ContextStyle& style, int x, int y, int size)
{
    int width = getWidth()-2;
    int thickness = width/4+1;
    int unit = width - thickness;
        
    painter.setBrush(style.getDefaultColor());
    painter.setPen(Qt::NoPen);
    QPointArray points(5);
    
    points.setPoint(1, x+unit/2, y);
    points.setPoint(2, x+unit/2+thickness, y);
    points.setPoint(3, x+unit/2+thickness, y+getHeight());
    points.setPoint(4, x+unit/2, y+getHeight());
    
    painter.drawPolygon(points, false, 1, 4);
}

void Artwork::drawLeftCornerBracket(QPainter& painter, const ContextStyle& style, int x, int y, int size)
{
    int width = getWidth()-2;
    int thickness = width/4+1;
    int unit = width - thickness;
        
    painter.setBrush(style.getDefaultColor());
    painter.setPen(Qt::NoPen);
    QPointArray points(5);
    
    points.setPoint(1, x, y);
    points.setPoint(2, x+unit, y + getHeight()/2);
    points.setPoint(3, x, y+getHeight());
    //points.setPoint(4, x, y+getHeight());

    painter.drawPolyline(points, 1, /*4*/3);
}

void Artwork::drawRightCornerBracket(QPainter& painter, const ContextStyle& style, int x, int y, int size)
{
    int width = getWidth()-2;
    int thickness = width/4+1;
    int unit = width - thickness;
        
    painter.setBrush(style.getDefaultColor());
    painter.setPen(Qt::NoPen);
    QPointArray points(5);
    
    points.setPoint(1, x+unit, y);
    points.setPoint(2, x, y + getHeight()/2);
    points.setPoint(3, x+unit, y+getHeight());
    //points.setPoint(4, x, y+getHeight());

    painter.drawPolyline(points, 1, /*4*/3);
}

void Artwork::drawLeftRoundBracket(QPainter& painter, const ContextStyle& style, int x, int y, int size)
{
    int thickness = getWidth()/4+1;
    
    //painter.setBrush(style.getDefaultColor());
    painter.setPen(QPen(style.getDefaultColor(), 2));
    
    painter.drawArc(x+thickness, y, getWidth(), getHeight(), 90*16, 180*16);
}

void Artwork::drawRightRoundBracket(QPainter& painter, const ContextStyle& style, int x, int y, int size)
{
    int thickness = getWidth()/4+1;
    
    //painter.setBrush(style.getDefaultColor());
    painter.setPen(QPen(style.getDefaultColor(), 2));
    
    painter.drawArc(x-thickness, y, getWidth(), getHeight(), 270*16, 180*16);
}

void Artwork::drawEmpty(QPainter&, int, int, int)
{
}
