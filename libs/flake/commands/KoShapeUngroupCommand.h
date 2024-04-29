/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2006 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPEUNGROUPCOMMAND_H
#define KOSHAPEUNGROUPCOMMAND_H

#include "KoShapeGroupCommand.h"

#include "flake_export.h"

#include <kundo2command.h>

/// The undo / redo command for ungrouping shapes
class FLAKE_EXPORT KoShapeUngroupCommand : public KoShapeGroupCommand
{
public:
    /**
     * Command to ungroup a set of shapes from one parent container.
     * @param container the group to ungroup the shapes from.
     * @param shapes a list of all the shapes that should be ungrouped.
     * @param topLevelShapes the top level shapes.
     * @param parent the parent command used for macro commands
     */
    KoShapeUngroupCommand(KoShapeContainer *container,
                          const QList<KoShape *> &shapes,
                          const QList<KoShape *> &topLevelShapes = QList<KoShape *>(),
                          KUndo2Command *parent = nullptr);
    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;
};

#endif
