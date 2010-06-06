/* This file is part of the KDE project
 * Copyright (C) 2006-2007, 2010 Thomas Zander <zander@kde.org>
 * Copyright (C) 2010 Ko Gmbh <casper.boemann@kogmbh.com>
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

#include "Outline.h"
#include "KWFrame.h"
#include "KWOutlineShape.h"
#include <KoShapeContainer.h>

#include <qnumeric.h>

Outline::Outline(KWFrame *frame, const QMatrix &matrix)
    : m_side(None),
    m_shape(frame->shape())
{
    KoShape *shape = frame->outlineShape();
    if (shape == 0)
        shape = frame->shape();
    QPainterPath path = shape->outline();
    if (frame->shape()->parent() && frame->shape()->parent()->isClipped(frame->shape())) {
        path = shape->transformation().map(path);
        path = frame->shape()->parent()->outline().intersected(path);
        path = shape->transformation().inverted().map(path);
    }

    init(matrix, path, frame->runAroundDistance());
    if (frame->textRunAround() == KWord::NoRunAround)
        m_side = Empty;
    else if (frame->runAroundSide() == KWord::LeftRunAroundSide)
        m_side = Right;
    else if (frame->runAroundSide() == KWord::RightRunAroundSide)
        m_side = Left;
    else if (frame->runAroundSide() == KWord::BothRunAroundSide)
        m_side = None;
}

Outline::Outline(KoShape *shape, const QMatrix &matrix)
    : m_side(None),
    m_shape(shape)
{
    KWFrame *frame = dynamic_cast<KWFrame*>(shape->applicationData());
    qreal distance = 0;
    if (frame) {
        distance = frame->runAroundDistance();
        if (frame->textRunAround() == KWord::NoRunAround) {
            // make the shape take the full width of the text area
            m_side = Empty;
        } else if (frame->textRunAround() == KWord::RunThrough) {
            // We don't exist.
            return;
        }
    }

    init(matrix, shape->outline(), distance);
}

void Outline::init(const QMatrix &matrix, const QPainterPath &outline, qreal distance)
{
    m_distance = distance;
    QPainterPath path =  matrix.map(outline);
    m_bounds = path.boundingRect();
    if (distance >= 0.0) {
        QMatrix grow = matrix;
        grow.translate(m_bounds.width() / 2.0, m_bounds.height() / 2.0);
        qreal scaleX = distance;
        if (m_bounds.width() > 0)
            scaleX = (m_bounds.width() + distance) / m_bounds.width();
        qreal scaleY = distance;
        if (m_bounds.height() > 0)
            scaleY = (m_bounds.height() + distance) / m_bounds.height();
        Q_ASSERT(!qIsNaN(scaleY));
        Q_ASSERT(!qIsNaN(scaleX));
        grow.scale(scaleX, scaleY);
        grow.translate(-m_bounds.width() / 2.0, -m_bounds.height() / 2.0);

        path =  grow.map(outline);
        // kDebug() <<"Grow" << distance <<", Before:" << m_bounds <<", after:" << path.boundingRect();
        m_bounds = path.boundingRect();
    }

    QPolygonF poly = path.toFillPolygon();

    QPointF prev = *(poly.begin());
    foreach (const QPointF &vtx, poly) { //initialized edges
        if (vtx.x() == prev.x() && vtx.y() == prev.y())
            continue;
        QLineF line;
        if (prev.y() < vtx.y()) // Make sure the vector lines all point downwards.
            line = QLineF(prev, vtx);
        else
            line = QLineF(vtx, prev);
        m_edges.insert(line.y1(), line);
        prev = vtx;
    }
}

QRectF Outline::limit(const QRectF &content)
{
    if (m_side == Empty) {
        if (content.intersects(m_bounds))
            return QRectF();
        return content;
    }

    if (m_side == None) { // first time for this text;
        qreal insetLeft = m_bounds.right() - content.left();
        qreal insetRight = content.right() - m_bounds.left();

        if (insetLeft < insetRight)
            m_side = Left;
        else
            m_side = Right;
    }
    if (!m_bounds.intersects(content))
        return content;

    // two points, as we are checking a rect, not a line.
    qreal points[2] = { content.top(), content.bottom() };
    QRectF answer = content;
    for (int i = 0; i < 2; i++) {
        const qreal y = points[i];
        qreal x = m_side == Left ? answer.left() : answer.right();
        bool first = true;
        QMap<qreal, QLineF>::const_iterator iter = m_edges.constBegin();
        for (;iter != m_edges.constEnd(); ++iter) {
            QLineF line = iter.value();
            if (line.y2() < y) // not a section that will intersect with ou Y yet
                continue;
            if (line.y1() > y) // section is below our Y, so abort loop
                break;
            if (qAbs(line.dy()) < 1E-10)  // horizontal lines don't concern us.
                continue;

            qreal intersect = xAtY(iter.value(), y);
            if (first) {
                x = intersect;
                first = false;
            } else if ((m_side == Left && intersect > x) || (m_side == Right && intersect < x)) {
                x = intersect;
            }
        }
        if (m_side == Left)
            answer.setLeft(qMax(answer.left(), x));
        else
            answer.setRight(qMin(answer.right(), x));
    }

    return answer;
}

qreal Outline::xAtY(const QLineF &line, qreal y)
{
    if (line.dx() == 0)
        return line.x1();
    return line.x1() + (y - line.y1()) / line.dy() * line.dx();
}

void Outline::changeMatrix(const QMatrix &matrix)
{
    m_edges.clear();
    init(matrix, m_shape->outline(), m_distance);
}
