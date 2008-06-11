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

#include <KoCanvasBase.h>
#include <KoShapeManager.h>
#include <KoSelection.h>
#include <KoShape.h>
#include <KoCanvasResourceProvider.h>
#include <KoShapeBackgroundCommand.h>
#include <KoPointerEvent.h>
#include <KoPattern.h>
#include <KoPatternBackground.h>

#include <QtGui/QPainter>
#include <QtGui/QWidget>
#include <QtGui/QUndoCommand>

KarbonPatternTool::KarbonPatternTool(KoCanvasBase *canvas)
: KoTool( canvas ), m_currentStrategy( 0 )
{
    connect( m_canvas->shapeManager(), SIGNAL(selectionContentChanged()), this, SLOT(initialize()));
}

KarbonPatternTool::~KarbonPatternTool()
{
}

void KarbonPatternTool::paint( QPainter &painter, const KoViewConverter &converter )
{
    painter.setBrush( Qt::green ); //TODO make configurable
    painter.setPen( Qt::blue ); //TODO make configurable

    // paint all the strategies
    foreach( KarbonPatternEditStrategy *strategy, m_patterns )
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
    foreach( KarbonPatternEditStrategy *strategy, m_patterns )
        m_canvas->updateCanvas( strategy->boundingRect() );
}

void KarbonPatternTool::mousePressEvent( KoPointerEvent *event )
{
    Q_UNUSED( event )
    if( m_currentStrategy )
        m_currentStrategy->setEditing( true );
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
    foreach( KarbonPatternEditStrategy *strategy, m_patterns )
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

void KarbonPatternTool::mouseReleaseEvent( KoPointerEvent *event )
{
    Q_UNUSED( event )
    // if we are editing, get out of edit mode and add a command to the stack
    if( m_currentStrategy )
    {
        m_currentStrategy->setEditing( false );
        m_canvas->addCommand( m_currentStrategy->createCommand() );
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

    foreach( KarbonPatternEditStrategy* strategy, m_patterns )
    {
        strategy->repaint();
        delete strategy;
    }
    m_patterns.clear();

    foreach( KoShape *shape, m_canvas->shapeManager()->selection()->selectedShapes() )
    {
        if( dynamic_cast<KoPatternBackground*>( shape->background() ) )
        {
            m_patterns.append( new KarbonPatternEditStrategy( shape ) );
            m_patterns.last()->repaint();
        }
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

    KarbonPatternEditStrategy::setHandleRadius( m_canvas->resourceProvider()->handleRadius() );

    useCursor(Qt::ArrowCursor, true);
}

void KarbonPatternTool::deactivate()
{
    foreach( KarbonPatternEditStrategy* strategy, m_patterns )
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
            foreach( KarbonPatternEditStrategy *strategy, m_patterns )
                strategy->repaint();

            KarbonPatternEditStrategy::setHandleRadius( res.toUInt() );

            foreach( KarbonPatternEditStrategy *strategy, m_patterns )
                strategy->repaint();
        break;
        default:
            return;
    }
}

QWidget * KarbonPatternTool::createOptionWidget()
{
    KarbonPatternChooser * chooser = new KarbonPatternChooser();
    connect( chooser, SIGNAL( itemDoubleClicked(QTableWidgetItem*)),
             this, SLOT(patternSelected(QTableWidgetItem*)));
    return chooser;
}

void KarbonPatternTool::patternSelected( QTableWidgetItem * item )
{
    KarbonPatternItem * currentPattern = dynamic_cast<KarbonPatternItem*>(item);
    if( ! currentPattern || ! currentPattern->pattern()->valid() )
        return;

    QList<KoShape*> selectedShapes = m_canvas->shapeManager()->selection()->selectedShapes();
    KoPatternBackground * newFill = new KoPatternBackground( currentPattern->pattern()->img() );
    m_canvas->addCommand( new KoShapeBackgroundCommand( selectedShapes, newFill ) );
    initialize();
}

#include "KarbonPatternTool.moc"
