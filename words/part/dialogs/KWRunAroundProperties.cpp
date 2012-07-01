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
#include "frames/KWTextFrameSet.h"

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

    widget.threshold->setUnit(state->document()->unit());

    m_runAroundContour = new QButtonGroup();
    m_runAroundContour->addButton(widget.box);
    m_runAroundContour->setId(widget.box, KoShape::ContourBox);
    m_runAroundContour->addButton(widget.outside);
    m_runAroundContour->setId(widget.outside, KoShape::ContourOutside);

    widget.distanceLeft->setUnit(state->document()->unit());
    widget.distanceTop->setUnit(state->document()->unit());
    widget.distanceRight->setUnit(state->document()->unit());
    widget.distanceBottom->setUnit(state->document()->unit());

    connect(widget.enough, SIGNAL(toggled(bool)), this, SLOT(enoughRunAroundToggled(bool)));
}

bool KWRunAroundProperties::open(const QList<KWFrame*> &frames)
{
    m_state->addUser();
    m_frames = frames;
    GuiHelper::State runaround = GuiHelper::Unset;
    GuiHelper::State raDistanceLeft = GuiHelper::Unset;
    GuiHelper::State raDistanceTop = GuiHelper::Unset;
    GuiHelper::State raDistanceRight = GuiHelper::Unset;
    GuiHelper::State raDistanceBottom = GuiHelper::Unset;
    GuiHelper::State raThreshold = GuiHelper::Unset;
    GuiHelper::State raContour = GuiHelper::Unset;
    KoShape::TextRunAroundSide side = KoShape::BiggestRunAroundSide;
    qreal distanceLeft = 0.0;
    qreal distanceTop = 0.0;
    qreal distanceRight = 0.0;
    qreal distanceBottom = 0.0;
    qreal threshold = 0.0;
    KoShape::TextRunAroundContour contour = KoShape::ContourBox;

    bool atLeastOne = false;

    foreach (KWFrame *frame, frames) {
        if (frame->frameSet()->type() == Words::TextFrameSet) {
            if (static_cast<KWTextFrameSet *>(frame->frameSet())->textFrameSetType() != Words::OtherTextFrameSet) {
                continue;
            }
        }
        atLeastOne = true;
        if (runaround == GuiHelper::Unset) {
            side = frame->shape()->textRunAroundSide();
            runaround = GuiHelper::On;
        } else if (side != frame->shape()->textRunAroundSide())
            runaround = GuiHelper::TriState;

        if (raThreshold == GuiHelper::Unset) {
            threshold = frame->shape()->textRunAroundThreshold();
            raThreshold = GuiHelper::On;
        } else if (threshold != frame->shape()->textRunAroundThreshold())
            raThreshold = GuiHelper::TriState;

        if (raContour == GuiHelper::Unset) {
            contour = frame->shape()->textRunAroundContour();
            raContour = GuiHelper::On;
        } else if (contour != frame->shape()->textRunAroundContour())
            raContour = GuiHelper::TriState;

        if (raDistanceLeft == GuiHelper::Unset) {
            distanceLeft = frame->shape()->textRunAroundDistanceLeft();
            raDistanceLeft = GuiHelper::On;
        } else if (distanceLeft != frame->shape()->textRunAroundDistanceLeft())
            raDistanceLeft = GuiHelper::TriState;
        if (raDistanceTop == GuiHelper::Unset) {
            distanceTop = frame->shape()->textRunAroundDistanceTop();
            raDistanceTop = GuiHelper::On;
        } else if (distanceTop != frame->shape()->textRunAroundDistanceTop())
            raDistanceTop = GuiHelper::TriState;
        if (raDistanceRight == GuiHelper::Unset) {
            distanceRight = frame->shape()->textRunAroundDistanceRight();
            raDistanceRight = GuiHelper::On;
        } else if (distanceRight != frame->shape()->textRunAroundDistanceRight())
            raDistanceRight = GuiHelper::TriState;
        if (raDistanceBottom == GuiHelper::Unset) {
            distanceBottom = frame->shape()->textRunAroundDistanceBottom();
            raDistanceBottom = GuiHelper::On;
        } else if (distanceBottom != frame->shape()->textRunAroundDistanceBottom())
            raDistanceBottom = GuiHelper::TriState;
    }

    if (!atLeastOne) {
        return false;
    }

    if (runaround != GuiHelper::TriState)
        m_runAroundSide->button(side)->setChecked(true);

    widget.threshold->changeValue(threshold);

    if (contour == KoShape::ContourFull) {
        contour = KoShape::ContourOutside;
    }

    if (raContour != GuiHelper::TriState) {
        m_runAroundContour->button(contour)->setChecked(true);
    }

    widget.distanceLeft->changeValue(distanceLeft);
    widget.distanceTop->changeValue(distanceTop);
    widget.distanceRight->changeValue(distanceRight);
    widget.distanceBottom->changeValue(distanceBottom);

    return true;
}

