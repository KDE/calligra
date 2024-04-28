/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRWATERFALLWIPETOPRIGHTSTRATEGY_H
#define KPRWATERFALLWIPETOPRIGHTSTRATEGY_H

#include "../KPrMatrixWipeStrategy.h"

class KPrWaterfallWipeTopRightStrategy : public KPrMatrixWipeStrategy
{
public:
    explicit KPrWaterfallWipeTopRightStrategy(Direction direction);
    ~KPrWaterfallWipeTopRightStrategy() override;

protected:
    int squareIndex(int x, int y, int columns, int rows) override;
    Direction squareDirection(int x, int y, int columns, int rows) override;
    int maxIndex(int columns, int rows) override;

private:
    Direction m_direction;
};

#endif // KPRWATERFALLWIPETOPRIGHTSTRATEGY_H
