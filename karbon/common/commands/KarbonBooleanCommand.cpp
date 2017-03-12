/* This file is part of the KDE project
 * Copyright (C) 2007,2010 Jan Hambrecht <jaham@gmx.net>
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
#include <KoShapeBasedDocumentBase.h>
#include <KoPathShape.h>
#include <KoShapeContainer.h>
#include <KoShapeGroup.h>
#include <KoShapeGroupCommand.h>

#include <klocalizedstring.h>

#include <QPainterPath>

class Q_DECL_HIDDEN KarbonBooleanCommand::Private
{
public:
    Private(KoShapeBasedDocumentBase * c)
            : shapeBasedDocument(c), pathA(0), pathB(0), resultingPath(0)
            , resultParent(0), resultParentCmd(0)
            , operation(Intersection), isExecuted(false)
    {}

    ~Private()
    {
        if (! isExecuted)
            delete resultingPath;
    }

    KoShapeBasedDocumentBase *shapeBasedDocument;
    KoPathShape * pathA;
    KoPathShape * pathB;
    KoPathShape * resultingPath;
    KoShapeContainer * resultParent;
    KUndo2Command * resultParentCmd;
    BooleanOperation operation;
    bool isExecuted;
};

KarbonBooleanCommand::KarbonBooleanCommand(
    KoShapeBasedDocumentBase *shapeBasedDocument, KoPathShape* pathA, KoPathShape * pathB,
    BooleanOperation operation, KUndo2Command *parent
)
        : KUndo2Command(parent), d(new Private(shapeBasedDocument))
{
    Q_ASSERT(shapeBasedDocument);

    d->pathA = pathA;
    d->pathB = pathB;
    d->operation = operation;

    setText(kundo2_i18n("Boolean Operation"));
}

KarbonBooleanCommand::~KarbonBooleanCommand()
{
    delete d;
}

void KarbonBooleanCommand::redo()
{
    if (! d->resultingPath) {
        // transform input paths to global coordinates
        QPainterPath pa = d->pathA->absoluteTransformation(0).map(d->pathA->outline());
        QPainterPath pb = d->pathB->absoluteTransformation(0).map(d->pathB->outline());
        QPainterPath pr;
        switch (d->operation) {
        case Intersection:
            pr = pa.intersected(pb);
            break;
        case Subtraction:
            pr = pa.subtracted(pb);
            break;
        case Exclusion:
            pr = pa.subtracted(pb);
            pr.addPath(pb.subtracted(pa));
            break;
        case Union:
            pr = pa.united(pb);
            break;
        }

        QTransform transformationA = d->pathA->absoluteTransformation(0);
        // transform resulting path to local coordinate system of input path A
        pr = transformationA.inverted().map(pr);
        // create a path shape from the resulting path in local coordinates
        d->resultingPath = KoPathShape::createShapeFromPainterPath(pr);
        d->resultingPath->setStroke(d->pathA->stroke());
        d->resultingPath->setBackground(d->pathA->background());
        d->resultingPath->setShapeId(d->pathA->shapeId());
        // the created shape has a transformation applied so we have to
        // apply the original transformation instead of replacing with it
        d->resultingPath->applyAbsoluteTransformation(transformationA);
        d->resultingPath->setName(d->pathA->name());
        d->resultingPath->setZIndex(d->pathA->zIndex());
        d->resultingPath->setFillRule(d->pathA->fillRule());

        KoShapeGroup * group = dynamic_cast<KoShapeGroup*>(d->pathA->parent());
        if (group) {
            QList<KoShape*> children;
            d->resultParentCmd = new KoShapeGroupCommand(group, children << d->resultingPath, this);
        }
    }

    if (d->shapeBasedDocument) {
        if (d->resultParent)
            d->resultParent->addShape(d->resultingPath);
        d->shapeBasedDocument->addShape(d->resultingPath);
    }

    KUndo2Command::redo();

    d->isExecuted = true;
}

void KarbonBooleanCommand::undo()
{
    KUndo2Command::undo();

    if (d->shapeBasedDocument && d->resultingPath) {
        if (! d->resultParentCmd) {
            d->resultParent = d->resultingPath->parent();
            if (d->resultParent)
                d->resultParent->removeShape(d->resultingPath);
        }
        d->shapeBasedDocument->removeShape(d->resultingPath);
    }

    d->isExecuted = false;
}
