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


#include "KPrEditAnimationTimeLineCommand.h"

#include "animations/KPrShapeAnimation.h"

#include <klocalizedstring.h>

KPrEditAnimationTimeLineCommand::KPrEditAnimationTimeLineCommand(KPrShapeAnimation *animation,
                                                                 const int begin, const int duration, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_animation(animation)
    , m_newBegin(begin)
    , m_newDuration(duration)
{
    QPair<int, int> range = m_animation->timeRange();
    m_oldBegin = range.first;
    m_oldDuration = range.second - range.first;
    setText(kundo2_i18n("Edit animation delay/duration"));
}

KPrEditAnimationTimeLineCommand::~KPrEditAnimationTimeLineCommand()
{
}

void KPrEditAnimationTimeLineCommand::redo()
{
    if (m_animation) {
        m_animation->setBeginTime(m_newBegin);
        m_animation->setGlobalDuration(m_newDuration);
    }
}

void KPrEditAnimationTimeLineCommand::undo()
{
    if (m_animation) {
        m_animation->setBeginTime(m_oldBegin);
        m_animation->setGlobalDuration(m_oldDuration);
    }
}
