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

#include "KarbonStyleDocker.h"
#include "KarbonStylePreview.h"
#include "KarbonStyleButtonBox.h"
#include "Karbon.h"
#include <KarbonPatternChooser.h>
#include <KarbonPatternItem.h>
#include <KarbonGradientChooser.h>
#include <KarbonGradientItem.h>
#include <KarbonGradientHelper.h>

#include <KoToolManager.h>
#include <KoCanvasBase.h>
#include <KoCanvasController.h>
#include <KoCanvasResourceProvider.h>
#include <KoShapeManager.h>
#include <KoSelection.h>
#include <KoTriangleColorSelector.h>
#include <KoCheckerBoardPainter.h>
#include <KoLineBorder.h>
#include <KoShapeBorderCommand.h>
#include <KoShapeBackgroundCommand.h>
#include <KoPathFillRuleCommand.h>
#include <KoPathShape.h>
#include <KoColorSpaceRegistry.h>
#include <KoColorBackground.h>
#include <KoPatternBackground.h>
#include <KoImageCollection.h>
#include <KoShapeController.h>
#include <KoColorSlider.h>

#include <klocale.h>

#include <QtGui/QGridLayout>
#include <QtGui/QStackedWidget>

KarbonStyleDocker::KarbonStyleDocker( QWidget * parent )
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

    QWidget * colorWidget = new QWidget( m_stack );
    QGridLayout * gLayout = new QGridLayout( colorWidget );
    m_colorChooser = new KoTriangleColorSelector( colorWidget );
    m_opacitySlider = new KoColorSlider( Qt::Vertical, colorWidget );
    m_opacitySlider->setFixedWidth(25);
    m_opacitySlider->setRange(0, 255);
    m_opacitySlider->setToolTip( i18n( "Opacity" ) );
    gLayout->addWidget( m_colorChooser, 0, 0 );
    gLayout->addWidget( m_opacitySlider, 0, 1 );
    m_stack->addWidget( colorWidget );

    m_gradientChooser = new KarbonGradientChooser( m_stack );
    m_gradientChooser->showButtons( false );
    m_stack->addWidget( m_gradientChooser );

    m_patternChooser = new KarbonPatternChooser( m_stack );
    m_patternChooser->showButtons( false );
    m_stack->addWidget( m_patternChooser );

    connect( m_preview, SIGNAL(fillSelected()), this, SLOT(fillSelected()) );
    connect( m_preview, SIGNAL(strokeSelected()), this, SLOT(strokeSelected()) );
    connect( m_buttons, SIGNAL(buttonPressed(int)), this, SLOT(styleButtonPressed(int)));
    connect( m_colorChooser, SIGNAL( colorChanged( const QColor &) ), 
             this, SLOT( colorChanged( const QColor &) ) );
    connect( m_gradientChooser, SIGNAL( selected( QTableWidgetItem * ) ), 
             this, SLOT( updateGradient( QTableWidgetItem* ) ) );
    connect( m_patternChooser, SIGNAL( selected( QTableWidgetItem * ) ), 
             this, SLOT( updatePattern( QTableWidgetItem* ) ) );
    connect(m_opacitySlider, SIGNAL(valueChanged(int)), this, SLOT(opacityChanged(int)));

    setWidget( mainWidget );
}

KarbonStyleDocker::~KarbonStyleDocker()
{
}

