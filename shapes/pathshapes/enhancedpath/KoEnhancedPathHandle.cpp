/* This file is part of the KDE project
 * Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>
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

#include "KoEnhancedPathHandle.h"
#include "KoEnhancedPathShape.h"
#include "KoEnhancedPathParameter.h"
#include <math.h>

#include <kdebug.h>

KoEnhancedPathHandle::KoEnhancedPathHandle( KoEnhancedPathParameter * x, KoEnhancedPathParameter * y )
: m_positionX( x ), m_positionY( y )
, m_minimumX( 0 ), m_minimumY( 0 )
, m_maximumX( 0 ), m_maximumY( 0 )
, m_polarX( 0 ), m_polarY( 0 )
, m_minRadius( 0 ), m_maxRadius( 0 )
{
}

KoEnhancedPathHandle::~KoEnhancedPathHandle()
{
}

QPointF KoEnhancedPathHandle::position( KoEnhancedPathShape * path )
{
    QPointF position( m_positionX->evaluate( path ), m_positionY->evaluate( path ) );
    if( isPolar() )
    {
        kDebug() << "KoEnhancedPathHandle::position" << endl;
        // convert polar coordinates into cartesian coordinates
        QPointF center( m_polarX->evaluate( path ), m_polarY->evaluate( path ) );
        kDebug() << "polar center = " << center << endl;
        kDebug() << "polar angle = " << position.x() << endl;
        kDebug() << "polar radius = " << position.y() << endl;
        double angleInRadian = position.x()/180.0*M_PI;
        position = center + position.y() * QPointF( cos( angleInRadian ), sin( angleInRadian ) );
        kDebug() << "polar position = " << position << endl;
    }

    return position;
}

void KoEnhancedPathHandle::setPosition( const QPointF &position, KoEnhancedPathShape * path )
{
    QPointF constrainedPosition( position );

    if( isPolar() )
    {
        kDebug() << "KoEnhancedPathHandle::setPosition" << endl;
        kDebug() << "raw position = " << position << endl;
        // convert cartesian coordinates into polar coordinates
        QPointF center( m_polarX->evaluate( path ), m_polarY->evaluate( path ) );
        kDebug() << "polar center = " << center << endl;
        constrainedPosition -= center;
        double radius = sqrt( constrainedPosition.x()*constrainedPosition.x() + constrainedPosition.y()*constrainedPosition.y() );
        double angle = atan2( constrainedPosition.y(), constrainedPosition.x() );
        if( angle < 0.0 )
            angle += 2 * M_PI;
        constrainedPosition.setX( angle * 180.0 / M_PI );
        kDebug() << "polar angle = " << constrainedPosition.x() << endl;
        if( m_minRadius )
            radius = qMax( m_minRadius->evaluate( path ), radius );
        if( m_maxRadius )
            radius = qMin( m_maxRadius->evaluate( path ), radius );
        kDebug() << "polar radius = " << radius << endl;
        constrainedPosition.setY( radius );
        kDebug() << "polar position = " << constrainedPosition << endl;
    }
    else
    {
        // constrain x coordinate
        if( m_minimumX )
            constrainedPosition.setX( qMax( m_minimumX->evaluate( path ), constrainedPosition.x() ) );
        if( m_maximumX )
            constrainedPosition.setX( qMin( m_maximumX->evaluate( path ), constrainedPosition.x() ) );

        // constrain y coordinate
        if( m_minimumY )
            constrainedPosition.setY( qMax( m_minimumY->evaluate( path ), constrainedPosition.y() ) );
        if( m_maximumY )
            constrainedPosition.setY( qMin( m_maximumY->evaluate( path ), constrainedPosition.y() ) );
    }

    m_positionX->modify( constrainedPosition.x(), path );
    m_positionY->modify( constrainedPosition.y(), path );
}

void KoEnhancedPathHandle::setRangeX( KoEnhancedPathParameter *minX, KoEnhancedPathParameter *maxX )
{
    m_minimumX = minX;
    m_maximumX = maxX;
}

void KoEnhancedPathHandle::setRangeY( KoEnhancedPathParameter *minY, KoEnhancedPathParameter *maxY )
{
    m_minimumY = minY;
    m_maximumY = maxY;
}

void KoEnhancedPathHandle::setPolarCenter( KoEnhancedPathParameter *polarX, KoEnhancedPathParameter *polarY )
{
    m_polarX = polarX;
    m_polarY = polarY;
}

void KoEnhancedPathHandle::setRadiusRange( KoEnhancedPathParameter *minRadius, KoEnhancedPathParameter *maxRadius )
{
    m_minRadius = minRadius;
    m_maxRadius = maxRadius;
}

bool KoEnhancedPathHandle::isPolar() const
{
    return m_polarX && m_polarY;
}
