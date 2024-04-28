/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Timothée Lacroix <dakeyras.khan@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRBARWIPEFROMLEFTSTRATEGY_H
#define KPRBARWIPEFROMLEFTSTRATEGY_H

#include "pageeffects/KPrPageEffectStrategy.h"

class KPrBarWipeFromLeftStrategy : public KPrPageEffectStrategy
{
public:
    KPrBarWipeFromLeftStrategy();
    ~KPrBarWipeFromLeftStrategy() override;

    void setup(const KPrPageEffect::Data &data, QTimeLine &timeLine) override;

    void paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data) override;

    void next(const KPrPageEffect::Data &data) override;
};

#endif // KPRBARWIPEFROMLEFTSTRATEGY_H
