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


#include "KPrReplaceAnimationCommand.h"
#include "KPrDocument.h"

KPrReplaceAnimationCommand::KPrReplaceAnimationCommand(KPrDocument *doc, KPrShapeAnimation *oldAnimation, KPrShapeAnimation *newAnimation,  KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_oldAnimation(oldAnimation)
    , m_newAnimation(newAnimation)
    , m_document(doc)
    , m_deleteAnimation(DeleteOld)
{

}

KPrReplaceAnimationCommand::~KPrReplaceAnimationCommand()
{
    if (m_deleteAnimation == DeleteOld) {
        delete m_oldAnimation;
    }
    else {
        delete m_newAnimation;
    }
}

void KPrReplaceAnimationCommand::redo()
{
    m_document->replaceAnimation(m_oldAnimation, m_newAnimation);
    m_deleteAnimation = DeleteOld;
}

void KPrReplaceAnimationCommand::undo()
{
    m_document->replaceAnimation(m_newAnimation, m_oldAnimation);
    m_deleteAnimation = DeleteNew;
}
