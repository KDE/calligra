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

#include "KPrParallelSnakesWipeVerticalStrategy.h"

static KPrPageEffect::SubType getSubType(bool reverseLeft, bool reverseRight, bool reverse) {
    if (reverseLeft) {
        if (reverseRight) return reverse ? KPrPageEffect::VerticalBottomSameOut : KPrPageEffect::VerticalBottomSameIn;
        else return reverse ? KPrPageEffect::VerticalBottomLeftOppositeOut : KPrPageEffect::VerticalBottomLeftOppositeIn;
    } else {
        if (reverseRight) return reverse ? KPrPageEffect::VerticalTopLeftOppositeOut : KPrPageEffect::VerticalTopLeftOppositeIn;
        else return reverse ? KPrPageEffect::VerticalTopSameOut : KPrPageEffect::VerticalTopSameIn;
    }
}

static const char* getSmilSubType(bool reverseLeft, bool reverseRight) {
    if (reverseLeft) {
        if (reverseRight) return "verticalBottomSame";
        else return "verticalBottomLeftOpposite";
    } else {
        if (reverseRight) return "verticalTopLeftOpposite";
        else return "verticalTopSame";
    }
}

KPrParallelSnakesWipeVerticalStrategy::KPrParallelSnakesWipeVerticalStrategy(bool reverseLeft, bool reverseRight, bool reverse)
    : KPrMatrixWipeStrategy( getSubType(reverseLeft, reverseRight, reverse), "parallelSnakesWipe", getSmilSubType(reverseLeft, reverseRight), reverse, true ),
    m_reverseLeft(reverseLeft), m_reverseRight(reverseRight)
{
    setNeedEvenSquares();
}

KPrParallelSnakesWipeVerticalStrategy::~KPrParallelSnakesWipeVerticalStrategy()
{
}

int KPrParallelSnakesWipeVerticalStrategy::squareIndex(int x, int y, int collumns, int rows)
{
    int Y = y;
    int idx;
    if (x < collumns / 2) {
        if (m_reverseLeft) Y = rows - Y - 1;
        if (x & 1) Y = rows - Y - 1;
        idx = Y + x * rows;
    } else {
        if (m_reverseRight) Y = rows - Y - 1;
        if (!(x & 1)) Y = rows - Y - 1;
        idx = Y + (collumns - x - 1) * rows;
    }
    if (reverse()) {
        return rows * collumns / 2 - idx - 1;
    } else {
        return idx;
    }
}

KPrMatrixWipeStrategy::Direction KPrParallelSnakesWipeVerticalStrategy::squareDirection(int x, int y, int collumns, int rows)
{
    bool reverse = false;
    if (x >= collumns / 2) reverse = !reverse;
    if (x & 1) reverse = !reverse;
    if (this->reverse()) reverse = !reverse;
    if (x < collumns / 2) {
        if (m_reverseLeft) reverse = !reverse;
    } else {
        if (m_reverseRight) reverse = !reverse;
    }
    return reverse ? BottomToTop : TopToBottom;
}

int KPrParallelSnakesWipeVerticalStrategy::maxIndex(int collumns, int rows)
{
    return collumns * rows / 2;
}

