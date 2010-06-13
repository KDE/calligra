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

#include "KPrAnimationLoader.h"

#include <KoXmlReader.h>
#include <KoXmlNS.h>
#include <KoShapeLoadingContext.h>
#include <KoTextBlockData.h>

#include <QVariant>
#include <QDomDocument>
#include <QDomNode>

#include <kdebug.h>

#include "KPrShapeAnimation.h"
#include "KPrAnimationBase.h"
#include "KPrAnimationFactory.h"
#include "KPrAnimationStep.h"
#include "KPrAnimationSubStep.h"

KPrAnimationLoader::KPrAnimationLoader()
{
}

KPrAnimationLoader::~KPrAnimationLoader()
{
}

void debugXml(const QString & pos, const KoXmlElement &element)
{
    QByteArray array;
    QDomDocument doc;
    QTextStream st(&array);
    st << element.asQDomNode(doc);
    kDebug() << pos << array;
}

bool KPrAnimationLoader::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    // have an overall structure for animations each step needs to be in its own QSequentialAnimationGroup subclass
    // use KPrAnimationStep for that
    KoXmlElement stepElement;
    forEachElement(stepElement, element) {
        if (stepElement.tagName() == "par" && stepElement.namespaceURI() == KoXmlNS::anim) {
            // this creates a new step
            KPrAnimationStep *animationStep = new KPrAnimationStep();

            KoXmlElement parElement;
            forEachElement(parElement, stepElement) {
                KoXmlElement innerParElement;
                forEachElement(innerParElement, parElement) {
                    if (innerParElement.tagName() == "par" && innerParElement.namespaceURI() == KoXmlNS::anim) {
                        loadOdfAnimation(&animationStep, innerParElement, context);
                    }
                }
            }
            m_animations.append(animationStep);
        }
        else {
            Q_ASSERT( 0 );
            // accoring to spec there should be onla par elements
        }
    }

    debug();

    return true;
}

void KPrAnimationLoader::debug()
{
    foreach(KPrAnimationStep *step, m_animations) {
        kDebug() << "step";
        debug(step, 1);
    }
}

void KPrAnimationLoader::debug(QAbstractAnimation *animation, int level)
{
    QString indent;
    for (int i = 0; i < level; ++i) {
        indent += ' ';
    }
    if (KPrAnimationStep *a = dynamic_cast<KPrAnimationStep*>(animation)) {
        kDebug() << indent + "animation step";
    }
    else if (KPrAnimationSubStep *a = dynamic_cast<KPrAnimationSubStep*>(animation)) {
        kDebug() << indent + "animation sub step";
    }
    else if (KPrShapeAnimation *a = dynamic_cast<KPrShapeAnimation*>(animation)) {
        kDebug() << indent + "shape animation";
    }
    else if (KPrAnimationBase *a = dynamic_cast<KPrAnimationBase*>(animation)) {
        kDebug() << indent + "animation base";
    }

    if (QAnimationGroup *group = dynamic_cast<QAnimationGroup*>(animation)) {
        for (int i = 0; i < group->animationCount(); ++i) {
            debug(group->animationAt(i), level + 1);
        }
    }
}

bool KPrAnimationLoader::loadOdfAnimation(KPrAnimationStep **animationStep, const KoXmlElement &element, KoShapeLoadingContext &context)
{
    QString nodeType = element.attributeNS(KoXmlNS::presentation, "node-type", "with-previous");

    kDebug() << "nodeType:" << nodeType;

    KPrAnimationSubStep *subStep = 0;
    if (nodeType == "on-click") {
        // if there is allready an aniation create a new step
        if ((*animationStep)->animationCount() != 0 || m_animations.isEmpty()) {
            m_animations.append(*animationStep);
            *animationStep = new KPrAnimationStep();
        }
        subStep = new KPrAnimationSubStep();
        (*animationStep)->addAnimation(subStep);
        // add par animation
    }
    else if (nodeType == "after-previous") {
        // add to sequence
        // add par
        subStep = new KPrAnimationSubStep();
        (*animationStep)->addAnimation(subStep);
        // add par animation
    }
    else {
        if (nodeType != "with-previous") {
            kWarning(33003) << "unsupported node-type" << nodeType << "found. Using with-previous";
        }
        // use the current substep
        if ((*animationStep)->animationCount()) {
            subStep = dynamic_cast<KPrAnimationSubStep*>((*animationStep)->animationAt((*animationStep)->animationCount() - 1));
        }
        else {
            subStep = new KPrAnimationSubStep();
            (*animationStep)->addAnimation(subStep);
        }
        // add par to current par
    }

    KPrShapeAnimation *shapeAnimation = 0;
    // The shape info and create a KPrShapeAnimation. If there is
    KoXmlElement e;
    forEachElement(e, element) {
        // TODO add a check that the shape animation is still the correct one
        if (shapeAnimation == 0) {
            QString targetElement(e.attributeNS(KoXmlNS::smil, "targetElement", QString()));
            if (!targetElement.isEmpty()) {
                KoShape *shape = 0;
                KoTextBlockData *textBlockData = 0;

                if (element.attributeNS(KoXmlNS::anim, "sub-item", "whole") == "text") {
                    QPair<KoShape *, QVariant> pair = context.shapeSubItemById(targetElement);
                    shape = pair.first;
                    textBlockData = pair.second.value<KoTextBlockData *>();
                    kDebug() << "subitem text" << textBlockData;
                }
                else {
                    shape = context.shapeById(targetElement);
                }
                kDebug() << "shape:" << shape << "textBlockData" << textBlockData;

                if (shape) {
                    shapeAnimation = new KPrShapeAnimation(shape, textBlockData);
                }
                else {
                    // shape animation not created
                    // TODO logging
                    continue;
                }
            }
        }

        KPrAnimationBase *animation(KPrAnimationFactory::createAnimationFromOdf(e, context, shapeAnimation));
        if (shapeAnimation && animation) {
            shapeAnimation->addAnimation(animation);
        }
    }

    if (shapeAnimation) {
        subStep->addAnimation(shapeAnimation);
    }
    return true;
}

QList<KPrAnimationStep *> KPrAnimationLoader::animations()
{
    return m_animations;
}
