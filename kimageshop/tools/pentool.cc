/*
 *  pentool.cc - part of KImageShop
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

#include "pentool.h"
#include "brush.h"
#include "kimageshop_doc.h"
#include "kimageshop_view.h"
#include "vec.h"

PenTool::PenTool(KImageShopDoc *doc, KImageShopView *view, const Brush *_brush)
  : Tool(doc, view)
{
  m_dragging = false;
  m_pBrush = _brush;
}

PenTool::~PenTool() {}

void PenTool::setBrush(const Brush *_brush)
{
  m_pBrush = _brush;
}

void PenTool::mousePress(QMouseEvent *e)
{
  if (e->button() != QMouseEvent::LeftButton)
    return;
  
  m_dragging = true;
  m_dragStart = e->pos();
  
  paint(e->pos());
  
  QRect updateRect(e->pos() - m_pBrush->hotSpot(), m_pBrush->size());
  m_pDoc->compositeImage(updateRect);
}

void PenTool::paint(QPoint pos)
{
  if (!m_pBrush)
    return;

  int red = m_pView->fgColor().R();
  int green = m_pView->fgColor().G();
  int blue = m_pView->fgColor().B();

  Layer *lay = m_pDoc->getCurrentLayer();

  QPoint start = pos - m_pBrush->hotSpot();
  int startx = start.x();
  int starty = start.y();

  uint dstPix;
  uchar *sl, *ptr;
  uchar bv;

  for (int y = 0; y < m_pBrush->height(); y++)
    {
      sl = m_pBrush->scanline(y);

      for (int x = 0; x < m_pBrush->width(); x++)
	{
	  bv = *(sl + x);
	  if (bv < 5) continue;

	  ptr = (uchar*)&dstPix;
	  *ptr++ = blue;
	  *ptr++ = green;
	  *ptr++ = red;

	  lay->setPixel(startx + x, starty + y, dstPix);
	}
    }
}

void PenTool::mouseMove(QMouseEvent *e)
{
  if(m_dragging)
    {
      if (!m_pBrush)
	return;
      
      KVector end(e->x(), e->y());
      KVector start(m_dragStart.x(), m_dragStart.y());
      
      KVector moveVec = end-start;
      float length = moveVec.length();
      
      QRect updateRect;
      
      if (length < 10)
	{
	  paint(e->pos());
	  updateRect = QRect(e->pos() - m_pBrush->hotSpot(), m_pBrush->size());
	}
      else
	{
	  int steps = (int) (length / 10); // FIXME: configurable stepping
	  moveVec.normalize();
	  
	  for (int i=0; i<=steps; i++)
	    {
	      if (i == steps)
		{
		  paint(e->pos());
		}
	      else
		{
		  KVector bpos = start + moveVec * i * 10;
		  paint(QPoint(bpos.x(), bpos.y()));
		}
	    }
	  
	  updateRect = QRect(QPoint(start.x(), start.y()) - m_pBrush->hotSpot(),
			     QSize(e->x(), e->y()) + m_pBrush->size());
	}
      
      m_pDoc->compositeImage(updateRect);
      m_dragStart = e->pos();
    }
}

void PenTool::mouseRelease(QMouseEvent *e)
{
  if (e->button() != LeftButton)
    return;
  m_dragging = false;
}


