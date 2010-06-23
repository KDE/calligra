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

#include "KPrAttributeRotate.h"
#include "../KPrAnimationCache.h"
#include "KoShape.h"

#include "kdebug.h"

KPrAttributeRotate::KPrAttributeRotate()
{
}

void KPrAttributeRotate::updateCache(KPrAnimationCache *cache, KoShape *shape, qreal value)
{
    QTransform transform;
    qreal tx = shape->size().width() * cache->zoom() / 2;
    qreal ty = shape->size().height() * cache->zoom() / 2;
    transform.translate(tx, ty).rotate(value).translate(-tx, -ty);
    cache->update(shape, "transform", transform);
}

void KPrAttributeRotate::initCache(KPrAnimationCache *animationCache, int step, KoShape * shape, qreal startValue, qreal endValue)
{
    qreal tx = shape->size().width() * animationCache->zoom() / 2;
    qreal ty = shape->size().height() * animationCache->zoom() / 2;
    animationCache->init(step, shape, "transform", QTransform().translate(tx, ty).rotate(startValue).translate(-tx, -ty));
    animationCache->init(step + 1, shape, "transform", QTransform().translate(tx, ty).rotate(endValue).translate(-tx, -ty));
}
