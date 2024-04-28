/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRSLIDEWIPETORIGHTSTRATEGY_H
#define KPRSLIDEWIPETORIGHTSTRATEGY_H

#include "pageeffects/KPrPageEffectStrategy.h"

class KPrSlideWipeToRightStrategy : public KPrPageEffectStrategy
{
public:
    KPrSlideWipeToRightStrategy();
    ~KPrSlideWipeToRightStrategy() override;

    void setup(const KPrPageEffect::Data &data, QTimeLine &timeLine) override;

    void paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data) override;

    void next(const KPrPageEffect::Data &data) override;
};

#endif // KPRSLIDEWIPETORIGHTSTRATEGY_H
