/*
 *  kis_tool_paste.cc - part of KImageShop
 *
 *  Copyright (c) 1999 Matthias Elter <me@kde.org>
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

#include "kis_tool_paste.h"
#include "kis_doc.h"
#include "kis_view.h"
#include "kis_vec.h"
#include "kis_cursor.h"
#include "kis_util.h"

PasteTool::PasteTool(KisDoc *doc, KisView *view, const KisSelection *_selection)
  : KisTool(doc, view)
{
    m_dragging = false;
    m_dragdist = 0;    
    m_pView = view;
        
    // m_Cursor = KisCursor::pasteCursor();
}

PasteTool::~PasteTool() {}


void PasteTool::setOpacity(int opacity)
{

}

/*
    On mouse press, simple paste clip image at mouse coords
*/

void PasteTool::mousePress(QMouseEvent *e)
{
    if (e->button() != QMouseEvent::LeftButton)
        return;

    if(!m_pDoc->current())
        return;

    if(!m_pDoc->current()->getCurrentLayer()->visible() )
        return;

    m_dragging = true;
    m_dragStart = e->pos();
    m_dragdist = 0;

    pasteColor(e->pos());
    
    m_pDoc->current()->markDirty(QRect(0, 0, 
        m_pDoc->current()->width(), m_pDoc->current()->height()));      

    //m_pDoc->current()->markDirty(QRect(e->pos() - m_pBrush->hotSpot(), m_pBrush->size()));  
}


bool PasteTool::pasteColor(QPoint pos)
{
    KisImage *img = m_pDoc->current();
    KisLayer *lay = img->getCurrentLayer();
    if (!img)	        return false;
    if (!lay)           return false;
    
    //QImage   *qimg = m_pDoc->getClipImage();
    QImage qImage = kapp->clipboard()->image();
    if (qImage.isNull()) 
    {
        kdDebug(0) << "PasteTool::pasteColor() clipboard image is null!" << endl;
        return false;
    }    
    else
    {
        kdDebug(0) << "PasteTool::pasteColor() clipboard image is NOT null!" << endl;
    }

    QImage *qimg = &qImage;
    
    // FIXME: Implement this for non-RGB modes.
    if (!img->colorMode() == cm_RGB && !img->colorMode() == cm_RGBA)
    {
        kdDebug(0) << "colormode is not RGB or RGBA!" << endl;
	return false;
    }
    /* if dealing with 1 or 8 bit images, convert to 16 bit */
    if(qimg->depth() < 16)
    {
        QImage Converted = qimg->smoothScale(qimg->width(), 
            qimg->height());
        qimg = &Converted;
    }
    
    int startx = pos.x();
    int starty = pos.y();

    QRect clipRect(startx, starty, qimg->width(), qimg->height());

    if (!clipRect.intersects(img->getCurrentLayer()->imageExtents()))
        return false;
  
    clipRect = clipRect.intersect(img->getCurrentLayer()->imageExtents());

    int sx = clipRect.left() - startx;
    int sy = clipRect.top() - starty;
    int ex = clipRect.right() - startx;
    int ey = clipRect.bottom() - starty;

    uchar r, g, b, a;
    int   v;

    int red     = m_pView->fgColor().R();
    int green   = m_pView->fgColor().G();
    int blue    = m_pView->fgColor().B();

    bool alpha = (img->colorMode() == cm_RGBA);
  
    for (int y = sy; y <= ey; y++)
    {
        for (int x = sx; x <= ex; x++)
	    {
            // destination binary values by channel
	        r = lay->pixel(0, startx + x, starty + y);
	        g = lay->pixel(1, startx + x, starty + y);
	        b = lay->pixel(2, startx + x, starty + y);

            // pixel value in scanline at x offset to right
            uint *p = (uint *)qimg->scanLine(y) + x;
            
	        lay->setPixel(0, startx + x, starty + y, qRed(*p));
	        lay->setPixel(1, startx + x, starty + y, qGreen(*p));
	        lay->setPixel(2, startx + x, starty + y, qBlue(*p));
                       	  
            if (alpha)
	        {
	            a = lay->pixel(3, startx + x, starty + y);

                /* v = a + bv;
		        if (v < 0 ) v = 0;
		        if (v > 255 ) v = 255;
		        a = (uchar) v; */
			  
		        lay->setPixel(3, startx + x, starty + y, a);
	        }
	    } 
    }

    return true;
}


