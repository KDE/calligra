/*
 *  kis_framebuffer.cc - part of Krayon
 *
 *  Copyright (c) 2001 JohnCaliff <jcaliff@compuzone.net>
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
#include "kis_image.h"
#include "kis_layer.h"
#include "kis_vec.h"
#include "kis_util.h"
#include "kis_framebuffer.h"


KisFrameBuffer::KisFrameBuffer(KisDoc *doc)
{
    pDoc = doc;
    pScratchLayer = 0;
}


KisFrameBuffer::~KisFrameBuffer()
{
}


void KisFrameBuffer::setRect(QRect & rect)
{
    destRect.setLeft(rect.left());
    destRect.setTop(rect.top());
    destRect.setRight(rect.right());
    destRect.setBottom(rect.bottom());    
}


void KisFrameBuffer::setNull()
{
    destRect.setWidth(0);
    destRect.setHeight(0);
}

void KisFrameBuffer::addScratchLayer(int width, int height)
{
    
}

void KisFrameBuffer::removeScratchLayer()
{
    
}


void KisFrameBuffer::setImage(QImage & img)
{
    srcImage = img;
}


/*
    erase a rectange within a the current layer
    ignore destination color values and alpha value
*/

bool KisFrameBuffer::eraseCurrentLayer()
{
    KisImage *img = pDoc->current();
    if (!img) return false;
    
    KisLayer *lay = img->getCurrentLayer();
    if (!lay) return false;
    
    if (!img->colorMode() == cm_RGB && !img->colorMode() == cm_RGBA)
    {
        kdDebug(0) << "colormode is not RGB or RGBA!" << endl;
	    return false;
    }

    QRect clipRect(destRect);
    
    if (!clipRect.intersects(lay->imageExtents()))
        return false;
  
    clipRect = clipRect.intersect(lay->imageExtents());

    int sx = clipRect.left();
    int sy = clipRect.top(); 
    int ex = clipRect.right();
    int ey = clipRect.bottom();

    for (int y = sy; y <= ey; y++)
    {
        for (int x = sx; x <= ex; x++)
	    {
	        lay->setPixel(0, x, y, 255);
	        lay->setPixel(1, x, y, 255);
	        lay->setPixel(2, x, y, 255);
	    } 
    }

    return true;
}



/* 
    scale - from a rectangle in current layer smoothing colors.
    first, add new layer the size of rectange. 
    This will become the new current layer.
    Scale from one layer to other by copying pixels and
    averaging 4 adjacent pixels -
*/

