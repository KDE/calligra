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

#include "KPrAnimSeq.h"

#include <KoXmlReader.h>
#include "KPrAnimationFactory.h"
#include "KPrAnimationCache.h"

#include <KoShapeLoadingContext.h>
#include <KoShapeSavingContext.h>

KPrAnimSeq::KPrAnimSeq()
{
}

KPrAnimSeq::~KPrAnimSeq()
{
}

bool KPrAnimSeq::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    KoXmlElement e;
    forEachElement(e, element) {
        KPrAnimationBase * animation = KPrAnimationFactory::createAnimationFromOdf(e, context);
        if (animation) {
            m_animations.append(animation);
        }
    }
    return true;
}

void KPrAnimSeq::saveOdf(KoShapeSavingContext &context) const
{
    Q_UNUSED(context);
}

void KPrAnimSeq::init(KPrAnimationCache *animationCache) const
{
    foreach(KPrAnimationBase *animation, m_animations) {
        animation->init(animationCache);
    }
}
