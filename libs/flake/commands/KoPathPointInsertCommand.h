/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2006, 2007 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPATHPOINTINSERTCOMMAND_H
#define KOPATHPOINTINSERTCOMMAND_H

#include "KoPathPointData.h"
#include "flake_export.h"
#include <QList>
#include <kundo2command.h>

class KoPathPointInsertCommandPrivate;
class KoPathPoint;

/// The undo / redo command for inserting path points
class FLAKE_EXPORT KoPathPointInsertCommand : public KUndo2Command
{
public:
    /**
     * Command to insert path points.
     *
     * This splits the segments at the given position by inserting new points.
     * The De Casteljau algorithm is used for calculating the position of the new
     * points.
     *
     * @param pointDataList describing the segments to split
     * @param insertPosition the position to insert at [0..1]
     * @param parent the parent command used for macro commands
     */
    KoPathPointInsertCommand(const QList<KoPathPointData> &pointDataList, qreal insertPosition, KUndo2Command *parent = nullptr);
    ~KoPathPointInsertCommand() override;

    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

    /// Returns list of inserted points
    QList<KoPathPoint *> insertedPoints() const;

private:
    KoPathPointInsertCommandPrivate *const d;
};

#endif // KOPATHPOINTINSERTCOMMAND_H
