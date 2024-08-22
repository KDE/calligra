/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoShapeTransparencyCommand.h"
#include "KoShape.h"

#include <KLocalizedString>

class Q_DECL_HIDDEN KoShapeTransparencyCommand::Private
{
public:
    Private() = default;
    ~Private() = default;

    QList<KoShape *> shapes; ///< the shapes to set background for
    QList<qreal> oldTransparencies; ///< the old transparencies
    QList<qreal> newTransparencies; ///< the new transparencies
};

KoShapeTransparencyCommand::KoShapeTransparencyCommand(const QList<KoShape *> &shapes, qreal transparency, KUndo2Command *parent)
    : KUndo2Command(parent)
    , d(new Private())
{
    d->shapes = shapes;
    foreach (KoShape *shape, d->shapes) {
        d->oldTransparencies.append(shape->transparency());
        d->newTransparencies.append(transparency);
    }

    setText(kundo2_i18n("Set opacity"));
}

KoShapeTransparencyCommand::KoShapeTransparencyCommand(KoShape *shape, qreal transparency, KUndo2Command *parent)
    : KUndo2Command(parent)
    , d(new Private())
{
    d->shapes.append(shape);
    d->oldTransparencies.append(shape->transparency());
    d->newTransparencies.append(transparency);

    setText(kundo2_i18n("Set opacity"));
}

KoShapeTransparencyCommand::KoShapeTransparencyCommand(const QList<KoShape *> &shapes, const QList<qreal> &transparencies, KUndo2Command *parent)
    : KUndo2Command(parent)
    , d(new Private())
{
    d->shapes = shapes;
    foreach (KoShape *shape, d->shapes) {
        d->oldTransparencies.append(shape->transparency());
    }
    d->newTransparencies = transparencies;

    setText(kundo2_i18n("Set opacity"));
}

KoShapeTransparencyCommand::~KoShapeTransparencyCommand()
{
    delete d;
}

void KoShapeTransparencyCommand::redo()
{
    KUndo2Command::redo();
    QList<qreal>::ConstIterator transparencyIt = d->newTransparencies.constBegin();
    foreach (KoShape *shape, d->shapes) {
        shape->setTransparency(*transparencyIt);
        shape->update();
        ++transparencyIt;
    }
}

void KoShapeTransparencyCommand::undo()
{
    KUndo2Command::undo();
    QList<qreal>::ConstIterator transparencyIt = d->oldTransparencies.constBegin();
    foreach (KoShape *shape, d->shapes) {
        shape->setTransparency(*transparencyIt);
        shape->update();
        ++transparencyIt;
    }
}
