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

#include "KWTextDocumentLayout.h"
#include "KWTextFrameSet.h"
#include "KWTextFrame.h"

#include <KoTextShapeData.h>
#include <KoParagraphStyle.h>
#include <KoCharacterStyle.h>
#include <KoListStyle.h>
#include <KoStyleManager.h>
#include <KoTextBlockData.h>

#include <kdebug.h>
#include <QTextBlock>
#include <QTextFragment>
#include <QTextLayout>
#include <QTextLine>
#include <QTextList>
#include <QPainter>
#include <QPaintDevice>

#include <limits.h>

/// helper class to disable any screen convertion as thats done in flake.
class PostscriptPaintDevice : public QPaintDevice {
public:
    PostscriptPaintDevice() {
    }
//   int devType () const {
//   }
    QPaintEngine *paintEngine () const {
        return 0;
    }
    int metric (PaintDeviceMetric metric) const {
        switch(metric) {
            case QPaintDevice::PdmWidth:
            case QPaintDevice::PdmHeight:
            case QPaintDevice::PdmWidthMM:
            case QPaintDevice::PdmHeightMM:
            case QPaintDevice::PdmNumColors:
                return INT_MAX;
            case QPaintDevice::PdmDepth:
                return 32;
            case QPaintDevice::PdmDpiX:
            case QPaintDevice::PdmDpiY:
            case QPaintDevice::PdmPhysicalDpiX:
            case QPaintDevice::PdmPhysicalDpiY:
                return 72;
        }
        return 0; // should never be hit
    }
};

// ---------------- layout helper ----------------
class LayoutState {
public:
    LayoutState(KWTextFrameSet *fs, KoStyleManager *sm) : m_frameSet(fs), m_styleManager(sm) {
        layout = 0;
        m_reset = true;
    }

    /// start layouting, return false when there is nothing to do
    bool start() {
        if(m_reset)
            resetPrivate();
        else
            nextParag();
        m_reset = false;
        return !(layout == 0 || m_frameSet->frameCount() <= frameNumber);
    }

    /// end layouting
    void end() {
        if(layout)
            layout->endLayout();
        layout = 0;
    }

    void reset() {
        m_reset = true;
    }

    /// returns true if reset has been called.
    bool interrupted() {
        return m_reset;
    }

    void setStyleManager(KoStyleManager *sm) {
        m_styleManager = sm;
    }

    double width() {
        double ptWidth = shape->size().width() - m_format.leftMargin() - m_format.rightMargin();
        if(m_newParag)
            ptWidth -= m_format.textIndent();
        if(m_newParag && m_block.textList()) // is a listItem
            ptWidth -= listIndent();
        return ptWidth;
    }

    double x() {
        double result = (m_newParag?m_format.textIndent():0.0) + m_format.leftMargin();
        if(m_block.textList()) { // is a listItem
            double indent = listIndent();
            KoTextBlockData *data = dynamic_cast<KoTextBlockData*> (m_block.userData());
            Q_ASSERT(data);
            data->setCounterPosition(QPointF(result, y()));
            result += indent;
        }
        return result;
    }

    double y() {
        return m_y;
    }

    /// return the y offset of the document at start of shape.
    double docOffsetInShape() const {
        return m_data->documentOffset();
    }

