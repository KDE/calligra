/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2010 KO Gmbh <boud@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "KoInlineNote.h"

#include <KoOdfNumberDefinition.h>
#include <KoParagraphStyle.h>
#include <KoShapeSavingContext.h>
#include <KoStyleManager.h>
#include <KoText.h>
#include <KoTextDocument.h>
#include <KoTextLoader.h>
#include <KoTextWriter.h>
#include <KoXmlNS.h>
#include <KoXmlWriter.h>
#include <writeodf/writeodfdc.h>
#include <writeodf/writeodfoffice.h>
#include <writeodf/writeodftext.h>

#include "TextDebug.h"

#include <QDateTime>
#include <QFontMetricsF>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextFrame>
#include <QTextInlineObject>
#include <QTextOption>

using namespace writeodf;

class Q_DECL_HIDDEN KoInlineNote::Private
{
public:
    Private(KoInlineNote::Type t)
        : textFrame(nullptr)
        , autoNumbering(false)
        , type(t)
    {
    }

    QTextDocument *document;
    QTextFrame *textFrame;
    QString label;
    QString author;
    QDateTime date;
    bool autoNumbering;
    KoInlineNote::Type type;
    int posInDocument;
};

KoInlineNote::KoInlineNote(Type type)
    : KoInlineObject(true)
    , d(new Private(type))
{
}

KoInlineNote::~KoInlineNote()
{
    delete d;
}

void KoInlineNote::setMotherFrame(QTextFrame *motherFrame)
{
    d->document = motherFrame->document();

    // We create our own subframe

    QTextCursor cursor(motherFrame->lastCursorPosition());
    QTextFrameFormat format;
    format.setProperty(KoText::SubFrameType, KoText::NoteFrameType);
    d->textFrame = cursor.insertFrame(format);

    // Now let's make sure it has the right paragraph
    KoOdfNotesConfiguration *notesConfig = nullptr;
    if (d->type == KoInlineNote::Footnote) {
        notesConfig = KoTextDocument(d->document).styleManager()->notesConfiguration(KoOdfNotesConfiguration::Footnote);
    } else if (d->type == KoInlineNote::Endnote) {
        notesConfig = KoTextDocument(d->document).styleManager()->notesConfiguration(KoOdfNotesConfiguration::Endnote);
    }

    KoParagraphStyle *style = static_cast<KoParagraphStyle *>(notesConfig->defaultNoteParagraphStyle());
    if (style) {
        QTextBlockFormat bf;
        QTextCharFormat cf;
        style->applyStyle(bf);
        style->KoCharacterStyle::applyStyle(cf);
        cursor.setBlockFormat(bf);
        cursor.setBlockCharFormat(cf);
    }
}

void KoInlineNote::setLabel(const QString &text)
{
    d->label = text;
}

void KoInlineNote::setAutoNumber(int autoNumber)
{
    if (d->autoNumbering) {
        KoOdfNotesConfiguration *notesConfig = nullptr;
        if (d->type == KoInlineNote::Footnote) {
            notesConfig = KoTextDocument(d->document).styleManager()->notesConfiguration(KoOdfNotesConfiguration::Footnote);
        } else if (d->type == KoInlineNote::Endnote) {
            notesConfig = KoTextDocument(d->document).styleManager()->notesConfiguration(KoOdfNotesConfiguration::Endnote);
        }
        d->label = notesConfig->numberFormat().formattedNumber(autoNumber + notesConfig->startValue());
    }
}

QTextFrame *KoInlineNote::textFrame() const
{
    return d->textFrame;
}

void KoInlineNote::setTextFrame(QTextFrame *textFrame)
{
    d->textFrame = textFrame;
}

QString KoInlineNote::label() const
{
    return d->label;
}

bool KoInlineNote::autoNumbering() const
{
    return d->autoNumbering;
}

void KoInlineNote::setAutoNumbering(bool on)
{
    d->autoNumbering = on;
}

KoInlineNote::Type KoInlineNote::type() const
{
    return d->type;
}

void KoInlineNote::updatePosition(const QTextDocument *document, int posInDocument, const QTextCharFormat &format)
{
    Q_UNUSED(document);
    Q_UNUSED(format);

    d->posInDocument = posInDocument;
}

void KoInlineNote::resize(const QTextDocument *document, QTextInlineObject &object, int posInDocument, const QTextCharFormat &format, QPaintDevice *pd)
{
    Q_UNUSED(document);
    Q_UNUSED(posInDocument);
    if (d->label.isEmpty()) {
        object.setWidth(0);
        object.setAscent(0);
        object.setDescent(0);
    } else {
        Q_ASSERT(format.isCharFormat());
        QFontMetricsF fm(format.font(), pd);
        object.setWidth(fm.boundingRect(d->label).width());
        object.setAscent(fm.ascent());
        object.setDescent(fm.descent());
    }
}