bool KisFrameBuffer::scaleSmooth(QRect & srcR, int newWidth, int newHeight)
{    
    KisImage *img = pDoc->current();
    if (!img) return false;
    
    KisLayer *lay = img->getCurrentLayer();
    if (!lay) return false;

    QRect nr(0, 0, newWidth, newHeight);    
    
    QString layerName; 
    layerName.sprintf("layer %d", img->layerList().count());
    img->addLayer(nr, white, false, layerName);
    
    // adding a layer makes it the new current layer
    KisLayer *nlay = img->getCurrentLayer();
    if(!nlay) 
    {
        kdDebug() << "scaleSmooth(): new layer not allocated!" << endl;
        return false;
    }    

    int srcXoffset = srcR.left();
    int srcYoffset = srcR.top();
    int srcWidth   = srcR.width();
    int srcHeight  = srcR.height();
    
    int x = 0, y = 0;
    int xpos = x, ypos = y;
    
    float r, g, b;
    int r1, g1, b1;
    int r2, g2, b2;
    int r3, g3, b3;
    int r4, g4, b4;

    float x1, y1;

    float xerr, yerr;
    float xfloat, yfloat;
    
    float ratio_x = (float)srcR.width()  / (float)newWidth;
    float ratio_y = (float)srcR.height() / (float)newHeight;

    for (ypos = y; ypos < y + newHeight; ypos++)
    {
        for (xpos = x; xpos < x + newWidth; xpos++) 
        {
            xfloat = (float)(xpos - x) * ratio_x;
            yfloat = (float)(ypos - y) * ratio_y;

            x1 = srcXoffset + (int)xfloat;
            y1 = srcYoffset + (int)yfloat;

            xerr = 1.0 - (xfloat - (float)(x1 - srcXoffset));
            yerr = 1.0 - (yfloat - (float)(y1 - srcYoffset));

            r1 = lay->pixel(0, x1, y1); 
            g1 = lay->pixel(1, x1, y1);
            b1 = lay->pixel(2, x1, y1);
            
            // do not exceed layer width with check 
            // on right edge in source        
            if((xpos < x + newWidth) 
            && (x1 + 1 < srcWidth + srcXoffset))
            {
                r2 = lay->pixel(0, x1+1, y1); 
                g2 = lay->pixel(1, x1+1, y1);
                b2 = lay->pixel(2, x1+1, y1);
            }
            else
            {
                r2 = r1; g2 = g1; b2 = b1;
            }
            // do not exceed layer width & height with check 
            // at bottom right corner pixel in source (unique condition!)
            if((xpos < x + newWidth) 
            && (ypos < y + newHeight)
            && (x1 + 1 < srcWidth + srcXoffset) 
            && (y1 + 1 < srcHeight + srcYoffset))
            {
                r3 = lay->pixel(0, x1+1, y1+1); 
                g3 = lay->pixel(1, x1+1, y1+1);
                b3 = lay->pixel(2, x1+1, y1+1);
            }
            else
            {
                r3 = r1; g3 = g1; b3 = b1;
            }

            // do not exceed layer height in check 
            // along bottom row in source 
            if((ypos < y + newHeight) 
            && (y1 + 1 < srcHeight + srcYoffset))
            {            
                r4 = lay->pixel(0, x1, y1+1); 
                g4 = lay->pixel(1, x1, y1+1);
                b4 = lay->pixel(2, x1, y1+1);
            }                
            else
            {
                r4 = r1; g4 = g1; b4 = b1;
            }

            r = (float)r1 * xerr +
                (float)r2 * (1.0 - xerr) +
                (float)r3 * (1.0 - xerr) +
                (float)r4 * xerr;

            r += (float)r1 * yerr +
                 (float)r2 * yerr +
                 (float)r3 * (1.0 - yerr) +
                 (float)r4 * (1.0 - yerr);

            r *= 0.25;

            g = (float)g1 * xerr +
                (float)g2 * (1.0 - xerr) +
                (float)g3 * (1.0 - xerr) +
                (float)g4 * xerr;

            g += (float)g1 * yerr +
                 (float)g2 * yerr +
                 (float)g3 * (1.0 - yerr) +
                 (float)g4 * (1.0 - yerr);

            g *= 0.25;

            b = (float)b1 * xerr +
                (float)b2 * (1.0 - xerr) +
                (float)b3 * (1.0 - xerr) +
                (float)b4 * xerr;

            b += (float)b1 * yerr +
                 (float)b2 * yerr +
                 (float)b3 * (1.0 - yerr) +
                 (float)b4 * (1.0 - yerr);

            b *= 0.25;

            nlay->setPixel(0, xpos - x, ypos - y, (int)r);
            nlay->setPixel(1, xpos - x, ypos - y, (int)g);
            nlay->setPixel(2, xpos - x, ypos - y, (int)b);
        }
    }

    return true;    
} 

