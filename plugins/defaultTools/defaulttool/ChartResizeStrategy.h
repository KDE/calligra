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

#ifndef CHARTRESIZESTRATEGY_H
#define CHARTRESIZESTRATEGY_H

#include <QPointF>
#include <QSizeF>
#include <QList>
#include <QVector>
#include <QTransform>

class KUndo2Command;
class KoShape;
class KoShapeContainer;

/**
 * Implements the resize action for the contents of a chart shape.
 * In general the plot area is resized, while other (text) shapes
 * are moved if necessary.
 */
class ChartResizeStrategy
{
public:
    /// Constructs a ChartResizeStrategy for @p shape
    /// The @p shape must be a ChartShape.
    explicit ChartResizeStrategy(KoShape *shape);
    /// Destructor
    ~ChartResizeStrategy() {}

    /// Relayouts the contents of the chart shape
    /// @p startSize is the start size of the chart shape,
    /// @p scaleX and @p scaleY is the scaling factors
    /// the chart shape will be scaled with.
    void setSize(const QSizeF &startSize, qreal scaleX, qreal scaleY);
    /// Creates the undo commands needed and adds them to @p cmd
    void createCommand(KUndo2Command *cmd);

private:
    KoShapeContainer *m_chart;
    // Plot area is resized
    KoShape *m_plotArea;
    QSizeF m_plotAreaStartSize;
    // All other shapes are moved
    QVector<QPointF> m_startPositions;
    QVector<KoShape*> m_shapes;
    // used for layouting
    QVector<KoShape*> m_overlapX;
    QVector<KoShape*> m_rightX;
    QVector<KoShape*> m_overlapY;
    QVector<KoShape*> m_belowY;
};

#endif
