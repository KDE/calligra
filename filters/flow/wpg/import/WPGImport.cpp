/* This file is part of the KDE project
 *
 * Based on writerperfect code, major Contributor(s):
 * Copyright (C) 2006 Ariya Hidayat (ariya@kde.org)
 * Copyright (C) 2006-2007 Fridrich Strba (fridrich.strba@bluewin.ch)
 *
 * For minor contributions see the writerperfect git repository.
 *
 * The contents of this file may be used under the terms
 * of the GNU Lesser General Public License Version 2.1 or later
 * (LGPLv2.1+), in which case the provisions of the LGPLv2.1+ are
 * applicable instead of those above.
 *
 * For further information visit http://libwpd.sourceforge.net
 */

#include "WPGImport.h"

#include <libwpg/libwpg.h>
#include <libodfgen/OdgGenerator.hxx>

#include "OutputFileHelper.hxx"
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
        if (!libwpg::WPGraphics::isSupported(input))
        {
            fprintf(stderr, "ERROR: We have no confidence that you are giving us a valid WordPerfect Graphics.\n");
            return false;
        }
        return true;
    }

    bool _convertDocument(WPXInputStream *input, const char * /* password */, OdfDocumentHandler *handler, OdfStreamType streamType)
    {
        OdgGenerator exporter(handler, streamType);
        return libwpg::WPGraphics::parse(input, &exporter);
    }
};

K_PLUGIN_FACTORY(VSDXImportFactory, registerPlugin<WPGImport>();)
K_EXPORT_PLUGIN(VSDXImportFactory("calligrafilters"))

WPGImport::WPGImport(QObject* parent, const QVariantList&)
        : KoFilter(parent)
{
}

WPGImport::~WPGImport()
{
}

KoFilter::ConversionStatus WPGImport::convert(const QByteArray& from, const QByteArray& to)
{
    if (from != "application/x-wpg" || to != KoOdf::mimeType(KoOdf::Graphics))
        return KoFilter::NotImplemented;

    const char manifestStr[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
            "<manifest:manifest xmlns:manifest=\"urn:oasis:names:tc:opendocument:xmlns:manifest:1.0\">"
            " <manifest:file-entry manifest:media-type=\"application/vnd.oasis.opendocument.graphics\" manifest:version=\"1.0\" manifest:full-path=\"/\"/>"
            " <manifest:file-entry manifest:media-type=\"text/xml\" manifest:full-path=\"content.xml\"/>"
            " <manifest:file-entry manifest:media-type=\"text/xml\" manifest:full-path=\"settings.xml\"/>"
            " <manifest:file-entry manifest:media-type=\"text/xml\" manifest:full-path=\"styles.xml\"/>"
            "</manifest:manifest>";


    QByteArray input = m_chain->inputFile().toLocal8Bit();
    QByteArray output = m_chain->outputFile().toLocal8Bit();
    const char *inputFile = input.data();
    const char *outputFile = output.data();

    OdgOutputFileHelper helper(outputFile, 0);

    if (!helper.writeChildFile("mimetype", KoOdf::mimeType(KoOdf::Graphics), (char)0)) {
        fprintf(stderr, "ERROR : Couldn't write mimetype\n");
        return KoFilter::ParsingError;
    }

    if (!helper.writeChildFile("META-INF/manifest.xml", manifestStr)) {
        fprintf(stderr, "ERROR : Couldn't write manifest\n");
        return KoFilter::ParsingError;
    }

    if (outputFile && !helper.writeConvertedContent("settings.xml", inputFile, ODF_SETTINGS_XML))
    {
        fprintf(stderr, "ERROR : Couldn't write document settings\n");
        return KoFilter::ParsingError;
    }

    if (outputFile && !helper.writeConvertedContent("styles.xml", inputFile, ODF_STYLES_XML))
    {
        fprintf(stderr, "ERROR : Couldn't write document styles\n");
        return KoFilter::ParsingError;
    }

    if (!helper.writeConvertedContent("content.xml", inputFile, ODF_CONTENT_XML))
    {
            fprintf(stderr, "ERROR : Couldn't write document content\n");
            return KoFilter::ParsingError;
    }

    return KoFilter::OK;
}
