/* This file is part of the KDE project
   Copyright (C) 2011-2013 Yue Liu <yue.liu@mail.com>

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

#include <libvisio/libvisio.h>
#include <libodfgen/OdgGenerator.hxx>

#include <OutputWriter.h>
#include <KoStore.h>
#include <KoFilterChain.h>
#include <KoGlobal.h>
#include <KoOdf.h>

#include <kpluginfactory.h>

#include <QString>
#include <QByteArray>

#include <stdio.h>

class OdgOutputWriter : public OutputWriter
{
public:
    OdgOutputWriter(KoStore *outputStore) :
        OutputWriter(outputStore) {}
    ~OdgOutputWriter() {}

private:
    bool _isSupportedFormat(WPXInputStream *input)
    {
        if (!libvisio::VisioDocument::isSupported(input))
        {
            fprintf(stderr, "ERROR: We have no confidence that you are giving us a valid Visio Document.\n");
            return false;
        }
        return true;
    }

    bool _convertDocument(WPXInputStream *input, OdfDocumentHandler *handler, OdfStreamType streamType)
    {
        OdgGenerator exporter(handler, streamType);
        return libvisio::VisioDocument::parse(input, &exporter);
    }
};

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
    if (from != "application/vnd.visio" || to != KoOdf::mimeType(KoOdf::Graphics))
        return KoFilter::NotImplemented;

    const char manifestStr[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
            "<manifest:manifest xmlns:manifest=\"urn:oasis:names:tc:opendocument:xmlns:manifest:1.0\">"
            " <manifest:file-entry manifest:media-type=\"application/vnd.oasis.opendocument.graphics\" manifest:version=\"1.0\" manifest:full-path=\"/\"/>"
            " <manifest:file-entry manifest:media-type=\"text/xml\" manifest:full-path=\"content.xml\"/>"
            " <manifest:file-entry manifest:media-type=\"text/xml\" manifest:full-path=\"settings.xml\"/>"
            " <manifest:file-entry manifest:media-type=\"text/xml\" manifest:full-path=\"styles.xml\"/>"
            "</manifest:manifest>";


    QByteArray inputFile = m_chain->inputFile().toLocal8Bit();

    KoStore* outputStore = KoStore::createStore( m_chain->outputFile(), KoStore::Write,
                                                 KoOdf::mimeType(KoOdf::Graphics), KoStore::Zip );
    if (!outputStore)
        return KoFilter::CreationError;

    OdgOutputWriter writer(outputStore);

    if (!writer.writeChildFile("mimetype", KoOdf::mimeType(KoOdf::Graphics))) {
        fprintf(stderr, "ERROR : Couldn't write mimetype\n");
        delete outputStore;
        return KoFilter::ParsingError;
    }

    if (!writer.writeChildFile("META-INF/manifest.xml", manifestStr)) {
        fprintf(stderr, "ERROR : Couldn't write manifest\n");
        delete outputStore;
        return KoFilter::ParsingError;
    }

    if (!writer.writeConvertedContent("settings.xml", inputFile.data(), ODF_SETTINGS_XML))
    {
        fprintf(stderr, "ERROR : Couldn't write document settings\n");
        delete outputStore;
        return KoFilter::ParsingError;
    }

    if (!writer.writeConvertedContent("styles.xml", inputFile.data(), ODF_STYLES_XML))
    {
        fprintf(stderr, "ERROR : Couldn't write document styles\n");
        delete outputStore;
        return KoFilter::ParsingError;
    }

    if (!writer.writeConvertedContent("content.xml", inputFile.data(), ODF_CONTENT_XML))
    {
            fprintf(stderr, "ERROR : Couldn't write document content\n");
            delete outputStore;
            return KoFilter::ParsingError;
    }

    delete outputStore;
    return KoFilter::OK;
}
