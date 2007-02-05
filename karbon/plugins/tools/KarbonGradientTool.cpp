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

#include "KarbonGradientTool.h"
#include "KarbonGradientEditStrategy.h"

#include <karbon_factory.h>
#include <karbon_resourceserver.h>
#include <vgradienttabwidget.h>

#include <KoShape.h>
#include <KoCanvasBase.h>
#include <KoShapeManager.h>
#include <KoSelection.h>
#include <KoPointerEvent.h>
#include <KoShapeBackgroundCommand.h>

#include <QGridLayout>

KarbonGradientTool::KarbonGradientTool(KoCanvasBase *canvas)
: KoTool( canvas )
, m_gradient( 0 )
, m_currentStrategy( 0 )
, m_gradientWidget( 0 )
{
}

KarbonGradientTool::~KarbonGradientTool()
{
    delete m_gradient;
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
        if( m_gradientWidget )
            m_gradientWidget->setGradient( m_currentStrategy->gradient() );
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

    initialize();

    if( m_gradients.count() == 0 )
    {
        emit sigDone();
        return;
    }

    useCursor(Qt::ArrowCursor, true);
}

void KarbonGradientTool::initialize()
{
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
        return;

    m_gradients.first()->setHandleRadius( m_canvas->resourceProvider()->handleRadius() );
    delete m_gradient;
    m_gradient = cloneGradient( m_gradients.first()->gradient() );
}

void KarbonGradientTool::deactivate()
{
    delete m_gradient;
    m_gradient = 0;

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

QWidget * KarbonGradientTool::createOptionWidget()
{
    QWidget *optionWidget = new QWidget();
    QGridLayout* layout = new QGridLayout( optionWidget );

    m_gradientWidget = new VGradientTabWidget( m_gradient, KarbonFactory::rServer(), optionWidget );
    layout->addWidget( m_gradientWidget );

    connect( m_gradientWidget, SIGNAL(changed()), this, SLOT(gradientChanged()) );

    return optionWidget;
}

void KarbonGradientTool::gradientChanged()
{
    QList<KoShape*> selectedShapes = m_canvas->shapeManager()->selection()->selectedShapes();
    QBrush newBrush( *m_gradientWidget->gradient() );
    m_canvas->addCommand( new KoShapeBackgroundCommand( selectedShapes, newBrush ) );
    initialize();
}

#include "KarbonGradientTool.moc"
