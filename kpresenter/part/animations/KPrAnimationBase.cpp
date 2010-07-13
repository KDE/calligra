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

#include "KPrAnimationBase.h"

#include <KoXmlNS.h>
#include "KPrDurationParser.h"
#include "KoXmlReader.h"
#include "KoXmlWriter.h"
#include "KPrAnimationCache.h"
#include "KPrShapeAnimation.h"
#include "KoShapeLoadingContext.h"

KPrAnimationBase::KPrAnimationBase(KPrShapeAnimation *shapeAnimation)
: m_shapeAnimation(shapeAnimation)
, m_begin(0)
,m_duration(1)
{
}

KPrAnimationBase::~KPrAnimationBase()
{
}

int KPrAnimationBase::duration() const
{
    return m_duration;
}

bool KPrAnimationBase::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    Q_UNUSED(context)
    m_begin = KPrDurationParser::durationMs(element.attributeNS(KoXmlNS::smil, "begin"));
    if (m_begin == -1) {
        m_begin = 0;
    }
    m_duration = KPrDurationParser::durationMs(element.attributeNS(KoXmlNS::smil, "dur"));
    if (m_duration == -1) {
        m_duration = 1;
    }
    m_duration += m_begin;
    return true;
}

void KPrAnimationBase::updateCache(const QString &id, const QVariant &value)
{
    m_animationCache->update(m_shapeAnimation->shape(), m_shapeAnimation->textBlockData(), id, value);
}

void KPrAnimationBase::updateCurrentTime(int currentTime)
{
    if (currentTime >= m_begin) {
        next(currentTime - m_begin);
    }
}

int KPrAnimationBase::animationDuration() const
{
    return totalDuration() - m_begin;
}

bool KPrAnimationBase::saveAttribute(KoPASavingContext &paContext) const
{
    KoXmlWriter &writer = paContext.xmlWriter();
    writer.addAttribute("smil:begin", KPrDurationParser::msToString(m_begin));
    writer.addAttribute("smil:dur", KPrDurationParser::msToString(m_duration));
    writer.addAttribute("smil:targetElement", paContext.drawId(m_shapeAnimation->shape(), false));
    return true;
}
