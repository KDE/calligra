/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2006-2008 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoShapeStrokeCommand.h"
#include "KoShape.h"
#include "KoShapeStrokeModel.h"

#include <KLocalizedString>

class Q_DECL_HIDDEN KoShapeStrokeCommand::Private
{
public:
    Private() = default;
    ~Private()
    {
        foreach (KoShapeStrokeModel *stroke, oldStrokes) {
            if (stroke && !stroke->deref())
                delete stroke;
        }
    }

    void addOldStroke(KoShapeStrokeModel *oldStroke)
    {
        if (oldStroke)
            oldStroke->ref();
        oldStrokes.append(oldStroke);
    }

    void addNewStroke(KoShapeStrokeModel *newStroke)
    {
        if (newStroke)
            newStroke->ref();
        newStrokes.append(newStroke);
    }

    QList<KoShape *> shapes; ///< the shapes to set stroke for
    QList<KoShapeStrokeModel *> oldStrokes; ///< the old strokes, one for each shape
    QList<KoShapeStrokeModel *> newStrokes; ///< the new strokes to set
};

KoShapeStrokeCommand::KoShapeStrokeCommand(const QList<KoShape *> &shapes, KoShapeStrokeModel *stroke, KUndo2Command *parent)
    : KUndo2Command(parent)
    , d(new Private())
{
    d->shapes = shapes;

    // save old strokes
    foreach (KoShape *shape, d->shapes) {
        d->addOldStroke(shape->stroke());
        d->addNewStroke(stroke);
    }

    setText(kundo2_i18n("Set stroke"));
}

KoShapeStrokeCommand::KoShapeStrokeCommand(const QList<KoShape *> &shapes, const QList<KoShapeStrokeModel *> &strokes, KUndo2Command *parent)
    : KUndo2Command(parent)
    , d(new Private())
{
    Q_ASSERT(shapes.count() == strokes.count());

    d->shapes = shapes;

    // save old strokes
    foreach (KoShape *shape, shapes)
        d->addOldStroke(shape->stroke());
    foreach (KoShapeStrokeModel *stroke, strokes)
        d->addNewStroke(stroke);

    setText(kundo2_i18n("Set stroke"));
}

KoShapeStrokeCommand::KoShapeStrokeCommand(KoShape *shape, KoShapeStrokeModel *stroke, KUndo2Command *parent)
    : KUndo2Command(parent)
    , d(new Private())
{
    d->shapes.append(shape);
    d->addNewStroke(stroke);
    d->addOldStroke(shape->stroke());

    setText(kundo2_i18n("Set stroke"));
}

KoShapeStrokeCommand::~KoShapeStrokeCommand()
{
    delete d;
}

void KoShapeStrokeCommand::redo()
{
    KUndo2Command::redo();
    QList<KoShapeStrokeModel *>::ConstIterator strokeIt = d->newStrokes.constBegin();
    foreach (KoShape *shape, d->shapes) {
        shape->update();
        shape->setStroke(*strokeIt);
        shape->update();
        ++strokeIt;
    }
}

void KoShapeStrokeCommand::undo()
{
    KUndo2Command::undo();
    QList<KoShapeStrokeModel *>::ConstIterator strokeIt = d->oldStrokes.constBegin();
    foreach (KoShape *shape, d->shapes) {
        shape->update();
        shape->setStroke(*strokeIt);
        shape->update();
        ++strokeIt;
    }
}
