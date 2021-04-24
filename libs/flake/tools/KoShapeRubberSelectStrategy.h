/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2006 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOSHAPERUBBERSELECTSTRATEGY_H
#define KOSHAPERUBBERSELECTSTRATEGY_H

#include "KoInteractionStrategy.h"

#include <QRectF>

#include "flake_export.h"

class KoToolBase;
class KoShapeRubberSelectStrategyPrivate;

/**
 * Implement the rubber band selection of flake objects.
 */
class FLAKE_EXPORT KoShapeRubberSelectStrategy : public KoInteractionStrategy
{
public:
    /**
     * Constructor that initiates the rubber select.
     * A rubber select is basically rectangle area that the user drags out
     * from @p clicked to a point later provided in the handleMouseMove() continuously
     * showing a semi-transparent 'rubber-mat' over the objects it is about to select.
     * @param tool the parent tool which controls this strategy
     * @param clicked the initial point that the user depressed (in pt).
     * @param useSnapToGrid use the snap-to-grid settings while doing the rubberstamp.
     */
    KoShapeRubberSelectStrategy(KoToolBase *tool, const QPointF &clicked, bool useSnapToGrid = false);

    void paint(QPainter &painter, const KoViewConverter &converter) override;
    void handleMouseMove(const QPointF &mouseLocation, Qt::KeyboardModifiers modifiers) override;
    KUndo2Command *createCommand() override;
    void finishInteraction(Qt::KeyboardModifiers modifiers) override;

protected:
    /// constructor
    KoShapeRubberSelectStrategy(KoShapeRubberSelectStrategyPrivate &);

private:
    Q_DECLARE_PRIVATE(KoShapeRubberSelectStrategy)
};

#endif /* KOSHAPERUBBERSELECTSTRATEGY_H */
