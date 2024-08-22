/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrSnakeWipeBottomRightHorizontalStrategy.h"
#include "KPrSnakeWipeEffectFactory.h"

KPrSnakeWipeBottomRightHorizontalStrategy::KPrSnakeWipeBottomRightHorizontalStrategy()
    : KPrMatrixWipeStrategy(KPrSnakeWipeEffectFactory::FromBottom, "snakeWipe", "topLeftHorizontal", true, true)
{
}

KPrSnakeWipeBottomRightHorizontalStrategy::~KPrSnakeWipeBottomRightHorizontalStrategy() = default;

int KPrSnakeWipeBottomRightHorizontalStrategy::squareIndex(int x, int y, int columns, int rows)
{
    int X = x;
    if (y & 1)
        X = columns - x - 1;
    return X + (rows - y - 1) * columns;
}

KPrMatrixWipeStrategy::Direction KPrSnakeWipeBottomRightHorizontalStrategy::squareDirection(int x, int y, int columns, int rows)
{
    Q_UNUSED(x);
    Q_UNUSED(columns);
    Q_UNUSED(rows);
    if (y & 1)
        return RightToLeft;
    else
        return LeftToRight;
}

int KPrSnakeWipeBottomRightHorizontalStrategy::maxIndex(int columns, int rows)
{
    return columns * rows;
}
