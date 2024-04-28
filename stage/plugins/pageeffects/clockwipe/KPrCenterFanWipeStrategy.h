/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRCENTERFANWIPESTRATEGY_H
#define KPRCENTERFANWIPESTRATEGY_H

#include <KPrPageEffectStrategy.h>

class KPrCenterFanWipeStrategy : public KPrPageEffectStrategy
{
public:
    KPrCenterFanWipeStrategy(int startAngle, int fanCount, int subType, const char *smilType, const char *smilSubType, bool reverse);
    ~KPrCenterFanWipeStrategy() override;

    void setup(const KPrPageEffect::Data &data, QTimeLine &timeLine) override;
    void paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data) override;
    void next(const KPrPageEffect::Data &data) override;

private:
    double m_startAngle;
    int m_fanCount;
};

#endif // KPRCENTERFANWIPESTRATEGY_H
