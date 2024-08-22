/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2006 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef SHAPEMOVESTRATEGY_H
#define SHAPEMOVESTRATEGY_H

#include <KoInteractionStrategy.h>

#include <QList>
#include <QPointF>
#include <QVector>

class KoCanvasBase;
class KoToolBase;
class KoShape;

/**
 * Implements the Move action on an object or selected objects.
 */
class ShapeMoveStrategy : public KoInteractionStrategy
{
public:
    /**
     * Constructor that starts to move the objects.
     * @param tool the parent tool which controls this strategy
     * @param canvas the canvas interface which will supply things like a selection object
     * @param clicked the initial point that the user depressed (in pt).
     */
    ShapeMoveStrategy(KoToolBase *tool, const QPointF &clicked);
    ~ShapeMoveStrategy() override = default;

    void handleMouseMove(const QPointF &mouseLocation, Qt::KeyboardModifiers modifiers) override;
    KUndo2Command *createCommand() override;
    void finishInteraction(Qt::KeyboardModifiers modifiers) override;
    void paint(QPainter &painter, const KoViewConverter &converter) override;
    void handleCustomEvent(KoPointerEvent *event) override;

private:
    void moveSelection();
    QVector<QPointF> m_previousPositions;
    QVector<QPointF> m_newPositions;
    QVector<QPointF> m_previousOffsets;
    QVector<QPointF> m_newOffsets;
    QPointF m_start, m_diff, m_initialSelectionPosition, m_initialOffset;
    QList<KoShape *> m_selectedShapes;
    KoCanvasBase *m_canvas;
    bool m_firstMove;
};

#endif
