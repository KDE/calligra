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
#include <qregion.h>
#include <kdebug.h>

#include "kis_doc.h"
#include "kis_view.h"
#include "kis_canvas.h"
#include "kis_vec.h"
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
      
    setSelectCursor();

    m_index = 0;
    m_dragging = false;
    moveSelectArea = false;
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
    if( event->button() == LeftButton && !moveSelectArea ) {
        m_dragging = true;
        clearOld();
        start( event->pos() );
        
        m_dragStart = event->pos();
        m_dragEnd = event->pos();
        dragSelectArea = false;
    }
    else if( event->button() == LeftButton && moveSelectArea ) {
        dragSelectArea = true;
        dragFirst = true;
        m_dragStart = event->pos();
        m_dragdist = 0;

        m_hotSpot = event->pos();
        int x = zoomed( m_hotSpot.x() );
        int y = zoomed( m_hotSpot.y() );
        m_hotSpot = QPoint( x - m_imageRect.topLeft().x(), y - m_imageRect.topLeft().y() );

        oldDragPoint = event->pos();
        setClipImage();
    }
}


void FreehandSelectTool::mouseMove( QMouseEvent* event )
{
    if (m_pDoc->isEmpty()) return;

    if( m_dragging && !dragSelectArea ) {
        m_dragEnd = event->pos();

        m_pointArray.putPoints( m_index, 1, m_dragStart.x(),m_dragStart.y() );
        ++m_index;

        drawLine( m_dragStart, m_dragEnd );
        m_dragStart = m_dragEnd;
    }
    else if ( !m_dragging && !dragSelectArea ) {
        if ( !m_selectRegion.isNull() && m_selectRegion.contains( event->pos() ) ) {
            setMoveCursor();
            moveSelectArea = true;
        }
        else {
            setSelectCursor();
            moveSelectArea = false;
        }
    }
    else if ( dragSelectArea ) {
        if ( dragFirst ) {
            // remove select image
            m_pDoc->getSelection()->erase();

            // refresh canvas
            clearOld();
            m_pView->slotUpdateImage();
            dragFirst = false;
        }

        int spacing = 10;
        float zF = m_pView->zoomFactor();
        QPoint pos = event->pos();
        int mouseX = pos.x();
        int mouseY = pos.y();

        KisVector end( mouseX, mouseY );
        KisVector start( m_dragStart.x(), m_dragStart.y() );

        KisVector dragVec = end - start;
        float saved_dist = m_dragdist;
        float new_dist = dragVec.length();
        float dist = saved_dist + new_dist;

        if ( (int)dist < spacing ) {
            m_dragdist += new_dist;
            m_dragStart = pos;
            return;
        }
        else
            m_dragdist = 0;

        dragVec.normalize();
        KisVector step = start;

        while ( dist >= spacing ) {
            if ( saved_dist > 0 ) {
                step += dragVec * ( spacing - saved_dist );
                saved_dist -= spacing;
            }
            else
                step += dragVec * spacing;

            QPoint p( qRound( step.x() ), qRound( step.y() ) );

            QRect ur( zoomed( oldDragPoint.x() ) - m_hotSpot.x() - m_pView->xScrollOffset(),
                      zoomed( oldDragPoint.y() ) - m_hotSpot.y() - m_pView->yScrollOffset(),
                      (int)( clipPixmap.width() * ( zF > 1.0 ? zF : 1.0 ) ),
                      (int)( clipPixmap.height() * ( zF > 1.0 ? zF : 1.0 ) ) );

            m_pView->updateCanvas( ur );

            dragSelectImage( p );

            oldDragPoint = p;
            dist -= spacing;
        }

        if ( dist > 0 ) 
            m_dragdist = dist;
        m_dragStart = pos;
    }
}


