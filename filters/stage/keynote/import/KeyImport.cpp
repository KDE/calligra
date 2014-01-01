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

#include "KeyImport.h"

#include <libetonyek/libetonyek.h>
#include <libodfgen/OdpGenerator.hxx>

#include "OutputFileHelper.hxx"
#include <KoFilterChain.h>
#include <KoGlobal.h>
#include <KoOdf.h>

#include <kpluginfactory.h>

#include <QString>
#include <QByteArray>

#include <stdio.h>

class OdpOutputFileHelper : public OutputFileHelper
{
public:
    OdpOutputFileHelper(const char *outFileName, const char *password) :
        OutputFileHelper(outFileName, password) {}
    ~OdpOutputFileHelper() {}

private:
    bool _isSupportedFormat(WPXInputStream *input, const char * /* password */)
    {
        if (!libetonyek::KEYDocument::isSupported(input))
        {
            fprintf(stderr, "ERROR: We have no confidence that you are giving us a valid Keynote Document.\n");
            return false;
        }
        return true;
    }

    bool _convertDocument(WPXInputStream *input, const char * /* password */, OdfDocumentHandler *handler, OdfStreamType streamType)
    {
        OdpGenerator exporter(handler, streamType);
        return libetonyek::KEYDocument::parse(input, &exporter);
    }
};


K_PLUGIN_FACTORY(KeyImportFactory, registerPlugin<KeyImport>();)
K_EXPORT_PLUGIN(KeyImportFactory("calligrafilters"))

KeyImport::KeyImport(QObject* parent, const QVariantList&)
        : KoFilter(parent)
{
}

KeyImport::~KeyImport()
{
}

KoFilter::ConversionStatus KeyImport::convert(const QByteArray& from, const QByteArray& to)
{
    if (from != "application/x-iwork-keynote-sffkey" || to != KoOdf::mimeType(KoOdf::Presentation))
        return KoFilter::NotImplemented;

    const char manifestStr[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                               "<manifest:manifest xmlns:manifest=\"urn:oasis:names:tc:opendocument:xmlns:manifest:1.0\">"
                               " <manifest:file-entry manifest:media-type=\"application/vnd.oasis.opendocument.presentation\" manifest:version=\"1.0\" manifest:full-path=\"/\"/>"
                               " <manifest:file-entry manifest:media-type=\"text/xml\" manifest:full-path=\"content.xml\"/>"
                               " <manifest:file-entry manifest:media-type=\"text/xml\" manifest:full-path=\"settings.xml\"/>"
                               " <manifest:file-entry manifest:media-type=\"text/xml\" manifest:full-path=\"styles.xml\"/>"
                               "</manifest:manifest>";


    QByteArray input = m_chain->inputFile().toLocal8Bit();
    QByteArray output = m_chain->outputFile().toLocal8Bit();
    const char *inputFile = input.data();
    const char *outputFile = output.data();

    OdpOutputFileHelper helper(outputFile, 0);

    if (!helper.writeChildFile("mimetype", KoOdf::mimeType(KoOdf::Presentation), (char)0)) {
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
