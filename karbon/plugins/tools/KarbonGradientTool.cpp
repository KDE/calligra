/* This file is part of the KDE project
 * Copyright (C) 2007-2008 Jan Hambrecht <jaham@gmx.net>
 * Copyright (C) 2007 Thorsten Zachmann <zachmann@kde.org>
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

#include <vgradienttabwidget.h>
#include <KarbonCursor.h>
#include <KarbonGradientHelper.h>

#include <KoShape.h>
#include <KoCanvasBase.h>
#include <KoCanvasResourceProvider.h>
#include <KoShapeManager.h>
#include <KoSelection.h>
#include <KoPointerEvent.h>
#include <KoShapeBackgroundCommand.h>
#include <KoShapeBorderCommand.h>
#include <KoResourceServerProvider.h>

#include <QGridLayout>
#include <QPainter>

// helper function
GradientStrategy * createStrategy( KoShape * shape, const QGradient * gradient, GradientStrategy::Target target )
{
    if( ! shape || ! gradient )
        return 0;

    if( gradient->type() == QGradient::LinearGradient )
        return new LinearGradientStrategy( shape, static_cast<const QLinearGradient*>( gradient), target );
    else if( gradient->type() == QGradient::RadialGradient )
        return new RadialGradientStrategy( shape, static_cast<const QRadialGradient*>( gradient ), target );
    else if( gradient->type() == QGradient::ConicalGradient )
        return new ConicalGradientStrategy( shape, static_cast<const QConicalGradient*>( gradient ), target );
    else
        return 0;
}

KarbonGradientTool::KarbonGradientTool(KoCanvasBase *canvas)
: KoTool( canvas )
, m_gradient( 0 )
, m_currentStrategy( 0 )
, m_gradientWidget( 0 )
, m_currentCmd( 0 )
{
}

KarbonGradientTool::~KarbonGradientTool()
{
    delete m_gradient;
}

void KarbonGradientTool::paint( QPainter &painter, const KoViewConverter &converter )
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
        m_canvas->updateCanvas( strategy->boundingRect( *m_canvas->viewConverter() ) );
}


void KarbonGradientTool::mousePressEvent( KoPointerEvent *event )
{
    Q_UNUSED( event )
    if( m_currentStrategy )
    {
        m_currentStrategy->setEditing( true );
    }
    else
    {
        QRectF roi( QPointF(), QSizeF( GradientStrategy::handleRadius(), GradientStrategy::handleRadius() ) );
        roi.moveCenter( event->point );
        // check if we on a shape without a gradient yet
        QList<KoShape*> shapes = m_canvas->shapeManager()->shapesAt( roi );
        VGradientTabWidget::VGradientTarget target = m_gradientWidget->target();

        GradientStrategy * newStrategy = 0;

        foreach( KoShape * shape, shapes )
        {
            if( target == VGradientTabWidget::FillGradient )
            {
                // target is fill so check the background style
                Qt::BrushStyle style = shape->background().style();
                if( style < Qt::LinearGradientPattern || style > Qt::RadialGradientPattern )
                {
                    m_currentCmd = new KoShapeBackgroundCommand( shape, QBrush( *m_gradient ) );
                    shape->setBackground( QBrush( *m_gradient ) );
                    newStrategy = createStrategy( shape, m_gradient, GradientStrategy::Fill );
                }
            }
            else
            {
                // target is stroke so check the border style
                KoLineBorder * border = dynamic_cast<KoLineBorder*>( shape->border() );
                if( ! border )
                {
                    border = new KoLineBorder( 1.0 );
                    border->setLineBrush( QBrush( *m_gradient ) );
                    m_currentCmd = new KoShapeBorderCommand( shape, border );
                    shape->setBorder( border );
                    newStrategy = createStrategy( shape, m_gradient, GradientStrategy::Stroke );
                    break;
                }
                else
                {
                    Qt::BrushStyle style = border->lineBrush().style();
                    if( style < Qt::LinearGradientPattern || style > Qt::RadialGradientPattern )
                    {
                        KoLineBorder * newBorder = new KoLineBorder( *border );
                        newBorder->setLineBrush( QBrush( *m_gradient ) );
                        m_currentCmd = new KoShapeBorderCommand( shape, newBorder );
                        border->setLineBrush( QBrush( *m_gradient ) ); 
                        newStrategy = createStrategy( shape, m_gradient, GradientStrategy::Stroke );
                        break;
                    }
                }
            }
        }

        if( newStrategy )
        {
            m_currentStrategy = newStrategy;
            m_gradients.append( m_currentStrategy );
            m_currentStrategy->startDrawing( event->point );
        }
    }
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
    // first check if we hit any handles
    foreach( GradientStrategy *strategy, m_gradients )
    {
        if( strategy->selectHandle( event->point, *m_canvas->viewConverter() ) )
        {
            m_currentStrategy = strategy;
            m_currentStrategy->repaint();
            useCursor( KarbonCursor::needleMoveArrow() );
            return;
        }
    }
    // now check if we hit any gradient stops
    foreach( GradientStrategy *strategy, m_gradients )
    {
        if( strategy->selectStop( event->point, *m_canvas->viewConverter() ) )
        {
            m_currentStrategy = strategy;
            m_currentStrategy->repaint();
            useCursor( KarbonCursor::needleMoveArrow() );
            return;
        }
    }
    // now check if we hit any lines
    foreach( GradientStrategy *strategy, m_gradients )
    {
        if( strategy->selectLine( event->point, *m_canvas->viewConverter() ) )
        {
            m_currentStrategy = strategy;
            m_currentStrategy->repaint();
            useCursor( Qt::SizeAllCursor );
            return;
        }
    }

    m_currentStrategy = 0;
    useCursor( KarbonCursor::needleArrow() );
}

void KarbonGradientTool::mouseReleaseEvent( KoPointerEvent *event )
{
    Q_UNUSED( event )
    // if we are editing, get out of edit mode and add a command to the stack
    if( m_currentStrategy )
    {
        m_currentStrategy->setEditing( false );
        QUndoCommand * cmd = m_currentStrategy->createCommand( m_currentCmd );
        m_canvas->addCommand( m_currentCmd ? m_currentCmd : cmd );
        m_currentCmd = 0;
        if( m_gradientWidget )
        {
            m_gradientWidget->setGradient( m_currentStrategy->gradient() );
            if( m_currentStrategy->target() == GradientStrategy::Fill )
                m_gradientWidget->setTarget( VGradientTabWidget::FillGradient );
            else
                m_gradientWidget->setTarget( VGradientTabWidget::StrokeGradient );
        }
    }
}

void KarbonGradientTool::mouseDoubleClickEvent( KoPointerEvent *event )
{
    if( ! m_currentStrategy )
        return;

    m_canvas->updateCanvas( m_currentStrategy->boundingRect( *m_canvas->viewConverter() ) );

    if( m_currentStrategy->handleDoubleClick( event->point ) )
    {
        QUndoCommand * cmd = m_currentStrategy->createCommand( m_currentCmd );
        m_canvas->addCommand( m_currentCmd ? m_currentCmd : cmd );
        m_currentCmd = 0;
        if( m_gradientWidget )
        {
            m_gradientWidget->setGradient( m_currentStrategy->gradient() );
            if( m_currentStrategy->target() == GradientStrategy::Fill )
                m_gradientWidget->setTarget( VGradientTabWidget::FillGradient );
            else
                m_gradientWidget->setTarget( VGradientTabWidget::StrokeGradient );
        }
        m_canvas->updateCanvas( m_currentStrategy->boundingRect( *m_canvas->viewConverter() ) );
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
        emit done();
        return;
    }

    initialize();
    repaintDecorations();

    useCursor( KarbonCursor::needleArrow(), true);
}

void KarbonGradientTool::initialize()
{
    m_gradients.clear();
    m_currentStrategy = 0;

    foreach( KoShape *shape, m_canvas->shapeManager()->selection()->selectedShapes() )
    {
        GradientStrategy * fillStrategy = createStrategy( shape, shape->background().gradient(), GradientStrategy::Fill );
        if( fillStrategy )
        {
            m_gradients.append( fillStrategy );
            fillStrategy->repaint();
        }

        KoLineBorder * stroke = dynamic_cast<KoLineBorder*>( shape->border() );
        if( ! stroke )
            continue;

        GradientStrategy * strokeStrategy = createStrategy( shape, stroke->lineBrush().gradient(), GradientStrategy::Stroke );
        if( strokeStrategy )
        {
            m_gradients.append( strokeStrategy );
            strokeStrategy->repaint();
        }
    }

    if( m_gradients.count() == 0 )
    {
        // create a default gradient
        m_gradient = new QLinearGradient( QPointF(0,0), QPointF(100,100) );
        m_gradient->setColorAt( 0.0, Qt::white );
        m_gradient->setColorAt( 1.0, Qt::green );
        return;
    }

    m_gradients.first()->setHandleRadius( m_canvas->resourceProvider()->handleRadius() );
    delete m_gradient;
    GradientStrategy * strategy = m_gradients.first();
    m_gradient = KarbonGradientHelper::cloneGradient( strategy->gradient() );
    if( m_gradientWidget )
    {
        m_gradientWidget->setGradient( m_gradient );
        if( strategy->target() == GradientStrategy::Fill )
            m_gradientWidget->setTarget( VGradientTabWidget::FillGradient );
        else
            m_gradientWidget->setTarget( VGradientTabWidget::StrokeGradient );
    }
}

void KarbonGradientTool::deactivate()
{
    delete m_gradient;
    m_gradient = 0;

    m_currentStrategy = 0;
    qDeleteAll( m_gradients );
    m_gradients.clear();
}

void KarbonGradientTool::resourceChanged( int key, const QVariant & res )
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
    QVBoxLayout* layout = new QVBoxLayout( optionWidget );
    layout->setMargin(6);

    m_gradientWidget = new VGradientTabWidget( optionWidget );
    m_gradientWidget->setGradient( m_gradient );
    layout->addWidget( m_gradientWidget );

    connect( m_gradientWidget, SIGNAL(changed()), this, SLOT(gradientChanged()) );

    return optionWidget;
}

void KarbonGradientTool::gradientChanged()
{
    QList<KoShape*> selectedShapes = m_canvas->shapeManager()->selection()->selectedShapes();
    QBrush newBrush( *m_gradientWidget->gradient() );
    if( m_gradientWidget->target() == VGradientTabWidget::FillGradient )
    {
        m_canvas->addCommand( new KoShapeBackgroundCommand( selectedShapes, newBrush ) );
    }
    else
    {
        QList<KoShapeBorderModel*> newBorders;
        foreach( KoShape * shape, selectedShapes )
        {
            KoLineBorder * border = dynamic_cast<KoLineBorder*>( shape->border() );
            KoLineBorder * newBorder = 0;
            if( border )
                newBorder = new KoLineBorder( *border );
            else
                newBorder = new KoLineBorder( 1.0 );
            newBorder->setLineBrush( newBrush );
            newBorders.append( newBorder );
        }
        m_canvas->addCommand( new KoShapeBorderCommand( selectedShapes, newBorders ) );
    }
    initialize();
}

#include "KarbonGradientTool.moc"
