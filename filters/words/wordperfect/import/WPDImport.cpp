/* This file is part of the KDE project
   Copyright (C) 2012 Yue Liu <yue.liu@mail.com>

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

#include "WPDImport.h"

#include <libwpd/libwpd.h>
#define USE_LIBWPG
#include <libwpg/libwpg.h>

#include <OutputFileHelper.hxx>
#include <OdtGenerator.hxx>
#ifdef USE_LIBWPG
#include <OdgGenerator.hxx>
#endif
#include <KoFilterChain.h>
#include <KoGlobal.h>
#include <KoOdf.h>

#include <kpluginfactory.h>

#include <QString>
#include <QByteArray>

#include <stdio.h>

class OdtOutputFileHelper : public OutputFileHelper
{
public:
    OdtOutputFileHelper(const char *outFileName,const char *password) :
        OutputFileHelper(outFileName, password) {};
    ~OdtOutputFileHelper() {};

private:
    bool _isSupportedFormat(WPXInputStream *input, const char * password)
    {
        WPDConfidence confidence = WPDocument::isFileFormatSupported(input);
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
        if (confidence == WPD_CONFIDENCE_SUPPORTED_ENCRYPTION && password && (WPD_PASSWORD_MATCH_OK != WPDocument::verifyPassword(input, password)))
        {
            fprintf(stderr, "ERROR: The WordPerfect document is encrypted and we either\n");
            fprintf(stderr, "ERROR: don't know how to decrypt it or the given password is wrong.\n");
            return false;
        }

        return true;
    }

    static bool handleEmbeddedWPG(const WPXBinaryData &data, OdfDocumentHandler *pHandler,  const OdfStreamType streamType)
    {
#ifdef USE_LIBWPG
        OdgGenerator exporter(pHandler, streamType);

        libwpg::WPGFileFormat fileFormat = libwpg::WPG_AUTODETECT;

        if (!libwpg::WPGraphics::isSupported(const_cast<WPXInputStream *>(data.getDataStream())))
            fileFormat = libwpg::WPG_WPG1;

        return libwpg::WPGraphics::parse(const_cast<WPXInputStream *>(data.getDataStream()), &exporter, fileFormat);
#else
        return true;
#endif
    }

    bool _convertDocument(WPXInputStream *input, const char *password, OdfDocumentHandler *handler, const OdfStreamType streamType)
    {
        OdtGenerator collector(handler, streamType);
        collector.registerEmbeddedObjectHandler("image/x-wpg", &handleEmbeddedWPG);
        if (WPD_OK == WPDocument::parse(input, &collector, password))
            return true;
        return false;
    }
};

K_PLUGIN_FACTORY_WITH_JSON(WPDImportFactory, "calligra_filter_wpd2odt.json", registerPlugin<WPDImport>();)
//K_EXPORT_PLUGIN(WPDImportFactory("calligrafilters"))

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

    const char mimetypeStr[] = "application/vnd.oasis.opendocument.text";

    const char manifestStr[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
            "<manifest:manifest xmlns:manifest=\"urn:oasis:names:tc:opendocument:xmlns:manifest:1.0\">"
            " <manifest:file-entry manifest:media-type=\"application/vnd.oasis.opendocument.text\" manifest:full-path=\"/\"/>"
            " <manifest:file-entry manifest:media-type=\"text/xml\" manifest:full-path=\"content.xml\"/>"
            " <manifest:file-entry manifest:media-type=\"text/xml\" manifest:full-path=\"styles.xml\"/>"
            "</manifest:manifest>";

    const char stylesStr[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
            "<office:document-styles xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\" "
            "xmlns:style=\"urn:oasis:names:tc:opendocument:xmlns:style:1.0\" xmlns:text=\"urn:oasis:names:tc:opendocument:xmlns:text:1.0\" "
            "xmlns:table=\"urn:oasis:names:tc:opendocument:xmlns:table:1.0\" xmlns:draw=\"urn:oasis:names:tc:opendocument:xmlns:drawing:1.0\" "
            "xmlns:fo=\"urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" "
            "xmlns:number=\"urn:oasis:names:tc:opendocument:xmlns:datastyle:1.0\" "
            "xmlns:svg=\"urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0\" "
            "xmlns:chart=\"urn:oasis:names:tc:opendocument:xmlns:chart:1.0\" xmlns:dr3d=\"urn:oasis:names:tc:opendocument:xmlns:dr3d:1.0\" "
            "xmlns:form=\"urn:oasis:names:tc:opendocument:xmlns:form:1.0\" xmlns:script=\"urn:oasis:names:tc:opendocument:xmlns:script:1.0\">"
            "<office:styles>"
            "<style:default-style style:family=\"paragraph\">"
            "<style:paragraph-properties style:use-window-font-color=\"true\" style:text-autospace=\"ideograph-alpha\" "
            "style:punctuation-wrap=\"hanging\" style:line-break=\"strict\" style:writing-mode=\"page\"/>"
            "</style:default-style>"
            "<style:default-style style:family=\"table\"/>"
            "<style:default-style style:family=\"table-row\">"
            "<style:table-row-properties fo:keep-together=\"auto\"/>"
            "</style:default-style>"
            "<style:default-style style:family=\"table-column\"/>"
            "<style:style style:name=\"Standard\" style:family=\"paragraph\" style:class=\"text\"/>"
            "<style:style style:name=\"Text_body\" style:display-name=\"Text body\" style:family=\"paragraph\" "
            "style:parent-style-name=\"Standard\" style:class=\"text\"/>"
            "<style:style style:name=\"List\" style:family=\"paragraph\" style:parent-style-name=\"Text_body\" style:class=\"list\"/>"
            "<style:style style:name=\"Header\" style:family=\"paragraph\" style:parent-style-name=\"Standard\" style:class=\"extra\"/>"
            "<style:style style:name=\"Footer\" style:family=\"paragraph\" style:parent-style-name=\"Standard\" style:class=\"extra\"/>"
            "<style:style style:name=\"Caption\" style:family=\"paragraph\" style:parent-style-name=\"Standard\" style:class=\"extra\"/>"
            "<style:style style:name=\"Footnote\" style:family=\"paragraph\" style:parent-style-name=\"Standard\" style:class=\"extra\"/>"
            "<style:style style:name=\"Endnote\" style:family=\"paragraph\" style:parent-style-name=\"Standard\" style:class=\"extra\"/>"
            "<style:style style:name=\"Index\" style:family=\"paragraph\" style:parent-style-name=\"Standard\" style:class=\"index\"/>"
            "<style:style style:name=\"Footnote_Symbol\" style:display-name=\"Footnote Symbol\" style:family=\"text\">"
            "<style:text-properties style:text-position=\"super 58%\"/>"
            "</style:style>"
            "<style:style style:name=\"Endnote_Symbol\" style:display-name=\"Endnote Symbol\" style:family=\"text\">"
            "<style:text-properties style:text-position=\"super 58%\"/>"
            "</style:style>"
            "<style:style style:name=\"Footnote_anchor\" style:display-name=\"Footnote anchor\" style:family=\"text\">"
            "<style:text-properties style:text-position=\"super 58%\"/>"
            "</style:style>"
            "<style:style style:name=\"Endnote_anchor\" style:display-name=\"Endnote anchor\" style:family=\"text\">"
            "<style:text-properties style:text-position=\"super 58%\"/>"
            "</style:style>"
            "<text:notes-configuration text:note-class=\"footnote\" text:citation-style-name=\"Footnote_Symbol\" "
            "text:citation-body-style-name=\"Footnote_anchor\" style:num-format=\"1\" text:start-value=\"0\" "
            "text:footnotes-position=\"page\" text:start-numbering-at=\"document\"/>"
            "<text:notes-configuration text:note-class=\"endnote\" text:citation-style-name=\"Endnote_Symbol\" "
            "text:citation-body-style-name=\"Endnote_anchor\" text:master-page-name=\"Endnote\" "
            "style:num-format=\"i\" text:start-value=\"0\"/>"
            "<text:linenumbering-configuration text:number-lines=\"false\" text:offset=\"0.1965in\" "
            "style:num-format=\"1\" text:number-position=\"left\" text:increment=\"5\"/>"
            "</office:styles>"
            "<office:automatic-styles>"
            "<style:page-layout style:name=\"PM0\">"
            "<style:page-layout-properties fo:margin-bottom=\"1.0000in\" fo:margin-left=\"1.0000in\" "
            "fo:margin-right=\"1.0000in\" fo:margin-top=\"1.0000in\" fo:page-height=\"11.0000in\" "
            "fo:page-width=\"8.5000in\" style:print-orientation=\"portrait\">"
            "<style:footnote-sep style:adjustment=\"left\" style:color=\"#000000\" style:distance-after-sep=\"0.0398in\" "
            "style:distance-before-sep=\"0.0398in\" style:rel-width=\"25%\" style:width=\"0.0071in\"/>"
            "</style:page-layout-properties>"
            "</style:page-layout>"
            "<style:page-layout style:name=\"PM1\">"
            "<style:page-layout-properties fo:margin-bottom=\"1.0000in\" fo:margin-left=\"1.0000in\" "
            "fo:margin-right=\"1.0000in\" fo:margin-top=\"1.0000in\" fo:page-height=\"11.0000in\" "
            "fo:page-width=\"8.5000in\" style:print-orientation=\"portrait\">"
            "<style:footnote-sep style:adjustment=\"left\" style:color=\"#000000\" style:rel-width=\"25%\"/>"
            "</style:page-layout-properties>"
            "</style:page-layout>"
            "</office:automatic-styles>"
            "<office:master-styles>"
            "<style:master-page style:name=\"Standard\" style:page-layout-name=\"PM0\"/>"
            "<style:master-page style:name=\"Endnote\" style:page-layout-name=\"PM1\"/>"
            "</office:master-styles>"
            "</office:document-styles>";

    QByteArray input = m_chain->inputFile().toLocal8Bit();
    m_inputFile = input.data();
    QByteArray output = m_chain->outputFile().toLocal8Bit();
    m_outputFile = output.data();

    OdtOutputFileHelper helper(m_outputFile, 0);

    if (!helper.writeChildFile("mimetype", mimetypeStr, (char)0)) {
        fprintf(stderr, "ERROR : Couldn't write mimetype\n");
        return KoFilter::ParsingError;
    }

    if (!helper.writeChildFile("META-INF/manifest.xml", manifestStr)) {
        fprintf(stderr, "ERROR : Couldn't write manifest\n");
        return KoFilter::ParsingError;
    }

    if (!helper.writeChildFile("styles.xml", stylesStr)) {
        fprintf(stderr, "ERROR : Couldn't write styles\n");
        return KoFilter::ParsingError;
    }

    if (!helper.writeConvertedContent("content.xml", m_inputFile, ODF_CONTENT_XML))
    {
        fprintf(stderr, "ERROR : Couldn't write document content\n");
        return KoFilter::ParsingError;
    }

    return KoFilter::OK;
}

#include "WPDImport.moc"
