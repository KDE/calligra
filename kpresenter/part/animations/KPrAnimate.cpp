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

#include "KPrAnimate.h"

#include "KPrAnimationCache.h"
#include <KoXmlNS.h>
#include <KoXmlReader.h>
#include <KoShapeLoadingContext.h>
#include <KoShapeSavingContext.h>
#include <KoTextBlockData.h>
#include <KoShapeLoadingContext.h>
#include <KoShapeSavingContext.h>
#include <KoXmlReader.h>

#include "KPrAnimationCache.h"

#include <kdebug.h>

KPrAnimate::KPrAnimate(KPrShapeAnimation *shapeAnimation)
: KPrAnimationBase(shapeAnimation)
{
}

KPrAnimate::~KPrAnimate()
{
}

bool KPrAnimate::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    bool retval = false;
#if 0
    QString targetElement(element.attributeNS(KoXmlNS::smil, "targetElement", QString()));
    if (!targetElement.isEmpty()) {
        bool isText = element.attributeNS(KoXmlNS::anim, "sub-item", "whole") == "text";
        if(isText) {
            QPair<KoShape *, QVariant> pair = context.shapeSubItemById(targetElement);
            m_shape = pair.first;
            m_textBlockData = pair.second.value<KoTextBlockData *>();
        } else {
            m_shape = context.shapeById(targetElement);
        }

        if (m_shape) {
            QString attributeName(element.attributeNS(KoXmlNS::smil, "attributeName", QString()));
            if (attributeName == "x") {
                QString from = element.attributeNS(KoXmlNS::smil, "from", "0");
                QString to = element.attributeNS(KoXmlNS::smil, "to", "0");
                kDebug(33003) << "animate x for shape with id" << targetElement << from << to;
            }
            else if (attributeName == "y") {
                QString from = element.attributeNS(KoXmlNS::smil, "from", "0");
                QString to = element.attributeNS(KoXmlNS::smil, "to", "0");
                kDebug(33003) << "animate y for shape with id" << targetElement << from << to;
            }
            else {
                kWarning(33003) << "attributeName" << attributeName << "not yet supported";
            }
        }
        else {
            kWarning(33003) << "shape not found for id" << targetElement;
        }
    }
    else {
        kWarning(33003) << "target element not found";
    }
#endif
    return retval;
}

void KPrAnimate::saveOdf(KoShapeSavingContext &context) const
{
    Q_UNUSED(context);
}

void KPrAnimate::init(KPrAnimationCache *animationCache, int step)
{
    Q_UNUSED(animationCache);
}

void KPrAnimate::updateCurrentTime(int currentTime)
{
}
