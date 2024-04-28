/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KarbonWhirlPinchCommand.h"

#include <KoPathPoint.h>
#include <KoPathShape.h>

#include <KLocalizedString>
#include <math.h>

// the original data of a path point
struct PointData {
    PointData(KoPathPoint *p)
    {
        KoPathShape *shape = p->parent();
        // save points in document coordinates
        oldNode = shape->shapeToDocument(p->point());
        oldControlPoint1 = shape->shapeToDocument(p->controlPoint1());
        oldControlPoint2 = shape->shapeToDocument(p->controlPoint2());
    }

    void restorePoint(KoPathPoint *p)
    {
        KoPathShape *shape = p->parent();
        p->setPoint(shape->documentToShape(oldNode));
        if (p->activeControlPoint1())
            p->setControlPoint1(shape->documentToShape(oldControlPoint1));
        if (p->activeControlPoint2())
            p->setControlPoint2(shape->documentToShape(oldControlPoint2));
    }

    QPointF oldNode;
    QPointF oldControlPoint1;
    QPointF oldControlPoint2;
};

class KarbonWhirlPinchCommand::Private
{
public:
    Private(KoPathShape *path, qreal angle, qreal pinch, qreal radius)
        : pathShape(path)
        , whirlAngle(angle)
        , pinchAmount(pinch)
        , effectRadius(radius)
    {
        effectCenter = pathShape->boundingRect().center();
        if (pinchAmount < -1.0)
            pinchAmount = -1.0;
        else if (pinchAmount > 1.0)
            pinchAmount = 1.0;
    }

    QPointF whirlPinch(const QPointF &point)
    {
        // transform to document coordinates first
        QPointF docPoint = pathShape->shapeToDocument(point);

        QPointF delta = docPoint - effectCenter;
        qreal distToCenter = sqrt(delta.x() * delta.x() + delta.y() * delta.y());

        if (distToCenter < effectRadius) {
            QTransform m;

            distToCenter /= effectRadius;

            qreal scale = pow(sin(M_PI_2 * distToCenter), -pinchAmount);

            // pinch:
            m.translate(effectCenter.x(), effectCenter.y());
            m.scale(scale, scale);

            // whirl:
            m.rotate(whirlAngle * (1.0 - distToCenter) * (1.0 - distToCenter));
            m.translate(-effectCenter.x(), -effectCenter.y());

            // transform back to shape coordinates
            return pathShape->documentToShape(m.map(docPoint));
        }
        return point;
    }

    KoPathShape *pathShape;
    qreal whirlAngle;
    qreal pinchAmount;
    qreal effectRadius;
    QPointF effectCenter;
    QList<QList<PointData>> pathData;
};

KarbonWhirlPinchCommand::KarbonWhirlPinchCommand(KoPathShape *path, qreal angle, qreal pinch, qreal radius, KUndo2Command *parent)
    : KUndo2Command(parent)
    , d(new Private(path, angle, pinch, radius))
{
    setText(kundo2_i18n("Whirl & pinch"));

    // save the path point data used for undo
    uint subpathCount = d->pathShape->subpathCount();
    for (uint subpathIndex = 0; subpathIndex < subpathCount; ++subpathIndex) {
        QList<PointData> subpathData;
        int pointCount = d->pathShape->subpathPointCount(subpathIndex);
        for (int pointIndex = 0; pointIndex < pointCount; ++pointIndex) {
            KoPathPoint *p = d->pathShape->pointByIndex(KoPathPointIndex(subpathIndex, pointIndex));
            subpathData.append(PointData(p));
        }
        d->pathData.append(subpathData);
    }
}

KarbonWhirlPinchCommand::~KarbonWhirlPinchCommand()
{
    delete d;
}

void KarbonWhirlPinchCommand::redo()
{
    d->pathShape->update();
    uint subpathCount = d->pathData.count();
    for (uint subpathIndex = 0; subpathIndex < subpathCount; ++subpathIndex) {
        uint pointCount = d->pathData[subpathIndex].count();
        for (uint pointIndex = 0; pointIndex < pointCount; ++pointIndex) {
            KoPathPoint *p = d->pathShape->pointByIndex(KoPathPointIndex(subpathIndex, pointIndex));
            p->setPoint(d->whirlPinch(p->point()));
            if (p->activeControlPoint1())
                p->setControlPoint1(d->whirlPinch(p->controlPoint1()));
            if (p->activeControlPoint2())
                p->setControlPoint2(d->whirlPinch(p->controlPoint2()));
        }
    }
    d->pathShape->normalize();
    d->pathShape->update();

    KUndo2Command::redo();
}

void KarbonWhirlPinchCommand::undo()
{
    d->pathShape->update();
    uint subpathCount = d->pathData.count();
    for (uint subpathIndex = 0; subpathIndex < subpathCount; ++subpathIndex) {
        uint pointCount = d->pathData[subpathIndex].count();
        for (uint pointIndex = 0; pointIndex < pointCount; ++pointIndex) {
            KoPathPoint *p = d->pathShape->pointByIndex(KoPathPointIndex(subpathIndex, pointIndex));
            d->pathData[subpathIndex][pointIndex].restorePoint(p);
        }
    }
    d->pathShape->normalize();
    d->pathShape->update();

    KUndo2Command::undo();
}
