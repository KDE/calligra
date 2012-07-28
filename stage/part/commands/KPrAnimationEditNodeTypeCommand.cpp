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
#include "KPrPage.h"
#include "KLocale"
#include "KDebug"

const int INVALID = -1;

KPrAnimationEditNodeTypeCommand::KPrAnimationEditNodeTypeCommand(KPrShapeAnimation *animation, KPrAnimationStep *newStep,
                                                                 KPrAnimationSubStep *newSubStep, KPrShapeAnimation::Node_Type newType,
                                                                 QList<KPrShapeAnimation *> children, QList<KPrAnimationSubStep *> movedSubSteps,
                                                                 KPrShapeAnimations *animationModel, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_animation(animation)
    , m_newStep(newStep)
    , m_newSubStep(newSubStep)
    , m_newType(newType)
    , m_children(children)
    , m_substeps(movedSubSteps)
    , m_animationsModel(animationModel)
    , m_oldSubstepRow(INVALID)
    , m_newSubstepRow(INVALID)
    , m_oldStepRow(INVALID)
    , m_newStepRow(INVALID)
{
    setText(i18nc("(qtundo-format)", "Edit animation trigger event" ) );
    m_oldStep = m_animation->step();
    m_oldSubStep = m_animation->subStep();
    m_oldType = animation->NodeType();
}

KPrAnimationEditNodeTypeCommand::~KPrAnimationEditNodeTypeCommand()
{

}

void KPrAnimationEditNodeTypeCommand::redo()
{
    bool notifyOnClickChange = false;
    if (m_animation) {
        // if new subStep reparent main item and children
        if (m_newSubStep != m_oldSubStep) {
            if (m_oldSubStep->indexOfAnimation(m_animation) >= 0) {
                m_newSubStep->addAnimation(m_oldSubStep->takeAnimation(m_oldSubStep->indexOfAnimation(m_animation)));
            }
            if (!m_children.isEmpty()) {
                foreach(KPrShapeAnimation *anim, m_children) {
                    if ((m_oldSubStep->indexOfAnimation(anim) >= 0) && (m_oldSubStep->indexOfAnimation(anim) < m_oldSubStep->animationCount())) {
                        m_newSubStep->addAnimation(m_oldSubStep->takeAnimation(m_oldSubStep->indexOfAnimation(anim)));
                    }
                }
            }
        }
        // If newStep reparent subSteps and children
        if (m_newStep != m_animation->step() || (m_newStep != m_newSubStep->parent())) {
            if (m_substeps.isEmpty()) {
                m_substeps.append(m_newSubStep);
            }
            else {
                m_substeps.insert(0, m_newSubStep);
            }
            foreach(KPrAnimationSubStep *subStep, m_substeps) {
                int row = 0;
                if (m_newSubstepRow != INVALID) {
                    m_newStep->insertAnimation(m_newSubstepRow + row, subStep);
                    row++;
                }
                else {
                    m_newStep->addAnimation(subStep);
                }
            }
            m_substeps.removeAll(m_newSubStep);
        }

        // If old substep or spte is empty remove from list;
        if (m_oldSubStep->children().isEmpty()) {
            m_oldSubstepRow = m_oldStep->indexOfAnimation(m_oldSubStep);
            m_oldSubStep->setParent(0);
        }
        if (m_oldStep->children().isEmpty() && m_animationsModel) {
            m_oldStepRow = m_animationsModel->steps().indexOf(m_oldStep);
            m_animationsModel->removeStep(m_oldStep);
        }

        // If new Step is not in step lists add it.
        if (m_animationsModel && !m_animationsModel->steps().contains(m_newStep)) {
            if (m_newStepRow != INVALID) {
                m_animationsModel->insertStep(m_newStepRow, m_newStep);
            }
            else {
                m_animationsModel->insertStep(m_animationsModel->steps().count(), m_newStep);
            }
        }

        if ((m_oldType == KPrShapeAnimation::On_Click) || notifyOnClickChange || ((m_newType == KPrShapeAnimation::On_Click))) {
            m_animationsModel->notifyOnClickEventChanged();
        }
        m_animationsModel->notifyAnimationChanged(m_animation);
        m_animationsModel->resyncStepsWithAnimations();
    }
}

void KPrAnimationEditNodeTypeCommand::undo()
{
    bool notifyOnClickChange = false;
    if (m_animation) {
        if (m_newSubStep != m_oldSubStep) {
            if (m_newSubStep->indexOfAnimation(m_animation) >= 0) {
                m_oldSubStep->addAnimation(m_newSubStep->takeAnimation(m_newSubStep->indexOfAnimation(m_animation)));
            }
            if (!m_children.isEmpty()) {
                foreach(KPrShapeAnimation *anim, m_children) {
                    if ((m_newSubStep->indexOfAnimation(anim) >= 0) && (m_newSubStep->indexOfAnimation(anim) < m_newSubStep->animationCount())) {
                        m_oldSubStep->addAnimation(m_newSubStep->takeAnimation(m_newSubStep->indexOfAnimation(anim)));
                    }
                }
            }
        }
        if (m_oldStep != m_animation->step() || (m_oldStep != m_oldSubStep->parent())) {
            if (m_substeps.isEmpty()) {
                m_substeps.append(m_oldSubStep);
            }
            else {
                m_substeps.insert(0, m_oldSubStep);
            }
            foreach(KPrAnimationSubStep *subStep, m_substeps) {
                int row = 0;
                if (m_oldSubstepRow != INVALID) {
                    m_oldStep->insertAnimation(m_oldSubstepRow + row, subStep);
                    row++;
                }
                else {
                    m_oldStep->addAnimation(subStep);
                }
            }
            m_substeps.removeAll(m_oldSubStep);
        }

        if (m_newSubStep->children().isEmpty()) {
            if (m_newStep && (m_newStep->indexOfAnimation(m_newSubStep) >= 0)) {
                m_newSubstepRow = m_newStep->indexOfAnimation(m_newSubStep);
            } else {
                m_newSubstepRow = m_oldStep->indexOfAnimation(m_newSubStep);
            }
            m_newSubStep->setParent(0);
        }
        if (m_newStep->children().isEmpty() && m_animationsModel) {
            m_newStepRow =m_animationsModel->steps().indexOf(m_newStep);
            m_animationsModel->removeStep(m_newStep);
        }

        // If old Step is not in step lists add it.
        if (m_animationsModel && !m_animationsModel->steps().contains(m_oldStep)) {
            if (m_oldStepRow != INVALID) {
                m_animationsModel->insertStep(m_oldStepRow, m_oldStep);
            }
            else {
                m_animationsModel->insertStep(m_animationsModel->steps().count(), m_oldStep);
            }
        }
        if ((m_oldType == KPrShapeAnimation::On_Click) || notifyOnClickChange || ((m_newType == KPrShapeAnimation::On_Click))) {
            m_animationsModel->notifyOnClickEventChanged();
        }
        m_animationsModel->notifyAnimationChanged(m_animation);
        m_animationsModel->resyncStepsWithAnimations();
    }
}
