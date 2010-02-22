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

#include "KPrAnimationFactory.h"

#include "KPrAnimSeq.h"
#include "KPrAnimSet.h"
#include "KPrAnimPar.h"
#include "KPrAnimate.h"
#include "KPrAnimateColor.h"
#include "KPrAnimateMotion.h"
#include "KPrAnimateTransform.h"

#include <KoXmlReader.h>
#include <KoXmlNS.h>
#include <KoShapeLoadingContext.h>

KPrAnimationBase * KPrAnimationFactory::createAnimationFromOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    KPrAnimationBase * animation = 0;
    if (element.namespaceURI() == KoXmlNS::anim) {
        if (element.tagName() == "par") {
            animation = new KPrAnimPar();
        }
        else if (element.tagName() == "seq") {
            animation = new KPrAnimSeq();
        }
        else if (element.tagName() == "set") {
            animation = new KPrAnimSet();
        }
        else if (element.tagName() == "animate") {
            animation = new KPrAnimate();
        }
        else if (element.tagName() == "animateMotion") {
            animation = new KPrAnimateMotion();
        }
        else if (element.tagName() == "animateColor") {
            animation = new KPrAnimateColor();
        }
        else if (element.tagName() == "animationTransform") {
            animation = new KPrAnimateTransform();
        }
        else if (element.tagName() == "transitionFilter") {
        }

        if (animation) {
            animation->loadOdf(element, context);
        }
        else {
        }
    }
    return animation;
}
