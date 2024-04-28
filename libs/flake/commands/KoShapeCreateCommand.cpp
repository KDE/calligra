/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2006 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoShapeCreateCommand.h"
#include "KoShape.h"
#include "KoShapeBasedDocumentBase.h"
#include "KoShapeContainer.h"

#include <KLocalizedString>

class Q_DECL_HIDDEN KoShapeCreateCommand::Private
{
public:
    Private(KoShapeBasedDocumentBase *c, KoShape *s)
        : controller(c)
        , shape(s)
        , shapeParent(shape->parent())
        , deleteShape(true)
    {
    }
    ~Private()
    {
        if (shape && deleteShape)
            delete shape;
    }

    KoShapeBasedDocumentBase *controller;
    KoShape *shape;
    KoShapeContainer *shapeParent;
    bool deleteShape;
};

KoShapeCreateCommand::KoShapeCreateCommand(KoShapeBasedDocumentBase *controller, KoShape *shape, KUndo2Command *parent)
    : KUndo2Command(parent)
    , d(new Private(controller, shape))
{
    setText(kundo2_i18n("Create shape"));
}

KoShapeCreateCommand::~KoShapeCreateCommand()
{
    delete d;
}

void KoShapeCreateCommand::redo()
{
    KUndo2Command::redo();
    Q_ASSERT(d->shape);
    Q_ASSERT(d->controller);
    if (d->shapeParent)
        d->shapeParent->addShape(d->shape);
    // the parent has to be there when it is added to the KoShapeBasedDocumentBase
    d->controller->addShape(d->shape);
    d->shapeParent = d->shape->parent(); // update parent if the 'addShape' changed it
    d->deleteShape = false;
}

void KoShapeCreateCommand::undo()
{
    KUndo2Command::undo();
    Q_ASSERT(d->shape);
    Q_ASSERT(d->controller);
    // the parent has to be there when it is removed from the KoShapeBasedDocumentBase
    d->controller->removeShape(d->shape);
    if (d->shapeParent)
        d->shapeParent->removeShape(d->shape);
    d->deleteShape = true;
}
