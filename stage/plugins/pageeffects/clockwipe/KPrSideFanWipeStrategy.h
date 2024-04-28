/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRSIDEFANWIPESTRATEGY_H
#define KPRSIDEFANWIPESTRATEGY_H

#include <KPrPageEffectStrategy.h>

class KPrSideFanWipeStrategy : public KPrPageEffectStrategy
{
public:
    KPrSideFanWipeStrategy(int positionAngle, int fanCount, int subType, const char *smilType, const char *smilSubType, bool reverse);
    ~KPrSideFanWipeStrategy() override;

    void setup(const KPrPageEffect::Data &data, QTimeLine &timeLine) override;
    void paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data) override;
    void next(const KPrPageEffect::Data &data) override;

private:
    double m_startAngle;
    double m_positionAngle;
    int m_fanCount;
};

#endif // KPRSIDEFANWIPESTRATEGY_H