void KoInlineNote::paint(QPainter &painter,
                         QPaintDevice *pd,
                         const QTextDocument *document,
                         const QRectF &rect,
                         const QTextInlineObject &object,
                         int posInDocument,
                         const QTextCharFormat &originalFormat)
{
    Q_UNUSED(document);
    Q_UNUSED(posInDocument);

    if (d->label.isEmpty())
        return;

    QTextCharFormat format = originalFormat;
    KoOdfNotesConfiguration *notesConfig = nullptr;
    if (d->type == KoInlineNote::Footnote) {
        notesConfig = KoTextDocument(d->document).styleManager()->notesConfiguration(KoOdfNotesConfiguration::Footnote);
    } else if (d->type == KoInlineNote::Endnote) {
        notesConfig = KoTextDocument(d->document).styleManager()->notesConfiguration(KoOdfNotesConfiguration::Endnote);
    }
    KoCharacterStyle *style = static_cast<KoCharacterStyle *>(notesConfig->citationBodyTextStyle());
    if (style) {
        style->applyStyle(format);
    }

    QFont font(format.font(), pd);
    QTextLayout layout(d->label, font, pd);
    layout.setCacheEnabled(true);
    QVector<QTextLayout::FormatRange> layouts;
    QTextLayout::FormatRange range;
    range.start = 0;
    range.length = d->label.length();
    range.format = format;
    layouts.append(range);
    layout.setFormats(layouts);

    QTextOption option(Qt::AlignLeft | Qt::AlignAbsolute);
    option.setTextDirection(object.textDirection());
    layout.setTextOption(option);
    layout.beginLayout();
    layout.createLine();
    layout.endLayout();
    layout.draw(&painter, rect.topLeft());
}

bool KoInlineNote::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    KoTextLoader loader(context);
    QTextCursor cursor(d->textFrame);

    if (element.namespaceURI() == KoXmlNS::text && element.localName() == "note") {
        QString className = element.attributeNS(KoXmlNS::text, "note-class");
        if (className == "footnote") {
            d->type = Footnote;
        } else if (className == "endnote") {
            d->type = Endnote;
        } else {
            return false;
        }

        for (KoXmlNode node = element.firstChild(); !node.isNull(); node = node.nextSibling()) {
            KoXmlElement ts = node.toElement();
            if (ts.namespaceURI() != KoXmlNS::text)
                continue;
            if (ts.localName() == "note-body") {
                loader.loadBody(ts, cursor);
            } else if (ts.localName() == "note-citation") {
                d->label = ts.attributeNS(KoXmlNS::text, "label");
                if (d->label.isEmpty()) {
                    setAutoNumbering(true);
                    d->label = ts.text();
                }
            }
        }
    } else if (element.namespaceURI() == KoXmlNS::office && element.localName() == "annotation") {
        d->author = element.attributeNS(KoXmlNS::text, "dc-creator");
        d->date = QDateTime::fromString(element.attributeNS(KoXmlNS::text, "dc-date"), Qt::ISODate);
        loader.loadBody(element, cursor); // would skip author and date, and do just the <text-p> and <text-list> elements
    } else {
        return false;
    }

    return true;
}

void KoInlineNote::saveOdf(KoShapeSavingContext &context)
{
    KoXmlWriter *writer = &context.xmlWriter();

    if (d->type == Footnote || d->type == Endnote) {
        text_note note(writer, (d->type == Footnote) ? "footnote" : "endnote");
        text_note_citation cite(note.add_text_note_citation());
        if (!autoNumbering()) {
            cite.set_text_label(d->label);
        }
        cite.addTextNode(d->label);

        text_note_body body(note.add_text_note_body());
        KoTextWriter textWriter(context);
        textWriter.write(d->document, d->textFrame->firstPosition(), d->textFrame->lastPosition());
    } else if (d->type == Annotation) {
        office_annotation annotation(writer);
        if (!d->author.isEmpty()) {
            dc_creator creator(annotation.add_dc_creator());
            creator.addTextNode(d->author);
        }
        if (d->date.isValid()) {
            dc_date date(annotation.add_dc_date());
            date.addTextNode(d->date.toString(Qt::ISODate));
        }

        KoTextWriter textWriter(context);
        textWriter.write(d->document, d->textFrame->firstPosition(), d->textFrame->lastPosition());
    }
}

int KoInlineNote::getPosInDocument() const
{
    return d->posInDocument;
}
