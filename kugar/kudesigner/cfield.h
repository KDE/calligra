/***************************************************************************
                          cfield.h  -  description
                             -------------------
    begin                : 07.06.2002
    copyright            : (C) 2002 by Alexander Dymo
    email                : cloudtemple@mksat.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef CFIELD_H
#define CFIELD_H

#include "canvdefs.h"
#include "clabel.h"

class CanvasField: public CanvasLabel{
public:
    CanvasField(int x, int y, int width, int height, QCanvas * canvas);
    virtual int rtti() const { return RttiCanvasField; }
    virtual QString getXml();
    virtual void draw(QPainter &painter);
    virtual void updateGeomProps()
    {
	CanvasLabel::updateGeomProps();
    }
};

#endif
