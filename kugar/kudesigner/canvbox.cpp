/***************************************************************************
                          canvbox.cpp  -  description
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
#include <klocale.h>
#include <kglobalsettings.h>

#include <qcanvas.h>

#include "canvbox.h"
#include "mycanvas.h"

CanvasBox::~CanvasBox()
{
}

void CanvasBox::registerAs(int type)
{
    if (((MyCanvas*)canvas())->document()->plugin()) {
        (((MyCanvas*)canvas())->document()->plugin())->newCanvasBox(type,this);
    }
}

void CanvasBox::scale(int scale)
{
    setSize(width()*scale, height()*scale);
}

void CanvasBox::draw(QPainter &painter)
{
    QCanvasRectangle::draw(painter);
}

