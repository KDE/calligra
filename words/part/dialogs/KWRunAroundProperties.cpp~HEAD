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

#include "KWRunAroundProperties.h"
#include "KWFrameDialog.h"
#include "KWDocument.h"
#include "frames/KWFrame.h"

KWRunAroundProperties::KWRunAroundProperties(FrameConfigSharedState *state)
        : m_state(state),
        m_shape(0)
{
    widget.setupUi(this);

    m_runAroundSide = new QButtonGroup();
    m_runAroundSide->addButton(widget.left);
    m_runAroundSide->setId(widget.left, KoShape::LeftRunAroundSide);
    m_runAroundSide->addButton(widget.right);
    m_runAroundSide->setId(widget.right, KoShape::RightRunAroundSide);
    m_runAroundSide->addButton(widget.longest);
    m_runAroundSide->setId(widget.longest, KoShape::BiggestRunAroundSide);
    m_runAroundSide->addButton(widget.both);
    m_runAroundSide->setId(widget.both, KoShape::BothRunAroundSide);
    m_runAroundSide->addButton(widget.runThrough);
    m_runAroundSide->setId(widget.runThrough, KoShape::RunThrough);
    m_runAroundSide->addButton(widget.noRunaround);
    m_runAroundSide->setId(widget.noRunaround, KoShape::NoRunAround);

    widget.distance->setUnit(state->document()->unit());
}

void KWRunAroundProperties::open(const QList<KWFrame*> &frames)
{
    m_state->addUser();
    m_frames = frames;
    GuiHelper::State runaround = GuiHelper::Unset, raDistance = GuiHelper::Unset;
    KoShape::TextRunAroundSide side = KoShape::BiggestRunAroundSide;
    qreal distance = 10.0;
    foreach (KWFrame *frame, frames) {
        if (runaround == GuiHelper::Unset) {
            side = frame->shape()->textRunAroundSide();
            runaround = GuiHelper::On;
        } else if (side != frame->shape()->textRunAroundSide())
            runaround = GuiHelper::TriState;

        if (raDistance == GuiHelper::Unset) {
            distance = frame->shape()->textRunAroundDistance();
            raDistance = GuiHelper::On;
        } else if (distance != frame->shape()->textRunAroundDistance())
            raDistance = GuiHelper::TriState;
    }

    if (runaround != GuiHelper::TriState)
        m_runAroundSide->button(side)->setChecked(true);
    widget.distance->changeValue(distance);
}

void KWRunAroundProperties::open(KoShape *shape)
{
    m_state->addUser();
    m_shape = shape;
    m_runAroundSide->button(shape->textRunAroundSide())->setChecked(true);
    widget.distance->changeValue(shape->textRunAroundDistance());
}

void KWRunAroundProperties::save()
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
        if (m_runAroundSide->checkedId() != -1) {
            KoShape::TextRunAroundSide rrs = static_cast<KoShape::TextRunAroundSide>(m_runAroundSide->checkedId());
            if (frame->shape()->textRunAroundSide() != rrs) {
                frame->shape()->setTextRunAroundSide(rrs);
                needRelayout = true;
            }
        }
        if (frame->shape()->textRunAroundDistance() != widget.distance->value()) {
            frame->shape()->setTextRunAroundDistance(widget.distance->value());
            needRelayout = true;
        }
        if (needRelayout)
            frame->shape()->notifyChanged();
    }
    m_state->removeUser();
}
