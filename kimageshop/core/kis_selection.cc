/*
 *  kis_gradient.cc - part of KImageShop
 *
 *  Copyright (c) 1999 Michael Koch <koch@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#include <qcolor.h>
#include <qclipboard.h>
#include <kapp.h>
#include <kdebug.h>

#include "kis_doc.h"
#include "kis_view.h"
#include "kis_vec.h"
#include "kis_cursor.h"
#include "kis_util.h"
#include "kis_selection.h"


KisSelection::KisSelection(KisDoc *doc)
{
    pDoc  = doc;
}

KisSelection::~KisSelection()
{
}

void KisSelection::setRect(QRect & rect)
{
    selectRect.setLeft(rect.left());
    selectRect.setTop(rect.top());
    selectRect.setRight(rect.right());
    selectRect.setBottom(rect.bottom());    
}


void KisSelection::setNull()
{
    selectRect.setWidth(0);
    selectRect.setHeight(0);
}


void KisSelection::setImage(QImage & img)
{
    selectImage = img;
}


/*
    erase a selection rectange within a the current layer
*/
bool KisSelection::erase()
{
    KisImage *img = pDoc->current();
    KisLayer *lay = img->getCurrentLayer();

    if (!img)	        return false;
    if (!lay)           return false;
    
    // FIXME: Implement this for non-RGB modes.
    if (!img->colorMode() == cm_RGB && !img->colorMode() == cm_RGBA)
    {
        kdDebug(0) << "colormode is not RGB or RGBA!" << endl;
	    return false;
    }

    // selectionRect = pDoc->getSelectRect();    

    QRect clipRect = selectRect;
    
    if (!clipRect.intersects(img->getCurrentLayer()->imageExtents()))
        return false;
  
    clipRect = clipRect.intersect(img->getCurrentLayer()->imageExtents());

    int sx = clipRect.left();
    int sy = clipRect.top(); 
    int ex = clipRect.right();
    int ey = clipRect.bottom();

    uchar r, g, b, a;

    bool alpha = (img->colorMode() == cm_RGBA);
      
    for (int y = sy; y <= ey; y++)
    {
        for (int x = sx; x <= ex; x++)
	    {
            // destination binary values by channel
	        r = lay->pixel(0, x,  y);
	        g = lay->pixel(1, x,  y);
	        b = lay->pixel(2, x,  y);
            
	        lay->setPixel(0, x, y, 255);
	        lay->setPixel(1, x, y, 255);
	        lay->setPixel(2, x, y, 255);
                       	  
            if (alpha)
	        {
	            a = lay->pixel(3, x, y);
		        lay->setPixel(3, x, y, a);
	        }
	    } 
    }

    return true;
}

#include "kis_selection.moc"

