/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRSLIDEWIPETOBOTTOMSTRATEGY_H
#define KPRSLIDEWIPETOBOTTOMSTRATEGY_H

#include "pageeffects/KPrPageEffectStrategy.h"

class KPrSlideWipeToBottomStrategy : public KPrPageEffectStrategy
{
public:
    KPrSlideWipeToBottomStrategy();
    ~KPrSlideWipeToBottomStrategy() override;

    void setup(const KPrPageEffect::Data &data, QTimeLine &timeLine) override;

    void paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data) override;

    void next(const KPrPageEffect::Data &data) override;
};

#endif // KPRSLIDEWIPETOBOTTOMSTRATEGY_H
