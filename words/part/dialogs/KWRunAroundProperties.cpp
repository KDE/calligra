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

#include <commands/KoShapeRunAroundCommand.h>

#include <kundo2command.h>

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
    m_runAroundSide->addButton(widget.enough);
    m_runAroundSide->setId(widget.enough, KoShape::EnoughRunAroundSide);

    widget.distance->setUnit(state->document()->unit());
    widget.threshold->setUnit(state->document()->unit());

    connect(widget.enough, SIGNAL(toggled(bool)), this, SLOT(enoughRunAroundToggled(bool)));
}

void KWRunAroundProperties::open(const QList<KWFrame*> &frames)
{
    m_state->addUser();
    m_frames = frames;
    GuiHelper::State runaround = GuiHelper::Unset;
    GuiHelper::State raDistance = GuiHelper::Unset;
    GuiHelper::State raThreshold = GuiHelper::Unset;
    KoShape::TextRunAroundSide side = KoShape::BiggestRunAroundSide;
    qreal distance = 10.0;
    qreal threshold = 0.0;
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

        if (raThreshold == GuiHelper::Unset) {
            threshold = frame->shape()->textRunAroundThreshold();
            raThreshold = GuiHelper::On;
        } else if (threshold != frame->shape()->textRunAroundThreshold())
            raThreshold = GuiHelper::TriState;
    }

    if (runaround != GuiHelper::TriState)
        m_runAroundSide->button(side)->setChecked(true);

    widget.distance->changeValue(distance);
    widget.threshold->changeValue(threshold);
}

void KWRunAroundProperties::open(KoShape *shape)
{
    m_state->addUser();
    m_shape = shape;
    m_runAroundSide->button(shape->textRunAroundSide())->setChecked(true);
    widget.distance->changeValue(shape->textRunAroundDistance());
    widget.threshold->changeValue(shape->textRunAroundThreshold());
}

void KWRunAroundProperties::save()
{
    save(0);
}

void KWRunAroundProperties::save(KUndo2Command *macro)
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
        KoShape *shape = frame->shape();
        KoShape::TextRunAroundSide side = shape->textRunAroundSide();
        int runThrough = shape->runThrough();
        qreal distance = shape->textRunAroundDistance();
        qreal threshold = shape->textRunAroundThreshold();

        if (m_runAroundSide->checkedId() != -1) {
            KoShape::TextRunAroundSide rrs = static_cast<KoShape::TextRunAroundSide>(m_runAroundSide->checkedId());
            if (side != rrs) {
                side = rrs;
            }
        }
        if (distance != widget.distance->value()) {
            distance = widget.distance->value();
        }
        if (shape->textRunAroundThreshold() != widget.threshold->value()) {
            threshold = widget.threshold->value();
        }
        if (macro) {
            new KoShapeRunAroundCommand(shape, side, runThrough, distance, threshold, macro);
        } else {
            shape->setTextRunAroundSide(side, KoShape::Background);
            shape->setRunThrough(runThrough);
            shape->setTextRunAroundDistance(distance);
            shape->setTextRunAroundThreshold(threshold);
            shape->notifyChanged();
        }
    }
    m_state->removeUser();
}

void KWRunAroundProperties::enoughRunAroundToggled(bool checked) {
    widget.threshold->setEnabled(checked);
}
