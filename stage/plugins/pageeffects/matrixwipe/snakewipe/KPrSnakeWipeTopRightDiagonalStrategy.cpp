/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrSnakeWipeTopRightDiagonalStrategy.h"
#include "KPrSnakeWipeEffectFactory.h"

KPrSnakeWipeTopRightDiagonalStrategy::KPrSnakeWipeTopRightDiagonalStrategy()
    : KPrSnakeWipeDiagonalStrategy(KPrSnakeWipeEffectFactory::FromTopRight, "snakeWipe", "topRightDiagonal", false)
{
}

KPrSnakeWipeTopRightDiagonalStrategy::~KPrSnakeWipeTopRightDiagonalStrategy() = default;

int KPrSnakeWipeTopRightDiagonalStrategy::squareIndex(int x, int y, int columns, int rows)
{
    return m_indices[(columns - x - 1) * rows + y];
}
