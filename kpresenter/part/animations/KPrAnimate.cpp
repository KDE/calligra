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
#include "KPrShapeAnimation.h"

#include "strategy/KPrSmilValues.h"
#include "strategy/KPrAnimationValue.h""
#include "strategy/KPrAnimationAttribute.h"
#include "strategy/KPrAttributeX.h"
#include "strategy/KPrAttributeY.h"

#include "KoShape.h"
#include <kdebug.h>

KPrAnimate::KPrAnimate(KPrShapeAnimation *shapeAnimation)
: KPrAnimationBase(shapeAnimation)
,m_attribute(0)
,m_values(0)
{
}

KPrAnimate::~KPrAnimate()
{
    if(m_attribute)
        delete m_attribute;
    if(m_values)
        delete m_values;
}

bool KPrAnimate::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    KPrAnimationBase::loadOdf(element, context);
    bool retval = true;
    // attributeName
    QString attributeName(element.attributeNS(KoXmlNS::smil, "attributeName", QString()));
    if (attributeName == "x") {
        m_attribute = new KPrAttributeX();
    }
    else if (attributeName == "y") {
        m_attribute = new KPrAttributeY();
    }
    else {
        kWarning(33003) << "attributeName" << attributeName << "not yet supported";
        retval = false;
    }

    if (!retval){
        return false;
    }

    // calcMode
    KPrAnimationValue::SmilCalcMode smilCalcMode = KPrAnimationValue::linear;
    QString calcMode = element.attributeNS(KoXmlNS::smil, "calcMode", "linear");
    if(calcMode == "linear"){
        smilCalcMode = KPrAnimationValue::linear;
    } else if (calcMode == "discrete") {
        smilCalcMode = KPrAnimationValue::discrete;
        kWarning(33003) << "calcMode discrete not yes supported";
        retval = false;
    } else if (calcMode == "paced") {
        smilCalcMode = KPrAnimationValue::paced;
        kWarning(33003) << "calcMode paced not yes supported";
        retval = false;
    } else if (calcMode == "spline") {
        smilCalcMode = KPrAnimationValue::spline;
        kWarning(33003) << "calcMode spline not yes supported";
        retval = false;
    }


    // value
    QString formula = element.attributeNS(KoXmlNS::anim, "formula", QString());
    if (!formula.isEmpty()) {
        // formula
    }
    else {
        QString values = element.attributeNS(KoXmlNS::smil, "values", QString());
        if (!values.isEmpty()) {
            QString keyTimes = element.attributeNS(KoXmlNS::smil, "keyTimes", QString());
            QString keySplines = element.attributeNS(KoXmlNS::smil, "keySplines", QString());
            KPrSmilValues * smilValue = new KPrSmilValues();
            retval = retval && smilValue->loadValues(values, keyTimes, keySplines, smilCalcMode);
            m_values = smilValue;
        }
        else {
            QString from = element.attributeNS(KoXmlNS::smil, "from", "0");
            QString to = element.attributeNS(KoXmlNS::smil, "to", "0");
            QString by = element.attributeNS(KoXmlNS::smil, "by", "0");

        }
    }
    return retval;
}

void KPrAnimate::saveOdf(KoShapeSavingContext &context) const
{
    Q_UNUSED(context);
}

void KPrAnimate::init(KPrAnimationCache *animationCache, int step)
{
    m_animationCache = animationCache;
    m_attribute->initCache(animationCache, step, m_shapeAnimation->shape(), m_values->startValue(), m_values->endValue());
}

void KPrAnimate::next(int currentTime)
{
    qreal value = m_values->value(qreal(currentTime)/qreal(animationDuration()));
    m_attribute->updateCache(m_animationCache, m_shapeAnimation->shape(), value);
}
