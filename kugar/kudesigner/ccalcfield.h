/***************************************************************************
                          calcfield.h  -  description
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
#ifndef CCALCFIELD_H
#define CCALCFIELD_H

#include "cfield.h"
#include "canvdefs.h"

class CanvasCalculatedField: public CanvasField{
public:
    CanvasCalculatedField(int x, int y, int width, int height, QCanvas * canvas);
    virtual int rtti() const { return RttiCanvasCalculated; }
    virtual QString getXml();
    virtual void draw(QPainter &painter);
    virtual void updateGeomProps()
    {
	CanvasField::updateGeomProps();
    }
};

#endif
