/* This file is part of the  KDE project
 * Copyright (C) 2011 Smit Patel <smitpatel24@gmail.com>
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
#include "KoInlineCite.h"
#include "KoInlineTextObjectManager.h"

#include <KoXmlReader.h>
#include <KoXmlWriter.h>
#include <KoXmlNS.h>
#include <KoShapeLoadingContext.h>
#include <KoShapeSavingContext.h>
#include <KoTextLoader.h>
#include <KoTextWriter.h>
#include <KoTextDocument.h>
#include <KoText.h>
#include <KoOdfBibliographyConfiguration.h>
#include <KoGenStyles.h>
#include <KoStyleManager.h>
#include <KDebug>

#include <QTextDocument>
#include <QTextFrame>
#include <QTextCursor>
#include <QString>
#include <QTextInlineObject>
#include <QFontMetricsF>
#include <QTextOption>
#include <QBuffer>
#include <QMap>
//#include <QMessageBox>

class KoInlineCite::Private
{
public:
    Private(KoInlineCite::Type t)
        : type(t)
    {
    }

    KoInlineCite::Type type;
    int posInDocument;
    QString label;

    QMap<QString, QString> fields;
};

KoInlineCite::KoInlineCite(Type type)
    :KoInlineObject(true)
    ,d(new Private(type))
{
}

KoInlineCite::~KoInlineCite()
{
    delete d;
}

KoInlineCite::Type KoInlineCite::type() const
{
    return d->type;
}

void KoInlineCite::setType(Type t)
{
    d->type = t;
}

void KoInlineCite::setField(QString fieldName, QString fieldValue)
{
    if (KoOdfBibliographyConfiguration::bibDataFields.contains(fieldName.toLower())) {
        d->fields[fieldName.toLower()] = fieldValue;
    }
}

void KoInlineCite::setFields(QMap<QString, QString> fields)
{
    d->fields = fields;
}

QString KoInlineCite::value(QString fieldName) const
{
    return d->fields[fieldName];
}

QMap<QString, QString> KoInlineCite::fieldMap() const
{
    return d->fields;
}

int KoInlineCite::posInDocument() const
{
    return d->posInDocument;
}

bool KoInlineCite::operator!= (const KoInlineCite &cite) const
{
    bool ret = false;
    foreach(QString t, KoOdfBibliographyConfiguration::bibDataFields) {
        ret = ret || (d->fields[t] != cite.value(t));
    }

    return ret;
}

KoInlineCite &KoInlineCite::operator =(const  KoInlineCite &cite)
{
    d->fields = cite.fieldMap();
    return *this;
}

void KoInlineCite::updatePosition(const QTextDocument *document, int posInDocument, const QTextCharFormat &format)
{
    Q_UNUSED(document);
    Q_UNUSED(format);
    d->posInDocument = posInDocument;
}

void KoInlineCite::resize(const QTextDocument *document, QTextInlineObject object, int posInDocument, const QTextCharFormat &format, QPaintDevice *pd)
{
    Q_UNUSED(document);
    Q_UNUSED(posInDocument);
    if (d->fields["identifier"].isEmpty())
        return;

    KoOdfBibliographyConfiguration *bibConfiguration = KoTextDocument(document).styleManager()->bibliographyConfiguration();

    if (!bibConfiguration->numberedEntries()) {
        d->label = QString("%1%2%3").arg(bibConfiguration->prefix())
                    .arg(d->fields["identifier"]).arg(bibConfiguration->suffix());
    } else {
        d->label = QString("%1%2%3").arg(bibConfiguration->prefix())
                    .arg(QString::number(manager()->citationsSortedByPosition(true).indexOf(this) + 1))
                    .arg(bibConfiguration->suffix());
    }

    Q_ASSERT(format.isCharFormat());
    QFontMetricsF fm(format.font(), pd);
    object.setWidth(fm.width(d->label));
    object.setAscent(fm.ascent());
    object.setDescent(fm.descent());
}

void KoInlineCite::paint(QPainter &painter, QPaintDevice *pd, const QTextDocument *document, const QRectF &rect, QTextInlineObject object, int posInDocument, const QTextCharFormat &format)
{
    Q_UNUSED(document);
    Q_UNUSED(object);
    Q_UNUSED(posInDocument);

    if (d->fields["identifier"].isEmpty())
        return;

    KoOdfBibliographyConfiguration *bibConfiguration = KoTextDocument(document).styleManager()->bibliographyConfiguration();

    if (!bibConfiguration->numberedEntries()) {
        d->label = QString("%1%2%3").arg(bibConfiguration->prefix())
                    .arg(d->fields["identifier"]).arg(bibConfiguration->suffix());
    } else {
        d->label = QString("%1%2%3").arg(bibConfiguration->prefix())
                    .arg(QString::number(manager()->citationsSortedByPosition(true, document->firstBlock()).indexOf(this) + 1))
                    .arg(bibConfiguration->suffix());
    }

    QFont font(format.font(), pd);
    QTextLayout layout(d->label, font, pd);
    layout.setCacheEnabled(true);
    QList<QTextLayout::FormatRange> layouts;
    QTextLayout::FormatRange range;
    range.start = 0;
    range.length = d->label.length();
    range.format = format;
    range.format.setVerticalAlignment(QTextCharFormat::AlignNormal);
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

bool KoInlineCite::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    Q_UNUSED(context);
    //KoTextLoader loader(context);
    if (element.namespaceURI() == KoXmlNS::text && element.localName() == "bibliography-mark") {

        foreach(QString field, KoOdfBibliographyConfiguration::bibDataFields) {
            d->fields[field] = element.attributeNS(KoXmlNS::text, field);
        }

        //Now checking for cloned citation (with same identifier)
        if (manager()->citations(true).keys().count(d->fields["identifier"]) > 1) {
            this->setType(KoInlineCite::ClonedCitation);
        }
    }
    else {
        return false;
    }
    return true;
}

void KoInlineCite::saveOdf(KoShapeSavingContext &context)
{
    KoXmlWriter *writer = &context.xmlWriter();
    writer->startElement("text:bibliography-mark", false);

    if (!value("identifier").isEmpty())
        writer->addAttribute("text:identifier", value("identifier"));
    if (!value("bibliography-type").isEmpty())
        writer->addAttribute("text:bibliography-type", value("bibliography-type"));
    if (!value("address").isEmpty())
        writer->addAttribute("text:address", value("identifier"));
    if (!value("annote").isEmpty())
        writer->addAttribute("text:annote", value("annote"));
    if (!value("author").isEmpty())
        writer->addAttribute("text:author", value("author"));
    if (!value("booktitle").isEmpty())
        writer->addAttribute("text:booktitle", value("booktitle"));
    if (!value("chapter").isEmpty())
        writer->addAttribute("text:chapter", value("chapter"));
    if (!value("edition").isEmpty())
        writer->addAttribute("text:edition", value("edition"));
    if (!value("editor").isEmpty())
        writer->addAttribute("text:editor", value("editor"));
    if (!value("howpublished").isEmpty())
        writer->addAttribute("text:howpublished", value("howpublished"));
    if (!value("institution").isEmpty())
        writer->addAttribute("text:institution", value("institution"));
    if (!value("journal").isEmpty())
        writer->addAttribute("text:journal", value("journal"));
    if (!value("month").isEmpty())
        writer->addAttribute("text:month", value("month"));
    if (!value("note").isEmpty())
        writer->addAttribute("text:note", value("note"));
    if (!value("number").isEmpty())
        writer->addAttribute("text:number", value("number"));
    if (!value("organizations").isEmpty())
        writer->addAttribute("text:organizations", value("organizations"));
    if (!value("pages").isEmpty())
        writer->addAttribute("text:pages", value("pages"));
    if (!value("publisher").isEmpty())
        writer->addAttribute("text:publisher", value("publisher"));
    if (!value("school").isEmpty())
        writer->addAttribute("text:school", value("school"));
    if (!value("series").isEmpty())
        writer->addAttribute("text:series", value("series"));
    if (!value("title").isEmpty())
        writer->addAttribute("text:title", value("title"));
    if (!value("report-type").isEmpty())
        writer->addAttribute("text:report-type", value("report-type"));
    if (!value("volume").isEmpty())
        writer->addAttribute("text:volume", value("volume"));
    if (!value("year").isEmpty())
        writer->addAttribute("text:year", value("year"));
    if (!value("url").isEmpty())
        writer->addAttribute("text:url", value("url"));
    if (!value("isbn").isEmpty())
        writer->addAttribute("text:isbn", value("isbn"));
    if (!value("issn").isEmpty())
        writer->addAttribute("text:issn", value("issn"));
    if (!value("custom1").isEmpty())
        writer->addAttribute("text:custom1", value("custom1"));
    if (!value("custom2").isEmpty())
        writer->addAttribute("text:custom2", value("custom2"));
    if (!value("custom3").isEmpty())
        writer->addAttribute("text:custom3", value("custom3"));
    if (!value("custom4").isEmpty())
        writer->addAttribute("text:custom4", value("custom4"));
    if (!value("custom5").isEmpty())
        writer->addAttribute("text:custom5", value("custom5"));

    writer->addTextNode(d->label);
    writer->endElement();
}
