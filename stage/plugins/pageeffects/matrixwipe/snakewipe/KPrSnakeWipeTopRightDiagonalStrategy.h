/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRSNAKEWIPETOPRIGHTDIAGONALSTRATEGY_H
#define KPRSNAKEWIPETOPRIGHTDIAGONALSTRATEGY_H

#include "KPrSnakeWipeDiagonalStrategy.h"

class KPrSnakeWipeTopRightDiagonalStrategy : public KPrSnakeWipeDiagonalStrategy
{
public:
    KPrSnakeWipeTopRightDiagonalStrategy();
    ~KPrSnakeWipeTopRightDiagonalStrategy() override;

protected:
    int squareIndex(int x, int y, int columns, int rows) override;
};

#endif // KPRSNAKEWIPETOPRIGHTDIAGONALSTRATEGY_H
