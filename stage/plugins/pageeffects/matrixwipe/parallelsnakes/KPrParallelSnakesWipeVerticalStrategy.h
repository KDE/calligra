/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRPARALLELSNAKESWIPEVERTICALSTRATEGY_H
#define KPRPARALLELSNAKESWIPEVERTICALSTRATEGY_H

#include "../KPrMatrixWipeStrategy.h"

class KPrParallelSnakesWipeVerticalStrategy : public KPrMatrixWipeStrategy
{
public:
    KPrParallelSnakesWipeVerticalStrategy(bool reverseLeft, bool reverseRight, bool reverse);
    ~KPrParallelSnakesWipeVerticalStrategy() override;

protected:
    int squareIndex(int x, int y, int columns, int rows) override;
    Direction squareDirection(int x, int y, int columns, int rows) override;
    int maxIndex(int columns, int rows) override;

private:
    bool m_reverseLeft;
    bool m_reverseRight;
};

#endif // KPRPARALLELSNAKESWIPEVERTICALSTRATEGY_H
