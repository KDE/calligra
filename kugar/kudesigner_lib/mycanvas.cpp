/***************************************************************************
                          mycanvas.cpp  -  description
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
#include <qstringlist.h>

#include "mycanvas.h"
#include "canvbox.h"
#include "canvkutemplate.h"

MyCanvas::MyCanvas(int w, int h/*, KudesignerDoc *doc*/):
    QCanvas(w, h),m_plugin(0)//, m_doc(doc)
{
    selected.setAutoDelete(false);
    templ = 0;
}

MyCanvas::~MyCanvas()
{
    delete templ;
}

void MyCanvas::scaleCanvas(int scale)
{
    resize(width()*scale, height()*scale);

    //scale all child items if they are textbox's
    QCanvasItemList l=this->allItems();
    for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it)
    {
	if ((*it)->rtti() == KuDesignerRttiTextBox)
	{
/*	    CanvasTextBox* b = (CanvasTextBox*)(*it);
	    b->scale(scale);*/
	}
    }
}

void MyCanvas::drawForeground ( QPainter & painter, const QRect & clip )
{
	//kdDebug()<<"MyCanvas::drawForeGround"<<endl;
	for (CanvasBox *b=selected.first();b;b=selected.next())
	{
		b->drawHolders(painter);
	}
}

KuDesignerPlugin * MyCanvas::plugin( )
{
    return m_plugin;
}

void MyCanvas::setPlugin( KuDesignerPlugin * plugin )
{
    m_plugin = plugin;
}

