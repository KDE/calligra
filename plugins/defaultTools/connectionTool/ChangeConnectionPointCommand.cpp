/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ChangeConnectionPointCommand.h"
#include <KoShape.h>

ChangeConnectionPointCommand::ChangeConnectionPointCommand(KoShape *shape,
                                                           int connectionPointId,
                                                           const KoConnectionPoint &oldPoint,
                                                           const KoConnectionPoint &newPoint,
                                                           KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_shape(shape)
    , m_connectionPointId(connectionPointId)
    , m_oldPoint(oldPoint)
    , m_newPoint(newPoint)
{
    Q_ASSERT(m_shape);
}

ChangeConnectionPointCommand::~ChangeConnectionPointCommand() = default;

void ChangeConnectionPointCommand::redo()
{
    updateRoi(m_oldPoint.position);
    m_shape->setConnectionPoint(m_connectionPointId, m_newPoint);
    updateRoi(m_newPoint.position);

    KUndo2Command::redo();
}

void ChangeConnectionPointCommand::undo()
{
    KUndo2Command::undo();

    updateRoi(m_newPoint.position);
    m_shape->setConnectionPoint(m_connectionPointId, m_oldPoint);
    updateRoi(m_oldPoint.position);
}

void ChangeConnectionPointCommand::updateRoi(const QPointF &position)
{
    // TODO: is there a way we can get at the correct update size?
    QRectF roi(0, 0, 10, 10);
    roi.moveCenter(position);
    m_shape->update(roi);
}
