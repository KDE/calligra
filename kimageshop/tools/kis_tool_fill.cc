/*
 *  colorpicker.cc - part of KImageShop
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
#include <kdebug.h>
#include "kis_tool_fill.h"
#include "kis_doc.h"
#include "kis_view.h"
#include "kis_cursor.h"

Fill::Fill(KisDoc *doc, KisView *view)
  : KisTool(doc, view)
{
    m_Cursor = KisCursor::pickerCursor();
}

Fill::~Fill() {}


int Fill::checkTouching(KisLayer *lay, int x, int y, int r, int g, int b)
{
    // pixel is already the desired (new) color, return
    if( (lay->pixel(0, x, y) == nRed) &&
        (lay->pixel(1, x, y) == nGreen) &&
        (lay->pixel(2, x, y) == nBlue))
    {
        return 1;
    }

    // pixel is same as original color, set it to new color
    if( (lay->pixel(0, x, y) == r) &&
        (lay->pixel(1, x, y) == g) &&
        (lay->pixel(2, x, y) == b))
    {
        lay->setPixel(0, x, y, nRed);
        lay->setPixel(1, x, y, nGreen);
        lay->setPixel(2, x, y, nBlue);
        return 1;
    }
    
    // pixel is not same as either original or desired color, no action
    return 0;
}

/*
    drawFlood - recursive algorithm from kiconedit
    this doesn't check diagonals (for speed). It can be 
    optimized by removing variables from stack, 
    making lay, r, g and b class members, passing in 
    only x and y which will help performance with diagonal checking
    
    will cause crash due to stack overflow on large fills, so 
    limiting area is necessary by dividing into sections
*/

void Fill::drawFlood(KisLayer *lay, QRect & layerRect, int x, int y)
{
    // make sure desired pixel is in bounds - can operate on 
    // selection as well as entire image, so layerRect is not
    // always same as imageExtents (later)
    if(!layerRect.contains(x, y)) 
        return;

    // pixel is not same color as source pixel color, return
    if((lay->pixel(0, x, y) != sRed)
    || (lay->pixel(1, x, y) != sGreen) 
    || (lay->pixel(2, x, y) != sBlue))
    {
        return;
    }

    // pixel is already the desired (new) color, return
    if((lay->pixel(0, x, y) == nRed)
    && (lay->pixel(1, x, y) == nGreen) 
    && (lay->pixel(2, x, y) == nBlue))
    {
        return;
    }

    // pixel is same as original color, set it to new color
    lay->setPixel(0, x, y, nRed);
    lay->setPixel(1, x, y, nGreen);
    lay->setPixel(2, x, y, nBlue);
    
    if(layerRect.contains(x, y-1)) 
        drawFlood(lay, layerRect, x, y-1);
    if(layerRect.contains(x, y+1))         
        drawFlood(lay, layerRect, x, y+1);
    if(layerRect.contains(x-1, y))         
        drawFlood(lay, layerRect, x-1, y);
    if(layerRect.contains(x+1, y))         
        drawFlood(lay, layerRect, x+1, y);
}


bool Fill::flood(int startx, int starty)
{
    bool global = false;

    KisImage *img = m_pDoc->current();
    KisLayer *lay = img->getCurrentLayer();
    
    if (!img) return false;
    if (!lay) return false;

    // FIXME: Implement this for non-RGB modes.
    if (!img->colorMode() == cm_RGB && !img->colorMode() == cm_RGBA)
	    return false;
    
    // source color values of selected pixed
    sRed    = lay->pixel(0, startx, starty);
    sGreen  = lay->pixel(1, startx, starty);
    sBlue   = lay->pixel(2, startx, starty);

    // new color values from color selector 

    nRed     = m_pView->fgColor().R();
    nGreen   = m_pView->fgColor().G();
    nBlue    = m_pView->fgColor().B();

    QRect layerRect = lay->imageExtents();
    
    if(layerRect.width() > 320)
    { 
        layerRect.setLeft(startx - 160 > 0 ? startx - 160 : 0);  
        layerRect.setRight(startx + 160 < layerRect.right() ? startx + 160 :  layerRect.right());      
    }
    
    if(layerRect.height() > 320)
    { 
        layerRect.setTop(starty - 160 > 0 ? starty - 160 : 0);  
        layerRect.setBottom(starty + 160 < layerRect.bottom() ? starty + 160 : layerRect.bottom());      
    }
     
    // bool alpha = (img->colorMode() == cm_RGBA);
 
    drawFlood(lay, layerRect, startx, starty); 

    /* refresh canvas so changes show up */
    QRect updateRect(0, 0, img->width(), img->height());
    img->markDirty(updateRect);
  
    return true;
}


/* flood with color, gradient or pattern - uses quadrants 
   this can be modified later as an exchange colors 
   algorithm, setting all pixels of a desired color or 
   range to a different color - the recursive alogrithm
   above works better for bucket fills */
   
