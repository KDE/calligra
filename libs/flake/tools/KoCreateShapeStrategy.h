/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2006 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOCREATESHAPESTRATEGY_H
#define KOCREATESHAPESTRATEGY_H

#include "KoShapeRubberSelectStrategy.h"

#include <QPointF>
#include <QPainterPath>

class KoCreateShapesTool;

/**
 * A strategy for the KoCreateShapesTool.
 */
class KoCreateShapeStrategy : public KoShapeRubberSelectStrategy
{
public:
    /**
     * Constructor that starts to create a new shape.
     * @param tool the parent tool which controls this strategy
     * @param clicked the initial point that the user depressed (in pt).
     */
    KoCreateShapeStrategy(KoCreateShapesTool *tool, const QPointF &clicked);
    ~KoCreateShapeStrategy() override {}

    void finishInteraction(Qt::KeyboardModifiers modifiers) override;
    KUndo2Command* createCommand() override;
    void paint(QPainter &painter, const KoViewConverter &converter) override;
    void handleMouseMove(const QPointF &mouseLocation, Qt::KeyboardModifiers modifiers) override;

private:
    QPainterPath m_outline;
    QRectF m_outlineBoundingRect;
    Q_DECLARE_PRIVATE(KoShapeRubberSelectStrategy)
};

#endif

