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

#include "KPrAttributeHeight.h"
#include "../KPrAnimationCache.h"
#include "KoShape.h"

#include "kdebug.h"

KPrAttributeHeight::KPrAttributeHeight() : KPrAnimationAttribute("height")
{
}

void KPrAttributeHeight::updateCache(KPrAnimationCache *cache, KoShape *shape, qreal value)
{
    QTransform transform;
    value = value * cache->pageSize().height() / shape->size().height();
    qreal tx = shape->size().width() * cache->zoom() / 2;
    qreal ty = shape->size().height() * cache->zoom() / 2;
    transform.translate(tx, ty).scale(1, value).translate(-tx, -ty);
    cache->update(shape, "transform", transform);
}

void KPrAttributeHeight::initCache(KPrAnimationCache *animationCache, int step, KoShape * shape, qreal startValue, qreal endValue)
{
    qreal v1 = startValue * animationCache->pageSize().height() / shape->size().height();
    qreal v2 = endValue * animationCache->pageSize().height() / shape->size().height();
    qreal tx = shape->size().width() * animationCache->zoom() / 2;
    qreal ty = shape->size().height() * animationCache->zoom() / 2;
    animationCache->init(step, shape, "transform", QTransform().translate(tx, ty).scale(1, v1).translate(-tx, -ty));
    animationCache->init(step + 1, shape, "transform", QTransform().translate(tx, ty).scale(1, v2).translate(-tx, -ty));
}
