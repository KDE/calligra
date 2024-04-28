/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006, 2009, 2010 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2006, 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KoShapeGroupCommandPrivate_H
#define KoShapeGroupCommandPrivate_H

#include <QList>
#include <QPair>

class KoShape;
class KoShapeContainer;
class KUndo2Command;
class QRectF;

class KoShapeGroupCommandPrivate
{
public:
    KoShapeGroupCommandPrivate(KoShapeContainer *container,
                               const QList<KoShape *> &shapes,
                               const QList<bool> &clipped = QList<bool>(),
                               const QList<bool> &inheritTransform = QList<bool>());
    void init(KUndo2Command *q);
    QRectF containerBoundingRect();

    QList<KoShape *> shapes; ///< list of shapes to be grouped
    QList<bool> clipped; ///< list of booleans to specify the shape of the same index to be clipped
    QList<bool> inheritTransform; ///< list of booleans to specify the shape of the same index to inherit transform
    KoShapeContainer *container; ///< the container where the grouping should be for.
    QList<KoShapeContainer *> oldParents; ///< the old parents of the shapes
    QList<bool> oldClipped; ///< if the shape was clipped in the old parent
    QList<bool> oldInheritTransform; ///< if the shape was inheriting transform in the old parent
    QList<int> oldZIndex; ///< the old z-index of the shapes

    QList<QPair<KoShape *, int>> oldAncestorsZIndex; // only used by the ungroup command
};

#endif
