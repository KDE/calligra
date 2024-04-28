/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "MoveStartOffsetStrategy.h"
#include "ArtisticTextShape.h"
#include "ChangeTextOffsetCommand.h"
#include <KoPathSegment.h>
#include <KoPathShape.h>
#include <KoToolBase.h>
#include <math.h>

// helper function to calculate the squared distance between two points
qreal squaredDistance(const QPointF &p1, const QPointF &p2)
{
    qreal dx = p1.x() - p2.x();
    qreal dy = p1.y() - p2.y();
    return dx * dx + dy * dy;
}

MoveStartOffsetStrategy::MoveStartOffsetStrategy(KoToolBase *tool, ArtisticTextShape *text)
    : KoInteractionStrategy(tool)
    , m_text(text)
{
    m_oldStartOffset = m_text->startOffset();
    m_baselineShape = KoPathShape::createShapeFromPainterPath(m_text->baseline());
    // cache number of segments and their length
    const int subpathCount = m_baselineShape->subpathCount();
    for (int i = 0; i < subpathCount; ++i) {
        const int subpathPointCount = m_baselineShape->subpathPointCount(i);
        for (int j = 0; j < subpathPointCount; ++j) {
            KoPathSegment s = m_baselineShape->segmentByIndex(KoPathPointIndex(i, j));
            if (s.isValid()) {
                const qreal length = s.length();
                m_segmentLengths.append(length);
                m_totalLength += length;
            }
        }
    }
}

MoveStartOffsetStrategy::~MoveStartOffsetStrategy()
{
    delete m_baselineShape;
}

void MoveStartOffsetStrategy::handleMouseMove(const QPointF &mouseLocation, Qt::KeyboardModifiers /*modifiers*/)
{
    // map the global mouse position to local coordinates of our baseline path
    const QPointF localMousePoint = m_baselineShape->transformation().inverted().map(mouseLocation);

    // create a roi to check segments at
    QRectF grabRect;
    grabRect.setHeight(2 * grabSensitivity());
    grabRect.setWidth(2 * grabSensitivity());
    grabRect.moveCenter(localMousePoint);

    // get all segments intersecting our roi
    QList<KoPathSegment> segments = m_baselineShape->segmentsAt(grabRect);

    // now find the nearest point of all picked segments
    KoPathSegment nearestSegment;
    qreal nearestPointParam = 0.0;
    KoPathPointIndex nearestPathPoint;
    qreal minDistance = HUGE_VAL;
    foreach (const KoPathSegment &s, segments) {
        qreal t = s.nearestPoint(localMousePoint);
        qreal distance = squaredDistance(localMousePoint, s.pointAt(t));
        if (distance < minDistance) {
            nearestPointParam = t;
            nearestSegment = s;
            nearestPathPoint = m_baselineShape->pathPointIndex(s.first());
            minDistance = distance;
        }
    }

    // now we need to map back to the length of the baseline path
    if (nearestSegment.isValid()) {
        // count number of path segments
        int segmentCount = 0;
        int nearestSegment = 0;
        const int subpathCount = m_baselineShape->subpathCount();
        for (int i = 0; i < subpathCount; ++i) {
            const int subpathPointCount = m_baselineShape->subpathPointCount(i);
            if (i == nearestPathPoint.first) {
                nearestSegment = segmentCount + nearestPathPoint.second;
            }
            segmentCount += m_baselineShape->isClosedSubpath(i) ? subpathPointCount : subpathPointCount - 1;
        }
        qreal length = 0.0;
        for (int i = 0; i < nearestSegment; ++i) {
            length += m_segmentLengths[i];
        }
        length += nearestPointParam * m_segmentLengths[nearestSegment];
        tool()->repaintDecorations();
        m_text->setStartOffset(length / m_totalLength);
        tool()->repaintDecorations();
    }
}

KUndo2Command *MoveStartOffsetStrategy::createCommand()
{
    return new ChangeTextOffsetCommand(m_text, m_oldStartOffset, m_text->startOffset());
}

void MoveStartOffsetStrategy::finishInteraction(Qt::KeyboardModifiers /*modifiers*/)
{
}
