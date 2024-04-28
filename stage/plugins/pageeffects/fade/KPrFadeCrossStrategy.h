/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2010 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPRFADECROSSSTRATEGY_H
#define KPRFADECROSSSTRATEGY_H

#include "pageeffects/KPrPageEffectStrategy.h"

class KPrFadeCrossStrategy : public KPrPageEffectStrategy
{
public:
    KPrFadeCrossStrategy();
    ~KPrFadeCrossStrategy() override;

    void setup(const KPrPageEffect::Data &data, QTimeLine &timeLine) override;

    void paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data) override;

    void next(const KPrPageEffect::Data &data) override;

    void finish(const KPrPageEffect::Data &data) override;
};

#endif /* KPRFADECROSSSTRATEGY_H */
