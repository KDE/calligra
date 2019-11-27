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

#include "WPSImport.h"

#include <librevenge/librevenge.h>
#include <libwps/libwps.h>
#include <libodfgen/libodfgen.hxx>

#include <writerperfect_utils.hxx>
#include <OutputFileHelper.hxx>
#include <StringDocumentHandler.hxx>

#include <KoFilterChain.h>
#include <KoOdf.h>

#include <kpluginfactory.h>

#include <QString>
#include <QByteArray>

#include <stdio.h>

using namespace libwps;

class OdtOutputFileHelper : public OutputFileHelper
{
public:
    OdtOutputFileHelper(const char *outFileName,const char *password) :
        OutputFileHelper(outFileName, password) {};
    ~OdtOutputFileHelper() override {};
    bool convertDocument(librevenge::RVNGInputStream &input, bool isFlat)
    {
        OdtGenerator collector;
        collector.registerEmbeddedObjectHandler("image/wks-ods", &handleEmbeddedWKSObject);
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
            if (WPS_OK != WPSDocument::parse(&input, &collector))
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
    bool isSupportedFormat(librevenge::RVNGInputStream &input)
    {
        WPSKind kind = WPS_TEXT;
        WPSCreator creator = WPS_MSWORKS;
        bool needCharSetEncoding = false;
        WPSConfidence confidence = WPSDocument::isFileFormatSupported(&input, kind, creator, needCharSetEncoding);
        if (confidence == WPS_CONFIDENCE_NONE || kind != WPS_TEXT)
            return false;
        // TODO: handle needCharSetEncoding, for now falling to Western Europe encoding used as default
        return true;
    }

private:

    static bool handleEmbeddedWKSObject(const librevenge::RVNGBinaryData &data, OdfDocumentHandler *pHandler,  const OdfStreamType streamType)
    {
        if (!data.size()) return false;
        OdsGenerator exporter;
        exporter.addDocumentHandler(pHandler, streamType);
        return WPSDocument::parse(const_cast<librevenge::RVNGInputStream *>(data.getDataStream()), &exporter)==WPS_OK;
    }
};

K_PLUGIN_FACTORY_WITH_JSON(WPSImportFactory, "calligra_filter_wps2odt.json", registerPlugin<WPSImport>();)

WPSImport::WPSImport(QObject* parent, const QVariantList&)
        : KoFilter(parent)
{
}

WPSImport::~WPSImport()
{
}

KoFilter::ConversionStatus WPSImport::convert(const QByteArray& from, const QByteArray& to)
{
    if (from != "application/vnd.ms-works" || to != KoOdf::mimeType(KoOdf::Text))
        return KoFilter::NotImplemented;

    QByteArray inputFile = m_chain->inputFile().toLocal8Bit();
    QByteArray outputFile = m_chain->outputFile().toLocal8Bit();

    OdtOutputFileHelper helper(outputFile.constData(), 0);
    librevenge::RVNGFileStream input(inputFile.constData());
    if (!helper.isSupportedFormat(input))
    {
        fprintf(stderr, "ERROR: We have no confidence that you are giving us a valid Microsoft Works document.\n");
        return KoFilter::ParsingError;
    }

    if (!helper.convertDocument(input, false))
    {
        fprintf(stderr, "ERROR : Couldn't convert the document\n");
        return KoFilter::ParsingError;
    }

    return KoFilter::OK;
}

#include "WPSImport.moc"
