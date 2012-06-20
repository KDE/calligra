/* This file is part of the KDE project
 * Copyright ( C ) 2007 Thorsten Zachmann <zachmann@kde.org>
 * Copyright ( C ) 2010 Benjamin Port <port.benjamin@gmail.com>
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
#include <QList>
#include <QSet>
#include <kdebug.h>
#include "KoShape.h"
#include "KPrShapeAnimations.h"
#include "animations/KPrAnimationStep.h"
#include "animations/KPrAnimationSubStep.h"
#include "animations/KPrShapeAnimation.h"

KPrShapeAnimations::KPrShapeAnimations()
{
}

KPrShapeAnimations::~KPrShapeAnimations()
{
}

void KPrShapeAnimations::init(const QList<KPrAnimationStep *> animations)
{
    m_shapeAnimations = animations;
}

void KPrShapeAnimations::add(KPrShapeAnimation * animation)
{
    if (!steps().contains(animation->step())) {
        if ((animation->stepIndex() >= 0) && (animation->stepIndex() <= steps().count())) {
            steps().insert(animation->stepIndex(), animation->step());
        }
        else {
            steps().append(animation->step());
        }
    }
    if (!(animation->step()->indexOfAnimation(animation->subStep()) > 0)) {
        if ((animation->subStepIndex() >= 0) &&
                (animation->subStepIndex() <= animation->step()->animationCount())) {
            animation->step()->insertAnimation(animation->subStepIndex(), animation->subStep());
        }
        else {
            animation->step()->addAnimation(animation);
        }
    }
    if ((animation->animIndex() >= 0) &&
            (animation->animIndex() <= animation->subStep()->animationCount())) {
        animation->subStep()->insertAnimation(animation->animIndex(), animation);
    }
    else {
        animation->subStep()->addAnimation(animation);
    }
    return;
}

void KPrShapeAnimations::remove(KPrShapeAnimation *animation)
{
    KPrAnimationStep *step = animation->step();
    KPrAnimationSubStep *subStep = animation->subStep();
    if (subStep->animationCount() <= 1) {
        if (step->animationCount() <= 1) {
            animation->setStepIndex(steps().indexOf(step));
            steps().removeAll(step);
        }
        animation->setSubStepIndex(step->indexOfAnimation(subStep));
        step->removeAnimation(subStep);
    }
    animation->setAnimIndex(subStep->indexOfAnimation(animation));
    subStep->removeAnimation(animation);
}

QList<KPrAnimationStep *> KPrShapeAnimations::steps() const
{
    return m_shapeAnimations;
}
