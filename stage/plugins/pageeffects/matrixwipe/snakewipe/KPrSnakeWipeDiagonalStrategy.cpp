/* This file is part of the KDE project
   Copyright (C) 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

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

#include "KPrSnakeWipeTopLeftDiagonalStrategy.h"

KPrSnakeWipeDiagonalStrategy::KPrSnakeWipeDiagonalStrategy(int subType, const char * smilType, const char *smilSubType, bool reverse)
    : KPrMatrixWipeStrategy( subType, smilType, smilSubType, reverse )
{
}

KPrSnakeWipeDiagonalStrategy::~KPrSnakeWipeDiagonalStrategy()
{
}

int KPrSnakeWipeDiagonalStrategy::maxIndex(int columns, int rows)
{
    m_indices.resize(columns * rows);
    int idx = 0;
    int i = 0, j = 0;
    int dx = 1, dy = -1;
    while (idx < columns*rows) {
        m_indices[i * rows + j] = idx;
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
            dx = -dx; dy = -dy;
        }
    }
    return columns * rows;
}

