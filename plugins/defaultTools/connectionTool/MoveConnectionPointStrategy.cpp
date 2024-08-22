/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "MoveConnectionPointStrategy.h"
#include "ChangeConnectionPointCommand.h"
#include <KoCanvasBase.h>
#include <KoShape.h>
#include <KoToolBase.h>

MoveConnectionPointStrategy::MoveConnectionPointStrategy(KoShape *shape, int connectionPointId, KoToolBase *parent)
    : KoInteractionStrategy(parent)
    , m_shape(shape)
    , m_connectionPointId(connectionPointId)
{
    Q_ASSERT(m_shape);
    m_oldPoint = m_newPoint = m_shape->connectionPoint(m_connectionPointId);
}

MoveConnectionPointStrategy::~MoveConnectionPointStrategy() = default;

void MoveConnectionPointStrategy::paint(QPainter &painter, const KoViewConverter &converter)
{
    KoInteractionStrategy::paint(painter, converter);
}

void MoveConnectionPointStrategy::handleMouseMove(const QPointF &mouseLocation, Qt::KeyboardModifiers /*modifiers*/)
{
    m_newPoint.position = m_shape->documentToShape(mouseLocation);
    m_shape->setConnectionPoint(m_connectionPointId, m_newPoint);
}

void MoveConnectionPointStrategy::cancelInteraction()
{
    KoInteractionStrategy::cancelInteraction();
    m_shape->setConnectionPoint(m_connectionPointId, m_oldPoint);
}

void MoveConnectionPointStrategy::finishInteraction(Qt::KeyboardModifiers /*modifiers*/)
{
}

KUndo2Command *MoveConnectionPointStrategy::createCommand()
{
    int grabDistance = grabSensitivity();
    const qreal dx = m_newPoint.position.x() - m_oldPoint.position.x();
    const qreal dy = m_newPoint.position.y() - m_oldPoint.position.y();
    // check if we have moved the connection point at least a little bit
    if (dx * dx + dy * dy < grabDistance * grabDistance)
        return nullptr;

    return new ChangeConnectionPointCommand(m_shape, m_connectionPointId, m_oldPoint, m_newPoint);
}
