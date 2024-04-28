/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoShapeTransformCommand.h"
#include "KoShape.h"

#include <QList>
#include <QTransform>

#include <FlakeDebug.h>

class Q_DECL_HIDDEN KoShapeTransformCommand::Private
{
public:
    Private(const QList<KoShape *> &list)
        : shapes(list)
    {
    }
    QList<KoShape *> shapes;
    QVector<QTransform> oldState;
    QVector<QTransform> newState;
};

KoShapeTransformCommand::KoShapeTransformCommand(const QList<KoShape *> &shapes,
                                                 const QVector<QTransform> &oldState,
                                                 const QVector<QTransform> &newState,
                                                 KUndo2Command *parent)
    : KUndo2Command(parent)
    , d(new Private(shapes))
{
    Q_ASSERT(shapes.count() == oldState.count());
    Q_ASSERT(shapes.count() == newState.count());
    d->oldState = oldState;
    d->newState = newState;
}

KoShapeTransformCommand::~KoShapeTransformCommand()
{
    delete d;
}

void KoShapeTransformCommand::redo()
{
    KUndo2Command::redo();

    const int shapeCount = d->shapes.count();
    for (int i = 0; i < shapeCount; ++i) {
        KoShape *shape = d->shapes[i];
        shape->update();
        shape->setTransformation(d->newState[i]);
        shape->update();
    }
}

void KoShapeTransformCommand::undo()
{
    KUndo2Command::undo();

    const int shapeCount = d->shapes.count();
    for (int i = 0; i < shapeCount; ++i) {
        KoShape *shape = d->shapes[i];
        shape->update();
        shape->setTransformation(d->oldState[i]);
        shape->update();
    }
}