    /// when a line is added, update internal vars.  Return true if line does not fit in shape
    bool addLine(const QTextLine &line) {
//kDebug() << "addLine" << endl;
        double height = m_format.doubleProperty(KoParagraphStyle::FixedLineHeight);
        bool useFixedLineHeight = height != 0.0;
        bool useFontProperties = m_format.boolProperty(KoParagraphStyle::LineSpacingFromFont);
        if(! useFixedLineHeight) {
            if(useFontProperties)
                height = line.height();
            else {
                if(m_fragmentIterator.atEnd()) // no text in parag.
                    height = m_block.charFormat().fontPointSize();
                else {
                    // read max font height
                    height = qMax(height,
                            m_fragmentIterator.fragment().charFormat().fontPointSize());
                    while(! (m_fragmentIterator.atEnd() || m_fragmentIterator.fragment().contains(
                                   m_block.position() + line.textStart() + line.textLength() -1))) {
                        m_fragmentIterator++;
                        height = qMax(height, m_fragmentIterator.fragment().charFormat().fontPointSize());
                    }
                }
                if(height < 0.01) height = 12; // default size for uninitialized styles.
            }
        }

        if(m_data->documentOffset() + shape->size().height() < m_y + height) {
//kDebug() << "   NEXT frame" << endl;
            // line does not fit.
            m_data->setEndPosition(m_block.position() + line.textStart()-1);
            nextFrame();
            if(m_data)
                m_data->setPosition(m_block.position() + line.textStart());
            return true;
        }

        // add linespacing
        if(! useFixedLineHeight) {
            double linespacing = m_format.doubleProperty(KoParagraphStyle::LineSpacing);;
            if(linespacing == 0.0) { // unset
                int percent = m_format.intProperty(KoParagraphStyle::FixedLineHeight);
                if(percent != 0)
                    linespacing = height * ((percent - 100) / 100.0);
                else if(linespacing == 0.0)
                    linespacing = height * 0.2; // default
            }
            height += linespacing;
        }

        double minimum = m_format.doubleProperty(KoParagraphStyle::MinimumLineHeight);
        if(minimum > 0.0)
            height = qMax(height, minimum);
        m_y += height;
        m_newShape = false;
        m_newParag = false;
        return false;
    }

    /// prepare for next paragraph; return false if there is no next parag.
    bool nextParag() {
        if(layout) { // guard against first time
            layout->endLayout();
            m_block = m_block.next();
            if(!m_newShape) {// this parag is first line on new shape
                m_y += m_format.bottomMargin();
                if(m_format.boolProperty(KoParagraphStyle::BreakAfter)) {
                    m_data->setEndPosition(m_block.position()-1);
                    nextFrame();
                    if(m_data)
                        m_data->setPosition(m_block.position());
                }
            }
        }
        layout = 0;
        if(! m_block.isValid()) {
            QTextBlock block = m_block.previous(); // last correct one.
            m_data->setEndPosition(block.position() + block.length());
            return false;
        }
        m_format = m_block.blockFormat();
        if(! m_newShape) { // ignore margins at top of shape
            m_y += m_format.topMargin();
            if(m_format.boolProperty(KoParagraphStyle::BreakBefore)) {
                m_data->setEndPosition(m_block.position()-1);
                nextFrame();
                if(m_data)
                    m_data->setPosition(m_block.position());
            }
        }
        layout = m_block.layout();
        QTextOption options = layout->textOption();
        options.setAlignment(m_format.alignment());
        layout->setTextOption(options);

        layout->beginLayout();
        m_fragmentIterator = m_block.begin();
        m_newParag = true;

//kDebug()<< "nextParag " << m_block.textList() << " for " << m_block.text() << endl;
        return true;
    }

    int frameNumber;
    KoShape *shape;
    QTextLayout *layout;

  private:
    /// move to next frame (aka shape)
    void nextFrame() {
        frameNumber++;
        m_newShape = true;
        if(frameNumber >= m_frameSet->frameCount()) {
            shape = 0;
            m_data = 0;
            return;
        }

        if(m_data) {
            Q_ASSERT(m_data->endPosition() >= m_data->position());
            m_y = m_data->documentOffset() + shape->size().height() + 10.0;
            m_data->wipe();
        }
        shape = m_frameSet->frames()[frameNumber]->shape();
        m_data = dynamic_cast<KoTextShapeData*> (shape->userData());
        m_data->setDocumentOffset(m_y);
        m_data->faul(); // make dirty since this one needs relayout at this point.
    }

    /**
     * return the list-indenting (in pt) of the current block.
     * This is also known as the space available for the counter of this list item.
     * Should only be called for blocks that _are_ a counter.
     */
    double listIndent() {
        Q_ASSERT(m_block.textList());
        QTextList *textList = m_block.textList();
        QTextListFormat format = textList->format();
        int styleId = format.intProperty(KoListStyle::CharacterStyleId);
        KoCharacterStyle *charStyle = 0;
        if(styleId > 0 && m_styleManager)
            charStyle = m_styleManager->characterStyle(styleId);
        if(!charStyle && m_styleManager) { // try the one from paragraph style
            KoParagraphStyle *ps = m_styleManager->paragraphStyle(
                    m_format.intProperty(KoParagraphStyle::StyleId));
            if(ps)
                charStyle = ps->characterStyle();
        }

        KoTextBlockData *data = dynamic_cast<KoTextBlockData*> (m_block.userData());
        if(! (data && data->hasCounterData())) {
            QFont font;
            if(charStyle)
                font = QFont(charStyle->fontFamily(), qRound(charStyle->fontPointSize()),
                        charStyle->fontWeight(), charStyle->fontItalic());
            else {
                QTextCursor cursor(m_block);
                font = cursor.charFormat().font();
            }
            ListItemsHelper lih(textList, font);
            lih.recalculate();
            data = dynamic_cast<KoTextBlockData*> (m_block.userData());
        }
        Q_ASSERT(data);
        return data->counterWidth();
    }

