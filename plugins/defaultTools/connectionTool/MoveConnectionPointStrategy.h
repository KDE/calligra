/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef MOVECONNECTIONPOINTSTRATEGY_H
#define MOVECONNECTIONPOINTSTRATEGY_H

#include <KoConnectionPoint.h>
#include <KoInteractionStrategy.h>
#include <QPointF>

class KoShape;

class MoveConnectionPointStrategy : public KoInteractionStrategy
{
public:
    /// Constructor
    MoveConnectionPointStrategy(KoShape *shape, int connectionPointId, KoToolBase *parent);
    /// Destructor
    ~MoveConnectionPointStrategy() override;

    /// reimplemented from KoInteractionStrategy
    void paint(QPainter &painter, const KoViewConverter &converter) override;

    /// reimplemented from KoInteractionStrategy
    void handleMouseMove(const QPointF &mouseLocation, Qt::KeyboardModifiers modifiers) override;

    /// reimplemented from KoInteractionStrategy
    KUndo2Command *createCommand() override;

    /// reimplemented from KoInteractionStrategy
    void cancelInteraction() override;

    /// reimplemented from KoInteractionStrategy
    void finishInteraction(Qt::KeyboardModifiers modifiers) override;

private:
    KoShape *m_shape;
    int m_connectionPointId;
    KoConnectionPoint m_oldPoint;
    KoConnectionPoint m_newPoint;
};

#endif // MOVECONNECTIONPOINTSTRATEGY_H
