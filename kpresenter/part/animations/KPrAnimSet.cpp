
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

#include "KPrAnimSet.h"

#include <QString>
#include <KoXmlNS.h>
#include <KoXmlReader.h>
#include <KoShapeLoadingContext.h>
#include <KoShapeSavingContext.h>
#include <KoTextBlockData.h>
#include "KPrAnimationCache.h"
#include "KPrTextBlockPaintStrategy.h"

#include <kdebug.h>

KPrAnimSet::KPrAnimSet(KPrShapeAnimation *shapeAnimation)
: KPrAnimationBase(shapeAnimation)
{
}

KPrAnimSet::~KPrAnimSet()
{
}

bool KPrAnimSet::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    bool retval = false;

    QString attributeName(element.attributeNS(KoXmlNS::smil, "attributeName", QString()));
    if (attributeName == "visibility") {
        m_visible = element.attributeNS(KoXmlNS::smil, "to", "hidden") == "visible";
        retval = true;
        kDebug(33003) << "animate visibility for shape with id" << m_visible;
    }
    else {
        kWarning(33003) << "attributeName" << attributeName << "not yet supported";
    }

    return retval;
}

void KPrAnimSet::saveOdf(KoShapeSavingContext &context) const
{
    Q_UNUSED(context);
}


void KPrAnimSet::init(KPrAnimationCache *animationCache, int step) const
{
#if 0
    QPair<KoShape *, KoTextBlockData *> m_shapeAnimation->animationShape() const;

    if (m_textBlockData) {
        dynamic_cast<KPrTextBlockPaintStrategy*>(m_textBlockData->paintStrategy())->setAnimationCache(animationCache);
        if (!animationCache->hasValue(m_textBlockData, "visibility")) {
            animationCache->setValue(m_textBlockData, "visibility", !m_visible);
        }
    }
    else {
        if (!animationCache->hasValue(m_shape, "visibility")) {
            animationCache->setValue(m_shape, "visibility", !m_visible);
        }
    }
#endif
}

void KPrAnimSet::updateCurrentTime(int currentTime)
{
}
