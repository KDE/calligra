/* This file is part of the KDE project
 * Copyright (C) 2007-2008 Jan Hambrecht <jaham@gmx.net>
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
#include <KoShapeBorderCommand.h>

#include <QBrush>
#include <QGradient>
#include <QUndoCommand>
#include <QPainter>

#include <math.h>

#include <kdebug.h>

int GradientStrategy::m_handleRadius = 3;

GradientStrategy::GradientStrategy( KoShape *shape, Target target )
: m_shape( shape ),m_selectedHandle( -1 ), m_selectedLine(false), m_editing( false )
, m_target( target )
{
    if( m_target == Fill )
    {
        m_matrix = m_shape->background().matrix() * m_shape->absoluteTransformation( 0 );
    }
    else
    {
        KoLineBorder * stroke = dynamic_cast<KoLineBorder*>( m_shape->border() );
        if( stroke )
            m_matrix = stroke->lineBrush().matrix() * m_shape->absoluteTransformation( 0 );
    }
}

void GradientStrategy::setEditing( bool on )
{
    m_editing = on;
    // if we are going into editing mode, save the old background
    // for use inside the command emitted when finished
    if( on )
    {
        if( m_target == Fill )
        {
            m_oldBrush = m_shape->background();
        }
        else
        {
            KoLineBorder * stroke = dynamic_cast<KoLineBorder*>( m_shape->border() );
            if( stroke )
            {
                m_oldStroke = *stroke;
                m_oldBrush = stroke->lineBrush();
            }
        }
    }
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

bool GradientStrategy::selectLine( const QPointF & )
{
    m_selectedLine = false;
    return false;
}

void GradientStrategy::paintHandle( QPainter &painter, const KoViewConverter &converter, const QPointF &position )
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

bool GradientStrategy::mouseAtLineSegment( const QPointF &mousePos, const QPointF &segStart, const QPointF &segStop )
{
    QPointF seg = segStop - segStart;
    double segLength = sqrt( seg.x()*seg.x() + seg.y()*seg.y() );
    // calculate normalized segment vector
    QPointF normSeg = seg / segLength;
    // mouse position relative to segment start point
    QPointF relMousePos = mousePos - segStart;
    // project relative mouse position onto segment
    double scalar = normSeg.x()*relMousePos.x() + normSeg.y()*relMousePos.y();
    // check if projection is between start and stop point
    if( scalar < 0.0 || scalar > segLength )
        return false;
    // calculate vector between relative mouse position and projected mouse position
    QPointF distVec = scalar * normSeg - relMousePos;
    double dist = distVec.x()*distVec.x() + distVec.y()*distVec.y();
    if( dist > handleRadius()*handleRadius() )
        return false;

    return true;
}

void GradientStrategy::handleMouseMove(const QPointF &mouseLocation, Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED( modifiers )

    QMatrix invMatrix = m_matrix.inverted();
    if( m_selectedLine )
    {
        uint handleCount = m_handles.count();
        QPointF delta = invMatrix.map( mouseLocation ) - invMatrix.map( m_lastMousePos );
        for( uint i = 0; i < handleCount; ++i )
            m_handles[i] += delta;
        m_lastMousePos = mouseLocation;
    }
    else
    {
        m_handles[m_selectedHandle] = invMatrix.map( mouseLocation );
    }

    m_newBrush = brush();
    if( m_target == Fill )
    {
        m_shape->setBackground( m_newBrush );
    }
    else
    {
        KoLineBorder * stroke = dynamic_cast<KoLineBorder*>( m_shape->border() );
        if( stroke )
            stroke->setLineBrush( m_newBrush );
    }
}

QUndoCommand * GradientStrategy::createCommand( QUndoCommand * parent )
{
    if( m_target == Fill )
    {
        m_shape->setBackground( m_oldBrush );
        return new KoShapeBackgroundCommand( m_shape, m_newBrush, parent );
    }
    else
    {
        KoLineBorder * stroke = dynamic_cast<KoLineBorder*>( m_shape->border() );
        if( stroke )
        {
            *stroke = m_oldStroke;
            KoLineBorder * newStroke = new KoLineBorder( *stroke );
            newStroke->setLineBrush( m_newBrush );
            return new KoShapeBorderCommand( m_shape, newStroke, parent );
        }
    }

    return 0;
}

QRectF GradientStrategy::boundingRect() const
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
    m_shape->update();
}

const QGradient * GradientStrategy::gradient()
{
    if( m_target == Fill )
    {
        return m_shape->background().gradient();
    }
    else
    {
        KoLineBorder * stroke = dynamic_cast<KoLineBorder*>( m_shape->border() );
        if( ! stroke )
            return 0;
        return stroke->lineBrush().gradient();
    }
}

GradientStrategy::Target GradientStrategy::target() const
{
    return m_target;
}

void GradientStrategy::startDrawing( const QPointF &mousePos )
{
    QMatrix invMatrix = m_matrix.inverted();

    int handleCount = m_handles.count();
    for( int handleId = 0; handleId < handleCount; ++handleId )
        m_handles[handleId] = invMatrix.map( mousePos );

    m_selectedHandle = handleCount-1;
    setEditing( true );
}

/////////////////////////////////////////////////////////////////
// strategy implementations
/////////////////////////////////////////////////////////////////

LinearGradientStrategy::LinearGradientStrategy( KoShape *shape, const QLinearGradient *gradient, Target target )
: GradientStrategy( shape, target )
{
    m_handles.append( gradient->start() );
    m_handles.append( gradient->finalStop() );
}

void LinearGradientStrategy::paint( QPainter &painter, const KoViewConverter &converter )
{
    QPointF startPoint = m_matrix.map( m_handles[start] );
    QPointF stopPoint = m_matrix.map( m_handles[stop] );

    m_shape->applyConversion( painter, converter );
    painter.drawLine( startPoint, stopPoint );
    paintHandle( painter, converter, startPoint );
    paintHandle( painter, converter, stopPoint );
}

bool LinearGradientStrategy::selectLine( const QPointF &mousePos )
{
    m_selectedLine = mouseAtLineSegment( mousePos, m_matrix.map( m_handles[start] ), m_matrix.map( m_handles[stop] ) );
    if( m_selectedLine )
        m_lastMousePos = mousePos;

    return m_selectedLine;
}

QBrush LinearGradientStrategy::brush()
{
    QLinearGradient gradient( m_handles[start], m_handles[stop] );
    gradient.setStops( m_oldBrush.gradient()->stops() );
    gradient.setSpread( m_oldBrush.gradient()->spread() );
    QBrush brush = QBrush( gradient );
    brush.setMatrix( m_oldBrush.matrix() );
    return brush;
}

RadialGradientStrategy::RadialGradientStrategy( KoShape *shape, const QRadialGradient *gradient, Target target )
: GradientStrategy( shape, target )
{
    m_handles.append( gradient->center() );
    m_handles.append( gradient->focalPoint() );
    m_handles.append( gradient->center() + QPointF( gradient->radius(), 0 ) );
}

void RadialGradientStrategy::paint( QPainter &painter, const KoViewConverter &converter )
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

bool RadialGradientStrategy::selectLine( const QPointF &mousePos )
{
    m_selectedLine = mouseAtLineSegment( mousePos, m_matrix.map( m_handles[center] ), m_matrix.map( m_handles[radius] ) );
    if( m_selectedLine )
        m_lastMousePos = mousePos;

    return m_selectedLine;
}

QBrush RadialGradientStrategy::brush()
{
    QPointF d = m_handles[radius]-m_handles[center];
    double r = sqrt( d.x()*d.x() + d.y()*d.y() );
    QRadialGradient gradient( m_handles[center], r, m_handles[focal] );
    gradient.setStops( m_oldBrush.gradient()->stops() );
    gradient.setSpread( m_oldBrush.gradient()->spread() );
    QBrush brush = QBrush( gradient );
    brush.setMatrix( m_oldBrush.matrix() );
    return brush;
}

ConicalGradientStrategy::ConicalGradientStrategy( KoShape *shape, const QConicalGradient *gradient, Target target )
: GradientStrategy( shape, target )
{
    double angle = gradient->angle() * M_PI / 180.0;
    double scale = 0.25 * ( shape->size().height() + shape->size().width() );
    m_handles.append( gradient->center() );
    m_handles.append( gradient->center() + scale * QPointF( cos( angle ), -sin( angle ) ) );
}

void ConicalGradientStrategy::paint( QPainter &painter, const KoViewConverter &converter )
{
    QPointF centerPoint = m_matrix.map( m_handles[center] );
    QPointF directionPoint = m_matrix.map( m_handles[direction] );

    m_shape->applyConversion( painter, converter );
    painter.drawLine( centerPoint, directionPoint );
    paintHandle( painter, converter, centerPoint );
    paintHandle( painter, converter, directionPoint );
}

bool ConicalGradientStrategy::selectLine( const QPointF &mousePos )
{
    m_selectedLine = mouseAtLineSegment( mousePos, m_matrix.map( m_handles[center] ), m_matrix.map( m_handles[direction] ) );
    if( m_selectedLine )
        m_lastMousePos = mousePos;

    return m_selectedLine;
}

QBrush ConicalGradientStrategy::brush()
{
    QPointF d = m_handles[direction]-m_handles[center];
    double angle = atan2( -d.y(), d.x() ) / M_PI * 180.0;
    if( angle < 0.0 )
        angle += 360;
    QConicalGradient gradient( m_handles[center], angle );
    gradient.setStops( m_oldBrush.gradient()->stops() );
    gradient.setSpread( m_oldBrush.gradient()->spread() );
    QBrush brush = QBrush( gradient );
    brush.setMatrix( m_oldBrush.matrix() );
    return brush;
}
