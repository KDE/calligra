/*
 *  kis_tool_pen.cc - part of Krayon
 *
 *  Copyright (c) 1999 Matthias Elter <me@kde.org>
 *                2001 John Califf
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

#include <qpainter.h>
#include <qbitmap.h>

#include "kis_brush.h"
#include "kis_doc.h"
#include "kis_view.h"
#include "kis_vec.h"
#include "kis_framebuffer.h"
#include "kis_cursor.h"
#include "kis_tool_pen.h"
#include "kis_dlg_toolopts.h"

PenTool::PenTool(KisDoc *doc, KisView *view,
    KisCanvas *canvas, KisBrush *_brush)
  : KisTool(doc, view)
{
    m_dragging = false;
    m_pView  = view;
    m_pCanvas = canvas;
    m_pDoc = doc;

    // initialize pen tool settings
    KisDoc::PenToolSettings s = m_pDoc->getPenToolSettings();
    penColorThreshold = s.paintThreshold;
    penOpacity        = s.opacity;
    usePattern        = s.paintWithPattern;
    useGradient       = s.paintWithGradient;

    lineThickness = 1;
    lineOpacity = 1;

    setBrush(_brush);
}


PenTool::~PenTool()
{
}


void PenTool::setBrush(KisBrush *_brush)
{
    m_pBrush = _brush;

    int w = m_pBrush->pixmap().width();
    int h = m_pBrush->pixmap().height();

    if((w < 33 && h < 33) && (w > 9 && h > 9))
    {
        QBitmap mask(w, h);
        QPixmap pix(m_pBrush->pixmap());
        mask = pix.createHeuristicMask();
        pix.setMask(mask);
        m_pView->kisCanvas()->setCursor(QCursor(pix));
        m_Cursor = QCursor(pix);
    }
    else
    {
        m_pView->kisCanvas()->setCursor(KisCursor::brushCursor());
        m_Cursor = KisCursor::brushCursor();
    }
}


void PenTool::mousePress(QMouseEvent *e)
{
    /* do all status checking on mouse press only!
    Not needed elsewhere and slows things down if
    done in mouseMove and Paint routines.  Nothing
    happens unless mouse is first pressed anyway */

    KisImage * img = m_pDoc->current();
    if (!img) return;

    if(!img->getCurrentLayer())
        return;

    if(!img->getCurrentLayer()->visible())
        return;

    if (e->button() != QMouseEvent::LeftButton)
        return;

    if(!m_pDoc->frameBuffer())
        return;

    m_dragging = true;

    QPoint pos = e->pos();
    pos = zoomed(pos);
    m_dragStart = pos;
    m_dragdist = 0;

    if(paint(pos))
    {
         m_pDoc->current()->markDirty(QRect(pos
            - m_pBrush->hotSpot(), m_pBrush->size()));
    }
}


bool PenTool::paint(QPoint pos)
{
    KisImage * img = m_pDoc->current();
    KisLayer *lay = img->getCurrentLayer();
    KisFrameBuffer *fb = m_pDoc->frameBuffer();

    int startx = (pos - m_pBrush->hotSpot()).x();
    int starty = (pos - m_pBrush->hotSpot()).y();

    QRect clipRect(startx, starty, m_pBrush->width(), m_pBrush->height());

    if (!clipRect.intersects(lay->imageExtents()))
        return false;

    clipRect = clipRect.intersect(lay->imageExtents());

    int sx = clipRect.left() - startx;
    int sy = clipRect.top() - starty;
    int ex = clipRect.right() - startx;
    int ey = clipRect.bottom() - starty;

    uchar *sl;
    uchar bv;
    uchar r, g, b;

    int red = m_pView->fgColor().R();
    int green = m_pView->fgColor().G();
    int blue = m_pView->fgColor().B();

    bool alpha = (img->colorMode() == cm_RGBA);

    for (int y = sy; y <= ey; y++)
    {
        sl = m_pBrush->scanline(y);

        for (int x = sx; x <= ex; x++)
	    {
            // no color blending with pen tool (only with brush)
	        // alpha blending only (maybe)

	        bv = *(sl + x);
	        if (bv < penColorThreshold) continue;

		    r   = red;
            g   = green;
            b   = blue;

            // use foreround color
            if(!usePattern)
            {
	            lay->setPixel(0, startx + x, starty + y, r);
	            lay->setPixel(1, startx + x, starty + y, g);
	            lay->setPixel(2, startx + x, starty + y, b);
            }
            // map pattern to pen pixel
            else
            {
	            fb->setPatternToPixel(lay, startx + x, starty + y, 0);
            }

            if (alpha)
	        {
		        lay->setPixel(3, startx + x, starty + y, bv);
	        }
	    }
    }

    return true;
}


void PenTool::mouseMove(QMouseEvent *e)
{
    KisImage * img = m_pDoc->current();

    int spacing = m_pBrush->spacing();
    if (spacing <= 0) spacing = 1;

    if(m_dragging)
    {
        QPoint pos = e->pos();
        int mouseX = e->x();
        int mouseY = e->y();

        pos = zoomed(pos);
        mouseX = zoomed(mouseX);
        mouseY = zoomed(mouseY);

        KisVector end(mouseX, mouseY);
        KisVector start(m_dragStart.x(), m_dragStart.y());

        KisVector dragVec = end - start;
        float saved_dist = m_dragdist;
        float new_dist = dragVec.length();
        float dist = saved_dist + new_dist;

        if ((int)dist < spacing)
	    {
            // save for next movevent
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

	        QPoint p(qRound(step.x()), qRound(step.y()));

	        if (paint(p))
               img->markDirty(QRect(p - m_pBrush->hotSpot(), m_pBrush->size()));

 	        dist -= spacing;
	    }
        //save for next movevent
        if (dist > 0) m_dragdist = dist;
        m_dragStart = pos;
    }
}


void PenTool::mouseRelease(QMouseEvent *e)
{
    if (e->button() != LeftButton)
        return;

    m_dragging = false;
}

void PenTool::optionsDialog()
{
    ToolOptsStruct ts;

    ts.usePattern       = usePattern;
    ts.useGradient      = useGradient;
    ts.penThreshold     = penColorThreshold;
    ts.opacity          = penOpacity;

    bool old_usePattern         = usePattern;
    bool old_useGradient        = useGradient;
    int old_penColorThreshold   = penColorThreshold;
    int old_penOpacity          = penOpacity;

    ToolOptionsDialog *pOptsDialog
        = new ToolOptionsDialog(tt_pentool, ts);

    pOptsDialog->exec();

    if(!pOptsDialog->result() == QDialog::Accepted)
        return;
    else {
        usePattern          = pOptsDialog->penToolTab()->usePattern();
        useGradient         = pOptsDialog->penToolTab()->useGradient();
        penColorThreshold   = pOptsDialog->penToolTab()->penThreshold();
        penOpacity          = pOptsDialog->penToolTab()->opacity();

        // User change value ?
        if ( old_usePattern != usePattern || old_useGradient != useGradient
             || old_penColorThreshold != penColorThreshold || old_penOpacity != penOpacity ) {
            // set pen tool settings
            KisDoc::PenToolSettings s;
            s.paintThreshold     = penColorThreshold;
            s.opacity            = penOpacity;
            s.paintWithPattern   = usePattern;
            s.paintWithGradient  = useGradient;

            m_pDoc->setPenToolSettings( s );

            m_pDoc->setModified( true );
        }
    }
}
