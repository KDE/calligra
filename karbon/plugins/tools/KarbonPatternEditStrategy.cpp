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

#include "KarbonPatternEditStrategy.h"

#include <KoShape.h>
#include <KoViewConverter.h>
#include <KoShapeBackgroundCommand.h>

#include <QPainter>
#include <QUndoCommand>

#include <math.h>

int KarbonPatternEditStrategy::m_handleRadius = 3;

KarbonPatternEditStrategy::KarbonPatternEditStrategy( KoShape * shape )
: m_shape( shape ),m_selectedHandle( -1 ), m_editing( false )
{
    // cache the shapes transformation matrix
    m_matrix = m_shape->transformationMatrix( 0 );
    QSizeF size = m_shape->size();
    // the fixed length of half the average shape dimension
    m_normalizedLength = 0.25 * ( size.width() + size.height() );
    // get the brush tranformation matrix
    QMatrix brushMatrix = m_shape->background().matrix();

    // the center handle at the center point of the shape
    //m_origin = QPointF( 0.5 * size.width(), 0.5 * size.height() );
    m_handles.append( brushMatrix.map( QPointF() ) );
    // the direction handle with the length of half the average shape dimension
    QPointF dirVec = QPointF( m_normalizedLength, 0.0 );
    m_handles.append( brushMatrix.map( dirVec ) );
}

KarbonPatternEditStrategy::~KarbonPatternEditStrategy()
{
}

void KarbonPatternEditStrategy::paint( QPainter &painter, KoViewConverter &converter )
{
    QPointF centerPoint = m_matrix.map( m_origin + m_handles[center] );
    QPointF directionPoint = m_matrix.map( m_origin + m_handles[direction] );

    m_shape->applyConversion( painter, converter );
    painter.drawLine( centerPoint, directionPoint );
    paintHandle( painter, converter, centerPoint );
    paintHandle( painter, converter, directionPoint );
}

void KarbonPatternEditStrategy::paintHandle( QPainter &painter, KoViewConverter &converter, const QPointF &position )
{
    QRectF handleRect = converter.viewToDocument( QRectF( m_handleRadius, m_handleRadius, 2*m_handleRadius, 2*m_handleRadius ) );
    handleRect.moveCenter( position );
    painter.drawRect( handleRect );
}

bool KarbonPatternEditStrategy::mouseInsideHandle( const QPointF &mousePos, const QPointF &handlePos )
{
    QPointF handle = m_matrix.map( m_origin + handlePos );
    if( mousePos.x() < handle.x()-m_handleRadius )
        return false;
    if( mousePos.x() > handle.x()+m_handleRadius )
        return false;
    if( mousePos.y() < handle.y()-m_handleRadius )
        return false;
    if( mousePos.y() > handle.y()+m_handleRadius )
        return false;
    return true;
}

bool KarbonPatternEditStrategy::selectHandle( const QPointF &mousePos )
{
    int handleIndex = 0;
    foreach( QPointF handle, m_handles )
    {
        if( mouseInsideHandle( mousePos, handle ) )
        {
            m_selectedHandle = handleIndex;
            return true;
        }
        handleIndex++;
    }
    m_selectedHandle = -1;
    return false;
}

void KarbonPatternEditStrategy::handleMouseMove(const QPointF &mouseLocation, Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED( modifiers )

    if( m_selectedHandle == direction )
    {
        QPointF newPos = m_matrix.inverted().map( mouseLocation ) - m_origin - m_handles[center];
        // calculate the temporary length after handle movement
        double newLength = sqrt( newPos.x()*newPos.x() + newPos.y()*newPos.y() );
        // set the new direction vector with the new direction and normalized length
        m_handles[m_selectedHandle] = m_handles[center] + m_normalizedLength / newLength * newPos;
    }
    else if( m_selectedHandle == center )
    {
        QPointF diffPos = m_matrix.inverted().map( mouseLocation ) - m_origin - m_handles[center];
        m_handles[center] += diffPos;
        m_handles[direction] += diffPos;
    }

    m_newBackground = background();
    m_shape->setBackground( m_newBackground );
}

void KarbonPatternEditStrategy::setEditing( bool on )
{
    m_editing = on;
    // if we are going into editing mode, save the old background
    // for use inside the command emitted when finished
    if( on )
        m_oldBackground = m_shape->background();
}

QUndoCommand * KarbonPatternEditStrategy::createCommand()
{
    m_shape->setBackground( m_oldBackground );
    QList<KoShape*> shapes;
    return new KoShapeBackgroundCommand( shapes << m_shape, m_newBackground, 0 );
}

void KarbonPatternEditStrategy::repaint() const
{
    m_shape->repaint();
}

QRectF KarbonPatternEditStrategy::boundingRect()
{
    // calculate the bounding rect of the handles
    QRectF bbox( m_matrix.map( m_origin + m_handles[0] ), QSize(0,0) );
    for( int i = 1; i < m_handles.count(); ++i )
    {
        QPointF handle = m_matrix.map( m_origin + m_handles[i] );
        bbox.setLeft( qMin( handle.x(), bbox.left() ) );
        bbox.setRight( qMax( handle.x(), bbox.right() ) );
        bbox.setTop( qMin( handle.y(), bbox.top() ) );
        bbox.setBottom( qMax( handle.y(), bbox.bottom() ) );
    }
    return bbox.adjusted( -m_handleRadius, -m_handleRadius, m_handleRadius, m_handleRadius );
}

QBrush KarbonPatternEditStrategy::background()
{
    // the direction vector controls the rotation of the pattern
    QPointF dirVec = m_handles[direction]-m_handles[center];
    double angle = atan2( dirVec.y(), dirVec.x() ) * 180.0 / M_PI;
    QMatrix matrix;
    matrix.translate( m_handles[center].x(), m_handles[center].y() );
    matrix.rotate( angle );

    QBrush newBrush( m_oldBackground );
    newBrush.setMatrix( matrix );
    return newBrush;
}
