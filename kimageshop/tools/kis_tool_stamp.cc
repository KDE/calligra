/*
 *  kis_tool_stamp.cc - part of Krayon
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

#include "kis_tool_stamp.h"
#include "kis_doc.h"
#include "kis_view.h"
#include "kis_vec.h"
#include "kis_cursor.h"
#include "kis_util.h"
#include "kis_pattern.h"


StampTool::StampTool(KisDoc *doc, KisView *view, 
    KisCanvas *canvas, const KisPattern *pattern)
  : KisTool(doc, view)
{
    m_dragging = false;
    m_dragdist = 0;    
    m_pView = view;
    m_pCanvas = canvas;
    
    setPattern(pattern);
}

StampTool::~StampTool() {}


void StampTool::setPattern(const KisPattern *pattern)
{
    m_pPattern = const_cast<KisPattern*>(pattern);    
    
    // use this to establish pattern size and the
    // "hot spot" in center of image, will be the
    // same for all stamps, no need to vary it.
    // when tiling patterns, use point 0,0 instead
    // these are simple variables for speed to avoid
    // copy constructors within loops
    
    patternWidth = m_pPattern->width();
    patternHeight = m_pPattern->height();
    mPatternSize = QSize(patternWidth, patternHeight);
    mHotSpotX = patternWidth/2;
    mHotSpotY = patternHeight/2;
    mHotSpot = QPoint(mHotSpotX, mHotSpotY);
    spacing = m_pPattern->spacing();
    if (spacing < 1) spacing = 3;

}


void StampTool::setOpacity(int /* opacity */)
{

}


/*
    On mouse press, the image is stamped or pasted 
    into the current layer
*/

void StampTool::mousePress(QMouseEvent *e)
{
    if (e->button() != QMouseEvent::LeftButton) return;

    // do sanity checking here, if possible, not inside loops
    // when moving mouse!
    KisImage *img = m_pDoc->current();
    if (!img)  return;    

    if (!img->colorMode() == cm_RGB && !img->colorMode() == cm_RGBA)
    {
        kdDebug(0) << "colormode is not RGB or RGBA!" << endl;
	    return;
    }

    KisLayer *lay = img->getCurrentLayer();
    if (!lay)  return;

    if(!lay->visible()) return;

    QImage qImage = *(m_pPattern->image());
    if(qImage.isNull()) 
    {
        kdDebug(0) << "Stamptool::no pattern image!" << endl;
        return;
    }    
    if(qImage.depth() < 16)
    {
        kdDebug(0) << "Stamptool::pattern less than 16 bit!" << endl;
        return;
    }    
    
    spacing = m_pPattern->spacing();
    if (spacing < 1) spacing = 3;

    m_dragging = true;

    QPoint pos = e->pos();
    pos = zoomed(pos);
    
    m_dragStart = pos;
    m_dragdist = 0;

    // stamp the pattern image into the layer memory
    if(stampColor(pos - mHotSpot))
    {
        img->markDirty(QRect(e->pos() - mHotSpot, mPatternSize));
    }
}


/*
    Stamp to canvas - stamp the pattern only onto canvas -
    it will not affect the layer or image 
*/

bool StampTool::stampToCanvas(QPoint pos)
{
    KisImage* img = m_pDoc->current();

    QPainter p;
    p.begin(m_pCanvas);
    p.scale( m_pView->zoomFactor(), m_pView->zoomFactor() );

    QRect ur(pos.x() - mHotSpotX, pos.y()-mHotSpotY, 
        patternWidth, patternHeight);
    
    ur = ur.intersect(img->imageExtents());
    //ur.setBottom(ur.bottom()+1);
    //ur.setRight(ur.right()+1);

    if (ur.top()    > img->height() 
    || ur.left()    > img->width()
    || ur.bottom()  < 0
    || ur.right()   < 0)
    {
        p.end();
        return false;
    }

    int startX = 0;
    int startY = 0;

    if(((pos.x() > 0 - mHotSpotX) && (pos.x() < mHotSpotX))) 
        startX += (mHotSpotX - pos.x());    
    if(startX > ur.width())
        startX = ur.width();
    if(((pos.y() > 0 - mHotSpotY) && (pos.y() < mHotSpotY))) 
        startY += (mHotSpotY - pos.y());    
    if(startY > ur.height())
        startY = ur.height();

    int xt = m_pView->xPaintOffset()- m_pView->xScrollOffset();
    int yt = m_pView->yPaintOffset()- m_pView->yScrollOffset();

    p.translate(xt, yt);

    p.drawPixmap( ur.left(), ur.top(), 
                  m_pPattern->pixmap(), 
                  startX, startY, 
                  ur.width(), ur.height() );
                  
    p.end();

    return true;
}

