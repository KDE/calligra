/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef SHAPEROTATESTRATEGY_H
#define SHAPEROTATESTRATEGY_H

#include <KoInteractionStrategy.h>

#include <QList>
#include <QPointF>
#include <QRectF>
#include <QTransform>

class KoToolBase;
class KoShape;

/**
 * A strategy for the KoInteractionTool.
 * This strategy is invoked when the user starts a rotate of a selection of objects,
 * the strategy will then rotate the objects interactively and provide a command afterwards.
 */
class ShapeRotateStrategy : public KoInteractionStrategy
{
public:
    /**
     * Constructor that starts to rotate the objects.
     * @param tool the parent tool which controls this strategy
     * @param clicked the initial point that the user depressed (in pt).
     */
    ShapeRotateStrategy(KoToolBase *tool, const QPointF &clicked, Qt::MouseButtons buttons);
    ~ShapeRotateStrategy() override = default;

    void handleMouseMove(const QPointF &mouseLocation, Qt::KeyboardModifiers modifiers) override;
    KUndo2Command *createCommand() override;
    void finishInteraction(Qt::KeyboardModifiers modifiers) override
    {
        Q_UNUSED(modifiers);
    }
    void paint(QPainter &painter, const KoViewConverter &converter) override;
    void handleCustomEvent(KoPointerEvent *event) override;

private:
    void rotateBy(qreal angle);
    QRectF m_initialBoundingRect;
    QPointF m_start;
    QTransform m_rotationMatrix;
    QTransform m_initialSelectionMatrix;
    QVector<QTransform> m_oldTransforms;
    QPointF m_rotationCenter;
    QList<KoShape *> m_selectedShapes;
};

#endif
