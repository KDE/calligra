/*
 *  linetool.cc - part of KImageShop
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
#include "kis_color.h"
#include "kis_canvas.h"
#include "kis_gradient.h"
#include "kis_tool_line.h"


LineTool::LineTool( KisDoc* _doc, KisView* _view, 
    KisCanvas* _canvas, KisGradient* _gradient )
  : KisTool( _doc, _view )
  , m_dragging( false )
  , m_pCanvas( _canvas )
  , m_gradient( _gradient )
{
}

LineTool::~LineTool()
{
}


void LineTool::mousePress( QMouseEvent* event )
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


void LineTool::mouseMove( QMouseEvent* event )
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


void LineTool::mouseRelease( QMouseEvent* event )
{
    if ( m_pDoc->isEmpty() )
        return;

    if(( m_dragging) 
    && ( event->state() == LeftButton))
    {
        drawLine( m_dragStart, m_dragEnd );
        m_dragging = false;
        drawLine( m_dragStart, m_dragEnd );
    }
}


void LineTool::drawLine( const QPoint& start, const QPoint& end )
{
    QPainter p;

    p.begin( m_pCanvas );
    p.setRasterOp( Qt::NotROP );
    p.drawLine( start, end );
    p.end();
}


void LineTool::drawGradientLine( const QPoint& start, const QPoint& end )
{
    KisImage * img = m_pDoc->current();
    if (!img)	return;
    int swap = 0;
    int x1, x2, y1, y2;

    x1 = start.x();
    y1 = start.y();
    x2 = end.x();
    y2 = end.y();

    if( x1 > x2 ) 
    {
        //swap( x1, x2 );
        swap = x1; x1 = x2; x2 = swap; 
    }    
    if( y1 > y2 )
    {
        //swap( y1, y2 );
        swap = y1; y1 = y2; y2 = swap;         
    }    
    // draw gradient

    int length = y2 - y1;

    // int length = sqrt( sqr(x1 - x2) + sqr(y1 - y2) );

    KisColor startColor = m_pView->fgColor();
    KisColor endColor   = m_pView->bgColor();

    if( ( startColor.native() == cs_RGB ) &&
        ( endColor.native() == cs_RGB ) )
    {
        // draw RGB gradient
        uint color = 0;
        
        // -jwc- need to define getPixel by color in kis_layer.cc        
        uchar r = img->getCurrentLayer()->pixel(0, x1, y1 );
        uchar g = img->getCurrentLayer()->pixel(1, x1, y1 );
        uchar b = img->getCurrentLayer()->pixel(2, x1, y1 );
        
        color = (((uint)r << 16) + ((uint)g << 8) + (b));        
        uint mask = color & 0xFF000000;

        int rDiff = ( endColor.R() - startColor.R() );
        int gDiff = ( endColor.G() - startColor.G() );
        int bDiff = ( endColor.B() - startColor.B() );
  
        if( rDiff > 0 ) kdDebug() <<  "rDiff ist positiv"  << endl;
        if( gDiff > 0 ) kdDebug() <<  "gDiff ist positiv"  << endl;
        if( bDiff > 0 ) kdDebug() <<  "bDiff ist positiv"  << endl;
  
        register int rl = rDiff << 16;
        register int gl = gDiff << 16;
        register int bl = bDiff << 16;
   
        int rcDelta = ( 1<<16 ) / length * rDiff;
        int gcDelta = ( 1<<16 ) / length * gDiff;
        int bcDelta = ( 1<<16 ) / length * bDiff;
    
        // draw parallelogram
        for( int y = y1 ; y < y2 ; y++ )
        {
            // calc color
            rl += rcDelta;
            gl += gcDelta;
            bl += bcDelta;

            color = mask |  ( ( rl>>16 ) * 0x010000 ) |
                            ( ( gl>>16 ) * 0x000100 ) |
                              ( bl>>16 );

            // draw line of color
            for( int x = x1 ; x < x2 ; x++ )
            {
                // -jwc- need to define setPixel by color in kis_layer.cc
                img->getCurrentLayer()->setPixel(0, x, y, (color & 0x00ff0000) >> 16 );
                img->getCurrentLayer()->setPixel(1, x, y, (color & 0x0000ff00) >>  8 );
                img->getCurrentLayer()->setPixel(2, x, y, (color & 0x000000ff) );                
            }
        }
    
        QRect updateRect( x1, y1, x2, y2 );
        img->markDirty(updateRect);
    }
}