#if 0
bool Fill::flood(int startx, int starty)
{
    bool global = false;

    KisImage *img = m_pDoc->current();
    KisLayer *lay = img->getCurrentLayer();
    
    if (!img) return false;
    if (!lay) return false;

    // FIXME: Implement this for non-RGB modes.
    if (!img->colorMode() == cm_RGB && !img->colorMode() == cm_RGBA)
	    return false;
    
    // source color values of selected pixed
    int sRed    = lay->pixel(0, startx, starty);
    int sGreen  = lay->pixel(1, startx, starty);
    int sBlue   = lay->pixel(2, startx, starty);

    QRect layerRect = lay->imageExtents();
            
    int x, y;
    
    int sx = layerRect.left();
    int sy = layerRect.top();
    int ex = layerRect.right();
    int ey = layerRect.bottom();

    int count = 0;
    int linecount = 0;
    uchar a;

    // new color values from color selector 

    nRed     = m_pView->fgColor().R();
    nGreen   = m_pView->fgColor().G();
    nBlue    = m_pView->fgColor().B();

    bool alpha = (img->colorMode() == cm_RGBA);

    // find adjacent pixels from start in bottom right quadrant
    for ( y = starty; y <= ey; y++)
    {
        for (x = startx; x <= ex; x++)
	    {
            //kdDebug(0) << "x = " << x  << " y = " << y << endl;
            count = 0;
        
            // check adjacent pixels and set to desired color
            if(true)     count += checkTouching(lay, x,   y, sRed, sGreen, sBlue);
            if(x+1 < ex) count += checkTouching(lay, x+1, y, sRed, sGreen, sBlue);
            //if(x-1 > sx) count += checkTouching(lay, x-1, y, sRed, sGreen, sBlue);
            if(y+1 < ey) count += checkTouching(lay, x, y+1, sRed, sGreen, sBlue);
            //if(y-1 > sy) count += checkTouching(lay, x, y-1, sRed, sGreen, sBlue);
                       	  
            if (alpha)
	        {
	            a = lay->pixel(3, x, y);
		        lay->setPixel(3, x, y, a);
	        }
            
            linecount += count;    
            if(!count && !global) break;
	    }
         
        if(!linecount && !global) break;
    }
    
    linecount = 0;
    // find adjacent pixels from start in bottom left quadrand
    for (y = starty; y <= ey; y++)
    {
        for (x = startx; x >= sx; x--)
	    {
            //kdDebug(0) << "x = " << x  << " y = " << y << endl;
        
            count = 0;
        
            // check adjacent pixels and set to desired color
            if(true)     count += checkTouching(lay, x,   y, sRed, sGreen, sBlue);
            //if(x+1 < ex) count += checkTouching(lay, x+1, y, sRed, sGreen, sBlue);
            if(x-1 > sx) count += checkTouching(lay, x-1, y, sRed, sGreen, sBlue);
            if(y+1 < ey) count += checkTouching(lay, x, y+1, sRed, sGreen, sBlue);
            //if(y-1 > sy) count += checkTouching(lay, x, y-1, sRed, sGreen, sBlue);
                       	  
            if (alpha)
	        {
	            a = lay->pixel(3, x, y);
		        lay->setPixel(3, x, y, a);
	        }
            linecount += count;    
            if(!count && !global) break;
	    }

        if(!linecount && !global) break; 
    }

    linecount = 0;
    // find adjacent pixels from start in top left quadrand
    for (y = starty; y >= sy; y--)
    {
        for (x = startx; x >= sx; x--)
	    {
            // kdDebug(0) << "x = " << x  << " y = " << y << endl;

            count = 0;
                
            // check adjacent pixels and set to desired color
            if(true)     count += checkTouching(lay, x,   y, sRed, sGreen, sBlue);
            //if(x+1 < ex) count += checkTouching(lay, x+1, y, sRed, sGreen, sBlue);
            if(x-1 > sx) count += checkTouching(lay, x-1, y, sRed, sGreen, sBlue);
            //if(y+1 < ey) count += checkTouching(lay, x, y+1, sRed, sGreen, sBlue);
            if(y-1 > sy) count += checkTouching(lay, x, y-1, sRed, sGreen, sBlue);
                       	  
            if (alpha)
	        {
	            a = lay->pixel(3, x, y);
		        lay->setPixel(3, x, y, a);
	        }
            linecount += count;    
            if(!count && !global) break;
	    }
        
        if(!linecount && !global) break; 
    }

    linecount = 0;
    // find adjacent pixels from start in top right quadrand
    for (y = starty; y >= sy; y--)
    {
        for (x = startx; x <= ex; x++)
	    {
            //kdDebug(0) << "x = " << x  << " y = " << y << endl;      
        
            count = 0;
        
            // check adjacent pixels and set to desired color
            if(true)     count += checkTouching(lay, x,   y, sRed, sGreen, sBlue);
            if(x+1 < ex) count += checkTouching(lay, x+1, y, sRed, sGreen, sBlue);
            //if(x-1 > sx) count += checkTouching(lay, x-1, y, sRed, sGreen, sBlue);
            //if(y+1 < ey) count += checkTouching(lay, x, y+1, sRed, sGreen, sBlue);
            if(y-1 > sy) count += checkTouching(lay, x, y-1, sRed, sGreen, sBlue);
                       	  
            if (alpha)
	        {
	            a = lay->pixel(3, x, y);
		        lay->setPixel(3, x, y, a);
	        }
            linecount += count;            
            if(!count && !global) break;
	    }

        if(!linecount && !global) break;         
    }
    
    /* refresh canvas so changes show up */
    QRect updateRect(0, 0, img->width(), img->height());
    img->markDirty(updateRect);

    kdDebug(0) << "pixels changed = " << count << endl;      
    return true;
}
#endif

void Fill::mousePress(QMouseEvent *e)
{
    KisImage * img = m_pDoc->current();
    if (!img) return;

    if (e->button() != QMouseEvent::LeftButton
    && e->button() != QMouseEvent::RightButton)
        return;

    if( !img->getCurrentLayer()->visible() )
        return;
  
    if( !img->getCurrentLayer()->imageExtents().contains( e->pos() ))
        return;
  
    if (e->button() == QMouseEvent::LeftButton)
        flood(e->pos().x(), e->pos().y());
        
    else if (e->button() == QMouseEvent::RightButton)
        flood(e->pos().x(), e->pos().y());
}

