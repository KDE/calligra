/***************************************************************************
                          canvbox.h  -  description
                             -------------------
    begin                : 07.06.2002
    copyright            : (C) 2002 by Alexander Dymo
    email                : cloudtemple@mksat.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/
#ifndef CANVBOX_H
#define CANVBOX_H

#include <map>

#include <qcanvas.h>

#include "canvdefs.h"
#include "property.h"


class CanvasBox: public QCanvasRectangle{
public:
    CanvasBox(int x, int y, int width, int height, QCanvas * canvas):
        QCanvasRectangle(x, y, width, height, canvas)
    {
        setSelected(false);
    }
    virtual ~CanvasBox();

    virtual int rtti() const { return KuDesignerRttiCanvasBox; }
    virtual void draw(QPainter &painter);
    void scale(int scale);
    virtual QString getXml() { return ""; }

    enum ResizeEnum {ResizeNothing=0,ResizeLeft=1,ResizeTop=2,ResizeRight=4,ResizeBottom=8};
    
    virtual int isInHolder(const QPoint ) {return ResizeNothing;}
    virtual void drawHolders(QPainter &) {}

    virtual void updateGeomProps(){;}

    /**
      Property format
      map<QString, PropPtr >
    */
    std::map<QString, PropPtr > props;

protected:
    void registerAs(int type);
};

#endif
