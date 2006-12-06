/* This file is part of the KDE project
   Copyright (C) 2006 Jan Hambrecht <jaham@gmx.net>

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

#include "star/KoStarShape.h"

#include <QPainter>
#include <math.h>

KoStarShape::KoStarShape()
: m_cornerCount( 5 )
, m_zoomX( 1.0 )
, m_zoomY( 1.0 )
, m_convex( false )
{
    double radianStep = M_PI / static_cast<double>(m_cornerCount);

    m_radius[base] = 25.0;
    m_radius[tip] = 50.0;
    m_angles[base] = M_PI_2-2*radianStep;
    m_angles[tip] = M_PI_2-2*radianStep;
    m_roundness[base] = m_roundness[tip] = 0.0f;

    createPath();

    m_center = computeCenter();
}

KoStarShape::~KoStarShape()
{
}

void KoStarShape::setCornerCount( uint cornerCount )
{
    if( cornerCount >= 3 )
    {
        m_cornerCount = cornerCount;
        createPath();
    }
}

uint KoStarShape::cornerCount()
{
    return m_cornerCount;
}

void KoStarShape::setBaseRadius( double baseRadius )
{
    m_radius[base] = fabs( baseRadius );
    updatePath( QSize() );
}

double KoStarShape::baseRadius()
{
    return m_radius[base];
}

void KoStarShape::setTipRadius( double tipRadius )
{
    m_radius[tip] = fabs( tipRadius );
    updatePath( QSize() );
}

double KoStarShape::tipRadius()
{
    return m_radius[tip];
}

void KoStarShape::setBaseRoundness( double baseRoundness )
{
    m_roundness[base] = baseRoundness;
    updatePath( QSize() );
}

void KoStarShape::setTipRoundness( double tipRoundness )
{
    m_roundness[tip] = tipRoundness;
    updatePath( QSize() );
}

void KoStarShape::setConvex( bool convex )
{
    m_convex = convex;
    createPath();
}

void KoStarShape::moveHandleAction( int handleId, const QPointF & point, Qt::KeyboardModifiers modifiers )
{
    if( modifiers & Qt::ShiftModifier )
    {
        QPointF tangentVector = point - m_handles[handleId];
        double distance = sqrt( tangentVector.x()*tangentVector.x() + tangentVector.y()*tangentVector.y() );
        QPointF radialVector = m_handles[handleId] - m_center;
        // cross product to determine in which direction the user is dragging
        double moveDirection = radialVector.x()*tangentVector.y() - radialVector.y()*tangentVector.x();
        // make the roundness stick to zero if distance is under a certain value
        float snapDistance = 3.0;
        if( distance >= 0.0 )
            distance = distance < snapDistance ? 0.0 : distance-snapDistance;
        else
            distance = distance > -snapDistance ? 0.0 : distance+snapDistance;
        // control changes roundness on both handles, else only the actual handle roundness is changed
        if( modifiers & Qt::ControlModifier )
            m_roundness[handleId] = moveDirection < 0.0f ? distance : -distance;
        else
            m_roundness[base] = m_roundness[tip] = moveDirection < 0.0f ? distance : -distance;
    }
    else
    {
        QPointF distVector = point - m_center;
        // unapply scaling
        distVector.rx() /= m_zoomX;
        distVector.ry() /= m_zoomY;
        m_radius[handleId] = sqrt( distVector.x()*distVector.x() + distVector.y()*distVector.y() );

        double angle = atan2( distVector.y(), distVector.x() );
        if( angle < 0.0 )
            angle += 2.0*M_PI;
        double diffAngle = angle-m_angles[handleId];
        if( handleId == tip )
        {
            double radianStep = M_PI / static_cast<double>(m_cornerCount);
            m_angles[tip] += diffAngle-radianStep;
            m_angles[base] += diffAngle-radianStep;
        }
        else
        {
            // control make the base point move freely
            if( modifiers & Qt::ControlModifier )
                m_angles[base] += diffAngle;
            else
                m_angles[base] = m_angles[tip];
        }
    }
}

void KoStarShape::updatePath( const QSizeF &size )
{
    Q_UNUSED(size);
    double radianStep = M_PI / static_cast<double>(m_cornerCount);

    uint index = 0;
    for( uint i = 0; i < 2*m_cornerCount; ++i )
    {
        uint cornerType = i % 2;
        if( cornerType == base && m_convex )
            continue;
        double radian = static_cast<double>( (i+1)*radianStep ) + m_angles[cornerType];
        QPointF cornerPoint = QPointF( m_zoomX * m_radius[cornerType] * cos( radian ), m_zoomY * m_radius[cornerType] * sin( radian ) );

        m_points[index]->setPoint( m_center + cornerPoint );
        if( m_roundness[cornerType] > 1e-10 || m_roundness[cornerType] < -1e-10 )
        {
            // normalized cross product to compute tangential vector for handle point
            QPointF tangentVector( cornerPoint.y()/m_radius[cornerType], -cornerPoint.x()/m_radius[cornerType] );
            m_points[index]->setControlPoint2( m_points[index]->point() - m_roundness[cornerType] * tangentVector );
            m_points[index]->setControlPoint1( m_points[index]->point() + m_roundness[cornerType] * tangentVector );
        }
        else
        {
            m_points[index]->unsetProperty( KoPathPoint::HasControlPoint1 );
            m_points[index]->unsetProperty( KoPathPoint::HasControlPoint2 );
        }
        index++;
    }

    normalize();
    m_handles[tip] = m_points.at(tip)->point();
    if( ! m_convex )
        m_handles[base] = m_points.at(base)->point();
    m_center = computeCenter();
}

void KoStarShape::createPath()
{
    if( m_subpaths.size() )
    {
        foreach( KoPathPoint *p, *m_subpaths[0] )
            delete p;
        delete m_subpaths[0];
        m_subpaths.clear();
    }

    double radianStep = M_PI / static_cast<double>(m_cornerCount);

    QPointF center = QPointF( m_radius[tip], m_radius[tip] );

    QPointF cornerPoint( m_radius[tip] * cos( m_angles[tip] + radianStep ), m_radius[tip] * sin( m_angles[tip] + radianStep ) );
    moveTo( center + cornerPoint );
    for( uint i = 1; i < 2*m_cornerCount; ++i )
    {
        uint cornerType = i % 2;
        if( cornerType == base && m_convex )
            continue;
        double radian = static_cast<double>( (i+1)*radianStep )  + m_angles[cornerType];
        cornerPoint = QPointF( m_radius[cornerType] * cos( radian ), m_radius[cornerType] * sin( radian ) );
        lineTo( center + cornerPoint );
    }
    close();
    normalize();

    m_points = *m_subpaths[0];
    m_handles.clear();
    m_handles.push_back( m_points.at(tip)->point() );
    if( ! m_convex )
        m_handles.push_back( m_points.at(base)->point() );
}

void KoStarShape::resize( const QSizeF &newSize )
{
    QSizeF oldSize = size();
    // apply the new aspect ratio
    m_zoomX *= newSize.width() / oldSize.width();
    m_zoomY *= newSize.height() / oldSize.height();

    // this transforms the handles
    KoParameterShape::resize( newSize );

    m_center = computeCenter();
}

QPointF KoStarShape::computeCenter() const
{
    QPointF center( 0, 0 );
    for( uint i = 0; i < m_cornerCount; ++i )
    {
        if( m_convex )
            center += m_points[i]->point();
        else
            center += m_points[2*i]->point();
    }
    return center / static_cast<double>( m_cornerCount );
}
