/*
 *  kis_tool_brush.cc - part of Krayon
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
#include <qbitmap.h>
#include <kdebug.h>
#include <kapp.h>


#include "kis_brush.h"
#include "kis_doc.h"
#include "kis_view.h"
#include "kis_canvas.h"
#include "kis_vec.h"
#include "kis_cursor.h"
#include "kis_util.h"
#include "kis_tool_brush.h"
#include "kis_dlg_toolopts.h"


BrushTool::BrushTool(KisDoc *doc, KisView *view, KisBrush *_brush)
  : KisTool(doc, view)
{
    m_dragging = false;
    m_dragdist = 0;
    
    usePattern = false;
    useGradient = false;
    opacity = 255;
        
    setBrush(_brush);
}

BrushTool::~BrushTool() {}


void BrushTool::setBrush(KisBrush *_brush)
{
    m_pBrush = _brush;
    
    brushWidth = m_pBrush->pixmap().width();
    brushHeight = m_pBrush->pixmap().height();
    hotSpot  = m_pBrush->hotSpot();
    hotSpotX = m_pBrush->hotSpot().x();
    hotSpotY = m_pBrush->hotSpot().y();
    brushSize = QSize(brushWidth, brushHeight);
        
    // make custom cursor from brush pixmap 
    // if brush pixmap is of reasonable size
    if((brushWidth < 33 && brushHeight < 33) 
    && (brushWidth > 9 && brushHeight > 9))
    {
        QBitmap mask(brushWidth, brushHeight);
        QPixmap pix(m_pBrush->pixmap());
        mask = pix.createHeuristicMask();
        pix.setMask(mask);
        m_pView->kisCanvas()->setCursor(QCursor(pix));
        m_Cursor = QCursor(pix);   
    } 
    // use default brush cursor   
    else 
    {
        m_pView->kisCanvas()->setCursor(KisCursor::brushCursor()); 
        m_Cursor = KisCursor::brushCursor();
    }    
}


void BrushTool::mousePress(QMouseEvent *e)
{
    KisImage * img = m_pDoc->current();
    if (!img) return;

    if (e->button() != QMouseEvent::LeftButton)
        return;

    if( !img->getCurrentLayer()->visible() )
        return;

    red = m_pView->fgColor().R();
    green = m_pView->fgColor().G();
    blue = m_pView->fgColor().B();

    alpha = (img->colorMode() == cm_RGBA);
    spacing = m_pBrush->spacing();
    if (spacing <= 0) spacing = 3;

    m_dragging = true;

    QPoint pos = e->pos();
    pos = zoomed(pos);
    m_dragStart = pos;
    m_dragdist = 0;
    
    if(paintMonochrome(pos))
    {
         img->markDirty(QRect(pos - hotSpot, brushSize));      
    }
}

bool BrushTool::paintCanvas(QPoint /* pos */)
{
    return true;
}


bool BrushTool::paintMonochrome(QPoint pos)
{
    KisImage * img = m_pDoc->current();
    KisLayer *lay = img->getCurrentLayer();

    int startx = pos.x() - hotSpotX;
    int starty = pos.y() - hotSpotY;
    
    QRect clipRect(startx, starty, brushWidth, brushHeight);
 
    if (!clipRect.intersects(lay->imageExtents()))
        return false;
  
    clipRect = clipRect.intersect(lay->imageExtents());
    
    int sx = clipRect.left() - startx;
    int sy = clipRect.top() - starty;
    int ex = clipRect.right() - startx;
    int ey = clipRect.bottom() - starty;
    
    uchar *sl;
    uchar bv, invbv;
    uchar r, g, b, a;
    int   v;

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
	            a = lay->pixel(3, startx + x, starty + y);

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

    if(m_dragging)
    {
        QPoint pos = e->pos();      
        int mouseX = e->x();
        int mouseY = e->y();

        pos = zoomed(pos);
        mouseX = zoomedX(mouseX);
        mouseY = zoomedY(mouseY);        

        KisVector end(mouseX, mouseY);
        KisVector start(m_dragStart.x(), m_dragStart.y());
            
        KisVector dragVec = end - start;
        float saved_dist = m_dragdist;
        float new_dist = dragVec.length();
        float dist = saved_dist + new_dist;

        if ((int)dist < spacing)
	    {
	        m_dragdist += new_dist; 
	        m_dragStart = pos;
	        return;
	    }
        else 
	        m_dragdist = 0; 

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
               img->markDirty(QRect(p - hotSpot, brushSize));

 	        dist -= spacing;
	    }

        if (dist > 0) m_dragdist = dist; 
        m_dragStart = pos;
    }
}


void BrushTool::mouseRelease(QMouseEvent *e)
{
    if (e->button() != LeftButton)
        return;
        
    m_dragging = false;
}


bool BrushTool::paintColor(QPoint /*pos*/)
{
    return true;
}

void BrushTool::optionsDialog()
{
    ToolOptsStruct ts;    
    
    ts.usePattern       = usePattern;
    ts.useGradient      = useGradient;
    ts.opacity          = opacity;

    ToolOptionsDialog *pOptsDialog 
        = new ToolOptionsDialog(tt_brushtool, ts);

    pOptsDialog->exec();
    
    if(!pOptsDialog->result() == QDialog::Accepted)
        return;

    opacity   = pOptsDialog->brushToolTab()->opacity();
    usePattern    = pOptsDialog->brushToolTab()->usePattern();
    useGradient   = pOptsDialog->brushToolTab()->useGradient();
}
