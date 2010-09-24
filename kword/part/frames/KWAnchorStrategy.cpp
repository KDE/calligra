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

#include <QTextBlock>
#include <QTextLine>

#include <KDebug>

KWAnchorStrategy::KWAnchorStrategy(KoTextAnchor *anchor)
        : m_anchor(anchor),
        m_finished(false),
        m_currentLineY(0),
        m_pass(0),
        m_lastknownPosInDoc(-1),
        m_lastVerticalAnchorAlignment(KoTextAnchor::TopOfFrame),
        m_lastHorizontalAnchorAlignment(KoTextAnchor::Left)
{
    calculateKnowledgePoint();
}

KWAnchorStrategy::~KWAnchorStrategy()
{
}

bool KWAnchorStrategy::checkState(KoTextDocumentLayout::LayoutState *state, int startOfBlock, int startOfBlockText, KWTextFrameSet *frameSet)
{
    m_nextShapeNeeded = false;

    KoTextShapeData *data = 0;
    KoShape *shapeOfAnchor = 0;
    foreach (KWFrame *frame, frameSet->frames()) { //find the frame for the anchor
        KoTextShapeData *tmpData = qobject_cast<KoTextShapeData*>(frame->shape()->userData());
        if(tmpData != 0) {
            if(m_anchor->positionInDocument() > tmpData->position()) {
                data = tmpData;
                shapeOfAnchor = frame->shape();
            }
            if(m_anchor->positionInDocument() < tmpData->endPosition()) {
                break;
            }
        }
    }
    Q_ASSERT(data);

    if (m_anchor->shape()->parent() == 0) { // it should be parented to our current shape
//        KoShapeContainer *sc = dynamic_cast<KoShapeContainer*>(state->shape);
        KoShapeContainer *sc = dynamic_cast<KoShapeContainer*>(shapeOfAnchor);
        if (sc == 0) {
            kWarning(32002) << "Failed to attach the anchored shape to a text shape...";
            return false;
        }
        sc->addShape(m_anchor->shape());
        calculateKnowledgePoint();
    }
    if (m_knowledgePoint < 0)
        return false;

    if (m_lastknownPosInDoc != m_anchor->positionInDocument()
            || m_lastOffset != m_anchor->offset()
            || m_lastVerticalAnchorAlignment != m_anchor->verticalAlignment()
            || m_lastHorizontalAnchorAlignment != m_anchor->horizontalAlignment()) { // different layout run
        m_finished = false;
        m_lastknownPosInDoc = m_anchor->positionInDocument();
        m_lastOffset = m_anchor->offset();
        m_lastVerticalAnchorAlignment = m_anchor->verticalAlignment();
        m_lastHorizontalAnchorAlignment = m_anchor->horizontalAlignment();
        m_pass = 0;
    }
    QTextBlock block = m_anchor->document()->findBlock(m_anchor->positionInDocument());
    // kDebug() << m_anchor->positionInDocument() << "pass:" << m_pass <<"pos:" << state->cursorPosition() <<" (need;" << m_knowledgePoint << (m_finished?") Already finished!":")");
    // exit when finished or when we can expect another call with a higher cursor position
    if (m_finished || (m_knowledgePoint > state->cursorPosition()))
        return false;

    // *** alter 'state' to relayout the part we want.
    QTextLayout *layout = block.layout();
    int recalcFrom = state->cursorPosition(); // the position from which we will restart layout.

    QRectF boundingRect = m_anchor->shape()->boundingRect();
    QRectF containerBoundingRect = m_anchor->shape()->parent()->boundingRect();
    QPointF newPosition;
    switch (m_anchor->horizontalAlignment()) {
    case KoTextAnchor::ClosestToBinding: // TODO figure out a way to do pages...
    case KoTextAnchor::Left: {
        // take into account text indent and alignment.
        Qt::Alignment alignment = block.layout()->textOption().alignment() & Qt::AlignHorizontal_Mask;
        if ((alignment & Qt::AlignLeft) || (alignment & Qt::AlignJustify)) {
            newPosition.setX(state->x());
        } else if (alignment & Qt::AlignRight) {
            newPosition.setX(state->x() + state->width() - boundingRect.width()
                    - m_anchor->offset().x());
        } else if (alignment & Qt::AlignHCenter) {
            newPosition.setX((state->x() + state->width()) / 2 - boundingRect.width() / 2);
        }
        recalcFrom = block.position();
        break;
    }
    case KoTextAnchor::FurtherFromBinding: // TODO figure out a way to do pages...
    case KoTextAnchor::Right:
        newPosition.setX(containerBoundingRect.width() - boundingRect.width());
        recalcFrom = block.position();
        break;
    case KoTextAnchor::Center:
        newPosition.setX((containerBoundingRect.width() - boundingRect.width()) / 2.0);
        recalcFrom = block.position();
        break;
    case KoTextAnchor::HorizontalOffset: {
        qreal x;
        if (m_anchor->positionInDocument() == block.position()) {
            // at first position of parag.
            x = state->x();
        } else {
            Q_ASSERT(layout->lineCount());
            QTextLine tl = layout->lineForTextPosition(m_anchor->positionInDocument() - block.position());
            Q_ASSERT(tl.isValid());
            x = tl.cursorToX(m_anchor->positionInDocument() - block.position());
            recalcFrom = 0;
        }
        newPosition.setX(x);
        m_finished = true;
        break;
    }
    case KoTextAnchor::LeftOfPage: {
        newPosition.setX(-containerBoundingRect.x());
        break;
    }
    case KoTextAnchor::RightOfPage: {
        KWPageTextInfo *pageInfo = dynamic_cast<KWPageTextInfo *>(data->page());
        if (pageInfo) {
            newPosition.setX(pageInfo->page().width() - containerBoundingRect.x() - boundingRect.width());
        }
        break;
    }
    case KoTextAnchor::CenterOfPage: {
        KWPageTextInfo *pageInfo = dynamic_cast<KWPageTextInfo *>(data->page());
        if (pageInfo) {
            newPosition.setX(pageInfo->page().width()/2 - containerBoundingRect.x() - boundingRect.width()/2);
        }
        break;
    }
    default:
        Q_ASSERT(false); // new enum added?
    }

    switch (m_anchor->verticalAlignment()) {
    case KoTextAnchor::TopOfFrame:
        recalcFrom = qMax(recalcFrom, data->position());
        break;
    case KoTextAnchor::TopOfParagraph: {
        qDebug() <<"top of paragraph";
        if (layout->lineCount() == 0) {
            m_finished = false;
            return false;
        }
        qreal topOfParagraph = layout->lineAt(0).y();
        newPosition.setY(topOfParagraph - data->documentOffset());
        recalcFrom = qMax(recalcFrom, block.position());
        qDebug() <<"at"<<topOfParagraph<<data->documentOffset();
        break;
    }
    case KoTextAnchor::AboveCurrentLine:
    case KoTextAnchor::BelowCurrentLine: {
        QTextLine tl = layout->lineForTextPosition(m_anchor->positionInDocument() - block.position());
        Q_ASSERT(tl.isValid());
        m_currentLineY = tl.y() + tl.height() - data->documentOffset();
        if (m_anchor->verticalAlignment() == KoTextAnchor::BelowCurrentLine)
            newPosition.setY(m_currentLineY);
        else
            newPosition.setY(m_currentLineY - boundingRect.height() - tl.height());
        recalcFrom = qMax(recalcFrom, block.position());
        break;
    }
    case KoTextAnchor::BottomOfParagraph: {
        QTextLine tl = layout->lineAt(layout->lineCount() - 1);
        Q_ASSERT(tl.isValid());
        newPosition.setY(tl.y() + tl.height() - data->documentOffset());
        recalcFrom = qMax(recalcFrom, block.position());
        break;
    }
    case KoTextAnchor::BottomOfFrame:
        newPosition.setY(containerBoundingRect.height() - boundingRect.height());
        recalcFrom = qMax(recalcFrom, block.position()); // TODO move further back if shape is tall
        break;
    case KoTextAnchor::VerticalOffset: {
        qreal y;
        if (block.length() == 2 && !m_anchor->isPositionedInline()) {
            //m_anchor->horizontalAlignment()!=KoTextAnchor::HorizontalOffset) {
            // the anchor is the only thing in the block, and not inline
            y = state->y();
        } else if (layout->lineCount()) {
            QTextLine tl = layout->lineForTextPosition(m_anchor->positionInDocument() - block.position());
            Q_ASSERT(tl.isValid());
            y = tl.y() + tl.ascent();
            recalcFrom = block.position();
            m_finished = true;
        } else {
            m_finished = false;
            return false; // lets go for a second round.
        }
        newPosition.setY(y - data->documentOffset());
        break;
    }
    case KoTextAnchor::TopOfPage: {
        KWPageTextInfo *pageInfo = dynamic_cast<KWPageTextInfo *>(data->page());
        if(pageInfo) {
            newPosition.setY(pageInfo->page().offsetInDocument() - containerBoundingRect.y());
        }
        break;
    }
    case KoTextAnchor::BottomOfPage: {
        KWPageTextInfo *pageInfo = dynamic_cast<KWPageTextInfo *>(data->page());
        if(pageInfo) {
            newPosition.setY(pageInfo->page().offsetInDocument() + pageInfo->page().height()
                            - containerBoundingRect.y() - boundingRect.height());
        }
        break;
    }
    case KoTextAnchor::TopOfPageContent: {
        KWPageTextInfo *pageInfo = dynamic_cast<KWPageTextInfo *>(data->page());
        if (!pageInfo)
            break;
        // find main frame
        KWTextFrameSet *tfs =frameSet->kwordDocument()->mainFrameSet();
        if (tfs) {
            foreach (KWFrame *frame, tfs->frames()) { //find main frame for current page
                KoTextShapeData *tmpData = qobject_cast<KoTextShapeData*>(frame->shape()->userData());
                if(tmpData != 0) {
                    KWPageTextInfo *tmpPageInfo = dynamic_cast<KWPageTextInfo *>(tmpData->page());
                    if (tmpPageInfo != 0) {
                        if (tmpPageInfo->pageNumber() == pageInfo->pageNumber()) {//found main frame for current page
                            newPosition.setY(frame->shape()->boundingRect().y() - containerBoundingRect.y());
                            break;
                        }
                    }
                }
            }
            break;
        }
        break;
    }
    case KoTextAnchor::BottomOfPageContent: {
        KWPageTextInfo *pageInfo = dynamic_cast<KWPageTextInfo *>(data->page());
        if (!pageInfo)
            break;
        // find main frame
        KWTextFrameSet *tfs =frameSet->kwordDocument()->mainFrameSet();
        if (tfs) {
            foreach (KWFrame *frame, tfs->frames()) { //find main frame for current page
                KoTextShapeData *tmpData = qobject_cast<KoTextShapeData*>(frame->shape()->userData());
                if(tmpData != 0) {
                    KWPageTextInfo *tmpPageInfo = dynamic_cast<KWPageTextInfo *>(tmpData->page());
                    if (tmpPageInfo != 0) {
                        if (tmpPageInfo->pageNumber() == pageInfo->pageNumber()) {//found main frame for current page
                            newPosition.setY(frame->shape()->boundingRect().bottom() -
                                             containerBoundingRect.y() - frame->shape()->boundingRect().height());
                            break;
                        }
                    }
                }
            }
            break;
        }
        break;
    }
    default:
        Q_ASSERT(false); // new enum added?
    }
    newPosition = newPosition + m_anchor->offset();

    if (shapeOfAnchor) {
        qDebug() << "checking"<<newPosition.y()<<newPosition.y() + m_anchor->shape()->boundingRect().height()<<shapeOfAnchor->boundingRect().height();
        //Check if the new position will place the anchor shape (even partly) beyond textframe
        if (newPosition.y() + m_anchor->shape()->boundingRect().height()
                > shapeOfAnchor->boundingRect().height()) {
            m_nextShapeNeeded = true;
            qDebug() << "SETTING NEEDED";
        }
    }

    QPointF diff = newPosition - m_anchor->shape()->position();
    const bool moved = qAbs(diff.x()) > 0.5 || qAbs(diff.y()) > 0.5;
    if (!m_finished && m_pass > 0) { // already been here
        // for the cases where we align with text; check if the text is within margin. If so; set finished to true.
        m_finished = !moved;
    } else if (!m_finished && m_pass == 0 && m_anchor->isPositionedInline()) {
        // inline chars are 'run around' already by the qt text flow. Only one pass needed
        m_finished = true;
    }
    m_pass++;

    if (moved) {
        // set the shape to the proper position based on the data
        m_anchor->shape()->update();
        //kDebug() << "anchor positioned" << newPosition << "/" << m_anchor->shape()->position();
        //kDebug() << "finished" << m_finished;
        m_anchor->shape()->setPosition(newPosition);
        m_anchor->shape()->update();
    }

    if (m_finished) // no second pass needed
        return false;

    do { // move the layout class back a couple of paragraphs.
        if (recalcFrom >= startOfBlock && recalcFrom < startOfBlockText)
            break;
        if (state->cursorPosition() <= recalcFrom)
            break;
    } while (state->previousParag());
    return true;
}

