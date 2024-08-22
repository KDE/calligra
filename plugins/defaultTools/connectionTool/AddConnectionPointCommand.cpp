/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "AddConnectionPointCommand.h"
#include <KoShape.h>

AddConnectionPointCommand::AddConnectionPointCommand(KoShape *shape, const QPointF &connectionPoint, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_shape(shape)
    , m_connectionPoint(connectionPoint)
    , m_connectionPointId(-1)
{
    Q_ASSERT(m_shape);
}

AddConnectionPointCommand::~AddConnectionPointCommand() = default;

void AddConnectionPointCommand::redo()
{
    if (m_connectionPointId < 0) {
        m_connectionPointId = m_shape->addConnectionPoint(m_connectionPoint);
    } else {
        m_shape->setConnectionPoint(m_connectionPointId, m_connectionPoint);
    }
    updateRoi();

    KUndo2Command::redo();
}

void AddConnectionPointCommand::undo()
{
    KUndo2Command::undo();

    m_shape->removeConnectionPoint(m_connectionPointId);
    updateRoi();
}

void AddConnectionPointCommand::updateRoi()
{
    // TODO: is there a way we can get at the correct update size?
    QRectF roi(0, 0, 10, 10);
    roi.moveCenter(m_connectionPoint);
    m_shape->update(roi);
}
