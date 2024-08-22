/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2007 Boudewijn Rempt <boud@kde.org>
 * SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2007, 2009, 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoPathConnectionPointStrategy.h"
#include "KoCanvasBase.h"
#include "KoConnectionShape.h"
#include "KoPathConnectionPointStrategy_p.h"
#include "KoShapeConnectionChangeCommand.h"
#include "KoShapeManager.h"

#include <float.h>
#include <math.h>

const int InvalidConnectionPointId = INT_MIN;

KoPathConnectionPointStrategy::KoPathConnectionPointStrategy(KoToolBase *tool, KoConnectionShape *shape, int handleId)
    : KoParameterChangeStrategy(*(new KoPathConnectionPointStrategyPrivate(tool, shape, handleId)))
{
}

KoPathConnectionPointStrategy::~KoPathConnectionPointStrategy() = default;

void KoPathConnectionPointStrategy::handleMouseMove(const QPointF &mouseLocation, Qt::KeyboardModifiers modifiers)
{
    Q_D(KoPathConnectionPointStrategy);

    const qreal MAX_DISTANCE = 20.0; // TODO make user definable
    const qreal MAX_DISTANCE_SQR = MAX_DISTANCE * MAX_DISTANCE;

    d->newConnectionShape = nullptr;
    d->newConnectionId = InvalidConnectionPointId;

    QRectF roi(mouseLocation - QPointF(MAX_DISTANCE, MAX_DISTANCE), QSizeF(2 * MAX_DISTANCE, 2 * MAX_DISTANCE));
    QList<KoShape *> shapes = d->tool->canvas()->shapeManager()->shapesAt(roi, true);
    if (shapes.count() < 2) {
        // we are not near any other shape, so remove the corresponding connection
        if (d->handleId == 0)
            d->connectionShape->connectFirst(nullptr, InvalidConnectionPointId);
        else
            d->connectionShape->connectSecond(nullptr, InvalidConnectionPointId);

        KoParameterChangeStrategy::handleMouseMove(mouseLocation, modifiers);
    } else {
        qreal minimalDistance = DBL_MAX;
        QPointF nearestPoint;
        KoShape *nearestShape = nullptr;
        int nearestPointId = InvalidConnectionPointId;

        foreach (KoShape *shape, shapes) {
            // we do not want to connect to ourself
            if (shape == d->connectionShape)
                continue;

            KoConnectionPoints connectionPoints = shape->connectionPoints();
            if (!connectionPoints.count()) {
                QSizeF size = shape->size();
                connectionPoints[-1] = QPointF(0.0, 0.0);
                connectionPoints[-2] = QPointF(size.width(), 0.0);
                connectionPoints[-3] = QPointF(size.width(), size.height());
                connectionPoints[-4] = QPointF(0.0, size.height());
                connectionPoints[-5] = 0.5 * (connectionPoints[-1].position + connectionPoints[-2].position);
                connectionPoints[-6] = 0.5 * (connectionPoints[-2].position + connectionPoints[-3].position);
                connectionPoints[-7] = 0.5 * (connectionPoints[-3].position + connectionPoints[-4].position);
                connectionPoints[-8] = 0.5 * (connectionPoints[-4].position + connectionPoints[-1].position);
            }
            QPointF localMousePosition = shape->absoluteTransformation(nullptr).inverted().map(mouseLocation);
            KoConnectionPoints::const_iterator cp = connectionPoints.constBegin();
            KoConnectionPoints::const_iterator lastCp = connectionPoints.constEnd();
            for (; cp != lastCp; ++cp) {
                QPointF difference = localMousePosition - cp.value().position;
                qreal distance = difference.x() * difference.x() + difference.y() * difference.y();
                if (distance > MAX_DISTANCE_SQR)
                    continue;
                if (distance < minimalDistance) {
                    nearestShape = shape;
                    nearestPoint = cp.value().position;
                    nearestPointId = cp.key();
                    minimalDistance = distance;
                }
            }
        }

        if (nearestShape) {
            nearestPoint = nearestShape->absoluteTransformation(nullptr).map(nearestPoint);
        } else {
            nearestPoint = mouseLocation;
        }
        d->newConnectionShape = nearestShape;
        d->newConnectionId = nearestPointId;
        if (d->handleId == 0)
            d->connectionShape->connectFirst(nearestShape, nearestPointId);
        else
            d->connectionShape->connectSecond(nearestShape, nearestPointId);
        KoParameterChangeStrategy::handleMouseMove(nearestPoint, modifiers);
    }
}

void KoPathConnectionPointStrategy::finishInteraction(Qt::KeyboardModifiers modifiers)
{
    KoParameterChangeStrategy::finishInteraction(modifiers);
}

KUndo2Command *KoPathConnectionPointStrategy::createCommand()
{
    Q_D(KoPathConnectionPointStrategy);

    // check if we connect to a shape and if the connection point is already present
    if (d->newConnectionShape && d->newConnectionId < 0 && d->newConnectionId != InvalidConnectionPointId) {
        // map handle position into document coordinates
        QPointF p = d->connectionShape->shapeToDocument(d->connectionShape->handlePosition(d->handleId));
        // and add as connection point in shape coordinates
        d->newConnectionId = d->newConnectionShape->addConnectionPoint(d->newConnectionShape->absoluteTransformation(nullptr).inverted().map(p));
    }

    KUndo2Command *cmd = KoParameterChangeStrategy::createCommand();
    if (!cmd)
        return nullptr;

    // change connection
    new KoShapeConnectionChangeCommand(d->connectionShape,
                                       static_cast<KoConnectionShape::HandleId>(d->handleId),
                                       d->oldConnectionShape,
                                       d->oldConnectionId,
                                       d->newConnectionShape,
                                       d->newConnectionId,
                                       cmd);
    return cmd;
}
