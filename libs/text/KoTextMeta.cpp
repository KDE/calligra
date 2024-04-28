/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 KO GmbH <ben.martin@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoTextMeta.h"

#include <KoShapeSavingContext.h>
#include <KoTextInlineRdf.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>

#include <QPointer>
#include <QTextDocument>
#include <QTextInlineObject>

#include "TextDebug.h"

// Include Q_UNSUSED classes, for building on Windows
#include <KoShapeLoadingContext.h>

class Q_DECL_HIDDEN KoTextMeta::Private
{
public:
    Private(const QTextDocument *doc)
        : document(doc)
        , posInDocument(0)
    {
    }
    const QTextDocument *document;
    int posInDocument;
    QPointer<KoTextMeta> endBookmark;
    BookmarkType type;
};

KoTextMeta::KoTextMeta(const QTextDocument *document)
    : KoInlineObject(false)
    , d(new Private(document))
{
    d->endBookmark.clear();
}

KoTextMeta::~KoTextMeta()
{
    delete d;
}

void KoTextMeta::saveOdf(KoShapeSavingContext &context)
{
    KoXmlWriter &writer = context.xmlWriter();

    debugText << "kom.save() this:" << (void *)this << " d->type:" << d->type;
    if (inlineRdf()) {
        debugText << "kom.save() have inline Rdf";
    }

    if (d->type == StartBookmark) {
        writer.startElement("text:meta", false);
        writer.addAttribute("text:name", "foo");

        if (inlineRdf()) {
            inlineRdf()->saveOdf(context, &writer);
        }
    } else {
        debugText << "adding endelement.";
        writer.endElement();
    }
    debugText << "kom.save() done this:" << (void *)this << " d->type:" << d->type;
}

bool KoTextMeta::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
    debugText << "kom.load()";
    return true;
}

void KoTextMeta::updatePosition(const QTextDocument *document, int posInDocument, const QTextCharFormat &format)
{
    Q_UNUSED(format);
    d->document = document;
    d->posInDocument = posInDocument;
}

void KoTextMeta::resize(const QTextDocument *document, QTextInlineObject &object, int posInDocument, const QTextCharFormat &format, QPaintDevice *pd)
{
    Q_UNUSED(document);
    Q_UNUSED(posInDocument);
    Q_UNUSED(format);
    Q_UNUSED(pd);
    object.setWidth(0);
    object.setAscent(0);
    object.setDescent(0);
}

void KoTextMeta::paint(QPainter &, QPaintDevice *, const QTextDocument *, const QRectF &, const QTextInlineObject &, int, const QTextCharFormat &)
{
    // nothing to paint.
}

void KoTextMeta::setType(BookmarkType type)
{
    d->type = type;
}

KoTextMeta::BookmarkType KoTextMeta::type() const
{
    return d->type;
}

void KoTextMeta::setEndBookmark(KoTextMeta *bookmark)
{
    d->type = StartBookmark;
    bookmark->d->type = EndBookmark;
    d->endBookmark = bookmark;
}

KoTextMeta *KoTextMeta::endBookmark() const
{
    return d->endBookmark.data();
}

int KoTextMeta::position() const
{
    return d->posInDocument;
}
