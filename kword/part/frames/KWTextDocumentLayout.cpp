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
#include "KWAnchorStrategy.h"
#include "KWOutlineShape.h"
#include "Outline.h"

#include <KoTextShapeData.h>
#include <KoShapeContainer.h>
#include <KoInlineTextObjectManager.h>
#include <KoTextAnchor.h>
#include <KDebug>

#include <QList>
#include <QPainterPath>
#include <QTextBlock>

// #define DEBUG_TEXT
// #define DEBUG_ANCHORS

#ifdef DEBUG_TEXT
#define TDEBUG kDebug(32002)
#else
#define TDEBUG if(0) kDebug(32002)
#endif

#ifdef DEBUG_ANCHORS
#define ADEBUG kDebug(32002)
#else
#define ADEBUG if(0) kDebug(32002)
#endif

// helper methods

static qreal yAtX(const QLineF &line, qreal x)
{
    if (line.dy() == 0)
        return line.y1();
    return line.y1() + (x - line.x1()) / line.dx() * line.dy();
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
        qreal x = Outline::xAtY(line, rect.top());
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
        qreal x = Outline::xAtY(line, rect.bottom());
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
        foreach (KWAnchorStrategy *strategy, m_activeAnchors + m_newAnchors) {
            if (strategy->anchor() == anchor)
                return;
        }
        ADEBUG << "new anchor";
        m_newAnchors.append(new KWAnchorStrategy(anchor));
    }
}

