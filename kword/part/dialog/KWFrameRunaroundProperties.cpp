/* This file is part of the KDE project
 * Copyright (C) 2006 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
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

#include "KWFrameRunaroundProperties.h"
#include "KWFrameDialog.h"
#include "frame/KWFrame.h"

KWFrameRunaroundProperties::KWFrameRunaroundProperties(FrameConfigSharedState *state)
    : m_shape(0),
    m_state(state)
{
    m_state->addUser();
    widget.setupUi(this);

    m_runAroundSide = new QButtonGroup();
    m_runAroundSide->addButton(widget.left);
    m_runAroundSide->setId(widget.left, KWord::LeftRunAroundSide);
    m_runAroundSide->addButton(widget.right);
    m_runAroundSide->setId(widget.right, KWord::RightRunAroundSide);
    m_runAroundSide->addButton(widget.longest);
    m_runAroundSide->setId(widget.longest, KWord::BiggestRunAroundSide);
    m_runAround = new QButtonGroup();
    m_runAround->addButton(widget.runThrough);
    m_runAround->setId(widget.runThrough, KWord::RunThrough);
    m_runAround->addButton(widget.runAround);
    m_runAround->setId(widget.runAround, KWord::RunAround);
    m_runAround->addButton(widget.noRunaround);
    m_runAround->setId(widget.noRunaround, KWord::NoRunAround);
}

KWFrameRunaroundProperties::~KWFrameRunaroundProperties() {
    m_state->removeUser();
}

void KWFrameRunaroundProperties::open(const QList<KWFrame*> &frames) {
    m_frames = frames;
    GuiHelper::State layout = GuiHelper::Unset, runaround = GuiHelper::Unset, raDistance = GuiHelper::Unset;
    KWord::RunAroundSide side = KWord::BiggestRunAroundSide;
    KWord::TextRunAround ra = KWord::RunAround;
    double distance = 10.0;
    foreach(KWFrame *frame, frames) {
        if(layout == GuiHelper::Unset) {
            side = frame->runAroundSide();
            layout = GuiHelper::On;
        } else if(side != frame->runAroundSide())
            layout = GuiHelper::TriState;

        if(runaround == GuiHelper::Unset) {
            ra = frame->textRunAround();
            runaround = GuiHelper::On;
        } else if(ra != frame->textRunAround())
            runaround = GuiHelper::TriState;

        if(raDistance == GuiHelper::Unset) {
            distance = frame->runAroundDistance();
            raDistance = GuiHelper::On;
        } else if(distance != frame->runAroundDistance())
            raDistance = GuiHelper::TriState;
    }

    if(layout != GuiHelper::TriState)
        m_runAroundSide->button(side)->setChecked(true);
    if(runaround != GuiHelper::TriState)
        m_runAround->button(ra)->setChecked(true);
    widget.distance->setValue(distance);
}

void KWFrameRunaroundProperties::open(KoShape *shape) {
    m_shape = shape;
    widget.runAround->setChecked(true);
    widget.longest->setChecked(true);
    widget.distance->setValue(10);
}

void KWFrameRunaroundProperties::save() {
    if(m_frames.count() == 0) {
        KWFrame *frame = m_state->frame();
        if(frame == 0 && m_shape)
            frame = m_state->createFrame(m_shape);
        Q_ASSERT(frame);
        m_state->markFrameUsed();
        m_frames.append(frame);
    }
    foreach(KWFrame *frame, m_frames) {
        if(m_runAround->checkedId() != -1)
            frame->setTextRunAround( static_cast<KWord::TextRunAround> (m_runAround->checkedId()) );
        if(m_runAroundSide->checkedId() != -1)
            frame->setRunAroundSide( static_cast<KWord::RunAroundSide> (m_runAroundSide->checkedId()) );
        frame->setRunAroundDistance( widget.distance->value() );
    }
}

KAction *KWFrameRunaroundProperties::createAction() {
    return 0;
}

// TODO alter the spinbox to be a unit-double

#include "KWFrameRunaroundProperties.moc"
