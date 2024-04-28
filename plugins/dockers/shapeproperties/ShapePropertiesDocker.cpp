/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ShapePropertiesDocker.h"
#include <KoShape.h>
#include <KoPathShape.h>
#include <KoShapeConfigWidgetBase.h>
#include <KoShapeManager.h>
#include <KoShapeFactoryBase.h>
#include <KoShapeRegistry.h>
#include <KoCanvasBase.h>
#include <KoCanvasController.h>
#include <KoSelection.h>
#include <KoParameterShape.h>
#include <KoUnit.h>

#include <KLocalizedString>

#include <QStackedWidget>

class ShapePropertiesDocker::Private {
public:
    Private() : widgetStack(0), currentShape(0), currentPanel(0), canvas(0) {}
    QStackedWidget * widgetStack;
    KoShape * currentShape;
    KoShapeConfigWidgetBase * currentPanel;
    KoCanvasBase * canvas;
};


ShapePropertiesDocker::ShapePropertiesDocker(QWidget *parent)
    : QDockWidget(i18n("Shape Properties"), parent),
    d( new Private() )
{
    d->widgetStack = new QStackedWidget();
    setWidget(d->widgetStack);
}

ShapePropertiesDocker::~ShapePropertiesDocker()
{
    delete d;
}

void ShapePropertiesDocker::unsetCanvas()
{
    setEnabled(false);
    d->canvas = 0;
}

void ShapePropertiesDocker::setCanvas( KoCanvasBase *canvas )
{
    setEnabled(canvas != 0);

    if (d->canvas) {
        d->canvas->disconnectCanvasObserver(this); // "Every connection you make emits a signal, so duplicate connections emit two signals"
    }

    d->canvas = canvas;

    if( d->canvas )  {
        connect( d->canvas->shapeManager(), &KoShapeManager::selectionChanged,
            this, &ShapePropertiesDocker::selectionChanged );
        connect( d->canvas->shapeManager(), &KoShapeManager::selectionContentChanged,
            this, &ShapePropertiesDocker::selectionChanged );
        connect( d->canvas->resourceManager(), &KoCanvasResourceManager::canvasResourceChanged,
            this, &ShapePropertiesDocker::canvasResourceChanged );
    }
}

void ShapePropertiesDocker::selectionChanged()
{
    if( ! d->canvas )
        return;

    KoSelection *selection = d->canvas->shapeManager()->selection();
    if( selection->count() == 1 )
        addWidgetForShape( selection->firstSelectedShape() );
    else
        addWidgetForShape( 0 );
}

void ShapePropertiesDocker::addWidgetForShape( KoShape * shape )
{
    // remove the config widget if a null shape is set, or the shape has changed
    if( ! shape || shape != d->currentShape )
    {
        while( d->widgetStack->count() )
            d->widgetStack->removeWidget( d->widgetStack->widget( 0 ) );
    }

    if( ! shape )
    {
        d->currentShape = 0;
        d->currentPanel = 0;
        return;
    }
    else if( shape != d->currentShape )
    {
        // when a shape is set and is differs from the previous one
        // get the config widget and insert it into the option widget
        d->currentShape = shape;
        if( ! d->currentShape )
            return;
        QString shapeId = shape->shapeId();
        KoPathShape * path = dynamic_cast<KoPathShape*>( shape );
        if( path )
        {
            // use the path specific shape id if shape is a path, otherwise use the shape id
            shapeId = path->pathShapeId();
            // check if we have an edited parametric shape, then we use the path shape id
            KoParameterShape * paramShape = dynamic_cast<KoParameterShape*>( shape );
            if( paramShape && ! paramShape->isParametricShape() )
                shapeId = shape->shapeId();
        }
        KoShapeFactoryBase *factory = KoShapeRegistry::instance()->value( shapeId );
        if( ! factory )
            return;
        QList<KoShapeConfigWidgetBase*> panels = factory->createShapeOptionPanels();
        if( ! panels.count() )
            return;

        d->currentPanel = 0;
        uint panelCount = panels.count();
        for( uint i = 0; i < panelCount; ++i )
        {
            if( panels[i]->showOnShapeSelect() ) {
                d->currentPanel = panels[i];
                break;
            }
        }
        if( d->currentPanel )
        {
            if( d->canvas )
                d->currentPanel->setUnit( d->canvas->unit() );
            d->widgetStack->insertWidget( 0, d->currentPanel );
            connect( d->currentPanel, &KoShapeConfigWidgetBase::propertyChanged,
                     this, &ShapePropertiesDocker::shapePropertyChanged);
        }
    }

    if( d->currentPanel )
        d->currentPanel->open( shape );
}

void ShapePropertiesDocker::shapePropertyChanged()
{
    if( d->canvas && d->currentPanel )
    {
        KUndo2Command * cmd = d->currentPanel->createCommand();
        if( ! cmd )
            return;
        d->canvas->addCommand( cmd );
    }
}

void ShapePropertiesDocker::canvasResourceChanged(int key, const QVariant &variant)
{
    if (key == KoCanvasResourceManager::Unit && d->currentPanel)
        d->currentPanel->setUnit(variant.value<KoUnit>());
}
