/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoShapeTraversal.h"

#include <PageAppDebug.h>

#include <KoShape.h>
#include <KoShapeContainer.h>

#include <algorithm>

KoShape *KoShapeTraversal::nextShape(const KoShape *current)
{
    return nextShapeStep(current, nullptr);
}

KoShape *KoShapeTraversal::nextShape(const KoShape *current, const QString &shapeId)
{
    KoShape *next = nextShapeStep(current, nullptr);

    while (next != nullptr && next->shapeId() != shapeId) {
        next = nextShapeStep(next, nullptr);
    }

    return next;
}

KoShape *KoShapeTraversal::previousShape(const KoShape *current)
{
    return previousShapeStep(current, nullptr);
}

KoShape *KoShapeTraversal::previousShape(const KoShape *current, const QString &shapeId)
{
    KoShape *previous = previousShapeStep(current, nullptr);

    while (previous != nullptr && previous->shapeId() != shapeId) {
        previous = previousShapeStep(previous, nullptr);
    }

    return previous;
}

KoShape *KoShapeTraversal::last(KoShape *current)
{
    KoShape *last = current;
    while (const KoShapeContainer *container = dynamic_cast<const KoShapeContainer *>(last)) {
        QList<KoShape *> shapes = container->shapes();
        if (!shapes.isEmpty()) {
            last = shapes.last();
        } else {
            break;
        }
    }
    return last;
}

KoShape *KoShapeTraversal::nextShapeStep(const KoShape *current, const KoShapeContainer *parent)
{
    Q_ASSERT(current);
    if (!current) {
        return nullptr;
    }

    KoShape *next = nullptr;

    if (parent) {
        const QList<KoShape *> shapes = parent->shapes();
        QList<KoShape *>::const_iterator it(std::find(shapes.begin(), shapes.end(), current));
        Q_ASSERT(it != shapes.end());

        if (it == shapes.end()) {
            warnPageApp << "the shape is not in the list of children of his parent";
            return nullptr;
        }

        ++it;
        if (it != shapes.end()) {
            next = *it;
        } else {
            KoShapeContainer *currentParent = parent->parent();
            next = currentParent ? nextShapeStep(parent, currentParent) : nullptr;
        }
    } else {
        if (const KoShapeContainer *container = dynamic_cast<const KoShapeContainer *>(current)) {
            QList<KoShape *> shapes = container->shapes();
            if (!shapes.isEmpty()) {
                next = shapes[0];
            }
        }

        if (next == nullptr) {
            KoShapeContainer *currentParent = current->parent();
            next = currentParent ? nextShapeStep(current, currentParent) : nullptr;
        }
    }

    return next;
}

KoShape *KoShapeTraversal::previousShapeStep(const KoShape *current, const KoShapeContainer *parent)
{
    Q_ASSERT(current);
    if (!current) {
        return nullptr;
    }

    KoShape *previous = nullptr;

    if (parent) {
        if (previous == nullptr) {
            const QList<KoShape *> shapes = parent->shapes();
            QList<KoShape *>::const_iterator it(std::find(shapes.begin(), shapes.end(), current));
            Q_ASSERT(it != shapes.end());

            if (it == shapes.end()) {
                warnPageApp << "the shape is not in the list of children of his parent";
                return nullptr;
            }

            if (it != shapes.begin()) {
                --it;
                previous = last(*it);
            } else {
                previous = current->parent();
            }
        }
    } else {
        KoShapeContainer *currentParent = current->parent();
        previous = currentParent ? previousShapeStep(current, currentParent) : nullptr;
    }

    return previous;
}
