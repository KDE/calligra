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

KPrAnimationLoader();
{
}

~KPrAnimationLoader();
{
}

bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context);
{
    // have an overall structure for animations each step needs to be in its own QSequentialAnimationGroup subclass
    // use KPrAnimationStep for that
    KoXmlElement stepElement;
    forEachElement(stepElement, element) {
        if (stepElement.tagName() == "par" && stepElement.namespaceURI() == KoXmlNS::anim) {
            // this creates a new step
            KPrAnimationStep *animationStep = new KPrAnimationStep();

            KoXmlElement parElement;
            forEachElement(parElement, e) {
                if (parElement.tagName() == "par" && parElement.namespaceURI() == KoXmlNS::anim) {
                    loadOdfAnimation(animationStep, parElement, context);
                }
            }
            m_animations.append(animationStep);
        }
        else {
            Q_ASSERT( 0 );
            // accoring to spec there should be onla par elements 
        }
    }
}

bool loadOdfAnimation(KPrAnimationStep *animationStep, const KoXmlElement &element, KoShapeLoadingContext &context)

    KPrShapeAnimation *animation = new KPrShapeAnimation();

    QString nodeType = element.attributeNS(KoXmlNS::presentation, "node-type", "with-previous");

    if (nodeType == "on-click") {
        // if there is allready an aniation create a new step
        if (animationStep->animationCount() != 0) {
            m_animations.append(animationStep);
            animationStep = new KPrAnimationStep();
        }
        animationStep.addAnimation(new QParallelAnimationGroup)
        // add par
        // add par animation
    }
    else if (nodeType == "after-previous") {
        // add to sequence
        // add par
        // add par animation
    }
    else {
        if (nodeType != "with-previous") {
            kWarning(33003) << "unsupported node-type" << nodeType << "found. Using with-previous";
        }
        // add par to current par
    }

    KPrShapeAnimation *shapeAnimation = 0;
    // The shape info and create a KPrShapeAnimation. If there is 
    KoXmlElement e;
    foreach (e, element) {
        if (shapeAnimation == 0) {
            QString targetElement(e.attributeNS(KoXmlNS::smil, "targetElement", QString()));
            if (!targetElement.isEmpty()) {
                KoShape *shape = 0;
                KoTextBlockData *textBlockData;

                if (element.attributeNS(KoXmlNS::anim, "sub-item", "whole") == "text") {
                    QPair<KoShape *, QVariant> pair = context.shapeSubItemById(targetElement);
                    shape = pair.first;
                    textBlockData = pair.second.value<KoTextBlockData *>();
                }
                else {
                    shape = context.shapeById(targetElement);
                }

                if (shape) {
                    shapeAnimation = new KPrShapeAnimation(shape, textBlockData);
                }
            }
        }

        KPrAnimationBase *animation(KPrAnimationFactory::createAnimationFromOdf(e, context, shapeAnimation));
        if (shapeAnimation && animation) {
            shapeAnimation->addAnimation(animation);
        }
    }
}
