/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2011 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoTextOdfSaveHelper.h"

#include "KoTextWriter.h"
#include <KoOdf.h>
#include <KoShapeSavingContext.h>
#include <KoTextDocument.h>
#include <KoXmlWriter.h>

#include <QTextDocument>

struct Q_DECL_HIDDEN KoTextOdfSaveHelper::Private {
    Private(const QTextDocument *document, int from, int to)
        : context(nullptr)
        , document(document)
        , from(from)
        , to(to)
#ifdef SHOULD_BUILD_RDF
        , rdfModel(0)
#endif
    {
    }

    KoShapeSavingContext *context;
    const QTextDocument *document;

    int from;
    int to;

#ifdef SHOULD_BUILD_RDF
    QSharedPointer<Soprano::Model> rdfModel; //< This is so cut/paste can serialize the relevant RDF to the clipboard
#endif
};

KoTextOdfSaveHelper::KoTextOdfSaveHelper(const QTextDocument *document, int from, int to)
    : d(new Private(document, from, to))
{
}

KoTextOdfSaveHelper::~KoTextOdfSaveHelper()
{
    delete d;
}

bool KoTextOdfSaveHelper::writeBody()
{
    if (d->to < d->from) {
        qSwap(d->to, d->from);
    }
    Q_ASSERT(d->context);
    KoXmlWriter &bodyWriter = d->context->xmlWriter();
    bodyWriter.startElement("office:body");
    bodyWriter.startElement(KoOdf::bodyContentElement(KoOdf::Text, true));

    KoTextWriter writer(*d->context, nullptr);
    writer.write(d->document, d->from, d->to);

    bodyWriter.endElement(); // office:element
    bodyWriter.endElement(); // office:body
    return true;
}

KoShapeSavingContext *KoTextOdfSaveHelper::context(KoXmlWriter *bodyWriter, KoGenStyles &mainStyles, KoEmbeddedDocumentSaver &embeddedSaver)
{
    d->context = new KoShapeSavingContext(*bodyWriter, mainStyles, embeddedSaver);
    return d->context;
}

#ifdef SHOULD_BUILD_RDF
void KoTextOdfSaveHelper::setRdfModel(QSharedPointer<Soprano::Model> m)
{
    d->rdfModel = m;
}

QSharedPointer<Soprano::Model> KoTextOdfSaveHelper::rdfModel() const
{
    return d->rdfModel;
}
#endif

KoStyleManager *KoTextOdfSaveHelper::styleManager() const
{
    return KoTextDocument(d->document).styleManager();
}
