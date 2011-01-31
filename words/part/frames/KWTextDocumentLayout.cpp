/* This file is part of the KDE project
 * Copyright (C) 2006-2007, 2010 Thomas Zander <zander@kde.org>
 * Copyright (C) 2010 Ko Gmbh <casper.boemann@kogmbh.com>
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

#include "KWTextDocumentLayout.h"
#include "KWTextFrameSet.h"
#include "KWTextFrame.h"
#include "KWCopyShape.h"
#include "../KWDocument.h"
#include "../KWPage.h"
#include "../KWPageTextInfo.h"
#include "KWOutlineShape.h"

#include <KoTextShapeData.h>
#include <KoShapeContainer.h>
#include <KoInlineTextObjectManager.h>
#include <KoTextAnchor.h>
#include <KDebug>

#include <QList>
#include <QPainterPath>
#include <QTextBlock>
#include <QRectF>

// #define DEBUG_TEXT


#ifdef DEBUG_TEXT
#define TDEBUG kDebug(32002)
#else
#define TDEBUG if(0) kDebug(32002)
#endif

// helper methods

static qreal yAtX(const QLineF &line, qreal x)
{
    if (line.dy() == 0)
        return line.y1();
    return line.y1() + (x - line.x1()) / line.dx() * line.dy();
}

static qreal xAtY(const QLineF &line, qreal y)
{
    if (line.dx() == 0)
        return line.x1();
    return line.x1() + (y - line.y1()) / line.dy() * line.dx();
}

/// Returns 0, one or two points where the line intersects with the rectangle.
static QList<QPointF> intersect(const QRectF &rect, const QLineF &line)
{
    QList<QPointF> answer;
    QPointF startOfLine = line.p1();
    QPointF endOfLine = line.p2();
    // top edge
    if ((startOfLine.y() <= rect.top() && endOfLine.y() >= rect.top()) ||
            (startOfLine.y() >= rect.top() && endOfLine.y() <= rect.top())) {
        qreal x = xAtY(line, rect.top());
        if (x >= rect.left() && x <= rect.right() && x)
            answer.append(QPointF(x, rect.top()));
    }

    // left
    if ((startOfLine.x() <= rect.left() && endOfLine.x() >= rect.left()) ||
            (startOfLine.x() >= rect.left() && endOfLine.x() <= rect.left())) {
        qreal y = yAtX(line, rect.left());
        if (y >= rect.top() && y <= rect.bottom())
            answer.append(QPointF(rect.left(), y));
    }

    // bottom edge
    if ((startOfLine.y() <= rect.bottom() && endOfLine.y() >= rect.bottom()) ||
            (startOfLine.y() >= rect.bottom() && endOfLine.y() <= rect.bottom())) {
        qreal x = xAtY(line, rect.bottom());
        if (x >= rect.left() && x <= rect.right())
            answer.append(QPointF(x, rect.bottom()));
    }

    // right
    if ((startOfLine.x() <= rect.right() && endOfLine.x() >= rect.right()) ||
            (startOfLine.x() >= rect.right() && endOfLine.x() <= rect.right())) {
        qreal y = yAtX(line, rect.right());
        if (y >= rect.top() && y <= rect.bottom())
            answer.append(QPointF(rect.right(), y));
    }

    return answer;
}

class KWTextDocumentLayout::DummyShape : public KoShape
{
public:
    DummyShape(QTextDocument *doc) : textShapeData(new KoTextShapeData()) {
        textShapeData->setDocument(doc, false);
        setUserData(textShapeData);
        //setPosition(QPointF(10E6, 10E6));
    }

    KoTextShapeData * const textShapeData; // will be deleted by KoShape

private:
    virtual void paint(QPainter&, const KoViewConverter&) {}
    virtual void saveOdf(KoShapeSavingContext &) const { }
    virtual bool loadOdf(const KoXmlElement &, KoShapeLoadingContext &) {
        return false;
    }
};

KWTextDocumentLayout::KWTextDocumentLayout(KWTextFrameSet *frameSet)
        : KoTextDocumentLayout(frameSet->document()),
        m_frameSet(frameSet),
        m_dummyShape(new DummyShape(frameSet->document())),
        m_lastKnownFrameCount(0)
{
    if (m_frameSet->frameCount()) {
        KoTextShapeData *data = qobject_cast<KoTextShapeData*>(m_frameSet->frames().first()->shape()->userData());
        if (data) { // reset layout.
            data->setEndPosition(-1);
            data->foul();
        }
    }
}

KWTextDocumentLayout::~KWTextDocumentLayout()
{
    m_frameSet = 0;
    delete m_dummyShape;
}

QList<KoShape*> KWTextDocumentLayout::shapes() const
{
    QList<KoShape*> answer;
    foreach (KWFrame *frame, m_frameSet->frames()) {
        if (frame->isCopy())
            continue;
        answer.append(frame->shape());
    }
    return answer;
}

void KWTextDocumentLayout::relayout()
{
    if (! m_frameSet->allowLayout())
        return;
    const QList<KWFrame*> frames = m_frameSet->frames();
    QList<KWFrame*> dirtyFrames = frames;
    KWFrame *firstDirtyFrame = 0;
    foreach (KWFrame *frame, frames) {
        KoTextShapeData *data = qobject_cast<KoTextShapeData*>(frame->shape()->userData());
        if (!firstDirtyFrame && data && data->isDirty())
            firstDirtyFrame = frame;
        if (!firstDirtyFrame)
            dirtyFrames.removeAll(frame);
    }

    // make sure the ordering is proper
    m_frameSet->sortFrames();
    if (firstDirtyFrame) {
        // if the dirty frame has been resorted to no longer be the first one, then we should
        // mark dirty any frame that were previously later in the flow, but are now before it.
        foreach (KWFrame *frame, frames) {
            if (frame == firstDirtyFrame)
                break;
            if (dirtyFrames.contains(frame)) {
                static_cast<KoTextShapeData*>(frame->shape()->userData())->foul();
                // just the first is enough.
                break;
            }
        }
    }

    layout();
}

void KWTextDocumentLayout::positionInlineObject(QTextInlineObject item, int position, const QTextFormat &f)
{
    KoTextDocumentLayout::positionInlineObject(item, position, f);
#ifndef DEBUG
    if (inlineTextObjectManager() == 0) {
        kWarning(32002) << "Need to call setInlineObjectTextManager on the KoTextDocument!!";
        return;
    }
#endif
    KoTextAnchor *anchor = dynamic_cast<KoTextAnchor*>(inlineTextObjectManager()->inlineTextObject(f.toCharFormat()));
    if (anchor) { // special case anchors as positionInlineObject is called before layout; which is no good.

        KoShape *parent = anchor->shape()->parent();
        if (parent == 0) {
            return;
        }

        KWPage page = m_frameSet->pageManager()->page(parent);
        QRectF pageRect(0,page.offsetInDocument(),page.width(),page.height());
        QRectF pageContentRect = parent->boundingRect();
        int pageNumber = m_frameSet->pageManager()->pageNumber(parent);

        anchor->setPageRect(pageRect);
        //todo get the right position for headers and footers
        anchor->setPageContentRect(pageContentRect);
        anchor->setPageNumber(pageNumber);

        // if there is no anchor strategy set send the textAnchor into the layout to create anchor strategy and position it
        if (anchor->anchorStrategy() == 0) {
            m_state->insertInlineObject(anchor);
        }
    }
}


void KWTextDocumentLayout::layout()
{
    class End
    {
    public:
        End(KWTextFrameSet *frameSet, KoTextDocumentLayout::LayoutState *state) {
            m_frameSet = frameSet;
            m_state = state;
            //m_frameSet->setAllowLayout(false);
        }
        ~End() {
            m_state->end();
            //m_frameSet->setAllowLayout(true);
        }
    private:
        KWTextFrameSet *m_frameSet;
        KoTextDocumentLayout::LayoutState *m_state;
    };
    End ender(m_frameSet, m_state); // poor mans finally{}

    if (! m_state->start())
        return;

    qreal endPos = 1E9;
    qreal bottomOfText = 0.0;
    bool newParagraph = true;
    bool requestFrameResize = false, firstParagraph = true;
    int startOfBlock = -1; // the first position in a block (aka paragraph)
    int startOfBlockText = -1; // the first position of text in a block. Will be different only if anchors preceed text.
    KoShape *currentShape = 0;

    while (m_state->shape) {
        if (m_state->shape != currentShape) { // next shape
            TDEBUG << "New shape";
            currentShape = m_state->shape;
            if (m_frameSet->kwordDocument()) {

                //todo do the logic how shapes from different frame sets interact with text
//                QRectF bounds = m_state->shape->boundingRect();
//                foreach (KWFrameSet *fs, m_frameSet->kwordDocument()->frameSets()) {
//                    KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*>(fs);
//                    if (tfs && tfs->textFrameSetType() == KWord::MainTextFrameSet)
//                        continue;
//                    foreach (KWFrame *frame, fs->frames()) {
//                        if (frame->shape() == currentShape)
//                            continue;
//                        if (! frame->shape()->isVisible(true))
//                            continue;
//                        if (frame->shape()->textRunAroundSide() == KoShape::RunThrough)
//                            continue;
//                        if (frame->shape()->zIndex() <= currentShape->zIndex()) {
//                            continue;
//                        }
//                        if (! bounds.intersects(frame->shape()->boundingRect()))
//                            continue;
//                        bool isChild = false;
//                        KoShape *parent = frame->shape()->parent();
//                        while (parent && !isChild) {
//                            if (parent == currentShape)
//                                isChild = true;
//                            parent = parent->parent();
//                        }
//                        if (isChild)
//                            continue;
//                        m_state->registerRunAroundShape(frame->shape());
//                    }
//                }
                // set the page for the shape.
                KWPage page = m_frameSet->pageManager()->page(currentShape);
                Q_ASSERT(page.isValid());
                KoTextShapeData *data = qobject_cast<KoTextShapeData*>(currentShape->userData());
                Q_ASSERT(data);
                data->setPageDirection(page.directionHint());
                data->setPage(new KWPageTextInfo(page));
            }
        }

        QTextLine line = m_state->createLine();
        if (!line.isValid()) { // end of parag
            const qreal posY = m_state->y();
            if (firstParagraph) {
                // start counting after the resumed paragraph
                firstParagraph = false;
                endPos = posY + m_state->shape->size().height() * 2.2;
            }
            bool moreText = m_state->nextParag();
            if (m_state->shape && m_state->y() > posY)
                m_state->shape->update(QRectF(0, posY,
                                              m_state->shape->size().width(), m_state->y() - posY));

            if (! moreText) {
                const int frameCount = m_frameSet->frameCount();
                const int framesInUse = m_state->shapeNumber + 1;
                if (framesInUse < frameCount && framesInUse != m_lastKnownFrameCount)
                    m_frameSet->framesEmpty(frameCount - framesInUse);
                m_lastKnownFrameCount = frameCount;
                if (requestFrameResize) // text ran out while placing it in the dummy shape.
                    m_frameSet->requestMoreFrames(m_state->y() - m_dummyShape->textShapeData->documentOffset());
                else {
                    // if there is more space in the shape then there is text. Reset the no-grow bool.
                    QList<KWFrame*>::const_iterator iter = m_frameSet->frames().end();
                    KWTextFrame *lastFrame;
                    do {
                        iter--;
                        lastFrame = dynamic_cast<KWTextFrame*>(*iter);
                    } while (lastFrame == 0);
                    KoTextShapeData *data = qobject_cast<KoTextShapeData*>(lastFrame->shape()->userData());
                    Q_ASSERT(data);
                    // Now we probably need to shrink again to free space that is unused in the frame. Header/footer are special in that
                    // we only change the minimum height but not there actual height like with every other frameset.
                    qreal height = KWord::isHeaderFooter(m_frameSet) ? lastFrame->minimumFrameHeight() : lastFrame->shape()->size().height();
                    qreal spaceLeft = height - bottomOfText + data->documentOffset();
                    data->wipe();
                    if (spaceLeft > 3) {
                        // note that this may delete the data and lastFrame !!  Do not access them after this point.
                        m_frameSet->spaceLeft(spaceLeft - 3);
                    }
                }

                m_frameSet->layoutDone();
                emit finishedLayout();
                return; // done!
            } else if (m_state->shape == 0) {
                TDEBUG << "encountered an 'end of page' break, we need an extra page to honor that!";
                // encountered an 'end of page' break but we don't have any more pages(/shapes)
                m_state->clearTillEnd();
                m_frameSet->requestMoreFrames(0); // new page, please.
                currentShape->update();
                return;
            }
            newParagraph = true;
            continue;
        }
        if (m_state->isInterrupted() || (newParagraph && m_state->y() > endPos)) {
            // enough for now. Try again later.
            kDebug() << "schedule a next layout due to having done a layout of quite some space, interrupted="<<m_state->isInterrupted()<<"m_state->y()="<<m_state->y()<<"endPos="<<endPos;
            scheduleLayoutWithoutInterrupt();
            return;
        }
        newParagraph = false;

        m_state->fitLineForRunAround( /* resetHorizontalPosition */ false );

