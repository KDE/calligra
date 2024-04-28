/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRSWEEPWIPESTRATEGY_H
#define KPRSWEEPWIPESTRATEGY_H

#include <KPrPageEffectStrategy.h>

class KPrSweepWipeStrategy : public KPrPageEffectStrategy
{
public:
    KPrSweepWipeStrategy(int subType, const char *smilType, const char *smilSubType, bool reverse);
    ~KPrSweepWipeStrategy() override;

    void next(const KPrPageEffect::Data &data) override;

protected:
    void drawSweep(QPainter &p, double startAngle, double rotationRange, QRect boundingRect, const KPrPageEffect::Data &data);
};

#endif // KPRSWEEPWIPESTRATEGY_H
