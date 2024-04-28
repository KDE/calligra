/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRWINDSHIELDWIPESTRATEGY_H
#define KPRWINDSHIELDWIPESTRATEGY_H

#include "KPrSweepWipeStrategy.h"
#include <KPrPageEffectStrategy.h>

class KPrWindShieldWipeStrategy : public KPrSweepWipeStrategy
{
public:
    KPrWindShieldWipeStrategy(int subType, const char *smilType, const char *smilSubType, bool reverse);
    ~KPrWindShieldWipeStrategy() override;

    void setup(const KPrPageEffect::Data &data, QTimeLine &timeLine) override;
    void paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data) override;
};

#endif // KPRWINDSHIELDWIPESTRATEGY_H
