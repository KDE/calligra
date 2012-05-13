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

#include "KPrAnimTransitionFilter.h"
#include "KoShapeSavingContext.h"
#include <KoXmlNS.h>

#include "KPrTransitionFilterRegistry.h"
#include "KPrAnimTransitionFilterEffect.h"


KPrAnimTransitionFilter::KPrAnimTransitionFilter(KPrShapeAnimation *shapeAnimation)
: KPrAnimationBase(shapeAnimation)
,m_effect(0)
{
}

KPrAnimTransitionFilter::~KPrAnimTransitionFilter()
{
    if(m_effect) {
        delete m_effect;
    }
}

bool KPrAnimTransitionFilter::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{

    KPrAnimationBase::loadOdf(element, context);
    if ( element.hasAttributeNS( KoXmlNS::smil, "type" ) ) {
        QString smilType(element.attributeNS(KoXmlNS::smil,"type"));
        qDebug() << "has type: " << smilType;
        QString smilSubType( element.attributeNS( KoXmlNS::smil, "subtype" ) );
        qDebug() << smilSubType;
        bool reverse = false;
        if ( element.attributeNS( KoXmlNS::smil, "direction" ) == "reverse" ) {
            qDebug() << "has reverse";
            reverse = true;
        }
        qDebug() << "Ready to create object in AnimTransitionFilter.cpp";
        m_effect = KPrTransitionFilterRegistry::instance()->createTransitionFilterEffect(element);
    }
    return true;
}

bool KPrAnimTransitionFilter::saveOdf(KoPASavingContext & paContext) const
{
    KoXmlWriter &writer = paContext.xmlWriter();
    writer.startElement("anim:transitionFilter");
    saveAttribute(paContext);
    writer.endElement();
    return true;
}

void KPrAnimTransitionFilter::init(KPrAnimationCache *animationCache, int step)
{
    Q_UNUSED(animationCache);
    Q_UNUSED(step);
}

void KPrAnimTransitionFilter::next(int currentTime)
{
    Q_UNUSED(currentTime);
}
