/*
 *  kis_tool_rectangle.cc - part of Krayon
 *
 *  Copyright (c) 2000 John Califf <jcaliff@compuzone.net>
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
#include <kdebug.h>

#include "kis_doc.h"
#include "kis_view.h"
#include "kis_painter.h"
#include "kis_color.h"
#include "kis_canvas.h"
#include "kis_tool_rectangle.h"
#include "opts_line_dlg.h"

RectangleTool::RectangleTool( KisDoc* _doc, KisView* _view, KisCanvas* _canvas)
  : KisTool( _doc, _view )
  , m_dragging( false )
  , pCanvas( _canvas )
{
    lineThickness = 4;
    lineOpacity = 255;
    fillSolid = false;
}

RectangleTool::~RectangleTool()
{
}


void RectangleTool::mousePress( QMouseEvent* event )
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


void RectangleTool::mouseMove( QMouseEvent* event )
{
    if ( m_pDoc->isEmpty() )
        return;

    if( m_dragging )
    {
        // erase old lines on canvas
        drawRectangle( m_dragStart, m_dragEnd );
        // get current mouse position
        m_dragEnd = event->pos();
        // draw new lines on canvas
        drawRectangle( m_dragStart, m_dragEnd );
    }
}


void RectangleTool::mouseRelease( QMouseEvent* event )
{
    if ( m_pDoc->isEmpty() )
        return;

    if(( m_dragging) 
    && ( event->state() == LeftButton))
    {
        // erase old lines on canvas
        drawRectangle( m_dragStart, m_dragEnd );
        m_dragging = false;
    }
    
    // draw final lines onto layer
    KisPainter *p = m_pView->kisPainter();
    QRect rect(zoomed(m_dragStart), zoomed(m_dragEnd)) ;
    p->drawRectangle( rect );
}


void RectangleTool::drawRectangle( const QPoint& start, const QPoint& end )
{
    QPainter p;
    QPen pen;
    pen.setWidth(lineThickness);
    
    p.begin( pCanvas );
    p.setPen(pen);
    p.setRasterOp( Qt::NotROP );
    float zF = m_pView->zoomFactor();
    p.drawRect( QRect(start.x() + m_pView->xPaintOffset() 
                                - (int)(zF * m_pView->xScrollOffset()),
                      start.y() + m_pView->yPaintOffset() 
                                - (int)(zF * m_pView->yScrollOffset()), 
                      end.x() - start.x(), 
                      end.y() - start.y()) );
    p.end();
}

void RectangleTool::optionsDialog()
{
    LineOptionsDialog *pOptsDialog 
        = new LineOptionsDialog(fillSolid, lineThickness, lineOpacity);
    pOptsDialog->exec();
    if(!pOptsDialog->result() == QDialog::Accepted)
        return;

    lineThickness = pOptsDialog->thickness();
    lineOpacity   = pOptsDialog->opacity();
    fillSolid     = pOptsDialog->solid();
        
    KisPainter *p = m_pView->kisPainter();
    p->setLineThickness(lineThickness);
    p->setLineOpacity(lineOpacity);
    p->setFilledRectangle(fillSolid);
}