/*
    stamp the pattern into the layer
*/

bool StampTool::stampColor(QPoint pos)
{
    KisImage *img = m_pDoc->current();
    KisLayer *lay = img->getCurrentLayer();
    QImage  *qimg = m_pPattern->image();
    
    int startx = pos.x();
    int starty = pos.y();

    QRect clipRect(startx, starty, patternWidth, patternHeight);

    if (!clipRect.intersects(lay->imageExtents()))
        return false;
  
    clipRect = clipRect.intersect(lay->imageExtents());

    int sx = clipRect.left() - startx;
    int sy = clipRect.top() - starty;
    int ex = clipRect.right() - startx;
    int ey = clipRect.bottom() - starty;

    uchar r, g, b, a;
    int   v = 255;
    int   bv = 0;
    
    int red     = m_pView->fgColor().R();
    int green   = m_pView->fgColor().G();
    int blue    = m_pView->fgColor().B();

    bool blending = false;
    bool grayscale = false;
    bool layerAlpha =  (img->colorMode() == cm_RGBA); 
    bool patternAlpha = (qimg->hasAlphaBuffer());
  
    for (int y = sy; y <= ey; y++)
    {
        for (int x = sx; x <= ex; x++)
	    {
            // destination binary values by channel
            if(blending)
            {
	            r = lay->pixel(0, startx + x, starty + y);
	            g = lay->pixel(1, startx + x, starty + y);
	            b = lay->pixel(2, startx + x, starty + y);
            }    

            // pixel value in scanline at x offset to right
            uint *p = (uint *)qimg->scanLine(y) + x;

            // if it has an alpha channel value less than 100%,
            // don't paint the pixel. However, we need to combine
            // the actual alpha value of the pattern pixel with 
            // that of layer for correct blending, eventually
            if(patternAlpha)
            {
                if (!(*p & 0xff000000)) continue;
            }
                        
            // set layer pixel to be same as image
	        lay->setPixel(0, startx + x, starty + y, qRed(*p));
	        lay->setPixel(1, startx + x, starty + y, qGreen(*p));
	        lay->setPixel(2, startx + x, starty + y, qBlue(*p));
                      	  
            if (layerAlpha)
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


bool StampTool::stampMonochrome(QPoint /*pos*/)
{
    return true;
}


void StampTool::mouseMove(QMouseEvent *e)
{
    KisImage * img = m_pDoc->current();
    if(!img) return;
    
    KisLayer *lay = img->getCurrentLayer();
    if (!lay)  return;

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

            if (stampColor(p - mHotSpot))
            {
	            img->markDirty(QRect(p - mHotSpot, mPatternSize));
            }    
        }
        else
        {
            /* Button is not down. Refresh canvas from the layer
            and then blit the image to the canvas without affecting 
            the layer at all ! No need for double buffer!!!    
            Refresh first - markDirty relies on timer, 
            so we need force by directly updating the canvas. */
                

            if(oldp.x() < mHotSpotX) 
                oldp.setX(mHotSpotX);
            if(oldp.x() > lay->imageExtents().right() - mHotSpotX) 
                oldp.setX(lay->imageExtents().right() - mHotSpotX);
            if(oldp.y() < mHotSpotY)     
                oldp.setY(mHotSpotY);
            if(oldp.y() > lay->imageExtents().bottom() - mHotSpotY) 
                oldp.setY(lay->imageExtents().bottom() - mHotSpotY);
                
            QRect ur(oldp.x() - mHotSpotX - m_pView->xScrollOffset(), 
                     oldp.y() - mHotSpotY - m_pView->yScrollOffset(), 
                     patternWidth, 
                     patternHeight);
                         
            m_pView->updateCanvas(ur);
                                
            // after old spot is refreshed, stamp image into canvas
            // at current location. This may be slow or messy as updates
            // rely on a timer - need threads and semaphores here to let
            // us know when old marking has been replaced with image
            // if timer is used, but it's not used for this.
                
             if(!stampToCanvas(p /*- mHotSpot*/))
             {
                 kdDebug(0) << "canvas error!" << endl;                
             }            
        }
            
	    oldp = p; 
        dist -= spacing; 
	}
	  
    if (dist > 0) 
        m_dragdist = dist; 
    m_dragStart = e->pos();
}


void StampTool::mouseRelease(QMouseEvent *e)
{
    if (e->button() != LeftButton)
        return;
        
    m_dragging = false;
}


