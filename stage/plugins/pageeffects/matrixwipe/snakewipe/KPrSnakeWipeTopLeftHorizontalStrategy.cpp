/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrSnakeWipeTopLeftHorizontalStrategy.h"
#include "KPrSnakeWipeEffectFactory.h"

KPrSnakeWipeTopLeftHorizontalStrategy::KPrSnakeWipeTopLeftHorizontalStrategy()
    : KPrMatrixWipeStrategy(KPrSnakeWipeEffectFactory::FromTop, "snakeWipe", "topLeftHorizontal", false, true)
{
}

KPrSnakeWipeTopLeftHorizontalStrategy::~KPrSnakeWipeTopLeftHorizontalStrategy() = default;

int KPrSnakeWipeTopLeftHorizontalStrategy::squareIndex(int x, int y, int columns, int rows)
{
    Q_UNUSED(rows);

    int X = x;
    if (y & 1)
        X = columns - x - 1;
    return X + y * columns;
}

KPrMatrixWipeStrategy::Direction KPrSnakeWipeTopLeftHorizontalStrategy::squareDirection(int x, int y, int columns, int rows)
{
    Q_UNUSED(x);
    Q_UNUSED(columns);
    Q_UNUSED(rows);
    if (y & 1)
        return RightToLeft;
    else
        return LeftToRight;
}

int KPrSnakeWipeTopLeftHorizontalStrategy::maxIndex(int columns, int rows)
{
    return columns * rows;
}
