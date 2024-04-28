/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRSLIDEWIPEFROMLEFTSTRATEGY_H
#define KPRSLIDEWIPEFROMLEFTSTRATEGY_H

#include "pageeffects/KPrPageEffectStrategy.h"

class KPrSlideWipeFromLeftStrategy : public KPrPageEffectStrategy
{
public:
    KPrSlideWipeFromLeftStrategy();
    ~KPrSlideWipeFromLeftStrategy() override;

    void setup(const KPrPageEffect::Data &data, QTimeLine &timeLine) override;

    void paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data) override;

    void next(const KPrPageEffect::Data &data) override;
};

#endif // KPRSLIDEWIPEFROMLEFTSTRATEGY_H
