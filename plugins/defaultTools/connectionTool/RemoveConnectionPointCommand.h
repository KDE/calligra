/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef REMOVECONNECTIONPOINTCOMMAND_H
#define REMOVECONNECTIONPOINTCOMMAND_H

#include <KoConnectionPoint.h>
#include <QPointF>
#include <kundo2command.h>

class KoShape;

class RemoveConnectionPointCommand : public KUndo2Command
{
public:
    /// Creates new command to remove connection point from shape
    RemoveConnectionPointCommand(KoShape *shape, int connectionPointId, KUndo2Command *parent = nullptr);
    ~RemoveConnectionPointCommand() override;
    /// reimplemented from KUndo2Command
    void redo() override;
    /// reimplemented from KUndo2Command
    void undo() override;

private:
    void updateRoi();

    KoShape *m_shape;
    KoConnectionPoint m_connectionPoint;
    int m_connectionPointId;
};

#endif // REMOVECONNECTIONPOINTCOMMAND_H
