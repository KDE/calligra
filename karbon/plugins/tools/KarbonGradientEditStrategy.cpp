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

#include "KarbonGradientEditStrategy.h"

#include <KoShape.h>
#include <KoViewConverter.h>
#include <KoShapeBackgroundCommand.h>

#include <QBrush>
#include <QGradient>
#include <QUndoCommand>
#include <QPainter>

#include <math.h>

int GradientStrategy::m_handleRadius = 3;

GradientStrategy::GradientStrategy( KoShape *shape )
: m_shape( shape ),m_selectedHandle( -1 ), m_editing( false )
{
    m_matrix = m_shape->background().matrix() * m_shape->transformationMatrix( 0 );
}

void GradientStrategy::setEditing( bool on )
{
    m_editing = on;
    // if we are going into editing mode, save the old background
    // for use inside the command emitted when finished
    if( on )
        m_oldBackground = m_shape->background();
}

bool GradientStrategy::selectHandle( const QPointF &mousePos )
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

void GradientStrategy::paintHandle( QPainter &painter, KoViewConverter &converter, const QPointF &position )
{
    QRectF handleRect = converter.viewToDocument( QRectF( m_handleRadius, m_handleRadius, 2*m_handleRadius, 2*m_handleRadius ) );
    handleRect.moveCenter( position );
    painter.drawRect( handleRect );
}

bool GradientStrategy::mouseInsideHandle( const QPointF &mousePos, const QPointF &handlePos )
{
    QPointF handle = m_matrix.map( handlePos );
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

void GradientStrategy::handleMouseMove(const QPointF &mouseLocation, Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED( modifiers )

    m_handles[m_selectedHandle] = m_matrix.inverted().map( mouseLocation );

    m_newBackground = background();
    m_shape->setBackground( m_newBackground );
}

QUndoCommand * GradientStrategy::createCommand()
{
    m_shape->setBackground( m_oldBackground );
    QList<KoShape*> shapes;
    return new KoShapeBackgroundCommand( shapes << m_shape, m_newBackground, 0 );
}

QRectF GradientStrategy::boundingRect()
{
    // calculate the bounding rect of the handles
    QRectF bbox( m_matrix.map( m_handles[0] ), QSize(0,0) );
    for( int i = 1; i < m_handles.count(); ++i )
    {
        QPointF handle = m_matrix.map( m_handles[i] );
        bbox.setLeft( qMin( handle.x(), bbox.left() ) );
        bbox.setRight( qMax( handle.x(), bbox.right() ) );
        bbox.setTop( qMin( handle.y(), bbox.top() ) );
        bbox.setBottom( qMax( handle.y(), bbox.bottom() ) );
    }
    return bbox.adjusted( -m_handleRadius, -m_handleRadius, m_handleRadius, m_handleRadius );
}

void GradientStrategy::repaint() const
{
    m_shape->repaint();
}

LinearGradientStrategy::LinearGradientStrategy( KoShape *shape, const QLinearGradient *gradient )
: GradientStrategy( shape )
{
    m_handles.append( gradient->start() );
    m_handles.append( gradient->finalStop() );
}

void LinearGradientStrategy::paint( QPainter &painter, KoViewConverter &converter )
{
    QPointF startPoint = m_matrix.map( m_handles[start] );
    QPointF stopPoint = m_matrix.map( m_handles[stop] );

    m_shape->applyConversion( painter, converter );
    painter.drawLine( startPoint, stopPoint );
    paintHandle( painter, converter, startPoint );
    paintHandle( painter, converter, stopPoint );
}

QBrush LinearGradientStrategy::background()
{
    QLinearGradient gradient( m_handles[start], m_handles[stop] );
    gradient.setStops( m_oldBackground.gradient()->stops() );
    QBrush background = QBrush( gradient );
    background.setMatrix( m_oldBackground.matrix() );
    return background;
}

RadialGradientStrategy::RadialGradientStrategy( KoShape *shape, const QRadialGradient *gradient )
: GradientStrategy( shape )
{
    m_handles.append( gradient->center() );
    m_handles.append( gradient->focalPoint() );
    m_handles.append( gradient->center() + QPointF( gradient->radius(), 0 ) );
}

void RadialGradientStrategy::paint( QPainter &painter, KoViewConverter &converter )
{
    QPointF centerPoint = m_matrix.map( m_handles[center] );
    QPointF radiusPoint = m_matrix.map( m_handles[radius] );
    QPointF focalPoint = m_matrix.map( m_handles[focal] );

    m_shape->applyConversion( painter, converter );
    painter.drawLine( centerPoint, radiusPoint );
    paintHandle( painter, converter, centerPoint );
    paintHandle( painter, converter, radiusPoint );
    paintHandle( painter, converter, focalPoint );
}

QBrush RadialGradientStrategy::background()
{
    QPointF d = m_handles[radius]-m_handles[center];
    double r = sqrt( d.x()*d.x() + d.y()*d.y() );
    QRadialGradient gradient( m_handles[center], r, m_handles[focal] );
    gradient.setStops( m_oldBackground.gradient()->stops() );
    QBrush background = QBrush( gradient );
    background.setMatrix( m_oldBackground.matrix() );
    return background;
}

ConicalGradientStrategy::ConicalGradientStrategy( KoShape *shape, const QConicalGradient *gradient )
: GradientStrategy( shape )
{
    double angle = gradient->angle() * M_PI / 180.0;
    double scale = 0.25 * ( shape->size().height() + shape->size().width() );
    m_handles.append( gradient->center() );
    m_handles.append( gradient->center() + scale * QPointF( cos( angle ), -sin( angle ) ) );
}

void ConicalGradientStrategy::paint( QPainter &painter, KoViewConverter &converter )
{
    QPointF centerPoint = m_matrix.map( m_handles[center] );
    QPointF directionPoint = m_matrix.map( m_handles[direction] );

    m_shape->applyConversion( painter, converter );
    painter.drawLine( centerPoint, directionPoint );
    paintHandle( painter, converter, centerPoint );
    paintHandle( painter, converter, directionPoint );
}

QBrush ConicalGradientStrategy::background()
{
    QPointF d = m_handles[direction]-m_handles[center];
    double angle = atan2( -d.y(), d.x() ) / M_PI * 180.0;
    if( angle < 0.0 )
        angle += 360;
    QConicalGradient gradient( m_handles[center], angle );
    gradient.setStops( m_oldBackground.gradient()->stops() );
    QBrush background = QBrush( gradient );
    background.setMatrix( m_oldBackground.matrix() );
    return background;
}
