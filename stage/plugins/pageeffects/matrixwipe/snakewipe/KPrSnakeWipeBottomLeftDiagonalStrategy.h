/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRSNAKEWIPEBOTTOMLEFTDIAGONALSTRATEGY_H
#define KPRSNAKEWIPEBOTTOMLEFTDIAGONALSTRATEGY_H

#include "KPrSnakeWipeDiagonalStrategy.h"

class KPrSnakeWipeBottomLeftDiagonalStrategy : public KPrSnakeWipeDiagonalStrategy
{
public:
    KPrSnakeWipeBottomLeftDiagonalStrategy();
    ~KPrSnakeWipeBottomLeftDiagonalStrategy() override;

protected:
    int squareIndex(int x, int y, int columns, int rows) override;
};

#endif // KPRSNAKEWIPEBOTTOMLEFTDIAGONALSTRATEGY_H
