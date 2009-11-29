/* This file is part of the KDE project
 * Copyright (C) 2007,2009 Jan Hambrecht <jaham@gmx.net>
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

#include "KarbonPatternTool.h"
#include "KarbonPatternEditStrategy.h"
#include <KarbonPatternOptionsWidget.h>

#include <KoCanvasBase.h>
#include <KoShapeManager.h>
#include <KoSelection.h>
#include <KoShape.h>
#include <KoCanvasResourceProvider.h>
#include <KoShapeBackgroundCommand.h>
#include <KoPointerEvent.h>
#include <KoPattern.h>
#include <KoPatternBackground.h>
#include <KoImageCollection.h>
#include <KoShapeController.h>
#include <KoResource.h>
#include <KoResourceServerProvider.h>
#include <KoResourceItemChooser.h>
#include <KoResourceServerAdapter.h>

#include <KLocale>

#include <QtGui/QPainter>
#include <QtGui/QWidget>
#include <QtGui/QUndoCommand>

KarbonPatternTool::KarbonPatternTool(KoCanvasBase *canvas)
    : KoTool( canvas ), m_currentStrategy( 0 ), m_optionsWidget(0)
{
}

KarbonPatternTool::~KarbonPatternTool()
{
}

void KarbonPatternTool::paint( QPainter &painter, const KoViewConverter &converter )
{
    painter.setBrush( Qt::green ); //TODO make configurable
    painter.setPen( Qt::blue ); //TODO make configurable

    // paint all the strategies
    foreach( KarbonPatternEditStrategyBase *strategy, m_strategies )
    {
        if( strategy == m_currentStrategy )
            continue;
        
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

void KarbonPatternTool::repaintDecorations()
{
    foreach( KarbonPatternEditStrategyBase *strategy, m_strategies )
        m_canvas->updateCanvas( strategy->boundingRect() );
}

void KarbonPatternTool::mousePressEvent( KoPointerEvent *event )
{
    //m_currentStrategy = 0;

    foreach( KarbonPatternEditStrategyBase *strategy, m_strategies )
    {
        if( strategy->selectHandle( event->point, *m_canvas->viewConverter() ) )
        {
            m_currentStrategy = strategy;
            m_currentStrategy->repaint();
            useCursor(Qt::SizeAllCursor);
            break;
        }
    }
    if( m_currentStrategy )
    {
        m_currentStrategy->setEditing( true );
        updateOptionsWidget();
    }
}

void KarbonPatternTool::mouseMoveEvent( KoPointerEvent *event )
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
    foreach( KarbonPatternEditStrategyBase *strategy, m_strategies )
    {
        if( strategy->selectHandle( event->point, *m_canvas->viewConverter() ) )
        {
            useCursor(Qt::SizeAllCursor);
            return;
        }
    }
    useCursor(Qt::ArrowCursor);
}

void KarbonPatternTool::mouseReleaseEvent( KoPointerEvent *event )
{
    Q_UNUSED( event )
    // if we are editing, get out of edit mode and add a command to the stack
    if( m_currentStrategy && m_currentStrategy->isEditing() )
    {
        m_currentStrategy->setEditing( false );
        QUndoCommand * cmd = m_currentStrategy->createCommand();
        if( cmd )
            m_canvas->addCommand( cmd );

        updateOptionsWidget();
    }
}

void KarbonPatternTool::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
        case Qt::Key_I:
        {
            uint handleRadius = m_canvas->resourceProvider()->handleRadius();
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

void KarbonPatternTool::initialize()
{
    if( m_currentStrategy && m_currentStrategy->isEditing() )
        return;

    QList<KoShape*> selectedShapes = m_canvas->shapeManager()->selection()->selectedShapes();
    
    // remove all pattern strategies no longer applicable
    foreach( KarbonPatternEditStrategyBase * strategy, m_strategies )
    {
        // is this gradient shape still selected ?
        if( ! selectedShapes.contains( strategy->shape() ) || ! strategy->shape()->isEditable() )
        {
            m_strategies.remove( strategy->shape() );
            if( m_currentStrategy == strategy )
                m_currentStrategy = 0;
            delete strategy;
            continue;
        }
        
        // does the shape has no fill pattern anymore ?
        KoPatternBackground * fill = dynamic_cast<KoPatternBackground*>( strategy->shape()->background() );
        if( ! fill )
        {
            // delete the gradient
            m_strategies.remove( strategy->shape() );
            if( m_currentStrategy == strategy )
                m_currentStrategy = 0;
            delete strategy;
            continue;
        }
        
        strategy->updateHandles();
        strategy->repaint();
    }

    KoDataCenter * dataCenter = m_canvas->shapeController()->dataCenter( "ImageCollection" );
    KoImageCollection * imageCollection = dynamic_cast<KoImageCollection*>( dataCenter );

    // now create new strategies if needed
    foreach( KoShape *shape, selectedShapes )
    {
        if( ! shape->isEditable() )
            continue;
        
        // do we already have a strategy for that shape?
        if( m_strategies.contains(shape) )
            continue;
        
        if( dynamic_cast<KoPatternBackground*>( shape->background() ) )
        {
            KarbonPatternEditStrategyBase * s = new KarbonOdfPatternEditStrategy( shape, imageCollection );
            m_strategies.insert( shape, s );
            s->repaint();
        }
    }
    // automatically select strategy when editing single shape
    if( m_strategies.count() == 1 && ! m_currentStrategy )
    {
        m_currentStrategy = m_strategies.begin().value();
        updateOptionsWidget();
    }
    
    if( m_currentStrategy )
        m_currentStrategy->repaint();
}

void KarbonPatternTool::activate( bool temporary )
{
    Q_UNUSED(temporary);
    if( ! m_canvas->shapeManager()->selection()->count() )
    {
        emit done();
        return;
    }

    initialize();

    KarbonPatternEditStrategyBase::setHandleRadius( m_canvas->resourceProvider()->handleRadius() );
    KarbonPatternEditStrategyBase::setGrabSensitivity( m_canvas->resourceProvider()->grabSensitivity() );
    
    useCursor(Qt::ArrowCursor, true);

    connect( m_canvas->shapeManager(), SIGNAL(selectionContentChanged()), this, SLOT(initialize()));
}

void KarbonPatternTool::deactivate()
{
    // we are not interested in selection content changes when not active
    disconnect( m_canvas->shapeManager(), SIGNAL(selectionContentChanged()), this, SLOT(initialize()));

    foreach( KarbonPatternEditStrategyBase * strategy, m_strategies ) {
        strategy->repaint();
    }

    qDeleteAll(m_strategies);
    m_strategies.clear();
    
    foreach( KoShape *shape, m_canvas->shapeManager()->selection()->selectedShapes() )
        shape->update();
    
    m_currentStrategy = 0;
}

void KarbonPatternTool::resourceChanged( int key, const QVariant & res )
{
    switch( key )
    {
        case KoCanvasResource::HandleRadius:
            foreach( KarbonPatternEditStrategyBase *strategy, m_strategies )
                strategy->repaint();

            KarbonPatternEditStrategyBase::setHandleRadius( res.toUInt() );

            foreach( KarbonPatternEditStrategyBase *strategy, m_strategies )
                strategy->repaint();
            break;
        case KoCanvasResource::GrabSensitivity:
            KarbonPatternEditStrategyBase::setGrabSensitivity( res.toUInt() );
            break;
        default:
            return;
    }
}

QMap<QString, QWidget *> KarbonPatternTool::createOptionWidgets()
{
    QMap<QString, QWidget *> widgets;

    m_optionsWidget = new KarbonPatternOptionsWidget();
    connect( m_optionsWidget, SIGNAL(patternChanged()),
             this, SLOT(patternChanged()) );

    KoResourceServer<KoPattern> * rserver = KoResourceServerProvider::instance()->patternServer();
    KoAbstractResourceServerAdapter* adapter = new KoResourceServerAdapter<KoPattern>(rserver);
    KoResourceItemChooser * chooser = new KoResourceItemChooser(adapter, m_optionsWidget);
    chooser->setObjectName("KarbonPatternChooser");

    connect( chooser, SIGNAL(resourceSelected(KoResource*)),
             this, SLOT(patternSelected(KoResource*)));

    widgets.insert( i18n("Pattern Options"), m_optionsWidget );
    widgets.insert( i18n("Patterns"), chooser );

    updateOptionsWidget();

    return widgets;
}

void KarbonPatternTool::patternSelected( KoResource * resource )
{
    KoPattern * currentPattern = dynamic_cast<KoPattern*>(resource);
    if( ! currentPattern || ! currentPattern->valid() )
        return;

    KoDataCenter * dataCenter = m_canvas->shapeController()->dataCenter( "ImageCollection" );
    KoImageCollection * imageCollection = dynamic_cast<KoImageCollection*>( dataCenter );
    if( imageCollection )
    {
        QList<KoShape*> selectedShapes = m_canvas->shapeManager()->selection()->selectedShapes();
        KoPatternBackground * newFill = new KoPatternBackground( imageCollection );
        newFill->setPattern( currentPattern->image() );
        m_canvas->addCommand( new KoShapeBackgroundCommand( selectedShapes, newFill ) );
        initialize();
    }
}

void KarbonPatternTool::updateOptionsWidget()
{
    if( m_optionsWidget && m_currentStrategy )
    {
        KoPatternBackground * fill = dynamic_cast<KoPatternBackground*>( m_currentStrategy->shape()->background() );
        if( fill )
        {
            m_optionsWidget->setRepeat( fill->repeat() );
            m_optionsWidget->setReferencePoint( fill->referencePoint() );
            m_optionsWidget->setReferencePointOffset( fill->referencePointOffset() );
            m_optionsWidget->setTileRepeatOffset( fill->tileRepeatOffset() );
            m_optionsWidget->setPatternSize( fill->patternDisplaySize().toSize() );
        }
    }
}

void KarbonPatternTool::patternChanged()
{
    if( m_currentStrategy )
    {
        KoShape * shape = m_currentStrategy->shape();
        KoPatternBackground * oldFill = dynamic_cast<KoPatternBackground*>( shape->background() );
        if( ! oldFill )
            return;
        KoDataCenter * dataCenter = m_canvas->shapeController()->dataCenter( "ImageCollection" );
        if( ! dataCenter )
            return;
        KoImageCollection * imageCollection = dynamic_cast<KoImageCollection*>( dataCenter );
        if( ! imageCollection )
            return;
        KoPatternBackground * newFill = new KoPatternBackground( imageCollection );
        if( ! newFill )
            return;
        *newFill = *oldFill;
        newFill->setRepeat( m_optionsWidget->repeat() );
        newFill->setReferencePoint( m_optionsWidget->referencePoint() );
        newFill->setReferencePointOffset( m_optionsWidget->referencePointOffset() );
        newFill->setTileRepeatOffset( m_optionsWidget->tileRepeatOffset() );
        newFill->setPatternDisplaySize( m_optionsWidget->patternSize() );
        m_canvas->addCommand( new KoShapeBackgroundCommand( shape, newFill ) );
    }
}

#include "KarbonPatternTool.moc"
