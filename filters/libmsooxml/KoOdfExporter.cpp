/* This file is part of the KDE project
   Copyright (C) 2007 Thorsten Zachmann <zachmann@kde.org>
   Copyright (C) 2009 Jarosław Staniek <staniek@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KoOdfExporter.h"
#include "MsooXmlUtils.h"

#include <QBuffer>
#include <QByteArray>

#include "MsooXmlDebug.h"

#include <KoOdfWriteStore.h>
#include <KoStoreDevice.h>
#include <KoFilterChain.h>
#include <KoGenStyles.h>
#include <KoXmlWriter.h>

#include <memory>

KoOdfWriters::KoOdfWriters()
        : content(0), body(0), meta(0), manifest(0), mainStyles(0)
{
}

//------------------------------------------

class Q_DECL_HIDDEN KoOdfExporter::Private
{
public:
    Private() {}
    QByteArray bodyContentElement;
};

//------------------------------------------

KoOdfExporter::KoOdfExporter(const QString& bodyContentElement, QObject* parent)
        : KoFilter(parent)
        , d(new Private)
{
    d->bodyContentElement = QByteArray("office:") + bodyContentElement.toLatin1();
}

KoOdfExporter::~KoOdfExporter()
{
    delete d;
}

KoFilter::ConversionStatus KoOdfExporter::convert(const QByteArray& from, const QByteArray& to)
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

    //create output files
    KoStore *outputStore = KoStore::createStore(m_chain->outputFile(), KoStore::Write, to, KoStore::Zip);
    if (!outputStore || outputStore->bad()) {
        warnMsooXml << "Unable to open output file!";
        delete outputStore;
        return KoFilter::FileNotFound;
    }
    debugMsooXml << "created outputStore.";
    KoOdfWriteStore oasisStore(outputStore);

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

    //open contentWriter & bodyWriter *temp* writers
    //so we can write picture files while we parse
    QBuffer contentBuf;
    KoXmlWriter contentWriter(&contentBuf);
    writers.content = &contentWriter;
    QBuffer bodyBuf;
    KoXmlWriter bodyWriter(&bodyBuf);
    writers.body = &bodyWriter;

    // open main tags
    bodyWriter.startElement("office:body");
    bodyWriter.startElement(d->bodyContentElement.constData());

    RETURN_IF_ERROR( createDocument(outputStore, &writers) )

    //save the office:automatic-styles & and fonts in content.xml
    mainStyles.saveOdfStyles(KoGenStyles::FontFaceDecls, &contentWriter);
    mainStyles.saveOdfStyles(KoGenStyles::DocumentAutomaticStyles, &contentWriter);

    //close tags in body
    bodyWriter.endElement();//office:*
    bodyWriter.endElement();//office:body

    //now create real content/body writers & dump the information there
    KoXmlWriter* realContentWriter = oasisStore.contentWriter();
    if (!realContentWriter) {
        warnMsooXml << "Error creating the content writer.";
        delete outputStore;
        return KoFilter::CreationError;
    }
    realContentWriter->addCompleteElement(&contentBuf);

    KoXmlWriter* realBodyWriter = oasisStore.bodyWriter();
    if (!realBodyWriter) {
        warnMsooXml << "Error creating the body writer.";
        delete outputStore;
        return KoFilter::CreationError;
    }
    realBodyWriter->addCompleteElement(&bodyBuf);

    //now close content & body writers
    if (!oasisStore.closeContentWriter()) {
        warnMsooXml << "Error closing content.";
        delete outputStore;
        return KoFilter::CreationError;
    }

    debugMsooXml << "closed content & body writers.";

    //create the manifest file
    KoXmlWriter* realManifestWriter = oasisStore.manifestWriter(to);
    //create the styles.xml file
    mainStyles.saveOdfStylesDotXml(outputStore, realManifestWriter);
    realManifestWriter->addManifestEntry("content.xml", "text/xml");
    realManifestWriter->addCompleteElement(&manifestBuf);

    debugMsooXml << "created manifest and styles.xml";

    // create settings.xml, apparently it is used to note calligra that msoffice files should
    // have different behavior with some things
    if (!outputStore->open("settings.xml")) {
        delete outputStore;
        return KoFilter::CreationError;
    }

    KoStoreDevice settingsDev(outputStore);
    KoXmlWriter* settings = KoOdfWriteStore::createOasisXmlWriter(&settingsDev, "office:document-settings");
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
        delete outputStore;
        return KoFilter::CreationError;
    }

    //create meta.xml
    if (!outputStore->open("meta.xml")) {
        delete outputStore;
        return KoFilter::CreationError;
    }
    KoStoreDevice metaDev(outputStore);
    KoXmlWriter* meta = KoOdfWriteStore::createOasisXmlWriter(&metaDev, "office:document-meta");
    meta->startElement("office:meta");
    meta->addCompleteElement(&buf);
    meta->endElement(); //office:meta
    meta->endElement(); //office:document-meta
    meta->endDocument();
    delete meta;
    if (!outputStore->close()) {
        delete outputStore;
        return KoFilter::CreationError;
    }
    realManifestWriter->addManifestEntry("meta.xml", "text/xml");
    oasisStore.closeManifestWriter();
    delete outputStore;

    return KoFilter::OK;
}
