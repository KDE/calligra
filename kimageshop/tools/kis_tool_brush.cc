/*
 *  kis_tool_brush.cc - part of KImageShop
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
#include "kis_tool_brush.h"
#include "kis_brush.h"
#include "kis_doc.h"
#include "kis_view.h"
#include "kis_vec.h"
#include "kis_cursor.h"
#include "kis_util.h"
#include "kapp.h"

BrushTool::BrushTool(KisDoc *doc, KisView *view, const KisBrush *_brush)
  : KisTool(doc, view)
{
    m_dragging = false;
    m_Cursor = KisCursor::brushCursor();
    m_pBrush = _brush;
    m_dragdist = 0;
}

BrushTool::~BrushTool() {}


void BrushTool::setBrush(const KisBrush *_brush)
{
    m_pBrush = _brush;
}


void BrushTool::mousePress(QMouseEvent *e)
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

    paintMonochrome(e->pos());
         m_pDoc->current()->markDirty(QRect(e->pos() - m_pBrush->hotSpot(), m_pBrush->size()));  

#if 0
    QPixmap *brushPM = m_pBrush->m_pPixmap;

    if(paintColor(e->pos()))
    {
        int left    =  m_dragStart.x() - brushPM->width()/2;
        int top     =  m_dragStart.y() - brushPM->height()/2;
        int wid     =  brushPM->width();
        int hgt     =  brushPM->height();

        QRect updateRect(left, top, wid, hgt);
        m_pView->updateCanvas( updateRect);        
    }
#endif
        
}


bool BrushTool::paintColor(QPoint pos)
{
#if 0
    KisImage * img = m_pDoc->current();
    if (!img)	        return false;    

    KisLayer *lay = img->getCurrentLayer();
    if (!lay)        return false;

    if (!m_pBrush)      return false;

    if (!img->colorMode() == cm_RGB && !img->colorMode() == cm_RGBA)
	return false;

    int startx = (pos - m_pBrush->hotSpot()).x();
    int starty = (pos - m_pBrush->hotSpot()).y();
    
    QPixmap *imPM = m_pDoc->current()->imagePixmap;
    QPixmap *brushPM = m_pBrush->m_pPixmap;
    
    bitBlt(imPM, startx, starty, brushPM , 0, 0, 
            brushPM->width() , brushPM->height() );    
#endif        
    return true;
}


bool BrushTool::paintMonochrome(QPoint pos)
{

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

    int red = m_pView->fgColor().R();
    int green = m_pView->fgColor().G();
    int blue = m_pView->fgColor().B();

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

    return true;
}



void BrushTool::mouseMove(QMouseEvent *e)
{
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
	    m_dragdist = 0; // reset
	  
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

#if 0
            if (paintColor(p))
            {
                QPixmap *brushPM = m_pBrush->m_pPixmap;            
                int left    =  p.x() - brushPM->width()/2;
                int top     =  p.y() - brushPM->height()/2;
                int wid     =  brushPM->width();
                int hgt     =  brushPM->height();

                QRect updateRect(left, top, wid, hgt);
                m_pView->updateCanvas( updateRect);                        
            }
#endif
            
	    dist -= spacing;
	}
	  
        if (dist > 0) m_dragdist = dist; //save for next moveevent
        m_dragStart = e->pos();
    }
}


void BrushTool::mouseRelease(QMouseEvent *e)
{
    if (e->button() != LeftButton)
        return;
        
    m_dragging = false;
}