void KarbonStyleDocker::setCanvas( KoCanvasBase * canvas )
{
    m_canvas = canvas;
    if( ! m_canvas )
    {
        updateStyle( 0, 0 );
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
    {
        QVariant variant = m_canvas->resourceProvider()->resource( KoCanvasResource::CurrentPage );
        if( !variant.isNull() )
        {
            KoShape* page = static_cast<KoShape*>( variant.value<void*>() );
            if( page )
            {
                updateStyle( page->border(), page->background() );
            }
        }
        else
        {
            updateStyle( 0, 0 );
        }
    }
}

void KarbonStyleDocker::selectionChanged()
{
    if( ! m_canvas )
        return;

    KoShape * shape = m_canvas->shapeManager()->selection()->firstSelectedShape();
    if( shape )
        updateStyle( shape->border(), shape->background() );
    else
        updateStyle( 0, 0 );
}

bool KarbonStyleDocker::strokeIsSelected() const
{
    return m_preview->strokeIsSelected();
}

void KarbonStyleDocker::updateStyle( const KoShapeBorderModel * stroke, const KoShapeBackground * fill )
{
    KoCanvasResourceProvider * provider = m_canvas->resourceProvider();
    int activeStyle = provider->resource( Karbon::ActiveStyle ).toInt();

    QColor qColor;
    if( activeStyle == Karbon::Foreground )
    {
        const KoLineBorder * border = dynamic_cast<const KoLineBorder*>( stroke );
        if( border )
            qColor = border->color();
        else
            qColor = m_canvas->resourceProvider()->foregroundColor().toQColor();
    }
    else
    {
        const KoColorBackground * background = dynamic_cast<const KoColorBackground*>( fill );
        if( background )
            qColor = background->color();
        else
            qColor = m_canvas->resourceProvider()->backgroundColor().toQColor();
    }
    m_colorChooser->setQColor( qColor );

    KoColor minColor( qColor, KoColorSpaceRegistry::instance()->rgb8() );
    KoColor maxColor( qColor, KoColorSpaceRegistry::instance()->rgb8() );
    minColor.setOpacity( 0 );
    maxColor.setOpacity( 255 );
    m_opacitySlider->blockSignals( true );
    m_opacitySlider->setColors( minColor, maxColor );
    m_opacitySlider->setValue( qColor.alpha() );
    m_opacitySlider->blockSignals( false );

    m_preview->update( stroke, fill );
}

void KarbonStyleDocker::fillSelected()
{
    if( ! m_canvas )
        return;

    m_canvas->resourceProvider()->setResource( Karbon::ActiveStyle, Karbon::Background );
    m_buttons->setFill();
}

void KarbonStyleDocker::strokeSelected()
{
    if( ! m_canvas )
        return;

    m_canvas->resourceProvider()->setResource( Karbon::ActiveStyle, Karbon::Foreground );
    m_buttons->setStroke();
}

void KarbonStyleDocker::resourceChanged( int key, const QVariant& )
{
    switch( key )
    {
        case KoCanvasResource::ForegroundColor:
        case KoCanvasResource::BackgroundColor:
            selectionChanged();
            break;
    }
}

void KarbonStyleDocker::styleButtonPressed( int buttonId )
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
                m_canvas->addCommand( new KoShapeBackgroundCommand( selection->selectedShapes(), 0 ) );
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

void KarbonStyleDocker::opacityChanged( int opacity )
{
    QColor currentColor = m_colorChooser->color();
    currentColor.setAlpha( opacity);
    updateColor( currentColor );
}

void KarbonStyleDocker::colorChanged( const QColor &c )
{
    QColor currentColor = c;
    int opacity = m_opacitySlider->value();
    currentColor.setAlpha( opacity );

    KoColor minColor( c, KoColorSpaceRegistry::instance()->rgb8() );
    minColor.setOpacity( 0 );
    KoColor maxColor = minColor;
    maxColor.setOpacity( 255 );

    m_opacitySlider->setColors( minColor, maxColor );

    updateColor( currentColor );
}

void KarbonStyleDocker::updateColor( const QColor &c )
{
    if( ! m_canvas )
        return;

    KoColor kocolor( c, KoColorSpaceRegistry::instance()->rgb8() );

    KoCanvasResourceProvider * provider = m_canvas->resourceProvider();
    int activeStyle = provider->resource( Karbon::ActiveStyle ).toInt();

    KoSelection *selection = m_canvas->shapeManager()->selection();
    if( ! selection || ! selection->count() )
    {
        QVariant variant = provider->resource( KoCanvasResource::CurrentPage );
        if( !variant.isNull() )
        {
            KoShape* page = static_cast<KoShape*>( variant.value<void*>() );
            if( page )
            {
                // check which color to set foreground == border, background == fill
                if( activeStyle == Karbon::Foreground )
                {
                    // get the border of the first selected shape and check if it is a line border
                    KoLineBorder * oldBorder = dynamic_cast<KoLineBorder*>( page->border() );
                    KoLineBorder * newBorder = 0;
                    if( oldBorder )
                    {
                        // preserve the properties of the old border if it is a line border
                        newBorder = new KoLineBorder( *oldBorder );
                        newBorder->setColor( c );
                    }
                    else
                        newBorder = new KoLineBorder( 1.0, c );

                    KoShapeBorderCommand * cmd = new KoShapeBorderCommand( selection->selectedShapes(), newBorder );
                    m_canvas->addCommand( cmd );
                    m_canvas->resourceProvider()->setForegroundColor( kocolor );
                }
                else
                {
                    KoShapeBackground * fill = new KoColorBackground( c );
                    KoShapeBackgroundCommand *cmd = new KoShapeBackgroundCommand( page, fill );
                    m_canvas->addCommand( cmd );
                    m_canvas->resourceProvider()->setBackgroundColor( kocolor );
                }
                return;
            }
        }

        if( activeStyle == Karbon::Foreground )
            m_canvas->resourceProvider()->setForegroundColor( kocolor );
        else
            m_canvas->resourceProvider()->setBackgroundColor( kocolor );
        return;
    }

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
            newBorder->setColor( c );
        }
        else
            newBorder = new KoLineBorder( 1.0, c );

        KoShapeBorderCommand * cmd = new KoShapeBorderCommand( selection->selectedShapes(), newBorder );
        m_canvas->addCommand( cmd );
        m_canvas->resourceProvider()->setForegroundColor( kocolor );
    }
    else
    {
        KoShapeBackground * fill = new KoColorBackground( c );
        KoShapeBackgroundCommand *cmd = new KoShapeBackgroundCommand( selection->selectedShapes(), fill );
        m_canvas->addCommand( cmd );
        m_canvas->resourceProvider()->setBackgroundColor( kocolor );
    }
    selectionChanged();
}

