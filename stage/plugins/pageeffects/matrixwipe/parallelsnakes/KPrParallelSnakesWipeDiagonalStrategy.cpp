/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrParallelSnakesWipeDiagonalStrategy.h"
#include "KPrParallelSnakesWipeEffectFactory.h"

KPrParallelSnakesWipeDiagonalStrategy::KPrParallelSnakesWipeDiagonalStrategy(bool reverseAngle, bool reverse)
    : KPrMatrixWipeStrategy(reverse ? (reverseAngle ? KPrParallelSnakesWipeEffectFactory::DiagonalBottomLeftOppositeIn
                                                    : KPrParallelSnakesWipeEffectFactory::DiagonalTopLeftOppositeIn)
                                    : (reverseAngle ? KPrParallelSnakesWipeEffectFactory::DiagonalBottomLeftOppositeOut
                                                    : KPrParallelSnakesWipeEffectFactory::DiagonalTopLeftOppositeOut),
                            "parallelSnakesWipe",
                            reverseAngle ? "diagonalTopLeftOpposite" : "diagonalBottomLeftOpposite",
                            reverse)
    , m_reverseAngle(reverseAngle)
{
    setNeedEvenSquares(true, false);
}

KPrParallelSnakesWipeDiagonalStrategy::~KPrParallelSnakesWipeDiagonalStrategy() = default;

int KPrParallelSnakesWipeDiagonalStrategy::maxIndex(int columns, int rows)
{
    m_indices.resize(columns * rows);
    int idx = 0;
    int i = 0, j = 0;
    int dx = 1, dy = -1;
    while (idx < columns * rows / 2) {
        m_indices[i * rows + j] = idx;
        m_indices[(columns - i - 1) * rows + (rows - j - 1)] = idx;
        idx++;
        i += dx;
        j += dy;
        if (i < 0 || j < 0 || i >= columns || j >= rows) {
            if (i >= columns) {
                i = columns - 1;
                j += 2;
            }
            if (j >= rows) {
                j = rows - 1;
                i += 2;
            }
            if (i < 0) {
                i = 0;
            }
            if (j < 0) {
                j = 0;
            }
            dx = -dx;
            dy = -dy;
        }
    }
    return columns * rows;
}

int KPrParallelSnakesWipeDiagonalStrategy::squareIndex(int x, int y, int columns, int rows)
{
    if (m_reverseAngle)
        y = rows - y - 1;
    int idx = m_indices[x * rows + y];
    if (reverse())
        return idx;
    else
        return columns * rows / 2 - idx - 1;
}
