/* This file is part of the KDE project
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
 * Copyright (C) 2010 KO Gmbh <boud@kogmbh.com>
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
#include "KoInlineNote.h"

#include <KoXmlReader.h>
#include <KoXmlWriter.h>
#include <KoXmlNS.h>
#include <KoShapeSavingContext.h>
#include <KoTextLoader.h>
#include <KoTextWriter.h>
#include <KoTextDocument.h>
#include <KoText.h>
#include <KoInlineTextObjectManager.h>
#include <KoGenStyles.h>
#include <KoStyleManager.h>
#include <KoCharacterStyle.h>
#include <KoElementReference.h>
#include <kdebug.h>
#include <writeodf/writeodftext.h>
#include <writeodf/writeodfoffice.h>
#include <writeodf/writeodfdc.h>

#include <QTextDocument>
#include <QTextFrame>
#include <QTextCursor>
#include <QString>
#include <QTextInlineObject>
#include <QFontMetricsF>
#include <QTextOption>
#include <QDateTime>
#include <QWeakPointer>

using namespace writeodf;

class KoInlineNote::Private
{
public:
    Private(KoInlineNote::Type t, const QTextDocument *d)
        : document(d)
        , textFrame(0)
        , autoNumbering(false)
        , type(t)
        , notesConfig(0)
    {
    }

    const QTextDocument *document;
    QTextFrame *textFrame;
    QString label;
    QString author;
    QDateTime date;
    bool autoNumbering;
    KoInlineNote::Type type;
    int posInDocument;
    KoOdfNotesConfiguration *notesConfig;
};

KoInlineNote::KoInlineNote(Type type, const QTextDocument *document)
    : KoInlineObject(true)
    , d(new Private(type, document))
{
    if (d->type == KoInlineNote::Footnote) {
        d->notesConfig = KoTextDocument(d->document).styleManager()->notesConfiguration(KoOdfNotesConfiguration::Footnote);
    } else if (d->type == KoInlineNote::Endnote) {
        d->notesConfig = KoTextDocument(d->document).styleManager()->notesConfiguration(KoOdfNotesConfiguration::Endnote);
    }
}

KoInlineNote::~KoInlineNote()
{
    delete d;
}

void KoInlineNote::setMotherFrame(QTextFrame *motherFrame)
{
    // We create our own subframe

    QTextCursor cursor(motherFrame->lastCursorPosition());
    QTextFrameFormat format;
    format.setProperty(KoText::SubFrameType, KoText::NoteFrameType);
    KoCharacterStyle *citationBodyTextStyle = KoTextDocument(d->document).styleManager()->characterStyle(d->notesConfig->citationBodyTextStyle());
    d->textFrame = cursor.insertFrame(format);
}

void KoInlineNote::setLabel(const QString &text)
{
    d->label = text;
}

void KoInlineNote::setAutoNumber(int autoNumber)
{
    if (d->autoNumbering) {
        d->label = d->notesConfig->numberFormat().formattedNumber(autoNumber + d->notesConfig->startValue());
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
    if (d->posInDocument == -1) {
        qDebug() << "undo";
    }
    d->posInDocument = posInDocument;
}

void KoInlineNote::resize(const QTextDocument *document, QTextInlineObject object, int posInDocument, const QTextCharFormat &format, QPaintDevice *pd)
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
        object.setWidth(fm.width(d->label));
        object.setAscent(fm.ascent());
        object.setDescent(fm.descent());
    }
}

void KoInlineNote::paint(QPainter &painter, QPaintDevice *pd, const QTextDocument *document, const QRectF &rect, QTextInlineObject object, int posInDocument, const QTextCharFormat &format)
{
    Q_UNUSED(document);
    Q_UNUSED(object);
    Q_UNUSED(posInDocument);

    if (d->label.isEmpty())
        return;
    QFont font(format.font(), pd);
    QTextLayout layout(d->label, font, pd);
    layout.setCacheEnabled(true);
    QList<QTextLayout::FormatRange> layouts;
    QTextLayout::FormatRange range;
    range.start = 0;
    range.length = d->label.length();
    range.format = format;
    KoCharacterStyle *citationTextStyle = KoTextDocument(d->document).styleManager()->characterStyle(d->notesConfig->citationTextStyle());
    qDebug() << "painting footnote" << d->notesConfig->citationTextStyle();
    if (citationTextStyle) {
        citationTextStyle->applyStyle(range.format, false);
    }
    range.format.setVerticalAlignment(QTextCharFormat::AlignSuperScript);
    layouts.append(range);
    layout.setAdditionalFormats(layouts);

    QTextOption option(Qt::AlignLeft | Qt::AlignAbsolute);
    option.setTextDirection(object.textDirection());
    layout.setTextOption(option);
    layout.beginLayout();
    layout.createLine();
    layout.endLayout();
    layout.draw(&painter, rect.topLeft());
}

bool KoInlineNote::loadOdf(const KoXmlElement & element, KoShapeLoadingContext &context)
{
    KoTextLoader loader(context);
    QTextCursor cursor(d->textFrame);

    if (element.namespaceURI() == KoXmlNS::text && element.localName() == "note") {

        QString className = element.attributeNS(KoXmlNS::text, "note-class");
        if (className == "footnote") {
            d->type = Footnote;
        }
        else if (className == "endnote") {
            d->type = Endnote;
        }
        else {
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
    }
    else if (element.namespaceURI() == KoXmlNS::office && element.localName() == "annotation") {
        d->author = element.attributeNS(KoXmlNS::text, "dc-creator");
        d->date = QDateTime::fromString(element.attributeNS(KoXmlNS::text, "dc-date"), Qt::ISODate);
        loader.loadBody(element, cursor); // would skip author and date, and do just the <text-p> and <text-list> elements
    }
    else {
        return false;
    }

    return true;
}

void KoInlineNote::saveOdf(KoShapeSavingContext & context)
{
    KoXmlWriter *writer = &context.xmlWriter();

    if (d->type == Footnote || d->type == Endnote) {
        text_note note(writer, (d->type == Footnote) ?"footnote" :"endnote");
        text_note_citation cite(note.add_text_note_citation());
        if (!autoNumbering()) {
            cite.set_text_label(d->label);
        }
        cite.addTextNode(d->label);

        text_note_body body(note.add_text_note_body());
        KoTextWriter textWriter(context);
        textWriter.write(d->document, d->textFrame->firstPosition(), d->textFrame->lastPosition());
    }
    else if (d->type == Annotation) {
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
        textWriter.write(d->document, d->textFrame->firstPosition(),d->textFrame->lastPosition());
    }
}

int KoInlineNote::getPosInDocument()
{
    return d->posInDocument;
}
