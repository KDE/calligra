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

#include "kis_doc.h"
#include "kis_view.h"
#include "kis_tool_zoom.h"

ZoomTool::ZoomTool( KisView *view )
  : KisTool( 0L, view )
{
}

ZoomTool::~ZoomTool()
{
}

void ZoomTool::mousePress(QMouseEvent *e)
{
  if( (e->button() != LeftButton) &&
      (e->button() != RightButton) )
    return;

  if( e->button() == LeftButton )
      m_pView->zoom_in();
  else
      m_pView->zoom_out();
}

void ZoomTool::mouseMove(QMouseEvent *)
{
}

void ZoomTool::mouseRelease(QMouseEvent *)
{
}
