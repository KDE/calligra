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
#include <kdebug.h>

#include <QTextDocument>
#include <QTextFrame>
#include <QTextCursor>
#include <QString>
#include <QTextInlineObject>
#include <QFontMetricsF>
#include <QTextOption>
#include <QBuffer>
#include <QMap>

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

void KoInlineCite::setFields(const QMap<QString, QString> &fields)
{
    d->fields = fields;
}

QString KoInlineCite::value(const QString &fieldName) const
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

    foreach(const QString &type, KoOdfBibliographyConfiguration::bibDataFields) {
        if (!value(type).isEmpty()) {
            writer->addAttribute(qPrintable(QString("text:").append(type)), value(type));
        }
    }

    writer->addTextNode(d->label);
    writer->endElement();
}
