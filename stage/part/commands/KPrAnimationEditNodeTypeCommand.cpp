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
#include "KPrShapeAnimations.h"

const int INVALID = -1;

KPrAnimationEditNodeTypeCommand::KPrAnimationEditNodeTypeCommand(KPrShapeAnimation *animation, KPrShapeAnimation::NodeType oldType,
                                                                 KPrShapeAnimation::NodeType newType,
                                                                 KPrShapeAnimations *animationModel, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_animation(animation)
    , m_newType(newType)
    , m_oldType(oldType)
    , m_model(animationModel)
{
    setText(kundo2_i18n("Edit animation trigger event" ) );
}

KPrAnimationEditNodeTypeCommand::~KPrAnimationEditNodeTypeCommand()
{
}

void KPrAnimationEditNodeTypeCommand::redo()
{
    m_model->setNodeType(m_animation, m_newType);
}

void KPrAnimationEditNodeTypeCommand::undo()
{
    m_model->setNodeType(m_animation, m_oldType);
}
