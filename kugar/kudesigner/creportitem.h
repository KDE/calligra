/***************************************************************************
                          creportitem.h  -  description
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
#ifndef CREPORTITEM_H
#define CREPORTITEM_H

#include "canvdefs.h"
#include "canvbox.h"

class QString;
class QStringList;

class CanvasReportItem: public CanvasBox{
public:
    CanvasReportItem(int x, int y, int width, int height, QCanvas * canvas):
	    CanvasBox(x, y, width, height, canvas)
    {
	setZ(20);
	parentSection = 0;
    }
    virtual int rtti() const { return RttiCanvasReportItem; }
    
    virtual void updateGeomProps();
    virtual void draw(QPainter &painter) { CanvasBox::draw(painter); }
    virtual QString getXml();
    
    QRect topLeftResizableRect();
    QRect bottomLeftResizableRect();
    QRect topRightResizableRect();
    QRect bottomRightResizableRect();
    
    CanvasBand *section();
    void setSection(CanvasBand *section);
    void setSectionUndestructive(CanvasBand *section);
    
protected:
    CanvasBand *parentSection;
};

#endif
