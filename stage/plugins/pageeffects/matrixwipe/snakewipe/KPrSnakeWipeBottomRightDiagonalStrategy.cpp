/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrSnakeWipeBottomRightDiagonalStrategy.h"
#include "KPrSnakeWipeEffectFactory.h"

KPrSnakeWipeBottomRightDiagonalStrategy::KPrSnakeWipeBottomRightDiagonalStrategy()
    : KPrSnakeWipeDiagonalStrategy(KPrSnakeWipeEffectFactory::FromBottomRight, "snakeWipe", "bottomRightDiagonal", false)
{
}

KPrSnakeWipeBottomRightDiagonalStrategy::~KPrSnakeWipeBottomRightDiagonalStrategy() = default;

int KPrSnakeWipeBottomRightDiagonalStrategy::squareIndex(int x, int y, int columns, int rows)
{
    return m_indices[(columns - x - 1) * rows + (rows - y - 1)];
}