    void resetPrivate() {
        m_y = 0;
        m_data = 0;
        shape =0;
        layout = 0;
        m_newShape = true;
        m_block = m_frameSet->document()->begin();

        frameNumber = 0;
        int lastPos = -1;
        foreach(KWFrame *frame, m_frameSet->frames()) {
            KoShape *shape = frame->shape();
            KoTextShapeData *data = dynamic_cast<KoTextShapeData*> (shape->userData());
            Q_ASSERT(data);
            if(data->isDirty()) {
                // this shape needs to be recalculated.
                data->setPosition(lastPos+1);
                m_block = m_frameSet->document()->findBlock( lastPos+1 );
                m_y = data->documentOffset();

                if(m_block.layout() && m_block.layout()->lineCount() > 0) {
                    // block has been layouted. So use its offset.
                    m_y = m_block.layout()->lineAt(0).position().y();
                    if(m_y < data->documentOffset()) {
                        Q_ASSERT(frameNumber > 0);
                        // since we only recalc whole parags; we need to go back a little.
                        frameNumber--;
                        shape = m_frameSet->frames()[frameNumber]->shape();
                        data = dynamic_cast<KoTextShapeData*> (shape->userData());
                        m_newShape = false;
                    }
                }
                break;
            }
            lastPos = data->endPosition();
            frameNumber++;
        }
        Q_ASSERT(frameNumber >= 0);
        if(m_frameSet->frameCount() == 0)
            return;
        shape = m_frameSet->frames()[frameNumber]->shape();
        m_data = dynamic_cast<KoTextShapeData*> (shape->userData());

        if(! nextParag())
            frameNumber++;
    }

  private:
    KWTextFrameSet *m_frameSet;
    KoStyleManager *m_styleManager;

    double m_y;
    QTextBlock m_block;
    QTextBlockFormat m_format;
    QTextBlock::Iterator m_fragmentIterator;
    KoTextShapeData *m_data;
    bool m_newShape, m_newParag, m_reset;
};


KWTextDocumentLayout::KWTextDocumentLayout(KWTextFrameSet *frameSet)
    : QAbstractTextDocumentLayout(frameSet->document()),
    m_frameSet(frameSet),
    m_styleManager(0),
    m_state(0),
    m_lastKnownFrameCount(0)
{
    m_state = new LayoutState(m_frameSet, m_styleManager);
    setPaintDevice( new PostscriptPaintDevice() );
}

void KWTextDocumentLayout::setStyleManager(KoStyleManager *sm) {
    m_styleManager = sm;
    m_state->setStyleManager(sm);
}

KWTextDocumentLayout::~KWTextDocumentLayout() {
    m_styleManager = 0;
    m_frameSet = 0;
    delete m_state;
}

QRectF KWTextDocumentLayout::blockBoundingRect(const QTextBlock &block) const {
    // nobody calls this code and I have no way of implementing it anyway...
    Q_UNUSED(block);
    kWarning() << "KWTextDocumentLayout::blockBoundingRect is not implemented"<< endl;
    return QRectF(0, 0, 10, 10);
}

QSizeF KWTextDocumentLayout::documentSize() const {
    // nobody calls this code and I have no way of implementing it anyway...
    kWarning() << "KWTextDocumentLayout::documentSize is not implemented"<< endl;
    return QSizeF(10, 10);
}