bool KWAnchorStrategy::isFinished()
{
    // if, for the second time, we passed the point where the anchor was inserted, return true
    return m_finished;
}

KoShape *KWAnchorStrategy::anchoredShape() const
{
    if (m_anchor->isPositionedInline())
        return 0;
    return m_anchor->shape();
}

void KWAnchorStrategy::calculateKnowledgePoint()
{
    m_knowledgePoint = -1;
    // figure out until what cursor position we need to layout to get all the info we need
    switch (m_anchor->horizontalAlignment()) {
    case KoTextAnchor::ClosestToBinding:
    case KoTextAnchor::Left:
    case KoTextAnchor::FurtherFromBinding:
    case KoTextAnchor::Right:
    case KoTextAnchor::Center:
    case KoTextAnchor::LeftOfPage:
    case KoTextAnchor::RightOfPage:
    case KoTextAnchor::CenterOfPage: {
        if (m_anchor->shape()->parent() == 0) // not enough info yet.
            return;
        KoTextShapeData *data = qobject_cast<KoTextShapeData*>(m_anchor->shape()->parent()->userData());
        Q_ASSERT(data);
        m_knowledgePoint = data->position();
        break;
    }
    case KoTextAnchor::HorizontalOffset:
        m_knowledgePoint = m_anchor->positionInDocument()+1;
    }
    switch (m_anchor->verticalAlignment()) {
    case KoTextAnchor::TopOfParagraph:
        m_knowledgePoint = qMax(m_knowledgePoint,
                                m_anchor->document()->findBlock(m_anchor->positionInDocument()).position() + 1);
        break;
    case KoTextAnchor::VerticalOffset:
    case KoTextAnchor::AboveCurrentLine:
    case KoTextAnchor::BelowCurrentLine:
    case KoTextAnchor::TopOfPage:
    case KoTextAnchor::BottomOfPage:
    case KoTextAnchor::TopOfPageContent:
    case KoTextAnchor::BottomOfPageContent:
        m_knowledgePoint = qMax(m_knowledgePoint, m_anchor->positionInDocument()+1);
        break;
    case KoTextAnchor::TopOfFrame:
    case KoTextAnchor::BottomOfFrame: {
        if (m_anchor->shape()->parent() == 0) // not enough info yet.
            return;
        KoTextShapeData *data = qobject_cast<KoTextShapeData*>(m_anchor->shape()->parent()->userData());
        Q_ASSERT(data);
        m_knowledgePoint = qMax(m_knowledgePoint, data->position() + 1);
        break;
    }
    case KoTextAnchor::BottomOfParagraph: {
        QTextBlock block = m_anchor->document()->findBlock(m_anchor->positionInDocument());
        m_knowledgePoint = qMax(m_knowledgePoint, block.position() + block.length() - 2);
        break;
    }
    }
}
