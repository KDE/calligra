/* This file is part of the KDE project
 * Copyright (C) 2010 Thorsten Zachmann <zachmann@kde.org>
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

#ifndef KPRANIMATIONBASE_H
#define KPRANIMATIONBASE_H

#include <QAbstractAnimation>
#include "KPrAnimationData.h"

class KoXmlElement;
class KoShapeLoadingContext;
class KoShapeSavingContext;
class KoShape;
class KoTextBlockData;
class KPrAnimationCache;
class KPrShapeAnimation;

class KPrAnimationBase : public QAbstractAnimation, KPrAnimationData
{
public:
    KPrAnimationBase(KPrShapeAnimation *shapeAnimation);
    virtual ~KPrAnimationBase();

    virtual bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context) = 0;
    virtual void saveOdf(KoShapeSavingContext &context) const = 0;

#if XXX
    // how do I know which step we are in? only animations for the current step should do something
    // in the cache we should set the values for the current step freshly again and again
    // stuff that is still in the cache when a step is done should be kept there. if it is no longer relevant it should#
    // be no longer in the step of the cache
    // 1. Init cache with values that take effect even before the animation is started
    //    This information should be kept in the first stack entry
    //    I think this is only needed for visibility as the other take only effect in the presentation step
    // 2. For the animation on 1. step
    //    copy falues form previous step that are still there.
    //    update values with the values from the animations of that current step
    //    when all is finished start with the next step
    //    do the same for each time the effect is triggert until all effects in this step are finished.
    // go on with step 2 until there are no more steps left
    virtual void updateCache(int currentTime)
#endif

    virtual int duration() const;
    virtual void init(KPrAnimationCache *animationCache, int step) = 0;
protected:
    virtual void updateCurrentTime(int currentTime) = 0;

    KPrShapeAnimation *m_shapeAnimation; // we could also use the group() but that would mean we need to cast all the time
    KPrAnimationCache * m_animationCache;
    int m_begin; // in milliseconds
    int m_duration; // in milliseconds
};

#endif /* KPRANIMATIONBASE_H */
