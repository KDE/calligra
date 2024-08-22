/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrSnakeWipeTopLeftDiagonalStrategy.h"

KPrSnakeWipeDiagonalStrategy::KPrSnakeWipeDiagonalStrategy(int subType, const char *smilType, const char *smilSubType, bool reverse)
    : KPrMatrixWipeStrategy(subType, smilType, smilSubType, reverse)
{
}

KPrSnakeWipeDiagonalStrategy::~KPrSnakeWipeDiagonalStrategy() = default;

int KPrSnakeWipeDiagonalStrategy::maxIndex(int columns, int rows)
{
    m_indices.resize(columns * rows);
    int idx = 0;
    int i = 0, j = 0;
    int dx = 1, dy = -1;
    while (idx < columns * rows) {
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
            dx = -dx;
            dy = -dy;
        }
    }
    return columns * rows;
}
