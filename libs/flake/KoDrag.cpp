/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007-2008 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2009 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoDrag.h"
#include "KoDragOdfSaveHelper.h"

#include <QApplication>
#include <QBuffer>
#include <QByteArray>
#include <QClipboard>
#include <QMimeData>
#include <QString>

#include <FlakeDebug.h>

#include "KoShapeSavingContext.h"
#include <KoDocumentBase.h>
#include <KoEmbeddedDocumentSaver.h>
#include <KoGenStyles.h>
#include <KoOdfWriteStore.h>
#include <KoStore.h>
#include <KoXmlWriter.h>

class KoDragPrivate
{
public:
    KoDragPrivate()
        : mimeData(nullptr)
    {
    }
    ~KoDragPrivate()
    {
        delete mimeData;
    }
    QMimeData *mimeData;
};

KoDrag::KoDrag()
    : d(new KoDragPrivate())
{
}

KoDrag::~KoDrag()
{
    delete d;
}

bool KoDrag::setOdf(const char *mimeType, KoDragOdfSaveHelper &helper)
{
    struct Finally {
        Finally(KoStore *s)
            : store(s)
        {
        }
        ~Finally()
        {
            delete store;
        }
        KoStore *store;
    };

    QBuffer buffer;
    KoStore *store = KoStore::createStore(&buffer, KoStore::Write, mimeType);
    Finally finally(store); // delete store when we exit this scope
    Q_ASSERT(store);
    Q_ASSERT(!store->bad());

    KoOdfWriteStore odfStore(store);
    KoEmbeddedDocumentSaver embeddedSaver;

    KoXmlWriter *manifestWriter = odfStore.manifestWriter(mimeType);
    KoXmlWriter *contentWriter = odfStore.contentWriter();

    if (!contentWriter) {
        return false;
    }

    KoGenStyles mainStyles;
    KoXmlWriter *bodyWriter = odfStore.bodyWriter();
    KoShapeSavingContext *context = helper.context(bodyWriter, mainStyles, embeddedSaver);

    if (!helper.writeBody()) {
        return false;
    }

    mainStyles.saveOdfStyles(KoGenStyles::DocumentAutomaticStyles, contentWriter);

    odfStore.closeContentWriter();

    // add manifest line for content.xml
    manifestWriter->addManifestEntry("content.xml", "text/xml");

    if (!mainStyles.saveOdfStylesDotXml(store, manifestWriter)) {
        return false;
    }

    if (!context->saveDataCenter(store, manifestWriter)) {
        debugFlake << "save data centers failed";
        return false;
    }

    // Save embedded objects
    KoDocumentBase::SavingContext documentContext(odfStore, embeddedSaver);
    if (!embeddedSaver.saveEmbeddedDocuments(documentContext)) {
        debugFlake << "save embedded documents failed";
        return false;
    }

    // Write out manifest file
    if (!odfStore.closeManifestWriter()) {
        return false;
    }

    delete store; // make sure the buffer if fully flushed.
    finally.store = nullptr;
    setData(mimeType, buffer.buffer());

    return true;
}

void KoDrag::setData(const QString &mimeType, const QByteArray &data)
{
    if (d->mimeData == nullptr) {
        d->mimeData = new QMimeData();
    }
    d->mimeData->setData(mimeType, data);
}

void KoDrag::addToClipboard()
{
    if (d->mimeData) {
        QApplication::clipboard()->setMimeData(d->mimeData);
        d->mimeData = nullptr;
    }
}

QMimeData *KoDrag::mimeData()
{
    QMimeData *mimeData = d->mimeData;
    d->mimeData = nullptr;
    return mimeData;
}
