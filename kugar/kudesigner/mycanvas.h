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
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/
#ifndef MYCANVAS_H
#define MYCANVAS_H

#include <qcanvas.h>
#include "canvbox.h"
#include "kudesigner_doc.h"

class MyCanvas: public QCanvas{
public:
    MyCanvas(int w, int h,KudesignerDoc *doc): QCanvas(w, h),m_doc(doc)
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
    
    KudesignerDoc *document(){return m_doc;}
    CanvasKugarTemplate *templ;
private:
    KudesignerDoc *m_doc;
    void scaleCanvas(int scale);
};

#endif
