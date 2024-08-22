/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoShapeBackgroundCommand.h"
#include "KoShape.h"
#include "KoShapeBackground.h"

#include <KLocalizedString>

class Q_DECL_HIDDEN KoShapeBackgroundCommand::Private
{
public:
    Private() = default;
    ~Private()
    {
        oldFills.clear();
        newFills.clear();
    }

    void addOldFill(QSharedPointer<KoShapeBackground> oldFill)
    {
        oldFills.append(oldFill);
    }

    void addNewFill(QSharedPointer<KoShapeBackground> newFill)
    {
        newFills.append(newFill);
    }

    QList<KoShape *> shapes; ///< the shapes to set background for
    QList<QSharedPointer<KoShapeBackground>> oldFills;
    QList<QSharedPointer<KoShapeBackground>> newFills;
};

KoShapeBackgroundCommand::KoShapeBackgroundCommand(const QList<KoShape *> &shapes, QSharedPointer<KoShapeBackground> fill, KUndo2Command *parent)
    : KUndo2Command(parent)
    , d(new Private())
{
    d->shapes = shapes;
    foreach (KoShape *shape, d->shapes) {
        d->addOldFill(shape->background());
        d->addNewFill(fill);
    }

    setText(kundo2_i18n("Set background"));
}

KoShapeBackgroundCommand::KoShapeBackgroundCommand(KoShape *shape, QSharedPointer<KoShapeBackground> fill, KUndo2Command *parent)
    : KUndo2Command(parent)
    , d(new Private())
{
    d->shapes.append(shape);
    d->addOldFill(shape->background());
    d->addNewFill(fill);

    setText(kundo2_i18n("Set background"));
}

KoShapeBackgroundCommand::KoShapeBackgroundCommand(const QList<KoShape *> &shapes, const QList<QSharedPointer<KoShapeBackground>> &fills, KUndo2Command *parent)
    : KUndo2Command(parent)
    , d(new Private())
{
    d->shapes = shapes;
    foreach (KoShape *shape, d->shapes) {
        d->addOldFill(shape->background());
    }
    foreach (QSharedPointer<KoShapeBackground> fill, fills) {
        d->addNewFill(fill);
    }

    setText(kundo2_i18n("Set background"));
}

void KoShapeBackgroundCommand::redo()
{
    KUndo2Command::redo();
    QList<QSharedPointer<KoShapeBackground>>::ConstIterator brushIt = d->newFills.constBegin();
    foreach (KoShape *shape, d->shapes) {
        shape->setBackground(*brushIt);
        shape->update();
        ++brushIt;
    }
}

void KoShapeBackgroundCommand::undo()
{
    KUndo2Command::undo();
    QList<QSharedPointer<KoShapeBackground>>::ConstIterator brushIt = d->oldFills.constBegin();
    foreach (KoShape *shape, d->shapes) {
        shape->setBackground(*brushIt);
        shape->update();
        ++brushIt;
    }
}

KoShapeBackgroundCommand::~KoShapeBackgroundCommand()
{
    delete d;
}
