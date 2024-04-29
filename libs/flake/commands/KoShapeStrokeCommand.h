/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2006-2008 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPESTROKECOMMAND_H
#define KOSHAPESTROKECOMMAND_H

#include "flake_export.h"

#include <QList>
#include <kundo2command.h>

class KoShape;
class KoShapeStrokeModel;

/// The undo / redo command for setting the shape stroke
class FLAKE_EXPORT KoShapeStrokeCommand : public KUndo2Command
{
public:
    /**
     * Command to set a new shape stroke.
     * @param shapes a set of all the shapes that should get the new stroke.
     * @param stroke the new stroke, the same for all given shapes
     * @param parent the parent command used for macro commands
     */
    KoShapeStrokeCommand(const QList<KoShape *> &shapes, KoShapeStrokeModel *stroke, KUndo2Command *parent = nullptr);

    /**
     * Command to set new shape strokes.
     * @param shapes a set of all the shapes that should get a new stroke.
     * @param strokes the new strokes, one for each shape
     * @param parent the parent command used for macro commands
     */
    KoShapeStrokeCommand(const QList<KoShape *> &shapes, const QList<KoShapeStrokeModel *> &strokes, KUndo2Command *parent = nullptr);

    /**
     * Command to set a new shape stroke.
     * @param shape a single shape that should get the new stroke.
     * @param stroke the new stroke
     * @param parent the parent command used for macro commands
     */
    KoShapeStrokeCommand(KoShape *shape, KoShapeStrokeModel *stroke, KUndo2Command *parent = nullptr);

    ~KoShapeStrokeCommand() override;
    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    class Private;
    Private *const d;
};

#endif
