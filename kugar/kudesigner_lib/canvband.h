/* This file is part of the KDE project
   Copyright (C) 2003 Alexander Dymo <cloudtemple@mksat.net>

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
#ifndef CANVBAND_H
#define CANVBAND_H

#include "canvdefs.h"
#include "canvsection.h"

class QCanvas;
class QCanvasItemList;
class QPainter;
class QRect;
class QPoint;

class CanvasBand: public CanvasSection{
public:
    CanvasBand(int x, int y, int width, int height, QCanvas * canvas):
        CanvasSection(x, y, width, height, canvas)
    {
        setZ(10);
    }
    ~CanvasBand();
    virtual void draw(QPainter &painter);
    virtual int rtti() const { return KuDesignerRttiCanvasBand; }
    virtual QString getXml();
    virtual int isInHolder(const QPoint );
    virtual void drawHolders(QPainter &);
    int minHeight();
    QRect bottomMiddleResizableRect();
    void arrange(int base, bool destructive = TRUE);
    virtual void updateGeomProps();

    QCanvasItemList items;
};

#endif
