/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrSnakeWipeTopLeftDiagonalStrategy.h"
#include "KPrSnakeWipeEffectFactory.h"

KPrSnakeWipeTopLeftDiagonalStrategy::KPrSnakeWipeTopLeftDiagonalStrategy()
    : KPrSnakeWipeDiagonalStrategy(KPrSnakeWipeEffectFactory::FromTopLeft, "snakeWipe", "topLeftDiagonal", false)
{
}

KPrSnakeWipeTopLeftDiagonalStrategy::~KPrSnakeWipeTopLeftDiagonalStrategy() = default;

int KPrSnakeWipeTopLeftDiagonalStrategy::squareIndex(int x, int y, int columns, int rows)
{
    Q_UNUSED(columns);
    return m_indices[x * rows + y];
}
