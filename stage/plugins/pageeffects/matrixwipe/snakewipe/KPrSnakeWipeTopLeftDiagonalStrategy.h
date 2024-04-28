/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRSNAKEWIPETOPLEFTDIAGONALSTRATEGY_H
#define KPRSNAKEWIPETOPLEFTDIAGONALSTRATEGY_H

#include "KPrSnakeWipeDiagonalStrategy.h"

class KPrSnakeWipeTopLeftDiagonalStrategy : public KPrSnakeWipeDiagonalStrategy
{
public:
    KPrSnakeWipeTopLeftDiagonalStrategy();
    ~KPrSnakeWipeTopLeftDiagonalStrategy() override;

protected:
    int squareIndex(int x, int y, int columns, int rows) override;
};

#endif // KPRSNAKEWIPETOPLEFTDIAGONALSTRATEGY_H
