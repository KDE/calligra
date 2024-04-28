/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef SHAPERESIZESTRATEGY_H
#define SHAPERESIZESTRATEGY_H

#include <KoFlake.h>
#include <KoInteractionStrategy.h>

#include <QList>
#include <QMap>
#include <QPointF>
#include <QTransform>

class KoToolBase;
class KoShape;

class ChartResizeStrategy;

/**
 * A strategy for the KoInteractionTool.
 * This strategy is invoked when the user starts a resize of a selection of objects,
 * the strategy will then resize the objects interactively and provide a command afterwards.
 */
class ShapeResizeStrategy : public KoInteractionStrategy
{
public:
    /**
     * Constructor
     */
    ShapeResizeStrategy(KoToolBase *tool, const QPointF &clicked, KoFlake::SelectionHandle direction);
    ~ShapeResizeStrategy() override;

    void handleMouseMove(const QPointF &mouseLocation, Qt::KeyboardModifiers modifiers) override;
    KUndo2Command *createCommand() override;
    void finishInteraction(Qt::KeyboardModifiers modifiers) override;
    void paint(QPainter &painter, const KoViewConverter &converter) override;
    void handleCustomEvent(KoPointerEvent *event) override;

private:
    void resizeBy(const QPointF &center, qreal zoomX, qreal zoomY);

    QPointF m_start;
    QVector<QPointF> m_startPositions;
    QVector<QSizeF> m_startSizes;
    bool m_top, m_left, m_bottom, m_right;
    QTransform m_unwindMatrix, m_windMatrix;
    QSizeF m_initialSize;
    QPointF m_initialPosition;
    QTransform m_scaleMatrix;
    QVector<QTransform> m_oldTransforms;
    QVector<QTransform> m_transformations;
    QPointF m_lastScale;
    QList<KoShape *> m_selectedShapes;
    QMap<KoShape *, ChartResizeStrategy *> m_chartShapes;
};

#endif