void KWRunAroundProperties::open(KoShape *shape)
{
    m_state->addUser();
    m_shape = shape;
    m_runAroundSide->button(shape->textRunAroundSide())->setChecked(true);
    widget.threshold->changeValue(shape->textRunAroundThreshold());
    widget.distanceLeft->changeValue(shape->textRunAroundDistanceLeft());
    widget.distanceTop->changeValue(shape->textRunAroundDistanceTop());
    widget.distanceRight->changeValue(shape->textRunAroundDistanceRight());
    widget.distanceBottom->changeValue(shape->textRunAroundDistanceBottom());
    if (shape->textRunAroundContour() == KoShape::ContourFull) {
        m_runAroundContour->button(KoShape::ContourOutside)->setChecked(true);
    } else {
        m_runAroundContour->button(shape->textRunAroundContour())->setChecked(true);
    }
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
        if (frame->frameSet()->type() == Words::TextFrameSet) {
            if (static_cast<KWTextFrameSet *>(frame->frameSet())->textFrameSetType() != Words::OtherTextFrameSet) {
                continue;
            }
        }
        KoShape *shape = frame->shape();
        KoShape::TextRunAroundSide side = shape->textRunAroundSide();
        int runThrough = shape->runThrough();
        qreal distanceLeft = shape->textRunAroundDistanceLeft();
        qreal distanceTop = shape->textRunAroundDistanceTop();
        qreal distanceRight = shape->textRunAroundDistanceRight();
        qreal distanceBottom = shape->textRunAroundDistanceBottom();
        qreal threshold = shape->textRunAroundThreshold();
        KoShape::TextRunAroundContour contour = shape->textRunAroundContour();

        if (m_runAroundSide->checkedId() != -1) {
            KoShape::TextRunAroundSide rrs = static_cast<KoShape::TextRunAroundSide>(m_runAroundSide->checkedId());
            if (side != rrs) {
                side = rrs;
            }
        }
        if (shape->textRunAroundThreshold() != widget.threshold->value()) {
            threshold = widget.threshold->value();
        }
        if (m_runAroundContour->checkedId() != -1) {
            KoShape::TextRunAroundContour rrc = static_cast<KoShape::TextRunAroundContour>(m_runAroundContour->checkedId());
            if (contour != rrc) {
                contour = rrc;
            }
        }
        if (distanceLeft != widget.distanceLeft->value()) {
            distanceLeft = widget.distanceLeft->value();
        }
        if (distanceTop != widget.distanceTop->value()) {
            distanceTop = widget.distanceTop->value();
        }
        if (distanceRight != widget.distanceRight->value()) {
            distanceRight = widget.distanceRight->value();
        }
        if (distanceBottom != widget.distanceBottom->value()) {
            distanceBottom = widget.distanceBottom->value();
        }
        if (macro) {
            new KoShapeRunAroundCommand(shape, side, runThrough, distanceLeft, distanceTop, distanceRight, distanceBottom, threshold, contour, macro);
        } else {
            shape->setTextRunAroundSide(side, KoShape::Background);
            shape->setRunThrough(runThrough);
            shape->setTextRunAroundThreshold(threshold);
            shape->setTextRunAroundContour(contour);
            shape->setTextRunAroundDistanceLeft(distanceLeft);
            shape->setTextRunAroundDistanceTop(distanceTop);
            shape->setTextRunAroundDistanceRight(distanceRight);
            shape->setTextRunAroundDistanceBottom(distanceBottom);
            shape->notifyChanged();
        }
    }
    m_state->removeUser();
}

void KWRunAroundProperties::enoughRunAroundToggled(bool checked) {
    widget.threshold->setEnabled(checked);
}
