/* This file is part of the KDE project
 * Copyright (C) 2007-2008 Jan Hambrecht <jaham@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KarbonPathFlattenCommand.h"
#include <KoPathShape.h>
#include <KoPathPoint.h>
#include <KoPathPointData.h>
#include <klocale.h>
#include <QtGlobal>
#include <math.h>

#ifndef log2
# define log2(x) (log(x) / M_LN2)
#endif

/*
 * <cite from GNU ghostscript's gxpflat.c>
 *
 * To calculate how many points to sample along a path in order to
 * approximate it to the desired degree of flatness, we define
 *      dist((x,y)) = abs(x) + abs(y);
 * then the number of points we need is
 *      N = 1 + sqrt(3/4 * D / flatness),
 * where
 *      D = max(dist(p0 - 2*p1 + p2), dist(p1 - 2*p2 + p3)).
 * Since we are going to use a power of 2 for the number of intervals,
 * we can avoid the square root by letting
 *      N = 1 + 2^(ceiling(log2(3/4 * D / flatness) / 2)).
 * (Reference: DEC Paris Research Laboratory report #1, May 1989.)
 *
 * We treat two cases specially.  First, if the curve is very
 * short, we halve the flatness, to avoid turning short shallow curves
 * into short straight lines.  Second, if the curve forms part of a
 * character (indicated by flatness = 0), we let
 *      N = 1 + 2 * max(abs(x3-x0), abs(y3-y0)).
 * This is probably too conservative, but it produces good results.
 *
 * </cite from GNU ghostscript's gxpflat.c>
 */

// used for storing the data needed undo
struct PointData
{
    PointData()
            : insertedPoints(0)
    {}
    // old control points in document coordinates
    QPointF oldControlPoint1;
    QPointF oldControlPoint2;
    KoPathPoint::PointProperties oldProperties;
    uint insertedPoints;
    bool activeControlPoint1;
    bool activeControlPoint2;
};

class KarbonPathFlattenCommand::Private
{
public:
    Private(KoPathShape * p, qreal f)
            : path(p), flatness(f), flattened(false)
    {}

    qreal distance(const QPointF &p)
    {
        return qAbs(p.x()) + qAbs(p.y());
    }

    // splits the given segment at the splitPosition and returns the inserted point
    KoPathPoint * splitSegment(KoPathPoint *p1, KoPathPoint *p2, qreal splitPosition)
    {
        if (!p1->activeControlPoint2() && ! p2->activeControlPoint1())
            return 0;

        QPointF q[4] = {
            p1->point(),
            p1->activeControlPoint2() ? p1->controlPoint2() : p1->point(),
            p2->activeControlPoint1() ? p2->controlPoint1() : p2->point(),
            p2->point()
        };

        QPointF p[3];
        // the De Casteljau algorithm.
        for (unsigned short j = 1; j <= 3; ++j) {
            for (unsigned short i = 0; i <= 3 - j; ++i) {
                q[i] = (1.0 - splitPosition) * q[i] + splitPosition * q[i + 1];
            }
            // modify the new segment.
            p[j - 1] = q[0];
        }

        p1->setControlPoint2(p[0]);
        p2->setControlPoint1(q[2]);

        KoPathPoint * splitPoint = new KoPathPoint(p1->parent(), p[2]);
        splitPoint->setControlPoint1(p[1]);
        splitPoint->setControlPoint2(q[1]);
        return splitPoint;
    }

    // determines number of splits for given segment
    uint splitCount(KoPathPoint *curr, KoPathPoint *next)
    {
        QPointF p0 = curr->point();
        QPointF p1 = curr->activeControlPoint2() ? curr->controlPoint2() : curr->point();
        QPointF p2 = next->activeControlPoint1() ? next->controlPoint1() : next->point();
        QPointF p3 = next->point();
        uint insertCount = 0;
        if (flatness == 0)
            insertCount = 1 + static_cast<uint>(2 * qMax(qAbs(p3.x() - p0.x()), qAbs(p3.y() - p0.y())));
        else {
            qreal d = qMax(distance(p0 - 2 * p1 + p2), distance(p1 - 2 * p2 + p3));
            insertCount = 1 + static_cast<uint>(pow(2, (ceil(log2(0.75 * d / flatness) / 2))));
        }

        return insertCount;
    }

    KoPathShape * path;
    qreal flatness;
    qreal flattened;
    QList< QList<PointData> > oldPointData;
};

