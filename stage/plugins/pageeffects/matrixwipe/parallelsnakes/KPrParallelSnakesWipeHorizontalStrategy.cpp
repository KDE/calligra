/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrParallelSnakesWipeHorizontalStrategy.h"
#include "KPrParallelSnakesWipeEffectFactory.h"

static int getSubType(bool reverseTop, bool reverseBottom, bool reverse)
{
    if (reverseTop) {
        if (reverseBottom)
            return reverse ? KPrParallelSnakesWipeEffectFactory::HorizontalRightSameOut : KPrParallelSnakesWipeEffectFactory::HorizontalRightSameIn;
        else
            return reverse ? KPrParallelSnakesWipeEffectFactory::HorizontalTopRightOppositeOut
                           : KPrParallelSnakesWipeEffectFactory::HorizontalTopRightOppositeIn;
    } else {
        if (reverseBottom)
            return reverse ? KPrParallelSnakesWipeEffectFactory::HorizontalTopLeftOppositeOut : KPrParallelSnakesWipeEffectFactory::HorizontalTopLeftOppositeIn;
        else
            return reverse ? KPrParallelSnakesWipeEffectFactory::HorizontalLeftSameOut : KPrParallelSnakesWipeEffectFactory::HorizontalLeftSameIn;
    }
}

static const char *getSmilSubType(bool reverseTop, bool reverseBottom)
{
    if (reverseTop) {
        if (reverseBottom)
            return "horizontalRightSame";
        else
            return "horizontalTopRightOpposite";
    } else {
        if (reverseBottom)
            return "horizontalTopLeftOpposite";
        else
            return "horizontalLeftSame";
    }
}

KPrParallelSnakesWipeHorizontalStrategy::KPrParallelSnakesWipeHorizontalStrategy(bool reverseTop, bool reverseBottom, bool reverse)
    : KPrMatrixWipeStrategy(getSubType(reverseTop, reverseBottom, reverse), "parallelSnakesWipe", getSmilSubType(reverseTop, reverseBottom), reverse, true)
    , m_reverseTop(reverseTop)
    , m_reverseBottom(reverseBottom)
{
    setNeedEvenSquares();
}

KPrParallelSnakesWipeHorizontalStrategy::~KPrParallelSnakesWipeHorizontalStrategy() = default;

int KPrParallelSnakesWipeHorizontalStrategy::squareIndex(int x, int y, int columns, int rows)
{
    int X = x;
    int idx;
    if (y < rows / 2) {
        if (m_reverseTop)
            X = columns - X - 1;
        if (y & 1)
            X = columns - X - 1;
        idx = X + y * columns;
    } else {
        if (m_reverseBottom)
            X = columns - X - 1;
        if (!(y & 1))
            X = columns - X - 1;
        idx = X + (rows - y - 1) * columns;
    }
    if (reverse()) {
        return rows * columns / 2 - idx - 1;
    } else {
        return idx;
    }
}

KPrMatrixWipeStrategy::Direction KPrParallelSnakesWipeHorizontalStrategy::squareDirection(int x, int y, int columns, int rows)
{
    Q_UNUSED(x);
    Q_UNUSED(columns);
    bool reverse = false;
    if (y >= rows / 2)
        reverse = !reverse;
    if (y & 1)
        reverse = !reverse;
    if (this->reverse())
        reverse = !reverse;
    if (y < rows / 2) {
        if (m_reverseTop)
            reverse = !reverse;
    } else {
        if (m_reverseBottom)
            reverse = !reverse;
    }
    return reverse ? RightToLeft : LeftToRight;
}

int KPrParallelSnakesWipeHorizontalStrategy::maxIndex(int columns, int rows)
{
    return columns * rows / 2;
}
