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

#include "kis_tool_colorpicker.h"
#include "kis_doc.h"
#include "kis_view.h"
#include "kis_cursor.h"

ColorPicker::ColorPicker(KisDoc *doc, KisView *view)
  : KisTool(doc, view)
{
  m_Cursor = KisCursor::pickerCursor();
  m_dragging = false;
}

ColorPicker::~ColorPicker() {}

KisColor ColorPicker::pick(int x, int y)
{
  KisLayer *lay = m_pDoc->getCurrentLayer();
  uint pixel = lay->getPixel(x, y);

  uchar* ptr = (uchar*)&pixel;
  uchar b = *ptr++;
  uchar g = *ptr++;
  uchar r = *ptr++;
  
  return KisColor((int)r, (int)g, (int)b, RGB);
}

void ColorPicker::mousePress(QMouseEvent *e)
{
  if (e->button() != QMouseEvent::LeftButton
      && e->button() != QMouseEvent::RightButton)
    return;

  if( !m_pDoc->getCurrentLayer()->isVisible() )
    return;
  
  if( !m_pDoc->getCurrentLayer()->imageExtents().contains( e->pos() ))
    return;
  
  m_dragging = true;
  
  if (e->button() == QMouseEvent::LeftButton)
    m_pView->slotSetFGColor(pick(e->pos().x(), e->pos().y()));
  else if (e->button() == QMouseEvent::RightButton)
    m_pView->slotSetBGColor(pick(e->pos().x(), e->pos().y()));
}

void ColorPicker::mouseMove(QMouseEvent *e)
{
  if(m_dragging)
    {
      if( !m_pDoc->getCurrentLayer()->isVisible() )
	return;
      
      if( !m_pDoc->getCurrentLayer()->imageExtents().contains( e->pos() ))
	  return;

      if (e->button() == QMouseEvent::LeftButton)
	m_pView->slotSetFGColor(pick(e->pos().x(), e->pos().y()));
      else if (e->button() == QMouseEvent::RightButton)
	m_pView->slotSetBGColor(pick(e->pos().x(), e->pos().y()));
    }
}

void ColorPicker::mouseRelease(QMouseEvent *e)
{
  if (e->button() != QMouseEvent::LeftButton
      && e->button() != QMouseEvent::RightButton)
    return;
  
  m_dragging = false;
}
