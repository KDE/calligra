/* This file is part of the KDE project
 * Copyright (C) 2006 Jan Hambrecht <jaham@gmx.net>
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

#include "KarbonGradientTool.h"

#include <KoShape.h>
#include <KoCanvasBase.h>
#include <KoShapeManager.h>
#include <KoSelection.h>
#include <KoPointerEvent.h>
#include <KoShapeBackgroundCommand.h>

#include <QBrush>
#include <QGradient>
#include <QLinearGradient>
#include <QUndoCommand>

#include <math.h>

KarbonGradientTool::KarbonGradientTool(KoCanvasBase *canvas)
: KoTool( canvas )
, m_currentStrategy( 0 )
{
}

KarbonGradientTool::~KarbonGradientTool()
{
}

void KarbonGradientTool::paint( QPainter &painter, KoViewConverter &converter )
{
    painter.setBrush( Qt::green ); //TODO make configurable
    painter.setPen( Qt::blue ); //TODO make configurable

    // paint all the strategies
    foreach( GradientStrategy *strategy, m_gradients )
    {
        painter.save();
        strategy->paint( painter, converter );
        painter.restore();
    }

    // paint selected strategy with another color
    if( m_currentStrategy )
    {
        painter.setBrush( Qt::red ); //TODO make configurable
        m_currentStrategy->paint( painter, converter );
    }
}

void KarbonGradientTool::repaintDecorations()
{
    foreach( GradientStrategy *strategy, m_gradients )
        m_canvas->updateCanvas( strategy->boundingRect() );
}


void KarbonGradientTool::mousePressEvent( KoPointerEvent *event )
{
    Q_UNUSED( event )
    if( m_currentStrategy )
        m_currentStrategy->setEditing( true );
}

void KarbonGradientTool::mouseMoveEvent( KoPointerEvent *event )
{
    if( m_currentStrategy )
    {
        m_currentStrategy->repaint();
        if( m_currentStrategy->isEditing() )
        {
            m_currentStrategy->handleMouseMove( event->point, event->modifiers() );
            m_currentStrategy->repaint();
            return;
        }
    }
    foreach( GradientStrategy *strategy, m_gradients )
    {
        if( strategy->selectHandle( event->point ) )
        {
            m_currentStrategy = strategy;
            m_currentStrategy->repaint();
            useCursor(Qt::SizeAllCursor);
            return;
        }
    }
    m_currentStrategy = 0;
    useCursor(Qt::ArrowCursor);
}

void KarbonGradientTool::mouseReleaseEvent( KoPointerEvent *event )
{
    Q_UNUSED( event )
    // if we are editing, get out of edit mode and add a command to the stack
    if( m_currentStrategy )
    {
        m_currentStrategy->setEditing( false );
        m_canvas->addCommand( m_currentStrategy->createCommand() );
    }
}

void KarbonGradientTool::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
        case Qt::Key_I:
        {
            uint handleRadius = GradientStrategy::handleRadius();
            if(event->modifiers() & Qt::ControlModifier)
                handleRadius--;
            else
                handleRadius++;
            m_canvas->resourceProvider()->setHandleRadius( handleRadius );
        }
        break;
        default:
            event->ignore();
            return;
    }
    event->accept();
}

void KarbonGradientTool::activate( bool temporary )
{
    Q_UNUSED(temporary);
    if( ! m_canvas->shapeManager()->selection()->count() )
    {
        emit sigDone();
        return;
    }

    m_gradients.clear();
    m_currentStrategy = 0;

    foreach( KoShape *shape, m_canvas->shapeManager()->selection()->selectedShapes() )
    {
        const QBrush &background = shape->background();
        if( background.style() == Qt::LinearGradientPattern )
        {
            const QLinearGradient *gradient = static_cast<const QLinearGradient*>( background.gradient() );
            m_gradients.append( new LinearGradientStrategy( shape, gradient ) );
        }
        else if( background.style() == Qt::RadialGradientPattern )
        {
            const QRadialGradient *gradient = static_cast<const QRadialGradient*>( background.gradient() );
            m_gradients.append( new RadialGradientStrategy( shape, gradient ) );
        }
        else if( background.style() == Qt::ConicalGradientPattern )
        {
            const QConicalGradient *gradient = static_cast<const QConicalGradient*>( background.gradient() );
            m_gradients.append( new ConicalGradientStrategy( shape, gradient ) );
        }
        else
            continue;
        m_gradients.last()->repaint();
    }

    if( m_gradients.count() == 0 )
    {
        emit sigDone();
        return;
    }
    m_gradients.first()->setHandleRadius( m_canvas->resourceProvider()->handleRadius() );
    useCursor(Qt::ArrowCursor, true);
}

void KarbonGradientTool::deactivate()
{
    foreach( GradientStrategy* strategy, m_gradients )
    {
        strategy->repaint();
        delete strategy;
    }
    m_gradients.clear();
    foreach( KoShape *shape, m_canvas->shapeManager()->selection()->selectedShapes() )
        shape->repaint();
}

void KarbonGradientTool::resourceChanged( KoCanvasResource::EnumCanvasResource key, const QVariant & res )
{
    switch( key )
    {
        case KoCanvasResource::HandleRadius:
            foreach( GradientStrategy *strategy, m_gradients )
                strategy->repaint();
            GradientStrategy::setHandleRadius( res.toUInt() );
            foreach( GradientStrategy *strategy, m_gradients )
                strategy->repaint();
        break;
        default:
            return;
    }
}

int KarbonGradientTool::GradientStrategy::m_handleRadius = 3;

KarbonGradientTool::GradientStrategy::GradientStrategy( KoShape *shape )
: m_shape( shape ),m_selectedHandle( -1 ), m_editing( false )
{
}

void KarbonGradientTool::GradientStrategy::setEditing( bool on )
{
    m_editing = on;
    // if we are going into editing mode, save the old background
    // for use inside the command emitted when finished
    if( on )
        m_oldBackground = m_shape->background();
}

bool KarbonGradientTool::GradientStrategy::selectHandle( const QPointF &mousePos )
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

void KarbonGradientTool::GradientStrategy::paintHandle( QPainter &painter, const QPointF &position )
{
    painter.drawRect( QRectF( position.x()-m_handleRadius, position.y()-m_handleRadius, 2*m_handleRadius, 2*m_handleRadius ) );
}

bool KarbonGradientTool::GradientStrategy::mouseInsideHandle( const QPointF &mousePos, const QPointF &handlePos )
{
    QPointF handle = m_shape->transformationMatrix(0).map( handlePos );
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

void KarbonGradientTool::GradientStrategy::handleMouseMove(const QPointF &mouseLocation, Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED( modifiers )

    m_handles[m_selectedHandle] = m_shape->transformationMatrix(0).inverted().map( mouseLocation );

    m_newBackground = background();
    m_shape->setBackground( m_newBackground );
}

QUndoCommand * KarbonGradientTool::GradientStrategy::createCommand()
{
    m_shape->setBackground( m_oldBackground );
    QList<KoShape*> shapes;
    return new KoShapeBackgroundCommand( shapes << m_shape, m_newBackground, 0 );
}

QRectF KarbonGradientTool::GradientStrategy::boundingRect()
{
    QMatrix matrix = m_shape->transformationMatrix( 0 );
    // calculate the bounding rect of the handles
    QRectF bbox( matrix.map( m_handles[0] ), QSize(0,0) );
    for( int i = 1; i < m_handles.count(); ++i )
    {
        QPointF handle = matrix.map( m_handles[i] );
        bbox.setLeft( qMin( handle.x(), bbox.left() ) );
        bbox.setRight( qMax( handle.x(), bbox.right() ) );
        bbox.setTop( qMin( handle.y(), bbox.top() ) );
        bbox.setBottom( qMax( handle.y(), bbox.bottom() ) );
    }
    return bbox.adjusted( -m_handleRadius, -m_handleRadius, m_handleRadius, m_handleRadius );
}

void KarbonGradientTool::GradientStrategy::repaint() const
{
    m_shape->repaint();
}

KarbonGradientTool::LinearGradientStrategy::LinearGradientStrategy( KoShape *shape, const QLinearGradient *gradient )
: GradientStrategy( shape )
{
    m_handles.append( gradient->start() );
    m_handles.append( gradient->finalStop() );
}

void KarbonGradientTool::LinearGradientStrategy::paint( QPainter &painter, KoViewConverter &converter )
{
    QPointF startPoint = converter.documentToView( m_handles[start] );
    QPointF stopPoint = converter.documentToView( m_handles[stop] );

    painter.setWorldMatrix( m_shape->transformationMatrix( &converter ), true );
    painter.drawLine( startPoint, stopPoint );
    paintHandle( painter, startPoint );
    paintHandle( painter, stopPoint );
}

QBrush KarbonGradientTool::LinearGradientStrategy::background()
{
    QLinearGradient gradient( m_handles[start], m_handles[stop] );
    gradient.setStops( m_oldBackground.gradient()->stops() );
    return QBrush( gradient );
}

KarbonGradientTool::RadialGradientStrategy::RadialGradientStrategy( KoShape *shape, const QRadialGradient *gradient )
: GradientStrategy( shape )
{
    m_handles.append( gradient->center() );
    m_handles.append( gradient->focalPoint() );
    m_handles.append( gradient->center() + QPointF( gradient->radius(), 0 ) );
}

void KarbonGradientTool::RadialGradientStrategy::paint( QPainter &painter, KoViewConverter &converter )
{
    QPointF centerPoint = converter.documentToView( m_handles[center] );
    QPointF radiusPoint = converter.documentToView( m_handles[radius] );
    QPointF focalPoint = converter.documentToView( m_handles[focal] );

    painter.setWorldMatrix( m_shape->transformationMatrix( &converter ), true );
    painter.drawLine( centerPoint, radiusPoint );
    paintHandle( painter, centerPoint );
    paintHandle( painter, radiusPoint );
    paintHandle( painter, focalPoint );
}

QBrush KarbonGradientTool::RadialGradientStrategy::background()
{
    QPointF d = m_handles[radius]-m_handles[center];
    double r = sqrt( d.x()*d.x() + d.y()*d.y() );
    QRadialGradient gradient( m_handles[center], r, m_handles[focal] );
    gradient.setStops( m_oldBackground.gradient()->stops() );
    return QBrush( gradient );
}

KarbonGradientTool::ConicalGradientStrategy::ConicalGradientStrategy( KoShape *shape, const QConicalGradient *gradient )
: GradientStrategy( shape )
{
    double angle = gradient->angle() * M_PI / 180.0;
    double scale = 0.25 * ( shape->size().height() + shape->size().width() );
    m_handles.append( gradient->center() );
    m_handles.append( gradient->center() + scale * QPointF( cos( angle ), -sin( angle ) ) );
}

void KarbonGradientTool::ConicalGradientStrategy::paint( QPainter &painter, KoViewConverter &converter )
{
    QPointF centerPoint = converter.documentToView( m_handles[center] );
    QPointF directionPoint = converter.documentToView( m_handles[direction] );

    painter.setWorldMatrix( m_shape->transformationMatrix( &converter ), true );
    painter.drawLine( centerPoint, directionPoint );
    paintHandle( painter, centerPoint );
    paintHandle( painter, directionPoint );
}

QBrush KarbonGradientTool::ConicalGradientStrategy::background()
{
    QPointF d = m_handles[direction]-m_handles[center];
    double angle = atan2( -d.y(), d.x() ) / M_PI * 180.0;
    if( angle < 0.0 )
        angle += 360;
    QConicalGradient gradient( m_handles[center], angle );
    gradient.setStops( m_oldBackground.gradient()->stops() );
    return QBrush( gradient );
}

#include "KarbonGradientTool.moc"
