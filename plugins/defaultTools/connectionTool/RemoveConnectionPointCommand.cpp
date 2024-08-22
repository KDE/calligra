/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "RemoveConnectionPointCommand.h"
#include <KoShape.h>

RemoveConnectionPointCommand::RemoveConnectionPointCommand(KoShape *shape, int connectionPointId, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_shape(shape)
    , m_connectionPointId(connectionPointId)
{
    Q_ASSERT(m_shape);
    m_connectionPoint = m_shape->connectionPoint(m_connectionPointId);
}

RemoveConnectionPointCommand::~RemoveConnectionPointCommand() = default;

void RemoveConnectionPointCommand::redo()
{
    // TODO: check if there is a connection shape attached
    // and handle that appropriately
    m_shape->removeConnectionPoint(m_connectionPointId);
    updateRoi();

    KUndo2Command::redo();
}

void RemoveConnectionPointCommand::undo()
{
    KUndo2Command::undo();

    m_shape->setConnectionPoint(m_connectionPointId, m_connectionPoint);
    updateRoi();
}

void RemoveConnectionPointCommand::updateRoi()
{
    // TODO: is there a way we can get at the correct update size?
    QRectF roi(0, 0, 10, 10);
    roi.moveCenter(m_connectionPoint.position);
    m_shape->update(roi);
}
