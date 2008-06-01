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
    : KoTool( canvas ), m_path( 0 ), m_strokeWidth( 50 ), m_angle( M_PI/6.0 ),
      m_mass( 20.0 ), m_isDrawing( false )
{
}

KarbonCalligraphyTool::~KarbonCalligraphyTool()
{
    //while ( ! m_pieces.isEmpty() ) 
    //    delete m_pieces.takeLast();
}

void KarbonCalligraphyTool::paint( QPainter &painter, const KoViewConverter &converter )
{
    /*foreach ( KoPathShape *shape, m_pieces )
    {
        painter.save();

        painter.setMatrix( shape->absoluteTransformation( &converter ) * painter.matrix() );
        shape->paint( painter, converter );

        painter.restore();
    }*/

    if ( ! m_path )
        return;

    painter.save();

    painter.setMatrix( m_path->absoluteTransformation( &converter ) * painter.matrix() );
    m_path->paint( painter, converter );

    painter.restore();
}

void KarbonCalligraphyTool::mousePressEvent( KoPointerEvent *event )
{
    if ( m_isDrawing ) 
        return;

    m_lastPoint = event->point;
    m_speed = QPointF(0, 0);
    addPoint( event->point );
}

void KarbonCalligraphyTool::mouseMoveEvent( KoPointerEvent *event )
{
    if ( ! m_isDrawing )
        return;

    addPoint( event->point );
}

void KarbonCalligraphyTool::mouseReleaseEvent( KoPointerEvent *event )
{ 
    if ( ! m_isDrawing )
        return;

    addPoint( event->point );
    m_isDrawing = false;

    KoPathShape *finalPath = m_path;//->simplified( m_strokeWidth/60.0 );
    
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
    
    m_canvas->updateCanvas( m_path->boundingRect() );
    m_canvas->updateCanvas( finalPath->boundingRect() );

    //delete m_path;
    m_path = 0;

    /*while ( ! m_pieces.isEmpty() )
    {
        KoPathShape *piece =  m_pieces.takeLast();
        QUndoCommand * cmd = m_canvas->shapeController()->addShape( piece );
        if( cmd )
        {
            m_canvas->addCommand( cmd );
        }
        else
        {
            m_canvas->updateCanvas( piece->boundingRect() );
            delete piece;
        }
    }*/
}

