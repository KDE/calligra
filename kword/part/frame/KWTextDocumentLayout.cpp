/* This file is part of the KDE project
 * Copyright (C) 2006-2007 Thomas Zander <zander@kde.org>
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
#include "KWDocument.h"
#include "KWPage.h"

#include <KoTextShapeData.h>

#include <QList>
#include <QPainterPath>

// helper methods
static double xAtY(const QLineF &line, double y) {
    if(line.dx() == 0)
        return line.x1();
    return line.x1() +  (y - line.y1()) / line.dy() * line.dx();
}

static double yAtX(const QLineF &line, double x) {
    if(line.dy() == 0)
        return line.y1();
    return line.y1() +  (x - line.x1()) / line.dx() * line.dy();
}

/// Returns 0, one or two points where the line intersects with the rectangle.
static QList<QPointF> intersect(const QRectF &rect, const QLineF &line) {
    QList<QPointF> answer;
    QPointF startOfLine = line.p1();
    QPointF endOfLine = line.p2();
    // top edge
    if(startOfLine.y() <= rect.top() && endOfLine.y() >= rect.top() ||
            startOfLine.y() >= rect.top() && endOfLine.y() <= rect.top()) {
        double x = xAtY(line, rect.top());
        if(x >= rect.left() && x <= rect.right() && x)
            answer.append(QPointF(x, rect.top()));
    }

    // left
    if(startOfLine.x() <= rect.left() && endOfLine.x() >= rect.left() ||
            startOfLine.x() >= rect.left() && endOfLine.x() <= rect.left()) {
        double y = yAtX(line, rect.left());
        if(y >= rect.top() && y <= rect.bottom())
            answer.append(QPointF(rect.left(), y));
    }

    // bottom edge
    if(startOfLine.y() <= rect.bottom() && endOfLine.y() >= rect.bottom() ||
            startOfLine.y() >= rect.bottom() && endOfLine.y() <= rect.bottom()) {
        double x = xAtY(line, rect.bottom());
        if(x >= rect.left() && x <= rect.right())
            answer.append(QPointF(x, rect.bottom()));
    }

    // right
    if(startOfLine.x() <= rect.right() && endOfLine.x() >= rect.right() ||
            startOfLine.x() >= rect.right() && endOfLine.x() <= rect.right()) {
        double y = yAtX(line, rect.right());
        if(y >= rect.top() && y <= rect.bottom())
            answer.append(QPointF(rect.right(), y));
    }

    return answer;
}
// TODO  can we replace the above?
// Qt 4.3 has something like the above already.
// QPolygonF is a QVector<QPointF>, it has a constructor that takes a QRectF and it has a QPolygonF intersected(const QPolygonF &) method

// ----------------- Class that allows us with the runaround of QPainterPaths ----------------
class Outline {
public:
    Outline(KWFrame *frame, const QMatrix &matrix) : m_side(None), m_frame(frame) {
        QPainterPath path =  matrix.map(frame->shape()->outline());
        m_bounds = path.boundingRect();
        if(frame->runAroundDistance() >= 0.0) {
            QMatrix grow = matrix;
            grow.translate(m_bounds.width() / 2.0, m_bounds.height() / 2.0);
            const double scaleX = (m_bounds.width() + frame->runAroundDistance()) / m_bounds.width();
            const double scaleY = (m_bounds.height() + frame->runAroundDistance()) / m_bounds.height();
            grow.scale(scaleX, scaleY);
            grow.translate(-m_bounds.width() / 2.0, -m_bounds.height() / 2.0);

            path =  grow.map(frame->shape()->outline());
            // kDebug() << "Grow " << frame->runAroundDistance() << ", Before: " << m_bounds << ", after: " << path.boundingRect() << endl;
            m_bounds = path.boundingRect();
        }

        QPolygonF poly = path.toFillPolygon();

        QPointF prev = *(poly.begin());
        foreach(QPointF vtx, poly) { //initialized edges
            if(vtx.x() == prev.x() && vtx.y() == prev.y())
               continue;
            QLineF line;
            if(prev.y() < vtx.y()) // Make sure the vector lines all point downwards.
                line = QLineF(prev, vtx);
            else
                line = QLineF(vtx, prev);
            m_edges.insert(line.y1(), line);
            prev = vtx;
        }

        if(m_frame->runAroundSide() == KWord::LeftRunAroundSide)
            m_side = Right;
        else if(m_frame->runAroundSide() == KWord::RightRunAroundSide)
            m_side = Left;
    }

    QRectF limit(const QRectF &content) {
        if(m_side == None) { // first time for this text;
            double insetLeft = m_bounds.right() - content.left();
            double insetRight = content.right() - m_bounds.left();

            if(insetLeft < insetRight)
                m_side = Left;
            else
                m_side = Right;
        }
        if(!m_bounds.intersects(content))
            return content;
        if(m_frame->textRunAround() == KWord::NoRunAround)
            return QRectF(); // empty

        // two points, as we are checking a rect, not a line.
        double points[2] = { content.top(), content.bottom() };
        QRectF answer = content;
        for(int i=0; i < 2; i++) {
            const double y = points[i];
            double x = m_side==Left?answer.left():answer.right();
            bool first = true;
            QMap<double, QLineF>::const_iterator iter = m_edges.constBegin();
            for(;iter != m_edges.constEnd(); ++iter) {
                QLineF line = iter.value();
                if(line.y2() < y) // not a section that will intersect with ou Y yet
                    continue;
                if(line.y1() > y) // section is below our Y, so abort loop
                    break;
                if(qAbs(line.dy()) < 1E-10 ) // horizontal lines don't concern us.
                    continue;

                double intersect = xAtY(iter.value(), y);
                if(first) {
                    x = intersect;
                    first = false;
                }
                else if(m_side == Left && intersect > x || m_side == Right && intersect < x)
                    x = intersect;
            }
            if(m_side == Left)
                answer.setLeft( qMax(answer.left(), x));
            else
                answer.setRight( qMin(answer.right(), x));
        }
        return answer;
    }

    KWFrame *frame() { return m_frame; }

private:
    enum Side { None, Left, Right };
    Side m_side;
    QMultiMap<double, QLineF> m_edges; //sorted with y-coord
    QRectF m_bounds;
    KWFrame *m_frame;
};

class KWTextDocumentLayout::DummyShape : public KoShape {
public:
    DummyShape(QTextDocument *doc) : textShapeData(new KoTextShapeData())
    {
        textShapeData->setDocument(doc, false);
        setUserData(textShapeData);
        //setPosition(QPointF(10E6, 10E6));
    }

    KoTextShapeData * const textShapeData; // will be deleted by KoShape

private:
    void paint(QPainter&, const KoViewConverter&) {}
};

KWTextDocumentLayout::KWTextDocumentLayout(KWTextFrameSet *frameSet)
    : KoTextDocumentLayout(frameSet->document()),
    m_frameSet(frameSet),
    m_dummyShape(new DummyShape(frameSet->document())),
    m_lastKnownFrameCount(0)
{
    if(m_frameSet->frameCount()) {
        KoTextShapeData *data = dynamic_cast<KoTextShapeData*> (m_frameSet->frames().first()->shape()->userData());
        if(data) { // reset layout.
            data->setEndPosition(-1);
            data->faul();
        }
    }
}

KWTextDocumentLayout::~KWTextDocumentLayout() {
    m_frameSet = 0;
    delete m_dummyShape;
}

QList<KoShape*> KWTextDocumentLayout::shapes() const {
    QList<KoShape*> answer;
    foreach(KWFrame *frame, m_frameSet->frames()) {
        if(frame->isCopy())
            continue;
        answer.append(frame->shape());
    }
    return answer;
}

void KWTextDocumentLayout::scheduleLayout() {
    m_frameSet->scheduleLayout();
}

void KWTextDocumentLayout::layout() {
//kDebug() << "KWTextDocumentLayout::layout" << endl;
    QList<Outline*> outlines;
    class End {
    public:
        End(KoTextDocumentLayout::LayoutState *state, QList<Outline*> *outlines) { m_state = state; m_outlines = outlines; }
        ~End() { m_state->end(); qDeleteAll(*m_outlines); }
    private:
        KoTextDocumentLayout::LayoutState *m_state;
        QList<Outline*> *m_outlines;
    };
    End ender(m_state, &outlines); // poor mans finally{}

    if(! m_state->start())
        return;
    double endPos = m_state->y() + 1000;
    double bottomOfText = 0.0;
    bool newParagraph = true;
    bool requestFrameResize = false;
    KoShape *currentShape = 0;
    while(m_state->shape) {
        class Line {
          public:
            Line(KoTextDocumentLayout::LayoutState *state) : m_state(state) {
                line = m_state->layout->createLine();
            }
            bool isValid() const {
                return line.isValid();
            }
            void tryFit() {
                QRectF rect(m_state->x(), m_state->y(), m_state->width(), 1.);
                rect = limit(rect);

                while(true) {
                    line.setLineWidth(rect.width());
                    rect.setHeight(line.height());
                    QRectF newLine = limit(rect);
                    if(newLine.width() <= 0.)
                        // TODO be more intelligent then just moving down 10 pt
                        rect = QRectF(m_state->x(), rect.top() + 10, m_state->width(), rect.height());
                    else if(qAbs(newLine.left() - rect.left()) < 1E-10 && qAbs(newLine.right() - rect.right()) < 1E-10)
                        break;
                    else
                        rect = newLine;
                }
                line.setPosition(QPointF(rect.x(), rect.y()));
            }
            void setOutlines(const QList<Outline*> &outlines) { m_outlines = &outlines; }

            QTextLine line;
          private:
            QRectF limit(const QRectF &rect) {
                QRectF answer = rect;
                foreach(Outline *outline, *m_outlines)
                    answer = outline->limit(answer);
                return answer;
            }
            KoTextDocumentLayout::LayoutState *m_state;
            const QList<Outline*> *m_outlines;
        };

        if(m_state->shape != currentShape) { // next shape; refresh the outlines cache.
            currentShape = m_state->shape;
            if(m_frameSet->kwordDocument()) {
                qDeleteAll(outlines);
                outlines.clear();
                QRectF bounds = m_state->shape->boundingRect();
                foreach(KWFrameSet *fs, m_frameSet->kwordDocument()->frameSets()) {
                    KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*> (fs);
                    if(tfs && tfs->textFrameSetType() == KWord::MainTextFrameSet)
                        continue;
                    foreach(KWFrame *frame, fs->frames()) {
                        if(frame->shape() == currentShape)
                            continue;
                        if(frame->textRunAround() == KWord::RunThrough)
                            continue;
                        if(frame->shape()->zIndex() < currentShape->zIndex())
                            continue;
                        if(! bounds.intersects( frame->shape()->boundingRect()))
                            continue;
                        QMatrix matrix = frame->shape()->transformationMatrix(0);
                        matrix = matrix * currentShape->transformationMatrix(0).inverted();
                        matrix.translate(0, m_state->documentOffsetInShape());
                        outlines.append(new Outline(frame, matrix));
                    }
                }
            }
        }

        Line line(m_state);
        if (!line.isValid()) { // end of parag
            double posY = m_state->y();
            bool moreText = m_state->nextParag();
            if(m_state->shape && m_state->y() > posY)
                m_state->shape->repaint(QRectF(0, posY,
                            m_state->shape->size().width(), m_state->y() - posY));

            if(! moreText) {
                const int frameCount = m_frameSet->frameCount();
                const int framesInUse = m_state->shapeNumber+1;
                if(framesInUse < frameCount && framesInUse != m_lastKnownFrameCount)
                    m_frameSet->framesEmpty(frameCount - framesInUse);
                m_lastKnownFrameCount = frameCount;
                if(requestFrameResize) // text ran out while placing it in the dummy shape.
                    m_frameSet->requestMoreFrames(m_state->y() - m_dummyShape->textShapeData->documentOffset());
                else {
                    // if there is more space in the shape then there is text. Reset the no-grow bool.
                    KWTextFrame *lastFrame = static_cast<KWTextFrame*> (m_frameSet->frames().last());
                    KoTextShapeData *data = static_cast<KoTextShapeData*> (lastFrame->shape()->userData());
                    double spaceLeft = lastFrame->shape()->size().height() - bottomOfText + data->documentOffset();
                    if(spaceLeft > 0)
                        m_frameSet->spaceLeft(spaceLeft);
                    lastFrame->allowToGrow();
                    data->wipe();
                }

                return; // done!
            }
            newParagraph = true;
            continue;
        }
        if(m_state->interrupted() || newParagraph && m_state->y() > endPos) {
            // enough for now. Try again later.
            m_frameSet->scheduleLayout();
            return;
        }
        newParagraph = false;
        line.setOutlines(outlines);
        line.tryFit();
        bottomOfText = line.line.y() + line.line.height();

        while(m_state->addLine(line.line)) {
            if(m_state->shape == 0) { // no more shapes to put the text in!
                line.line.setPosition(QPointF(0, m_state->y()+20));

                if(requestFrameResize) { // plenty more text, but first lets resize the shape.
                    m_frameSet->requestMoreFrames(m_dummyShape->size().height());
                    m_frameSet->requestMoreFrames(0);
                    return; // done!
                }

                KWFrame *lastFrame = m_frameSet->frames().last();
                if(lastFrame->frameBehavior() == KWord::IgnoreContentFrameBehavior)
                    return; // done!

                // find out the maximum size this frame can be extended to while still
                // fitting in the page.  We'll continue doing layout and see if there is text till end of page.
                KWPage *page = m_frameSet->pageManager()->page(lastFrame->shape());
                QRectF pageRect = page->rect();
                pageRect.adjust(page->leftMargin(), page->topMargin(), -page->rightMargin(), -page->bottomMargin());

                QLineF top(QPointF(0, 0), QPointF(lastFrame->shape()->size().width(), 0));
                top = lastFrame->shape()->transformationMatrix(0).map(top);
                const double multiplier = qMax(pageRect.height(), pageRect.width()) / top.length();
                QLineF down(top.p1(), QPointF( top.p1().x() - top.dy() * multiplier,
                            top.p1().y() + top.dx() * multiplier));
                QLineF down2(top.p2(), QPointF( top.p2().x() - top.dy() * multiplier,
                            top.p2().y() + top.dx() * multiplier));

                QList<QPointF> list = intersect(pageRect, down);
                if(list.count() > 0)
                    down = QLineF(down.p1(), list.last());
                list = intersect(pageRect, down2);
                if(list.count() > 0)
                    down2 = QLineF(down2.p1(), list.last());
                const double maxFrameLength = qMin(down.length(), down2.length());
                if(maxFrameLength <= currentShape->size().height()) {
                    m_frameSet->requestMoreFrames(0); // new page, please.
                    return;
                }
                KoTextShapeData *data = dynamic_cast<KoTextShapeData*> (lastFrame->shape()->userData());
                Q_ASSERT(data);

                m_dummyShape->resize(QSizeF(currentShape->size().width(), maxFrameLength - currentShape->size().height()));
                m_dummyShape->textShapeData->setShapeMargins(data->shapeMargins());
                if(! m_state->setFollowupShape(m_dummyShape))
                    return;
                requestFrameResize = true;
            }
            line.tryFit();
        }

        QRectF repaintRect = line.line.rect();
        repaintRect.moveTop(repaintRect.y() - m_state->docOffsetInShape());
        repaintRect.setX(0.0); // just take full width since we can't force a repaint of
        repaintRect.setWidth(m_state->shape->size().width()); // where lines were before layout.
        m_state->shape->repaint(repaintRect);
    }
    if(requestFrameResize)
        m_frameSet->requestMoreFrames(m_dummyShape->size().height());
}
