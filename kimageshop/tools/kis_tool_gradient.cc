/*
 *  gradienttool.h - part of KImageShop
 *
 *  Copyright (c) 1999 Michael Koch <koch@kde.org>
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

#include "kis_doc.h"
#include "kis_canvas.h"
#include "kis_gradient.h"
#include "kis_tool_gradient.h"

GradientTool::GradientTool( KisDoc* _doc, KisCanvas* _canvas, KisGradient* _gradient )
  : KisTool( _doc )
  , m_dragging( false )
  , m_pCanvas( _canvas )
  , m_gradient( _gradient )
{
}

GradientTool::~GradientTool()
{
}

void GradientTool::mousePress( QMouseEvent* event )
{
  if ( m_pDoc->isEmpty() )
    return;

  if( event->button() == LeftButton )
  {
    m_dragging = true;
    m_dragStart = event->pos();
    m_dragEnd = event->pos();
  }
}

void GradientTool::mouseMove( QMouseEvent* event )
{
  if ( m_pDoc->isEmpty() )
    return;

  if( m_dragging )
  {
    drawLine( m_dragStart, m_dragEnd );
    m_dragEnd = event->pos();
    drawLine( m_dragStart, m_dragEnd );
  }
}

void GradientTool::mouseRelease( QMouseEvent* event )
{
  if ( m_pDoc->isEmpty() )
    return;

  if( ( m_dragging ) &&
      ( event->state() == LeftButton ) )
  {
    drawLine( m_dragStart, m_dragEnd );
    m_dragging = false;
  }
}

void GradientTool::drawLine( const QPoint& start, const QPoint& end )
{
  QPainter p;

  p.begin( m_pCanvas );
  p.setRasterOp( Qt::NotROP );
  p.drawLine( start, end );
  p.end();
}
