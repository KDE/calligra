/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2006 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPEMOVECOMMAND_H
#define KOSHAPEMOVECOMMAND_H

#include "flake_export.h"

#include <QPointF>
#include <QVector>
#include <kundo2command.h>

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
    KoShapeMoveCommand(const QList<KoShape *> &shapes,
                       const QVector<QPointF> &previousPositions,
                       const QVector<QPointF> &newPositions,
                       KUndo2Command *parent = nullptr);

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
    KoShapeMoveCommand(const QList<KoShape *> &shapes,
                       const QVector<QPointF> &previousPositions,
                       const QVector<QPointF> &newPositions,
                       const QVector<QPointF> &previousOffsets,
                       const QVector<QPointF> &newOffsets,
                       KUndo2Command *parent = nullptr);
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
    Private *const d;
};

#endif
