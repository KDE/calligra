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

class OdgOutputFileHelper : public OutputFileHelper
{
public:
    OdgOutputFileHelper(const char *outFileName, const char *password) :
        OutputFileHelper(outFileName, password) {}
    ~OdgOutputFileHelper() override {}

    bool convertDocument(librevenge::RVNGInputStream &input, bool isFlat)
    {
        OdgGenerator collector;
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
            if (!libwpg::WPGraphics::parse(&input, &collector))
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

        const char s_mimetypeStr[] = "application/vnd.oasis.opendocument.graphics";
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
    bool isSupportedFormat(librevenge::RVNGInputStream &input)
    {
        try
        {
            return libwpg::WPGraphics::isSupported(&input);
        }
        catch (...)
        {
            return false;
        }
    }
};

K_PLUGIN_FACTORY_WITH_JSON(VSDXImportFactory, "calligra_filter_wpg2odg.json", registerPlugin<WPGImport>();)

WPGImport::WPGImport(QObject* parent, const QVariantList&)
        : KoFilter(parent)
{
}

WPGImport::~WPGImport()
{
}

KoFilter::ConversionStatus WPGImport::convert(const QByteArray& from, const QByteArray& to)
{
    if (from != QByteArrayLiteral("application/x-wpg") || to != KoOdf::mimeType(KoOdf::Graphics))
        return KoFilter::NotImplemented;

    QByteArray inputFile = m_chain->inputFile().toLocal8Bit();
    QByteArray outputFile = m_chain->outputFile().toLocal8Bit();

    OdgOutputFileHelper helper(outputFile.constData(), nullptr);
    librevenge::RVNGFileStream input(inputFile.constData());
    if (!helper.isSupportedFormat(input))
    {
        fprintf(stderr, "ERROR: We have no confidence that you are giving us a valid WordPerfect Graphics.\n");
        return KoFilter::ParsingError;
    }

    if (!helper.convertDocument(input, outputFile.constData()==nullptr))
    {
        fprintf(stderr, "ERROR : Couldn't write convert the document\n");
        return KoFilter::ParsingError;
    }

    return KoFilter::OK;
}

#include "WPGImport.moc"
