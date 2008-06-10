/* This file is part of the KDE project
   Copyright (C) 2008 Fela Winkelmolen <fela.kde@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KarbonCalligraphicShape.h"

#include <KoPathPoint.h>
#include <KarbonCurveFit.h>

#include <KDebug>

#include <cmath>

KarbonCalligraphicShape::KarbonCalligraphicShape()
{
}

KarbonCalligraphicShape::~KarbonCalligraphicShape()
{
}

void KarbonCalligraphicShape::appendPoint( const QPointF &point,
                                           double angle,
                                           double width )
{
    // convert the point from canvas to shape coordinates
    QPointF p = point - position();
    KarbonCalligraphicPoint *calligraphicPoint =
            new KarbonCalligraphicPoint( p, angle, width );

    m_handles.append( p );
    m_points.append(calligraphicPoint);
    appendPointToPath(*calligraphicPoint);
}

void KarbonCalligraphicShape::
        appendPointToPath( const KarbonCalligraphicPoint &p )
{
    double dx = std::cos( p.angle() ) * p.width();
    double dy = std::sin( p.angle() ) * p.width();

    // find the outline points
    QPointF p1 = p.point() - QPointF( dx/2, dy/2 );
    QPointF p2 = p.point() + QPointF( dx/2, dy/2 );

    if ( pointCount() == 0 ) 
    {
        moveTo( p1 );
        lineTo( p2 );
        normalize();
        m_flipped = false;
        return;
    }
    // pointCount > 0

    int flip = 0;
    if ( pointCount() > 2 ) 
        flip = flipDetected( p1, p2 );

    if ( flip )
    {
        // handle flip
        m_flipped = ! m_flipped;

        int index = pointCount() / 2;

        // find the previous two points
        QPointF last1 = pointByIndex( KoPathPointIndex(0, index-1) )->point();
        QPointF last2 = pointByIndex( KoPathPointIndex(0, index) )->point();

        // and add them in reverse order
        appendPointsToPathAux( last2, last1 );
    }

    if ( m_flipped )
    {
        appendPointsToPathAux( p2, p1 );
    }
    else
    {
        appendPointsToPathAux( p1, p2 );
    }

    normalize();
}

void KarbonCalligraphicShape::appendPointsToPathAux( const QPointF &p1,
                                                     const QPointF &p2 )
{
    KoPathPoint *pathPoint1 = new KoPathPoint(this, p1);
    KoPathPoint *pathPoint2 = new KoPathPoint(this, p2);

    // calculate the index of the insertion position
    int index = pointCount() / 2;

    insertPoint( pathPoint2, KoPathPointIndex(0, index) );
    insertPoint( pathPoint1, KoPathPointIndex(0, index) );
}

KoPathShape *KarbonCalligraphicShape::simplified( float error )
{
    QList<QPointF> points;

    const int pc = pointCount();
    for (int i = 0; i < pc; ++i)
    {
        points << pointByIndex( KoPathPointIndex(0, i) )->point();
    }
    
    KoPathShape *res = bezierFit( points, error );

    res->setShapeId( KoPathShapeId );
    res->setFillRule( Qt::WindingFill );
    res->setBackground( Qt::black );
    res->setBorder( 0 );
    res->setPosition( position() );

    return res;
}

const QRectF KarbonCalligraphicShape::lastPieceBoundingRect()
{
    if ( pointCount() < 4 )
        return QRectF();

    int index = pointCount() / 2;

    QPointF last1 = pointByIndex( KoPathPointIndex(0, index-1) )->point();
    QPointF prev1 = pointByIndex( KoPathPointIndex(0, index-2) )->point();

    QPointF last2 = pointByIndex( KoPathPointIndex(0, index) )->point();
    QPointF prev2 = pointByIndex( KoPathPointIndex(0, index+1) )->point();

    QPainterPath p;
    p.moveTo( prev1 );
    p.lineTo( last1 );
    p.lineTo( last2 );
    p.lineTo( prev2 );

    return p.boundingRect().translated( position() );
}


int KarbonCalligraphicShape::flipDetected( const QPointF &p1, const QPointF &p2 )
{
    int index = pointCount() / 2;

    QPointF last1 = pointByIndex( KoPathPointIndex(0, index-1) )->point();
    QPointF prev1 = pointByIndex( KoPathPointIndex(0, index-2) )->point();

    QPointF last2 = pointByIndex( KoPathPointIndex(0, index) )->point();
    QPointF prev2 = pointByIndex( KoPathPointIndex(0, index+1) )->point();

    //                 prev2  last2           p2
    //    ...o-----o-----o-----o  <-- index   o
    //    ...                  |
    //    ...o-----o-----o-----o              o
    //                 prev1  last1           p1

    // the path has already flipped a even number of times
    // so the indexes of where p1 and p2 will be inserted are swapped
    if ( m_flipped ) 
    {
        qSwap( last1, last2 );
        qSwap( prev1, prev2 );
    }

    // detect possible flips
    // FIXME: think about the degenerate cases (ccw() == 0)
    if ( ccw( prev1, last1, p1 ) == ccw( prev2, prev1, last1 ) &&
         ccw( prev1, last1, p1 ) != ccw( last1, p2, last2 ) )
    {
        return +1;
    }

    // the symmetric of the above
    if ( ccw( prev2, last2, p2 ) == ccw(prev1, prev2, last2) &&
         ccw( prev2, last2, p2 ) != ccw(last2, p1, last1) )
    {
        return -1;
    }
    // no flip was detected
    return 0;
}

int KarbonCalligraphicShape::ccw( const QPointF &p1,
                                 const QPointF &p2,
                                 const QPointF &p3 )
{
    // calculate two times the area of the triangle fomed by the points given
    double area2 = ( p2.x() - p1.x() ) * ( p3.y() - p1.y() ) -
                   ( p2.y() - p1.y() ) * ( p3.x() - p1.x() );
    if ( area2 > 0 ) 
    {
        return +1; // the points are given in counterclockwise order
    }
    else if ( area2 < 0 ) 
    {
        return -1; // the points are given in clockwise order
    }
    else 
    {
        return 0; // the points form a degenerate triangle
    }
}



void KarbonCalligraphicShape::setSize( const QSizeF &newSize )
{
    QSizeF oldSize = size();
    // TODO: check
    KoParameterShape::setSize( newSize );
}

QPointF KarbonCalligraphicShape::normalize()
{
    QPointF offset( KoParameterShape::normalize() );
    QMatrix matrix;
    matrix.translate( -offset.x(), -offset.y() );

    for( int i = 0; i < m_handles.size(); ++i )
    {
        m_points[i]->setPoint( matrix.map( m_points[i]->point() ) );
    }

    return offset;
}

void KarbonCalligraphicShape::moveHandleAction( int handleId,
                                                const QPointF & point,
                                               Qt::KeyboardModifiers modifiers )
{
    Q_UNUSED(modifiers);
    m_points[handleId]->setPoint( point );
}

void KarbonCalligraphicShape::updatePath( const QSizeF &size )
{
    Q_UNUSED(size);

    QPointF pos = position();
    
    // remove all points
    clear();
    setPosition( QPoint(0, 0) );

    foreach(KarbonCalligraphicPoint *p, m_points)
        appendPointToPath(*p);

    simplifyPath();

    for (int i = 0; i < m_points.size(); ++i)
        m_handles[i] = m_points[i]->point();

    setPosition( pos );
}

void KarbonCalligraphicShape::simplifyPath()
{
    QList<QPointF> points;

    for (int i = 0; i < pointCount(); ++i)
    {
        points << pointByIndex( KoPathPointIndex(0, i) )->point();
    }

    // TODO: no magic numbers
    double error = m_points[0]->width() / 40;
    KoPathShape *newPath = bezierFit( points, error );

    QPointF oldPosition = position();
    clear();
    setPosition(QPoint(0, 0));

    // TODO: if subpath allowed the insertion also when empty
    //       the following would work better
    m_subpaths.append(new KoSubpath());
    for (int i = 0; i < newPath->pointCount(); ++i)
    {
        KoPathPointIndex index(0, i);
        KoPathPoint *p = new KoPathPoint( *newPath->pointByIndex(index) );
        p->setParent(this);
        m_subpaths[0]->append(p);
    }

    setPosition(oldPosition);
    normalize();

    delete newPath;
}

QString KarbonCalligraphicShape::pathShapeId() const
{
    return KarbonCalligraphicShapeId;
}
