/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoShapeClipCommand.h"
#include "KoClipPath.h"
#include "KoPathShape.h"
#include "KoShape.h"
#include "KoShapeBasedDocumentBase.h"
#include "KoShapeContainer.h"

#include <KLocalizedString>

class Q_DECL_HIDDEN KoShapeClipCommand::Private
{
public:
    Private(KoShapeBasedDocumentBase *c)
        : controller(c)
        , executed(false)
    {
    }

    ~Private()
    {
        if (executed) {
            qDeleteAll(oldClipPaths);
        } else {
            clipData->removeClipShapesOwnership();
            qDeleteAll(newClipPaths);
        }
    }

    QList<KoShape *> shapesToClip;
    QList<KoClipPath *> oldClipPaths;
    QList<KoPathShape *> clipPathShapes;
    QList<KoClipPath *> newClipPaths;
    QList<KoShapeContainer *> oldParents;
    QExplicitlySharedDataPointer<KoClipData> clipData;
    KoShapeBasedDocumentBase *controller;
    bool executed;
};

KoShapeClipCommand::KoShapeClipCommand(KoShapeBasedDocumentBase *controller,
                                       const QList<KoShape *> &shapes,
                                       const QList<KoPathShape *> &clipPathShapes,
                                       KUndo2Command *parent)
    : KUndo2Command(parent)
    , d(new Private(controller))
{
    d->shapesToClip = shapes;
    d->clipPathShapes = clipPathShapes;
    d->clipData = new KoClipData(clipPathShapes);
    foreach (KoShape *shape, d->shapesToClip) {
        d->oldClipPaths.append(shape->clipPath());
        d->newClipPaths.append(new KoClipPath(shape, d->clipData.data()));
    }

    foreach (KoPathShape *path, clipPathShapes) {
        d->oldParents.append(path->parent());
    }

    setText(kundo2_i18n("Clip Shape"));
}

KoShapeClipCommand::KoShapeClipCommand(KoShapeBasedDocumentBase *controller, KoShape *shape, const QList<KoPathShape *> &clipPathShapes, KUndo2Command *parent)
    : KUndo2Command(parent)
    , d(new Private(controller))
{
    d->shapesToClip.append(shape);
    d->clipPathShapes = clipPathShapes;
    d->clipData = new KoClipData(clipPathShapes);
    d->oldClipPaths.append(shape->clipPath());
    d->newClipPaths.append(new KoClipPath(shape, d->clipData.data()));

    foreach (KoPathShape *path, clipPathShapes) {
        d->oldParents.append(path->parent());
    }

    setText(kundo2_i18n("Clip Shape"));
}

KoShapeClipCommand::~KoShapeClipCommand()
{
    delete d;
}

void KoShapeClipCommand::redo()
{
    const uint shapeCount = d->shapesToClip.count();
    for (uint i = 0; i < shapeCount; ++i) {
        d->shapesToClip[i]->setClipPath(d->newClipPaths[i]);
        d->shapesToClip[i]->update();
    }

    const uint clipPathCount = d->clipPathShapes.count();
    for (uint i = 0; i < clipPathCount; ++i) {
        d->controller->removeShape(d->clipPathShapes[i]);
        if (d->oldParents.at(i))
            d->oldParents.at(i)->removeShape(d->clipPathShapes[i]);
    }

    d->executed = true;

    KUndo2Command::redo();
}

void KoShapeClipCommand::undo()
{
    KUndo2Command::undo();

    const uint shapeCount = d->shapesToClip.count();
    for (uint i = 0; i < shapeCount; ++i) {
        d->shapesToClip[i]->setClipPath(d->oldClipPaths[i]);
        d->shapesToClip[i]->update();
    }

    const uint clipPathCount = d->clipPathShapes.count();
    for (uint i = 0; i < clipPathCount; ++i) {
        if (d->oldParents.at(i))
            d->oldParents.at(i)->addShape(d->clipPathShapes[i]);
        // the parent has to be there when it is added to the KoShapeBasedDocumentBase
        d->controller->addShape(d->clipPathShapes[i]);
    }

    d->executed = false;
}
