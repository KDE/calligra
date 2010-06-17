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
    virtual bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context);
    virtual void saveOdf(KoShapeSavingContext &context) const = 0;

    virtual int duration() const;
    virtual void init(KPrAnimationCache *animationCache, int step) = 0;
    int animationDuration() const;
protected:
    virtual void updateCurrentTime(int currentTime);
    virtual void next(int currentTime) = 0;
    void updateCache(const QString &id, const QVariant &value);


    KPrShapeAnimation *m_shapeAnimation; // we could also use the group() but that would mean we need to cast all the time
    KPrAnimationCache * m_animationCache;
    int m_begin; // in milliseconds
    int m_duration; // in milliseconds
};

#endif /* KPRANIMATIONBASE_H */
