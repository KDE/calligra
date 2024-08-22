/* This file is part of the KDE project
 *
 *   SPDX-FileCopyrightText: 2017 Dag Andersen <danders@get2net.dk>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef CHARTRESIZESTRATEGY_H
#define CHARTRESIZESTRATEGY_H

#include <QList>
#include <QPointF>
#include <QSizeF>
#include <QTransform>
#include <QVector>

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
    ~ChartResizeStrategy() = default;

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
    QVector<KoShape *> m_shapes;
    // used for layouting
    QVector<KoShape *> m_overlapX;
    QVector<KoShape *> m_rightX;
    QVector<KoShape *> m_overlapY;
    QVector<KoShape *> m_belowY;
};

#endif
