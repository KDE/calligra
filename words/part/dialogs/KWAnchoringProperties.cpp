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

#include "KWAnchoringProperties.h"
#include "KWFrameDialog.h"
#include "KWDocument.h"
#include "frames/KWFrame.h"

#include <KoTextAnchor.h>

KWAnchoringProperties::KWAnchoringProperties(FrameConfigSharedState *state)
        : m_state(state),
        m_shape(0)
{
    widget.setupUi(this);

    m_anchor = new QButtonGroup();
    m_anchor->addButton(widget.rAnchorAsCharacter);
    m_anchor->setId(widget.rAnchorAsCharacter, KoTextAnchor::AnchorAsCharacter);
    m_anchor->addButton(widget.rAnchorToCharacter);
    m_anchor->setId(widget.rAnchorToCharacter, KoTextAnchor::AnchorToCharacter);
    m_anchor->addButton(widget.rAnchorParagraph);
    m_anchor->setId(widget.rAnchorParagraph, KoTextAnchor::AnchorParagraph);
    m_anchor->addButton(widget.rAnchorPage);
    m_anchor->setId(widget.rAnchorPage, KoTextAnchor::AnchorPage);
}
/*
KWAnchoringProperties::~KWAnchoringProperties()
{
}
*/
void KWAnchoringProperties::open(const QList<KWFrame*> &frames)
{
    m_state->addUser();
    m_frames = frames;
    GuiHelper::State anchorTypeHelper = GuiHelper::Unset;
    KoTextAnchor::AnchoringType anchorType = KoTextAnchor::AnchorPage;
    foreach (KWFrame *frame, frames) {
        KoTextAnchor::AnchoringType anchorTypeOfFrame = KoTextAnchor::AnchorPage;
    //TODO fetch type
        if (anchorTypeHelper == GuiHelper::Unset) {
            anchorType = anchorTypeOfFrame;
            anchorTypeHelper = GuiHelper::On;
        } else if (anchorType != anchorTypeOfFrame)
            anchorTypeHelper = GuiHelper::TriState;

    }

    if (anchorTypeHelper != GuiHelper::TriState)
        m_anchor->button(anchorType)->setChecked(true);
}

void KWAnchoringProperties::open(KoShape *shape)
{
    m_state->addUser();
    m_shape = shape;
    KoTextAnchor::AnchoringType anchorTypeOfShape = KoTextAnchor::AnchorPage;
    //TODO fetch type
    m_anchor->button(anchorTypeOfShape)->setChecked(true);
}

void KWAnchoringProperties::save()
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
        if (m_anchor->checkedId() != -1) {
            KoTextAnchor::AnchoringType type = static_cast<KoTextAnchor::AnchoringType>(m_anchor->checkedId());
            //TODO fetch anchor
            /*
            if (frame->shape()->textRunAroundSide() != type) {
                frame->shape()->setTextRunAroundSide(type);
                needRelayout = true;
            }*/
        }
        if (needRelayout)
            frame->shape()->notifyChanged();
    }
    m_state->removeUser();
}
