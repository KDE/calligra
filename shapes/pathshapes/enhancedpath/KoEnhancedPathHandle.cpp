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

KoEnhancedPathHandle::KoEnhancedPathHandle( KoEnhancedPathParameter * x, KoEnhancedPathParameter * y )
: m_positionX( x ), m_positionY( y )
, m_minimumX( 0 ), m_minimumY( 0 )
, m_maximumX( 0 ), m_maximumY( 0 )
{
}

KoEnhancedPathHandle::~KoEnhancedPathHandle()
{
}

QPointF KoEnhancedPathHandle::position( KoEnhancedPathShape * path )
{
    return QPointF( m_positionX->evaluate( path ), m_positionY->evaluate( path ) );
}

void KoEnhancedPathHandle::setPosition( const QPointF &position, KoEnhancedPathShape * path )
{
    QPointF constrainedPosition( position );

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
