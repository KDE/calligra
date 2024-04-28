/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRPARALLELSNAKESWIPEDIAGONALSTRATEGY_H
#define KPRPARALLELSNAKESWIPEDIAGONALSTRATEGY_H

#include "../KPrMatrixWipeStrategy.h"
#include <QVector>

class KPrParallelSnakesWipeDiagonalStrategy : public KPrMatrixWipeStrategy
{
public:
    KPrParallelSnakesWipeDiagonalStrategy(bool reverseAngle, bool reverse);
    ~KPrParallelSnakesWipeDiagonalStrategy() override;

protected:
    int maxIndex(int columns, int rows) override;
    int squareIndex(int x, int y, int columns, int rows) override;
    QVector<int> m_indices;
    bool m_reverseAngle;
};

#endif // KPRPARALLELSNAKESWIPEDIAGONALSTRATEGY_H
