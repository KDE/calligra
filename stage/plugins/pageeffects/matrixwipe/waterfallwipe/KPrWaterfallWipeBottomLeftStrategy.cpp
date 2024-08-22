/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrWaterfallWipeBottomLeftStrategy.h"
#include "KPrWaterfallWipeEffectFactory.h"

KPrWaterfallWipeBottomLeftStrategy::KPrWaterfallWipeBottomLeftStrategy(Direction direction)
    : KPrMatrixWipeStrategy(direction == BottomToTop ? KPrWaterfallWipeEffectFactory::BottomLeftVertical : KPrWaterfallWipeEffectFactory::BottomLeftHorizontal,
                            "waterfallWipe",
                            direction == BottomToTop ? "verticalRight" : "horizontalRight",
                            true,
                            true)
    , m_direction(direction)
{
}

KPrWaterfallWipeBottomLeftStrategy::~KPrWaterfallWipeBottomLeftStrategy() = default;

int KPrWaterfallWipeBottomLeftStrategy::squareIndex(int x, int y, int columns, int rows)
{
    Q_UNUSED(columns);
    return x + (rows - y - 1);
}

KPrMatrixWipeStrategy::Direction KPrWaterfallWipeBottomLeftStrategy::squareDirection(int x, int y, int columns, int rows)
{
    Q_UNUSED(x);
    Q_UNUSED(y);
    Q_UNUSED(columns);
    Q_UNUSED(rows);

    return m_direction;
}

int KPrWaterfallWipeBottomLeftStrategy::maxIndex(int columns, int rows)
{
    return columns + rows;
}
