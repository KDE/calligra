/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPEREORDERCOMMAND_H
#define KOSHAPEREORDERCOMMAND_H

#include "flake_export.h"

#include <QList>
#include <kundo2command.h>

class KoShape;
class KoShapeManager;
class KoShapeReorderCommandPrivate;

/// This command allows you to change the zIndex of a number of shapes.
class FLAKE_EXPORT KoShapeReorderCommand : public KUndo2Command
{
public:
    /**
     * Constructor.
     * @param shapes the set of objects that are moved.
     * @param newIndexes the new indexes for the shapes.
     *  this list naturally must have the same amount of items as the shapes set.
     * @param parent the parent command used for macro commands
     */
    KoShapeReorderCommand(const QList<KoShape *> &shapes, QList<int> &newIndexes, KUndo2Command *parent = nullptr);
    ~KoShapeReorderCommand() override;

    /// An enum for defining what kind of reordering to use.
    enum MoveShapeType {
        RaiseShape, ///< raise the selected shape to the level that it is above the shape that is on top of it.
        LowerShape, ///< Lower the selected shape to the level that it is below the shape that is below it.
        BringToFront, ///< Raise the selected shape to be on top of all shapes.
        SendToBack ///< Lower the selected shape to be below all other shapes.
    };

    /**
     * Create a new KoShapeReorderCommand by calculating the new indexes required to move the shapes
     * according to the move parameter.
     * @param shapes all the shapes that should be moved.
     * @param manager the shapeManager that contains all the shapes that could have their indexes changed.
     * @param move the moving type.
     * @param parent the parent command for grouping purposes.
     * @return command for reordering the shapes or 0 if no reordering happened
     */
    static KoShapeReorderCommand *createCommand(const QList<KoShape *> &shapes, KoShapeManager *manager, MoveShapeType move, KUndo2Command *parent = nullptr);

    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    KoShapeReorderCommandPrivate *const d;
};

#endif
