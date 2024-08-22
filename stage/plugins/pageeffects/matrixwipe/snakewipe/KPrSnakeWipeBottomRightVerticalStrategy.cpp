/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrSnakeWipeBottomRightVerticalStrategy.h"
#include "KPrSnakeWipeEffectFactory.h"

KPrSnakeWipeBottomRightVerticalStrategy::KPrSnakeWipeBottomRightVerticalStrategy()
    : KPrMatrixWipeStrategy(KPrSnakeWipeEffectFactory::FromRight, "snakeWipe", "topLeftVertical", true, true)
{
}

KPrSnakeWipeBottomRightVerticalStrategy::~KPrSnakeWipeBottomRightVerticalStrategy() = default;

int KPrSnakeWipeBottomRightVerticalStrategy::squareIndex(int x, int y, int columns, int rows)
{
    int Y = y;
    if (x & 1)
        Y = rows - y - 1;
    return Y + (columns - x - 1) * rows;
}

KPrMatrixWipeStrategy::Direction KPrSnakeWipeBottomRightVerticalStrategy::squareDirection(int x, int y, int columns, int rows)
{
    Q_UNUSED(y);
    Q_UNUSED(columns);
    Q_UNUSED(rows);
    if (x & 1)
        return BottomToTop;
    else
        return TopToBottom;
}

int KPrSnakeWipeBottomRightVerticalStrategy::maxIndex(int columns, int rows)
{
    return columns * rows;
}
