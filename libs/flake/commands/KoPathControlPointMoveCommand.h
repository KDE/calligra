/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2006, 2007 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPATHCONTROLPOINTMOVECOMMAND_H
#define KOPATHCONTROLPOINTMOVECOMMAND_H

#include "KoPathPoint.h"
#include "KoPathPointData.h"
#include "flake_export.h"
#include <QPointF>
#include <kundo2command.h>

/// The undo / redo command for path point moving.
class FLAKE_EXPORT KoPathControlPointMoveCommand : public KUndo2Command
{
public:
    /**
     * Command to move one control path point.
     * @param pointData the point data
     * @param offset the offset by which the point is moved in document coordinates
     * @param pointType the type of the point to move
     * @param parent the parent command used for macro commands
     */
    KoPathControlPointMoveCommand(const KoPathPointData &pointData, const QPointF &offset, KoPathPoint::PointType pointType, KUndo2Command *parent = nullptr);
    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    KoPathPointData m_pointData;
    // the offset in shape coordinates
    QPointF m_offset;
    KoPathPoint::PointType m_pointType;
};

#endif // KOPATHCONTROLPOINTMOVECOMMAND_H
