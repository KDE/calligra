/* This file is part of the KDE project
   Copyright (C) 2008 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KPrParallelSnakesWipeDiagonalStrategy.h"

KPrParallelSnakesWipeDiagonalStrategy::KPrParallelSnakesWipeDiagonalStrategy(bool reverseAngle, bool reverse)
    : KPrMatrixWipeStrategy( reverse ? (reverseAngle ? KPrPageEffect::DiagonalBottomLeftOppositeIn : KPrPageEffect::DiagonalTopLeftOppositeIn) : (reverseAngle ? KPrPageEffect::DiagonalBottomLeftOppositeOut : KPrPageEffect::DiagonalTopLeftOppositeOut), "parallelSnakesWipe", reverseAngle ? "diagonalTopLeftOpposite" : "diagonalBottomLeftOpposite", reverse ),
    m_reverseAngle(reverseAngle)
{
    setNeedEvenSquares(true, false);
}

KPrParallelSnakesWipeDiagonalStrategy::~KPrParallelSnakesWipeDiagonalStrategy()
{
}

int KPrParallelSnakesWipeDiagonalStrategy::maxIndex(int collumns, int rows)
{
    m_indices.resize(collumns * rows);
    int idx = 0;
    int i = 0, j = 0;
    int dx = 1, dy = -1;
    while (idx < collumns*rows / 2) {
        m_indices[i * rows + j] = idx;
        m_indices[(collumns - i - 1) * rows + (rows - j - 1)] = idx;
        idx++;
        i += dx;
        j += dy;
        if (i < 0 || j < 0 || i >= collumns || j >= rows) {
            if (i >= collumns) {
                i = collumns - 1;
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
            dx = -dx; dy = -dy;
        }
    }
    return collumns * rows;
}

int KPrParallelSnakesWipeDiagonalStrategy::squareIndex(int x, int y, int collumns, int rows){
    if (m_reverseAngle) y = rows - y - 1;
    int idx = m_indices[x * rows + y];
    if (reverse()) return idx;
    else return collumns * rows / 2 - idx - 1;
}
