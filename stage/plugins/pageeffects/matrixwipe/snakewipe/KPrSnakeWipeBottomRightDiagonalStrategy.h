/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRSNAKEWIPEBOTTOMRIGHTDIAGONALSTRATEGY_H
#define KPRSNAKEWIPEBOTTOMRIGHTDIAGONALSTRATEGY_H

#include "KPrSnakeWipeDiagonalStrategy.h"

class KPrSnakeWipeBottomRightDiagonalStrategy : public KPrSnakeWipeDiagonalStrategy
{
public:
    KPrSnakeWipeBottomRightDiagonalStrategy();
    ~KPrSnakeWipeBottomRightDiagonalStrategy() override;

protected:
    int squareIndex(int x, int y, int columns, int rows) override;
};

#endif // KPRSNAKEWIPEBOTTOMRIGHTDIAGONALSTRATEGY_H
