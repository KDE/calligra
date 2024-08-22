/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoShapeShadowCommand.h"
#include "KoShape.h"
#include "KoShapeShadow.h"

#include <KLocalizedString>

class Q_DECL_HIDDEN KoShapeShadowCommand::Private
{
public:
    Private() = default;
    ~Private()
    {
        foreach (KoShapeShadow *shadow, oldShadows) {
            if (shadow && !shadow->deref())
                delete shadow;
        }
    }

    void addOldShadow(KoShapeShadow *oldShadow)
    {
        if (oldShadow)
            oldShadow->ref();
        oldShadows.append(oldShadow);
    }

    void addNewShadow(KoShapeShadow *newShadow)
    {
        if (newShadow)
            newShadow->ref();
        newShadows.append(newShadow);
    }

    QList<KoShape *> shapes; ///< the shapes to set shadow for
    QList<KoShapeShadow *> oldShadows; ///< the old shadows, one for each shape
    QList<KoShapeShadow *> newShadows; ///< the new shadows to set
};

KoShapeShadowCommand::KoShapeShadowCommand(const QList<KoShape *> &shapes, KoShapeShadow *shadow, KUndo2Command *parent)
    : KUndo2Command(parent)
    , d(new Private())
{
    d->shapes = shapes;
    // save old shadows
    foreach (KoShape *shape, d->shapes) {
        d->addOldShadow(shape->shadow());
        d->addNewShadow(shadow);
    }

    setText(kundo2_i18n("Set Shadow"));
}

KoShapeShadowCommand::KoShapeShadowCommand(const QList<KoShape *> &shapes, const QList<KoShapeShadow *> &shadows, KUndo2Command *parent)
    : KUndo2Command(parent)
    , d(new Private())
{
    Q_ASSERT(shapes.count() == shadows.count());

    d->shapes = shapes;

    // save old shadows
    foreach (KoShape *shape, shapes)
        d->addOldShadow(shape->shadow());
    foreach (KoShapeShadow *shadow, shadows)
        d->addNewShadow(shadow);

    setText(kundo2_i18n("Set Shadow"));
}

KoShapeShadowCommand::KoShapeShadowCommand(KoShape *shape, KoShapeShadow *shadow, KUndo2Command *parent)
    : KUndo2Command(parent)
    , d(new Private())
{
    d->shapes.append(shape);
    d->addNewShadow(shadow);
    d->addOldShadow(shape->shadow());

    setText(kundo2_i18n("Set Shadow"));
}

KoShapeShadowCommand::~KoShapeShadowCommand()
{
    delete d;
}

void KoShapeShadowCommand::redo()
{
    KUndo2Command::redo();
    int shapeCount = d->shapes.count();
    for (int i = 0; i < shapeCount; ++i) {
        KoShape *shape = d->shapes[i];
        shape->update();
        shape->setShadow(d->newShadows[i]);
        shape->update();
    }
}

void KoShapeShadowCommand::undo()
{
    KUndo2Command::undo();
    int shapeCount = d->shapes.count();
    for (int i = 0; i < shapeCount; ++i) {
        KoShape *shape = d->shapes[i];
        shape->update();
        shape->setShadow(d->oldShadows[i]);
        shape->update();
    }
}