/*
    Scale area without smoothing colors based on averaging
    of adjacent colors - a must for indexed mode
*/
bool KisFrameBuffer::scaleRough(QRect & srcR, int newWidth, int newHeight)
{    
    KisImage *img = pDoc->current();
    if (!img) return false;
    
    KisLayer *lay = img->getCurrentLayer();
    if (!lay) return false;

    QRect nr(0, 0, newWidth, newHeight);    

    QString layerName; 
    layerName.sprintf("layer %d", img->layerList().count());
    img->addLayer(nr, white, false, layerName);

    // adding a layer makes it the new current layer
    KisLayer *nlay = img->getCurrentLayer();
    if(!nlay) 
    {
        kdDebug() << "scaleRough(): new layer not allocated!" << endl;
        return false;
    }    

    int srcXoffset = srcR.left();
    int srcYoffset = srcR.top();
    
    int x = 0, y = 0;
    int xpos = x, ypos = y;    

    kdDebug() << "srcR.left() " << srcR.left() 
              << "srcR.top() " << srcR.top()  << endl;
    
    float r, g, b;
    float x1, y1;
    float xfloat, yfloat;
    
    float ratio_x = (float)srcR.width()  / (float)newWidth;
    float ratio_y = (float)srcR.height() / (float)newHeight;

    for (ypos = y; ypos < y + newHeight; ypos++)
    {
        for (xpos = x; xpos < x + newWidth; xpos++) 
        {
            xfloat = (xpos - x) * ratio_x;
            yfloat = (ypos - y) * ratio_y;

            x1 = srcXoffset + (int)xfloat;
            y1 = srcYoffset + (int)yfloat;

            r = lay->pixel(0, x1, y1); 
            g = lay->pixel(1, x1, y1);
            b = lay->pixel(2, x1, y1);
            
            nlay->setPixel(0, xpos - x , ypos - y, (int)r);
            nlay->setPixel(1, xpos - x,  ypos - y, (int)g);
            nlay->setPixel(2, xpos - x,  ypos - y, (int)b);
        }
    }

    return true;
}


bool KisFrameBuffer::mirror(QRect & )
{    
    // add new layer
    // copy from end of src row data to 
    // beginning of dest row for each row.

    KisImage *img = pDoc->current();
    if (!img) return false;
    
    KisLayer *lay = img->getCurrentLayer();
    if (!lay) return false;

    return true;    
} 


bool KisFrameBuffer::flip(QRect & )
{    
    // add new layer - same x and y sizes as src
    // copy last row in src to first row in dest for each row 
    // data in each row should be same

    KisImage *img = pDoc->current();
    if (!img) return false;
    
    KisLayer *lay = img->getCurrentLayer();
    if (!lay) return false;
    
    return true;    
} 


bool KisFrameBuffer::rotate90(QRect & )
{    
    // add new layer - reverse x and y sizes
    // for each row in src, start at end of
    // row in dest and place data in reverse order

    KisImage *img = pDoc->current();
    if (!img) return false;
    
    KisLayer *lay = img->getCurrentLayer();
    if (!lay) return false;

    return true;    
} 


bool KisFrameBuffer::rotate180(QRect & )
{    
    // add new layer - same x and y sizes
    // start at top left of src  and copy data in
    // each row in reverse order from bottom of dest 
    // to top
    
    KisImage *img = pDoc->current();
    if (!img) return false;
    
    KisLayer *lay = img->getCurrentLayer();
    if (!lay) return false;

    return true;    
} 


bool KisFrameBuffer::rotate270(QRect & )
{    
    // add new layer - reverse x and y sizes
    // start at top left of src and copy data in
    // each row in src to each column in dest

    KisImage *img = pDoc->current();
    if (!img) return false;
    
    KisLayer *lay = img->getCurrentLayer();
    if (!lay) return false;

    return true;    
} 
    

bool KisFrameBuffer::QImageToLayer(QImage *qimg, QRect & srcR, QRect & destR)
{    
    // use current layer only
    // copy from rectangle in QImage to rectangle in current layer
    KisImage *img = pDoc->current();
    if (!img) return false;
    
    KisLayer *lay = img->getCurrentLayer();
    if (!lay) return false;

    return true;    
} 


bool KisFrameBuffer::layerToQImage(QImage *qimg, QRect & srcR, QRect & destR)
{    
    // use current layer only
    // normally src and destination rectangles are same size
    KisImage *img = pDoc->current();
    if (!img) return false;
    
    KisLayer *lay = img->getCurrentLayer();
    if (!lay) return false;

    return true;
} 


#include "kis_framebuffer.moc"

