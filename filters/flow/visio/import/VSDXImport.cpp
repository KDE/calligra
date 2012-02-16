/* This file is part of the KDE project
   Copyright (C) 2011 Helder Cesar <heldercro@gmail.com>
   Copyright (C) 2011 Yue Liu <yue.liu@mail.com>

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
   Boston, MA 02110-1301, USA.
*/

#include "VSDXImport.h"
#include "OutputHandler.h"
#include "OdfDocumentHandler.hxx"
#include "OdgGenerator.hxx"

#include <KoFilterChain.h>
#include <KoGlobal.h>
#include <KoUnit.h>
#include <KoOdf.h>
#include <KoStore.h>

#include <kpluginfactory.h>
#include <KDebug>

#include <QIODevice>
#include <QtCore/QString>
#include <QtCore/QFile>

#include <libvisio/libvisio.h>
#include <libwpd-stream/libwpd-stream.h>
#include <iostream>

K_PLUGIN_FACTORY(VSDXImportFactory, registerPlugin<VSDXImport>();)
K_EXPORT_PLUGIN(VSDXImportFactory("calligrafilters"))

VSDXImport::VSDXImport(QObject* parent, const QVariantList&)
        : KoFilter(parent)
{
}

VSDXImport::~VSDXImport()
{
}

KoFilter::ConversionStatus VSDXImport::convert(const QByteArray& from, const QByteArray& to)
{
    //const char mimetypeStr[] = "application/vnd.oasis.opendocument.graphics";

    const char manifestStr[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
            "<manifest:manifest xmlns:manifest=\"urn:oasis:names:tc:opendocument:xmlns:manifest:1.0\">"
            " <manifest:file-entry manifest:media-type=\"application/vnd.oasis.opendocument.graphics\" manifest:version=\"1.0\" manifest:full-path=\"/\"/>"
            " <manifest:file-entry manifest:media-type=\"text/xml\" manifest:full-path=\"content.xml\"/>"
            " <manifest:file-entry manifest:media-type=\"text/xml\" manifest:full-path=\"settings.xml\"/>"
            " <manifest:file-entry manifest:media-type=\"text/xml\" manifest:full-path=\"styles.xml\"/>"
            "</manifest:manifest>";
    
    if (from != "application/vnd.visio" || to != KoOdf::mimeType(KoOdf::Graphics))
        return KoFilter::NotImplemented;

    WPXInputStream* input = new WPXFileStream(m_chain->inputFile().toLocal8Bit());
    if (input->isOLEStream()) {
        WPXInputStream* olestream = input->getDocumentOLEStream("Anything");
        if (olestream) {
            delete input;
            input = olestream;
        }
     }

    if (!libvisio::VisioDocument::isSupported(input)) {
        kWarning() << "ERROR: Unsupported file format (unsupported version) or file is encrypted!";
        delete input;
        return KoFilter::NotImplemented;
    }

    // create output store
    KoStore* output = KoStore::createStore(m_chain->outputFile(), KoStore::Write,
                        KoOdf::mimeType(KoOdf::Graphics), KoStore::Zip);
    if (!output) {
        return KoFilter::StorageCreationError;
    }
/*
    output->open("mimetype");
    output->write(QByteArray(mimetypeStr));
    output->close();
*/
    kDebug()<<output->currentPath();

    output->enterDirectory("META-INF");
    output->open("manifest.xml");
    output->write(QByteArray(manifestStr));
    output->close();
    output->leaveDirectory();

    kDebug()<<output->currentPath();

    writeConvertedContent(output, "settings.xml", input, ODF_SETTINGS_XML);
    kDebug()<<output;

    writeConvertedContent(output, "styles.xml", input, ODF_STYLES_XML);
    kDebug()<<output;

    writeConvertedContent(output, "content.xml", input, ODF_CONTENT_XML);
    kDebug()<<output;


    return KoFilter::OK;
}

bool VSDXImport::writeConvertedContent(KoStore *output, const char *childFileName, WPXInputStream* input, const OdfStreamType streamType)
{
    OutputHandler *handler = new OutputHandler();
    OdgGenerator exporter(handler, streamType);
    libvisio::VisioDocument::parse(input, &exporter);
    output->open(childFileName);
    output->write(handler->array());
    output->close();
    return true;
}