void KWTextDocumentLayout::draw(QPainter *painter, const PaintContext &context) {
    Q_UNUSED(context);
    if(document()->begin().layout()->lineCount() == 0) // only first time
        layout();
    const QRegion clipRegion = painter->clipRegion();
    painter->setBrush(QBrush(Qt::black));
    painter->setPen(QPen(Qt::black));
    // da real work
    QTextBlock block = document()->begin();
    while(block.isValid()) {
        QTextLayout *layout = block.layout();
        if(!painter->hasClipping() || ! clipRegion.intersect(QRegion(layout->boundingRect().toRect())).isEmpty()) {
            KoTextBlockData *data = dynamic_cast<KoTextBlockData*> (block.userData());
            if(data && data->hasCounterData()) {
                QTextCursor cursor(block); // I know this is longwinded, but just using the blocks
                                           // charformat does not work, apparantly
                QFont font(cursor.charFormat().font(), paintDevice());

                QTextLayout layout(data->counterText(), font, paintDevice());
                layout.setCacheEnabled(true);
                QTextOption option(Qt::AlignLeft | Qt::AlignAbsolute);
                option.setTextDirection(block.blockFormat().layoutDirection());
                layout.setTextOption(option);
                layout.beginLayout();
                layout.createLine();
                layout.endLayout();
                QFontMetricsF fm(font);
                layout.draw(painter, data->counterPosition());
            }
            layout->draw(painter, QPointF(0,0));
        }
        block = block.next();
    }
}

QRectF KWTextDocumentLayout::frameBoundingRect(QTextFrame *frame) const {
    Q_UNUSED(frame);
    // nobody calls this code and I have no way of implementing it anyway...
    kWarning() << "KWTextDocumentLayout::frameBoundingRect is not implemented"<< endl;
    return QRectF(0, 0, 10, 10);
}

int KWTextDocumentLayout::hitTest(const QPointF &point, Qt::HitTestAccuracy accuracy) const {
    // kDebug() << "hitTest[" << point.x() << "," << point.y() << "]" << endl;
    QTextBlock block = document()->begin();
    int position = -1;
    while(block.isValid()) {
        QTextLayout *layout = block.layout();
        if(point.y() > layout->boundingRect().bottom()) {
            position = block.position() + block.length();
            block = block.next();
            continue;
        }
        for(int i=0; i < layout->lineCount(); i++) {
            QTextLine line = layout->lineAt(i);
            // kDebug() << " + line[" << line.textStart() << "]: " << line.y() << "-" << line.height() << endl;
            if(point.y() > line.y() + line.height()) {
                position = line.textStart() + line.textLength();
                continue;
            }
            if(accuracy == Qt::ExactHit && point.y() < line.y()) // between lines
                return -1;
            if(accuracy == Qt::ExactHit && // left or right of line
                    (point.x() < line.x() || point.x() > line.x() + line.width()))
                return -1;
            return block.position() + line.xToCursor(point.x());
        }
        block = block.next();
    }
    if(accuracy == Qt::ExactHit)
        return -1;
    return position;
}

int KWTextDocumentLayout::pageCount () const {
kDebug() << "KWTextDocumentLayout::pageCount"<< endl;
    return 1;
}

void KWTextDocumentLayout::documentChanged(int position, int charsRemoved, int charsAdded) {
    Q_UNUSED(charsAdded);
    Q_UNUSED(charsRemoved);
    if(m_frameSet->frameCount() == 0) // nothing to do.
        return;

    foreach(KWFrame *frame, m_frameSet->frames()) {
        if(frame->isCopy())
            continue;
        KoTextShapeData *data = dynamic_cast<KoTextShapeData*> (frame->shape()->userData());
        if(data->position() <= position && data->endPosition() >= position) {
            // found our (first) frame to re-layout
            data->faul();
            m_state->reset();
            m_frameSet->requestLayout();
            return;
        }
    }
    // if still here; then the change was not in any frame, lets relayout the last for now.
    KWFrame *last = m_frameSet->frames().last();
    KoTextShapeData *data = dynamic_cast<KoTextShapeData*> (last->shape()->userData());
    Q_ASSERT(data);
    data->faul();

    m_state->reset();
    m_frameSet->requestLayout();
}

