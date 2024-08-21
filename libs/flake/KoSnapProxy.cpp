/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008-2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoSnapProxy.h"
#include "KoCanvasBase.h"
#include "KoPathPoint.h"
#include "KoPathShape.h"
#include "KoShapeManager.h"
#include "KoSnapGuide.h"
#include <KoSnapData.h>

KoSnapProxy::KoSnapProxy(KoSnapGuide *snapGuide)
    : m_snapGuide(snapGuide)
{
}

QVector<QPointF> KoSnapProxy::pointsInRect(const QRectF &rect) const
{
    QVector<QPointF> result;
    QList<KoShape *> shapes = shapesInRect(rect);
    for (KoShape *shape : shapes) {
        // There exists a problem on msvc with for(each) and QVector<QPointF>
        QVector<QPointF> points(pointsFromShape(shape));
        for (int i = 0; i < points.count(); ++i) {
            const QPointF point(points[i]);
            if (rect.contains(point))
                result.append(point);
        }
    }

    return result;
}

QList<KoShape *> KoSnapProxy::shapesInRect(const QRectF &rect, bool omitEditedShape) const
{
    QList<KoShape *> shapes = m_snapGuide->canvas()->shapeManager()->shapesAt(rect);
    for (KoShape *shape : m_snapGuide->ignoredShapes()) {
        int index = shapes.indexOf(shape);
        if (index >= 0)
            shapes.removeAt(index);
    }
    if (!omitEditedShape && m_snapGuide->editedShape()) {
        QRectF bound = m_snapGuide->editedShape()->boundingRect();
        if (rect.intersects(bound) || rect.contains(bound))
            shapes.append(m_snapGuide->editedShape());
    }
    return shapes;
}

QVector<QPointF> KoSnapProxy::pointsFromShape(KoShape *shape) const
{
    QVector<QPointF> snapPoints;
    // no snapping to hidden shapes
    if (!shape->isVisible(true))
        return snapPoints;

    // return the special snap points of the shape
    snapPoints += shape->snapData().snapPoints();

    KoPathShape *path = dynamic_cast<KoPathShape *>(shape);
    if (path) {
        QTransform m = path->absoluteTransformation(nullptr);

        QList<KoPathPoint *> ignoredPoints = m_snapGuide->ignoredPathPoints();

        int subpathCount = path->subpathCount();
        for (int subpathIndex = 0; subpathIndex < subpathCount; ++subpathIndex) {
            int pointCount = path->subpathPointCount(subpathIndex);
            for (int pointIndex = 0; pointIndex < pointCount; ++pointIndex) {
                KoPathPoint *p = path->pointByIndex(KoPathPointIndex(subpathIndex, pointIndex));
                if (!p || ignoredPoints.contains(p))
                    continue;

                snapPoints.append(m.map(p->point()));
            }
        }
    } else {
        // add the bounding box corners as default snap points
        QRectF bbox = shape->boundingRect();
        snapPoints.append(bbox.topLeft());
        snapPoints.append(bbox.topRight());
        snapPoints.append(bbox.bottomRight());
        snapPoints.append(bbox.bottomLeft());
    }

    return snapPoints;
}

QList<KoPathSegment> KoSnapProxy::segmentsInRect(const QRectF &rect) const
{
    QList<KoShape *> shapes = shapesInRect(rect, true);
    QList<KoPathPoint *> ignoredPoints = m_snapGuide->ignoredPathPoints();

    QList<KoPathSegment> segments;
    for (KoShape *shape : shapes) {
        QList<KoPathSegment> shapeSegments;
        QRectF rectOnShape = shape->documentToShape(rect);
        KoPathShape *path = dynamic_cast<KoPathShape *>(shape);
        if (path) {
            shapeSegments = path->segmentsAt(rectOnShape);
        } else {
            for (const KoPathSegment &s : shape->snapData().snapSegments()) {
                QRectF controlRect = s.controlPointRect();
                if (!rect.intersects(controlRect) && !controlRect.contains(rect))
                    continue;
                QRectF bound = s.boundingRect();
                if (!rect.intersects(bound) && !bound.contains(rect))
                    continue;
                shapeSegments.append(s);
            }
        }

        QTransform m = shape->absoluteTransformation(nullptr);
        // transform segments to document coordinates
        for (const KoPathSegment &s : shapeSegments) {
            if (ignoredPoints.contains(s.first()) || ignoredPoints.contains(s.second()))
                continue;
            segments.append(s.mapped(m));
        }
    }
    return segments;
}

QList<KoShape *> KoSnapProxy::shapes(bool omitEditedShape) const
{
    QList<KoShape *> allShapes = m_snapGuide->canvas()->shapeManager()->shapes();
    QList<KoShape *> filteredShapes;
    QList<KoShape *> ignoredShapes = m_snapGuide->ignoredShapes();

    // filter all hidden and ignored shapes
    for (KoShape *shape : allShapes) {
        if (!shape->isVisible(true))
            continue;
        if (ignoredShapes.contains(shape))
            continue;

        filteredShapes.append(shape);
    }
    if (!omitEditedShape && m_snapGuide->editedShape())
        filteredShapes.append(m_snapGuide->editedShape());

    return filteredShapes;
}

KoCanvasBase *KoSnapProxy::canvas() const
{
    return m_snapGuide->canvas();
}