void FreehandSelectTool::mouseRelease( QMouseEvent* event )
{
    if ( !moveSelectArea ) {
        // stop drawing freehand.
        m_dragging = false;

        m_imageRect = getDrawRect( m_pointArray );
        QPointArray points = zoomPointArray( m_pointArray );

        // need to connect start and end positions to close the freehand line.
        finish( event->pos() );
        
        // we need a bounding rectangle and a point array of 
        // points in the freehand line        

        m_pDoc->getSelection()->setPolygonalSelection( m_imageRect, points, m_pDoc->current()->getCurrentLayer());

        kdDebug(0) << "selectRect" 
                   << " left: "   << m_imageRect.left() 
                   << " top: "    << m_imageRect.top()
                   << " right: "  << m_imageRect.right() 
                   << " bottom: " << m_imageRect.bottom()
                   << endl;

        if ( m_pointArray.size() > 1 )
            m_selectRegion = QRegion( m_pointArray, true );
        else
            m_selectRegion = QRegion();

        // Initialize
        m_index = 0;
        m_pointArray.resize( 0 );
    }
    else {
        // Initialize
        dragSelectArea = false;
        m_selectRegion = QRegion();
        setSelectCursor();
        moveSelectArea = false;

        QPoint pos = event->pos();

        KisImage *img = m_pDoc->current();
        if ( !img )
            return;
        if( !img->getCurrentLayer()->visible() )
            return;
        if( pasteClipImage( zoomed( pos ) - m_hotSpot ) )
            img->markDirty( QRect( zoomed( pos ) - m_hotSpot, clipPixmap.size() ) );
    }
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

void FreehandSelectTool::setSelectCursor()
{
    m_Cursor = KisCursor::selectCursor();
    m_pView->kisCanvas()->setCursor( KisCursor::selectCursor() );
}

void FreehandSelectTool::setMoveCursor()
{
    m_Cursor = KisCursor::moveCursor();
    m_pView->kisCanvas()->setCursor( KisCursor::moveCursor() );
}

void FreehandSelectTool::setClipImage()
{
    // set select area clip
    if ( !m_pDoc->setClipImage() ) {
        kdDebug( 0 ) << "FreehandSelectTool::setClipImage(): m_pDoc->setClipImage() failed" << endl;
        return;
    }

    // get select area clip
    if ( m_pDoc->getClipImage() ) {
        kdDebug( 0 ) << "FreehandSelectTool::setClipImage(): m_pDoc->getClipImage() success!!" << endl;
        clipImage = *m_pDoc->getClipImage();

        if ( clipImage.isNull() ) {
            kdDebug( 0 ) << "FreehandSelectTool::setClipImage(): clip image is null!" << endl;
            return;
        }
        // if dealing with 1 or 8 bit image, convert to 16 bit
        if ( clipImage.depth() < 16 ) {
            QImage smoothImage = clipImage.smoothScale( clipImage.width(), clipImage.height() );
            clipImage = smoothImage;

            if ( clipImage.isNull() ) {
                kdDebug( 0 ) << "FreehandSelectTool::setClipImage(): smooth scale clip image is null!" << endl;
                return;
            }
        }

        clipPixmap.convertFromImage( clipImage, QPixmap::AutoColor );
        if ( clipPixmap.isNull() ) {
            kdDebug() << "FreehandSelectTool::setClipImage(): can't convert from image!" << endl;
            return;
        }

        if ( !clipImage.hasAlphaBuffer() )
            kdDebug( 0 ) << "FreehandSelectTool::setClipImage(): clip image has no alpha buffer!" << endl;
    }

    kdDebug( 0 ) << "FreehandSelectTool::setClipImage(): Success set up clip image!!" << endl;
}

void FreehandSelectTool::dragSelectImage( QPoint dragPoint )
{
    KisImage *img = m_pDoc->current();
    if ( !img )
        return;

    KisLayer *lay = img->getCurrentLayer();
    if ( !lay )
        return;

    float zF = m_pView->zoomFactor();
    int pX = dragPoint.x();
    int pY = dragPoint.y();
    pX = (int)( pX / zF );
    pY = (int)( pY / zF );
    QPoint point = QPoint( pX, pY );

    QPainter p;
    p.begin( m_canvas );
    p.scale( zF, zF );

    QRect imageRect( point.x() - m_hotSpot.x(), point.y() - m_hotSpot.y(), 
                     clipPixmap.width(), clipPixmap.height() );
    imageRect = imageRect.intersect( img->imageExtents() );

    if ( imageRect.top() > img->height() || imageRect.left() > img->width()
         || imageRect.bottom() < 0 || imageRect.right() < 0 ) {
        p.end();
        return;
    }

    if ( !imageRect.intersects( img->imageExtents() ) ) {
        p.end();
        return;
    }

    imageRect = imageRect.intersect( img->imageExtents() );

    int startX = 0;
    int startY = 0;

    if ( clipPixmap.width() > imageRect.right() )
        startX = clipPixmap.width() - imageRect.right();
    if ( clipPixmap.height() > imageRect.bottom() )
        startY = clipPixmap.height() - imageRect.bottom();

    // paranioa
    if( startX < 0 ) 
        startX = 0;
    if( startY < 0 )
        startY = 0;
    if( startX > clipPixmap.width() )
        startX = clipPixmap.width();
    if( startY > clipPixmap.height() )
        startY = clipPixmap.height();

    int xt = m_pView->xPaintOffset() - m_pView->xScrollOffset();
    int yt = m_pView->yPaintOffset() - m_pView->yScrollOffset();

    p.translate( xt, yt );

    p.drawPixmap( imageRect.left(), imageRect.top(),
                  clipPixmap,
                  startX, startY,
                  imageRect.width(), imageRect.height() );

    p.end();
}

bool FreehandSelectTool::pasteClipImage( QPoint pos )
{
    KisImage *img = m_pDoc->current();
    if ( !img )
        return false;

    KisLayer *lay = img->getCurrentLayer();
    if ( !lay )
        return false;

    QImage *qimg = &clipImage;

    int startx = pos.x();
    int starty = pos.y();

    QRect clipRect( startx, starty, qimg->width(), qimg->height() );

    if ( !clipRect.intersects( img->getCurrentLayer()->imageExtents() ) )
        return false;

    clipRect = clipRect.intersect( img->getCurrentLayer()->imageExtents() );

    int sx = clipRect.left() - startx;
    int sy = clipRect.top() - starty;
    int ex = clipRect.right() - startx;
    int ey = clipRect.bottom() - starty;

    uchar r, g, b, a;
    int   v = 255;
    int   bv = 0;

    int red     = m_pView->fgColor().R();
    int green   = m_pView->fgColor().G();
    int blue    = m_pView->fgColor().B();

    bool grayscale = false;
    bool colorBlending = false;
    bool layerAlpha = ( img->colorMode() == cm_RGBA );
    bool imageAlpha = qimg->hasAlphaBuffer();

    for ( int y = sy; y <= ey; ++y ) {
        for (int x = sx; x <= ex; ++x) {
            // destination binary values by channel
            r = lay->pixel(0, startx + x, starty + y);
            g = lay->pixel(1, startx + x, starty + y);
            b = lay->pixel(2, startx + x, starty + y);

            // pixel value in scanline at x offset to right
            uint *p = (uint *)qimg->scanLine(y) + x;

            // if the alpha value of the pixel in the selection
            // image is 0, don't paint the pixel.  It's transparent.
            if( ( imageAlpha ) && ( ( (*p) >> 24 ) == 0 ) ) 
                continue;

            if( colorBlending ) {
                // make mud!
                lay->setPixel( 0, startx + x, starty + y, ( qRed(*p) + r + red ) / 3 );
                lay->setPixel( 1, startx + x, starty + y, ( qGreen(*p) + g + green ) / 3 );
                lay->setPixel( 2, startx + x, starty + y, ( qBlue(*p) + b + blue ) / 3 );
            }
            else {
                // set layer pixel to be same as image
                lay->setPixel( 0, startx + x, starty + y, qRed(*p) );
                lay->setPixel( 1, startx + x, starty + y, qGreen(*p) );
                lay->setPixel( 2, startx + x, starty + y, qBlue(*p) );
            }

            if ( layerAlpha ) {
                a = lay->pixel(3, startx + x, starty + y);
                if( grayscale ) {
                    v = a + bv;
                    if ( v < 0 )
                        v = 0;
                    if ( v > 255 )
                        v = 255;
                    a = (uchar) v;
                }

                lay->setPixel( 3, startx + x, starty + y, a );
            }
        }
    }

    return true;
}
