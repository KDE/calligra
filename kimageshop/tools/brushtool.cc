/*
 *  brushtool.cc - part of KImageShop
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

#include "brushtool.h"
#include "brush.h"
#include "kimageshop_doc.h"
#include "kimageshop_view.h"
#include "vec.h"

BrushTool::BrushTool(KImageShopDoc *doc, KImageShopView *view, const Brush *_brush)
  : Tool(doc, view)
{
  m_dragging = false;
  m_pBrush = _brush;
}

BrushTool::~BrushTool() {}

void BrushTool::setBrush(const Brush *_brush)
{
  m_pBrush = _brush;
}

void BrushTool::mousePress(QMouseEvent *e)
{
  if (e->button() != QMouseEvent::LeftButton)
    return;

  if( !m_pDoc->imageExtents().contains( e->pos() ))
    return;

  if( !m_pDoc->getCurrentLayer()->isVisible() )
    return;

  if( !m_pDoc->getCurrentLayer()->imageExtents().contains( e->pos() ))
    return;
  
  m_dragging = true;
  m_dragStart = e->pos();
  
  paint(e->pos());
  
  QRect updateRect(e->pos() - m_pBrush->hotSpot(), m_pBrush->size());
  m_pDoc->compositeImage(updateRect);
}

void BrushTool::paint(QPoint pos)
{
  if (!m_pBrush)
    return;

  // test color
  int red = m_pView->fgColor().R();
  int green = m_pView->fgColor().G();
  int blue = m_pView->fgColor().B();

  Layer *lay = m_pDoc->getCurrentLayer();

  QPoint start = pos - m_pBrush->hotSpot();
  int startx = start.x();
  int starty = start.y();

  uint srcPix, dstPix;
  uchar *sl, *ptr;
  uchar bv, invbv, r, g, b;

  for (int y = 0; y < m_pBrush->height(); y++)
    {
      sl = m_pBrush->scanline(y);

      for (int x = 0; x < m_pBrush->width(); x++)
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
	}
    }
}

void BrushTool::mouseMove(QMouseEvent *e)
{
  if(m_dragging)
    {
      if( !m_pDoc->imageExtents().contains( e->pos() ))
	return;

      if( !m_pDoc->getCurrentLayer()->isVisible() )
	return;

      if( !m_pDoc->getCurrentLayer()->imageExtents().contains( e->pos() ))
	  return;

      KVector end(e->x(), e->y());
      KVector start(m_dragStart.x(), m_dragStart.y());
      
      KVector moveVec = end-start;
      float length = moveVec.length();
      
      QRect updateRect = QRect(e->pos() - m_pBrush->hotSpot(), m_pBrush->size());
      paint(e->pos());
      
      /*if (length < 5)
	{
	  paint(e->pos());	  
	}
      else
	{
	  int steps = (int) (length / 5); // FIXME: configurable stepping
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

		  updateRect.unite(QRect(QPoint(bpos.x(), bpos.y()), m_pBrush->size()));
		}
	    }
	}
      */
      
      m_pDoc->compositeImage(updateRect);
      m_dragStart = e->pos();
    }
}

void BrushTool::mouseRelease(QMouseEvent *e)
{
  if (e->button() != LeftButton)
    return;
  m_dragging = false;
}


