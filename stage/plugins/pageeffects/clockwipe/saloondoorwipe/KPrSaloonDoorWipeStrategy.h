/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRSALOONDOORWIPESTRATEGY_H
#define KPRSALOONDOORWIPESTRATEGY_H

#include "KPrSweepWipeStrategy.h"

class KPrSaloonDoorWipeStrategy : public KPrSweepWipeStrategy
{
public:
    KPrSaloonDoorWipeStrategy(int subType, const char *smilType, const char *smilSubType, bool reverse);
    ~KPrSaloonDoorWipeStrategy() override;

    void setup(const KPrPageEffect::Data &data, QTimeLine &timeLine) override;
    void paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data) override;
};

#endif // KPRSALOONDOORWIPESTRATEGY_H
