/*
 *  kis_tool_paste.cc - part of Krayon
 *
 *  Copyright (c) 2000 John Califf <jcaliff@compuzone.net>
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
#include "kis_canvas.h"
#include "kis_vec.h"
#include "kis_cursor.h"
#include "kis_util.h"

PasteTool::PasteTool(KisDoc *doc, KisView *view, 
    KisCanvas *canvas, const KisSelection *_selection)
    : KisTool(doc, view)
{
    m_dragging = false;
    m_dragdist = 0;    
    m_pView = view;
    m_pCanvas = canvas;
        
    m_Cursor = KisCursor::crossCursor();
}

PasteTool::~PasteTool() {}


bool PasteTool::setClip()
{
    clipImage = kapp->clipboard()->image();
    
    if (clipImage.isNull()) 
    {
        kdDebug(0) << "PasteTool:: clipboard image is null!" << endl;
        return false;
    }    
    else
    {
        kdDebug(0) << "PasteTool:: clipboard image is NOT null!" << endl;
    }

    /* if dealing with 1 or 8 bit images, convert to 16 bit */
    if(clipImage.depth() < 16)
    {
        QImage sI = clipImage.smoothScale(clipImage.width(), clipImage.height());
        clipImage = sI;
        
        if(clipImage.isNull())
        {
            kdDebug(0) << "PasteTool:: can't smooth scale clip image!" << endl;
            return false;
        }    
    }
    
    clipPix.convertFromImage(clipImage, QPixmap::AutoColor);
    if(clipPix.isNull())
    {
        kdDebug(0) << "PasteTool:: can't convernt from image!" << endl;    
        return false;
    }
            
    // use this to establish clip size and the
    // "hot spot" in center of image, will be the
    // same for all clips, no need to vary it.
    
    clipWidth = clipPix.width();
    clipHeight = clipPix.height();
    mClipSize = QSize(clipWidth, clipHeight);
    mHotSpotX = 0;
    mHotSpotY = 0;
    mHotSpot = QPoint(mHotSpotX, mHotSpotY);
    
    return true;
}


void PasteTool::setOpacity(int /*opacity*/)
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

    QPoint pos = e->pos();
    pos = zoomed(pos);
    
    m_dragStart = pos;
    m_dragdist = 0;

    pasteColor(pos);
    
    m_pDoc->current()->markDirty(QRect(0, 0, 
        m_pDoc->current()->width(), m_pDoc->current()->height()));      

    /* m_pDoc->current()->markDirty(QRect(e->pos() - mHotSpot, 
         m_pBrush->size())); */ 
}


bool PasteTool::pasteColor(QPoint pos)
{
    KisImage *img = m_pDoc->current();
    if (!img)   return false;    

    KisLayer *lay = img->getCurrentLayer();
    if (!lay)   return false;
    
    QImage *qimg = &clipImage;
    
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
    int   v = 255;
    int   bv = 0;
    
    //int red     = m_pView->fgColor().R();
    //int green   = m_pView->fgColor().G();
    //int blue    = m_pView->fgColor().B();

    bool grayscale = false;
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
            
            // set layer pixel to be same as image
	        lay->setPixel(0, startx + x, starty + y, qRed(*p));
	        lay->setPixel(1, startx + x, starty + y, qGreen(*p));
	        lay->setPixel(2, startx + x, starty + y, qBlue(*p));
                       	  
            if (alpha)
	        {
	            a = lay->pixel(3, startx + x, starty + y);
                if(grayscale)
                {
                    v = a + bv;
		            if (v < 0 ) v = 0;
		            if (v > 255 ) v = 255;
		            a = (uchar) v; 
			    }
                
		        lay->setPixel(3, startx + x, starty + y, a);
	        }
	    } 
    }

    return true;
}


bool PasteTool::pasteMonochrome(QPoint /* pos */)
{
    return true;
}


/*
    Stamp to canvas - stamp the pattern only onto canvas -
    it will not affect the layer or image 
*/

bool PasteTool::pasteToCanvas(QPoint pos)
{
    KisImage* img = m_pDoc->current();
    if (!img) return false;

    QPainter p;
    p.begin(m_pCanvas);
    p.scale( m_pView->zoomFactor(), m_pView->zoomFactor() );

    QRect ur(pos.x(), pos.y(), clipPix.width(), clipPix.height());
    
#if 0
    ur.moveBy( - m_pView->xPaintOffset() + m_pView->xScrollOffset() , 
               - m_pView->yPaintOffset() + m_pView->yScrollOffset());
#endif

    ur = ur.intersect(img->imageExtents());
    ur.setBottom(ur.bottom()+1);
    ur.setRight(ur.right()+1);

    if (ur.top() - mHotSpotY > img->height()
    || ur.left() - mHotSpotX > img->width()
    || ur.bottom() - mHotSpotY < 0
    || ur.right() -  mHotSpotX < 0)
    {
        p.end();
        return false;
    }

    int xt = m_pView->xPaintOffset()- m_pView->xScrollOffset();
    int yt = m_pView->yPaintOffset()- m_pView->yScrollOffset();

    p.translate(xt, yt);

    p.drawPixmap( ur.left(), ur.top(), 
                  clipPix, 
                  0, 0, ur.width(), ur.height() );
    p.end();

    // Alternate bitBlt method, needs resarching.  This may be better
    // than QPainter because it allows raster operations for neat effects.
    
    /*
    bitBlt (m_pCanvas, 
        pos.x() + m_pView->xPaintOffset() - m_pView->xScrollOffset(), 
        pos.y() + m_pView->yPaintOffset() - m_pView->yScrollOffset(), 
        m_pPattern->pixmap(), 0, 0, 
        m_pPattern->width(), m_pPattern->height());    
    */
    
    return true;
}


void PasteTool::mouseMove(QMouseEvent *e)
{
    KisImage * img = m_pDoc->current();
    if (!img) return;

    int spacing = 10;
    
    if(true)
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
	        m_dragdist += new_dist; 
	        m_dragStart = e->pos();
	        return;
	    }
        else
        {
	        m_dragdist = 0; 
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
            
	        if(m_dragging)
            {
                /* mouse button is down. Actually draw the 
                image into the layer so long as spacing is 
                less than distance moved */

                if (pasteColor(p - mHotSpot))
                {
		            img->markDirty(QRect(p - mHotSpot, clipPix.size()));
                }    
            }
            else
            {
                /* Button is not down. Refresh canvas from the layer
                and then blit the image to the canvas without affecting 
                the layer at all ! No need for double buffer!!!    
                Refresh first - markDirty relies on timer, 
                so we need force by directly updating the canvas. */
                
                QRect ur(oldp.x() - mHotSpotX - m_pView->xScrollOffset() -2, 
                         oldp.y() - mHotSpotY - m_pView->yScrollOffset() -2, 
                         clipPix.width()  + 2, 
                         clipPix.height() + 2);
                         
                m_pView->updateCanvas(ur);
                
                // after old spot is refreshed, stamp image into canvas
                // at current location. This may be slow or messy as updates
                // rely on a timer - need threads and semaphores here to let
                // us know when old marking has been replaced with image
                // if timer is used, but it's not used for this.
                
                if(!pasteToCanvas(p - mHotSpot))
                {
                    kdDebug(0) << "canvas error!" << endl;                
                }            
            }
            
	        oldp = p; 
            dist -= spacing; 
	    }
	  
        if (dist > 0) m_dragdist = dist; 
        m_dragStart = e->pos();
    }
}


void PasteTool::mouseRelease(QMouseEvent *e)
{
    if (e->button() != LeftButton)
        return;
        
    m_dragging = false;
}


