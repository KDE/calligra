/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Benjamin Port <port.benjamin@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRSPACEROTATIONFROMRIGHTSTRATEGY_H
#define KPRSPACEROTATIONFROMRIGHTSTRATEGY_H

#include "pageeffects/KPrPageEffectStrategy.h"
#include <QTransform>

class KPrSpaceRotationFromRightStrategy : public KPrPageEffectStrategy
{
public:
    KPrSpaceRotationFromRightStrategy();
    ~KPrSpaceRotationFromRightStrategy() override;

    void setup(const KPrPageEffect::Data &data, QTimeLine &timeLine) override;

    void paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data) override;

    void next(const KPrPageEffect::Data &data) override;

    void finish(const KPrPageEffect::Data &data) override;

private:
    QTransform m_transform;
};

#endif // KPRSPACEROTATIONFROMRIGHTSTRATEGY_H
