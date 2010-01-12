/* This file is part of the KDE project
 * Copyright (C) 2008 Jan Hambrecht <jaham@gmx.net>
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

#include "RoundCornersCommand.h"

#include <KoPathShape.h>
#include <KoPathSegment.h>
#include <KoPathPoint.h>

#include <klocale.h>

#include <math.h>

RoundCornersCommand::RoundCornersCommand(KoPathShape * path, qreal radius, QUndoCommand * parent)
        : QUndoCommand(parent), m_path(path), m_copy(0)
{
    Q_ASSERT(path);

    // Set members.
    m_radius = radius > 0.0 ? radius : 1.0;
    // copy original path data
    m_copy = new KoPathShape();
    copyPath(m_copy, m_path);
    m_copy->normalize();
    setText(i18n("Round Corners"));
}

RoundCornersCommand::~RoundCornersCommand()
{
    delete m_copy;
}

void RoundCornersCommand::redo()
{
    m_path->update();
    roundPath();
    m_path->normalize();
    m_path->update();

    QUndoCommand::redo();
}

void RoundCornersCommand::undo()
{
    QUndoCommand::undo();

    m_path->update();
    copyPath(m_path, m_copy);
    m_path->update();
}

void RoundCornersCommand::roundPath()
{
    /*
    * This algorithm is worked out by <kudling AT kde DOT org> to produce similar results as
    * the "round corners" algorithms found in other applications. Neither code nor
    * algorithms from any 3rd party is used though.
    *
    * We want to replace all corners with round corners having "radius" m_radius.
    * The algorithm doesn't really produce circular arcs, but that's ok since
    * the algorithm achieves nice looking results and is generic enough to be applied
    * to all kind of paths.
    * Note also, that this algorithm doesn't touch smooth joins (in the sense of
    * KoPathPoint::isSmooth() ).
    *
    * We'll manipulate the input path for bookkeeping purposes and construct a new
    * temporary path in parallel. We finally replace the input path with the new path.
    *
    *
    * Without restricting generality, let's assume the input path is closed and
    * contains segments which build a rectangle.
    *
    *           2
    *    O------------O
    *    |            |        Numbers reflect the segments' order
    *   3|            |1       in the path. We neglect the "begin"
    *    |            |        segment here.
    *    O------------O
    *           0
    *
    * There are three unique steps to process. The second step is processed
    * many times in a loop.
    *
    * 1) Begin
    *    -----
    *    Split the first segment of the input path (called "path[0]" here)
    *    at parameter t
    *
    *        t = path[0]->param( m_radius )
    *
    *    and move newPath to this new knot. If current segment is too small
    *    (smaller than 2 * m_radius), we always set t = 0.5 here and in the further
    *    steps as well.
    *
    *    path:                 new path:
    *
    *           2
    *    O------------O
    *    |            |
    *  3 |            | 1                    The current segment is marked with "#"s.
    *    |            |
    *    O##O#########O        ...O
    *           0                     0
    *
    * 2) Loop
    *    ----
    *    The loop step is iterated over all segments. After each appliance the index n
    *    is incremented and the loop step is reapplied until no untouched segment is left.
    *
    *    Split the current segment path[n] of the input path at parameter t
    *
    *        t = path[n]->param( path[n]->length() - m_radius )
    *
    *    and add the first subsegment of the curent segment to newPath.
    *
    *    path:                 new path:
    *
    *           2
    *    O------------O
    *    |            |
    *  3 |            | 1
    *    |            |
    *    O--O######O##O           O------O...
    *           0                     0
    *
    *    Now make the second next segment (the original path[1] segment in our example)
    *    the current one. Split it at parameter t
    *
    *        t = path[n]->param( m_radius )
    *
    *    path:                 new path:
    *
    *           2
    *    O------------O
    *    |            #
    *  3 |            O 1
    *    |            #
    *    O--O------O--O           O------O...
    *           0                     0
    *
    *    Make the first subsegment of the current segment the current one.
    *
    *    path:                 new path:
    *
    *           2
    *    O------------O
    *    |            |
    *  3 |            O 1                   O
    *    |            #                    /.1
    *    O--O------O--O           O------O...
    *           0                     0
    *
    * 3) End
    *    ---
    *
    *    path:                 new path:
    *
    *           2                     4
    *    O--O------O--O        5 .O------O. 3
    *    |            |         /          \
    *  3 O            O 1    6 O            O 2
    *    |            |      7 .\          /
    *    O--O------O--O        ...O------O. 1
    *           0                     0
    */

    // TODO: not sure if we should only touch flat segment joins as the original algorithm

    m_path->clear();

    int subpathCount = m_copy->subpathCount();
    for (int subpathIndex = 0; subpathIndex < subpathCount; ++subpathIndex) {
        int pointCount = m_copy->subpathPointCount(subpathIndex);
        if (! pointCount)
            continue;

        // check if we have sufficient number of points
        if (pointCount < 3) {
            // copy the only segment
            KoPathSegment s = m_copy->segmentByIndex(KoPathPointIndex(subpathIndex, 0));
            m_path->moveTo(m_copy->pointByIndex(KoPathPointIndex(subpathIndex, 0))->point());
            addSegment(m_path, s);

            continue;
        }

        KoPathSegment prevSeg = m_copy->segmentByIndex(KoPathPointIndex(subpathIndex, pointCount - 1));
        KoPathSegment nextSeg = m_copy->segmentByIndex(KoPathPointIndex(subpathIndex, 0));
        KoPathSegment lastSeg;

        KoPathPoint * currPoint = nextSeg.first();
        KoPathPoint * firstPoint = 0;
        KoPathPoint * lastPoint = 0;

        // check if first path point is a smooth join with the closing segment
        bool firstPointIsCorner = m_copy->isClosedSubpath(subpathIndex)
                                  && ! currPoint->isSmooth(prevSeg.first(), nextSeg.second());

        // Begin: take care of the first path point
        if (firstPointIsCorner) {
            // split the previous segment at length - radius
            qreal prevLength = prevSeg.length();
            qreal prevSplit = prevLength > m_radius ? prevSeg.paramAtLength(prevLength - m_radius) : 0.5;
            QPair<KoPathSegment, KoPathSegment> prevParts = prevSeg.splitAt(prevSplit);

            // split the next segment at radius
            qreal nextLength = nextSeg.length();
            qreal nextSplit = nextLength > m_radius ? nextSeg.paramAtLength(m_radius) : 0.5;
            QPair<KoPathSegment, KoPathSegment> nextParts = nextSeg.splitAt(nextSplit);

            // calculate smooth tangents
            QPointF P0 = prevParts.first.second()->point();
            QPointF P3 = nextParts.first.second()->point();
            qreal tangentLength1 = 0.5 * QLineF(P0, currPoint->point()).length();
            qreal tangentLength2 = 0.5 * QLineF(P3, currPoint->point()).length();
            QPointF P1 = P0 - tangentLength1 * tangentAtEnd(prevParts.first);
            QPointF P2 = P3 - tangentLength2 * tangentAtStart(nextParts.second);

            // start the subpath
            firstPoint = m_path->moveTo(prevParts.second.first()->point());
            // connect the split points with curve
            // TODO: shall we create a correct arc?
            m_path->curveTo(P1, P2, P3);

            prevSeg = nextParts.second;
            lastSeg = prevParts.first;
        } else {
            firstPoint = m_path->moveTo(currPoint->point());
            prevSeg = nextSeg;
        }

        // Loop:
        for (int pointIndex = 1; pointIndex < pointCount; ++pointIndex) {
            nextSeg = m_copy->segmentByIndex(KoPathPointIndex(subpathIndex, pointIndex));
            if (! nextSeg.isValid())
                break;

            currPoint = nextSeg.first();
            if (! currPoint)
                continue;

            if (currPoint->isSmooth(prevSeg.first(), nextSeg.second())) {
                // the current point has a smooth join, so we can add the previous segment
                // to our new path
                addSegment(m_path, prevSeg);
                prevSeg = nextSeg;
            } else {
                // split the previous segment at length - radius
                qreal prevLength = prevSeg.length();
                qreal prevSplit = prevLength > m_radius ? prevSeg.paramAtLength(prevLength - m_radius) : 0.5;
                QPair<KoPathSegment, KoPathSegment> prevParts = prevSeg.splitAt(prevSplit);

                // add the remaining part up to the split point of the pervious segment
                lastPoint = addSegment(m_path, prevParts.first);

                // split the next segment at radius
                qreal nextLength = nextSeg.length();
                qreal nextSplit = nextLength > m_radius ? nextSeg.paramAtLength(m_radius) : 0.5;
                QPair<KoPathSegment, KoPathSegment> nextParts = nextSeg.splitAt(nextSplit);

                // calculate smooth tangents
                QPointF P0 = prevParts.first.second()->point();
                QPointF P3 = nextParts.first.second()->point();
                qreal tangentLength1 = 0.5 * QLineF(P0, currPoint->point()).length();
                qreal tangentLength2 = 0.5 * QLineF(P3, currPoint->point()).length();
                QPointF P1 = P0 - tangentLength1 * tangentAtEnd(prevParts.first);
                QPointF P2 = P3 - tangentLength2 * tangentAtStart(nextParts.second);

                // connect the split points with curve
                // TODO: shall we create a correct arc?
                lastPoint = m_path->curveTo(P1, P2, P3);

                prevSeg = nextParts.second;
            }

        }

        // End: take care of the last path point
        if (firstPointIsCorner) {
            // construct the closing segment
            lastPoint->setProperty(KoPathPoint::CloseSubpath);
            firstPoint->setProperty(KoPathPoint::CloseSubpath);
            switch (lastSeg.degree()) {
            case 1:
                lastPoint->removeControlPoint2();
                firstPoint->removeControlPoint1();
                break;
            case 2:
                if (lastSeg.first()->activeControlPoint2()) {
                    lastPoint->setControlPoint2(lastSeg.first()->controlPoint2());
                    firstPoint->removeControlPoint1();
                } else {
                    lastPoint->removeControlPoint2();
                    firstPoint->setControlPoint1(lastSeg.second()->controlPoint1());
                }
                break;
            case 3:
                lastPoint->setControlPoint2(lastSeg.first()->controlPoint2());
                firstPoint->setControlPoint1(lastSeg.second()->controlPoint1());
                break;
            }
        } else {
            // add the last remaining segment
            addSegment(m_path, prevSeg);
        }
    }
}

