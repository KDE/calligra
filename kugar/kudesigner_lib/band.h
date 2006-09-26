/* This file is part of the KDE project
 Copyright (C) 2003-2004 Alexander Dymo <adymo@mksat.net>

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
 the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/
#ifndef BAND_H
#define BAND_H

#include "commdefs.h"
#include "section.h"

class Q3CanvasItemList;
class QPainter;
class QRect;
class QPoint;

namespace Kudesigner
{

class Band: public Section
{
public:
    Band( int x, int y, int width, int height, Canvas *canvas ) :
            Section( x, y, width, height, canvas )
    {
        setZ( 10 );
    }
    ~Band();

    virtual int rtti() const
    {
        return Rtti_Band;
    }
    virtual void draw( QPainter &painter );
    virtual QString getXml();
    virtual int isInHolder( const QPoint );
    virtual void drawHolders( QPainter & );

    int minHeight();
    QRect bottomMiddleResizableRect();
    void arrange( int base, bool destructive = true );
    virtual void updateGeomProps();

    Q3CanvasItemList items;
};

}

#endif
