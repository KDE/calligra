/*
 *  polygontool.cc - part of Krayon
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
#include <qpointarray.h>

#include <kdebug.h>

#include <math.h>

#include "kis_doc.h"
#include "kis_view.h"
#include "kis_painter.h"
#include "kis_color.h"
#include "kis_canvas.h"
#include "kis_tool_polygon.h"
#include "kis_dlg_toolopts.h"

PolyGonTool::PolyGonTool( KisDoc* _doc, KisView* _view, KisCanvas* _canvas)
  : KisTool( _doc, _view )
  , m_dragging( false )
  , pCanvas( _canvas )
{
    m_pDoc = _doc;

    // initialize polygon tool settings
    KisDoc::PolygonToolSettings s = m_pDoc->getPolyGonToolSettings();
    lineThickness = s.thickness;
    lineOpacity = s.opacity;
    cornersValue = s.corners;
    sharpnessValue = s.sharpness;
    usePattern = s.useCurrentPattern;
    useGradient = s.fillWithGradient;
    useRegions = s.fillInteriorRegions;
    checkPolygon = s.polygon;
    checkConcavePolygon = s.concavePolygon;
        
    mStart  = QPoint(-1, -1);
    mFinish = QPoint(-1, -1);     
}


PolyGonTool::~PolyGonTool()
{
}

void PolyGonTool::mousePress( QMouseEvent* event )
{
    if ( m_pDoc->isEmpty() )
        return;

    if( event->button() == LeftButton ) {
        m_dragging = true;
        m_dragStart = event->pos();
        m_dragEnd = event->pos();
    }
}


void PolyGonTool::mouseMove( QMouseEvent* event )
{
    if ( m_pDoc->isEmpty() )
        return;

    if( m_dragging ) {
        // erase old polygon on canvas
        drawPolygon( m_dragStart, m_dragEnd );
        // get current mouse position
        m_dragEnd = event->pos();
        // draw new polygon on canvas
        drawPolygon( m_dragStart, m_dragEnd );
    }
}


void PolyGonTool::mouseRelease( QMouseEvent* event  )
{
    if ( m_pDoc->isEmpty() )
        return;

    if( m_dragging && event->state() == LeftButton ) {
        // erase old polygon on canvas
        drawPolygon( m_dragStart, m_dragEnd );
        m_dragging = false;

        // draw final polygon onto layer 
        KisPainter *p = m_pView->kisPainter();
        QRect rect = getDrawRect( drawPoints );
        QPointArray points = zoomPointArray( drawPoints );
        p->drawPolygon( points, rect );
    }
}


void PolyGonTool::drawPolygon( const QPoint& start, const QPoint& end )
{
    QPainter p;
    QPen pen;
    pen.setWidth( lineThickness );
    
    p.begin( pCanvas );
    p.setPen( pen );
    p.setRasterOp( Qt::NotROP );
    float zF = m_pView->zoomFactor();

    double angle = 2 * M_PI / cornersValue;
    float dx = (float) ::fabs( start.x () - end.x () );
    float dy = (float) ::fabs( start.y () - end.y () );
    float radius = (dx > dy ? dx / 2.0 : dy / 2.0);
    float xoff = start.x() + ( start.x() < end.x() ? radius : -radius )
                 + m_pView->xPaintOffset() - (int)( zF * m_pView->xScrollOffset() );
    float yoff = start.y() + ( start.y() < end.y() ? radius : -radius )
                 + m_pView->yPaintOffset() - (int)( zF * m_pView->yScrollOffset() );

    float xoff_draw = start.x() + ( start.x() < end.x() ? radius : -radius );
    float yoff_draw = start.y() + ( start.y() < end.y() ? radius : -radius );

    QPointArray points( checkConcavePolygon ? cornersValue * 2 : cornersValue );
    points.setPoint( 0, (int) xoff, (int) ( -radius + yoff ) );

    // for draw layer
    QPointArray m_drawPoints( checkConcavePolygon ? cornersValue * 2 : cornersValue );
    m_drawPoints.setPoint( 0, (int) xoff_draw, (int) ( -radius + yoff_draw ) );

    if ( checkConcavePolygon ) {
        angle = angle / 2.0;
        double a = angle;
        double r = radius - ( sharpnessValue / 100.0 * radius );
        for ( int i = 1; i < cornersValue * 2; ++i ) {
            double xp, yp;
            if ( i % 2 ) {
                xp =  r * sin( a );
                yp = -r * cos( a );
            }
            else {
                xp = radius * sin( a );
                yp = -radius * cos( a );
            }
            a += angle;
            points.setPoint( i, (int) ( xp + xoff ), (int) ( yp + yoff ) );
            m_drawPoints.setPoint( i, (int) ( xp + xoff_draw ), (int) ( yp + yoff_draw ) );
        }
    }
    else {
        double a = angle;
        for ( int i = 1; i < cornersValue; ++i ) {
            double xp = radius * sin( a );
            double yp = -radius * cos( a );
            a += angle;
            points.setPoint( i, (int) ( xp + xoff ), (int) ( yp + yoff ) );
            m_drawPoints.setPoint( i, (int) ( xp + xoff_draw ), (int) ( yp + yoff_draw ) );
        }
    }
    p.drawPolygon( points );
    p.end();

    drawPoints = m_drawPoints;
}

/*
    need options for connecting start and finish points 
    automatically and for winding mode also
*/
void PolyGonTool::optionsDialog()
{
    ToolOptsStruct ts;    
    
    ts.usePattern       = usePattern;
    ts.useGradient      = useGradient;
    ts.lineThickness    = lineThickness;
    ts.lineOpacity      = lineOpacity;
    ts.fillShapes       = useRegions;
    ts.polygonCorners   = cornersValue;
    ts.polygonSharpness = sharpnessValue;
    ts.convexPolygon    = checkPolygon;
    ts.concavePolygon   = checkConcavePolygon;

    bool old_usePattern       = usePattern;
    bool old_useGradient      = useGradient;
    int  old_lineThickness    = lineThickness;
    int  old_lineOpacity      = lineOpacity;
    bool old_useRegions       = useRegions;
    int old_cornersValue      = cornersValue;
    int old_sharpnessValue    = sharpnessValue;
    bool old_checkPolygon      = checkPolygon;
    bool old_checkConcavePolygon = checkConcavePolygon;
    
    ToolOptionsDialog *pOptsDialog 
        = new ToolOptionsDialog( tt_polygontool, ts );

    pOptsDialog->exec();
    
    if(!pOptsDialog->result() == QDialog::Accepted)
        return;
    else {
        lineThickness = pOptsDialog->polygonToolTab()->thickness();
        lineOpacity   = pOptsDialog->polygonToolTab()->opacity();
        cornersValue  = pOptsDialog->polygonToolTab()->corners();
        sharpnessValue = pOptsDialog->polygonToolTab()->sharpness();
        usePattern    = pOptsDialog->polygonToolTab()->usePattern();
        useGradient   = pOptsDialog->polygonToolTab()->useGradient();
        useRegions    = pOptsDialog->polygonToolTab()->solid();
        checkPolygon  = pOptsDialog->polygonToolTab()->convexPolygon();
        checkConcavePolygon = pOptsDialog->polygonToolTab()->concavePolygon();

        // User change value ?
        if ( old_usePattern != usePattern || old_useGradient != useGradient 
             || old_lineOpacity != lineOpacity || old_lineThickness != lineThickness
             || old_useRegions != useRegions || old_cornersValue != cornersValue
             || old_sharpnessValue != sharpnessValue || old_checkPolygon != checkPolygon
             || old_checkConcavePolygon != checkConcavePolygon  ) {    
            KisPainter *p = m_pView->kisPainter();
    
            p->setLineThickness( lineThickness );
            p->setLineOpacity( lineOpacity );
            p->setPatternFill( usePattern );
            p->setGradientFill( useGradient );
            p->setFilledPolygon( useRegions );

            // set polygon tool settings
            KisDoc::PolygonToolSettings s = m_pDoc->getPolyGonToolSettings();
            s.thickness            = lineThickness;
            s.opacity              = lineOpacity;
            s.useCurrentPattern    = usePattern;
            s.fillWithGradient     = useGradient;
            s.fillInteriorRegions  = useRegions;
            s.corners              = cornersValue;
            s.sharpness            = sharpnessValue;
            s.polygon              = checkPolygon;
            s.concavePolygon       = checkConcavePolygon;

            m_pDoc->setPolygonToolSettings( s );

            m_pDoc->setModified( true );
        }
    }
}

void PolyGonTool::setupAction(QObject *collection)
{
	KToggleAction *toggle = new KToggleAction(i18n("&Polygon tool"), "polygon", 0, this, SLOT(toolSelect()), collection, "tool_polygon");

	toggle -> setExclusiveGroup("tools");
}

void PolyGonTool::toolSelect()
{
	if (m_pView) {
		KisDoc::PolylineToolSettings s = m_pDoc -> getPolyLineToolSettings();
		KisPainter *gc = m_pView -> kisPainter();

		gc -> setLineThickness(s.thickness);
		gc -> setLineOpacity(s.opacity);
		gc -> setPatternFill(s.useCurrentPattern);
		gc -> setGradientFill(s.fillWithGradient);

		m_pView -> activateTool(this);
	}
}