KoPathPoint * RoundCornersCommand::addSegment(KoPathShape * p, KoPathSegment & s)
{
    switch (s.degree()) {
    case 1:
        return p->lineTo(s.second()->point());
        break;
    case 2:
        if (s.first()->activeControlPoint2())
            return p->curveTo(s.first()->controlPoint2(), s.second()->point());
        else
            return p->curveTo(s.second()->controlPoint1(), s.second()->point());
        break;
    case 3:
        return p->curveTo(s.first()->controlPoint2(),
                          s.second()->controlPoint1(),
                          s.second()->point());
        break;
    }
    return 0;
}

void RoundCornersCommand::copyPath(KoPathShape * dst, KoPathShape * src)
{
    dst->clear();

    int subpathCount = src->subpathCount();
    for (int subpathIndex = 0; subpathIndex < subpathCount; ++subpathIndex) {
        int pointCount = src->subpathPointCount(subpathIndex);
        if (! pointCount)
            continue;

        KoSubpath * subpath = new KoSubpath;
        for (int pointIndex = 0; pointIndex < pointCount; ++pointIndex) {
            KoPathPoint * p = src->pointByIndex(KoPathPointIndex(subpathIndex, pointIndex));
            KoPathPoint * c = new KoPathPoint(*p);
            c->setParent(dst);
            subpath->append(c);
        }
        dst->addSubpath(subpath, subpathIndex);
    }
    dst->setTransformation(src->transformation());
}

QPointF RoundCornersCommand::tangentAtStart(const KoPathSegment &s)
{
    QList<QPointF> cp = s.controlPoints();
    QPointF tn = cp[1] - cp.first();
    qreal length = sqrt(tn.x() * tn.x() + tn.y() * tn.y());
    return tn / length;
}

QPointF RoundCornersCommand::tangentAtEnd(const KoPathSegment &s)
{
    QList<QPointF> cp = s.controlPoints();
    QPointF tn = cp[cp.count()-2] - cp.last();
    qreal length = sqrt(tn.x() * tn.x() + tn.y() * tn.y());
    return tn / length;
}
