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
#include "kvector.h"

BrushTool::BrushTool(KImageShopDoc *doc, const Brush *_brush) : Tool(doc)
{
  m_dragging = false;
  m_pBrush = _brush;
}

BrushTool::~BrushTool() {}

void BrushTool::setBrush(const Brush *_brush)
{
  m_pBrush = _brush;
}

void BrushTool::mousePress(const KImageShop::MouseEvent& e)
{
  if (!e.leftButton)
    return;

  m_dragging = true;
  QPoint pos(e.posX, e.posY);
  m_dragStart = pos;

  if (!m_pBrush)
    return;

  m_pDoc->paintBrush(pos, m_pBrush);

  QRect updateRect(pos - m_pBrush->hotSpot(), m_pBrush->brushSize());
  m_pDoc->compositeImage(updateRect);
  m_pDoc->slotUpdateViews(updateRect);
}

void BrushTool::mouseMove(const KImageShop::MouseEvent& e)
{
  if(m_dragging)
    {
      if (!m_pBrush)
		return;
	
      QPoint pos(e.posX, e.posY);

      KVector end(e.posX, e.posY);
      KVector start(m_dragStart.x(), m_dragStart.y());
	
      KVector moveVec = end-start;
      float length = moveVec.length();
	
      QRect updateRect;

      if (length < 10)
		{
		  m_pDoc->paintBrush(pos, m_pBrush);
		  updateRect = QRect(pos - m_pBrush->hotSpot(), m_pBrush->brushSize());
		}
      else
		{
		  int steps = (int) (length / 10); // FIXME: configurable stepping
		  moveVec.normalize();
		
		  for (int i=0; i<=steps; i++)
			{
			  if (i == steps)
				{
				  m_pDoc->paintBrush(pos, m_pBrush);
				}
			  else
				{
				  KVector bpos = start + moveVec * i * 10;
				  m_pDoc->paintBrush(QPoint(bpos.x(), bpos.y()), m_pBrush);
				}
			}
		
		  updateRect = QRect(QPoint(start.x(), start.y()) - m_pBrush->hotSpot(),
							 QSize(e.posX, e.posY) + m_pBrush->brushSize());
		}

      m_pDoc->compositeImage(updateRect);
      m_pDoc->slotUpdateViews(updateRect);
	
      m_dragStart = pos;
    }
}

void BrushTool::mouseRelease(const KImageShop::MouseEvent& e)
{
  if (!e.leftButton)
    return;
  m_dragging = false;
}
