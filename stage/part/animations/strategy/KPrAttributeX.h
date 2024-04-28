// This file is part of the KDE project
// SPDX-FileCopyrightText: 2010 Benjamin Port <port.benjamin@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

#ifndef KPRATTRIBUTEX_H
#define KPRATTRIBUTEX_H

#include "KPrAnimationAttribute.h"

class KPrAttributeX : public KPrAnimationAttribute
{
public:
    KPrAttributeX();
    void updateCache(KPrAnimationCache *cache, KPrShapeAnimation *shapeAnimation, qreal value) override;
    void initCache(KPrAnimationCache *cache, int step, KPrShapeAnimation *shapeAnimation, qreal startValue, qreal endValue) override;
};

#endif // KPRATTRIBUTEX_H