void KWTextDocumentLayout::layout()
{
    TDEBUG << "starting layout pass" << ((void*)document())
#ifdef DEBUG_ANCHORS
     << "m_newAnchors" << m_newAnchors.count() << "m_activeAnchors" << m_activeAnchors.count();
#else
    ;
#endif

    QList<Outline*> outlines;
    class End
    {
    public:
        End(KoTextDocumentLayout::LayoutState *state, QList<Outline*> *outlines) {
            m_state = state; m_outlines = outlines;
        }
        ~End() {
            m_state->end(); qDeleteAll(*m_outlines);
        }
    private:
        KoTextDocumentLayout::LayoutState *m_state;
        QList<Outline*> *m_outlines;
    };
    End ender(m_state, &outlines); // poor mans finally{}

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
        ADEBUG << "> loop.... layout has" << m_state->layout->lineCount() << "lines, we have" << m_activeAnchors.count() << "+" << m_newAnchors.count() <<"anchors";
#ifndef DEBUG_ANCHORS
        for (int i = 0; i < m_state->layout->lineCount(); ++i) {
            QTextLine line = m_state->layout->lineAt(i);
            ADEBUG << i << "]" << (line.isValid() ? QString("%1 - %2").arg(line.textStart()).arg(line.textLength()) : QString("invalid"));
        }
#endif
        if (m_state->layout->lineCount() == 0 && startOfBlock != m_state->cursorPosition()) {  // new paragraph
            startOfBlock = m_state->cursorPosition();
            startOfBlockText = m_state->cursorPosition();
        }

        class Line
        {
        public:
            Line(KoTextDocumentLayout::LayoutState *state) : m_state(state) {
                line = m_state->layout->createLine();
            }
            bool isValid() const {
                return line.isValid();
            }
            void tryFit() {
                const qreal leftIndent = m_state->x();
                const qreal width = m_state->width();
                QRectF rect(leftIndent, m_state->y(), width, 1.);
                line.setLineWidth(rect.width());
                if (rect.width() <= 0. || line.textLength() == 0) {
                    // margin so small that the text can't fit.
                    if (m_state->layout->lineCount() > 1
                            || m_state->layout->text().length() > 0) // parag not empty
                        line.setNumColumns(1); // allow at least one char.
                    line.setPosition(QPointF(rect.x(), rect.y()));
                    return;
                }
                rect = limit(rect);

                qreal movedDown = 0;
                qreal eligableTop = rect.top();
                while (true) {
                    if (m_state->numColumns() > 0) {
                        line.setNumColumns(m_state->numColumns());
                        line.setPosition(QPointF(line.x(), rect.y()));
                    } else {
                        qreal x = rect.x();
                        qreal effectiveLineWidth = rect.width();
                        if (leftIndent < x) { // limit moved the left edge, keep the indent.
                            x += leftIndent;
                            effectiveLineWidth -= leftIndent;
                        }
                        line.setLineWidth(effectiveLineWidth);
                        line.setPosition(QPointF(x, rect.y()));
                    }
                    rect.setHeight(line.height());

                    QRectF newLine = limit(rect);
                    if (newLine.width() <= 0 || line.textLength() == 0
                            || line.cursorToX(line.textStart()+1) > newLine.right()) {
                        const int Move = 10;
                        movedDown += Move;
                        if (movedDown > m_state->shape->size().height() * 1.3) {
                            // TODO the magic number of shape height needs some history
                            // investigation here.
                            rect = QRectF(leftIndent, eligableTop, width, rect.height());
                            break;
                        }
                        if (newLine.width() > 0) {
                            // at least one char fits
                            eligableTop = rect.top();
                        }

                        rect = QRectF(leftIndent, rect.top() + Move, width, rect.height());
                    } else if (qAbs(newLine.left() - rect.left()) < 1E-10 && qAbs(newLine.right()
                                - rect.right()) < 1E-10) {
                        break;
                    } else if (newLine.isValid()) {
                        rect = newLine;
                    } else {
                        break;
                    }
                }
            }
            void setOutlines(const QList<Outline*> &outlines) {
                m_outlines = &outlines;
            }

            QTextLine line;
        private:
            QRectF limit(const QRectF &rect) {
                QRectF answer = rect;
                foreach (Outline *outline, *m_outlines)
                    answer = outline->limit(answer);

                return answer;
            }
            KoTextDocumentLayout::LayoutState *m_state;
            const QList<Outline*> *m_outlines;
            qreal m_initialIndent;
        };

        if (m_state->shape != currentShape) { // next shape
            TDEBUG << "New shape";
            currentShape = m_state->shape;
            if (m_frameSet->kwordDocument()) {
                // refresh the outlines cache.
                qDeleteAll(outlines);
                outlines.clear();

                QRectF bounds = m_state->shape->boundingRect();
                foreach (KWFrameSet *fs, m_frameSet->kwordDocument()->frameSets()) {
                    KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*>(fs);
                    if (tfs && tfs->textFrameSetType() == KWord::MainTextFrameSet)
                        continue;
                    foreach (KWFrame *frame, fs->frames()) {
                        if (frame->shape() == currentShape)
                            continue;
                        if (! frame->shape()->isVisible(true))
                            continue;
                        if (frame->textRunAround() == KWord::RunThrough)
                            continue;
                        if (frame->outlineShape()) {
                            if (frame->outlineShape()->zIndex() <= currentShape->zIndex())
                                continue;
                        } else if (frame->shape()->zIndex() <= currentShape->zIndex()) {
                            continue;
                        }
                        if (! bounds.intersects(frame->shape()->boundingRect()))
                            continue;
                        bool isChild = false;
                        KoShape *parent = frame->shape()->parent();
                        while (parent && !isChild) {
                            if (parent == currentShape)
                                isChild = true;
                            parent = parent->parent();
                        }
                        if (isChild)
                            continue;
                        QTransform matrix = (frame->outlineShape()
                                ? frame->outlineShape()
                                : frame->shape())->absoluteTransformation(0);
                        matrix = matrix * currentShape->absoluteTransformation(0).inverted();
                        matrix.translate(0, m_state->documentOffsetInShape());
                        outlines.append(new Outline(frame, matrix));
                    }
                }
                // set the page for the shape.
                KWPage page = m_frameSet->pageManager()->page(currentShape);
                Q_ASSERT(page.isValid());
                KoTextShapeData *data = qobject_cast<KoTextShapeData*>(currentShape->userData());
                Q_ASSERT(data);
                data->setPageDirection(page.directionHint());
                data->setPage(new KWPageTextInfo(page));
            }
        }

        // anchors might require us to do some layout again, give it the chance to 'do as it will'
        bool restartLine = false;
        foreach (KWAnchorStrategy *strategy, m_activeAnchors + m_newAnchors) {
            ADEBUG << "checking anchor";
            QPointF old;
            if (strategy->anchoredShape())
                old = strategy->anchoredShape()->position();
            if (strategy->checkState(m_state, startOfBlock, startOfBlockText, m_frameSet)) {
                ADEBUG << "  restarting line";
                restartLine = true;
            }
            if (strategy->anchoredShape() && old != strategy->anchoredShape()->position()) {
                // refresh outlines in case the shape moved.
                foreach (Outline *outline, outlines) {
                    if (outline->shape() == strategy->anchoredShape()) {
                        ADEBUG << "  refreshing outline";
                        QTransform matrix = strategy->anchoredShape()->absoluteTransformation(0);
                        matrix = matrix * currentShape->absoluteTransformation(0).inverted();
                        matrix.translate(0, m_state->documentOffsetInShape());
                        outline->changeMatrix(matrix);
                        break;
                    }
                }
            }
            if (restartLine)
                break;
        }

        if (restartLine)
            continue;
        foreach (KWAnchorStrategy *strategy, m_activeAnchors + m_newAnchors) {
            ADEBUG << " + isFinished?"<< strategy->isFinished();
            if (strategy->isFinished() && strategy->anchor()->positionInDocument() < m_state->cursorPosition()) {
                ADEBUG << "  is finished";
                m_activeAnchors.removeAll(strategy);
                m_newAnchors.removeAll(strategy);
                delete strategy;
            }
        }
        foreach (KWAnchorStrategy *strategy, m_newAnchors) {
            ADEBUG << "  migrating strategy!";
            if (strategy->anchoredShape()) {
                KWFrame *frame = dynamic_cast<KWFrame*>(strategy->anchoredShape()->applicationData());
                if (! frame || frame->textRunAround() != KWord::RunThrough) {
                    QTransform matrix = strategy->anchoredShape()->absoluteTransformation(0);
                    matrix = matrix * currentShape->absoluteTransformation(0).inverted();
                    matrix.translate(0, m_state->documentOffsetInShape());
                    outlines.append(new Outline(strategy->anchoredShape(), matrix));
                 }
                // if the anchor occupies the first character of our block, create one line for it.
                const int cursorPosition = m_state->cursorPosition();
                if (cursorPosition == startOfBlockText && strategy->anchor()->positionInDocument() == cursorPosition) {
                    ADEBUG << "   creating line for anchor";
                    QTextLine line = m_state->layout->createLine();
                    line.setNumColumns(1);
                    line.setPosition(QPointF(m_state->x(), m_state->y()));
                    m_state->addLine(line);
                    ++startOfBlockText;
                }
                restartLine = true;
                ADEBUG << "adding child outline";
            }
            m_activeAnchors.append(strategy);
        }
        m_newAnchors.clear();
        if (restartLine)
            continue;

        Line line(m_state);
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
                    qreal spaceLeft = lastFrame->shape()->size().height() - bottomOfText + data->documentOffset();
                    data->wipe();
                    if (spaceLeft > 3) {
                        // note that this may delete the data and lastFrame !!  Do not access them after this point.
                        m_frameSet->spaceLeft(spaceLeft - 3);
                    }
                }

                m_frameSet->layoutDone();
                cleanupAnchors();
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
            TDEBUG << "schedule a next layout due to having done a layout of quite some space";
            scheduleLayoutWithoutInterrupt();
            return;
        }
        newParagraph = false;
        line.setOutlines(outlines);
        const int anchorCount = m_newAnchors.count();
        line.tryFit();
        if (m_state->layout->lineCount() == 1 && anchorCount != m_newAnchors.count()) {
            // start parag over so we can correctly take the just found anchors into account.
            m_state->layout->endLayout();
            m_state->layout->beginLayout();
            continue;
        }
