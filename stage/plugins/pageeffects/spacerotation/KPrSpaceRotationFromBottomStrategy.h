/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Benjamin Port <port.benjamin@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRSPACEROTATIONFROMBOTTOMSTRATEGY_H
#define KPRSPACEROTATIONFROMBOTTOMSTRATEGY_H

#include "pageeffects/KPrPageEffectStrategy.h"
#include <QTransform>

class KPrSpaceRotationFromBottomStrategy : public KPrPageEffectStrategy
{
public:
    KPrSpaceRotationFromBottomStrategy();
    ~KPrSpaceRotationFromBottomStrategy() override;

    void setup(const KPrPageEffect::Data &data, QTimeLine &timeLine) override;

    void paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data) override;

    void next(const KPrPageEffect::Data &data) override;

    void finish(const KPrPageEffect::Data &data) override;

private:
    QTransform m_transform;
};

#endif // KPRSPACEROTATIONFROMBOTTOMSTRATEGY_H
