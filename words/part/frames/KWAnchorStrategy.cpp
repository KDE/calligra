/* This file is part of the KDE project
 * Copyright (C) 2007, 2009, 2010 Thomas Zander <zander@kde.org>
 * Copyright (C) 2010 Matus Hanzes <matus.hanzes@ixonos.com>
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
        m_lastknownPosInDoc(-1),
        m_makeSecondPass(false),
        m_lastVerticalPos(KoTextAnchor::VTop),
        m_lastVerticalRel(KoTextAnchor::VParagraph),
        m_lastHorizontalPos(KoTextAnchor::HLeft),
        m_lastHorizontalRel(KoTextAnchor::HPageContent),
        m_lastParent(0)
{
    calculateKnowledgePoint();
}

KWAnchorStrategy::~KWAnchorStrategy()
{
}

bool KWAnchorStrategy::checkState(KoTextDocumentLayout::LayoutState *state, KWTextFrameSet *frameSet)
{
    if (!m_anchor->shape()->parent()) {
        kDebug() << "no parent";
        return false;
    }

    // get the page data
    KoTextShapeData *data = qobject_cast<KoTextShapeData*>(m_anchor->shape()->parent()->userData());
    if (!data) {
        kDebug() << "no text shape data found";
        return false;
    }

    if (m_knowledgePoint < 0)
        return false;

    // there should be always at least one line
    if (state->layout->lineCount() == 0) {
        return false;
    }

    // second pass was needed, shape position is ok now
    if (m_makeSecondPass) {
        m_makeSecondPass = false;
        m_finished = true;
        return true;
    }

    if (m_anchor->behavesAsCharacter()) {
        //as-char means the horiz are invalid, but we need good values so let's just set them
        m_anchor->setHorizontalRel(KoTextAnchor::HChar);
        m_anchor->setHorizontalPos(KoTextAnchor::HLeft);
    }

    // check if there is change in anchor position and if yes than
    if (m_lastknownPosInDoc != m_anchor->positionInDocument()
            || m_lastOffset != m_anchor->offset()
            || m_lastVerticalPos != m_anchor->verticalPos()
            || m_lastVerticalRel != m_anchor->verticalRel()
            || m_lastHorizontalPos != m_anchor->horizontalPos()
            || m_lastHorizontalRel != m_anchor->horizontalRel()
            || m_lastParent != m_anchor->shape()->parent()) { // different layout run
        m_finished = false;
        m_lastknownPosInDoc = m_anchor->positionInDocument();
        m_lastOffset = m_anchor->offset();
        m_lastVerticalPos = m_anchor->verticalPos();
        m_lastVerticalRel = m_anchor->verticalRel();
        m_lastHorizontalPos = m_anchor->horizontalPos();
        m_lastHorizontalRel = m_anchor->horizontalRel();
        m_lastParent = m_anchor->shape()->parent();
        calculateKnowledgePoint();
    }

    // exit when finished or when we can expect another call with a higher cursor position
    if (m_finished || (m_knowledgePoint > state->cursorPosition()))
        return false;

    // *** alter 'state' to relayout the part we want.
    QTextBlock block = m_anchor->document()->findBlock(m_anchor->positionInDocument());
    QTextLayout *layout = block.layout();

    KWPageTextInfo *pageInfo = dynamic_cast<KWPageTextInfo *>(data->page());
    if (!pageInfo) {
        m_finished = false;
        return false;
    }

    QRectF boundingRect = m_anchor->shape()->boundingRect();
    QRectF containerBoundingRect = m_anchor->shape()->parent()->boundingRect();
    QRectF anchorBoundingRect;
    QPointF newPosition;

    // set anchor bounding rectangle horizontal position and size
    if (!countHorizontalRel(anchorBoundingRect, containerBoundingRect, pageInfo, state, block, layout)) {
        m_finished = false;
        return false;
    }

    // set anchor bounding rectangle vertical position
    if (!countVerticalRel(anchorBoundingRect, containerBoundingRect, pageInfo, frameSet, data, block, layout)) {
    m_finished = false;
    return false;
    }

    // Set shape horizontal alignment inside anchor bounding rectangle
    countHorizontalPos(newPosition, anchorBoundingRect, containerBoundingRect, pageInfo);

    // Set shape vertical alignment inside anchor bounding rectangle
    countVerticalPos(newPosition, anchorBoundingRect, containerBoundingRect);

    newPosition = newPosition + m_anchor->offset();

    //check the border of page an move the shape back to have it visible
    checkPageBorder(newPosition, containerBoundingRect, pageInfo);

    if (!m_finished && m_anchor->behavesAsCharacter()) {
        // If it behaves like a char it's 'run around' already by the qt text flow.
        // Only one pass needed
        m_finished = true;
    }

    // set the shape to the proper position based on the data
    m_anchor->shape()->update();
    m_anchor->shape()->setPosition(newPosition);
    m_anchor->shape()->update();

    if (m_finished) // shape is in right position no second pass needed
        return false;

    //layout recalculation

    // if shape is RunThrough then the shape is not intersecting the text and no relayout is needed
    if (m_anchor->shape()->runThrough() != 0) {
        m_finished = true;
        return true;
    }

    // check if the shape is intersecting the text if no than no relayout is needed
    QRectF shapeRect(newPosition.x(),newPosition.y(), m_anchor->shape()->size().width(),m_anchor->shape()->size().height());
    QPointF relayoutPos(containerBoundingRect.x() + containerBoundingRect.width(), containerBoundingRect.y() + containerBoundingRect.height());

    if (!checkTextIntersecion(relayoutPos, shapeRect, containerBoundingRect, state, data)) {
        m_finished = true;
        return true;
    }

    // if shape is positioned inside already layouted text, relayout is needed
    calculateRelayoutPosition(relayoutPos, state, data);

    m_makeSecondPass = true;
    return false;
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
   case KoTextAnchor::HPageContent:
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

   kDebug() << "m_knowledgePoint " << m_knowledgePoint;
}

bool KWAnchorStrategy::countHorizontalRel(QRectF &anchorBoundingRect, QRectF containerBoundingRect, KWPageTextInfo *pageInfo,
                                          KoTextDocumentLayout::LayoutState *state, QTextBlock &block, QTextLayout *layout)
{
    switch (m_anchor->horizontalRel()) {
     case KoTextAnchor::HPage:
         anchorBoundingRect.setX(0);
         anchorBoundingRect.setWidth(pageInfo->page().width());
         break;

     case KoTextAnchor::HParagraph:
     case KoTextAnchor::HPageContent:
         anchorBoundingRect.setX(containerBoundingRect.x());
         anchorBoundingRect.setWidth(containerBoundingRect.width());
         break;

     case KoTextAnchor::HParagraphContent:
         anchorBoundingRect.setX(state->x() + containerBoundingRect.x());
         anchorBoundingRect.setWidth(state->width());
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
             } else {
                 return false; // lets go for a second round.
             }
         } else {
             return false; // lets go for a second round.
         }
         break;

     case KoTextAnchor::HPageStartMargin:
     {
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
         break;
     }
     case KoTextAnchor::HPageEndMargin:
     {
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
         break;
     }
     case KoTextAnchor::HParagraphStartMargin:
     {
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
         break;
     }
     case KoTextAnchor::HParagraphEndMargin:
     {
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
         break;
     }
     default :
         kDebug(32002) << "horizontal-rel not handled";
     }
    return true;
}

void KWAnchorStrategy::countHorizontalPos(QPointF &newPosition, QRectF anchorBoundingRect, QRectF containerBoundingRect, KWPageTextInfo *pageInfo)
{
    switch (m_anchor->horizontalPos()) {
    case KoTextAnchor::HCenter:
        newPosition.setX(anchorBoundingRect.x() + anchorBoundingRect.width()/2 - containerBoundingRect.x());
        break;

    case KoTextAnchor::HFromInside:
    case KoTextAnchor::HInside:
    {
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
}

bool KWAnchorStrategy::countVerticalRel(QRectF &anchorBoundingRect, QRectF containerBoundingRect, KWPageTextInfo *pageInfo,
                                        KWTextFrameSet *frameSet, KoTextShapeData *data, QTextBlock &block, QTextLayout *layout)
{
    switch (m_anchor->verticalRel()) {
     case KoTextAnchor::VPage:
         anchorBoundingRect.setY(pageInfo->page().offsetInDocument());
         anchorBoundingRect.setHeight(pageInfo->page().height());
         break;

     case KoTextAnchor::VPageContent:
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
             KoTextBlockData *blockData = dynamic_cast<KoTextBlockData*>(block.userData());
             if(blockData && m_anchor->verticalRel() == KoTextAnchor::VParagraph) {
                 anchorBoundingRect.setY(blockData->effectiveTop() + containerBoundingRect.y()  - data->documentOffset());
             }
         } else {
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
             return false; // lets go for a second round.
         }
         break;

     case KoTextAnchor::VText: // same as char apparently only used when as-char
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
             return false; // lets go for a second round.
         }
         break;

     case KoTextAnchor::VBaseline:
         if (layout->lineCount()) {
             QTextLine tl = layout->lineForTextPosition(m_anchor->positionInDocument() - block.position());
             Q_ASSERT(tl.isValid());
             anchorBoundingRect.setY(tl.y() + tl.ascent() - m_anchor->shape()->size().height()
                + containerBoundingRect.y() - data->documentOffset());
             anchorBoundingRect.setHeight(2*m_anchor->shape()->size().height());
         } else {
             return false; // lets go for a second round.
         }
         break;
     default :
         kDebug(32002) << "vertical-rel not handled";
     }
    return true;
}

void KWAnchorStrategy::countVerticalPos(QPointF &newPosition, QRectF anchorBoundingRect, QRectF containerBoundingRect)
{
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

}

void KWAnchorStrategy::checkPageBorder(QPointF &newPosition, QRectF containerBoundingRect, KWPageTextInfo *pageInfo)
{
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

bool KWAnchorStrategy::checkTextIntersecion(QPointF &relayoutPos, QRectF shpRect, QRectF contRect, KoTextDocumentLayout::LayoutState *state,
                                            KoTextShapeData *data)
{
    bool intersectionX = false;
    bool intersectionY = false;

    //check text area
    QTextLayout *layout = state->layout;
    QTextLine tl = layout->lineAt(layout->lineCount() - 1);

    qreal shpRight = shpRect.left() + shpRect.width();
    qreal contRight = contRect.width();
    qreal shpBottom = shpRect.top() + shpRect.height();
    qreal contBottom = tl.y() + tl.height() - data->documentOffset();

    //horizontal
    if (((shpRect.left() > 0) && (shpRight > 0) && (shpRect.left() < contRight)) ||
        ((shpRight > 0) && (shpRect.left() < contRight) && (shpRight < contRight)) ||
        ((shpRect.left() < 0) && (shpRect.right() > contRight))) {
        if (shpRect.left() < 0) {
            relayoutPos.setX(0);
        } else {
            relayoutPos.setX(shpRect.left());
        }
        intersectionX = true;
    }

    //vertical
    if (((shpRect.top() > 0) && (shpBottom > 0) && (shpRect.top() < contBottom)) ||
        ((shpBottom > 0) && (shpRect.top() < contBottom) && (shpBottom < contBottom)) ||
        ((shpRect.top() < 0) && (shpBottom > contBottom))) {
        if (shpRect.top() < 0) {
            relayoutPos.setY(0);
        } else {
            relayoutPos.setY(shpRect.top());
        }
        intersectionY = true;
    }

    if (intersectionX && intersectionY) {
        return true;
    }

    return false;
}

void KWAnchorStrategy::calculateRelayoutPosition(QPointF relayoutPos, KoTextDocumentLayout::LayoutState *state, KoTextShapeData *data)
{

    qreal recalcFrom = relayoutPos.y() + data->documentOffset();

    do {
        if (recalcFrom >= state->y()) {
            break;
        }
    } while (state->previousParag());
}
