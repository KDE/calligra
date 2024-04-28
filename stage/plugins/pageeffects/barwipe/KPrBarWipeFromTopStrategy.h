/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Timothée Lacroix <dakeyras.khan@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRBARWIPEFROMTOPSTRATEGY_H
#define KPRBARWIPEFROMTOPSTRATEGY_H

#include "pageeffects/KPrPageEffectStrategy.h"

class KPrBarWipeFromTopStrategy : public KPrPageEffectStrategy
{
public:
    KPrBarWipeFromTopStrategy();
    ~KPrBarWipeFromTopStrategy() override;

    void setup(const KPrPageEffect::Data &data, QTimeLine &timeLine) override;

    void paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data) override;

    void next(const KPrPageEffect::Data &data) override;
};

#endif // KPRBARWIPEFROMTOPSTRATEGY_H
