/* This file is part of the KDE project
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

#include <libvisio/libvisio.h>

#include <OutputFileHelper.hxx>
#include <OdgGenerator.hxx>
#include <KoFilterChain.h>
#include <KoGlobal.h>
#include <KoOdf.h>

#include <kpluginfactory.h>

#include <QString>
#include <QByteArray>

#include <stdio.h>

class OdgOutputFileHelper : public OutputFileHelper
{
public:
    OdgOutputFileHelper(const char *outFileName, const char *password) :
        OutputFileHelper(outFileName, password) {}
    ~OdgOutputFileHelper() {}

private:
    bool _isSupportedFormat(WPXInputStream *input, const char * /* password */)
    {
        if (!libvisio::VisioDocument::isSupported(input))
        {
            fprintf(stderr, "ERROR: We have no confidence that you are giving us a valid Visio Document.\n");
            return false;
        }
        return true;
    }

    bool _convertDocument(WPXInputStream *input, const char * /* password */, OdfDocumentHandler *handler, OdfStreamType streamType)
    {
        OdgGenerator exporter(handler, streamType);
        return libvisio::VisioDocument::parse(input, &exporter);
    }
};

K_PLUGIN_FACTORY(VSDXImportFactory, registerPlugin<VSDXImport>();)
//K_EXPORT_PLUGIN(VSDXImportFactory("calligrafilters"))

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

    const char mimetypeStr[] = "application/vnd.oasis.opendocument.graphics";

    const char manifestStr[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
            "<manifest:manifest xmlns:manifest=\"urn:oasis:names:tc:opendocument:xmlns:manifest:1.0\">"
            " <manifest:file-entry manifest:media-type=\"application/vnd.oasis.opendocument.graphics\" manifest:version=\"1.0\" manifest:full-path=\"/\"/>"
            " <manifest:file-entry manifest:media-type=\"text/xml\" manifest:full-path=\"content.xml\"/>"
            " <manifest:file-entry manifest:media-type=\"text/xml\" manifest:full-path=\"settings.xml\"/>"
            " <manifest:file-entry manifest:media-type=\"text/xml\" manifest:full-path=\"styles.xml\"/>"
            "</manifest:manifest>";


    QByteArray input = m_chain->inputFile().toLocal8Bit();
    m_inputFile = input.data();
    QByteArray output = m_chain->outputFile().toLocal8Bit();
    m_outputFile = output.data();

    OdgOutputFileHelper helper(m_outputFile, 0);

    if (!helper.writeChildFile("mimetype", mimetypeStr, (char)0)) {
        fprintf(stderr, "ERROR : Couldn't write mimetype\n");
        return KoFilter::ParsingError;
    }

    if (!helper.writeChildFile("META-INF/manifest.xml", manifestStr)) {
        fprintf(stderr, "ERROR : Couldn't write manifest\n");
        return KoFilter::ParsingError;
    }

    if (m_outputFile && !helper.writeConvertedContent("settings.xml", m_inputFile, ODF_SETTINGS_XML))
    {
        fprintf(stderr, "ERROR : Couldn't write document settings\n");
        return KoFilter::ParsingError;
    }

    if (m_outputFile && !helper.writeConvertedContent("styles.xml", m_inputFile, ODF_STYLES_XML))
    {
        fprintf(stderr, "ERROR : Couldn't write document styles\n");
        return KoFilter::ParsingError;
    }

    if (!helper.writeConvertedContent("content.xml", m_inputFile, m_outputFile ? ODF_CONTENT_XML : ODF_FLAT_XML))
    {
            fprintf(stderr, "ERROR : Couldn't write document content\n");
            return KoFilter::ParsingError;
    }

    return KoFilter::OK;
}
