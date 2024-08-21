/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoOdfWriteStore.h"

#include <QBuffer>

#include <KLocalizedString>
#include <OdfDebug.h>
#include <QTemporaryFile>

#include <KoStore.h>
#include <KoStoreDevice.h>
#include <KoXmlWriter.h>

#include "KoXmlNS.h"

struct Q_DECL_HIDDEN KoOdfWriteStore::Private {
    Private(KoStore *store)
        : store(store)
        , storeDevice(nullptr)
        , contentWriter(nullptr)
        , bodyWriter(nullptr)
        , manifestWriter(nullptr)
        , contentTmpFile(nullptr)
    {
    }

    ~Private()
    {
        // If all the right close methods were called, nothing should remain,
        // so those deletes are really just in case.
        Q_ASSERT(!contentWriter);
        delete contentWriter;
        Q_ASSERT(!bodyWriter);
        delete bodyWriter;
        Q_ASSERT(!storeDevice);
        delete storeDevice;
        Q_ASSERT(!manifestWriter);
        delete manifestWriter;
    }

    KoStore *store;
    KoStoreDevice *storeDevice;
    KoXmlWriter *contentWriter;

    KoXmlWriter *bodyWriter;
    KoXmlWriter *manifestWriter;
    QTemporaryFile *contentTmpFile;
};

KoOdfWriteStore::KoOdfWriteStore(KoStore *store)
    : d(new Private(store))
{
}

KoOdfWriteStore::~KoOdfWriteStore()
{
    delete d;
}

KoXmlWriter *KoOdfWriteStore::createOasisXmlWriter(QIODevice *dev, const char *rootElementName)
{
    KoXmlWriter *writer = new KoXmlWriter(dev);
    writer->startDocument(rootElementName);
    writer->startElement(rootElementName);

    if (qstrcmp(rootElementName, "VL:version-list") == 0) {
        writer->addAttribute("xmlns:VL", KoXmlNS::VL);
        writer->addAttribute("xmlns:dc", KoXmlNS::dc);
        return writer;
    }

    writer->addAttribute("xmlns:office", KoXmlNS::office);
    writer->addAttribute("xmlns:meta", KoXmlNS::meta);

    if (qstrcmp(rootElementName, "office:document-meta") != 0) {
        writer->addAttribute("xmlns:config", KoXmlNS::config);
        writer->addAttribute("xmlns:text", KoXmlNS::text);
        writer->addAttribute("xmlns:table", KoXmlNS::table);
        writer->addAttribute("xmlns:draw", KoXmlNS::draw);
        writer->addAttribute("xmlns:presentation", KoXmlNS::presentation);
        writer->addAttribute("xmlns:dr3d", KoXmlNS::dr3d);
        writer->addAttribute("xmlns:chart", KoXmlNS::chart);
        writer->addAttribute("xmlns:form", KoXmlNS::form);
        writer->addAttribute("xmlns:script", KoXmlNS::script);
        writer->addAttribute("xmlns:style", KoXmlNS::style);
        writer->addAttribute("xmlns:number", KoXmlNS::number);
        writer->addAttribute("xmlns:math", KoXmlNS::math);
        writer->addAttribute("xmlns:svg", KoXmlNS::svg);
        writer->addAttribute("xmlns:fo", KoXmlNS::fo);
        writer->addAttribute("xmlns:anim", KoXmlNS::anim);
        writer->addAttribute("xmlns:smil", KoXmlNS::smil);
        writer->addAttribute("xmlns:calligra", KoXmlNS::calligra);
        writer->addAttribute("xmlns:officeooo", KoXmlNS::officeooo);
        writer->addAttribute("xmlns:delta", KoXmlNS::delta);
        writer->addAttribute("xmlns:split", KoXmlNS::split);
        writer->addAttribute("xmlns:ac", KoXmlNS::ac);
    }

    if (qstrcmp(rootElementName, "office:document-settings") == 0) {
        writer->addAttribute("xmlns:ooo", KoXmlNS::ooo);
    }

    writer->addAttribute("office:version", "1.2");

    writer->addAttribute("xmlns:dc", KoXmlNS::dc);
    writer->addAttribute("xmlns:xlink", KoXmlNS::xlink);
    return writer;
}

