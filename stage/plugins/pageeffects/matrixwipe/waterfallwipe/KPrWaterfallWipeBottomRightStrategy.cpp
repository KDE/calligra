/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrWaterfallWipeBottomRightStrategy.h"
#include "KPrWaterfallWipeEffectFactory.h"

KPrWaterfallWipeBottomRightStrategy::KPrWaterfallWipeBottomRightStrategy(Direction direction)
    : KPrMatrixWipeStrategy(direction == BottomToTop ? KPrWaterfallWipeEffectFactory::BottomRightVertical
                                                     : KPrWaterfallWipeEffectFactory::BottomRightHorizontal,
                            "waterfallWipe",
                            direction == BottomToTop ? "verticalLeft" : "horizontalLeft",
                            true,
                            true)
    , m_direction(direction)
{
}

KPrWaterfallWipeBottomRightStrategy::~KPrWaterfallWipeBottomRightStrategy() = default;

int KPrWaterfallWipeBottomRightStrategy::squareIndex(int x, int y, int columns, int rows)
{
    return (columns - x - 1) + (rows - y - 1);
}

KPrMatrixWipeStrategy::Direction KPrWaterfallWipeBottomRightStrategy::squareDirection(int x, int y, int columns, int rows)
{
    Q_UNUSED(x);
    Q_UNUSED(y);
    Q_UNUSED(columns);
    Q_UNUSED(rows);
    return m_direction;
}

int KPrWaterfallWipeBottomRightStrategy::maxIndex(int columns, int rows)
{
    return columns + rows;
}
