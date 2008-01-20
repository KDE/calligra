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
const double stopDistance = 15.0;


GradientStrategy::GradientStrategy( KoShape *shape, const QGradient * gradient, Target target )
: m_shape( shape ), m_selection( None ), m_selectionIndex(0)
, m_editing( false ), m_target( target ), m_gradientLine( 0, 1 )
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
    m_stops = gradient->stops();
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

bool GradientStrategy::selectHandle( const QPointF &mousePos, const KoViewConverter &converter )
{
    QRectF hr = handleRect( converter );

    int handleIndex = 0;
    foreach( QPointF handle, m_handles )
    {
        hr.moveCenter( m_matrix.map( handle ) );
        if( hr.contains( mousePos ) )
        {
            setSelection( Handle, handleIndex );
            return true;
        }
        handleIndex++;
    }

    setSelection( None );

    return false;
}

bool GradientStrategy::selectLine( const QPointF &mousePos, const KoViewConverter &converter )
{
    QPointF start = m_matrix.map( m_handles[m_gradientLine.first] );
    QPointF stop = m_matrix.map( m_handles[m_gradientLine.second] );
    double maxDistance = handleRect( converter ).size().width();
    if( mouseAtLineSegment( mousePos, start, stop, maxDistance ) )
    {
        m_lastMousePos = mousePos;
        setSelection( Line );
        return true;
    }

    setSelection( None );

    return false;
}

bool GradientStrategy::selectStop( const QPointF &mousePos, const KoViewConverter &converter )
{
    return false;
}

void GradientStrategy::paintHandle( QPainter &painter, const KoViewConverter &converter, const QPointF &position )
{
    QRectF hr = handleRect( converter );
    hr.moveCenter( position );
    painter.drawRect( hr );
}

void GradientStrategy::paintStops( QPainter &painter, const KoViewConverter &converter, const QPointF &start, const QPointF &stop )
{
    painter.save();

    // calculate orthogonal vector to the gradient line
    // using the cross product of the line vector and the z-axis
    QPointF diff = stop-start;
    QPointF ortho( -diff.y(), diff.x() );
    double orthoLength = sqrt( ortho.x()*ortho.x() + ortho.y()*ortho.y() );
    ortho *= stopDistance / orthoLength;

    QRectF hr = handleRect( converter );
    ortho = converter.viewToDocument( ortho );

    foreach( QGradientStop stop, m_stops )
    {
        QPointF p1 = start + stop.first * diff;
        QPointF p2 = p1 + ortho;
        painter.drawLine( p1, p2 );
        hr.moveCenter( p2 );
        painter.setBrush( stop.second );
        painter.drawEllipse( hr );
    }

    painter.restore();
}

void GradientStrategy::paint( QPainter &painter, const KoViewConverter &converter )
{
    m_shape->applyConversion( painter, converter );

    QPointF startPoint = m_matrix.map( m_handles[m_gradientLine.first] );
    QPointF stopPoint = m_matrix.map( m_handles[m_gradientLine.second] );

    // draw the gradient line
    painter.drawLine( startPoint, stopPoint );

    // draw the gradient stops
    paintStops( painter, converter, startPoint, stopPoint );

    // draw the gradient handles
    foreach( QPointF handle, m_handles )
        paintHandle( painter, converter, m_matrix.map( handle ) );
}

bool GradientStrategy::mouseAtLineSegment( const QPointF &mousePos, const QPointF &segStart, const QPointF &segStop, double maxDistance )
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
    if( dist > maxDistance*maxDistance )
        return false;

    return true;
}

void GradientStrategy::handleMouseMove(const QPointF &mouseLocation, Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED( modifiers )

    QMatrix invMatrix = m_matrix.inverted();
    switch( m_selection )
    {
        case Line:
        {
            uint handleCount = m_handles.count();
            QPointF delta = invMatrix.map( mouseLocation ) - invMatrix.map( m_lastMousePos );
            for( uint i = 0; i < handleCount; ++i )
                m_handles[i] += delta;
            m_lastMousePos = mouseLocation;
            break;
        }
        case Handle:
            m_handles[m_selectionIndex] = invMatrix.map( mouseLocation );
            break;
        default:
            return;
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
    // quick hack for gradient stops
    bbox.adjust( -stopDistance, -stopDistance, stopDistance, stopDistance );
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

    setSelection( Handle, handleCount-1 );
    setEditing( true );
}

void GradientStrategy::setGradientLine( int start, int stop )
{
    m_gradientLine = QPair<int,int>( start, stop );
}

QRectF GradientStrategy::handleRect( const KoViewConverter &converter ) const
{
    return converter.viewToDocument( QRectF( 0, 0, 2*m_handleRadius, 2*m_handleRadius ) );
}

void GradientStrategy::setSelection( SelectionType selection, int index )
{
    m_selection = selection;
    m_selectionIndex = index;
}

/////////////////////////////////////////////////////////////////
// strategy implementations
/////////////////////////////////////////////////////////////////

LinearGradientStrategy::LinearGradientStrategy( KoShape *shape, const QLinearGradient *gradient, Target target )
: GradientStrategy( shape, gradient, target )
{
    m_handles.append( gradient->start() );
    m_handles.append( gradient->finalStop() );
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
: GradientStrategy( shape, gradient, target )
{
    m_handles.append( gradient->center() );
    m_handles.append( gradient->focalPoint() );
    m_handles.append( gradient->center() + QPointF( gradient->radius(), 0 ) );
    setGradientLine( 0, 2 );
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
: GradientStrategy( shape, gradient, target )
{
    double angle = gradient->angle() * M_PI / 180.0;
    double scale = 0.25 * ( shape->size().height() + shape->size().width() );
    m_handles.append( gradient->center() );
    m_handles.append( gradient->center() + scale * QPointF( cos( angle ), -sin( angle ) ) );
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
