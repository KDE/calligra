/*
 *  kis_tool_select_freehand.cc - part of Krayon
 *
 *  Copyright (c) 2001 Toshitaka Fujioka <fujioka@kde.org>
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
#include "kis_tool_select_freehand.h"


FreehandSelectTool::FreehandSelectTool( KisDoc* _doc, KisView* _view, KisCanvas* _canvas )
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

    m_index = 0;
    m_dragging = false;
}

FreehandSelectTool::~FreehandSelectTool()
{
}


void FreehandSelectTool::start( QPoint p )
{
    mStart = p;
}


void FreehandSelectTool::finish( QPoint p )
{
    mFinish = p;
    drawLine( mStart, mFinish );
    m_pointArray.putPoints( m_index, 1, mFinish.x(),mFinish.y() );
}


void FreehandSelectTool::clearOld()
{
   if (m_pDoc->isEmpty()) return;

   // clear everything in 
   QRect updateRect(0, 0, m_pDoc->current()->width(), m_pDoc->current()->height());
   m_view->updateCanvas(updateRect);

   m_dragStart = QPoint(-1,-1);
   m_dragEnd =   QPoint(-1,-1);
}

void FreehandSelectTool::mousePress( QMouseEvent* event )
{
    if ( m_pDoc->isEmpty() )
        return;

    // start the freehand line.
    if( event->button() == LeftButton ) {
        m_dragging = true;
        clearOld();
        start( event->pos() );
        
        m_dragStart = event->pos();
        m_dragEnd = event->pos();
    }
}


void FreehandSelectTool::mouseMove( QMouseEvent* event )
{
    if (m_pDoc->isEmpty()) return;

    if( m_dragging ) {
        m_dragEnd = event->pos();

        m_pointArray.putPoints( m_index, 1, m_dragStart.x(),m_dragStart.y() );
        ++m_index;

        drawLine( m_dragStart, m_dragEnd );
        m_dragStart = m_dragEnd;
    }
}


void FreehandSelectTool::mouseRelease( QMouseEvent* event )
{
    // stop drawing freehand.
    m_dragging = false;

    QRect rect = getDrawRect( m_pointArray );
    QPointArray points = zoomPointArray( m_pointArray );

    // need to connect start and end positions to close the freehand line.
    finish( event->pos() );
        
    // we need a bounding rectangle and a point array of 
    // points in the freehand line        

    m_pDoc->getSelection()->setPolygonalSelection( rect, points, m_pDoc->current()->getCurrentLayer());

    kdDebug(0) << "selectRect" 
               << " left: "   << rect.left() 
               << " top: "    << rect.top()
               << " right: "  << rect.right() 
               << " bottom: " << rect.bottom()
               << endl;

    // Initialize
    m_index = 0;
    m_pointArray.resize( 0 );
}


void FreehandSelectTool::drawLine( const QPoint& start, const QPoint& end )
{
    QPainter p;
    
    p.begin( m_canvas );
    p.setRasterOp( Qt::NotROP );
    p.setPen( QPen( Qt::DotLine ) );
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

// get QRect for draw freehand in layer.
QRect FreehandSelectTool::getDrawRect( QPointArray & points )
{
    int maxX = 0, maxY = 0;
    int minX = 0, minY = 0;
    int tmpX = 0, tmpY = 0;
    bool first = true;

    QPointArray::Iterator it;
    for ( it = points.begin(); it != points.end(); ++it ) {
        QPoint point = (*it);
        tmpX = point.x();
        tmpY = point.y();

        if ( first ) {
            maxX = tmpX;
            maxY = tmpY;
            minX = tmpX;
            minY = tmpY;

            first = false;
        }

        if ( maxX < tmpX )
            maxX = tmpX;
        if ( maxY < tmpY )
            maxY = tmpY;
        if ( minX > tmpX )
            minX = tmpX;
        if ( minY > tmpY )
            minY = minY;
    }

    QPoint topLeft = QPoint( minX, minY );
    QPoint bottomRight = QPoint( maxX, maxY );
    QRect rect = QRect( zoomed( topLeft ), zoomed( bottomRight ) );

    return rect;
}

// get QPointArray for draw freehand in layer.
QPointArray FreehandSelectTool::zoomPointArray( QPointArray & points )
{
    QPointArray m_points( points.size() );

    int count = 0;
    QPointArray::Iterator it;
    for ( it = points.begin(); it != points.end(); ++it ) {
        m_points.setPoint( count, zoomed( *it ) );
        ++count;
    }

    return m_points;
}

