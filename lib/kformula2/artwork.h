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

#ifndef __ARTWORK_H
#define __ARTWORK_H

#include <qpoint.h>
#include <qsize.h>

class ContextStyle;
class QPainter;


/**
 * A piece of art that may be used by any element.
 *
 * One special thing is that artwork doesn't calc its absoulute
 * size itself. It only calcs a relative size and you have to
 * scale it to fit your needs.
 */
class Artwork {
public:
    enum SymbolType { integral, sum, product, arrow };

    Artwork(SymbolType type);
    
    virtual void calcSizes(ContextStyle& style, int parentSize);
    virtual void draw(QPainter& painter, ContextStyle& style,
                      int parentSize, const QPoint& origin);
    
    int getWidth() const { return size.width(); }
    int getHeight() const { return size.height(); }

    int getX() const { return point.x(); }
    int getY() const { return point.y(); }
    
    void setX(int x) { point.setX(x); }
    void setY(int y) { point.setY(y); }

    /**
     * Multiplies width and height with the factor.
     */
    void scale(double factor);
    
protected:

    void setWidth(int width) { size.setWidth(width); }
    void setHeight(int height) { size.setHeight(height); }
    
    void drawIntegral(QPainter& painter, int x, int y, int size);
    void drawSum(QPainter& painter, int x, int y, int size);
    void drawProduct(QPainter& painter, int x, int y, int size);
    void drawArrow(QPainter& painter, int x, int y, int size);
    
    QSize size;
    QPoint point;

    SymbolType type;
};


#endif // __ARTWORK_H
