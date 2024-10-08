/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoShapeReorderCommand.h"
#include "KoShape.h"
#include "KoShapeContainer.h"
#include "KoShapeManager.h"
#include "KoShape_p.h"

#include <FlakeDebug.h>
#include <KLocalizedString>
#include <algorithm>
#include <limits.h>

class KoShapeReorderCommandPrivate
{
public:
    KoShapeReorderCommandPrivate(const QList<KoShape *> &s, QList<int> &ni)
        : shapes(s)
        , newIndexes(ni)
    {
    }

    QList<KoShape *> shapes;
    QList<int> previousIndexes;
    QList<int> newIndexes;
};

KoShapeReorderCommand::KoShapeReorderCommand(const QList<KoShape *> &shapes, QList<int> &newIndexes, KUndo2Command *parent)
    : KUndo2Command(parent)
    , d(new KoShapeReorderCommandPrivate(shapes, newIndexes))
{
    Q_ASSERT(shapes.count() == newIndexes.count());
    foreach (KoShape *shape, shapes)
        d->previousIndexes.append(shape->zIndex());

    setText(kundo2_i18n("Reorder shapes"));
}

KoShapeReorderCommand::~KoShapeReorderCommand()
{
    delete d;
}

void KoShapeReorderCommand::redo()
{
    KUndo2Command::redo();
    for (int i = 0; i < d->shapes.count(); i++) {
        d->shapes.at(i)->update();
        d->shapes.at(i)->setZIndex(d->newIndexes.at(i));
        d->shapes.at(i)->update();
    }
}

void KoShapeReorderCommand::undo()
{
    KUndo2Command::undo();
    for (int i = 0; i < d->shapes.count(); i++) {
        d->shapes.at(i)->update();
        d->shapes.at(i)->setZIndex(d->previousIndexes.at(i));
        d->shapes.at(i)->update();
    }
}

static void prepare(KoShape *s, QHash<KoShape *, QList<KoShape *>> &newOrder, KoShapeManager *manager, KoShapeReorderCommand::MoveShapeType move)
{
    KoShapeContainer *parent = s->parent();
    QHash<KoShape *, QList<KoShape *>>::iterator it(newOrder.find(parent));
    if (it == newOrder.end()) {
        QList<KoShape *> children;
        if (parent != nullptr) {
            children = parent->shapes();
        } else {
            // get all toplevel shapes
            children = manager->topLevelShapes();
        }
        std::sort(children.begin(), children.end(), KoShape::compareShapeZIndex);
        // the append and prepend are needed so that the raise/lower of all shapes works as expected.
        children.append(nullptr);
        children.prepend(nullptr);
        it = newOrder.insert(parent, children);
    }
    QList<KoShape *> &shapes(newOrder[parent]);
    int index = shapes.indexOf(s);
    if (index != -1) {
        shapes.removeAt(index);
        switch (move) {
        case KoShapeReorderCommand::BringToFront:
            index = shapes.size();
            break;
        case KoShapeReorderCommand::RaiseShape:
            if (index < shapes.size()) {
                ++index;
            }
            break;
        case KoShapeReorderCommand::LowerShape:
            if (index > 0) {
                --index;
            }
            break;
        case KoShapeReorderCommand::SendToBack:
            index = 0;
            break;
        }
        shapes.insert(index, s);
    }
}

// static
KoShapeReorderCommand *KoShapeReorderCommand::createCommand(const QList<KoShape *> &shapes, KoShapeManager *manager, MoveShapeType move, KUndo2Command *parent)
{
    QList<int> newIndexes;
    QList<KoShape *> changedShapes;
    QHash<KoShape *, QList<KoShape *>> newOrder;
    QList<KoShape *> sortedShapes(shapes);
    std::sort(sortedShapes.begin(), sortedShapes.end(), KoShape::compareShapeZIndex);
    if (move == BringToFront || move == LowerShape) {
        for (int i = 0; i < sortedShapes.size(); ++i) {
            prepare(sortedShapes.at(i), newOrder, manager, move);
        }
    } else {
        for (int i = sortedShapes.size() - 1; i >= 0; --i) {
            prepare(sortedShapes.at(i), newOrder, manager, move);
        }
    }

    QHash<KoShape *, QList<KoShape *>>::ConstIterator newIt(newOrder.constBegin());
    for (; newIt != newOrder.constEnd(); ++newIt) {
        QList<KoShape *> order(newIt.value());
        order.removeAll(nullptr);
        int index = -KoShapePrivate::MaxZIndex - 1; // set minimum zIndex
        int pos = 0;
        for (; pos < order.size(); ++pos) {
            if (order[pos]->zIndex() > index) {
                index = order[pos]->zIndex();
            } else {
                break;
            }
        }

        if (pos == order.size()) {
            // nothing needs to be done
            continue;
        } else if (pos <= order.size() / 2) {
            // new index for the front
            int startIndex = order[pos]->zIndex() - pos;
            for (int i = 0; i < pos; ++i) {
                changedShapes.append(order[i]);
                newIndexes.append(startIndex++);
            }
        } else {
            // new index for the end
            for (int i = pos; i < order.size(); ++i) {
                changedShapes.append(order[i]);
                newIndexes.append(++index);
            }
        }
    }
    Q_ASSERT(changedShapes.count() == newIndexes.count());
    return changedShapes.isEmpty() ? nullptr : new KoShapeReorderCommand(changedShapes, newIndexes, parent);
}
