/*
 *  gradienttool.h - part of KImageShop
 *
 *  Copyright (c) 1999 Michael Koch <koch@kde.org>
 * Copyright (c) 2000 John Califf <jcaliff@compuzone.net>
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
#include "kis_tool_gradient.h"


GradientTool::GradientTool( KisDoc* _doc, KisView* _view, 
    KisCanvas* _canvas, KisGradient* _gradient )
  : KisTool( _doc, _view )
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

    if(( m_dragging) 
    && ( event->state() == LeftButton))
    {
        drawLine( m_dragStart, m_dragEnd );
        m_dragging = false;
        drawGradient( m_dragStart, m_dragEnd );
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


void GradientTool::drawGradient( const QPoint& start, const QPoint& end )
{
    KisImage * img = m_pDoc->current();
    if (!img)   return;
    int swap = 0;
    int x1, x2, y1, y2;

    x1 = start.x();
    y1 = start.y();
    x2 = end.x();
    y2 = end.y();

    // establish rectanglular area to apply gradient to
    // based on line endpoints
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
    
    // draw gradient within rectanguar area defined above
    // beware of divide by zero errors here!
    int length = (y2 - y1) > 0 ? y2 - y1 : 1;
    
    KisColor startColor = m_pView->fgColor();
    KisColor endColor   = m_pView->bgColor();

    if(( startColor.native() == cs_RGB ) && ( endColor.native() == cs_RGB ))
    {
        int rDiff = ( endColor.R() - startColor.R() );
        int gDiff = ( endColor.G() - startColor.G() );
        int bDiff = ( endColor.B() - startColor.B() );
  
        int rl = startColor.R();
        int gl = startColor.G();
        int bl = startColor.B();
 
        float rlFloat = (float)rl;
        float glFloat = (float)gl;
        float blFloat = (float)bl;

        // draw rect - can add a shape mask to this also based
        // on current selection within rectangular area
        
        // gradient defined vertically
        for( int y = y1 ; y < y2 ; y++ )
        {
            // calc color
            float rlFinFloat 
                = rlFloat + ((float) (y - y1) * (float)rDiff) / (float)length;
            float glFinFloat 
                = glFloat + ((float) (y - y1) * (float)gDiff) / (float)length;
            float blFinFloat 
                = blFloat + ((float) (y - y1) * (float)bDiff) / (float)length;

            // draw uniform horizontal line of color - 
            for( int x = x1 ; x < x2 ; x++ )
            {
                img->getCurrentLayer()->setPixel(0, x, y, (int)rlFinFloat);
                img->getCurrentLayer()->setPixel(1, x, y, (int)glFinFloat);
                img->getCurrentLayer()->setPixel(2, x, y, (int)blFinFloat);
            }
        }
    
        QRect updateRect( x1, y1, x2, y2 );
        img->markDirty(updateRect);
    }
}

