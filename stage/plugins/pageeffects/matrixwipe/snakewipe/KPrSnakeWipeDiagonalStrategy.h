/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRSNAKEWIPEDIAGONALSTRATEGY_H
#define KPRSNAKEWIPEDIAGONALSTRATEGY_H

#include "../KPrMatrixWipeStrategy.h"
#include <QVector>

class KPrSnakeWipeDiagonalStrategy : public KPrMatrixWipeStrategy
{
public:
    KPrSnakeWipeDiagonalStrategy(int subType, const char *smilType, const char *smilSubType, bool reverse);
    ~KPrSnakeWipeDiagonalStrategy() override;

protected:
    int maxIndex(int columns, int rows) override;
    QVector<int> m_indices;
};

#endif // KPRSNAKEWIPEDIAGONALSTRATEGY_H
