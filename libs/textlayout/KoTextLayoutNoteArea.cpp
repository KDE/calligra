/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Brijesh Patel <brijesh3105@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoTextLayoutNoteArea.h"

#include "FrameIterator.h"
#include "KoParagraphStyle.h"
#include "KoPointedAt.h"
#include "KoStyleManager.h"
#include "KoTextLayoutObstruction.h"
#include <KoInlineNote.h>
#include <KoOdfNumberDefinition.h>
#include <KoTextDocument.h>

#include <QPainter>

#define OVERLAPPREVENTION 1000

class Q_DECL_HIDDEN KoTextLayoutNoteArea::Private
{
public:
    Private() = default;
    KoInlineNote *note;
    QTextLayout *textLayout;
    QTextLayout *postLayout;
    qreal labelIndent;
    bool isContinuedArea;
    qreal labelWidth;
    qreal labelHeight;
    qreal labelYOffset;
};

KoTextLayoutNoteArea::KoTextLayoutNoteArea(KoInlineNote *note, KoTextLayoutArea *parent, KoTextDocumentLayout *documentLayout)
    : KoTextLayoutArea(parent, documentLayout)
    , d(new Private)
{
    Q_ASSERT(note);
    Q_ASSERT(parent);

    d->note = note;
    d->isContinuedArea = false;
    d->postLayout = nullptr;
}

KoTextLayoutNoteArea::~KoTextLayoutNoteArea()
{
    delete d;
}

void KoTextLayoutNoteArea::paint(QPainter *painter, const KoTextDocumentLayout::PaintContext &context)
{
    painter->save();
    if (d->isContinuedArea) {
        painter->translate(0, -OVERLAPPREVENTION);
    }

    KoTextLayoutArea::paint(painter, context);
    if (d->postLayout) {
        d->postLayout->draw(painter, QPointF(left() + d->labelIndent, top() + d->labelYOffset));
    }
    d->textLayout->draw(painter, QPointF(left() + d->labelIndent, top() + d->labelYOffset));
    painter->restore();
}

bool KoTextLayoutNoteArea::layout(FrameIterator *cursor)
{
    KoOdfNotesConfiguration *notesConfig = nullptr;
    if (d->note->type() == KoInlineNote::Footnote) {
        notesConfig = KoTextDocument(d->note->textFrame()->document()).styleManager()->notesConfiguration(KoOdfNotesConfiguration::Footnote);
    } else if (d->note->type() == KoInlineNote::Endnote) {
        notesConfig = KoTextDocument(d->note->textFrame()->document()).styleManager()->notesConfiguration(KoOdfNotesConfiguration::Endnote);
    }

    QString label;
    if (d->isContinuedArea) {
        if (!notesConfig->footnoteContinuationBackward().isEmpty()) {
            label = notesConfig->footnoteContinuationBackward() + " " + d->note->label();
        }
        setReferenceRect(left(), right(), top() + OVERLAPPREVENTION, maximumAllowedBottom() + OVERLAPPREVENTION);
    } else {
        label = d->note->label();
    }
    label.prepend(notesConfig->numberFormat().prefix());
    label.append(notesConfig->numberFormat().suffix());
    QPaintDevice *pd = documentLayout()->paintDevice();
    QTextBlock block = cursor->it.currentBlock();
    QTextCharFormat format = block.charFormat();
    KoCharacterStyle *style = static_cast<KoCharacterStyle *>(notesConfig->citationTextStyle());
    if (style) {
        style->applyStyle(format);
    }
    QFont font(format.font(), pd);
    d->textLayout = new QTextLayout(label, font, pd);
    QVector<QTextLayout::FormatRange> layouts;
    QTextLayout::FormatRange range;
    range.start = 0;
    range.length = label.length();
    range.format = format;
    layouts.append(range);
    d->textLayout->setFormats(layouts);

    QTextOption option(Qt::AlignLeft | Qt::AlignAbsolute);
    d->textLayout->setTextOption(option);
    d->textLayout->beginLayout();
    QTextLine line = d->textLayout->createLine();
    d->textLayout->endLayout();

    KoParagraphStyle pStyle(block.blockFormat(), QTextCharFormat());
    d->labelIndent = textIndent(d->note->textFrame()->begin().currentBlock(), nullptr, pStyle);
    if (line.naturalTextWidth() > -d->labelIndent) {
        KoTextLayoutArea::setExtraTextIndent(line.naturalTextWidth());
    } else {
        KoTextLayoutArea::setExtraTextIndent(-d->labelIndent);
    }
    d->labelIndent += pStyle.leftMargin();
    d->labelWidth = line.naturalTextWidth();
    d->labelHeight = line.naturalTextRect().bottom() - line.naturalTextRect().top();
    d->labelYOffset = -line.ascent();

    bool contNotNeeded = KoTextLayoutArea::layout(cursor);

    QTextLine blockLayoutLine = block.layout()->lineAt(0);

    if (blockLayoutLine.isValid()) {
        d->labelYOffset += blockLayoutLine.ascent();
    }

    if (!contNotNeeded) {
        QString contNote = notesConfig->footnoteContinuationForward();
        font.setBold(true);
        d->postLayout = new QTextLayout(contNote, font, pd);
        QVector<QTextLayout::FormatRange> contTextLayouts;
        QTextLayout::FormatRange contTextRange;
        contTextRange.start = 0;
        contTextRange.length = contNote.length();
        contTextRange.format = block.charFormat();
        ;
        contTextLayouts.append(contTextRange);
        d->postLayout->setFormats(contTextLayouts);

        QTextOption contTextOption(Qt::AlignLeft | Qt::AlignAbsolute);
        // option.setTextDirection();
        d->postLayout->setTextOption(contTextOption);
        d->postLayout->beginLayout();
        QTextLine contTextLine = d->postLayout->createLine();
        d->postLayout->endLayout();
        contTextLine.setPosition(QPointF(right() - contTextLine.naturalTextWidth(), bottom() - contTextLine.height()));

        documentLayout()->setContinuationObstruction(new KoTextLayoutObstruction(contTextLine.naturalTextRect(), false));
    }
    return contNotNeeded;
}

void KoTextLayoutNoteArea::setAsContinuedArea(bool isContinuedArea)
{
    d->isContinuedArea = isContinuedArea;
}

KoPointedAt KoTextLayoutNoteArea::hitTest(const QPointF &p, Qt::HitTestAccuracy accuracy) const
{
    KoPointedAt pointedAt;
    pointedAt.noteReference = -1;
    QPointF tmpP(p.x(), p.y() + (d->isContinuedArea ? OVERLAPPREVENTION : 0));

    pointedAt = KoTextLayoutArea::hitTest(tmpP, accuracy);

    if (tmpP.x() > left() && tmpP.x() < d->labelWidth && tmpP.y() < top() + d->labelYOffset + d->labelHeight) {
        pointedAt.noteReference = d->note->getPosInDocument();
        pointedAt.position = tmpP.x();
    }

    return pointedAt;
}

QRectF KoTextLayoutNoteArea::selectionBoundingBox(QTextCursor &cursor) const
{
    return KoTextLayoutArea::selectionBoundingBox(cursor).translated(0, d->isContinuedArea ? -OVERLAPPREVENTION : 0);
}
