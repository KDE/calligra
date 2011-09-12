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

#ifndef KPRANIMATE_H
#define KPRANIMATE_H

#include "KPrAnimationBase.h"

class KPrAnimationValue;
class KPrAnimationAttribute;

class KPrAnimate : public KPrAnimationBase
{
public:
    KPrAnimate(KPrShapeAnimation *shapeAnimation);
    virtual ~KPrAnimate();

    virtual bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context);
    virtual bool saveOdf(KoPASavingContext & paContext) const;
    virtual void init(KPrAnimationCache *animationCache, int step);
    virtual bool saveAttribute(KoPASavingContext &paContext) const;
protected:
    virtual void next(int currentTime);

private:
    KPrAnimationAttribute * m_attribute;
    KPrAnimationValue * m_values;
};

#endif /* KPRANIMATE_H */
