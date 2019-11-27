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

#include <writerperfect_utils.hxx>
#include <OutputFileHelper.hxx>
#include <StringDocumentHandler.hxx>

#include <KoFilterChain.h>
#include <KoOdf.h>

#include <kpluginfactory.h>

#include <QString>
#include <QByteArray>

#include <stdio.h>

using libetonyek::EtonyekDocument;

class OdpOutputFileHelper : public OutputFileHelper
{
public:
    OdpOutputFileHelper(const char *outFileName, const char *password) :
        OutputFileHelper(outFileName, password) {}
    ~OdpOutputFileHelper() override {}

    bool convertDocument(librevenge::RVNGInputStream &input, bool isFlat)
    {
        OdpGenerator collector;
        StringDocumentHandler stylesHandler, contentHandler, manifestHandler, settingsHandler;
        if (isFlat)
            collector.addDocumentHandler(&contentHandler, ODF_FLAT_XML);
        else
        {
            collector.addDocumentHandler(&contentHandler, ODF_CONTENT_XML);
            collector.addDocumentHandler(&manifestHandler, ODF_MANIFEST_XML);
            collector.addDocumentHandler(&settingsHandler, ODF_SETTINGS_XML);
            collector.addDocumentHandler(&stylesHandler, ODF_STYLES_XML);
        }
        try
        {
            if (!EtonyekDocument::parse(&input, &collector))
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

        const char s_mimetypeStr[] = "application/vnd.oasis.opendocument.presentation";
        if (!writeChildFile("mimetype", s_mimetypeStr, (char)0) ||
                !writeChildFile("META-INF/manifest.xml", manifestHandler.cstr()) ||
                !writeChildFile("content.xml", contentHandler.cstr()) ||
                !writeChildFile("settings.xml", settingsHandler.cstr()) ||
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

    static EtonyekDocument::Confidence isSupportedFormat(librevenge::RVNGInputStream &input, EtonyekDocument::Type *type = 0)
    {
        return EtonyekDocument::isSupported(&input, type);
    }
};


K_PLUGIN_FACTORY_WITH_JSON(KeyImportFactory, "calligra_filter_key2odp.json", registerPlugin<KeyImport>();)

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

    QByteArray inputFile = m_chain->inputFile().toLocal8Bit();
    QByteArray outputFile = m_chain->outputFile().toLocal8Bit();

    OdpOutputFileHelper helper(outputFile.constData(), 0);

    shared_ptr<librevenge::RVNGInputStream> input;
    if (librevenge::RVNGDirectoryStream::isDirectory(inputFile.constData()))
        input.reset(new librevenge::RVNGDirectoryStream(inputFile.constData()));
    else
        input.reset(new librevenge::RVNGFileStream(inputFile.constData()));

    EtonyekDocument::Type type = EtonyekDocument::TYPE_UNKNOWN;
    const EtonyekDocument::Confidence confidence = helper.isSupportedFormat(*input, &type);
    if ((EtonyekDocument::CONFIDENCE_NONE == confidence) || (EtonyekDocument::TYPE_KEYNOTE != type))
    {
        fprintf(stderr, "ERROR: We have no confidence that you are giving us a valid Keynote Document.\n");
        return KoFilter::ParsingError;
    }

    if (EtonyekDocument::CONFIDENCE_SUPPORTED_PART == confidence)
    {
        input.reset(librevenge::RVNGDirectoryStream::createForParent(inputFile.constData()));

        if (EtonyekDocument::CONFIDENCE_EXCELLENT != helper.isSupportedFormat(*input))
        {
            fprintf(stderr, "ERROR: We have no confidence that you are giving us a valid Keynote Document.\n");
            return KoFilter::ParsingError;
        }
    }

    if (!helper.convertDocument(*input, outputFile.constData()))
    {
        fprintf(stderr, "ERROR : Couldn't convert the document\n");
        return KoFilter::ParsingError;
    }

    return KoFilter::OK;
}

#include "KeyImport.moc"
