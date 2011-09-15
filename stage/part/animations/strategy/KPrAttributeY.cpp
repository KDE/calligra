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

#include "KPrAttributeY.h"
#include "../KPrAnimationCache.h"
#include "KoShape.h"
#include "../KPrShapeAnimation.h"

#include "kdebug.h"

KPrAttributeY::KPrAttributeY() : KPrAnimationAttribute("y")
{
}

void KPrAttributeY::updateCache(KPrAnimationCache *cache, KPrShapeAnimation *shapeAnimation, qreal value)
{
    KoShape *shape = shapeAnimation->shape();
    QTransform transform;
    value = value * cache->pageSize().height();
    value = value - shape->position().y();
    value = value * cache->zoom();
    transform.translate(0, value);
    cache->update(shape, shapeAnimation->textBlockData(), "transform", transform);
}

void KPrAttributeY::initCache(KPrAnimationCache *animationCache, int step, KPrShapeAnimation * shapeAnimation, qreal startValue, qreal endValue)
{
    KoShape * shape = shapeAnimation->shape();
    qreal v1 = (startValue * animationCache->pageSize().height() - shape->position().y()) * animationCache->zoom();
    qreal v2 = (endValue * animationCache->pageSize().height() - shape->position().y()) * animationCache->zoom();
    animationCache->init(step, shape, shapeAnimation->textBlockData(), "transform", QTransform().translate(0, v1));
    animationCache->init(step + 1, shape, shapeAnimation->textBlockData(), "transform", QTransform().translate(0, v2));
}
