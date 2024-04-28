/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2006 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoShapeUngroupCommand.h"
#include "KoShapeContainer.h"
#include "KoShapeGroupCommand_p.h"

#include <KLocalizedString>

#include <algorithm>

KoShapeUngroupCommand::KoShapeUngroupCommand(KoShapeContainer *container,
                                             const QList<KoShape *> &shapes,
                                             const QList<KoShape *> &topLevelShapes,
                                             KUndo2Command *parent)
    : KoShapeGroupCommand(*(new KoShapeGroupCommandPrivate(container, shapes)), parent)
{
    QList<KoShape *> orderdShapes(shapes);
    std::sort(orderdShapes.begin(), orderdShapes.end(), KoShape::compareShapeZIndex);
    d->shapes = orderdShapes;

    QList<KoShape *> ancestors = d->container->parent() ? d->container->parent()->shapes() : topLevelShapes;
    if (ancestors.count()) {
        std::sort(ancestors.begin(), ancestors.end(), KoShape::compareShapeZIndex);
        QList<KoShape *>::const_iterator it(std::find(ancestors.begin(), ancestors.end(), d->container));

        Q_ASSERT(it != ancestors.constEnd());
        for (; it != ancestors.constEnd(); ++it) {
            d->oldAncestorsZIndex.append(QPair<KoShape *, int>(*it, (*it)->zIndex()));
        }
    }

    int zIndex = d->container->zIndex();
    foreach (KoShape *shape, d->shapes) {
        d->clipped.append(d->container->isClipped(shape));
        d->oldParents.append(d->container->parent());
        d->oldClipped.append(d->container->isClipped(shape));
        d->oldInheritTransform.append(shape->parent() && shape->parent()->inheritsTransform(shape));
        d->inheritTransform.append(false);
        // TODO this might also need to change the children of the parent but that is very problematic if the parent is 0
        d->oldZIndex.append(zIndex++);
    }

    setText(kundo2_i18n("Ungroup shapes"));
}

void KoShapeUngroupCommand::redo()
{
    KoShapeGroupCommand::undo();
    if (d->oldAncestorsZIndex.count()) {
        int zIndex = d->container->zIndex() + d->oldZIndex.count() - 1;
        for (QList<QPair<KoShape *, int>>::const_iterator it(d->oldAncestorsZIndex.constBegin()); it != d->oldAncestorsZIndex.constEnd(); ++it) {
            it->first->setZIndex(zIndex++);
        }
    }
}

void KoShapeUngroupCommand::undo()
{
    KoShapeGroupCommand::redo();
    if (d->oldAncestorsZIndex.count()) {
        for (QList<QPair<KoShape *, int>>::const_iterator it(d->oldAncestorsZIndex.constBegin()); it != d->oldAncestorsZIndex.constEnd(); ++it) {
            it->first->setZIndex(it->second);
        }
    }
}
