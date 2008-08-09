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

#include "KarbonPatternTool.h"
#include "KarbonPatternChooser.h"
#include "KarbonPatternEditStrategy.h"
#include "KarbonPatternItem.h"
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
    foreach( KarbonPatternEditStrategyBase *strategy, m_patterns )
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

void KarbonPatternTool::repaintDecorations()
{
    foreach( KarbonPatternEditStrategyBase *strategy, m_patterns )
        m_canvas->updateCanvas( strategy->boundingRect() );
}

void KarbonPatternTool::mousePressEvent( KoPointerEvent *event )
{
    m_currentStrategy = 0;

    foreach( KarbonPatternEditStrategyBase *strategy, m_patterns )
    {
        if( strategy->selectHandle( event->point ) )
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
    foreach( KarbonPatternEditStrategyBase *strategy, m_patterns )
    {
        if( strategy->selectHandle( event->point ) )
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

    m_currentStrategy = 0;

    uint strategyCount = m_patterns.count();
    for( uint i = 0; i < strategyCount; ++i )
    {
        KarbonPatternEditStrategyBase * s = m_patterns.takeFirst();
        s->repaint();
        delete s;
    }

    KoDataCenter * dataCenter = m_canvas->shapeController()->dataCenter( "ImageCollection" );
    KoImageCollection * imageCollection = dynamic_cast<KoImageCollection*>( dataCenter );

    foreach( KoShape *shape, m_canvas->shapeManager()->selection()->selectedShapes() )
    {
        if( dynamic_cast<KoPatternBackground*>( shape->background() ) )
        {
            m_patterns.append( new KarbonOdfPatternEditStrategy( shape, imageCollection ) );
            m_patterns.last()->repaint();
        }
    }
    if( m_patterns.count() == 1 )
    {
        m_currentStrategy = m_patterns.first();
        updateOptionsWidget();
    }
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

    useCursor(Qt::ArrowCursor, true);

    connect( m_canvas->shapeManager(), SIGNAL(selectionContentChanged()), this, SLOT(initialize()));
}

void KarbonPatternTool::deactivate()
{
    // we are not interested in selection content changes when not active
    disconnect( m_canvas->shapeManager(), SIGNAL(selectionContentChanged()), this, SLOT(initialize()));

    foreach( KarbonPatternEditStrategyBase * strategy, m_patterns )
    {
        strategy->repaint();
        delete strategy;
    }
    m_patterns.clear();
    foreach( KoShape *shape, m_canvas->shapeManager()->selection()->selectedShapes() )
        shape->update();
}

void KarbonPatternTool::resourceChanged( int key, const QVariant & res )
{
    switch( key )
    {
        case KoCanvasResource::HandleRadius:
            foreach( KarbonPatternEditStrategyBase *strategy, m_patterns )
                strategy->repaint();

            KarbonPatternEditStrategyBase::setHandleRadius( res.toUInt() );

            foreach( KarbonPatternEditStrategyBase *strategy, m_patterns )
                strategy->repaint();
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

    KarbonPatternChooser * chooser = new KarbonPatternChooser();
    connect( chooser, SIGNAL( itemDoubleClicked(QTableWidgetItem*)),
             this, SLOT(patternSelected(QTableWidgetItem*)));

    widgets.insert( "Pattern Options", m_optionsWidget );
    widgets.insert( "Patterns", chooser );

    updateOptionsWidget();

    return widgets;
}

void KarbonPatternTool::patternSelected( QTableWidgetItem * item )
{
    KarbonPatternItem * currentPattern = dynamic_cast<KarbonPatternItem*>(item);
    if( ! currentPattern || ! currentPattern->pattern()->valid() )
        return;

    KoDataCenter * dataCenter = m_canvas->shapeController()->dataCenter( "ImageCollection" );
    KoImageCollection * imageCollection = dynamic_cast<KoImageCollection*>( dataCenter );
    if( imageCollection )
    {
        QList<KoShape*> selectedShapes = m_canvas->shapeManager()->selection()->selectedShapes();
        KoPatternBackground * newFill = new KoPatternBackground( imageCollection );
        newFill->setPattern( currentPattern->pattern()->img() );
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
