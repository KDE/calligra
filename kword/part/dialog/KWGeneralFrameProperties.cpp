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

#include "KWGeneralFrameProperties.h"
#include "KWFrameDialog.h"
#include "frame/KWFrame.h"
#include "frame/KWTextFrameSet.h"

#include <KoTextShape.h>

KWGeneralFrameProperties::KWGeneralFrameProperties(FrameConfigSharedState *state)
    : m_state(state),
    m_shape(0)
{
    m_state->addUser();
    widget.setupUi(this);
    m_textGroup = new QButtonGroup(widget.textGroupBox);
    m_textGroup->addButton(widget.createNewPage);
    m_textGroup->setId(widget.createNewPage, KWord::AutoCreateNewFrameBehavior);
    m_textGroup->addButton(widget.resizeLastFrame);
    m_textGroup->setId(widget.resizeLastFrame, KWord::AutoExtendFrameBehavior);
    m_textGroup->addButton(widget.noExtraText);
    m_textGroup->setId(widget.noExtraText, KWord::IgnoreContentFrameBehavior);
    m_newPageGroup = new QButtonGroup(widget.newPageGroupBox);
    m_newPageGroup->addButton(widget.noFollowup);
    m_newPageGroup->setId(widget.noFollowup, KWord::NoFollowupFrame);
    m_newPageGroup->addButton(widget.reconnect);
    m_newPageGroup->setId(widget.reconnect, KWord::ReconnectNewFrame);
    m_newPageGroup->addButton(widget.placeCopy);
    m_newPageGroup->setId(widget.placeCopy, KWord::CopyNewFrame);
}

KWGeneralFrameProperties::~KWGeneralFrameProperties() {
    m_state->removeUser();
}

void KWGeneralFrameProperties::open(KoShape *shape) {
    m_shape = shape;
    // set default values
    widget.isCopyOfPrevious->setEnabled(false);
    widget.protectContent->setVisible(false);
    widget.allFrames->setVisible(false);
    widget.resizeLastFrame->setChecked(true);
    widget.isCopyOfPrevious->setVisible(false);
    if(shape->shapeId() == KoTextShape_SHAPEID)
        widget.reconnect->setChecked(true);
    else {
        widget.noFollowup->setChecked(true);
        widget.reconnect->setVisible(false);
        widget.textGroupBox->setVisible(false);
    }
}

void KWGeneralFrameProperties::open(const QList<KWFrame*> &frames) {
    m_frames = frames;
    // checkboxes
    GuiHelper copyFrame, allFrames, protectContent, evenOdd;
    // radioGroups
    GuiHelper::State newFrame = GuiHelper::Unset, frameBehavior = GuiHelper::Unset;
    KWord::NewFrameBehavior nfb = KWord::ReconnectNewFrame;
    KWord::FrameBehavior fb = KWord::AutoExtendFrameBehavior;
    foreach (KWFrame *frame, frames) {
        if(frameBehavior == GuiHelper::Unset) {
            fb = frame->frameBehavior();
            frameBehavior = GuiHelper::On;
        } else if(fb != frame->frameBehavior())
            frameBehavior = GuiHelper::TriState;

        if(newFrame == GuiHelper::Unset) {
            nfb = frame->newFrameBehavior();
            newFrame = GuiHelper::On;
        } else if(nfb != frame->newFrameBehavior())
            newFrame = GuiHelper::TriState;


        if(frame->frameSet()->frameCount() > 1) {
            allFrames.addState(GuiHelper::On);
            if(frame->frameSet()->frames().indexOf(frame) > 0)
                copyFrame.addState(frame->isCopy() ? GuiHelper::On : GuiHelper::Off);
        }

        evenOdd.addState(frame->frameOnBothSheets() ? GuiHelper::On : GuiHelper::Off);

        KWTextFrameSet *textFs = dynamic_cast<KWTextFrameSet *> (frame->frameSet());
        if(textFs)
            protectContent.addState(textFs->protectContent() ? GuiHelper::On : GuiHelper::Off);
    }

    // update the GUI
    copyFrame.updateCheckBox(widget.isCopyOfPrevious, false);
    allFrames.updateCheckBox(widget.allFrames, true);
    protectContent.updateCheckBox(widget.protectContent, true);
    evenOdd.updateCheckBox(widget.evenOdd, false);

    if(newFrame == GuiHelper::On)
        m_newPageGroup->button(nfb)->setChecked(true);
    if(frameBehavior == GuiHelper::On)
        m_textGroup->button(fb)->setChecked(true);
    else if(frameBehavior == GuiHelper::Unset)
        widget.textGroupBox->setVisible(false);

    if(protectContent.m_state == GuiHelper::Unset) {
        // if there is no text frame in the whole list of frames.
        widget.reconnect->setVisible(false);
        widget.textGroupBox->setVisible(false);
    }
}

void KWGeneralFrameProperties::save() {
    if(m_frames.count() == 0) {
        KWFrame *frame = m_state->frame();
        if(frame == 0 && m_shape)
            frame = m_state->createFrame(m_shape);
        Q_ASSERT(frame);
        m_frames.append(frame);
    }
    foreach(KWFrame *frame, m_frames) {
        if(m_textGroup->checkedId() != -1) {
            KWord::FrameBehavior fb = static_cast<KWord::FrameBehavior> (m_textGroup->checkedId());
            frame->setFrameBehavior(fb);
        }
        if(m_newPageGroup->checkedId() != -1) {
            KWord::NewFrameBehavior nfb = static_cast<KWord::NewFrameBehavior> (m_newPageGroup->checkedId());
            frame->setNewFrameBehavior(nfb);
        }
        if(widget.evenOdd->checkState() != Qt::PartiallyChecked)
            frame->setFrameOnBothSheets( widget.evenOdd->checkState() == Qt::Checked );
        if(frame->frameSet()) {
            if(widget.protectContent->checkState() != Qt::PartiallyChecked) {
                KWTextFrameSet *textFs = dynamic_cast<KWTextFrameSet *> (frame->frameSet());
                if(textFs)
                    textFs->setProtectContent(widget.protectContent->checkState() == Qt::Checked);
            }
            if(widget.isCopyOfPrevious->checkState() != Qt::PartiallyChecked) {
                bool isCopy = widget.isCopyOfPrevious->checkState() == Qt::Checked;
                if(frame->frameSet()->frames().indexOf(frame) == 0)
                    isCopy = false;
                frame->setCopy(isCopy);
            }
            if(widget.allFrames->isEnabled() && widget.allFrames->checkState() == Qt::Checked) {
                foreach(KWFrame *otherFrame, frame->frameSet()->frames()) {
                    if(m_frames.contains(otherFrame))
                        continue;
                    // TODO add on KWFrame: virtual void copySettings(const KWFrame *frame)
                }
            }
        }
    }
}

KAction *KWGeneralFrameProperties::createAction() {
    return 0;
}

#include "KWGeneralFrameProperties.moc"
