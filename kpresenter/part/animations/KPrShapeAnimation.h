/* This file is part of the KDE project
 * Copyright (C) 2010 Thorsten Zachmann <zachmann@kde.org>
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

#ifndef KPRSHAPEANIMATION_H
#define KPRSHAPEANIMATION_H

#include <QParallelAnimationGroup>
#include "KPrAnimationData.h"
#include <QPair>

class KoShape;
class KoTextBlockData;
class KoXmlElement;
class KoShapeLoadingContext;
class KoShapeSavingContext;
class KPrAnimationCache;

class KPrShapeAnimation : public QParallelAnimationGroup, KPrAnimationData
{
public:
    KPrShapeAnimation(KoShape *shape, KoTextBlockData *textBlockData);
    virtual ~KPrShapeAnimation();

    bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context);
    virtual bool saveOdf(KoPASavingContext &paContext, bool startStep, bool startSubStep) const;

    KoShape * shape() const;
    virtual void init(KPrAnimationCache *animationCache, int step);

    /**
     * @return true if this shape animation change the visibility
     */
    bool visibilityChange();

    /**
     * @return true if this shape animation enable visibility of the shape
     */
    bool visible();

    /**
     * Read the value from the first KPrAnimationBase object
     */
    //QPair<KoShape *, KoTextBlockData *> animationShape() const;

private:
    KoShape *m_shape;
    KoTextBlockData *m_textBlockData;
};

#endif /* KPRSHAPEANIMATION_H */
