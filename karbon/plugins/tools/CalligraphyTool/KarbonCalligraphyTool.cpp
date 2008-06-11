/* This file is part of the KDE project
 * Copyright (C) 2008 Fela Winkelmolen <fela.kde@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KarbonCalligraphyTool.h"
#include "KarbonCalligraphicShape.h"

#include <KoPathShape.h>
#include <KoShapeGroup.h>
#include <KoLineBorder.h>
#include <KoPointerEvent.h>
#include <KoPathPoint.h>
#include <KoCanvasBase.h>
#include <KoShapeController.h>
#include <KoShapeManager.h>
#include <KoSelection.h>
#include <KarbonCurveFit.h>

#include <knuminput.h>
#include <klocale.h>
#include <kcombobox.h>

#include <QtGui/QStackedWidget>
#include <QtGui/QGroupBox>
#include <QtGui/QCheckBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QPainter>
#include <QtGui/QLabel>

#include <cmath>

#undef M_PI
const double M_PI = 3.1415927;


KarbonCalligraphyTool::KarbonCalligraphyTool(KoCanvasBase *canvas)
    : KoTool( canvas ), m_shape( 0 ), m_strokeWidth( 50 ), m_angle( M_PI/6.0 ),
      m_mass( 20.0 ), m_isDrawing( false )
{
}

KarbonCalligraphyTool::~KarbonCalligraphyTool()
{
}

void KarbonCalligraphyTool::paint( QPainter &painter,
                                   const KoViewConverter &converter )
{
    if ( ! m_shape )
        return;

    painter.save();

    painter.setMatrix( m_shape->absoluteTransformation(&converter) *
                       painter.matrix() );
    m_shape->paint( painter, converter );

    painter.restore();
}

void KarbonCalligraphyTool::mousePressEvent( KoPointerEvent *event )
{
    if ( m_isDrawing ) 
        return;

    m_lastPoint = event->point;
    m_speed = QPointF(0, 0);

    m_isDrawing = true;
    m_shape = new KarbonCalligraphicShape;
    m_shape->setShapeId( KoPathShapeId );
    m_shape->setFillRule( Qt::WindingFill );
    m_shape->setBackground( Qt::black );
    m_shape->setBorder( 0 );
    kDebug() << "=========";
    //addPoint( event );
}

void KarbonCalligraphyTool::mouseMoveEvent( KoPointerEvent *event )
{
    if ( ! m_isDrawing )
        return;

    addPoint( event );
}

void KarbonCalligraphyTool::mouseReleaseEvent( KoPointerEvent *event )
{ 
    if ( ! m_isDrawing )
        return;

    //addPoint( event );
    m_isDrawing = false;

    //m_shape->simplifyPath();
    KoPathShape *finalPath = m_shape;

    QUndoCommand * cmd = m_canvas->shapeController()->addShape( finalPath );
    if( cmd )
    {
        KoSelection *selection = m_canvas->shapeManager()->selection();
        selection->deselectAll();
        selection->select( finalPath );
        m_canvas->addCommand( cmd );
    }
    else
    {
        // TODO: when may this happen????
        delete finalPath;
    }
    
    m_canvas->updateCanvas( m_shape->boundingRect() );
    m_canvas->updateCanvas( finalPath->boundingRect() );

    m_shape = 0;
}

void KarbonCalligraphyTool::addPoint( KoPointerEvent *event )
{
    QPointF force = event->point - m_lastPoint;

    QPointF dSpeed = force/m_mass;
    m_speed = m_speed/2.0 + dSpeed;

    m_lastPoint = m_lastPoint + m_speed;

    kDebug() << event->pressure();
    double strokeWidth = m_strokeWidth * event->pressure();
    m_shape->appendPoint( m_lastPoint, m_angle, strokeWidth );

    m_canvas->updateCanvas( m_shape->lastPieceBoundingRect() );
}



void KarbonCalligraphyTool::activate( bool )
{
    useCursor(Qt::ArrowCursor, true);
}

void KarbonCalligraphyTool::deactivate()
{
}

//#include "KarbonCalligraphyTool.moc"
