/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRSINGLESWEEPWIPESTRATEGY_H
#define KPRSINGLESWEEPWIPESTRATEGY_H

#include <KPrPageEffectStrategy.h>

class KPrSingleSweepWipeStrategy : public KPrPageEffectStrategy
{
public:
    KPrSingleSweepWipeStrategy(double rotationX,
                               double rotationY,
                               int startAngle,
                               int rotationAngle,
                               int subType,
                               const char *smilType,
                               const char *smilSubType,
                               bool reverse);
    ~KPrSingleSweepWipeStrategy() override;

    void setup(const KPrPageEffect::Data &data, QTimeLine &timeLine) override;
    void paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data) override;
    void next(const KPrPageEffect::Data &data) override;

private:
    double m_startAngle;
    double m_rotationAngle;
    double m_rotationX;
    double m_rotationY;
};

#endif // KPRSINGLESWEEPWIPESTRATEGY_H
