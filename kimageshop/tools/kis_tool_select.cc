/*
 *  selecttool.h - part of KImageShop
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

#include "qpainter.h"

#include "kis_doc.h"
#include "kis_view.h"
#include "kis_canvas.h"
#include "kis_tool_select.h"

SelectTool::SelectTool( KisDoc* _doc, KisView* _view, KisCanvas* _canvas )
  : KisTool( _doc, _view)
  , m_dragging( false ) 
  , m_canvas( _canvas )
  , m_view( _view )
{
      m_drawn = false;
      m_init  = true;
      m_dragStart = QPoint(-1,-1);
      m_dragEnd =   QPoint(-1,-1);
}

SelectTool::~SelectTool()
{
}


void SelectTool::mousePress( QMouseEvent* event )
{
    if ( m_pDoc->isEmpty() )
        return;

    if( event->button() == LeftButton )
    {
        if(m_drawn && !m_init) // erase old rectangle
        {
            m_drawn = false;
            if(m_dragStart.x() != -1)
                drawRect( m_dragStart, m_dragEnd ); 
        }
        
        m_init = false;
        m_dragging = true;
        m_dragStart = event->pos();
        m_dragEnd = event->pos();
    }
}


void SelectTool::mouseMove( QMouseEvent* event )
{
    if ( m_pDoc->isEmpty() )
        return;

    if( m_dragging )
    {
        drawRect( m_dragStart, m_dragEnd );
        m_dragEnd = event->pos();
        drawRect( m_dragStart, m_dragEnd );
    }
}


void SelectTool::mouseRelease( QMouseEvent* event )
{
    if ( m_pDoc->isEmpty() )
        return;

    if( ( m_dragging ) && ( event->button() == LeftButton ) )
    {
        m_dragging = false;
        m_drawn = true;

        /* jwc - leave selection rectange boundary on screen
        it is only drawn to canvas, not to retained imagePixmap,
        and therefore will disappear when another tool action is used */
        // drawRect( m_dragStart, m_dragEnd ); 
    }
}


void SelectTool::drawRect( const QPoint& start, const QPoint& end )
{
    // QPixmap *imPM = m_pDoc->current()->imagePixmap;
    QPainter p, pCanvas;

    p.begin( m_canvas );
    //p.begin( imPM );
    p.setRasterOp( Qt::NotROP );
    p.drawRect( QRect( start, end ) );
    p.end();
    
    // jwc - don't update retained graphics, only canvas
    //QRect updateRect(0, 0, m_pDoc->current()->width(), m_pDoc->current()->height());
    //m_view->updateCanvas(updateRect);
}


