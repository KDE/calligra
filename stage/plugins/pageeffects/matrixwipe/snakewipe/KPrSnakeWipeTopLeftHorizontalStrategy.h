/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRSNAKEWIPETOPLEFTHORIZONTALSTRATEGY_H
#define KPRSNAKEWIPETOPLEFTHORIZONTALSTRATEGY_H

#include "../KPrMatrixWipeStrategy.h"

class KPrSnakeWipeTopLeftHorizontalStrategy : public KPrMatrixWipeStrategy
{
public:
    KPrSnakeWipeTopLeftHorizontalStrategy();
    ~KPrSnakeWipeTopLeftHorizontalStrategy() override;

protected:
    int squareIndex(int x, int y, int columns, int rows) override;
    Direction squareDirection(int x, int y, int columns, int rows) override;
    int maxIndex(int columns, int rows) override;
};

#endif // KPRSNAKEWIPETOPLEFTHORIZONTALSTRATEGY_H
