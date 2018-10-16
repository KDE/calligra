/* This file is part of the KDE project
 * Copyright (C) 2006 Thomas Zander <zander@kde.org>
 * Copyright (C) 2006 Jan Hambrecht <jaham@gmx.net>
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

#include "KoShapeMoveCommand.h"

#include <KoShape.h>
#include <KoShapeContainer.h>
#include <KoShapeContainerModel.h>
#include <KoShapeAnchor.h>

#include <klocalizedstring.h>

class Q_DECL_HIDDEN KoShapeMoveCommand::Private
{
public:
    QList<KoShape*> shapes;
    QVector<QPointF> previousPositions, newPositions, previousOffsets, newOffsets;
};

KoShapeMoveCommand::KoShapeMoveCommand(const QList<KoShape*> &shapes,
                                       const QVector<QPointF> &previousPositions, const QVector<QPointF> &newPositions,
                                       KUndo2Command *parent)
    : KUndo2Command(parent),
    d(new Private())
{
    d->shapes = shapes;
    d->previousPositions = previousPositions;
    d->newPositions = newPositions;
    Q_ASSERT(d->shapes.count() == d->previousPositions.count());
    Q_ASSERT(d->shapes.count() == d->newPositions.count());

    setText(kundo2_i18n("Move shapes"));
}

KoShapeMoveCommand::KoShapeMoveCommand(const QList<KoShape*> &shapes,
                                       const QVector<QPointF> &previousPositions, const QVector<QPointF> &newPositions,
                                       const QVector<QPointF> &previousOffsets, const QVector<QPointF> &newOffsets,
                                       KUndo2Command *parent)
        : KUndo2Command(parent),
        d(new Private())
{
    d->shapes = shapes;
    d->previousPositions = previousPositions;
    d->newPositions = newPositions;
    d->previousOffsets = previousOffsets;
    d->newOffsets = newOffsets;
    Q_ASSERT(d->shapes.count() == d->previousPositions.count());
    Q_ASSERT(d->shapes.count() == d->newPositions.count());

    setText(kundo2_i18n("Move shapes"));
}

KoShapeMoveCommand::~KoShapeMoveCommand()
{
    delete d;
}

void KoShapeMoveCommand::redo()
{
    KUndo2Command::redo();
    for (int i = 0; i < d->shapes.count(); i++) {
        d->shapes.at(i)->update();
        if (d->shapes.at(i)->anchor() && !d->newOffsets.isEmpty()) {
            d->shapes.at(i)->anchor()->setOffset(d->newOffsets.at(i));
        }
        d->shapes.at(i)->setPosition(d->newPositions.at(i));
        d->shapes.at(i)->update();
    }
}

void KoShapeMoveCommand::undo()
{
    KUndo2Command::undo();
    for (int i = 0; i < d->shapes.count(); i++) {
        d->shapes.at(i)->update();
        if (d->shapes.at(i)->anchor() && !d->previousOffsets.isEmpty()) {
            d->shapes.at(i)->anchor()->setOffset(d->previousOffsets.at(i));
        }
        d->shapes.at(i)->setPosition(d->previousPositions.at(i));
        d->shapes.at(i)->update();
    }
}

/// update newPositions list with new positions.
void KoShapeMoveCommand::setNewPositions(const QVector<QPointF> &newPositions)
{
    d->newPositions = newPositions;
}
