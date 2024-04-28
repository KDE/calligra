/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRSLIDEWIPEFROMRIGHTSTRATEGY_H
#define KPRSLIDEWIPEFROMRIGHTSTRATEGY_H

#include "pageeffects/KPrPageEffectStrategy.h"

class KPrSlideWipeFromRightStrategy : public KPrPageEffectStrategy
{
public:
    KPrSlideWipeFromRightStrategy();
    ~KPrSlideWipeFromRightStrategy() override;

    void setup(const KPrPageEffect::Data &data, QTimeLine &timeLine) override;

    void paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data) override;

    void next(const KPrPageEffect::Data &data) override;
};

#endif // KPRSLIDEWIPEFROMRIGHTSTRATEGY_H