void KarbonStyleDocker::updateGradient( QTableWidgetItem * item )
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
    if( ! newGradient )
        return;

    QGradientStops newStops = newGradient->stops();
    delete newGradient;

    KoCanvasResourceProvider * provider = m_canvas->resourceProvider();
    int activeStyle = provider->resource( Karbon::ActiveStyle ).toInt();

    // check which color to set foreground == border, background == fill
    if( activeStyle == Karbon::Background )
    {
        QUndoCommand * firstCommand = 0;
        foreach( KoShape * shape, selectedShapes )
        {
            KoShapeBackground * fill = KarbonGradientHelper::applyFillGradientStops( shape, newStops );
            if( ! fill )
                continue;
            if( ! firstCommand )
                firstCommand = new KoShapeBackgroundCommand( shape, fill );
            else
                new KoShapeBackgroundCommand( shape, fill, firstCommand );
        }
        m_canvas->addCommand( firstCommand );
    }
    else
    {
        QList<KoShapeBorderModel*> newBorders;
        foreach( KoShape * shape, selectedShapes )
        {
            QBrush brush = KarbonGradientHelper::applyStrokeGradientStops( shape, newStops );
            if( brush.style() == Qt::NoBrush )
                continue;

            KoLineBorder * border = dynamic_cast<KoLineBorder*>( shape->border() );
            KoLineBorder * newBorder = 0;
            if( border )
                newBorder = new KoLineBorder( *border );
            else
                newBorder = new KoLineBorder( 1.0 );
            newBorder->setLineBrush( brush );
            newBorders.append( newBorder );
        }
        m_canvas->addCommand( new KoShapeBorderCommand( selectedShapes, newBorders ) );
    }
    selectionChanged();
}

void KarbonStyleDocker::updatePattern( QTableWidgetItem * item )
{
    KarbonPatternItem * currentPattern = dynamic_cast<KarbonPatternItem*>(item);
    if( ! currentPattern || ! currentPattern->pattern()->valid() )
        return;

    QList<KoShape*> selectedShapes = m_canvas->shapeManager()->selection()->selectedShapes();
    if( ! selectedShapes.count() )
        return;

    KoDataCenter * dataCenter = m_canvas->shapeController()->dataCenter( "ImageCollection" );
    KoImageCollection * imageCollection = dynamic_cast<KoImageCollection*>( dataCenter );
    if( imageCollection )
    {
        KoPatternBackground * fill = new KoPatternBackground( imageCollection );
        fill->setPattern( currentPattern->pattern()->img() );
        m_canvas->addCommand( new KoShapeBackgroundCommand( selectedShapes, fill  ) );
        selectionChanged();
    }
}

void KarbonStyleDocker::updateFillRule( Qt::FillRule fillRule )
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

#include "KarbonStyleDocker.moc"