void KWTextDocumentLayout::layout() {
//kDebug() << "KWTextDocumentLayout::layout" << endl;
    class End {
    public:
        End(LayoutState *state) { m_state = state; }
        ~End() { m_state->end(); }
    private:
        LayoutState *m_state;
    };
    End ender(m_state); // poor mans finally{}

    if(! m_state->start())
        return;
    double endPos = m_state->y() + 1000;
    bool newParagraph = true;
    while(m_state->shape) {
        QTextLine line = m_state->layout->createLine();
        if (!line.isValid()) { // end of parag
            double posY = m_state->y();
            bool moreText = m_state->nextParag();
            if(m_state->shape && m_state->y() > posY)
                m_state->shape->repaint(QRectF(0, posY,
                            m_state->shape->size().width(), m_state->y() - posY));

            if(! moreText) {
                const int frameCount = m_frameSet->frameCount();
                const int framesInUse = m_state->frameNumber+1;
                if(framesInUse < frameCount && framesInUse != m_lastKnownFrameCount)
                    m_frameSet->framesEmpty(framesInUse);
                m_lastKnownFrameCount = frameCount;
                m_moreFramesRequested = false;

                return; // done!
            }
            newParagraph = true;
            continue;
        }
        if(m_state->interrupted() || newParagraph && m_state->y() > endPos) {
            // enough for now. Try again later.
            m_frameSet->requestLayout();
            return;
        }
        newParagraph = false;

        line.setLineWidth(m_state->width());
        line.setPosition(QPointF(m_state->x(), m_state->y()));
        while(m_state->addLine(line)) {
            if(m_state->shape == 0) { // no more shapes to put the text in!
                line.setPosition(QPointF(0, m_state->y()+20));
                if(! m_moreFramesRequested) {
                    m_frameSet->requestMoreFrames();
                    m_moreFramesRequested = true;
                }
                return; // done!
            }
            line.setLineWidth(m_state->width());
            line.setPosition(QPointF(m_state->x(), m_state->y()));
        }

        QRectF repaintRect = line.rect();
        repaintRect.moveTop(repaintRect.y() - m_state->docOffsetInShape());
        repaintRect.setX(0.0); // just take full width since we can't force a repaint of
        repaintRect.setY(m_state->shape->size().width()); // where lines were before layout.
        m_state->shape->repaint(repaintRect);
    }
    // finished normally. Meaning that amount of frames is perfect for this text.
    m_moreFramesRequested = false;
    m_lastKnownFrameCount = m_frameSet->frameCount();
}


// ------------------- ListItemsHelper ------------
class ListItemsPrivate {
public:
    ListItemsPrivate(QTextList *tl, const QFont &font)
        : textList( tl ),
          fm( font, textList->document()->documentLayout()->paintDevice() ),
          displayFont(font) {
    }
    QTextList *textList;
    QFontMetricsF fm;
    QFont displayFont;


};

ListItemsHelper::ListItemsHelper(QTextList *textList, const QFont &font) {
    d = new ListItemsPrivate(textList, font);
}

ListItemsHelper::~ListItemsHelper() {
    delete d;
}

