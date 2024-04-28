/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRDOUBLESWEEPWIPESTRATEGY_H
#define KPRDOUBLESWEEPWIPESTRATEGY_H

#include "KPrSweepWipeStrategy.h"
#include <KPrPageEffectStrategy.h>

class KPrDoubleSweepWipeStrategy : public KPrSweepWipeStrategy
{
public:
    KPrDoubleSweepWipeStrategy(int subType, const char *smilType, const char *smilSubType, bool reverse);
    ~KPrDoubleSweepWipeStrategy() override;

    void setup(const KPrPageEffect::Data &data, QTimeLine &timeLine) override;
    void paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data) override;
};

#endif // KPRDOUBLESWEEPWIPESTRATEGY_H
