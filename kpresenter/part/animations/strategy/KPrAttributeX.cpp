/* This file is part of the KDE project
 * Copyright (C) 2010 Benjamin Port <port.benjamin@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (  at your option ) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KPrAttributeX.h"
#include "../KPrAnimationCache.h"
#include "KoShape.h"

#include "kdebug.h"

KPrAttributeX::KPrAttributeX() : KPrAnimationAttribute("x")
{
}

void KPrAttributeX::updateCache(KPrAnimationCache *cache, KoShape *shape, qreal value)
{
    QTransform transform;
    value = value * cache->pageSize().width();
    value = value - shape->position().x();
    value = value * cache->zoom();
    transform.translate(value, 0);
    cache->update(shape, "transform", transform);
}

void KPrAttributeX::initCache(KPrAnimationCache *animationCache, int step, KoShape * shape, qreal startValue, qreal endValue)
{
    qreal v1 = startValue * animationCache->pageSize().width() - shape->position().x();
    qreal v2 = endValue * animationCache->pageSize().width() - shape->position().x();
    animationCache->init(step, shape, "transform", QTransform().translate(v1, 0));
    animationCache->init(step + 1, shape, "transform", QTransform().translate(v2, 0));
}
