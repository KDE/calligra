/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Benjamin Port <port.benjamin@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRSPACEROTATIONFROMTOPSTRATEGY_H
#define KPRSPACEROTATIONFROMTOPSTRATEGY_H

#include "pageeffects/KPrPageEffectStrategy.h"
#include <QTransform>

class KPrSpaceRotationFromTopStrategy : public KPrPageEffectStrategy
{
public:
    KPrSpaceRotationFromTopStrategy();
    ~KPrSpaceRotationFromTopStrategy() override;

    void setup(const KPrPageEffect::Data &data, QTimeLine &timeLine) override;

    void paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data) override;

    void next(const KPrPageEffect::Data &data) override;

    void finish(const KPrPageEffect::Data &data) override;

private:
    QTransform m_transform;
};

#endif // KPRSPACEROTATIONFROMTOPSTRATEGY_H
