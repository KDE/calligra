/* This file is part of the KDE project
 * Copyright (C) 2010 Thorsten Zachmann <zachmann@kde.org>
 * Copyright (C) 2010 Benjamin Port <port.benjamin@gmail.com>
 * Copyright (C) 2012 Paul Mendez <paulestebanms@gmail.com>
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

#include "KPrShapeAnimation.h"
#include "KPrAnimationBase.h"

#include "KoXmlReader.h"
#include "KoXmlWriter.h"
#include "KoShapeLoadingContext.h"
#include <KoPASavingContext.h>

#include "KoTextBlockData.h"
#include "KPrTextBlockPaintStrategy.h"

KPrShapeAnimation::KPrShapeAnimation(KoShape *shape, QTextBlockUserData *textBlockUserData)
: m_shape(shape)
, m_textBlockData(textBlockUserData)
, m_class(KPrShapeAnimation::None)
, m_step(0)
, m_subStep(0)
, m_stepIndex(-1)
, m_subStepIndex(-1)
, m_animIndex(-1)
{
    // this is needed so we save the xml id's on saving and therefor are able to
    // save animation back even when they have not yet run.
    if (m_textBlockData) {
        KoTextBlockData(m_textBlockData).setPaintStrategy(new KoTextBlockPaintStrategyBase());
    }
}

KPrShapeAnimation::~KPrShapeAnimation()
{
}

bool KPrShapeAnimation::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
    return false;
}

bool KPrShapeAnimation::saveOdf(KoPASavingContext &paContext, bool startStep, bool startSubStep) const
{
    KoXmlWriter &writer = paContext.xmlWriter();
    writer.startElement("anim:par");
    QString nodeType;
    QString l_presetClass = presetClassText();
    QString l_id = id();
    QString l_presetSubType = presetSubType();
    if (startStep && startSubStep) {
        nodeType = QString("on-click");
    }
    else if (startSubStep) {
        nodeType = QString("after-previous");
    }
    else {
        nodeType = QString("with-previous");
    }

    writer.addAttribute("presentation:node-type", nodeType);
    if (!l_presetClass.isEmpty()) {
        writer.addAttribute("presentation:preset-class", l_presetClass);
    }
    if (!l_id.isEmpty()) {
        writer.addAttribute("presentation:preset-id", l_id);
    }
    if (!l_presetSubType.isEmpty()) {
        writer.addAttribute("presentation:preset-sub-type", l_presetSubType);
    }
    for(int i = 0 ;i < this->animationCount(); i++) {
        QAbstractAnimation *animation = this->animationAt(i);
        if (KPrAnimationBase *a = dynamic_cast<KPrAnimationBase *>(animation)) {
            a->saveOdf(paContext);
        }
    }
    writer.endElement();
    return true;
}

KoShape * KPrShapeAnimation::shape() const
{
    return m_shape;
}

QTextBlockUserData *KPrShapeAnimation::textBlockUserData() const
{
    return m_textBlockData;
}

void KPrShapeAnimation::init(KPrAnimationCache *animationCache, int step)
{
    if (m_textBlockData) {
        KoTextBlockData(m_textBlockData).setPaintStrategy(new KPrTextBlockPaintStrategy(m_textBlockData, animationCache));
    }
    for (int i = 0; i < this->animationCount(); ++i) {
        QAbstractAnimation *animation = this->animationAt(i);
        if (KPrAnimationBase *a = dynamic_cast<KPrAnimationBase *>(animation)) {
            a->init(animationCache, step);
        }
    }
}

QPair<int, int> KPrShapeAnimation::timeRange() const
{
    const int INVALID_START = 99999;
    int minStart = INVALID_START;
    int maxEnd = 0;

    for (int i = 0;i < this->animationCount(); i++) {
        QAbstractAnimation * animation = this->animationAt(i);
        if (KPrAnimationBase *a = dynamic_cast<KPrAnimationBase *>(animation)) {
            minStart = qMin(minStart, a->begin());
            maxEnd = qMax(maxEnd, a->duration());
        }
    }
    QPair<int, int> pair;
    pair.first = (minStart == INVALID_START)? 0: minStart;
    pair.second = maxEnd;
    return pair;
}

int KPrShapeAnimation::globalDuration() const
{
    QPair<int, int> range = timeRange();
    return range.second - range.first;
}

void KPrShapeAnimation::setBeginTime(int timeMS)
{
    if (timeMS < 0) {
        return;
    }
    //Add timeMS to all animations begin time, relative to the first animation
    int minStart = timeRange().first;
    int timeDiff = timeMS - minStart;
    for (int i = 0;i < this->animationCount(); i++) {
        QAbstractAnimation *animation = this->animationAt(i);
        if (KPrAnimationBase *a = dynamic_cast<KPrAnimationBase *>(animation)) {
            a->setBegin(a->begin() + timeDiff);
        }
    }
    emit timeChanged(timeMS, timeRange().second);
}

void KPrShapeAnimation::setGlobalDuration(int timeMS)
{
    if (timeMS < 100) {
        return;
    }
    //Add timeMS duration to all animations, proportional to the max duration
    QPair<int, int> range = timeRange();
    int maxDuration = range.second - range.first;
    int minStart = range.first;
    qreal timeRatio = timeMS / (qreal)maxDuration;
    for (int i = 0;i < this->animationCount(); i++) {
        QAbstractAnimation *animation = this->animationAt(i);
        if (KPrAnimationBase *a = dynamic_cast<KPrAnimationBase *>(animation)) {
            a->setDuration((a->duration()-a->begin()) * timeRatio);
            a->setBegin((a->begin() - minStart) * timeRatio + minStart);
        }
    }
    emit timeChanged(timeRange().first, timeMS);
}

void KPrShapeAnimation::setTextBlockUserData(QTextBlockUserData *textBlockUserData)
{
    if (textBlockUserData) {
        m_textBlockData = textBlockUserData;
        KoTextBlockData(m_textBlockData).setPaintStrategy(new KoTextBlockPaintStrategyBase());
    }
}

void KPrShapeAnimation::deactivate()
{
    if (m_textBlockData) {
        KoTextBlockData(m_textBlockData).setPaintStrategy(new KoTextBlockPaintStrategyBase());
    }
}

// we could have a loader that would put the data into the correct pos
// KPrShapeAnimation would get all the data it would need
// onClick would create a new animation
// when putting data in it could check if the shape is the correct one if not create a parallel one (with previous)

void KPrShapeAnimation::setPresetClass(KPrShapeAnimation::PresetClass presetClass)
{
    m_class = presetClass;
}

void KPrShapeAnimation::setId(const QString &id)
{
    m_id = id;
}

void KPrShapeAnimation::setPresetSubType(const QString &subType)
{
    m_presetSubType = subType;
}

KPrShapeAnimation::PresetClass KPrShapeAnimation::presetClass() const
{
    return m_class;
}

QString KPrShapeAnimation::id() const
{
    return m_id;
}

QString KPrShapeAnimation::presetClassText() const
{
    if (presetClass() == KPrShapeAnimation::None) {
        return QString();
    }
    if (presetClass() == KPrShapeAnimation::Emphasis) {
        return QString("emphasis");
    }
    else if (presetClass() == KPrShapeAnimation::Entrance) {
        return QString("entrance");
    }
    else if (presetClass() == KPrShapeAnimation::Exit) {
        return QString("exit");
    }
    else if (presetClass() == KPrShapeAnimation::MotionPath) {
        return QString("motion-path");
    }
    else if (presetClass() == KPrShapeAnimation::OleAction) {
        return QString("ole-action");
    }
    else if (presetClass() == KPrShapeAnimation::MediaCall) {
        return QString("media-call");
    }
    else {
        return QString("custom");
    }
}

QString KPrShapeAnimation::presetSubType() const
{
    return m_presetSubType;
}

void KPrShapeAnimation::setStep(KPrAnimationStep *step)
{
    if (step != m_step) {
        m_step = step;
    }
}

void KPrShapeAnimation::setSubStep(KPrAnimationSubStep *subStep)
{
    if (subStep != m_subStep) {
        m_subStep = subStep;
    }
}

KPrAnimationStep *KPrShapeAnimation::step() const
{
    return m_step;
}

KPrAnimationSubStep *KPrShapeAnimation::subStep() const
{
    return m_subStep;
}

void KPrShapeAnimation::setStepIndex(int index)
{
    m_stepIndex = index;
}

void KPrShapeAnimation::setSubStepIndex(int index)
{
    m_subStepIndex = index;
}

void KPrShapeAnimation::setAnimIndex(int index)
{
    m_animIndex = index;
}

int KPrShapeAnimation::stepIndex() const
{
    return m_stepIndex;
}

int KPrShapeAnimation::subStepIndex() const
{
    return m_subStepIndex;
}

int KPrShapeAnimation::animIndex() const
{
    return m_animIndex;
}