KoStore *KoOdfWriteStore::store() const
{
    return d->store;
}

KoXmlWriter *KoOdfWriteStore::contentWriter()
{
    if (!d->contentWriter) {
        if (!d->store->open("content.xml")) {
            return nullptr;
        }
        d->storeDevice = new KoStoreDevice(d->store);
        d->contentWriter = createOasisXmlWriter(d->storeDevice, "office:document-content");
    }
    return d->contentWriter;
}

KoXmlWriter *KoOdfWriteStore::bodyWriter()
{
    if (!d->bodyWriter) {
        Q_ASSERT(!d->contentTmpFile);
        d->contentTmpFile = new QTemporaryFile;
        if (!d->contentTmpFile->open()) {
            warnOdf << "Failed to open the temporary content file";
            delete d->contentTmpFile;
            d->contentTmpFile = nullptr;
            return nullptr;
        }
        d->bodyWriter = new KoXmlWriter(d->contentTmpFile, 1);
    }
    return d->bodyWriter;
}

bool KoOdfWriteStore::closeContentWriter()
{
    Q_ASSERT(d->bodyWriter);
    Q_ASSERT(d->contentTmpFile);

    delete d->bodyWriter;
    d->bodyWriter = nullptr;

    // copy over the contents from the tempfile to the real one
    d->contentTmpFile->close(); // does not really close but seeks to the beginning of the file
    if (d->contentWriter) {
        d->contentWriter->addCompleteElement(d->contentTmpFile);
    }
    d->contentTmpFile->close(); // seek again to the beginning
    delete d->contentTmpFile;
    d->contentTmpFile = nullptr; // and finally close and remove the QTemporaryFile

    if (d->contentWriter) {
        d->contentWriter->endElement(); // document-content
        d->contentWriter->endDocument();
        delete d->contentWriter;
        d->contentWriter = nullptr;
    }

    delete d->storeDevice;
    d->storeDevice = nullptr;
    if (!d->store->close()) { // done with content.xml
        return false;
    }
    return true;
}

KoXmlWriter *KoOdfWriteStore::manifestWriter(const char *mimeType)
{
    if (!d->manifestWriter) {
        // the pointer to the buffer is already stored in the KoXmlWriter, no need to store it here as well
        QBuffer *manifestBuffer = new QBuffer;
        manifestBuffer->open(QIODevice::WriteOnly);
        d->manifestWriter = new KoXmlWriter(manifestBuffer);
        d->manifestWriter->startDocument("manifest:manifest");
        d->manifestWriter->startElement("manifest:manifest");
        d->manifestWriter->addAttribute("xmlns:manifest", KoXmlNS::manifest);
        d->manifestWriter->addAttribute("manifest:version", "1.2");
        d->manifestWriter->addManifestEntry("/", mimeType);
    }
    return d->manifestWriter;
}

KoXmlWriter *KoOdfWriteStore::manifestWriter()
{
    Q_ASSERT(d->manifestWriter);
    return d->manifestWriter;
}

bool KoOdfWriteStore::closeManifestWriter(bool writeMainfest)
{
    Q_ASSERT(d->manifestWriter);
    bool ok = true;
    if (writeMainfest) {
        d->manifestWriter->endElement();
        d->manifestWriter->endDocument();
        QBuffer *buffer = static_cast<QBuffer *>(d->manifestWriter->device());
        if (d->store->open("META-INF/manifest.xml")) {
            qint64 written = d->store->write(buffer->buffer());
            ok = (written == (qint64)buffer->buffer().size() && d->store->close());
        } else {
            ok = false;
        }
        delete buffer;
    }
    delete d->manifestWriter;
    d->manifestWriter = nullptr;
    return ok;
}
