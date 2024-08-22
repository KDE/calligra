/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2009 Jarosław Staniek <staniek@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoOdfExporter.h"
#include "MsooXmlUtils.h"

#include <QBuffer>
#include <QByteArray>

#include "MsooXmlDebug.h"

#include <KoFilterChain.h>
#include <KoGenStyles.h>
#include <KoOdfWriteStore.h>
#include <KoStoreDevice.h>
#include <KoXmlWriter.h>

#include <memory>

KoOdfWriters::KoOdfWriters()
    : content(nullptr)
    , body(nullptr)
    , meta(nullptr)
    , manifest(nullptr)
    , mainStyles(nullptr)
{
}

//------------------------------------------

class Q_DECL_HIDDEN KoOdfExporter::Private
{
public:
    Private() = default;
    QByteArray bodyContentElement;
};

//------------------------------------------

KoOdfExporter::KoOdfExporter(const QString &bodyContentElement, QObject *parent)
    : KoFilter(parent)
    , d(new Private)
{
    d->bodyContentElement = QByteArray("office:") + bodyContentElement.toLatin1();
}

KoOdfExporter::~KoOdfExporter()
{
    delete d;
}

KoFilter::ConversionStatus KoOdfExporter::convert(const QByteArray &from, const QByteArray &to)
{
    // check for proper conversion
    if (!acceptsSourceMimeType(from)) {
        warnMsooXml << "Invalid source mimetype" << from;
        return KoFilter::NotImplemented;
    }
    if (!acceptsDestinationMimeType(to)) {
        warnMsooXml << "Invalid destination mimetype" << to;
        return KoFilter::NotImplemented;
    }

    // create output files
    std::unique_ptr<KoStore> outputStore(KoStore::createStore(m_chain->outputFile(), KoStore::Write, to, KoStore::Zip));
    if (!outputStore || outputStore->bad()) {
        warnMsooXml << "Unable to open output file!";
        return KoFilter::FileNotFound;
    }
    debugMsooXml << "created outputStore.";
    KoOdfWriteStore oasisStore(outputStore.get());

    debugMsooXml << "created oasisStore.";

    // KoGenStyles for writing styles while we're parsing
    KoGenStyles mainStyles;

    KoOdfWriters writers;
    writers.mainStyles = &mainStyles;

    // create a writer for meta.xml
    QBuffer buf;
    buf.open(QIODevice::WriteOnly);
    KoXmlWriter metaWriter(&buf);
    writers.meta = &metaWriter;

    // create a writer for manifest.xml
    QBuffer manifestBuf;
    manifestBuf.open(QIODevice::WriteOnly);
    KoXmlWriter manifestWriter(&manifestBuf);
    writers.manifest = &manifestWriter;

    // open contentWriter & bodyWriter *temp* writers
    // so we can write picture files while we parse
    QBuffer contentBuf;
    KoXmlWriter contentWriter(&contentBuf);
    writers.content = &contentWriter;
    QBuffer bodyBuf;
    KoXmlWriter bodyWriter(&bodyBuf);
    writers.body = &bodyWriter;

    // open main tags
    bodyWriter.startElement("office:body");
    bodyWriter.startElement(d->bodyContentElement.constData());

    RETURN_IF_ERROR(createDocument(outputStore.get(), &writers))

    // save the office:automatic-styles & and fonts in content.xml
    mainStyles.saveOdfStyles(KoGenStyles::FontFaceDecls, &contentWriter);
    mainStyles.saveOdfStyles(KoGenStyles::DocumentAutomaticStyles, &contentWriter);

    // close tags in body
    bodyWriter.endElement(); // office:*
    bodyWriter.endElement(); // office:body

    // now create real content/body writers & dump the information there
    KoXmlWriter *realContentWriter = oasisStore.contentWriter();
    if (!realContentWriter) {
        warnMsooXml << "Error creating the content writer.";
        return KoFilter::CreationError;
    }
    realContentWriter->addCompleteElement(&contentBuf);

    KoXmlWriter *realBodyWriter = oasisStore.bodyWriter();
    if (!realBodyWriter) {
        warnMsooXml << "Error creating the body writer.";
        return KoFilter::CreationError;
    }
    realBodyWriter->addCompleteElement(&bodyBuf);

    // now close content & body writers
    if (!oasisStore.closeContentWriter()) {
        warnMsooXml << "Error closing content.";
        return KoFilter::CreationError;
    }

    debugMsooXml << "closed content & body writers.";

    // create the manifest file
    KoXmlWriter *realManifestWriter = oasisStore.manifestWriter(to);
    // create the styles.xml file
    mainStyles.saveOdfStylesDotXml(outputStore.get(), realManifestWriter);
    realManifestWriter->addManifestEntry("content.xml", "text/xml");
    realManifestWriter->addCompleteElement(&manifestBuf);

    debugMsooXml << "created manifest and styles.xml";

    // create settings.xml, apparently it is used to note calligra that msoffice files should
    // have different behavior with some things
    if (!outputStore->open("settings.xml")) {
        return KoFilter::CreationError;
    }

    KoStoreDevice settingsDev(outputStore.get());
    KoXmlWriter *settings = KoOdfWriteStore::createOasisXmlWriter(&settingsDev, "office:document-settings");
    settings->startElement("office:settings");
    settings->startElement("config:config-item-set");
    settings->addAttribute("config:name", "ooo:configuration-settings");
    writeConfigurationSettings(settings);
    settings->endElement(); // config:config-item-set
    settings->endElement(); // office:settings
    settings->endElement(); // office:document-settings
    settings->endDocument();
    delete settings;
    realManifestWriter->addManifestEntry("settings.xml", "text/xml");
    if (!outputStore->close()) {
        return KoFilter::CreationError;
    }

    // create meta.xml
    if (!outputStore->open("meta.xml")) {
        return KoFilter::CreationError;
    }
    KoStoreDevice metaDev(outputStore.get());
    KoXmlWriter *meta = KoOdfWriteStore::createOasisXmlWriter(&metaDev, "office:document-meta");
    meta->startElement("office:meta");
    meta->addCompleteElement(&buf);
    meta->endElement(); // office:meta
    meta->endElement(); // office:document-meta
    meta->endDocument();
    delete meta;
    if (!outputStore->close()) {
        return KoFilter::CreationError;
    }
    realManifestWriter->addManifestEntry("meta.xml", "text/xml");
    oasisStore.closeManifestWriter();

    return KoFilter::OK;
}