#ifdef DEBUG_TEXT
        if (line.isValid()) {
            QTextBlock b = document()->findBlock(m_state->cursorPosition());
            if (b.isValid()) {
                TDEBUG << "fitted line" << b.text().mid(line.textStart(), line.textLength());
                TDEBUG << "       1 @ " << line.position() << " from parag at pos " << b.position();
                TDEBUG << "         y " << line.y() << " h" <<  line.height();
            }
        }
#endif
        bottomOfText = line.y() + line.height();
        if (bottomOfText > m_state->shape->size().height() && document()->blockCount() == 1 && KWord::isHeaderFooter(m_frameSet)) {
            TDEBUG << "requestMoreFrames" << (bottomOfText - m_state->shape->size().height());
            m_frameSet->requestMoreFrames(bottomOfText - m_state->shape->size().height());
            return;
        }
        qreal lineheight = line.height();

        while (m_state->addLine() == false) {
            if (m_state->shape == 0) { // no more shapes to put the text in!
                TDEBUG << "no more shape for our text; bottom is" << m_state->y();

                if (requestFrameResize) { // plenty more text, but first lets resize the shape.
                    TDEBUG << "  we need more space; we require at least:" << m_dummyShape->size().height();
                    m_frameSet->requestMoreFrames(m_dummyShape->size().height());
                    m_frameSet->requestMoreFrames(0);
                    return; // done!
                }

                if (KWord::isHeaderFooter(m_frameSet)) { // more text, lets resize the header/footer.
                    TDEBUG << "  header/footer is too small resize:" << lineheight;
                    m_frameSet->requestMoreFrames(lineheight);
                    return; // done!
                }

                KWFrame *lastFrame = m_frameSet->frames().last();
                if (lastFrame->frameBehavior() == KWord::IgnoreContentFrameBehavior
                        || dynamic_cast<KWCopyShape*>(lastFrame)) {
                    m_state->clearTillEnd();
                    m_frameSet->layoutDone();
                    emit finishedLayout();
                    return; // done!
                }

                // find out the maximum size this frame can be extended to while still
                // fitting in the page.  We'll continue doing layout and see if there is text till end of page.
                KWPage page = m_frameSet->pageManager()->page(lastFrame->shape());
                QRectF pageRect = page.rect();
                pageRect.adjust(page.leftMargin(), page.topMargin(), -page.rightMargin(), -page.bottomMargin());

                QLineF top(QPointF(0, 0), QPointF(lastFrame->shape()->size().width(), 0));
                top = lastFrame->shape()->absoluteTransformation(0).map(top);
                const qreal multiplier = qMax(pageRect.height(), pageRect.width()) / top.length();
                QLineF down(top.p1(), QPointF(top.p1().x() - top.dy() * multiplier,
                                                top.p1().y() + top.dx() * multiplier));
                QLineF down2(top.p2(), QPointF(top.p2().x() - top.dy() * multiplier,
                                                top.p2().y() + top.dx() * multiplier));

                QList<QPointF> list = intersect(pageRect, down);
                if (list.count() > 0)
                    down = QLineF(down.p1(), list.last());
                list = intersect(pageRect, down2);
                if (list.count() > 0)
                    down2 = QLineF(down2.p1(), list.last());
                const qreal maxFrameLength = qMin(down.length(), down2.length());
                if (qAbs(maxFrameLength - currentShape->size().height()) < 1) {
                    m_state->clearTillEnd();
                    TDEBUG << "  we need another page";
                    m_frameSet->requestMoreFrames(0); // new page, please.
                    return;
                }
                KoTextShapeData *data = qobject_cast<KoTextShapeData*>(lastFrame->shape()->userData());
                Q_ASSERT(data);

                m_dummyShape->setSize(QSizeF(currentShape->size().width(), maxFrameLength - currentShape->size().height()));
                m_dummyShape->textShapeData->setShapeMargins(data->shapeMargins());
                if (! m_state->setFollowupShape(m_dummyShape)) { // if I can't render into a dummy shape
                    m_state->clearTillEnd();
                    m_frameSet->layoutDone();
                    emit finishedLayout();
                    return; // done!
                }
                requestFrameResize = true;
            }
            if (m_state->shape != currentShape) {
                // we are in a new shape, and line was not added !
                currentShape->update();
                KoTextShapeData *data = qobject_cast<KoTextShapeData*>(m_state->shape->userData());
                if (data) { // reset layout.
                    data->setEndPosition(-1);
                    data->foul();
                }
                m_state->clearTillEnd();
                break; //break so the next line (which contain the same) is fitted on new shape
            }

            // don't try to use line when layout is cleared
            if (m_state->layout->lineCount() == 0) {
                break;
            }

            m_state->fitLineForRunAround( /* resetHorizontalPosition */ true );

#ifdef DEBUG_TEXT
            if (line.isValid()) {
                QTextBlock b = document()->findBlock(m_state->cursorPosition());
                if (b.isValid()) {
                    TDEBUG << "fitted line" << b.text().mid(line.textStart(), line.textLength());
                    TDEBUG << "       2 @ " << line.position() << " from parag at pos " << b.position();
                }
            }
#endif
        }
    }
    if (requestFrameResize) {
        TDEBUG << "  requestFrameResize" << m_dummyShape->size().height();
        m_frameSet->requestMoreFrames(m_dummyShape->size().height());
    }
}
