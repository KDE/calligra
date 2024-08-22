/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrWaterfallWipeTopLeftStrategy.h"
#include "KPrWaterfallWipeEffectFactory.h"

KPrWaterfallWipeTopLeftStrategy::KPrWaterfallWipeTopLeftStrategy(Direction direction)
    : KPrMatrixWipeStrategy(direction == TopToBottom ? KPrWaterfallWipeEffectFactory::TopLeftVertical : KPrWaterfallWipeEffectFactory::TopLeftHorizontal,
                            "waterfallWipe",
                            direction == TopToBottom ? "verticalLeft" : "horizontalLeft",
                            false,
                            true)
    , m_direction(direction)
{
}

KPrWaterfallWipeTopLeftStrategy::~KPrWaterfallWipeTopLeftStrategy() = default;

int KPrWaterfallWipeTopLeftStrategy::squareIndex(int x, int y, int columns, int rows)
{
    Q_UNUSED(columns);
    Q_UNUSED(rows);
    return x + y;
}

KPrMatrixWipeStrategy::Direction KPrWaterfallWipeTopLeftStrategy::squareDirection(int x, int y, int columns, int rows)
{
    Q_UNUSED(x);
    Q_UNUSED(y);
    Q_UNUSED(columns);
    Q_UNUSED(rows);
    return m_direction;
}

int KPrWaterfallWipeTopLeftStrategy::maxIndex(int columns, int rows)
{
    return columns + rows;
}
