/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2006 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoShapeDeleteCommand.h"
#include "KoShapeBasedDocumentBase.h"
#include "KoShapeContainer.h"

#include <KLocalizedString>

class Q_DECL_HIDDEN KoShapeDeleteCommand::Private
{
public:
    Private(KoShapeBasedDocumentBase *c)
        : controller(c)
        , deleteShapes(false)
    {
    }

    ~Private()
    {
        if (!deleteShapes)
            return;

        qDeleteAll(shapes);
    }

    KoShapeBasedDocumentBase *controller; ///< the shape controller to use for removing/readding
    QList<KoShape *> shapes; ///< the list of shapes to delete
    QList<KoShapeContainer *> oldParents; ///< the old parents of the shapes
    bool deleteShapes; ///< shows if shapes should be deleted when deleting the command
};

KoShapeDeleteCommand::KoShapeDeleteCommand(KoShapeBasedDocumentBase *controller, KoShape *shape, KUndo2Command *parent)
    : KUndo2Command(parent)
    , d(new Private(controller))
{
    d->shapes.append(shape);
    d->oldParents.append(shape->parent());

    setText(kundo2_i18nc("Delete one shape", "Delete shape"));
}

KoShapeDeleteCommand::KoShapeDeleteCommand(KoShapeBasedDocumentBase *controller, const QList<KoShape *> &shapes, KUndo2Command *parent)
    : KUndo2Command(parent)
    , d(new Private(controller))
{
    d->shapes = shapes;
    for (KoShape *shape : std::as_const(d->shapes)) {
        d->oldParents.append(shape->parent());
    }

    setText(kundo2_i18np("Delete shape", "Delete shapes", shapes.count()));
}

KoShapeDeleteCommand::~KoShapeDeleteCommand()
{
    delete d;
}

void KoShapeDeleteCommand::redo()
{
    KUndo2Command::redo();
    if (!d->controller)
        return;

    for (int i = 0; i < d->shapes.count(); i++) {
        // the parent has to be there when it is removed from the KoShapeBasedDocumentBase
        d->controller->removeShape(d->shapes[i]);
        if (d->oldParents.at(i))
            d->oldParents.at(i)->removeShape(d->shapes[i]);
    }
    d->deleteShapes = true;
}

void KoShapeDeleteCommand::undo()
{
    KUndo2Command::undo();
    if (!d->controller)
        return;

    for (int i = 0; i < d->shapes.count(); i++) {
        if (d->oldParents.at(i))
            d->oldParents.at(i)->addShape(d->shapes[i]);
        // the parent has to be there when it is added to the KoShapeBasedDocumentBase
        d->controller->addShape(d->shapes[i]);
    }
    d->deleteShapes = false;
}
