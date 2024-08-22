/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrWaterfallWipeTopRightStrategy.h"
#include "KPrWaterfallWipeEffectFactory.h"

KPrWaterfallWipeTopRightStrategy::KPrWaterfallWipeTopRightStrategy(Direction direction)
    : KPrMatrixWipeStrategy(direction == TopToBottom ? KPrWaterfallWipeEffectFactory::TopRightVertical : KPrWaterfallWipeEffectFactory::TopRightHorizontal,
                            "waterfallWipe",
                            direction == TopToBottom ? "verticalRight" : "horizontalRight",
                            false,
                            true)
    , m_direction(direction)
{
}

KPrWaterfallWipeTopRightStrategy::~KPrWaterfallWipeTopRightStrategy() = default;

int KPrWaterfallWipeTopRightStrategy::squareIndex(int x, int y, int columns, int rows)
{
    Q_UNUSED(columns);
    Q_UNUSED(rows);
    return (columns - x - 1) + y;
}

KPrMatrixWipeStrategy::Direction KPrWaterfallWipeTopRightStrategy::squareDirection(int x, int y, int columns, int rows)
{
    Q_UNUSED(x);
    Q_UNUSED(y);
    Q_UNUSED(columns);
    Q_UNUSED(rows);
    return m_direction;
}

int KPrWaterfallWipeTopRightStrategy::maxIndex(int columns, int rows)
{
    return columns + rows;
}
