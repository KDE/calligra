/*
 *  zoomtool.cc - part of KImageShop
 *
 *  Copyright (c) 1999 Matthias Elter  <me@kde.org>
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

#include "qpoint.h"

#include "kimageshop_doc.h"
#include "zoomtool.h"

ZoomTool::ZoomTool(KImageShopView *view) : Tool(0L, view){}
ZoomTool::~ZoomTool() {}

void ZoomTool::mousePress(const KImageShop::MouseEvent& e)
{
  if (!e.leftButton)
    return;

  float zf = m_pView->zoomFactor();

  if (zf == 0) zf = 1;

  if (e.shiftButton)
    zf/=2;
  else
    zf*=2;
  
  m_pView->slotSetZoomFactor(zf);

  int x = static_cast<int>(e.posX*zf - m_pView->viewWidth()/2);
  int y = static_cast<int>(e.posY*zf - m_pView->viewHeight()/2);

  if (x < 0) x = 0;
  if (y < 0) y = 0;

  m_pView->scrollTo(QPoint(x,y));
}

void ZoomTool::mouseMove(const KImageShop::MouseEvent& e){}
void ZoomTool::mouseRelease(const KImageShop::MouseEvent& e){}
