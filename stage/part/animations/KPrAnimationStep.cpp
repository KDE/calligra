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

#include "KPrAnimationStep.h"
#include "KPrAnimationSubStep.h"
#include "KoXmlWriter.h"
#include "KoShape.h"
#include "KLocale"

KPrAnimationStep::KPrAnimationStep()
    : m_triggerEvent(KPrAnimationStep::On_Click)
    , m_class(KPrAnimationStep::Custom)
    , m_id(i18n("stage-unrecognized"))
    , m_state(KPrAnimationStep::Invalid)
    , m_targetElement(0)
{
}

KPrAnimationStep::~KPrAnimationStep()
{
}

void KPrAnimationStep::init(KPrAnimationCache *animationCache, int step)
{
    for(int i=0;i < this->animationCount(); i++) {
        QAbstractAnimation * animation = this->animationAt(i);
        if (KPrAnimationSubStep * a = dynamic_cast<KPrAnimationSubStep*>(animation)) {
            a->init(animationCache, step);
        }
    }
}

bool KPrAnimationStep::saveOdf(KoPASavingContext & paContext) const
{
    KoXmlWriter &writer = paContext.xmlWriter();
    writer.startElement("anim:par");
    for (int i=0; i < this->animationCount(); i++) {
        bool startStep = !i;
        QAbstractAnimation *animation = this->animationAt(i);
        if (KPrAnimationSubStep *a = dynamic_cast<KPrAnimationSubStep*>(animation)) {
            a->saveOdf(paContext, startStep, presetClassText(), id());
        }
    }
    writer.endElement();
    return true;
}

void KPrAnimationStep::deactivate()
{
    for (int i=0; i < this->animationCount(); i++) {
        QAbstractAnimation *animation = this->animationAt(i);
        if (KPrAnimationSubStep *a = dynamic_cast<KPrAnimationSubStep*>(animation)) {
            a->deactivate();
        }
    }
}

void KPrAnimationStep::setNodeType(KPrAnimationStep::Node_Type type)
{
    m_triggerEvent = type;
}

void KPrAnimationStep::setPresetClass(KPrAnimationStep::Preset_Class presetClass)
{
    m_class = presetClass;
}

void KPrAnimationStep::setId(QString id)
{
    m_id = id;
}

void KPrAnimationStep::setAnimationState(KPrAnimationStep::Animation_State state)
{
    m_state = state;
}

void KPrAnimationStep::setTargetElement(KoShape *shape)
{
    m_targetElement = shape;
}

KPrAnimationStep::Node_Type KPrAnimationStep::NodeType() const
{
    return m_triggerEvent;
}

KPrAnimationStep::Preset_Class KPrAnimationStep::presetClass() const
{
    return m_class;
}

KPrAnimationStep::Animation_State KPrAnimationStep::animationState() const
{
    return m_state;
}

QString KPrAnimationStep::id() const
{
    return m_id;
}

QString KPrAnimationStep::presetClassText() const
{
    if (presetClass() == KPrAnimationStep::Emphasis) {
        return QString("emphasis");
    }
    else if (presetClass() == KPrAnimationStep::Entrance) {
        return QString("entrance");
    }
    else if (presetClass() == KPrAnimationStep::Exit) {
        return QString("exit");
    }
    else if (presetClass() == KPrAnimationStep::Motion_Path) {
        return QString("motion-path");
    }
    else if (presetClass() == KPrAnimationStep::Ole_Action) {
        return QString("ole-action");
    }
    else if (presetClass() == KPrAnimationStep::Media_Call) {
        return QString("media-call");
    }
    else {
        return QString("custom");
    }
}

KoShape *KPrAnimationStep::targetElement() const
{
    return m_targetElement;
}

QPair<int, int> KPrAnimationStep::timeRange()
{
    int minStart = 99999;
    int maxEnd = 0;
    for (int i=0; i < this->animationCount(); i++) {
        QAbstractAnimation *animation = this->animationAt(i);
        if (KPrAnimationSubStep *a = dynamic_cast<KPrAnimationSubStep*>(animation)) {
            minStart = qMin(minStart, a->timeRange().first);
            maxEnd = qMax(maxEnd, a->timeRange().second);
        }
    }
    QPair<int, int> pair;
    pair.first = minStart;
    pair.second = maxEnd;
    return pair;
}
