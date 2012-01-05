/* This file is part of the KDE project
 * Copyright (C) 2006, 2009 Thomas Zander <zander@kde.org>
 * Copyright (C) 2011 KoGmbh <cbo@kogmbh.com>
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
#include <KoInlineTextObjectManager.h>

#include <QComboBox>

const int KWAnchoringProperties::vertRels[4][20] = {
    { // KoTextAnchor::AnchorAsCharacter
        KoTextAnchor::VBaseline,
        KoTextAnchor::VChar,
        KoTextAnchor::VLine,
        -1
    },
    { // KoTextAnchor::AnchorToCharacter
        KoTextAnchor::VChar,
        KoTextAnchor::VLine,
        KoTextAnchor::VParagraph,
        KoTextAnchor::VParagraphContent,
        KoTextAnchor::VPage,
        KoTextAnchor::VPageContent,
        -1
    },
    { // KoTextAnchor::AnchorParagraph
        KoTextAnchor::VParagraph,
        KoTextAnchor::VParagraphContent,
        KoTextAnchor::VPage,
        KoTextAnchor::VPageContent,
        -1
    },
    { // KoTextAnchor::AnchorPage
        KoTextAnchor::VPage,
        KoTextAnchor::VPageContent,
        -1
    }
};

const int KWAnchoringProperties::horizRels[4][20] = {
    { // KoTextAnchor::AnchorAsCharacter
        -1
    },
    { // KoTextAnchor::AnchorToCharacter
        KoTextAnchor::HChar,
        KoTextAnchor::HParagraph,
        KoTextAnchor::HParagraphContent,
        KoTextAnchor::HParagraphStartMargin,
        KoTextAnchor::HParagraphEndMargin,
        KoTextAnchor::HPageStartMargin,
        KoTextAnchor::HPageEndMargin,
        KoTextAnchor::HPage,
        KoTextAnchor::HPageContent,
        -1
    },
    { // KoTextAnchor::AnchorParagraph
        KoTextAnchor::HParagraph,
        KoTextAnchor::HParagraphContent,
        KoTextAnchor::HParagraphStartMargin,
        KoTextAnchor::HParagraphEndMargin,
        KoTextAnchor::HPageStartMargin,
        KoTextAnchor::HPageEndMargin,
        KoTextAnchor::HPage,
        KoTextAnchor::HPageContent,
        -1
    },
    { // KoTextAnchor::AnchorPage
        KoTextAnchor::HPageStartMargin,
        KoTextAnchor::HPageEndMargin,
        KoTextAnchor::HPage,
        KoTextAnchor::HPageContent,
        -1
    }
};

KWAnchoringProperties::KWAnchoringProperties(FrameConfigSharedState *state)
        : m_state(state),
        m_shape(0)
{
    widget.setupUi(this);

    m_anchorTypeGroup = new QButtonGroup();
    m_anchorTypeGroup->addButton(widget.rAnchorAsCharacter);
    m_anchorTypeGroup->setId(widget.rAnchorAsCharacter, KoTextAnchor::AnchorAsCharacter);
    m_anchorTypeGroup->addButton(widget.rAnchorToCharacter);
    m_anchorTypeGroup->setId(widget.rAnchorToCharacter, KoTextAnchor::AnchorToCharacter);
    m_anchorTypeGroup->addButton(widget.rAnchorParagraph);
    m_anchorTypeGroup->setId(widget.rAnchorParagraph, KoTextAnchor::AnchorParagraph);
    m_anchorTypeGroup->addButton(widget.rAnchorPage);
    m_anchorTypeGroup->setId(widget.rAnchorPage, KoTextAnchor::AnchorPage);
    connect(m_anchorTypeGroup, SIGNAL(buttonClicked(int)), this, SLOT(anchorTypeChanged(int)));

    m_vertPosGroup = new QButtonGroup();
    m_vertPosGroup->addButton(widget.rTop);
    m_vertPosGroup->setId(widget.rTop, KoTextAnchor::VTop);
    m_vertPosGroup->addButton(widget.rVCenter);
    m_vertPosGroup->setId(widget.rVCenter, KoTextAnchor::VMiddle);
    m_vertPosGroup->addButton(widget.rBottom);
    m_vertPosGroup->setId(widget.rBottom, KoTextAnchor::VBottom);
    m_vertPosGroup->addButton(widget.rVOffset);
    m_vertPosGroup->setId(widget.rVOffset, KoTextAnchor::VFromTop);
    connect(m_vertPosGroup, SIGNAL(buttonClicked(int)), this, SLOT(vertPosChanged(int)));

    m_horizPosGroup = new QButtonGroup();
    m_horizPosGroup->addButton(widget.rLeft);
    m_horizPosGroup->setId(widget.rLeft, KoTextAnchor::HLeft);
    m_horizPosGroup->addButton(widget.rHCenter);
    m_horizPosGroup->setId(widget.rHCenter, KoTextAnchor::HCenter);
    m_horizPosGroup->addButton(widget.rRight);
    m_horizPosGroup->setId(widget.rRight, KoTextAnchor::HRight);
    m_horizPosGroup->addButton(widget.rHOffset);
    m_horizPosGroup->setId(widget.rHOffset, KoTextAnchor::HFromLeft);
    connect(m_horizPosGroup, SIGNAL(buttonClicked(int)), this, SLOT(horizPosChanged(int)));
}

void KWAnchoringProperties::open(const QList<KWFrame*> &frames)
{
    m_state->addUser();
    m_frames = frames;
    
    KoInlineTextObjectManager *manager = m_state->document()->inlineTextObjectManager();

    GuiHelper::State anchorTypeHelper = GuiHelper::Unset;
    GuiHelper::State vertHelper = GuiHelper::Unset;
    GuiHelper::State horizHelper = GuiHelper::Unset;
    KoTextAnchor::AnchorType anchorType = KoTextAnchor::AnchorPage;
    
    m_vertPos = -1;
    m_horizPos = -1;
    m_vertRel = -1;
    m_horizRel = -1;
    QPointF offset;

    foreach (KWFrame *frame, frames) {
        KoTextAnchor *anchor = frame->anchor();
        KoTextAnchor::AnchorType anchorTypeOfFrame = anchor ? anchor->anchorType() : KoTextAnchor::AnchorPage;
        
        // FIXME these should fetch correct values if anchor == 0
        int vertPosOfFrame = anchor ? anchor->verticalPos() : KoTextAnchor::VFromTop;
        int horizPosOfFrame = anchor ? anchor->horizontalPos() : KoTextAnchor::HFromLeft;
        int vertRelOfFrame = anchor ? anchor->verticalRel() : KoTextAnchor::VPage;
        int horizRelOfFrame = anchor ? anchor->horizontalRel() : KoTextAnchor::HPage;
        QPointF offsetOfFrame = anchor ? anchor->offset() : QPointF();

        if (anchorTypeHelper == GuiHelper::Unset) {
            anchorType = anchorTypeOfFrame;
            anchorTypeHelper = GuiHelper::On;
        } else if (anchorType != anchorTypeOfFrame)
            anchorTypeHelper = GuiHelper::TriState;

        if (vertHelper == GuiHelper::Unset) {
            m_vertPos = vertPosOfFrame;
            m_vertRel = vertRelOfFrame;
            offset = offsetOfFrame;
            vertHelper = GuiHelper::On;
        } else if (m_vertPos != vertPosOfFrame || m_vertRel != vertRelOfFrame ||
            (m_vertPos == KoTextAnchor::VFromTop && offset.y() != offsetOfFrame.y())) {
            vertHelper = GuiHelper::TriState;
            m_vertPos = vertPosOfFrame;
            m_vertRel = vertRelOfFrame;
        }

        if (horizHelper == GuiHelper::Unset) {
            m_horizPos = horizPosOfFrame;
            m_horizRel = horizRelOfFrame;
            offset = offsetOfFrame;
            horizHelper = GuiHelper::On;
        } else if (m_horizPos != horizPosOfFrame || m_horizRel != horizRelOfFrame ||
            (m_horizPos == KoTextAnchor::HFromLeft && offset.x() != offsetOfFrame.x())) {
            horizHelper = GuiHelper::TriState;
            m_horizPos = -1;
            m_horizRel = -1;
        }
    }

    if (anchorTypeHelper != GuiHelper::TriState) {
        m_anchorTypeGroup->button(anchorType)->setChecked(true);

        anchorTypeChanged(anchorType);

        if (vertHelper != GuiHelper::TriState) {
            m_vertPosGroup->button(m_vertPos)->setChecked(true);
            vertPosChanged(m_vertPos, offset);
        } else {
            vertPosChanged(-1, QPointF());
        }

        if (horizHelper != GuiHelper::TriState) {
            m_horizPosGroup->button(m_horizPos)->setChecked(true);
            horizPosChanged(m_horizPos, offset);
        } else {
            horizPosChanged(-1, QPointF());
        }
    } else {
        m_anchorType = -1;
        widget.grpVert->setEnabled(false);
        widget.grpHoriz->setEnabled(false);
    }
}

void KWAnchoringProperties::vertPosChanged(int vertPos, QPointF offset)
{
    if (m_anchorType == -1) {
        return; //we should already be disabled
    }
    switch (vertPos) {
    case KoTextAnchor::VTop:
        widget.cTopArea->setEnabled(true);
        widget.cVCenterArea->setEnabled(false);
        widget.cRightArea->setEnabled(false);
        widget.cVOffsetArea->setEnabled(false);
        widget.sVOffset->setEnabled(false);
        widget.lVOffset->setEnabled(false);

        for (int i = 0; vertRels[m_anchorType][i] != -1; i++) {
            if (vertRels[m_anchorType][i] == m_vertRel) {
                widget.cTopArea->setCurrentIndex(i);
            }
        }
        break;
    case KoTextAnchor::VMiddle:
        widget.cTopArea->setEnabled(false);
        widget.cVCenterArea->setEnabled(true);
        widget.cBottomArea->setEnabled(false);
        widget.cVOffsetArea->setEnabled(false);
        widget.sVOffset->setEnabled(false);
        widget.lVOffset->setEnabled(false);

        for (int i = 0; vertRels[m_anchorType][i] != -1; i++) {
            if (vertRels[m_anchorType][i] == m_vertRel) {
                widget.cVCenterArea->setCurrentIndex(i);
            }
        }
        break;
    case KoTextAnchor::VBottom:
        widget.cTopArea->setEnabled(false);
        widget.cVCenterArea->setEnabled(false);
        widget.cBottomArea->setEnabled(true);
        widget.cVOffsetArea->setEnabled(false);
        widget.sVOffset->setEnabled(false);
        widget.lVOffset->setEnabled(false);

        for (int i = 0; vertRels[m_anchorType][i] != -1; i++) {
            if (vertRels[m_anchorType][i] == m_vertRel) {
                widget.cBottomArea->setCurrentIndex(i);
            }
        }
        break;
    case KoTextAnchor::VFromTop:
        widget.cTopArea->setEnabled(false);
        widget.cVCenterArea->setEnabled(false);
        widget.cBottomArea->setEnabled(false);
        widget.cVOffsetArea->setEnabled(true);
        widget.sVOffset->setEnabled(true);
        widget.lVOffset->setEnabled(true);

        for (int i = 0; vertRels[m_anchorType][i] != -1; i++) {
            if (vertRels[m_anchorType][i] == m_vertRel) {
                widget.cVOffsetArea->setCurrentIndex(i);
            }
        }
        if (m_vertRel != 1) {
            widget.sVOffset->setValue(offset.y());
        }
        break;
    case -1:
        widget.cTopArea->setEnabled(false);
        widget.cVCenterArea->setEnabled(false);
        widget.cBottomArea->setEnabled(false);
        widget.cVOffsetArea->setEnabled(false);
        widget.sVOffset->setEnabled(false);
        widget.lVOffset->setEnabled(false);
        break;
    default:
        break;
    }
    m_vertPos = vertPos;
}

void KWAnchoringProperties::horizPosChanged(int horizPos, QPointF offset)
{
    if (m_anchorType == -1) {
        return; //we should already be disabled
    }
    switch (horizPos) {
    case KoTextAnchor::HLeft:
        widget.cLeftArea->setEnabled(true);
        widget.cHCenterArea->setEnabled(false);
        widget.cRightArea->setEnabled(false);
        widget.cHOffsetArea->setEnabled(false);
        widget.sHOffset->setEnabled(false);
        widget.lHOffset->setEnabled(false);

        for (int i = 0; horizRels[m_anchorType][i] != -1; i++) {
            if (horizRels[m_anchorType][i] == m_horizRel) {
                widget.cLeftArea->setCurrentIndex(i);
            }
        }
        break;
    case KoTextAnchor::HCenter:
        widget.cLeftArea->setEnabled(false);
        widget.cHCenterArea->setEnabled(true);
        widget.cRightArea->setEnabled(false);
        widget.cHOffsetArea->setEnabled(false);
        widget.sHOffset->setEnabled(false);
        widget.lHOffset->setEnabled(false);

        for (int i = 0; horizRels[m_anchorType][i] != -1; i++) {
            if (horizRels[m_anchorType][i] == m_horizRel) {
                widget.cHCenterArea->setCurrentIndex(i);
            }
        }
        break;
    case KoTextAnchor::HRight:
        widget.cLeftArea->setEnabled(false);
        widget.cHCenterArea->setEnabled(false);
        widget.cRightArea->setEnabled(true);
        widget.cHOffsetArea->setEnabled(false);
        widget.sHOffset->setEnabled(false);
        widget.lHOffset->setEnabled(false);

        for (int i = 0; horizRels[m_anchorType][i] != -1; i++) {
            if (horizRels[m_anchorType][i] == m_horizRel) {
                widget.cRightArea->setCurrentIndex(i);
            }
        }
        break;
    case KoTextAnchor::HFromLeft:
        widget.cLeftArea->setEnabled(false);
        widget.cHCenterArea->setEnabled(false);
        widget.cRightArea->setEnabled(false);
        widget.cHOffsetArea->setEnabled(true);
        widget.sHOffset->setEnabled(true);
        widget.lHOffset->setEnabled(true);

        for (int i = 0; horizRels[m_anchorType][i] != -1; i++) {
            if (horizRels[m_anchorType][i] == m_horizRel) {
                widget.cHOffsetArea->setCurrentIndex(i);
            }
        }
        if (m_horizRel != -1) {
            widget.sHOffset->setValue(offset.x());
        }
        break;
    case -1:
        widget.cLeftArea->setEnabled(false);
        widget.cHCenterArea->setEnabled(false);
        widget.cRightArea->setEnabled(false);
        widget.cHOffsetArea->setEnabled(false);
        widget.sHOffset->setEnabled(false);
        widget.lHOffset->setEnabled(false);
        break;
    default:
        break;
    }
    m_horizPos = horizPos;
}

void KWAnchoringProperties::anchorTypeChanged(int type)
{
    KoTextAnchor::AnchorType anchorType = KoTextAnchor::AnchorType(type);

    QString vertRelStrings[20]; //NOTE: order needs to be the same as KoTextAnchor::VerticalRel
    vertRelStrings[0] = i18n("Baseline"); // KoTextAnchor::VBaseline
    vertRelStrings[1] = i18n("Character"); // KoTextAnchor::VChar
    vertRelStrings[2] = QString(); // KoTextAnchor::VFrame
    vertRelStrings[3] = QString(); // KoTextAnchor::VFrameContent
    vertRelStrings[4] = i18n("Row"); // KoTextAnchor::VLine
    vertRelStrings[5] = i18n("Page (entire) area"); // KoTextAnchor::VPage
    vertRelStrings[6] = i18n("Page text area"); // KoTextAnchor::VPageContent
    vertRelStrings[7] = i18n("Paragraph area"); // KoTextAnchor::VParagraph
    vertRelStrings[8] = i18n("Paragraph text area"); // KoTextAnchor::VParagraphContent
    vertRelStrings[9] = QString(); // KoTextAnchor::VText

    QString horizRelStrings[20]; //NOTE: order needs to be the same as KoTextAnchor::HorizontalRel
    horizRelStrings[0] = i18n("Character"); // KoTextAnchor::HChar
    horizRelStrings[1] = i18n("Page (entire) area"); // KoTextAnchor::HPage
    horizRelStrings[2] = i18n("Page text area"); // KoTextAnchor::HPageContent
    horizRelStrings[3] = i18n("Left page border"); // KoTextAnchor::HPageStartMargin
    horizRelStrings[4] = i18n("Right page border"); // KoTextAnchor::HPageEndMargin
    horizRelStrings[5] = QString(); // KoTextAnchor::HFrame
    horizRelStrings[6] = QString(); // KoTextAnchor::HFrameContent
    horizRelStrings[7] = QString(); // KoTextAnchor::HFrameEndMargin
    horizRelStrings[8] = QString(); // KoTextAnchor::HFrameStartMargin
    horizRelStrings[9] = i18n("Paragraph area"); // KoTextAnchor::HParagraph
    horizRelStrings[10] = i18n("Paragraph text area"); // KoTextAnchor::HParagraphContent
    horizRelStrings[11] = i18n("Right paragraph border"); // KoTextAnchor::HParagraphEndMargin
    horizRelStrings[12] = i18n("Left paragraph border"); // KoTextAnchor::HParagraphStartMargin


    widget.cTopArea->clear();
    widget.cVCenterArea->clear();
    widget.cBottomArea->clear();
    widget.cVOffsetArea->clear();
    for (int i = 0; vertRels[anchorType][i] != -1; i++) {
        widget.cTopArea->addItem(vertRelStrings[vertRels[anchorType][i]]);
        widget.cVCenterArea->addItem(vertRelStrings[vertRels[anchorType][i]]);
        widget.cBottomArea->addItem(vertRelStrings[vertRels[anchorType][i]]);
        widget.cVOffsetArea->addItem(vertRelStrings[vertRels[anchorType][i]]);
    }

    widget.cLeftArea->clear();
    widget.cHCenterArea->clear();
    widget.cRightArea->clear();
    widget.cHOffsetArea->clear();
    for (int i = 0; horizRels[anchorType][i] != -1; i++) {
        widget.cLeftArea->addItem(horizRelStrings[horizRels[anchorType][i]]);
        widget.cHCenterArea->addItem(horizRelStrings[horizRels[anchorType][i]]);
        widget.cRightArea->addItem(horizRelStrings[horizRels[anchorType][i]]);
        widget.cHOffsetArea->addItem(horizRelStrings[horizRels[anchorType][i]]);
    }
    if (anchorType == KoTextAnchor::AnchorAsCharacter) {
        widget.grpHoriz->setEnabled(false);
    } else {
        widget.grpHoriz->setEnabled(true);
    }
    m_anchorType = type;
    vertPosChanged(m_vertPos);
    horizPosChanged(m_horizPos);
}

void KWAnchoringProperties::open(KoShape *shape)
{
    m_state->addUser();
    m_shape = shape;
    KoTextAnchor::AnchorType anchorTypeOfShape = KoTextAnchor::AnchorPage;
    //TODO fetch type
    m_anchorTypeGroup->button(anchorTypeOfShape)->setChecked(true);
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

    KoInlineTextObjectManager *manager = m_state->document()->inlineTextObjectManager();
    
    foreach (KWFrame *frame, m_frames) {
        if (m_anchorTypeGroup->checkedId() != -1) {
            KoTextAnchor::AnchorType type   = KoTextAnchor::AnchorType(m_anchorTypeGroup->checkedId());
            KoTextAnchor            *anchor = 0;

            if (type != KoTextAnchor::AnchorPage) {
                anchor = m_state->document()->getAnchorOfShape(frame->shape(), true);
            }
            else {
                anchor = m_state->document()->getAnchorOfShape(frame->shape(), false);
                m_state->document()->inlineTextObjectManager()->removeInlineObject(anchor);
            }

            if (anchor) {
                anchor->setAnchorType(KoTextAnchor::AnchorType(m_anchorTypeGroup->checkedId()));
                anchor->setHorizontalRel(KoTextAnchor::HorizontalRel(m_horizRel));
                anchor->setVerticalRel(KoTextAnchor::VerticalRel(m_vertRel));
                anchor->setHorizontalPos(KoTextAnchor::HorizontalPos(m_horizPos));
                anchor->setVerticalPos(KoTextAnchor::VerticalPos(m_vertPos));
                anchor->shape()->notifyChanged();
            }
        }
    }
    m_state->removeUser();
}
