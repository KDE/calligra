/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Timothée Lacroix <dakeyras.khan@gmail.com>
   SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRBARWIPEFROMRIGHTSTRATEGY_H
#define KPRBARWIPEFROMRIGHTSTRATEGY_H

#include "pageeffects/KPrPageEffectStrategy.h"

class KPrBarWipeFromRightStrategy : public KPrPageEffectStrategy
{
public:
    KPrBarWipeFromRightStrategy();
    ~KPrBarWipeFromRightStrategy() override;

    void setup(const KPrPageEffect::Data &data, QTimeLine &timeLine) override;

    void paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data) override;

    void next(const KPrPageEffect::Data &data) override;
};

#endif // KPRBARWIPEFROMRIGHTSTRATEGY_H