bool PasteTool::pasteMonochrome(QPoint pos)
{
#if 0
    KisImage * img = m_pDoc->current();
    KisLayer *lay = img->getCurrentLayer();

    if (!img)	        return false;
    if (!lay)           return false;
    if (!m_pBrush)      return false;

    // FIXME: Implement this for non-RGB modes.
    if (!img->colorMode() == cm_RGB && !img->colorMode() == cm_RGBA)
	return false;

    int startx = (pos - m_pBrush->hotSpot()).x();
    int starty = (pos - m_pBrush->hotSpot()).y();

    QRect clipRect(startx, starty, m_pBrush->width(), m_pBrush->height());

    if (!clipRect.intersects(img->getCurrentLayer()->imageExtents()))
        return false;
  
    clipRect = clipRect.intersect(img->getCurrentLayer()->imageExtents());

    int sx = clipRect.left() - startx;
    int sy = clipRect.top() - starty;
    int ex = clipRect.right() - startx;
    int ey = clipRect.bottom() - starty;

    uchar *sl;
    uchar bv, invbv;
    uchar r, g, b, a;
    int   v;

    int red     = fgColor().R();
    int green   = fgColor().G();
    int blue    = fgColor().B();

    bool alpha = (img->colorMode() == cm_RGBA);
  
    for (int y = sy; y <= ey; y++)
    {
        sl = m_pBrush->scanline(y);

        for (int x = sx; x <= ex; x++)
	{
	    r = lay->pixel(0, startx + x, starty + y);
	    g = lay->pixel(1, startx + x, starty + y);
	    b = lay->pixel(2, startx + x, starty + y);
		  
	    bv = *(sl + x);
	    if (bv == 0) continue;
		  
	    invbv = 255 - bv;
		  
            b = ((blue * bv) + (b * invbv))/255;
	    g = ((green * bv) + (g * invbv))/255;
	    r = ((red * bv) + (r * invbv))/255;
            		  
	    lay->setPixel(0, startx + x, starty + y, r);
	    lay->setPixel(1, startx + x, starty + y, g);
	    lay->setPixel(2, startx + x, starty + y, b);
                       	  
            if (alpha)
	    {
	        a= lay->pixel(3, startx + x, starty + y);

		v = a + bv;
		if (v < 0 ) v = 0;
		if (v > 255 ) v = 255;
		a = (uchar) v;
			  
		lay->setPixel(3, startx + x, starty + y, a);
	    }
	} 
    }
#endif

    return true;
}



void PasteTool::mouseMove(QMouseEvent *e)
{
#if 0
    KisImage * img = m_pDoc->current();

    if (!img) return;
    if ( m_pDoc->isEmpty() )  return;

    int spacing = m_pBrush->spacing();
    if (spacing <= 0) spacing = 1;

    if(m_dragging)
    {
        if( !img->getCurrentLayer()->visible() )
	    return;
	  
        KisVector end(e->x(), e->y());
        KisVector start(m_dragStart.x(), m_dragStart.y());
            
        KisVector dragVec = end - start;
        float saved_dist = m_dragdist;
        float new_dist = dragVec.length();
        float dist = saved_dist + new_dist;
	  
        if ((int)dist < spacing)
	{
	    m_dragdist += new_dist; // save for next moveevent
	    m_dragStart = e->pos();
	    return;
	}
        else
        {
	    m_dragdist = 0; // reset
	}
          
        dragVec.normalize();
        KisVector step = start;

        while (dist >= spacing)
	{
	    if (saved_dist > 0)
	    {
		  step += dragVec * (spacing-saved_dist);
		  saved_dist -= spacing;
	    }
	    else
		step += dragVec * spacing;
		  
	    QPoint p(step.x(), step.y());

	    if (paintMonochrome(p))
		img->markDirty(QRect(p - m_pBrush->hotSpot(), m_pBrush->size()));
            
	    dist -= spacing;
	}
	  
        if (dist > 0) m_dragdist = dist; //save for next moveevent
        m_dragStart = e->pos();
    }
#endif    
}


void PasteTool::mouseRelease(QMouseEvent *e)
{
    if (e->button() != LeftButton)
        return;
        
    m_dragging = false;
}


