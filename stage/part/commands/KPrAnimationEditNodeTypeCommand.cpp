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

KPrAnimationEditNodeTypeCommand::KPrAnimationEditNodeTypeCommand(KPrShapeAnimation *animation, KPrAnimationStep *newStep,
                                                                 KPrAnimationSubStep *newSubStep, KPrShapeAnimation::Node_Type newType,
                                                                 QList<KPrShapeAnimation *> children, QList<KPrAnimationSubStep *> movedSubSteps,
                                                                 KPrPage *activePage, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_animation(animation)
    , m_newStep(newStep)
    , m_newSubStep(newSubStep)
    , m_newType(newType)
    , m_children(children)
    , m_substeps(movedSubSteps)
    , m_activePage(activePage)
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
        qDebug() << "redo for" << m_animation->id();
        if (m_newSubStep != m_oldSubStep) {
            m_newSubStep->addAnimation(m_oldSubStep->takeAnimation(m_oldSubStep->indexOfAnimation(m_animation)));
            m_animation->setSubStep(m_newSubStep);
            if (!m_children.isEmpty()) {
                foreach(KPrShapeAnimation *anim, m_children) {
                    m_newSubStep->addAnimation(m_oldSubStep->takeAnimation(m_oldSubStep->indexOfAnimation(anim)));
                    anim->setSubStep(m_newSubStep);
                }
            }
        }
        if (m_newStep != m_newSubStep->parent()) {
            if (m_substeps.isEmpty()) {
                m_substeps.append(m_newSubStep);
            }
            else {
                m_substeps.insert(0, m_newSubStep);
            }
            foreach(KPrAnimationSubStep *subStep, m_substeps) {
                m_newStep->addAnimation(subStep);
                for (int j=0; j < subStep->animationCount(); j++) {
                    QAbstractAnimation *shapeAnimation = subStep->animationAt(j);
                    if (KPrShapeAnimation *b = dynamic_cast<KPrShapeAnimation*>(shapeAnimation)) {
                        b->setStep(m_newStep);
                    }
                }
            }
        }

        if (m_oldSubStep->children().isEmpty()) {
            qDebug() << "set remove substep";
            m_oldSubStep->setParent(0);
        }
        if (m_oldStep->children().isEmpty() && m_activePage) {
            qDebug() << "set remove step";
            m_activePage->animations().removeStep(m_oldStep);
        }
        m_animation->setNodeType(m_newType);
    }
}

void KPrAnimationEditNodeTypeCommand::undo()
{
    if (m_animation) {
        qDebug() << "undo for" << m_animation->id();
        if (m_newSubStep != m_oldSubStep) {
            m_oldSubStep->addAnimation(m_newSubStep->takeAnimation(m_newSubStep->indexOfAnimation(m_animation)));
            m_animation->setSubStep(m_oldSubStep);
            if (!m_children.isEmpty()) {
                foreach(KPrShapeAnimation *anim, m_children) {
                    m_oldSubStep->addAnimation(m_newSubStep->takeAnimation(m_newSubStep->indexOfAnimation(anim)));
                    anim->setSubStep(m_oldSubStep);
                }
            }
        }
        if (m_oldStep != m_oldSubStep->parent()) {
            if (m_substeps.isEmpty()) {
                m_substeps.append(m_oldSubStep);
            }
            else {
                m_substeps.removeAll(m_newSubStep);
                m_substeps.insert(0, m_oldSubStep);
            }
            foreach(KPrAnimationSubStep *subStep, m_substeps) {
                m_oldStep->addAnimation(subStep);
                for (int j=0; j < subStep->animationCount(); j++) {
                    QAbstractAnimation *shapeAnimation = subStep->animationAt(j);
                    if (KPrShapeAnimation *b = dynamic_cast<KPrShapeAnimation*>(shapeAnimation)) {
                        b->setStep(m_oldStep);
                    }
                }
            }
        }
        if (m_newSubStep->children().isEmpty()) {
            qDebug() << "set remove new substep";
            m_newSubStep->setParent(0);
        }
        if (m_newStep->children().isEmpty() && m_activePage) {
            qDebug() << "set remove new step";
            m_activePage->animations().removeStep(m_newStep);
        }
        m_animation->setNodeType(m_oldType);
    }
}
