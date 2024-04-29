/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef ADDCONNECTIONPOINTCOMMAND_H
#define ADDCONNECTIONPOINTCOMMAND_H

#include <QPointF>
#include <kundo2command.h>

class KoShape;

class AddConnectionPointCommand : public KUndo2Command
{
public:
    /// Creates new command to add connection point to shape
    AddConnectionPointCommand(KoShape *shape, const QPointF &connectionPoint, KUndo2Command *parent = nullptr);
    ~AddConnectionPointCommand() override;
    /// reimplemented from KUndo2Command
    void redo() override;
    /// reimplemented from KUndo2Command
    void undo() override;

private:
    void updateRoi();

    KoShape *m_shape;
    QPointF m_connectionPoint;
    int m_connectionPointId;
};

#endif // ADDCONNECTIONPOINTCOMMAND_H
