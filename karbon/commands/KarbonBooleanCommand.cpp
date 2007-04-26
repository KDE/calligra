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

#include "KarbonBooleanCommand.h"
#include <KoShapeControllerBase.h>
#include <KoPathShape.h>

#include <klocale.h>

#include <QtGui/QPainterPath>

class KarbonBooleanCommand::Private
{
public:
    Private( KoShapeControllerBase * c )
    : controller( c ), pathA(0), pathB(0), resultingPath(0)
    , operation( Intersection ), isExecuted(false)
    {}

    ~Private()
    {
        if( isExecuted )
            delete resultingPath;
    }

    KoShapeControllerBase *controller;
    KoPathShape * pathA;
    KoPathShape * pathB;
    KoPathShape * resultingPath;
    BooleanOperation operation;
    bool isExecuted;
};

KarbonBooleanCommand::KarbonBooleanCommand( KoShapeControllerBase *controller, QUndoCommand *parent )
    : QUndoCommand( parent ), d( new Private( controller ) )
{
    setText( i18n( "Boolean Operation" ) );
}

KarbonBooleanCommand::~KarbonBooleanCommand()
{
    delete d;
}

void KarbonBooleanCommand::setFirstOperand( KoPathShape* pathA )
{
    d->pathA = pathA;
}

void KarbonBooleanCommand::setSecondOperand( KoPathShape * pathB )
{
    d->pathB = pathB;
}

void KarbonBooleanCommand::setOperation( BooleanOperation operation )
{
    d->operation = operation;
}

void KarbonBooleanCommand::redo()
{
    QUndoCommand::redo();

    if( ! d->resultingPath )
    {
        QPainterPath pa = d->pathA->transformationMatrix(0).map( d->pathA->outline() );
        QPainterPath pb = d->pathB->transformationMatrix(0).map( d->pathB->outline() );
        QPainterPath pr;
        switch( d->operation )
        {
            case Intersection:
                pr = pa.intersected( pb );
                break;
            case Subtraction:
                pr = pa.subtracted( pb );
                break;
            case Union:
                pr = pa.united( pb );
        }

        d->resultingPath = shapeFromPath( pr );
        d->resultingPath->setBorder( d->pathA->border() );
        d->resultingPath->setShapeId( d->pathA->shapeId() );
    }

    if( d->controller )
    {
        d->controller->removeShape( d->pathA );
        d->controller->removeShape( d->pathB );
        d->controller->addShape( d->resultingPath );
    }

    d->isExecuted = true;
}

void KarbonBooleanCommand::undo()
{
    QUndoCommand::undo();

    if( d->controller && d->resultingPath )
    {
        d->controller->removeShape( d->resultingPath );
        d->controller->addShape( d->pathA );
        d->controller->addShape( d->pathB );
    }

    d->isExecuted = false;
}

KoPathShape * KarbonBooleanCommand::shapeFromPath( const QPainterPath &path )
{
    KoPathShape * shape = new KoPathShape();

    int elementCount = path.elementCount();
    for( int i = 0; i < elementCount; i++ )
    {
        QPainterPath::Element element = path.elementAt( i );
        switch( element.type )
        {
            case QPainterPath::MoveToElement:
                shape->moveTo( QPointF( element.x, element.y ) );
                break;
            case QPainterPath::LineToElement:
                shape->lineTo( QPointF( element.x, element.y ) );
                break;
            case QPainterPath::CurveToElement:
                shape->curveTo( QPointF( element.x, element.y ),
                                QPointF( path.elementAt(i+1).x, path.elementAt(i+1).y),
                                QPointF( path.elementAt(i+2).x, path.elementAt(i+2).y) );
                break;
            default:
                continue;
        }
    }

    shape->normalize();
    return shape;
}
