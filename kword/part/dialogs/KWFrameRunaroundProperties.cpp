/* This file is part of the KDE project
 * Copyright (C) 2006, 2009 Thomas Zander <zander@kde.org>
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
#include "KWDocument.h"
#include "frames/KWFrame.h"

KWFrameRunaroundProperties::KWFrameRunaroundProperties(FrameConfigSharedState *state)
        : m_state(state),
        m_shape(0)
{
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

    widget.distance->setUnit(state->document()->unit());
}

void KWFrameRunaroundProperties::open(const QList<KWFrame*> &frames)
{
    m_frames = frames;
    GuiHelper::State layout = GuiHelper::Unset, runaround = GuiHelper::Unset, raDistance = GuiHelper::Unset;
    KWord::RunAroundSide side = KWord::BiggestRunAroundSide;
    KWord::TextRunAround ra = KWord::RunAround;
    qreal distance = 10.0;
    foreach (KWFrame *frame, frames) {
        if (layout == GuiHelper::Unset) {
            side = frame->runAroundSide();
            layout = GuiHelper::On;
        } else if (side != frame->runAroundSide())
            layout = GuiHelper::TriState;

        if (runaround == GuiHelper::Unset) {
            ra = frame->textRunAround();
            runaround = GuiHelper::On;
        } else if (ra != frame->textRunAround())
            runaround = GuiHelper::TriState;

        if (raDistance == GuiHelper::Unset) {
            distance = frame->runAroundDistance();
            raDistance = GuiHelper::On;
        } else if (distance != frame->runAroundDistance())
            raDistance = GuiHelper::TriState;
    }

    if (layout != GuiHelper::TriState)
        m_runAroundSide->button(side)->setChecked(true);
    if (runaround != GuiHelper::TriState)
        m_runAround->button(ra)->setChecked(true);
    widget.distance->changeValue(distance);
}

void KWFrameRunaroundProperties::open(KoShape *shape)
{
    m_state->addUser();
    m_shape = shape;
    widget.runAround->setChecked(true);
    widget.longest->setChecked(true);
    widget.distance->changeValue(MM_TO_POINT(3));
}

void KWFrameRunaroundProperties::save()
{
    if (m_frames.count() == 0) {
        KWFrame *frame = m_state->frame();
        if (frame == 0 && m_shape)
            frame = m_state->createFrame(m_shape);
        Q_ASSERT(frame);
        m_state->markFrameUsed();
        m_frames.append(frame);
    }
    foreach (KWFrame *frame, m_frames) {
        bool needRelayout = false;
        if (m_runAround->checkedId() != -1) {
            KWord::TextRunAround rra = static_cast<KWord::TextRunAround>(m_runAround->checkedId());
            if (frame->textRunAround() != rra) {
                frame->setTextRunAround(rra);
                needRelayout = true;
            }
        }
        if (m_runAroundSide->checkedId() != -1) {
            KWord::RunAroundSide rrs = static_cast<KWord::RunAroundSide>(m_runAroundSide->checkedId());
            if (frame->runAroundSide() != rrs) {
                frame->setRunAroundSide(rrs);
                needRelayout = true;
            }
        }
        if (frame->runAroundDistance() != widget.distance->value()) {
            frame->setRunAroundDistance(widget.distance->value());
            needRelayout = true;
        }
        if (needRelayout)
            frame->shape()->notifyChanged();
    }
    m_state->removeUser();
}
