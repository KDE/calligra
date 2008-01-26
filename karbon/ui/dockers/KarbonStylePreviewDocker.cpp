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

#include "KarbonStylePreviewDocker.h"
#include "KarbonStylePreview.h"
#include "KarbonStyleButtonBox.h"
#include "Karbon.h"
#include <KarbonPatternChooser.h>
#include <KarbonPatternItem.h>
#include <KarbonGradientChooser.h>
#include <KarbonGradientItem.h>

#include <KoToolManager.h>
#include <KoCanvasBase.h>
#include <KoCanvasController.h>
#include <KoCanvasResourceProvider.h>
#include <KoShapeManager.h>
#include <KoSelection.h>
#include <KoUniColorChooser.h>
#include <KoCheckerBoardPainter.h>
#include <KoLineBorder.h>
#include <KoShapeBorderCommand.h>
#include <KoShapeBackgroundCommand.h>
#include <KoPathFillRuleCommand.h>
#include <KoPathShape.h>
#include <KoColorSpaceRegistry.h>

#include <klocale.h>

#include <QtGui/QGridLayout>
#include <QtGui/QStackedWidget>

KarbonStylePreviewDocker::KarbonStylePreviewDocker( QWidget * parent )
    : QDockWidget( parent ), m_canvas(0)
{
    setWindowTitle( i18n( "Styles" ) );

    QWidget *mainWidget = new QWidget( this );
    QGridLayout * layout = new QGridLayout( mainWidget );

    m_preview = new KarbonStylePreview( mainWidget );
    layout->addWidget( m_preview, 0, 0 );

    m_buttons = new KarbonStyleButtonBox( mainWidget );
    layout->addWidget( m_buttons, 1, 0 );

    m_stack = new QStackedWidget( mainWidget );
    layout->addWidget( m_stack, 0, 1, 2, 1 );

    layout->setColumnStretch( 0, 1 );
    layout->setColumnStretch( 1, 3 );

    m_colorChooser = new KoUniColorChooser( m_stack, true );
    m_colorChooser->changeLayout( KoUniColorChooser::SimpleLayout );
    m_stack->addWidget( m_colorChooser );

    m_gradientChooser = new KarbonGradientChooser( m_stack );
    m_gradientChooser->showButtons( false );
    m_stack->addWidget( m_gradientChooser );

    m_patternChooser = new KarbonPatternChooser( m_stack );
    m_patternChooser->showButtons( false );
    m_stack->addWidget( m_patternChooser );

    connect( m_preview, SIGNAL(fillSelected()), this, SLOT(fillSelected()) );
    connect( m_preview, SIGNAL(strokeSelected()), this, SLOT(strokeSelected()) );
    connect( m_buttons, SIGNAL(buttonPressed(int)), this, SLOT(styleButtonPressed(int)));
    connect( m_colorChooser, SIGNAL( sigColorChanged( const KoColor &) ), 
             this, SLOT( updateColor( const KoColor &) ) );
    connect( m_gradientChooser, SIGNAL( itemDoubleClicked( QTableWidgetItem * ) ), 
             this, SLOT( updateGradient( QTableWidgetItem* ) ) );
    connect( m_patternChooser, SIGNAL( itemDoubleClicked( QTableWidgetItem * ) ), 
             this, SLOT( updatePattern( QTableWidgetItem* ) ) );

    setWidget( mainWidget );

    setCanvas( KoToolManager::instance()->activeCanvasController()->canvas() );
}

KarbonStylePreviewDocker::~KarbonStylePreviewDocker()
{
}

void KarbonStylePreviewDocker::setCanvas( KoCanvasBase * canvas )
{
    m_canvas = canvas;
    if( ! m_canvas )
    {
        updateStyle( 0, QBrush( Qt::NoBrush ) );
        return;
    }

    connect( m_canvas->shapeManager(), SIGNAL(selectionChanged()),
            this, SLOT(selectionChanged()));

    connect( m_canvas->resourceProvider(), SIGNAL(resourceChanged(int, const QVariant&)),
             this, SLOT(resourceChanged(int, const QVariant&)));

    KoShape * shape = m_canvas->shapeManager()->selection()->firstSelectedShape();
    if( shape )
        updateStyle( shape->border(), shape->background() );
    else
        updateStyle( 0, QBrush( Qt::NoBrush ) );
}