KarbonPathFlattenCommand::KarbonPathFlattenCommand(KoPathShape * path, qreal flatness, QUndoCommand * parent)
        : QUndoCommand(parent), d(new Private(path, flatness))
{
    // save original point data
    uint subpathCount = d->path->subpathCount();
    // iterate over all the subpaths
    for (uint subpathIndex = 0; subpathIndex < subpathCount; ++subpathIndex) {
        QList<PointData> subpathData;

        int pointCount = d->path->subpathPointCount(subpathIndex);
        // iterate over all the points/segments
        for (int pointIndex = 0; pointIndex < pointCount; ++pointIndex) {
            KoPathPoint * curr = d->path->pointByIndex(KoPathPointIndex(subpathIndex, pointIndex));

            // save the data of the current path point
            PointData pointData;
            pointData.oldControlPoint1 = curr->parent()->shapeToDocument(curr->controlPoint1());
            pointData.oldControlPoint2 = curr->parent()->shapeToDocument(curr->controlPoint2());
            pointData.oldProperties = curr->properties();
            pointData.activeControlPoint1 = curr->activeControlPoint1();
            pointData.activeControlPoint2 = curr->activeControlPoint2();
            subpathData.append(pointData);
        }
        d->oldPointData.append(subpathData);
    }
    setText(i18n("Flatten path"));
}

KarbonPathFlattenCommand::~KarbonPathFlattenCommand()
{
    delete d;
}

void KarbonPathFlattenCommand::redo()
{
    if (! d->flattened) {
        uint subpathCount = d->oldPointData.count();
        // iterate over all the subpaths
        for (uint subpathIndex = 0; subpathIndex < subpathCount; ++subpathIndex) {
            uint pointCount = d->oldPointData[subpathIndex].count();
            uint insertedPointOffset = 0;
            // iterate over all the points/segments
            for (uint pointIndex = 0; pointIndex < pointCount; ++pointIndex) {
                uint correctedPointIndex = pointIndex + insertedPointOffset;
                uint correctedPointCount = pointCount + insertedPointOffset;

                // break the loop if we are at the end of the subpath and the subpath is not closed
                if (correctedPointIndex + 1 == correctedPointCount && ! d->path->isClosedSubpath(subpathIndex))
                    break;

                KoPathPoint * curr = d->path->pointByIndex(KoPathPointIndex(subpathIndex, correctedPointIndex));
                KoPathPoint * next = d->path->pointByIndex(KoPathPointIndex(subpathIndex, (correctedPointIndex + 1) % correctedPointCount));

                // determine how many point we want to insert
                uint requestedSplitCount = d->splitCount(curr, next);

                KoPathPoint * segStart = curr;
                KoPathPoint * segEnd = next;

                uint achievedSplitCount = 0; // counter for executed splits
                // split the segment
                for (uint split = 0; split < requestedSplitCount; ++split) {
                    // calculate the split point...
                    segStart = d->splitSegment(segStart, segEnd, 1.0 / (requestedSplitCount - split + 1));
                    if (! segStart)
                        break;
                    // and insert it into the path
                    if (! curr->parent()->insertPoint(segStart, KoPathPointIndex(subpathIndex, correctedPointIndex + 1 + split))) {
                        delete segStart;
                        break;
                    }

                    achievedSplitCount++;
                }
                if (! segStart)
                    continue;

                // save number of points inserted, we need it for undo
                d->oldPointData[subpathIndex][pointIndex].insertedPoints = achievedSplitCount;
                // adjust the point index offset
                insertedPointOffset += achievedSplitCount;
            }
            int newPointCount = d->path->subpathPointCount(subpathIndex);
            // now remove all the control points from the subpath
            for (int pointIndex = 0; pointIndex < newPointCount; ++pointIndex) {
                KoPathPoint * point = d->path->pointByIndex(KoPathPointIndex(subpathIndex, pointIndex));
                point->removeControlPoint1();
                point->removeControlPoint2();
            }
        }
        d->flattened = true;
        d->path->normalize();
    } else {
        QUndoCommand::redo();
    }
    d->path->update();
}

void KarbonPathFlattenCommand::undo()
{
    QUndoCommand::undo();
    if (d->flattened) {
        uint subpathCount = d->oldPointData.count();
        for (uint subpathIndex = 0; subpathIndex < subpathCount; ++subpathIndex) {
            uint pointCount = d->oldPointData[subpathIndex].count();
            for (uint pointIndex = 0; pointIndex < pointCount; ++pointIndex) {
                PointData &data = d->oldPointData[subpathIndex][pointIndex];

                // remove inserted points for this original point
                for (uint i = 0; i < data.insertedPoints; ++i)
                    d->path->removePoint(KoPathPointIndex(subpathIndex, pointIndex + data.insertedPoints - i));

                // get the original point...
                KoPathPoint * p = d->path->pointByIndex(KoPathPointIndex(subpathIndex, pointIndex));
                if (! p)
                    continue;
                // and restore original point data
                p->setProperties(data.oldProperties);
                if (data.activeControlPoint1)
                    p->setControlPoint1(d->path->documentToShape(data.oldControlPoint1));
                if (data.activeControlPoint2)
                    p->setControlPoint2(d->path->documentToShape(data.oldControlPoint2));
            }
        }
        d->flattened = false;
        d->path->normalize();
    }

    d->path->update();
}

