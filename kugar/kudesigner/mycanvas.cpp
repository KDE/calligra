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
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <qstringlist.h>
#include "mycanvas.h"
#include "canvbox.h"

void MyCanvas::scaleCanvas(int scale)
{
    resize(width()*scale, height()*scale);
    
    //scale all child items if they are textbox's
    QCanvasItemList l=this->allItems();
    for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it)
    {
	if ((*it)->rtti() == RttiTextBox)
	{
/*	    CanvasTextBox* b = (CanvasTextBox*)(*it);
	    b->scale(scale);*/
	}
    }
}
