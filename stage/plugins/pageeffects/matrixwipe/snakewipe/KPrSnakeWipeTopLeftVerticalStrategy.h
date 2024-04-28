/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRSNAKEWIPETOPLEFTVERTICALSTRATEGY_H
#define KPRSNAKEWIPETOPLEFTVERTICALSTRATEGY_H

#include "../KPrMatrixWipeStrategy.h"

class KPrSnakeWipeTopLeftVerticalStrategy : public KPrMatrixWipeStrategy
{
public:
    KPrSnakeWipeTopLeftVerticalStrategy();
    ~KPrSnakeWipeTopLeftVerticalStrategy() override;

protected:
    int squareIndex(int x, int y, int columns, int rows) override;
    Direction squareDirection(int x, int y, int columns, int rows) override;
    int maxIndex(int columns, int rows) override;
};

#endif // KPRSNAKEWIPETOPLEFTVERTICALSTRATEGY_H
