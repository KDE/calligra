/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrSnakeWipeTopLeftVerticalStrategy.h"
#include "KPrSnakeWipeEffectFactory.h"

KPrSnakeWipeTopLeftVerticalStrategy::KPrSnakeWipeTopLeftVerticalStrategy()
    : KPrMatrixWipeStrategy(KPrSnakeWipeEffectFactory::FromLeft, "snakeWipe", "topLeftVertical", false, true)
{
}

KPrSnakeWipeTopLeftVerticalStrategy::~KPrSnakeWipeTopLeftVerticalStrategy() = default;

int KPrSnakeWipeTopLeftVerticalStrategy::squareIndex(int x, int y, int columns, int rows)
{
    Q_UNUSED(columns);
    int Y = y;
    if (x & 1)
        Y = rows - y - 1;
    return Y + x * rows;
}

KPrMatrixWipeStrategy::Direction KPrSnakeWipeTopLeftVerticalStrategy::squareDirection(int x, int y, int columns, int rows)
{
    Q_UNUSED(y);
    Q_UNUSED(columns);
    Q_UNUSED(rows);
    if (x & 1)
        return BottomToTop;
    else
        return TopToBottom;
}

int KPrSnakeWipeTopLeftVerticalStrategy::maxIndex(int columns, int rows)
{
    return columns * rows;
}
