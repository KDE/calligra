/* This file is part of the KDE project
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


#include "KPrAnimationEditNodeTypeCommand.h"

#include "animations/KPrShapeAnimation.h"
#include "animations/KPrAnimationStep.h"
#include "animations/KPrAnimationSubStep.h"
#include "KLocale"
#include "KDebug"

KPrAnimationEditNodeTypeCommand::KPrAnimationEditNodeTypeCommand(KPrShapeAnimation *animation,
                                                                 KPrAnimationStep *newStep, KPrAnimationSubStep *newSubStep,
                                                                 KPrShapeAnimation::Node_Type newType, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_animation(animation)
    , m_newStep(newStep)
    , m_newSubStep(newSubStep)
    , m_newType(newType)
{
    setText(i18nc("(qtundo-format)", "Edit animation trigger event" ) );
    m_oldStep = m_animation->step();
    m_oldSubStep = m_animation->subStep();
    m_oldType = m_animation->NodeType();

}

KPrAnimationEditNodeTypeCommand::~KPrAnimationEditNodeTypeCommand()
{
}

void KPrAnimationEditNodeTypeCommand::redo()
{
    if (m_animation) {
        if (m_newSubStep != m_oldSubStep) {
            m_newSubStep->addAnimation(m_oldSubStep->takeAnimation(m_oldSubStep->indexOfAnimation(m_animation)));
            m_animation->setSubStep(m_newSubStep);
        }
        if (m_newStep != m_newSubStep->parent()) {
            m_newStep->addAnimation(m_newSubStep);
            m_animation->setStep(m_newStep);
        }
        m_animation->setNodeType(m_newType);
    }
}

void KPrAnimationEditNodeTypeCommand::undo()
{
    if (m_animation) {
        if (m_newSubStep != m_oldSubStep) {
            m_oldSubStep->addAnimation(m_newSubStep->takeAnimation(m_newSubStep->indexOfAnimation(m_animation)));
            m_animation->setSubStep(m_oldSubStep);
        }
        if (m_oldStep != m_oldSubStep->parent()) {
            m_oldStep->addAnimation(m_oldSubStep);
            m_animation->setStep(m_oldStep);
        }
        m_animation->setNodeType(m_oldType);
    }
}