void KarbonStylePreviewDocker::selectionChanged()
{
    if( ! m_canvas )
        return;

    KoShape * shape = m_canvas->shapeManager()->selection()->firstSelectedShape();
    if( shape )
        updateStyle( shape->border(), shape->background() );
    else
        updateStyle( 0, QBrush( Qt::NoBrush ) );
}

bool KarbonStylePreviewDocker::strokeIsSelected() const
{
    return m_preview->strokeIsSelected();
}

void KarbonStylePreviewDocker::updateStyle( const KoShapeBorderModel * stroke, const QBrush & fill )
{
    KoCanvasResourceProvider * provider = m_canvas->resourceProvider();
    int activeStyle = provider->resource( Karbon::ActiveStyle ).toInt();

    QColor qColor( Qt::black );
    if( activeStyle == Karbon::Foreground )
    {
        const KoLineBorder * border = dynamic_cast<const KoLineBorder*>( stroke );
        if( border )
            qColor = border->color();
    }
    else
    {
        if( fill.style() == Qt::SolidPattern )
            qColor = fill.color();
    }
    KoColor c( qColor, qColor.alpha(), KoColorSpaceRegistry::instance()->rgb8() );
    m_colorChooser->setColor( c );

    m_preview->update( stroke, fill );
}

void KarbonStylePreviewDocker::fillSelected()
{
    if( ! m_canvas )
        return;

    m_canvas->resourceProvider()->setResource( Karbon::ActiveStyle, Karbon::Background );
    m_buttons->setFill();
}

void KarbonStylePreviewDocker::strokeSelected()
{
    if( ! m_canvas )
        return;

    m_canvas->resourceProvider()->setResource( Karbon::ActiveStyle, Karbon::Foreground );
    m_buttons->setStroke();
}

void KarbonStylePreviewDocker::resourceChanged( int key, const QVariant& )
{
    switch( key )
    {
        case KoCanvasResource::ForegroundColor:
        case KoCanvasResource::BackgroundColor:
            selectionChanged();
            break;
    }
}

void KarbonStylePreviewDocker::styleButtonPressed( int buttonId )
{
    switch( buttonId )
    {
        case KarbonStyleButtonBox::None:
        {
            KoCanvasResourceProvider * provider = m_canvas->resourceProvider();
            KoSelection *selection = m_canvas->shapeManager()->selection();
            if( ! selection || ! selection->count() )
                break;

            if( provider->resource( Karbon::ActiveStyle ).toInt() == Karbon::Background )
                m_canvas->addCommand( new KoShapeBackgroundCommand( selection->selectedShapes(), QBrush() ) );
            else
                m_canvas->addCommand( new KoShapeBorderCommand( selection->selectedShapes(), 0 ) );
            m_stack->setCurrentIndex( 0 );
            selectionChanged();
            break;
        }
        case KarbonStyleButtonBox::Solid:
            m_stack->setCurrentIndex( 0 );
            break;
        case KarbonStyleButtonBox::Gradient:
            m_stack->setCurrentIndex( 1 );
            break;
        case KarbonStyleButtonBox::Pattern:
            m_stack->setCurrentIndex( 2 );
            break;
        case KarbonStyleButtonBox::EvenOdd:
            updateFillRule( Qt::OddEvenFill );
            break;
        case KarbonStyleButtonBox::Winding:
            updateFillRule( Qt::WindingFill );
            break;
    }
}

