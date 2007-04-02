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

#include "Layout.h"
#include "ListItemsHelper.h"
#include "TextShape.h"

#include <KoTextDocumentLayout.h>
#include <KoTextShapeData.h>
#include <KoParagraphStyle.h>
#include <KoCharacterStyle.h>
#include <KoListStyle.h>
#include <KoStyleManager.h>
#include <KoTextBlockData.h>
#include <KoTextBlockBorderData.h>
#include <KoShape.h>

#include <kdeversion.h>
#include <kdebug.h>
#include <QTextList>
#include <QStyle>

// ---------------- layout helper ----------------
Layout::Layout(KoTextDocumentLayout *parent)
   : m_styleManager(0),
    m_blockData(0),
    m_data(0),
    m_reset(true),
    m_isRtl(false),
    m_demoText(false),
    m_endOfDemoText(false)
{
    m_parent = parent;
    layout = 0;
}

bool Layout::start() {
    if(m_reset)
        resetPrivate();
    else if(shape)
        nextParag();
    m_reset = false;
    return !(layout == 0 || m_parent->shapes().count() <= shapeNumber);
}

void Layout::end() {
    if(layout)
        layout->endLayout();
    layout = 0;
}

void Layout::reset() {
    m_reset = true;
}

bool Layout::interrupted() {
    return m_reset;
}

double Layout::width() {
    Q_ASSERT(shape);
    double ptWidth = shape->size().width() - m_format.leftMargin() - m_format.rightMargin();
    if(m_newParag)
        ptWidth -= m_format.textIndent();
    if(m_newParag && m_blockData)
        ptWidth -= m_blockData->counterWidth() + m_blockData->counterSpacing();
    ptWidth -= m_borderInsets.left + m_borderInsets.right + m_shapeBorder.right;
    return ptWidth;
}

double Layout::x() {
    double result = m_newParag?m_format.textIndent():0.0;
    result += m_isRtl ? m_format.rightMargin() : m_format.leftMargin();
    result += listIndent();
    result += m_borderInsets.left + m_shapeBorder.left;
    return result;
}

double Layout::y() {
    return m_y;
}

double Layout::docOffsetInShape() const {
    Q_ASSERT(m_data);
    return m_data->documentOffset();
}

