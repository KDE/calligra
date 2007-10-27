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
#include "KarbonPatternEditStrategy.h"
#include "KarbonPatternItem.h"

#include <karbon_factory.h>
#include <karbon_resourceserver.h>

#include <KoResourceChooser.h>
#include <KoCanvasBase.h>
#include <KoShapeManager.h>
#include <KoSelection.h>
#include <KoShape.h>
#include <KoCanvasResourceProvider.h>
#include <KoShapeBackgroundCommand.h>
#include <KoPointerEvent.h>
#include <KoPattern.h>

#include <klocale.h>
#include <kiconloader.h>
#include <kfiledialog.h>

#include <QPainter>
#include <QWidget>
#include <QGridLayout>
#include <QButtonGroup>
#include <QPushButton>
#include <QFileInfo>
#include <QUndoCommand>

KarbonPatternTool::KarbonPatternTool(KoCanvasBase *canvas)
: KoTool( canvas ), m_patternChooser( 0 ), m_buttonGroup( 0 )
, m_currentPattern( 0 ), m_currentStrategy( 0 )
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
    m_currentStrategy = 0;

    foreach( KarbonPatternEditStrategy* strategy, m_patterns )
    {
        strategy->repaint();
        delete strategy;
    }
    m_patterns.clear();

    foreach( KoShape *shape, m_canvas->shapeManager()->selection()->selectedShapes() )
    {
        const QBrush &background = shape->background();
        if( background.style() == Qt::TexturePattern )
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
    QWidget *optionWidget = new QWidget();
    QGridLayout* layout = new QGridLayout( optionWidget );

    QList<KoPattern*> patterns = KarbonFactory::rServer()->patterns();
    QList<QTableWidgetItem*> items;
    foreach( KoPattern* p, patterns )
        items.append( static_cast<QTableWidgetItem*>( new KarbonPatternItem( p ) ) );

    m_patternChooser = new KoPatternChooser( items, optionWidget );
    //m_patternChooser->setFixedSize( 180, 120 );
    layout->addWidget( m_patternChooser, 0, 0, 1, 3 );

    m_buttonGroup = new QButtonGroup( optionWidget );
    m_buttonGroup->setExclusive( false );

    QPushButton *button = new QPushButton( optionWidget );
    button->setIcon( SmallIcon( "list-add" ) );
    button->setToolTip( i18n("Import pattern") );
    button->setEnabled( true );
    m_buttonGroup->addButton( button, Button_Import );
    layout->addWidget( button, 1, 0 );

    button = new QPushButton( optionWidget );
    button->setIcon( SmallIcon( "list-remove" ) );
    button->setToolTip( i18n("Delete pattern") );
    button->setEnabled( false );
    m_buttonGroup->addButton( button, Button_Remove );
    layout->addWidget( button, 1, 1 );

    layout->setColumnStretch( 0, 1 );
    layout->setColumnStretch( 1, 1 );
    layout->setColumnStretch( 2, 2 );
    layout->setSpacing( 0 );
    layout->setMargin( 3 );

    connect( m_buttonGroup, SIGNAL( buttonClicked( int ) ), this, SLOT( slotButtonClicked( int ) ) );
    connect( m_patternChooser, SIGNAL( selected( QTableWidgetItem* ) ), this, SLOT( patternSelected( QTableWidgetItem* ) ) );

    if( !items.isEmpty() )
        m_currentPattern = static_cast<KarbonPatternItem*>( items.first() );

    return optionWidget;
}

void KarbonPatternTool::slotButtonClicked( int button )
{
    if( button == Button_Import )
        importPattern();
    else if( button == Button_Remove )
        deletePattern();
}

void KarbonPatternTool::patternSelected( QTableWidgetItem* item )
{
    m_currentPattern = dynamic_cast<KarbonPatternItem*>( item );
    if( ! m_currentPattern || ! m_currentPattern->pattern()->valid() )
        return;

    QAbstractButton * removeButton = m_buttonGroup->button( Button_Remove );
    if( removeButton )
        removeButton->setEnabled( QFileInfo( m_currentPattern->pattern()->filename() ).isWritable() );

    QList<KoShape*> selectedShapes = m_canvas->shapeManager()->selection()->selectedShapes();
    QBrush newBrush( m_currentPattern->pattern()->img() );
    m_canvas->addCommand( new KoShapeBackgroundCommand( selectedShapes, newBrush ) );
    initialize();
}

void KarbonPatternTool::importPattern()
{
    QString filter( "*.jpg *.gif *.png *.tif *.xpm *.bmp" );
    KoPattern* pattern = KarbonFactory::rServer()->addPattern(
        KFileDialog::getOpenFileName( KUrl(), filter, 0, i18n( "Choose Pattern to Add" ) ) );
    if( pattern )
        m_patternChooser->addPattern( new KarbonPatternItem( pattern ) );
}

void KarbonPatternTool::deletePattern()
{
    KoPattern * pattern = m_currentPattern->pattern();
    m_patternChooser->removePattern( m_currentPattern );
    KarbonFactory::rServer()->removePattern( pattern );
    m_currentPattern = static_cast<KarbonPatternItem*>( m_patternChooser->currentPattern() );
}

#include "KarbonPatternTool.moc"