/// is meant to take a QTextList and set the indent plus the string to render on each listitem
void ListItemsHelper::recalculate() {
    //kDebug() << "ListItemsHelper::recalculate" << endl;
    double width = 0.0;
    QTextListFormat format = d->textList->format();

    // when true don't let non-numbering parags restart numbering
    bool consecutiveNumbering = format.boolProperty(KoListStyle::ConsecutiveNumbering);
    int index = format.intProperty(KoListStyle::StartValue);
    QString prefix = format.stringProperty( KoListStyle::ListItemPrefix );
    QString suffix = format.stringProperty( KoListStyle::ListItemSuffix );
    const int level = format.intProperty(KoListStyle::Level);
    int displayLevel = format.intProperty(KoListStyle::DisplayLevel);
    if(displayLevel > level || displayLevel == 0)
        displayLevel = level;
    for(int i=0; i < d->textList->count(); i++) {
        QTextBlock tb = d->textList->item(i);
        KoTextBlockData *data = dynamic_cast<KoTextBlockData*> (tb.userData());
        if(!data) {
            data = new KoTextBlockData();
            tb.setUserData(data);
        }
        int paragIndex = tb.blockFormat().intProperty( KoListStyle::ExplicitListValue);
        if(paragIndex > 0)
            index = paragIndex;

        if(! consecutiveNumbering) { // check if before this parag there was a lower-level list
// from ODF spec: text:consecutive-numbering
// The text:consecutive-numbering attribute specifies whether or not the list style uses consecutive numbering for all list levels or whether each list level restarts the numbering.
            QTextBlock b = tb.previous();
            while(b.isValid()) {
                if(b.textList() == d->textList)
                    break; // all fine
                if(b.textList() == 0 || b.textList()->format().intProperty(KoListStyle::Level)
                        < level) {
                    index = format.intProperty(KoListStyle::StartValue);
                    break;
                }
                b = b.previous();
            }
        }

        QString item("");
        if(displayLevel > 1) {
            int checkLevel = level;
            for(QTextBlock b = tb.previous(); displayLevel > 1 && b.isValid(); b=b.previous()) {
                if(b.textList() == 0)
                    continue;
                QTextListFormat lf = b.textList()->format();
                const int otherLevel  = lf.intProperty(KoListStyle::Level);
                if(checkLevel <= otherLevel)
                    continue;
              /*if(needsRecalc(b->textList())) {
                    TODO
                } */
                KoTextBlockData *otherData = dynamic_cast<KoTextBlockData*> (b.userData());
Q_ASSERT(otherData);
                if(displayLevel-1 < otherLevel) { // can't just copy it fully since we are
                                                  // displaying less then the full counter
                    item += otherData->partialCounterText();
                    displayLevel--;
                    checkLevel--;
                    for(int i=otherLevel+1;i < level; i++) {
                        displayLevel--;
                        item += ".0"; // add missing counters.
                    }
                }
                else { // just copy previous counter as prefix
                    item += otherData->counterText();
                    for(int i=otherLevel+1;i < level; i++)
                        item += ".0"; // add missing counters.
                    break;
                }
            }
        }
        KoListStyle::Style listStyle = static_cast<KoListStyle::Style> (
                d->textList->format().style() );
        if((listStyle == KoListStyle::DecimalItem || listStyle == KoListStyle::AlphaLowerItem ||
                    listStyle == KoListStyle::UpperAlphaItem ||
                    listStyle == KoListStyle::RomanLowerItem ||
                    listStyle == KoListStyle::UpperRomanItem) &&
                !(item.isEmpty() || item.endsWith('.') || item.endsWith(' '))) {
            item += '.';
        }
        switch( listStyle ) {
            case KoListStyle::DecimalItem: {
                QString i = QString::number(index);
                data->setPartialCounterText(i);
                item += i;
                width = qMax(width, d->fm.width(item));
                break;
            }
            case KoListStyle::AlphaLowerItem:
                // TODO;
                break;
            case KoListStyle::UpperAlphaItem:
                // TODO;
                break;
            case KoListStyle::RomanLowerItem:
                // TODO;
                break;
            case KoListStyle::UpperRomanItem:
                // TODO;
                break;
            case KoListStyle::SquareItem:
            case KoListStyle::DiscItem:
            case KoListStyle::CircleItem:
            case KoListStyle::BoxItem: {
                width = d->displayFont.pointSizeF();
                int percent = format.intProperty(KoListStyle::BulletSize);
                if(percent > 0)
                    width = width * (percent / 100.0);
                break;
            }
            case KoListStyle::CustomCharItem:
                width =  19.0; //  TODO
                break;
            case KoListStyle::NoItem:
                width =  10.0; // simple indenting
                break;
            default:; // others we ignore.
        }
        data->setCounterText(prefix + item + suffix);
        index++;
    }
    width += d->fm.width(prefix + suffix); // same for all
    for(int i=0; i < d->textList->count(); i++) {
        QTextBlock tb = d->textList->item(i);
        KoTextBlockData *data = dynamic_cast<KoTextBlockData*> (tb.userData());
        data->setCounterWidth(width);
        //kDebug() << data->counterText() << " " << tb.text() << endl;
        //kDebug() << "    setCounterWidth: " << width << endl;
    }
    //kDebug() << endl;
}

// static
bool ListItemsHelper::needsRecalc(QTextList *textList) {
    Q_ASSERT(textList);
    QTextBlock tb = textList->item(0);
    KoTextBlockData *data = dynamic_cast<KoTextBlockData*> (tb.userData());
    if(data == 0)
        return true;
    return !data->hasCounterData();
}