bool Layout::addLine(QTextLine &line) {
    double height = m_format.doubleProperty(KoParagraphStyle::FixedLineHeight);
    bool useFixedLineHeight = height != 0.0;
    if(! useFixedLineHeight) {
        const bool useFontProperties = m_format.boolProperty(KoParagraphStyle::LineSpacingFromFont);
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

    if(m_data->documentOffset() + shape->size().height() < m_y + height + m_shapeBorder.bottom) {
        // line does not fit.
        m_data->setEndPosition(m_block.position() + line.textStart()-1);
        nextShape();
        if(m_data)
            m_data->setPosition(m_block.position() + line.textStart());

        // the demo-text feature means we have exactly the same amount of text as we have frame-space
        if(m_demoText)
            m_endOfDemoText = true;
        return true;
    }

    // add linespacing
    if(! useFixedLineHeight) {
        double linespacing = m_format.doubleProperty(KoParagraphStyle::LineSpacing);;
        if(linespacing == 0.0) { // unset
            int percent = m_format.intProperty(KoParagraphStyle::PercentLineHeight);
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
    if(qAbs(m_y - line.y()) < 0.126) // rounding problems due to Qt-scribe internally using ints.
        m_y += height;
    else
        m_y = line.y() + height; // The line got a pos <> from y(), follow that lead.
    m_newShape = false;
    m_newParag = false;
    return false;
}

bool Layout::nextParag() {
    if(layout) { // guard against first time
        layout->endLayout();
        m_block = m_block.next();
        if(m_endOfDemoText) {
            layout = 0;
            m_blockData = 0;
            return false;
        }
        double borderBottom = m_y;
        if(!m_newShape) { // only add bottom of prev parag if we did not go to a new shape for this parag.
            if(m_format.pageBreakPolicy() == QTextFormat::PageBreak_AlwaysAfter ||
                    m_format.boolProperty(KoParagraphStyle::BreakAfter)) {
                m_data->setEndPosition(m_block.position()-1);
                nextShape();
                if(m_data)
                    m_data->setPosition(m_block.position());
            }
            m_y += m_borderInsets.bottom;
            borderBottom = m_y; // don't inlude the bottom margin!
            m_y += m_format.bottomMargin();
        }
        if(m_blockData && m_blockData->border())
            m_blockData->border()->setParagraphBottom(borderBottom);
    }
    layout = 0;
    m_blockData = 0;
    if(! m_block.isValid()) {
        QTextBlock block = m_block.previous(); // last correct one.
        m_data->setEndPosition(block.position() + block.length());

        // repaint till end of shape.
        const double offsetInShape = m_y - m_data->documentOffset();
        shape->repaint(QRectF(0.0, offsetInShape, shape->size().width(), shape->size().width() - offsetInShape));
        // cleanup and repaint rest of shapes.
        cleanupShapes();
        return false;
    }
    m_format = m_block.blockFormat();
    m_blockData = dynamic_cast<KoTextBlockData*> (m_block.userData());
    m_isRtl = m_block.text().isRightToLeft();

    // initialize list item stuff for this parag.
    QTextList *textList = m_block.textList();
    if(textList) {
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

        if(! (m_blockData && m_blockData->hasCounterData())) {
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
            m_blockData = dynamic_cast<KoTextBlockData*> (m_block.userData());
        }
    }

    updateBorders(); // fill the border inset member vars.
    m_y += m_borderInsets.top;

    if(!m_newShape && (m_format.pageBreakPolicy() == QTextFormat::PageBreak_AlwaysBefore ||
            m_format.boolProperty(KoParagraphStyle::BreakBefore))) {
        m_data->setEndPosition(m_block.position()-1);
        nextShape();
        if(m_data)
            m_data->setPosition(m_block.position());
    }
    m_y += topMargin();
    layout = m_block.layout();
    QTextOption options = layout->textOption();
#if QT_VERSION < KDE_MAKE_VERSION(4,3,0)
    options.setWrapMode(QTextOption::WrapAnywhere);
#else
    options.setWrapMode(QTextOption::WordWrap);
#endif
    options.setAlignment( QStyle::visualAlignment(m_isRtl ? Qt::RightToLeft : Qt::LeftToRight, m_format.alignment()) );
    if(m_isRtl)
        options.setTextDirection(Qt::RightToLeft);
    layout->setTextOption(options);

    layout->beginLayout();
    m_fragmentIterator = m_block.begin();
    m_newParag = true;

    if(textList) {
        // if list set list-indent. Do this after borders init to we can account for them.
        // Also after we account for indents etc so the y() pos is correct.
        if(m_isRtl)
            m_blockData->setCounterPosition(QPointF(shape->size().width() - m_borderInsets.right -
                m_shapeBorder.right - m_format.leftMargin() - m_blockData->counterWidth(), y()));
        else
            m_blockData->setCounterPosition(QPointF(m_borderInsets.left + m_shapeBorder.left +
                        m_format.textIndent() + m_format.leftMargin() , y()));
    }

    return true;
}

double Layout::documentOffsetInShape() {
    return m_data->documentOffset();
}

void Layout::nextShape() {
    m_newShape = true;

    if(m_data) {
        Q_ASSERT(m_data->endPosition() >= m_data->position());
        m_y = m_data->documentOffset() + shape->size().height() + 10.0;
        m_data->wipe();
    }

    shape = 0;
    m_data = 0;

    QList<KoShape *> shapes = m_parent->shapes();
    for(shapeNumber++; shapeNumber < shapes.count(); shapeNumber++) {
        shape = shapes[shapeNumber];
        m_data = dynamic_cast<KoTextShapeData*> (shape->userData());
        if(m_data != 0)
            break;
        shape = 0;
        m_data = 0;
    }

    if(shape == 0)
        return;
    m_data->setDocumentOffset(m_y);
    m_data->faul(); // make dirty since this one needs relayout at this point.
    m_shapeBorder = shape->borderInsets();
    m_y += m_shapeBorder.top;
}

// and the end of text, make sure the rest of the frames have something sane to show.
void Layout::cleanupShapes() {
    int i = shapeNumber + 1;
    QList<KoShape *> shapes = m_parent->shapes();
    while(i < shapes.count())
        cleanupShape(shapes[i++]);
}

void Layout::cleanupShape(KoShape *daShape) {
    KoTextShapeData *textData = dynamic_cast<KoTextShapeData*> (daShape->userData());
    if(textData == 0)
        return;
    if(textData->position() == -1)
        return;
    textData->setPosition(-1);
    textData->setDocumentOffset(m_y + 10);
    textData->wipe();
    daShape->repaint();
}

double Layout::listIndent() {
    if(m_blockData == 0)
        return 0;
    if(m_isRtl)
        return 0;
    return m_blockData->counterWidth();
}

void Layout::resetPrivate() {
    m_demoText = false;
    m_endOfDemoText = false;
    m_y = 0;
    m_data = 0;
    shape =0;
    layout = 0;
    m_newShape = true;
    m_blockData = 0;
    m_newParag = true;
    m_block = m_parent->document()->begin();

    shapeNumber = 0;
    int lastPos = -1;
    QList<KoShape *> shapes = m_parent->shapes();
    foreach(KoShape *shape, shapes) {
        KoTextShapeData *data = dynamic_cast<KoTextShapeData*> (shape->userData());
        Q_ASSERT(data);
        if(data->isDirty()) {
            // this shape needs to be recalculated.
            data->setPosition(lastPos+1);
            m_block = m_parent->document()->findBlock( lastPos+1 );
            m_y = data->documentOffset();
            m_format = m_block.blockFormat();

            if(shapeNumber == 0) {
                // no matter what the previous data says, just start from zero.
                m_y = 0;
                data->setDocumentOffset(0);
                Q_ASSERT(lastPos == -1);
                break;
            }
            if(m_block.layout() && m_block.layout()->lineCount() > 0) {
                // block has been layouted. So use its offset.
                m_y = m_block.layout()->lineAt(0).position().y();
                if(m_y < data->documentOffset() - 0.126) { // 0.126 to account of rounding in Qt-scribe
                    Q_ASSERT(shapeNumber > 0);
                    // since we only recalc whole parags; we need to go back a little.
                    shapeNumber--;
                    shape = shapes[shapeNumber];
                    data = dynamic_cast<KoTextShapeData*> (shape->userData());
                    m_newShape = false;
                }
                if(m_y > data->documentOffset() + shape->size().height()) {
                    // hang on; this line is explicitly placed outside the shape. Shape is empty!
                    m_y = data->documentOffset();
                    break;
                }
                // in case this parag has a border we have to subtract that as well
                m_blockData = dynamic_cast<KoTextBlockData*> (m_block.userData());
                if(m_blockData && m_blockData->border()) {
                    double top = m_blockData->border()->inset(KoTextBlockBorderData::Top);
                    // but only when this border actually makes us have an indent.
                    if(qAbs(m_blockData->border()->rect().top() + top - m_y) < 1E-10)
                        m_y -= top;
                }
                // subtract the top margins as well.
                m_y -= topMargin();
            }
            break;
        }
        lastPos = data->endPosition();
        shapeNumber++;
    }
    Q_ASSERT(shapeNumber >= 0);
    if(shapes.count() == 0)
        return;
    shape = shapes[shapeNumber];
    m_demoText = (static_cast<TextShape*> (shape))->demoText();
    m_data = dynamic_cast<KoTextShapeData*> (shape->userData());
    m_shapeBorder = shape->borderInsets();
    if(m_y == 0)
        m_y = m_shapeBorder.top;

   if(! nextParag())
       shapeNumber++;
}

void Layout::updateBorders() {
    m_borderInsets = m_data->shapeMargins();
    m_borderInsets.top += m_format.doubleProperty(KoParagraphStyle::TopPadding);
    m_borderInsets.left += m_format.doubleProperty(KoParagraphStyle::LeftPadding);
    m_borderInsets.bottom += m_format.doubleProperty(KoParagraphStyle::BottomPadding);
    m_borderInsets.right += m_format.doubleProperty(KoParagraphStyle::RightPadding);

    KoTextBlockBorderData border(QRectF(this->x() - listIndent(), m_y + m_borderInsets.top + topMargin(), width(), 1.));
    border.setEdge(border.Left, m_format, KoParagraphStyle::LeftBorderStyle,
        KoParagraphStyle::LeftBorderWidth, KoParagraphStyle::LeftBorderColor,
        KoParagraphStyle::LeftBorderSpacing, KoParagraphStyle::LeftInnerBorderWidth);
    border.setEdge(border.Right, m_format, KoParagraphStyle::RightBorderStyle,
        KoParagraphStyle::RightBorderWidth, KoParagraphStyle::RightBorderColor,
        KoParagraphStyle::RightBorderSpacing, KoParagraphStyle::RightInnerBorderWidth);
    border.setEdge(border.Top, m_format, KoParagraphStyle::TopBorderStyle,
        KoParagraphStyle::TopBorderWidth, KoParagraphStyle::TopBorderColor,
        KoParagraphStyle::TopBorderSpacing, KoParagraphStyle::TopInnerBorderWidth);
    border.setEdge(border.Bottom, m_format, KoParagraphStyle::BottomBorderStyle,
        KoParagraphStyle::BottomBorderWidth, KoParagraphStyle::BottomBorderColor,
        KoParagraphStyle::BottomBorderSpacing, KoParagraphStyle::BottomInnerBorderWidth);

    // check if prev parag had a border.
    QTextBlock prev = m_block.previous();
    KoTextBlockBorderData *prevBorder = 0;
    if(prev.isValid()) {
        KoTextBlockData *bd = dynamic_cast<KoTextBlockData*> (prev.userData());
        if(bd)
            prevBorder = bd->border();
    }
    if(border.hasBorders()) {
        if(m_blockData == 0) {
            m_blockData = new KoTextBlockData();
            m_block.setUserData(m_blockData);
        }

        // then check if we can merge with the previous parags border.
        if(prevBorder && prevBorder->equals(border))
            m_blockData->setBorder(prevBorder);
        else {
            // can't merge; then these are our new borders.
            KoTextBlockBorderData *newBorder = new KoTextBlockBorderData(border);
            m_blockData->setBorder(newBorder);
            if(prevBorder && !m_newShape)
                m_y += prevBorder->inset(KoTextBlockBorderData::Bottom);
        }
        m_blockData->border()->applyInsets(m_borderInsets, m_y + m_borderInsets.top, false);
    }
    else { // this parag has no border.
        if(prevBorder && !m_newShape)
            m_y += prevBorder->inset(KoTextBlockBorderData::Bottom);
        if(m_blockData)
            m_blockData->setBorder(0); // remove an old one, if there was one.
    }
}

double Layout::topMargin() {
    bool allowMargin = true; // wheather to allow margins at top of shape
    if(m_newShape) {
        allowMargin = false; // false by default, but check 2 exceptions.
        if(m_format.boolProperty(KoParagraphStyle::BreakBefore))
            allowMargin = true;
        else if( m_styleManager && m_format.topMargin() > 0) {
            // also allow it when the paragraph has the margin, but the style has a different one.
            KoParagraphStyle *ps = m_styleManager->paragraphStyle(
                    m_format.intProperty(KoParagraphStyle::StyleId));
            if(ps == 0 || ps->topMargin() != m_format.topMargin())
                allowMargin = true;
        }
    }
    if(allowMargin)
        return m_format.topMargin();
    return 0.0;
}

void Layout::draw(QPainter *painter, const QAbstractTextDocumentLayout::PaintContext &context) {
    painter->setPen(context.palette.color(QPalette::Text)); // for text that has no color.
    const QRegion clipRegion = painter->clipRegion();
    QTextBlock block = m_parent->document()->begin();
    bool started=false;
    while(block.isValid()) {
        QTextLayout *layout = block.layout();

        // the following line is simpler, but due to a Qt bug doesn't work. Try to see if enabling this for Qt4.3
        // will not paint all paragraphs.
        //if(!painter->hasClipping() || ! clipRegion.intersect(QRegion(layout->boundingRect().toRect())).isEmpty()) {
        if(layout->lineCount() >= 1) {
            QTextLine first = layout->lineAt(0);
            QTextLine last = layout->lineAt(layout->lineCount()-1);
            QRectF parag(qMin(first.x(), last.x()), first.y(), qMax(first.width(), last.width()), last.y() + last.height());
            KoTextBlockData *blockData = dynamic_cast<KoTextBlockData*> (block.userData());
            if(blockData) {
                KoTextBlockBorderData *border = blockData->border();
                if(blockData->hasCounterData()) {
                    if(layout->textOption().textDirection() == Qt::RightToLeft)
                        parag.setRight(parag.right() + blockData->counterWidth() + blockData->counterSpacing());
                    else
                        parag.setLeft(blockData->counterPosition().x());
                }
                if(border) {
                    KoInsets insets;
                    border->applyInsets(insets, parag.top(), true);
                    parag.adjust(-insets.left, -insets.top, insets.right, insets.bottom);
                }
            }
            if(!painter->hasClipping() || ! clipRegion.intersect(QRegion(parag.toRect())).isEmpty()) {
                started=true;
                painter->save();
                decorateParagraph(painter, block);
                painter->restore();

                QVector<QTextLayout::FormatRange> selections;
                foreach(QAbstractTextDocumentLayout::Selection selection, context.selections) {
                    QTextCursor cursor = selection.cursor;
                    int begin = cursor.position();
                    int end = cursor.anchor();
                    if(begin > end)
                        qSwap(begin, end);

                   if(end < block.position() || begin > block.position() + block.length())
                       continue; // selection does not intersect this block.
                    QTextLayout::FormatRange fr;
                    fr.start = begin - block.position();
                    fr.length = end - begin;
                    fr.format = selection.format;
                    selections.append(fr);
                }
                layout->draw(painter, QPointF(0,0), selections);
            }
            else if(started) // when out of the cliprect, then we are done drawing.
                return;
        }
        block = block.next();
    }
}

void Layout::decorateParagraph(QPainter *painter, const QTextBlock &block) {
    KoTextBlockData *data = dynamic_cast<KoTextBlockData*> (block.userData());
    if(data == 0)
        return;

    QTextList *list = block.textList();
    if(list && data->hasCounterData()) {
        QTextListFormat listFormat = list->format();
        QTextCharFormat cf;
        bool filled=false;
        if(m_styleManager) {
            const int id = listFormat.intProperty(KoListStyle::CharacterStyleId);
            KoCharacterStyle *cs = m_styleManager->characterStyle(id);
            if(cs) {
                cs->applyStyle(cf);
                filled = true;
            }
        }
        if(! filled) {
            // use first char of block.
            QTextCursor cursor(block); // I know this is longwinded, but just using the blocks
            // charformat does not work, apparantly
            cf = cursor.charFormat();
        }
        if(! data->counterText().isEmpty()) {
            QFont font(cf.font(), m_parent->paintDevice());
            QTextLayout layout(data->counterText(), font, m_parent->paintDevice());
            layout.setCacheEnabled(true);
            QList<QTextLayout::FormatRange> layouts;
            QTextLayout::FormatRange format;
            format.start=0;
            format.length=data->counterText().length();
            format.format = cf;
            layouts.append(format);
            layout.setAdditionalFormats(layouts);

            Qt::Alignment align = static_cast<Qt::Alignment> (listFormat.intProperty(KoListStyle::Alignment));
            if(align == 0)
                align = Qt::AlignLeft;
            else if(align != Qt::AlignAuto)
                align |= Qt::AlignAbsolute;
            QTextOption option( align );
            option.setTextDirection(block.layout()->textOption().textDirection());
            if(option.textDirection() == Qt::RightToLeft || data->counterText().isRightToLeft())
                option.setAlignment(Qt::AlignRight);
            layout.setTextOption(option);
            layout.beginLayout();
            QTextLine line = layout.createLine();
            line.setLineWidth(data->counterWidth() - data->counterSpacing());
            layout.endLayout();
            layout.draw(painter, data->counterPosition());
        }

        KoListStyle::Style listStyle = static_cast<KoListStyle::Style> ( listFormat.style() );
        if(listStyle == KoListStyle::SquareItem || listStyle == KoListStyle::DiscItem ||
                listStyle == KoListStyle::CircleItem || listStyle == KoListStyle::BoxItem) {
            QFontMetricsF fm(cf.font(), m_parent->paintDevice());
#if 0
// helper lines to show the anatomy of this font.
painter->setPen(Qt::green);
painter->drawLine(QLineF(-1, data->counterPosition().y(), 200, data->counterPosition().y()));
painter->setPen(Qt::yellow);
painter->drawLine(QLineF(-1, data->counterPosition().y() + fm.ascent() - fm.xHeight(), 200, data->counterPosition().y() + fm.ascent() - fm.xHeight()));
painter->setPen(Qt::blue);
painter->drawLine(QLineF(-1, data->counterPosition().y() + fm.ascent(), 200, data->counterPosition().y() + fm.ascent()));
painter->setPen(Qt::gray);
painter->drawLine(QLineF(-1, data->counterPosition().y() + fm.height(), 200, data->counterPosition().y() + fm.height()));
#endif

            double width = fm.xHeight();
            double y = data->counterPosition().y() + fm.ascent() - fm.xHeight(); // at top of text.
            int percent = listFormat.intProperty(KoListStyle::BulletSize);
            if(percent > 0)
                width *= percent / 100.0;
            y -= width / 10.; // move it up just slightly
            double x = qMax(1., data->counterPosition().x() + fm.width(listFormat.stringProperty( KoListStyle::ListItemPrefix )));
            switch( listStyle ) {
                case KoListStyle::SquareItem:
                    painter->fillRect(QRectF(x, y, width, width), QBrush(Qt::black));
                    break;
                case KoListStyle::DiscItem:
                    painter->setBrush(QBrush(Qt::black));
                    // fall through!
                case KoListStyle::CircleItem:
                    painter->drawEllipse(QRectF(x, y, width, width));
                    break;
                case KoListStyle::BoxItem:
                    painter->drawRect(QRectF(x, y, width, width));
                    break;
                default:; // others we ignore.
            }
        }
    }

    KoTextBlockBorderData *border = dynamic_cast<KoTextBlockBorderData*> (data->border());
    // TODO make sure we only paint a border-set one time.
    if(border) {
        painter->save();
        border->paint(*painter);
        painter->restore();
    }
}

bool Layout::setFollowupShape(KoShape *followupShape) {
    if(m_demoText)
        return false;
    Q_ASSERT(shape == 0);
    Q_ASSERT(followupShape);

    m_data = dynamic_cast<KoTextShapeData*> (followupShape->userData());
    if(m_data == 0)
        return false;

    m_newShape = false;
    shape = followupShape;
    m_data->setDocumentOffset(m_y);
    m_shapeBorder = shape->borderInsets();
    return true;
}

void Layout::clearTillEnd() {
    QTextBlock block = m_block.next();
    while(block.isValid()) {
        if(block.layout()->lineCount() == 0)
            return;
        // erase the layouted lines
        block.layout()->beginLayout();
        block.layout()->endLayout();
        block = block.next();
    }
}
