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

#ifndef KOSHAPEMOVECOMMAND_H
#define KOSHAPEMOVECOMMAND_H

#include "flake_export.h"

#include <kundo2command.h>
#include <QVector>
#include <QPointF>

class KoShape;

/// The undo / redo command for shape moving.
class FLAKE_EXPORT KoShapeMoveCommand : public KUndo2Command
{
public:
    /**
     * Constructor.
     * @param shapes the set of objects that are moved.
     * @param previousPositions the known set of previous positions for each of the objects.
     *  this list naturally must have the same amount of items as the shapes set.
     * @param newPositions the new positions for the shapes.
     *  this list naturally must have the same amount of items as the shapes set.
     * @param parent the parent command used for macro commands
     */
    KoShapeMoveCommand(const QList<KoShape*> &shapes,
                       const QVector<QPointF> &previousPositions, const QVector<QPointF> &newPositions,
                       KUndo2Command *parent = 0);

    /**
     * Constructor.
     * @param shapes the set of objects that are moved.
     * @param previousPositions the known set of previous positions for each of the objects.
     *  This list naturally must have the same amount of items as the @p shapes.
     * @param newPositions the new positions for the shapes.
     *  This list naturally must have the same amount of items as the shapes set.
     * @param previousOffsets the old offsets for the shape anchors.
     *  This list naturally must have the same amount of items as the @p shapes.
     * @param newOffsets the new offsets for the shapes.
     *  This list naturally must have the same amount of items as the @p shapes.
     * @param parent the parent command used for macro commands
     */
    KoShapeMoveCommand(const QList<KoShape*> &shapes,
                       const QVector<QPointF> &previousPositions, const QVector<QPointF> &newPositions,
                       const QVector<QPointF> &previousOffsets, const QVector<QPointF> &newOffsets,
                       KUndo2Command *parent = 0);
    /// Destuctor.
    ~KoShapeMoveCommand() override;
    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

    /// update newPositions list with new positions.
    void setNewPositions(const QVector<QPointF> &newPositions);

private:
    class Private;
    Private * const d;
};

#endif
