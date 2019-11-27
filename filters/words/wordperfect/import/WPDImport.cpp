/* This file is part of the KDE project
 *
 * Based on writerperfect code, major Contributor(s):
 * Copyright (C) 2002-2004 William Lachance (wrlach@gmail.com)
 * Copyright (C) 2004-2006 Fridrich Strba (fridrich.strba@bluewin.ch)
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

#include "WPDImport.h"

#include <libwpd/libwpd.h>
#include <libwpg/libwpg.h>
#include <libodfgen/libodfgen.hxx>

#include <writerperfect_utils.hxx>
#include <OutputFileHelper.hxx>
#include <StringDocumentHandler.hxx>

#include <KoFilterChain.h>
#include <KoOdf.h>

#include <kpluginfactory.h>

#include <QString>
#include <QByteArray>

#include <cassert>
#include <stdio.h>

using namespace libwpd;

class OdtOutputFileHelper : public OutputFileHelper
{
public:
    OdtOutputFileHelper(const char *outFileName,const char *password) :
        OutputFileHelper(outFileName, password) {};
    ~OdtOutputFileHelper() override {};

    bool convertDocument(librevenge::RVNGInputStream &input, const char *password, bool isFlat)
    {
        OdtGenerator collector;
        collector.registerEmbeddedObjectHandler("image/x-wpg", &handleEmbeddedWPGObject);
        collector.registerEmbeddedImageHandler("image/x-wpg", &handleEmbeddedWPGImage);
        StringDocumentHandler stylesHandler, contentHandler, manifestHandler, metaHandler;
        if (isFlat)
            collector.addDocumentHandler(&contentHandler, ODF_FLAT_XML);
        else
        {
            collector.addDocumentHandler(&contentHandler, ODF_CONTENT_XML);
            collector.addDocumentHandler(&manifestHandler, ODF_MANIFEST_XML);
            collector.addDocumentHandler(&metaHandler, ODF_META_XML);
            collector.addDocumentHandler(&stylesHandler, ODF_STYLES_XML);
        }
        try
        {
            if (WPD_OK != WPDocument::parse(&input, &collector, password))
                return false;
        }
        catch (...)
        {
            return false;
        }
        if (isFlat)
        {
            printf("%s\n", contentHandler.cstr());
            return true;
        }

        static const char s_mimetypeStr[] = "application/vnd.oasis.opendocument.text";
        if (!writeChildFile("mimetype", s_mimetypeStr, (char)0) ||
                !writeChildFile("META-INF/manifest.xml", manifestHandler.cstr()) ||
                !writeChildFile("content.xml", contentHandler.cstr()) ||
                !writeChildFile("meta.xml", metaHandler.cstr()) ||
                !writeChildFile("styles.xml", stylesHandler.cstr()))
            return false;

        librevenge::RVNGStringVector objects=collector.getObjectNames();
        for (unsigned i=0; i<objects.size(); ++i)
        {
            StringDocumentHandler objectHandler;
            if (collector.getObjectContent(objects[i], &objectHandler))
                writeChildFile(objects[i].cstr(), objectHandler.cstr());
        }
        return true;
    }


    bool isSupportedFormat(librevenge::RVNGInputStream &input, const char *password)
    {
        try
        {
            WPDConfidence confidence = WPDocument::isFileFormatSupported(&input);
            if (WPD_CONFIDENCE_EXCELLENT != confidence && WPD_CONFIDENCE_SUPPORTED_ENCRYPTION != confidence)
            {
                fprintf(stderr, "ERROR: We have no confidence that you are giving us a valid WordPerfect document.\n");
                return false;
            }
            if (WPD_CONFIDENCE_SUPPORTED_ENCRYPTION == confidence && !password)
            {
                fprintf(stderr, "ERROR: The WordPerfect document is encrypted and you did not give us a password.\n");
                return false;
            }
            if (confidence == WPD_CONFIDENCE_SUPPORTED_ENCRYPTION && password &&
                    (WPD_PASSWORD_MATCH_OK != WPDocument::verifyPassword(&input, password)))
            {
                fprintf(stderr, "ERROR: The WordPerfect document is encrypted and we either\n");
                fprintf(stderr, "ERROR: don't know how to decrypt it or the given password is wrong.\n");
                return false;
            }
        }
        catch (...)
        {
            fprintf(stderr, "ERROR: We have no confidence that you are giving us a valid WordPerfect document.\n");
            return false;
        }

        return true;
    }

private:

    static bool handleEmbeddedWPGObject(const librevenge::RVNGBinaryData &data, OdfDocumentHandler *pHandler,  const OdfStreamType streamType)
    {
        OdgGenerator exporter;
        exporter.addDocumentHandler(pHandler, streamType);

        libwpg::WPGFileFormat fileFormat = libwpg::WPG_AUTODETECT;

        if (!libwpg::WPGraphics::isSupported(const_cast<librevenge::RVNGInputStream *>(data.getDataStream())))
            fileFormat = libwpg::WPG_WPG1;

        return libwpg::WPGraphics::parse(const_cast<librevenge::RVNGInputStream *>(data.getDataStream()), &exporter, fileFormat);
    }

    static bool handleEmbeddedWPGImage(const librevenge::RVNGBinaryData &input, librevenge::RVNGBinaryData &output)
    {
        libwpg::WPGFileFormat fileFormat = libwpg::WPG_AUTODETECT;

        if (!libwpg::WPGraphics::isSupported(const_cast<librevenge::RVNGInputStream *>(input.getDataStream())))
            fileFormat = libwpg::WPG_WPG1;

        librevenge::RVNGStringVector svgOutput;
        librevenge::RVNGSVGDrawingGenerator generator(svgOutput, "");
        bool result = libwpg::WPGraphics::parse(const_cast<librevenge::RVNGInputStream *>(input.getDataStream()), &generator, fileFormat);
        if (!result || svgOutput.empty() || svgOutput[0].empty())
            return false;

        output.clear();
        const char *svgHeader = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
                                "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\""
                                " \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n";
        output.append((unsigned char *)svgHeader, strlen(svgHeader));
        output.append((unsigned char *)svgOutput[0].cstr(), strlen(svgOutput[0].cstr()));

        return true;
    }
};

K_PLUGIN_FACTORY_WITH_JSON(WPDImportFactory, "calligra_filter_wpd2odt.json",registerPlugin<WPDImport>();)

WPDImport::WPDImport(QObject* parent, const QVariantList&)
    : KoFilter(parent)
{
}

WPDImport::~WPDImport()
{
}

KoFilter::ConversionStatus WPDImport::convert(const QByteArray& from, const QByteArray& to)
{
    if (from != "application/vnd.wordperfect" || to != KoOdf::mimeType(KoOdf::Text))
        return KoFilter::NotImplemented;

    QByteArray inputFile = m_chain->inputFile().toLocal8Bit();
    QByteArray outputFile = m_chain->outputFile().toLocal8Bit();
    const char* password = 0;

    OdtOutputFileHelper helper(outputFile.constData(), 0);
    librevenge::RVNGFileStream input(inputFile.constData());
    if (!helper.isSupportedFormat(input, password))
        return KoFilter::ParsingError;

    if (!helper.convertDocument(input, password, false))
    {
        fprintf(stderr, "ERROR : Couldn't convert the document\n");
        return KoFilter::ParsingError;
    }

    return KoFilter::OK;
}

#include "WPDImport.moc"
