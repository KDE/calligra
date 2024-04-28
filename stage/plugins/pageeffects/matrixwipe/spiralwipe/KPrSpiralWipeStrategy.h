/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRSPIRALWIPESTRATEGY_H
#define KPRSPIRALWIPESTRATEGY_H

#include "../KPrMatrixWipeStrategy.h"

class KPrSpiralWipeStrategy : public KPrMatrixWipeStrategy
{
public:
    KPrSpiralWipeStrategy(int firstLeg, bool clockwise, bool reverse);
    ~KPrSpiralWipeStrategy() override;

protected:
    int maxIndex(int columns, int rows) override;
    int squareIndex(int x, int y, int columns, int rows) override;

private:
    int m_firstLeg;
    bool m_clockwise;
};

#endif // KPRSPIRALWIPESTRATEGY_H
