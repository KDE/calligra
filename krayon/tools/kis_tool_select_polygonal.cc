/*
 *  kis_tool_select_polygonal.h - part of Krayon
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
#include "kis_canvas.h"
#include "kis_cursor.h"
#include "kis_tool_select_polygonal.h"


PolygonalSelectTool::PolygonalSelectTool( KisDoc* _doc, 
    KisView* _view, KisCanvas* _canvas )
  : KisTool( _doc, _view)
  , m_dragging( false ) 
  , m_view( _view )  
  , m_canvas( _canvas )

{
    m_dragStart = QPoint(-1,-1);
    m_dragEnd =   QPoint(-1,-1);

    mStart  = QPoint(-1, -1);
    mFinish = QPoint(-1, -1);     
      
    m_Cursor = KisCursor::selectCursor();
}

PolygonalSelectTool::~PolygonalSelectTool()
{
}


void PolygonalSelectTool::start(QPoint p)
{
    mStart = p;
}


void PolygonalSelectTool::finish(QPoint p)
{
    mFinish = p;
    drawLine( mStart, mFinish );
}


void PolygonalSelectTool::clearOld()
{
   if (m_pDoc->isEmpty()) return;
        
   // if(m_dragStart.x() != -1)
        // drawRect( m_dragStart, m_dragEnd ); 

    // clear everything in 
    QRect updateRect(0, 0, m_pDoc->current()->width(), 
        m_pDoc->current()->height());
    m_view->updateCanvas(updateRect);

    m_dragStart = QPoint(-1,-1);
    m_dragEnd =   QPoint(-1,-1);
}

void PolygonalSelectTool::mousePress( QMouseEvent* event )
{
    if ( m_pDoc->isEmpty() )
        return;

    // start the polyline, and/or complete the segment
    if( event->button() == LeftButton )
    {
        if( m_dragging )
        {
            // erase old line on canvas
            drawLine( m_dragStart, m_dragEnd );

            // get current position
            m_dragEnd = event->pos();

            // draw new and final line for this segment
            drawLine( m_dragStart, m_dragEnd );
            
            // here we need to add the point to the point array
            // so it can be passed to the selection class to determine
            // selection area and bounds.

            // draw final line into layer
            //KisPainter *p = m_pView->kisPainter();
            //p->drawLine(zoomed(m_dragStart.x()), zoomed(m_dragStart.y()),
            //    zoomed(m_dragEnd.x()),   zoomed(m_dragEnd.y()));
        }
        else
        {
            start(event->pos());
            // todo: add the start point to the point array
        }
        
        m_dragging = true;
        m_dragStart = event->pos();
        m_dragEnd = event->pos();
    }
    // stop drawing on right or middle click
    else
    {   
        m_dragging = false;
        finish(event->pos());

        // need to connect start and end positions to close the
        // polyline 
        
        // we need a bounding rectangle and a point array of 
        // points in the polyline
        // m_pDoc->getSelection()->setBounds(m_selectRect);        
    }    
}


void PolygonalSelectTool::mouseMove( QMouseEvent* event )
{
    if (m_pDoc->isEmpty()) return;

    if( m_dragging )
    {
        drawLine( m_dragStart, m_dragEnd );
        m_dragEnd = event->pos();
        drawLine( m_dragStart, m_dragEnd );
    }
}


void PolygonalSelectTool::mouseRelease( QMouseEvent * /* event */ )
{

}


void PolygonalSelectTool::drawLine( const QPoint& start, const QPoint& end )
{
    int lineThickness = 1;
    
    QPainter p;
    QPen pen;
    pen.setWidth(lineThickness);
    
    p.begin( m_canvas );
    p.setPen(pen);
    p.setRasterOp( Qt::NotROP );
    float zF = m_pView->zoomFactor();

    p.drawLine( QPoint( start.x() + m_pView->xPaintOffset() 
                          - (int)(zF * m_pView->xScrollOffset()),
                        start.y() + m_pView->yPaintOffset() 
                           - (int)(zF * m_pView->yScrollOffset())), 
                QPoint( end.x() + m_pView->xPaintOffset() 
                          - (int)(zF * m_pView->xScrollOffset()),
                        end.y() + m_pView->yPaintOffset() 
                           - (int)(zF * m_pView->yScrollOffset())) );

    p.end();
}


