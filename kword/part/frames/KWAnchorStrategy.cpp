/* This file is part of the KDE project
 * Copyright (C) 2007, 2009, 2010 Thomas Zander <zander@kde.org>
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

#include "KWAnchorStrategy.h"
#include "KWPageTextInfo.h"
#include "KWTextFrameSet.h"
#include "KWDocument.h"
#include "KWFrame.h"

#include <KoShapeContainer.h>
#include <KoTextShapeData.h>
#include <KoTextBlockData.h>

#include <QTextBlock>
#include <QTextLine>

#include <KDebug>

KWAnchorStrategy::KWAnchorStrategy(KoTextAnchor *anchor)
        : m_anchor(anchor),
        m_finished(false),
        m_currentLineY(0),
        m_pass(0),
        m_lastknownPosInDoc(-1),
        m_lastVerticalPos(KoTextAnchor::VTop),
        m_lastVerticalRel(KoTextAnchor::VParagraph),
        m_lastHorizontalPos(KoTextAnchor::HLeft),
        m_lastHorizontalRel(KoTextAnchor::HPageContent)
{
    calculateKnowledgePoint();
}

KWAnchorStrategy::~KWAnchorStrategy()
{
}

bool KWAnchorStrategy::checkState(KoTextDocumentLayout::LayoutState *state, int startOfBlock, int startOfBlockText, KWTextFrameSet *frameSet)
{
    kDebug() << "start";
    qDebug() << " ********************************************************************** shape pointer " << m_anchor->shape();
    m_nextShapeNeeded = false;

    KoTextShapeData *data = 0;
    KoShape *shapeContainingAnchor = 0;
    foreach (KWFrame *frame, frameSet->frames()) { //find the frame for the anchor
        KoTextShapeData *tmpData = qobject_cast<KoTextShapeData*>(frame->shape()->userData());
        if(tmpData != 0) {
            if(m_anchor->positionInDocument() < tmpData->position()) {
                break;
            }
            data = tmpData;
            shapeContainingAnchor = frame->shape();
        }
    }
    if (!shapeContainingAnchor) // can happen if the shape isn't there yet but will be added later
        return false;
    Q_ASSERT(data);

    if (m_anchor->shape()->parent() == 0) { // it should be parented to our current shape
//        KoShapeContainer *sc = dynamic_cast<KoShapeContainer*>(state->shape);
        KoShapeContainer *sc = dynamic_cast<KoShapeContainer*>(shapeContainingAnchor);
        if (sc == 0) {
            kWarning(32002) << "Failed to attach the anchored shape to a text shape...";
            return false;
        }
        sc->addShape(m_anchor->shape());
        calculateKnowledgePoint();
    }

    if (m_knowledgePoint < 0)
        return false;

    if (m_anchor->behavesAsCharacter()) {
        //as-char means the horiz are invalid, but we need good values so let's just set them
        m_anchor->setHorizontalRel(KoTextAnchor::HChar);
        m_anchor->setHorizontalPos(KoTextAnchor::HLeft);
    }

    if (m_lastknownPosInDoc != m_anchor->positionInDocument()
            || m_lastOffset != m_anchor->offset()
            || m_lastVerticalPos != m_anchor->verticalPos()
            || m_lastVerticalRel != m_anchor->verticalRel()
            || m_lastHorizontalPos != m_anchor->horizontalPos()
            || m_lastHorizontalRel != m_anchor->horizontalRel()) { // different layout run
        m_finished = false;
        m_lastknownPosInDoc = m_anchor->positionInDocument();
        m_lastOffset = m_anchor->offset();
        m_lastVerticalPos = m_anchor->verticalPos();
        m_lastVerticalRel = m_anchor->verticalRel();
        m_lastHorizontalPos = m_anchor->horizontalPos();
        m_lastHorizontalRel = m_anchor->horizontalRel();
        m_pass = 0;
    }

    // exit when finished or when we can expect another call with a higher cursor position
    if (m_finished || (m_knowledgePoint > state->cursorPosition()))
        return false;

    // *** alter 'state' to relayout the part we want.
    QTextBlock block = m_anchor->document()->findBlock(m_anchor->positionInDocument());
    QTextLayout *layout = block.layout();
    int recalcFrom = state->cursorPosition(); // the position from which we will restart layout.
    KWPageTextInfo *pageInfo = dynamic_cast<KWPageTextInfo *>(data->page());

    QRectF boundingRect = m_anchor->shape()->boundingRect();
    QRectF containerBoundingRect = m_anchor->shape()->parent()->boundingRect();
    QRectF anchorBoundingRect;
    QPointF newPosition;

    // set anchor bounding rectangle horizontal position and size
    switch (m_anchor->horizontalRel()) {
    case KoTextAnchor::HPage:
        if (!pageInfo) {
            m_finished = false;
            return false;
        }
        anchorBoundingRect.setX(0);
        anchorBoundingRect.setWidth(pageInfo->page().width());
        recalcFrom = block.position();
        break;

    case KoTextAnchor::HParagraph:
    case KoTextAnchor::HPageContent:
        anchorBoundingRect.setX(containerBoundingRect.x());
        anchorBoundingRect.setWidth(containerBoundingRect.width());
        recalcFrom = block.position();
        break;

    case KoTextAnchor::HParagraphContent:
        anchorBoundingRect.setX(state->x() + containerBoundingRect.x());
        anchorBoundingRect.setWidth(state->width());
        recalcFrom = block.position();
        break;

    case KoTextAnchor::HChar:
        if (m_anchor->positionInDocument() == block.position()) { // at first position of parag.
            anchorBoundingRect.setX(state->x() + containerBoundingRect.x());
            anchorBoundingRect.setWidth(0.1); // just some small value
        } else if (layout->lineCount() != 0) {
            QTextLine tl = layout->lineForTextPosition(m_anchor->positionInDocument() - block.position());
            if (tl.isValid()) {
                anchorBoundingRect.setX(tl.cursorToX(m_anchor->positionInDocument() - block.position()) + containerBoundingRect.x());
                anchorBoundingRect.setWidth(0.1); // just some small value
                recalcFrom = 0;
            } else {
                m_finished = false;
                return false; // lets go for a second round.
            }
        } else {
            m_finished = false;
            return false; // lets go for a second round.
        }
        recalcFrom = block.position();
        break;

    case KoTextAnchor::HPageStartMargin:
    {
        if (!pageInfo) {
            m_finished = false;
            return false;
        }

        int horizontalPos = m_anchor->horizontalPos();
        // if verticalRel is HFromInside or HInside or HOutside and the page number is even,
        // than set anchorBoundingRect to HPageEndMargin area
        if ((pageInfo->pageNumber()%2 == 0) && (horizontalPos == KoTextAnchor::HFromInside ||
                horizontalPos == KoTextAnchor::HInside || horizontalPos == KoTextAnchor::HOutside)) {
            anchorBoundingRect.setX(containerBoundingRect.x() + containerBoundingRect.width());
            anchorBoundingRect.setWidth(pageInfo->page().width() - anchorBoundingRect.x());
        } else {
            anchorBoundingRect.setX(0);
            anchorBoundingRect.setWidth(containerBoundingRect.x());
        }
        recalcFrom = block.position();
        break;
    }
    case KoTextAnchor::HPageEndMargin:
    {
        if (!pageInfo) {
            m_finished = false;
            return false;
        }

        int horizontalPos = m_anchor->horizontalPos();
        // if verticalRel is HFromInside or HInside or HOutside and the page number is even,
        // than set anchorBoundingRect to HPageStartMargin area
        if ((pageInfo->pageNumber()%2 == 0) && (horizontalPos == KoTextAnchor::HFromInside ||
                horizontalPos == KoTextAnchor::HInside || horizontalPos == KoTextAnchor::HOutside)) {
            anchorBoundingRect.setX(0);
            anchorBoundingRect.setWidth(containerBoundingRect.x());
        } else {
            anchorBoundingRect.setX(containerBoundingRect.x() + containerBoundingRect.width());
            anchorBoundingRect.setWidth(pageInfo->page().width() - anchorBoundingRect.x());
        }
        recalcFrom = block.position();
        break;
    }
    case KoTextAnchor::HParagraphStartMargin:
    {
        if (!pageInfo) {
            m_finished = false;
            return false;
        }

        int horizontalPos = m_anchor->horizontalPos();
        // if verticalRel is HFromInside or HInside or HOutside and the page number is even,
        // than set anchorBoundingRect to HParagraphEndMargin area
        if ((pageInfo->pageNumber()%2 == 0) && (horizontalPos == KoTextAnchor::HFromInside ||
                horizontalPos == KoTextAnchor::HInside || horizontalPos == KoTextAnchor::HOutside)) {
            anchorBoundingRect.setX(state->x() + containerBoundingRect.x() + state->width());
            anchorBoundingRect.setWidth(containerBoundingRect.x() + containerBoundingRect.width() - anchorBoundingRect.x());
        } else {
            anchorBoundingRect.setX(containerBoundingRect.x());
            anchorBoundingRect.setWidth(state->x());
        }
        recalcFrom = block.position();
        break;
    }
    case KoTextAnchor::HParagraphEndMargin:
    {
        if (!pageInfo) {
            m_finished = false;
            return false;
        }

        int horizontalPos = m_anchor->horizontalPos();
        // if verticalRel is HFromInside or HInside or HOutside and the page number is even,
        // than set anchorBoundingRect to HParagraphStartMargin area
        if ((pageInfo->pageNumber()%2 == 0) && (horizontalPos == KoTextAnchor::HFromInside ||
                horizontalPos == KoTextAnchor::HInside || horizontalPos == KoTextAnchor::HOutside)) {
            anchorBoundingRect.setX(containerBoundingRect.x());
            anchorBoundingRect.setWidth(state->x());
        } else {
            anchorBoundingRect.setX(state->x() + containerBoundingRect.x() + state->width());
            anchorBoundingRect.setWidth(containerBoundingRect.x() + containerBoundingRect.width() - anchorBoundingRect.x());
        }
        recalcFrom = block.position();
        break;
    }
    default :
        kDebug(32002) << "horizontal-rel not handled";
    }

    // set anchor bounding rectangle vertical position
    switch (m_anchor->verticalRel()) {
    case KoTextAnchor::VPage:
        if (!pageInfo) {
            m_finished = false;
            return false;
        }
        anchorBoundingRect.setY(pageInfo->page().offsetInDocument());
        anchorBoundingRect.setHeight(pageInfo->page().height());
        recalcFrom = qMax(recalcFrom, data->position());
        break;

    case KoTextAnchor::VPageContent:
        if (!pageInfo) {
            m_finished = false;
            return false;
        }
        // find main frame
        foreach (KWFrameSet *fs, frameSet->kwordDocument()->frameSets()) {
            KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*>(fs);
            if (tfs && tfs->textFrameSetType() == KWord::MainTextFrameSet) {
                foreach (KWFrame *frame, fs->frames()) { //find main frame for current page
                    KoTextShapeData *tmpData = qobject_cast<KoTextShapeData*>(frame->shape()->userData());
                    if(data != 0) {
                        KWPageTextInfo *tmpPageInfo = dynamic_cast<KWPageTextInfo *>(tmpData->page());
                        if (tmpPageInfo != 0) {
                            if (tmpPageInfo->pageNumber() == pageInfo->pageNumber()) {//found main frame for current page
                                anchorBoundingRect.setY(frame->shape()->boundingRect().y());
                                anchorBoundingRect.setHeight(frame->shape()->boundingRect().height());
                                recalcFrom = qMax(recalcFrom, data->position());
                                break;
                            }
                        }
                    }
                }
                break;
            }
        }
        break;

    case KoTextAnchor::VParagraph:
    case KoTextAnchor::VParagraphContent:
        if (layout->lineCount() != 0) {
            qreal top = layout->lineAt(0).y();
            QTextLine tl = layout->lineAt(layout->lineCount() - 1);
            anchorBoundingRect.setY(top + containerBoundingRect.y()  - data->documentOffset());
            anchorBoundingRect.setHeight(tl.y() + tl.height() - top);
            recalcFrom = qMax(recalcFrom, block.position());
            KoTextBlockData *blockData = dynamic_cast<KoTextBlockData*>(block.userData());
            if(blockData && m_anchor->verticalRel() == KoTextAnchor::VParagraph) {
                anchorBoundingRect.setY(blockData->effectiveTop() + containerBoundingRect.y()  - data->documentOffset());
            }
        } else {
            m_finished = false;
            return false; // lets go for a second round.
        }
        break;

    case KoTextAnchor::VLine:
        if (layout->lineCount()) {
            QTextLine tl = layout->lineForTextPosition(m_anchor->positionInDocument() - block.position());
            Q_ASSERT(tl.isValid());
            anchorBoundingRect.setY(tl.y() - m_anchor->shape()->size().height()
                            + containerBoundingRect.y() - data->documentOffset());
            anchorBoundingRect.setHeight(2*m_anchor->shape()->size().height());
        } else {
            m_finished = false;
            return false; // lets go for a second round.
        }
        recalcFrom = qMax(recalcFrom, block.position());
        break;

    case KoTextAnchor::VText:
    case KoTextAnchor::VChar:
        if (layout->lineCount()) {
            QTextLine tl = layout->lineForTextPosition(m_anchor->positionInDocument() - block.position());
            Q_ASSERT(tl.isValid());
            if (m_anchor->behavesAsCharacter() && m_anchor->verticalRel() == KoTextAnchor::VChar) {
                //char relative is behaving in a special way when as-char
                anchorBoundingRect.setY(tl.y() + containerBoundingRect.y() - data->documentOffset());
                anchorBoundingRect.setHeight(tl.height());
            } else {
                anchorBoundingRect.setY(tl.y() + containerBoundingRect.y() - data->documentOffset());
                anchorBoundingRect.setHeight(tl.height());
            }
        } else {
            m_finished = false;
            return false; // lets go for a second round.
        }
        recalcFrom = qMax(recalcFrom, block.position());
        break;

    case KoTextAnchor::VBaseline:
        if (layout->lineCount()) {
            QTextLine tl = layout->lineForTextPosition(m_anchor->positionInDocument() - block.position());
            Q_ASSERT(tl.isValid());
            anchorBoundingRect.setY(tl.y() + tl.ascent() - m_anchor->shape()->size().height()
               + containerBoundingRect.y() - data->documentOffset());
            anchorBoundingRect.setHeight(2*m_anchor->shape()->size().height());
        } else {
            m_finished = false;
            return false; // lets go for a second round.
        }
        recalcFrom = qMax(recalcFrom, block.position());
        break;
    default :
        kDebug(32002) << "vertical-rel not handled";
    }

    // Set shape horizontal alignment inside anchor bounding rectangle
    switch (m_anchor->horizontalPos()) {
    case KoTextAnchor::HCenter:
        newPosition.setX(anchorBoundingRect.x() + anchorBoundingRect.width()/2 - containerBoundingRect.x());
        break;

    case KoTextAnchor::HFromInside:
    case KoTextAnchor::HInside:
    {
        if (!pageInfo) {
            m_finished = false;
            return false;
        }

        if (pageInfo->pageNumber()%2 == 1) {
            newPosition.setX(anchorBoundingRect.x() - containerBoundingRect.x());
        } else {
            newPosition.setX(anchorBoundingRect.right() - containerBoundingRect.x() -
                    m_anchor->shape()->size().width() - 2*m_anchor->offset().x() );
        }
        break;
    }
    case KoTextAnchor::HFromLeft:
    case KoTextAnchor::HLeft:
        newPosition.setX(anchorBoundingRect.x() - containerBoundingRect.x());
        break;

    case KoTextAnchor::HOutside:
    {
        if (!pageInfo) {
            m_finished = false;
            return false;
        }

        if (pageInfo->pageNumber()%2 == 1) {
            newPosition.setX(anchorBoundingRect.right() - containerBoundingRect.x());
        } else {
            newPosition.setX(anchorBoundingRect.x() - containerBoundingRect.x() +
                             m_anchor->shape()->size().width() - 2*(m_anchor->offset().x() + m_anchor->shape()->size().width()) );
        }
        break;
    }
    case KoTextAnchor::HRight: {
        newPosition.setX(anchorBoundingRect.right() - containerBoundingRect.x());
        break;
    }
    default :
        kDebug(32002) << "horizontal-pos not handled";
    }

    // Set shape vertical alignment inside anchor bounding rectangle
    switch (m_anchor->verticalPos()) {
    case KoTextAnchor::VBottom:
        newPosition.setY(anchorBoundingRect.bottom() - containerBoundingRect.y()
        );//- m_anchor->shape()->size().height());
        break;
    case KoTextAnchor::VBelow:
        newPosition.setY(anchorBoundingRect.bottom() - containerBoundingRect.y());
        break;

    case KoTextAnchor::VMiddle:
        newPosition.setY(anchorBoundingRect.y() + anchorBoundingRect.height()/2 - containerBoundingRect.y());
        break;

    case KoTextAnchor::VFromTop:
    case KoTextAnchor::VTop:
        newPosition.setY(anchorBoundingRect.y() - containerBoundingRect.y());
        break;

    default :
        kDebug(32002) << "vertical-pos not handled";
    }
    newPosition = newPosition + m_anchor->offset();
qDebug()<<"BOUNDING RECT"<<anchorBoundingRect;
    if (!checkPageBorder(newPosition, containerBoundingRect, pageInfo))
    {
        m_finished = false;
        return false; // lets go for a second round.
    }

    QPointF diff = newPosition - m_anchor->shape()->position();
    const bool moved = qAbs(diff.x()) > 0.5 || qAbs(diff.y()) > 0.5;
    if (!m_finished && m_pass > 0) { // already been here
        // for the cases where we align with text; check if the text is within margin. If so; set finished to true.
        m_finished = !moved;
    } else if (!m_finished && m_pass == 0 && m_anchor->behavesAsCharacter()) {
        // If it behaves like a char it's 'run around' already by the qt text flow.
        // Only one pass needed
        m_finished = true;
    }
    m_pass++;

    if (moved) {
        // set the shape to the proper position based on the data
        m_anchor->shape()->update();
        //kDebug() << "anchor positioned" << newPosition << "/" << m_anchor->shape()->position();
        //kDebug() << "finished" << m_finished;
qDebug()<<"setting shape position to"<<newPosition;
m_anchor->shape()->setPosition(newPosition);
        m_anchor->shape()->update();
    }


    if (m_finished) // no second pass needed
        return false;

    // TODO layout recalculation

    do { // move the layout class back a couple of paragraphs.
        if (recalcFrom >= startOfBlock && recalcFrom < startOfBlockText)
            break;
        if (state->cursorPosition() <= recalcFrom)
            break;
    } while (state->previousParag());

    m_finished = true;
    return true;
}

bool KWAnchorStrategy::isFinished()
{
    // if, for the second time, we passed the point where the anchor was inserted, return true
    return m_finished;
}

KoShape *KWAnchorStrategy::anchoredShape() const
{
    if (m_anchor->behavesAsCharacter())
        return 0;
    return m_anchor->shape();
}

void KWAnchorStrategy::calculateKnowledgePoint()
{
    m_knowledgePoint = -1;

   // figure out until what cursor position we need to layout to get all the info we need
   switch (m_anchor->horizontalRel()) {
   case KoTextAnchor::HPage:
   case KoTextAnchor::HPageContent: {
       if (m_anchor->shape()->parent() == 0) // not enough info yet.
           return;
       KoTextShapeData *data = qobject_cast<KoTextShapeData*>(m_anchor->shape()->parent()->userData());
       Q_ASSERT(data);
       m_knowledgePoint = data->position();
       break;
   }
   case KoTextAnchor::HParagraph:
   case KoTextAnchor::HParagraphContent:
   case KoTextAnchor::HChar:
   case KoTextAnchor::HPageEndMargin:
   case KoTextAnchor::HPageStartMargin:
   case KoTextAnchor::HParagraphEndMargin:
   case KoTextAnchor::HParagraphStartMargin:
       m_knowledgePoint = m_anchor->positionInDocument();
       break;
   default :
       kDebug(32002) << "horizontal-rel not handled";
   }

   switch (m_anchor->verticalRel()) {
   case KoTextAnchor::VPage:
   case KoTextAnchor::VPageContent: {
       if (m_anchor->shape()->parent() == 0) // not enough info yet.
           return;
       KoTextShapeData *data = qobject_cast<KoTextShapeData*>(m_anchor->shape()->parent()->userData());
       Q_ASSERT(data);
       m_knowledgePoint = qMax(m_knowledgePoint, data->position() + 1);
       break;
   }
   case KoTextAnchor::VParagraphContent:
   case KoTextAnchor::VParagraph:
   case KoTextAnchor::VLine: {
       QTextBlock block = m_anchor->document()->findBlock(m_anchor->positionInDocument());
       m_knowledgePoint = qMax(m_knowledgePoint, block.position() + block.length()-2);
       break;
   }
   default :
       kDebug(32002) << "vertical-rel not handled";
   }
}

bool KWAnchorStrategy::checkPageBorder(QPointF &newPosition, QRectF containerBoundingRect, KWPageTextInfo *pageInfo)
{
        return true;
    if (!pageInfo) {
        return false;
    }

    //check left border and move the shape back to have the whole shape visible
    if (newPosition.x() < -containerBoundingRect.x()) {
        newPosition.setX(-containerBoundingRect.x());
    }

    //check right border and move the shape back to have the whole shape visible
    if ((newPosition.x() + m_anchor->shape()->size().width()) > (pageInfo->page().width() -containerBoundingRect.x())) {
        newPosition.setX(pageInfo->page().width() - m_anchor->shape()->size().width() - containerBoundingRect.x());
    }

    //check top border and move the shape back to have the whole shape visible
    if (newPosition.y() < (pageInfo->page().offsetInDocument() - containerBoundingRect.y())) {
        newPosition.setY(pageInfo->page().offsetInDocument() - containerBoundingRect.y());
    }

    //check bottom border and move the shape back to have the whole shape visible
    if ((newPosition.y() + m_anchor->shape()->size().height()) > (pageInfo->page().offsetInDocument() + pageInfo->page().height() - containerBoundingRect.y())) {
        newPosition.setY(pageInfo->page().offsetInDocument() + pageInfo->page().height() - m_anchor->shape()->size().height() - containerBoundingRect.y());
    }

}
