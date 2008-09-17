/* This file is part of the KDE project
 * Copyright (C) 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (  at your option ) any later version.
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

#include "KPrPlaceholderTool.h"

#include <QUndoCommand>
#include <klocale.h>

#include <KoShapeRegistry.h>
#include <KoShapeFactory.h>
#include <KoCanvasBase.h>
#include <KoShapeManager.h>
#include <KoShapeController.h>
#include <KoSelection.h>
#include <KoToolManager.h>

#include "KPrPlaceholderShape.h"

KPrPlaceholderTool::KPrPlaceholderTool( KoCanvasBase *canvas )
: KoTool( canvas )
{
}

KPrPlaceholderTool::~KPrPlaceholderTool()
{
}

void KPrPlaceholderTool::paint( QPainter &painter, const KoViewConverter &converter )
{
    Q_UNUSED( painter );
    Q_UNUSED( converter );
}

void KPrPlaceholderTool::mousePressEvent( KoPointerEvent *event )
{
    Q_UNUSED( event );
}

void KPrPlaceholderTool::mouseMoveEvent( KoPointerEvent *event )
{
    Q_UNUSED( event );
}

void KPrPlaceholderTool::mouseReleaseEvent( KoPointerEvent *event )
{
    Q_UNUSED( event );
}

void KPrPlaceholderTool::activate( bool temporary )
{
    QList<KPrPlaceholderShape *> selectedShapes;

    foreach( KoShape *shape, m_canvas->shapeManager()->selection()->selectedShapes() ) {
        if ( KPrPlaceholderShape * ps = dynamic_cast<KPrPlaceholderShape*>( shape ) ) {
            selectedShapes.append( ps );
        }
    }

    if ( selectedShapes.isEmpty() ) {
        emit done();
        return;
    }

    QUndoCommand *cmd = new QUndoCommand( i18n( "Edit Shape" ) );

    KoShape * shape = selectedShapes.at( 0 );

    KoShapeFactory *factory = KoShapeRegistry::instance()->value( "TextShapeID" );
    Q_ASSERT( factory );
    // Create a TextShape
    // TODO get the KoShapeControllerBase to add it here
    KoShape * newShape = factory->createDefaultShapeAndInit( 0 );
    Q_ASSERT( newShape );

    // copy settings from placeholder shape
    newShape->setParent( shape->parent() );
    newShape->setZIndex( shape->zIndex() );
    newShape->setSize( shape->size() );
    newShape->setPosition( shape->position() );
    newShape->setAdditionalAttribute( "presentation:class", shape->additionalAttribute( "presentation:class" ) );

    // replace placeholder by shape
    m_canvas->shapeController()->removeShape( shape, cmd );
    m_canvas->shapeController()->addShape( newShape, cmd );
    m_canvas->addCommand( cmd );

    // activate the correct tool for the shape
    QList<KoShape *> shapes;
    shapes.append( newShape );
    m_canvas->shapeManager()->selection()->select( newShape );
    activateTool( KoToolManager::instance()->preferredToolForSelection( shapes ) );
}
