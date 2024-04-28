/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRBOXSNAKESWIPESTRATEGY_H
#define KPRBOXSNAKESWIPESTRATEGY_H

#include "../KPrMatrixWipeStrategy.h"

class KPrBoxSnakesWipeStrategy : public KPrMatrixWipeStrategy
{
public:
    KPrBoxSnakesWipeStrategy(int horRepeat, int verRepeat, bool clockwise, bool reverse);
    ~KPrBoxSnakesWipeStrategy() override;

protected:
    int maxIndex(int columns, int rows) override;
    int squareIndex(int x, int y, int columns, int rows) override;

private:
    int m_horRepeat;
    int m_verRepeat;
    bool m_clockwise;
};

#endif // KPRBOXSNAKESWIPESTRATEGY_H
