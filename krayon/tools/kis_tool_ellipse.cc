/*
 *  kis_tool_ellipse.cc - part of Krayon
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
#include "kis_tool_ellipse.h"
#include "kis_dlg_toolopts.h"

EllipseTool::EllipseTool( KisDoc* _doc, KisView* _view, KisCanvas* _canvas)
  : KisTool( _doc, _view )
  , m_dragging( false )
  , pCanvas( _canvas )
{
    m_pDoc = _doc;

    // initialize ellipse tool settings
    KisDoc::EllipseToolSettings s = m_pDoc->getEllipseToolSettings();
    lineThickness = s.thickness;
    lineOpacity = s.opacity;
    usePattern = s.useCurrentPattern;
    useGradient = s.fillWithGradient;
    fillSolid = s.fillInteriorRegions;


    KisPainter *p = m_pView->kisPainter();
    
    p->setLineThickness( lineThickness );
    p->setLineOpacity( lineOpacity );
    p->setFilledEllipse( fillSolid );
    p->setGradientFill( useGradient );
    p->setPatternFill( usePattern );
}

EllipseTool::~EllipseTool()
{
}


void EllipseTool::mousePress( QMouseEvent* event )
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


void EllipseTool::mouseMove( QMouseEvent* event )
{
    if ( m_pDoc->isEmpty() )
        return;

    if( m_dragging )
    {
        // erase old ellipse on canvas
        drawEllipse( m_dragStart, m_dragEnd );
        // get current mouse position
        m_dragEnd = event->pos();
        // draw new ellipse on canvas
        drawEllipse( m_dragStart, m_dragEnd );
    }
}


void EllipseTool::mouseRelease( QMouseEvent* event )
{
    if ( m_pDoc->isEmpty() )
        return;

    if(( m_dragging) 
    && ( event->state() == LeftButton))
    {
        // erase old ellipse on canvas
        drawEllipse( m_dragStart, m_dragEnd );
        m_dragging = false;

        // draw final ellipse onto layer    
        QRect rect(zoomed(m_dragStart), zoomed(m_dragEnd));
        KisPainter *p = m_pView->kisPainter();
        p->drawEllipse( rect );
    }    
}


void EllipseTool::drawEllipse( const QPoint & start, const QPoint &end )
{
    QPainter p;
    QPen pen;
    pen.setWidth(lineThickness);
    
    p.begin( pCanvas );
    p.setPen(pen);
    p.setRasterOp( Qt::NotROP );
    float zF = m_pView->zoomFactor();

    p.drawEllipse( QRect(start.x() + m_pView->xPaintOffset() 
                                - (int)(zF * m_pView->xScrollOffset()),
                      start.y() + m_pView->yPaintOffset() 
                                - (int)(zF * m_pView->yScrollOffset()), 
                      end.x() - start.x(), 
                      end.y() - start.y()) );

    p.end();
}

void EllipseTool::optionsDialog()
{
    ToolOptsStruct ts;    
    
    ts.usePattern       = usePattern;
    ts.useGradient      = useGradient;
    ts.lineThickness    = lineThickness;
    ts.lineOpacity      = lineOpacity;
    ts.fillShapes       = fillSolid;
    ts.opacity          = lineOpacity;

    bool old_usePattern       = usePattern;
    bool old_useGradient      = useGradient;
    int  old_lineThickness    = lineThickness;
    int  old_lineOpacity      = lineOpacity;
    bool old_fillSolid        = fillSolid;
        
    ToolOptionsDialog *pOptsDialog 
        = new ToolOptionsDialog(tt_linetool, ts);

    pOptsDialog->exec();
    
    if(!pOptsDialog->result() == QDialog::Accepted)
        return;
    else {
        lineThickness = pOptsDialog->lineToolTab()->thickness();
        lineOpacity   = pOptsDialog->lineToolTab()->opacity();
        usePattern    = pOptsDialog->lineToolTab()->usePattern();
        fillSolid     = pOptsDialog->lineToolTab()->solid(); 
        useGradient   = pOptsDialog->lineToolTab()->useGradient();

        // User change value ?
        if ( old_usePattern != usePattern || old_useGradient != useGradient 
             || old_lineOpacity != lineOpacity || old_lineThickness != lineThickness
             || old_fillSolid != fillSolid ) {    
            KisPainter *p = m_pView->kisPainter();
    
            p->setLineThickness( lineThickness );
            p->setLineOpacity( lineOpacity );
            p->setFilledEllipse( fillSolid );
            p->setPatternFill( usePattern );
            p->setGradientFill( useGradient );

            // set ellipse tool settings
            KisDoc::EllipseToolSettings s = m_pDoc->getEllipseToolSettings();
            s.thickness            = lineThickness;
            s.opacity              = lineOpacity;
            s.useCurrentPattern    = usePattern;
            s.fillWithGradient     = useGradient;
            s.fillInteriorRegions  = fillSolid;

            m_pDoc->setEllipseToolSettings( s );

            m_pDoc->setModified( true );
        }
    }    
}


