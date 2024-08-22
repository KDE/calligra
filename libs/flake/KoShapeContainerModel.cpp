/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoShapeContainerModel.h"

#include "KoShapeContainer.h"

KoShapeContainerModel::KoShapeContainerModel() = default;

KoShapeContainerModel::~KoShapeContainerModel() = default;

void KoShapeContainerModel::proposeMove(KoShape *child, QPointF &move)
{
    Q_UNUSED(child);
    Q_UNUSED(move);
}

void KoShapeContainerModel::childChanged(KoShape *child, KoShape::ChangeType type)
{
    Q_UNUSED(type);
    if (type != KoShape::CollisionDetected) {
        KoShapeContainer *parent = child->parent();
        Q_ASSERT(parent);
        // propagate the change up the hierarchy
        KoShapeContainer *grandparent = parent->parent();
        if (grandparent) {
            grandparent->model()->childChanged(parent, KoShape::ChildChanged);
        }
    }
}

KoShape::AllowedInteractions KoShapeContainerModel::allowedInteractions(const KoShape *child) const
{
    KoShape::AllowedInteractions state = child->allowedInteractions(false);
    if (child->parent()) {
        state &= child->parent()->allowedInteractions();
    }
    return state;
}
