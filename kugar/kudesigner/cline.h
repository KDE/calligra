/***************************************************************************
                          cline.h  -  description
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
#ifndef CLINE_H
#define CLINE_H

#include "canvdefs.h"
#include "creportitem.h"

class CanvasLine: public CanvasReportItem{
public:
    CanvasLine(int x, int y, int width, int height, QCanvas * canvas);
    virtual int rtti() const { return RttiCanvasLine; }
    virtual QString getXml();
    virtual void draw(QPainter &painter);
        
    virtual void updateGeomProps();
    virtual void setSection(CanvasBand *section);
private:
    QPen getPenForShape();
};

#endif
