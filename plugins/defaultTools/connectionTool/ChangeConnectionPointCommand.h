/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef CHANGECONNECTIONPOINTCOMMAND_H
#define CHANGECONNECTIONPOINTCOMMAND_H

#include <KoConnectionPoint.h>
#include <QPointF>
#include <kundo2command.h>

class KoShape;

class ChangeConnectionPointCommand : public KUndo2Command
{
public:
    /// Creates new command to change connection point of shape
    ChangeConnectionPointCommand(KoShape *shape,
                                 int connectionPointId,
                                 const KoConnectionPoint &oldPoint,
                                 const KoConnectionPoint &newPoint,
                                 KUndo2Command *parent = nullptr);
    ~ChangeConnectionPointCommand() override;
    /// reimplemented from KUndo2Command
    void redo() override;
    /// reimplemented from KUndo2Command
    void undo() override;

private:
    void updateRoi(const QPointF &position);

    KoShape *m_shape;
    int m_connectionPointId;
    KoConnectionPoint m_oldPoint;
    KoConnectionPoint m_newPoint;
};

#endif // CHANGECONNECTIONPOINTCOMMAND_H
