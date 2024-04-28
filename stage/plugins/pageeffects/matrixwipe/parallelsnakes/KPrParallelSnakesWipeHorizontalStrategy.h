/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRPARALLELSNAKESWIPEHORIZONTALSTRATEGY_H
#define KPRPARALLELSNAKESWIPEHORIZONTALSTRATEGY_H

#include "../KPrMatrixWipeStrategy.h"

class KPrParallelSnakesWipeHorizontalStrategy : public KPrMatrixWipeStrategy
{
public:
    KPrParallelSnakesWipeHorizontalStrategy(bool reverseTop, bool reverseBottom, bool reverse);
    ~KPrParallelSnakesWipeHorizontalStrategy() override;

protected:
    int squareIndex(int x, int y, int columns, int rows) override;
    Direction squareDirection(int x, int y, int columns, int rows) override;
    int maxIndex(int columns, int rows) override;

private:
    bool m_reverseTop;
    bool m_reverseBottom;
};

#endif // KPRPARALLELSNAKESWIPEHORIZONTALSTRATEGY_H
