/***************************************************************************
                          mycanvas.h  -  description
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
#ifndef MYCANVAS_H
#define MYCANVAS_H

#include <qcanvas.h>
#include "canvbox.h"

class MyCanvas: public QCanvas{
public:
    MyCanvas(int w, int h): QCanvas(w, h) 
    {
	templ = 0;
    }
    ~MyCanvas()
    {
	delete templ;
/*	QCanvasItemList l = allItems();
	for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it)
	{
	    delete (*it);
	}*/
    }
    
    CanvasKugarTemplate *templ;
private:
    void scaleCanvas(int scale);
};

#endif
