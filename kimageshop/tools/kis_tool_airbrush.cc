/*
 *  kis_tool_airbrush.cc - part of KImageShop
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

#include "kis_tool_airbrush.h"
#include "kis_brush.h"
#include "kis_doc.h"
#include "kis_view.h"
#include "kis_vec.h"
#include "kis_cursor.h"
#include "kis_util.h"

AirKisBrushTool::AirKisBrushTool(KisDoc *doc, KisView *view, const KisBrush *_brush)
  : Tool(doc, view)
{
  m_dragging = false;
  m_Cursor = KisCursor::brushCursor();
  m_pKisBrush = _brush;
  m_dragdist = 0;
}

AirKisBrushTool::~AirKisBrushTool() {}

void AirKisBrushTool::setKisBrush(const KisBrush *_brush)
{
  m_pKisBrush = _brush;
}

void AirKisBrushTool::mousePress(QMouseEvent *e)
{
  if (e->button() != QMouseEvent::LeftButton)
    return;

   if( !m_pDoc->getCurrentLayer()->isVisible() )
    return;

  m_dragging = true;
  m_dragStart = e->pos();
  m_dragdist = 0;

  paint(e->pos());
  
  QRect updateRect(e->pos() - m_pKisBrush->hotSpot(), m_pKisBrush->size());
  m_pDoc->compositeImage(updateRect);
}

bool AirKisBrushTool::paint(QPoint pos)
{
  if (!m_pKisBrush)
    return false;

  QPoint start = pos - m_pKisBrush->hotSpot();
  int startx = start.x();
  int starty = start.y();

  QRect clipRect(startx, starty, m_pKisBrush->width(), m_pKisBrush->width());

  if (!clipRect.intersects(m_pDoc->getCurrentLayer()->imageExtents()))
    return false;
  
  clipRect = clipRect.intersect(m_pDoc->getCurrentLayer()->imageExtents());

  int sx = clipRect.left() - startx;
  int sy = clipRect.top() - starty;
  int ex = clipRect.right() - startx;
  int ey = clipRect.bottom() - starty;

  Layer *lay = m_pDoc->getCurrentLayer();
 
  uint srcPix, dstPix;
  uchar *sl, *ptr;
  uchar bv, invbv;
  uchar r, g, b, srcA, dstA;
  int v;

  int red = m_pView->fgColor().R();
  int green = m_pView->fgColor().G();
  int blue = m_pView->fgColor().B();

  for (int y = sy; y <= ey; y++)
    {
      sl = m_pKisBrush->scanline(y);

      for (int x = sx; x <= ex; x++)
	{
	  srcPix = lay->getPixel(startx + x, starty + y);

	  bv = *(sl + x);
	  if (bv == 0) continue;

	  invbv = 255 - bv;

	  ptr = (uchar*)&srcPix;
	  b = *ptr++;
	  g = *ptr++;
	  r = *ptr++;

	  ptr = (uchar*)&dstPix;
	  *ptr++ = ((blue * bv) + (b * invbv))/255;
	  *ptr++ = ((green * bv) + (g * invbv))/255;
	  *ptr++ = ((red * bv) + (r * invbv))/255;

	  lay->setPixel(startx + x, starty + y, dstPix);

	  if (lay->hasAlphaChannel())
	    {
	      srcA = (uchar) lay->getAlpha(startx + x, starty + y);
	      v = srcA + bv;
	      if (v < 0 ) v = 0;
	      if (v > 255 ) v = 255;
	      dstA = (uchar) v;

	      lay->setAlpha(startx + x, starty + y, (uint)dstA);
	    }
	}
    }
  return true;
}

void AirKisBrushTool::mouseMove(QMouseEvent *e)
{
  int spacing = m_pKisBrush->spacing();

  if (spacing <= 0) spacing = 1;

  if(m_dragging)
    {
      if( !m_pDoc->getCurrentLayer()->isVisible() )
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

      QRect updateRect;
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
	  	  
	  if (paint(p))
	    updateRect = updateRect.unite(QRect(p - m_pKisBrush->hotSpot(), m_pKisBrush->size()));
	  dist -= spacing;
	}
      if (!updateRect.isEmpty())
	m_pDoc->compositeImage(updateRect);

      if (dist > 0)
	m_dragdist = dist; //save for next moveevent
      m_dragStart = e->pos();
    }
}

void AirKisBrushTool::mouseRelease(QMouseEvent *e)
{
  if (e->button() != LeftButton)
    return;
  m_dragging = false;
}


