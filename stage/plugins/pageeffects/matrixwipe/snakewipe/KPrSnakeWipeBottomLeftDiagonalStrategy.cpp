/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrSnakeWipeBottomLeftDiagonalStrategy.h"
#include "KPrSnakeWipeEffectFactory.h"

KPrSnakeWipeBottomLeftDiagonalStrategy::KPrSnakeWipeBottomLeftDiagonalStrategy()
    : KPrSnakeWipeDiagonalStrategy(KPrSnakeWipeEffectFactory::FromBottomLeft, "snakeWipe", "bottomLeftDiagonal", false)
{
}

KPrSnakeWipeBottomLeftDiagonalStrategy::~KPrSnakeWipeBottomLeftDiagonalStrategy() = default;

int KPrSnakeWipeBottomLeftDiagonalStrategy::squareIndex(int x, int y, int columns, int rows)
{
    Q_UNUSED(columns);
    return m_indices[x * rows + (rows - y - 1)];
}