void KarbonCalligraphyTool::addPoint(const QPointF &mousePosition)
{
    QPointF force = mousePosition - m_lastPoint;

    QPointF dSpeed = force/m_mass;
    m_speed = m_speed/2.0 + dSpeed;

    m_lastPoint = m_lastPoint + m_speed;

    if ( ! m_isDrawing ) 
    {
        m_isDrawing = true;
        m_path = new KarbonCalligraphicShape;
        m_path->setShapeId( KoPathShapeId );
        m_path->setFillRule( Qt::WindingFill );
        m_path->setBackground( Qt::black );
        m_path->setBorder( 0 );
    }

    m_path->appendPoint(m_lastPoint, m_angle, m_strokeWidth);

    m_canvas->updateCanvas( m_path->lastPieceBoundingRect() );

    /*if (point2 == m_point1 || point3 == m_point0) {
        kDebug() << "equal points...";
        return;
    } else {
        kDebug() << "not equal:";
        kDebug() << "0:" << m_point0;
        kDebug() << "1:" << m_point1;
        kDebug() << "2:" << point2;
        kDebug() << "3:" << point3;
    }

    if ( ! m_isDrawing ) 
    {
        m_isDrawing = true;
    } 
    else
    {
        KoPathShape *path = new KoPathShape;
        path->setShapeId( KoPathShapeId );
        path->setFillRule( Qt::WindingFill );
        path->setBackground( Qt::black );
        path->setBorder( 0 );

        path->moveTo( m_point0 );
        path->lineTo( m_point1 );
        path->lineTo( point2 );
        path->lineTo( point3 );
        path->close();

        const KoPathPointIndex INDEX0(0, 0);
        const KoPathPointIndex INDEX1(0, 1);
        const KoPathPointIndex INDEX2(0, 2);
        const KoPathPointIndex INDEX3(0, 3);

        if ( ! m_pieces.isEmpty() ) {
            KoPathShape *last = m_pieces.last();
            QList<QPointF> points;
            // last->point2
            QPointF lastTopLeft = last->absolutePosition( KoFlake::TopLeftCorner );
            QPointF prev = last->pointByIndex( INDEX1 )->point();
            prev = prev + lastTopLeft;
            kDebug() << "prev:" << prev;
            // last->point2 and path->point2
            QPointF point = path->pointByIndex( INDEX1 )->point();
            kDebug() << "point:" << point;
            // path->point2
            QPointF next = path->pointByIndex( INDEX2 )->point();
            kDebug() << "next:" << next;
            kDebug() << "------";

            QPointF vector = next - prev;
            double dist = ( QLineF( prev, next ) ).length();
            // normalize the vector (make it's size 1)
            vector /= dist;
            // distance of the control points from the point
            double dist1 = ( QLineF( point, prev ) ).length();
            double dist2 = ( QLineF( point, next ) ).length();
            // normalize the values: make the sum equal to dist/2.0
            double multiplier = dist/2.0 / ( dist1 + dist2 );
            dist1 *= multiplier;
            dist2 *= multiplier;
            QPointF vector1 = vector * dist1;
            QPointF vector2 = vector * dist2;
            QPointF controlPoint1 = point - vector1 - lastTopLeft;
            QPointF controlPoint2 = point + vector2;

            // the two points that have the position of centralPoint
            last->pointByIndex( INDEX2 )->setControlPoint1( controlPoint1 );
            path->pointByIndex( INDEX1 )->setControlPoint2( controlPoint2 );


            prev = last->pointByIndex( INDEX0 )->point();
            prev = prev + lastTopLeft;
            kDebug() << "prev:" << prev;
            // last->point2 and path->point2
            point = path->pointByIndex( INDEX0 )->point();
            kDebug() << "point:" << point;
            // path->point2
            next = path->pointByIndex( INDEX3 )->point();
            kDebug() << "next:" << next;
            kDebug() << "------";

            vector = next - prev;
            dist = ( QLineF( prev, next ) ).length();
            // normalize the vector (make it's size 1)
            vector /= dist;
            // distance of the control points from the point
            dist1 = ( QLineF( point, prev ) ).length();
            dist2 = ( QLineF( point, next ) ).length();
            // normalize the values: make the sum equal to dist/2.0
            multiplier = dist/2.0 / ( dist1 + dist2 );
            dist1 *= multiplier;
            dist2 *= multiplier;
            vector1 = vector * dist1;
            vector2 = vector * dist2;
            controlPoint1 = point - vector1 - lastTopLeft;
            controlPoint2 = point + vector2;

            // the two points that have the position of centralPoint
            last->pointByIndex( INDEX3 )->setControlPoint2( controlPoint1  );
            path->pointByIndex( INDEX0 )->setControlPoint1( controlPoint2 );


            last->normalize();
            m_canvas->updateCanvas( last->boundingRect() );
        }
        path->normalize();

        m_pieces.append(path);
        m_canvas->updateCanvas( path->boundingRect() );
    }

    m_point0 = point3;
    m_point1 = point2;*/
}



void KarbonCalligraphyTool::activate( bool )
{
    useCursor(Qt::ArrowCursor, true);
}

void KarbonCalligraphyTool::deactivate()
{
    /*while ( ! m_pieces.isEmpty() )
    {
        m_canvas->updateCanvas(m_pieces.last()->boundingRect());
        delete m_pieces.takeLast();
    }*/
}

//#include "KarbonCalligraphyTool.moc"
