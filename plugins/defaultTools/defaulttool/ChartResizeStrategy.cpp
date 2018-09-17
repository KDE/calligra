/* This file is part of the KDE project
 * 
 *   Copyright (C) 2017 Dag Andersen <danders@get2net.dk>
 * 
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Library General Public
 *   License as published by the Free Software Foundation; either
 *   version 2 of the License, or (at your option) any later version.
 * 
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Library General Public License for more details.
 * 
 *   You should have received a copy of the GNU Library General Public License
 *   along with this library; see the file COPYING.LIB.  If not, write to
 *   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *   Boston, MA 02110-1301, USA.
 */

#include "ChartResizeStrategy.h"

#include <KoShape.h>
#include <KoShapeContainer.h>
#include <KoCanvasBase.h>
#include <KoShapeManager.h>
#include <KoShapeContainerModel.h>
#include <commands/KoShapeMoveCommand.h>
#include <commands/KoShapeSizeCommand.h>
#include <klocalizedstring.h>


ChartResizeStrategy::ChartResizeStrategy(KoShape *shape)
    : m_chart(dynamic_cast<KoShapeContainer*>(shape))
    , m_plotArea(0)
{
    if (m_chart) {
        QRectF rect = m_chart->boundingRect();
        QPointF middle((rect.right() - rect.left()) * 0.5, (rect.bottom() - rect.top()) * 0.5);
        for (KoShape *shape : m_chart->shapes()) {
            if (!shape->isVisible()) {
                continue;
            }
            if (shape->shapeId() == "ChartShapePlotArea") {
                // Plot area shall be resized
                m_plotArea = shape;
                m_plotAreaStartSize = shape->size();
                rect = shape->boundingRect();
                middle = QPointF(rect.left() + ((rect.right() - rect.left()) * 0.5), rect.top() + ((rect.bottom() - rect.top()) * 0.5));
            } else {
                m_shapes << shape;
                m_startPositions << shape->position();
            }
        }
        // NOTE: We use plot area middle point for positioning all other shapes
        // This is correct for axis titles/legend,
        // but the others should have used the *chart* middle.
        // As we do not know which is which and it works ok in "normal" situations
        // we go for this atm.
        for (KoShape *shape : m_shapes) {
            QRectF r = shape->boundingRect();
            if (r.left() > middle.x()) {
                // to the right
                m_rightX << shape;
            } else if (r.right() >= middle.x()) {
                // overlaps
                m_overlapX << shape;
            }
            if (r.top() > middle.y()) {
                // below
                m_belowY << shape;
            } else if (r.bottom() >= middle.y()) {
                // overlaps
                m_overlapY << shape;
            }
        }
    }
}

void ChartResizeStrategy::setSize(const QSizeF &startSize, qreal scaleX, qreal scaleY)
{
    if (!m_chart) {
        return;
    }
    qreal diffX = (startSize.width() * scaleX) - startSize.width();
    qreal diffY = (startSize.height() * scaleY) - startSize.height();
    if (m_plotArea) {
        // Resize plot area the same as the chart,
        // it will leave the same amount of free space as before to be used by other shapes.
        // All other components are just moved and do not take more/less space.
        QSizeF plotAreaSize = QSizeF(m_plotAreaStartSize.width() + diffX, m_plotAreaStartSize.height() + diffY);
        m_plotArea->setSize(plotAreaSize);
    }
    for (int i = 0; i < m_shapes.count(); ++i) {
        KoShape *shape = m_shapes.at(i);
        QPointF startPos = m_startPositions.at(i);
        QPointF diff; // default: keep distance to top/left
        if (m_rightX.contains(shape)) {
            // keep distance to right edge
            diff.setX(diffX);
        } else if (m_overlapX.contains(shape)) {
            // proportional move, could be improved but works pretty well ;)
            diff.setX(diffX * 0.5);
        }
        if (m_belowY.contains(shape)) {
            // keep distance to bottom edge
            diff.setY(diffY);
        } else if (m_overlapY.contains(shape)) {
            // proportional move, could be improved but works pretty well ;)
            diff.setY(diffY * 0.5);
        }
        shape->update();
        shape->setPosition(startPos + diff);
        shape->update();
    }
}

void ChartResizeStrategy::createCommand(KUndo2Command *cmd)
{
    if (!m_chart) {
        return;
    }
    // get the shapes that has actually been moved
    QVector<QPointF> oldPositions;
    QVector<QPointF> newPositions;
    QList<KoShape*> movedShapes;
    for (int i = 0; i < m_shapes.count(); ++i) {
        KoShape *shape = m_shapes.at(i);
        QPointF pos = shape->position();
        if (pos != m_startPositions.at(i)) {
            movedShapes << shape;
            oldPositions << m_startPositions.at(i);
            newPositions << pos;
        }
    }
    if (!movedShapes.isEmpty()) {
        new KoShapeMoveCommand(movedShapes, oldPositions, newPositions, cmd);
    }
    if (m_plotArea) {
        new KoShapeSizeCommand(QList<KoShape*>()<<m_plotArea, QVector<QSizeF>()<<m_plotAreaStartSize, QVector<QSizeF>()<<m_plotArea->size(), cmd );
    }
}