#ifdef DEBUG_TEXT
        if (line.line.isValid()) {
            QTextBlock b = document()->findBlock(m_state->cursorPosition());
            if (b.isValid()) {
                TDEBUG << "fitted line" << b.text().mid(line.line.textStart(), line.line.textLength());
                TDEBUG << "       1 @ " << line.line.position() << " from parag at pos " << b.position();
                TDEBUG << "         y " << line.line.y() << " h" <<  line.line.height();
            }
        }
#endif

        bottomOfText = line.line.y() + line.line.height();
        if (bottomOfText > m_state->shape->size().height() && document()->blockCount() == 1) {
            TDEBUG << "requestMoreFrames" << (bottomOfText - m_state->shape->size().height());
            m_frameSet->requestMoreFrames(bottomOfText - m_state->shape->size().height());
            cleanupAnchors();
            return;
        }

        while (m_state->addLine(line.line)) {
            if (m_state->shape == 0) { // no more shapes to put the text in!
                TDEBUG << "no more shape for our text; bottom is" << m_state->y();
                line.line.setPosition(QPointF(0, m_state->y() + 10000)); // move it away from any place that we're likely going to paint

                if (requestFrameResize) { // plenty more text, but first lets resize the shape.
                    TDEBUG << "  we need more space; we require at least:" << m_dummyShape->size().height();
                    m_frameSet->requestMoreFrames(m_dummyShape->size().height());
                    m_frameSet->requestMoreFrames(0);
                    return; // done!
                }
                if (KWord::isHeaderFooter(m_frameSet)) { // more text, lets resize the header/footer.
                    TDEBUG << "  header/footer is too small resize:" << line.line.height();
                    m_frameSet->requestMoreFrames(line.line.height());
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
            line.tryFit();
#ifdef DEBUG_TEXT
            if (line.line.isValid()) {
                QTextBlock b = document()->findBlock(m_state->cursorPosition());
                if (b.isValid()) {
                    TDEBUG << "fitted line" << b.text().mid(line.line.textStart(), line.line.textLength());
                    TDEBUG << "       2 @ " << line.line.position() << " from parag at pos " << b.position();
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

void KWTextDocumentLayout::cleanupAnchors()
{
    qDeleteAll(m_activeAnchors);
    m_activeAnchors.clear();
    qDeleteAll(m_newAnchors);
    m_newAnchors.clear();
}
