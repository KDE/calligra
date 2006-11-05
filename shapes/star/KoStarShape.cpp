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

#include "KoStarShape.h"

#include <QDebug>
#include <QPainter>
#include <math.h>

KoStarShape::KoStarShape()
: m_cornerCount( 5 )
, m_zoomX( 1.0 )
, m_zoomY( 1.0 )
{
    createPath();
    m_points = *m_subpaths[0];
    m_handles.push_back( m_points.at(base)->point() );
    m_handles.push_back( m_points.at(tip)->point() );
    m_center = computeCenter();
}

KoStarShape::~KoStarShape()
{
}

void KoStarShape::setCornerCount( uint cornerCount )
{
    if( cornerCount >= 3 )
        m_cornerCount = cornerCount;
}

void KoStarShape::moveHandleAction( int handleId, const QPointF & point, Qt::KeyboardModifiers modifiers )
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
        m_angles[base] += diffAngle;
    }
}

void KoStarShape::updatePath( const QSizeF &size )
{
    double radianStep = M_PI / static_cast<double>(m_cornerCount);

    for( uint i = 0; i < 2*m_cornerCount; ++i )
    {
        uint cornerType = i % 2;
        double radian = static_cast<double>( i*radianStep ) + m_angles[cornerType];
        QPointF cornerPoint = QPointF( m_zoomX * m_radius[cornerType] * cos( radian ), m_zoomY * m_radius[cornerType] * sin( radian ) );

        m_points[i]->setPoint( m_center + cornerPoint );
    }

    normalize();
    m_handles[base] = m_points.at(base)->point();
    m_handles[tip] = m_points.at(tip)->point();
    m_center = computeCenter();
}

void KoStarShape::createPath()
{
    double radianStep = M_PI / static_cast<double>(m_cornerCount);

    m_radius[base] = 25.0;
    m_radius[tip] = 50.0;
    m_angles[base] = M_PI_2;
    m_angles[tip] = M_PI_2;

    QPointF center = QPointF( m_radius[tip], m_radius[tip] );

    moveTo( center + QPointF( 0, m_radius[base] ) );
    for( uint i = 1; i < 2*m_cornerCount; ++i )
    {
        uint cornerType = i % 2;
        double radian = static_cast<double>( i*radianStep )  + m_angles[cornerType];
        QPointF cornerPoint( m_radius[cornerType] * cos( radian ), m_radius[cornerType] * sin( radian ) );
        lineTo( center + cornerPoint );
    }
    close();
    normalize();
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

QPointF KoStarShape::computeCenter()
{
    QPointF center( 0, 0 );
    for( uint i = 0; i < m_cornerCount; ++i )
        center += m_points[2*i]->point();
    return center / static_cast<double>( m_cornerCount );
}
