/***************************************************************************
                          clabel.h  -  description
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
#ifndef CLABEL_H
#define CLABEL_H

#include "creportitem.h"
#include "canvdefs.h"

class CanvasLabel: public CanvasReportItem{
public:
    CanvasLabel(int x, int y, int width, int height, QCanvas * canvas);
    virtual int rtti() const { return RttiCanvasLabel; }
    virtual QString getXml();
    
    int getTextAlignment();
    int getTextWrap();
    
    QFont getFont();
    QPen getPenForText();
    QPen getPenForShape();
    QBrush getBrush();
    
    virtual void draw(QPainter &painter);
    virtual void updateGeomProps()
    {
	CanvasReportItem::updateGeomProps();
    }
};

#endif