void KarbonStylePreviewDocker::updateColor( const KoColor &c )
{
    if( ! m_canvas )
        return;

    KoSelection *selection = m_canvas->shapeManager()->selection();
    if( ! selection || ! selection->count() )
        return;

    QColor color;
    quint8 opacity;
    c.toQColor(&color, &opacity);
    color.setAlpha(opacity);

    KoCanvasResourceProvider * provider = m_canvas->resourceProvider();
    int activeStyle = provider->resource( Karbon::ActiveStyle ).toInt();

    // check which color to set foreground == border, background == fill
    if( activeStyle == Karbon::Foreground )
    {
        // get the border of the first selected shape and check if it is a line border
        KoLineBorder * oldBorder = dynamic_cast<KoLineBorder*>( selection->firstSelectedShape()->border() );
        KoLineBorder * newBorder = 0;
        if( oldBorder )
        {
            // preserve the properties of the old border if it is a line border
            newBorder = new KoLineBorder( *oldBorder );
            newBorder->setColor( color );
        }
        else
            newBorder = new KoLineBorder( 1.0, color );

        KoShapeBorderCommand * cmd = new KoShapeBorderCommand( selection->selectedShapes(), newBorder );
        m_canvas->addCommand( cmd );
        m_canvas->resourceProvider()->setForegroundColor( c );
    }
    else
    {
        KoShapeBackgroundCommand *cmd = new KoShapeBackgroundCommand( selection->selectedShapes(), QBrush( color ) );
        m_canvas->addCommand( cmd );
        m_canvas->resourceProvider()->setBackgroundColor( c );
    }
    selectionChanged();
}

void KarbonStylePreviewDocker::updateGradient( QTableWidgetItem * item )
{
    if( ! item )
        return;

    KarbonGradientItem * gradientItem = dynamic_cast<KarbonGradientItem*>(item);
    if( ! gradientItem )
        return;

    QList<KoShape*> selectedShapes = m_canvas->shapeManager()->selection()->selectedShapes();
    if( ! selectedShapes.count() )
        return;

    QGradient * newGradient = gradientItem->gradient()->toQGradient();
    QBrush newBrush( *newGradient );
    delete newGradient;

    KoCanvasResourceProvider * provider = m_canvas->resourceProvider();
    int activeStyle = provider->resource( Karbon::ActiveStyle ).toInt();

    // check which color to set foreground == border, background == fill
    if( activeStyle == Karbon::Background )
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
    selectionChanged();
}

void KarbonStylePreviewDocker::updatePattern( QTableWidgetItem * item )
{
    KarbonPatternItem * currentPattern = dynamic_cast<KarbonPatternItem*>(item);
    if( ! currentPattern || ! currentPattern->pattern()->valid() )
        return;

    QList<KoShape*> selectedShapes = m_canvas->shapeManager()->selection()->selectedShapes();
    if( ! selectedShapes.count() )
        return;

    QBrush newBrush( currentPattern->pattern()->img() );
    m_canvas->addCommand( new KoShapeBackgroundCommand( selectedShapes, newBrush ) );
    selectionChanged();
}

void KarbonStylePreviewDocker::updateFillRule( Qt::FillRule fillRule )
{
    if( ! m_canvas )
        return;

    KoSelection *selection = m_canvas->shapeManager()->selection();
    if( ! selection || ! selection->count() )
        return;

    QList<KoPathShape*> shapes;
    foreach( KoShape * shape, selection->selectedShapes() )
    {
        KoPathShape * path = dynamic_cast<KoPathShape*>( shape );
        if( path && path->fillRule() != fillRule )
            shapes.append( path );
    }
    if( shapes.count() )
        m_canvas->addCommand( new KoPathFillRuleCommand( shapes, fillRule ) );
}


KarbonStylePreviewDockerFactory::KarbonStylePreviewDockerFactory()
{
}

QString KarbonStylePreviewDockerFactory::id() const
{
    return QString("Style Preview");
}

KoDockFactory::DockPosition KarbonStylePreviewDockerFactory::defaultDockPosition() const
{
    return DockLeft;
}

QDockWidget* KarbonStylePreviewDockerFactory::createDockWidget()
{
    KarbonStylePreviewDocker * widget = new KarbonStylePreviewDocker();
    widget->setObjectName(id());

    return widget;
}

#include "KarbonStylePreviewDocker.moc"
