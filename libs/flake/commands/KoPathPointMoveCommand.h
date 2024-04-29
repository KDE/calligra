/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006, 2009 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2006, 2007 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPATHPOINTMOVECOMMAND_H
#define KOPATHPOINTMOVECOMMAND_H

#include "flake_export.h"

#include <kundo2command.h>

#include "KoPathPointData.h"

class KoPathPointMoveCommandPrivate;
class QPointF;

/// The undo / redo command for path point moving.
class FLAKE_EXPORT KoPathPointMoveCommand : public KUndo2Command
{
public:
    /**
     * Command to move path points.
     * @param pointData the path points to move
     * @param offset the offset by which the point is moved in document coordinates
     * @param parent the parent command used for macro commands
     */
    KoPathPointMoveCommand(const QList<KoPathPointData> &pointData, const QPointF &offset, KUndo2Command *parent = nullptr);

    /**
     * Command to move path points.
     * @param pointData the path points to move
     * @param offsets the offsets by which the points are moved in document coordinates
     * @param parent the parent command used for macro commands
     */
    KoPathPointMoveCommand(const QList<KoPathPointData> &pointData, const QVector<QPointF> &offsets, KUndo2Command *parent = nullptr);

    ~KoPathPointMoveCommand() override;

    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    KoPathPointMoveCommandPrivate *const d;
};

#endif // KOPATHPOINTMOVECOMMAND_H
