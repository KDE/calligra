/*
 * This file is part of Office 2007 Filters for KOffice
 * Copyright (C) 2002 Laurent Montel <lmontel@mandrakesoft.com>
 * Copyright (C) 2003 David Faure <faure@kde.org>
 * Copyright (C) 2002, 2003, 2004 Nicolas GOUTTE <goutte@kde.org>
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include "DocxImport.h"

#include <MsooXmlUtils.h>
#include <MsooXmlSchemas.h>
#include <MsooXmlContentTypes.h>
#include <MsooXmlRelationships.h>
#include "DocxXmlDocumentReader.h"
#include "DocxXmlStylesReader.h"
#include "DocxXmlNumberingReader.h"
#include "DocxXmlFootnoteReader.h"
#include "DocxXmlCommentsReader.h"
#include "DocxXmlEndnoteReader.h"
#include "DocxXmlFontTableReader.h"

#include <QColor>
#include <QFile>
#include <QFont>
#include <QPen>
#include <QRegExp>
#include <QImage>

#include <kdeversion.h>
#include <KDebug>
#include <KZip>
#include <KGenericFactory>
#include <KMessageBox>

#include <KoOdfWriteStore.h>
#include <KoEmbeddedDocumentSaver.h>
#include <KoDocumentInfo.h>
#include <KoDocument.h>
#include <KoFilterChain.h>
#include <KoUnit.h>
#include <KoPageLayout.h>
#include <KoXmlWriter.h>

typedef KGenericFactory<DocxImport> DocxImportFactory;
K_EXPORT_COMPONENT_FACTORY(libdocximport, DocxImportFactory("kofficefilters"))

enum DocxDocumentType {
    DocxDocument,
    DocxTemplate
};

class DocxImport::Private
{
public:
    Private() : type(DocxDocument), macrosEnabled(false) {
    }

    const char* mainDocumentContentType() const
    {
        if (type == DocxTemplate)
            return MSOOXML::ContentTypes::wordTemplate;
        return MSOOXML::ContentTypes::wordDocument;
    }

    DocxDocumentType type;
    bool macrosEnabled;
};

DocxImport::DocxImport(QObject* parent, const QStringList &)
        : MSOOXML::MsooXmlImport(QLatin1String("text"), parent), d(new Private)
{
}

DocxImport::~DocxImport()
{
    delete d;
}

bool DocxImport::acceptsSourceMimeType(const QByteArray& mime) const
{
    kDebug() << "Entering DOCX Import filter: from " << mime;
    if (mime == "application/vnd.openxmlformats-officedocument.wordprocessingml.document") {
        d->type = DocxDocument;
        d->macrosEnabled = false;
    }
    else if (mime == "application/vnd.openxmlformats-officedocument.wordprocessingml.template") {
        d->type = DocxTemplate;
        d->macrosEnabled = false;
    }
    else if (mime == "application/vnd.ms-word.document.macroEnabled.12") {
        d->type = DocxDocument;
        d->macrosEnabled = true;
    }
    else if (mime == "application/vnd.ms-word.template.macroEnabled.12") {
        d->type = DocxTemplate;
        d->macrosEnabled = true;
    }
    else
        return false;
    return true;
}

bool DocxImport::acceptsDestinationMimeType(const QByteArray& mime) const
{
    kDebug() << "Entering DOCX Import filter: to " << mime;
    return mime == "application/vnd.oasis.opendocument.text";
}

KoFilter::ConversionStatus DocxImport::parseParts(KoOdfWriters *writers, MSOOXML::MsooXmlRelationships *relationships,
        QString& errorMessage)
{
    // more here...
    // 0. temporary styles

    //office:styles
    writers->mainStyles->insertRawOdfStyles(
        KoGenStyles::DocumentStyles,
        "    <!-- COPIED -->"
        "\n    <style:default-style style:family=\"graphic\">"
        "\n      <style:graphic-properties draw:shadow-offset-x=\"0.3cm\" draw:shadow-offset-y=\"0.3cm\" draw:start-line-spacing-horizontal=\"0.283cm\" draw:start-line-spacing-vertical=\"0.283cm\" draw:end-line-spacing-horizontal=\"0.283cm\" draw:end-line-spacing-vertical=\"0.283cm\" style:flow-with-text=\"false\"/>"
        "\n      <style:paragraph-properties fo:line-height=\"115%\" style:text-autospace=\"ideograph-alpha\" style:line-break=\"strict\" style:writing-mode=\"lr-tb\" style:font-independent-line-spacing=\"false\">"
        "\n        <style:tab-stops/>"
        "\n      </style:paragraph-properties>"
        "\n      <style:text-properties style:use-window-font-color=\"true\" fo:font-size=\"11pt\" fo:language=\"en\" fo:country=\"GB\" style:letter-kerning=\"true\" style:font-size-asian=\"11pt\" style:language-asian=\"en\" style:country-asian=\"US\" style:font-size-complex=\"11pt\" style:language-complex=\"ar\" style:country-complex=\"SA\"/>"
        "\n    </style:default-style>"
        "\n    <style:default-style style:family=\"paragraph\">"
        "\n      <style:paragraph-properties fo:margin-top=\"0cm\" fo:margin-bottom=\"0.353cm\" fo:line-height=\"115%\" fo:hyphenation-ladder-count=\"no-limit\" style:text-autospace=\"ideograph-alpha\" style:punctuation-wrap=\"hanging\" style:line-break=\"strict\" style:tab-stop-distance=\"1.251cm\" style:writing-mode=\"page\"/>"
        "\n      <style:text-properties style:use-window-font-color=\"true\" style:font-name=\"Calibri\" fo:font-size=\"11pt\" fo:language=\"en\" fo:country=\"GB\" style:letter-kerning=\"true\" style:font-name-asian=\"Arial1\" style:font-size-asian=\"11pt\" style:language-asian=\"en\" style:country-asian=\"US\" style:font-name-complex=\"F\" style:font-size-complex=\"11pt\" style:language-complex=\"ar\" style:country-complex=\"SA\" fo:hyphenate=\"false\" fo:hyphenation-remain-char-count=\"2\" fo:hyphenation-push-char-count=\"2\"/>"
        "\n    </style:default-style>"
        "\n    <style:default-style style:family=\"table\">"
        "\n      <style:table-properties style:border-model=\"collapsing\"/>"
        "\n    </style:default-style>"
        "\n    <style:default-style style:family=\"table-row\">"
        "\n      <style:table-row-properties fo:keep-together=\"auto\"/>"
        "\n    </style:default-style>"
        "\n    <style:style style:name=\"Standard\" style:family=\"paragraph\" style:default-outline-level=\"\" style:class=\"text\">"
        "\n      <style:paragraph-properties fo:orphans=\"2\" fo:widows=\"2\" style:writing-mode=\"lr-tb\"/>"
        "\n      <style:text-properties style:use-window-font-color=\"true\"/>"
        "\n    </style:style>"
        "\n    <style:style style:name=\"Heading\" style:family=\"paragraph\" style:parent-style-name=\"Standard\" style:next-style-name=\"Text_20_body\" style:class=\"text\">"
        "\n      <style:paragraph-properties fo:margin-top=\"0.423cm\" fo:margin-bottom=\"0.212cm\" fo:keep-with-next=\"always\"/>"
        "\n      <style:text-properties style:font-name=\"Arial\" fo:font-size=\"14pt\" style:font-name-asian=\"Arial1\" style:font-size-asian=\"14pt\" style:font-name-complex=\"Tahoma\" style:font-size-complex=\"14pt\"/>"
        "\n    </style:style>"
        "\n    <style:style style:name=\"Text_20_body\" style:display-name=\"Text body\" style:family=\"paragraph\" style:parent-style-name=\"Standard\" style:class=\"text\">"
        "\n      <style:paragraph-properties fo:margin-top=\"0cm\" fo:margin-bottom=\"0.212cm\"/>"
        "\n    </style:style>"
        "\n    <style:style style:name=\"List\" style:family=\"paragraph\" style:parent-style-name=\"Text_20_body\" style:class=\"list\">"
        "\n      <style:text-properties style:font-size-asian=\"12pt\" style:font-name-complex=\"Tahoma1\"/>"
        "\n    </style:style>"
        "\n    <style:style style:name=\"Caption\" style:family=\"paragraph\" style:parent-style-name=\"Standard\" style:class=\"extra\">"
        "\n      <style:paragraph-properties fo:margin-top=\"0.212cm\" fo:margin-bottom=\"0.212cm\" text:number-lines=\"false\" text:line-number=\"0\"/>"
        "\n      <style:text-properties fo:font-size=\"12pt\" fo:font-style=\"italic\" style:font-size-asian=\"12pt\" style:font-style-asian=\"italic\" style:font-name-complex=\"Tahoma1\" style:font-size-complex=\"12pt\" style:font-style-complex=\"italic\"/>"
        "\n    </style:style>"
        "\n    <style:style style:name=\"Index\" style:family=\"paragraph\" style:parent-style-name=\"Standard\" style:class=\"index\">"
        "\n      <style:paragraph-properties text:number-lines=\"false\" text:line-number=\"0\"/>"
        "\n      <style:text-properties style:font-size-asian=\"12pt\" style:font-name-complex=\"Tahoma1\"/>"
        "\n    </style:style>"
        "\n    <style:style style:name=\"Balloon_20_Text\" style:display-name=\"Balloon Text\" style:family=\"paragraph\" style:parent-style-name=\"Standard\"/>"
        "\n    <style:style style:name=\"Document_20_Map\" style:display-name=\"Document Map\" style:family=\"paragraph\" style:parent-style-name=\"Standard\"/>"
        "\n    <style:style style:name=\"Default_20_Paragraph_20_Font\" style:display-name=\"Default Paragraph Font\" style:family=\"text\"/>"
        "\n    <style:style style:name=\"Balloon_20_Text_20_Char\" style:display-name=\"Balloon Text Char\" style:family=\"text\" style:parent-style-name=\"Default_20_Paragraph_20_Font\"/>"
        "\n    <style:style style:name=\"Document_20_Map_20_Char\" style:display-name=\"Document Map Char\" style:family=\"text\" style:parent-style-name=\"Default_20_Paragraph_20_Font\"/>"
        "\n    <style:style style:name=\"Graphics\" style:family=\"graphic\">"
        "\n      <style:graphic-properties text:anchor-type=\"paragraph\" svg:x=\"0cm\" svg:y=\"0cm\" style:wrap=\"dynamic\" style:number-wrapped-paragraphs=\"no-limit\" style:wrap-contour=\"false\" style:vertical-pos=\"top\" style:vertical-rel=\"paragraph\" style:horizontal-pos=\"center\" style:horizontal-rel=\"paragraph\"/>"
        "\n    </style:style>"
        "\n    <text:outline-style style:name=\"Outline\">"
        "\n      <text:outline-level-style text:level=\"1\" style:num-format=\"\">"
        "\n        <style:list-level-properties text:list-level-position-and-space-mode=\"label-alignment\">"
        "\n          <style:list-level-label-alignment text:label-followed-by=\"listtab\" text:list-tab-stop-position=\"0.762cm\" fo:text-indent=\"-0.762cm\" fo:margin-left=\"0.762cm\"/>"
        "\n        </style:list-level-properties>"
        "\n      </text:outline-level-style>"
        "\n      <text:outline-level-style text:level=\"2\" style:num-format=\"\">"
        "\n        <style:list-level-properties text:list-level-position-and-space-mode=\"label-alignment\">"
        "\n          <style:list-level-label-alignment text:label-followed-by=\"listtab\" text:list-tab-stop-position=\"1.016cm\" fo:text-indent=\"-1.016cm\" fo:margin-left=\"1.016cm\"/>"
        "\n        </style:list-level-properties>"
        "\n      </text:outline-level-style>"
        "\n      <text:outline-level-style text:level=\"3\" style:num-format=\"\">"
        "\n        <style:list-level-properties text:list-level-position-and-space-mode=\"label-alignment\">"
        "\n          <style:list-level-label-alignment text:label-followed-by=\"listtab\" text:list-tab-stop-position=\"1.27cm\" fo:text-indent=\"-1.27cm\" fo:margin-left=\"1.27cm\"/>"
        "\n        </style:list-level-properties>"
        "\n      </text:outline-level-style>"
        "\n      <text:outline-level-style text:level=\"4\" style:num-format=\"\">"
        "\n        <style:list-level-properties text:list-level-position-and-space-mode=\"label-alignment\">"
        "\n          <style:list-level-label-alignment text:label-followed-by=\"listtab\" text:list-tab-stop-position=\"1.524cm\" fo:text-indent=\"-1.524cm\" fo:margin-left=\"1.524cm\"/>"
        "\n        </style:list-level-properties>"
        "\n      </text:outline-level-style>"
        "\n      <text:outline-level-style text:level=\"5\" style:num-format=\"\">"
        "\n        <style:list-level-properties text:list-level-position-and-space-mode=\"label-alignment\">"
        "\n          <style:list-level-label-alignment text:label-followed-by=\"listtab\" text:list-tab-stop-position=\"1.778cm\" fo:text-indent=\"-1.778cm\" fo:margin-left=\"1.778cm\"/>"
        "\n        </style:list-level-properties>"
        "\n      </text:outline-level-style>"
        "\n      <text:outline-level-style text:level=\"6\" style:num-format=\"\">"
        "\n        <style:list-level-properties text:list-level-position-and-space-mode=\"label-alignment\">"
        "\n          <style:list-level-label-alignment text:label-followed-by=\"listtab\" text:list-tab-stop-position=\"2.032cm\" fo:text-indent=\"-2.032cm\" fo:margin-left=\"2.032cm\"/>"
        "\n        </style:list-level-properties>"
        "\n      </text:outline-level-style>"
        "\n      <text:outline-level-style text:level=\"7\" style:num-format=\"\">"
        "\n        <style:list-level-properties text:list-level-position-and-space-mode=\"label-alignment\">"
        "\n          <style:list-level-label-alignment text:label-followed-by=\"listtab\" text:list-tab-stop-position=\"2.286cm\" fo:text-indent=\"-2.286cm\" fo:margin-left=\"2.286cm\"/>"
        "\n        </style:list-level-properties>"
        "\n      </text:outline-level-style>"
        "\n      <text:outline-level-style text:level=\"8\" style:num-format=\"\">"
        "\n        <style:list-level-properties text:list-level-position-and-space-mode=\"label-alignment\">"
        "\n          <style:list-level-label-alignment text:label-followed-by=\"listtab\" text:list-tab-stop-position=\"2.54cm\" fo:text-indent=\"-2.54cm\" fo:margin-left=\"2.54cm\"/>"
        "\n        </style:list-level-properties>"
        "\n      </text:outline-level-style>"
        "\n      <text:outline-level-style text:level=\"9\" style:num-format=\"\">"
        "\n        <style:list-level-properties text:list-level-position-and-space-mode=\"label-alignment\">"
        "\n          <style:list-level-label-alignment text:label-followed-by=\"listtab\" text:list-tab-stop-position=\"2.794cm\" fo:text-indent=\"-2.794cm\" fo:margin-left=\"2.794cm\"/>"
        "\n        </style:list-level-properties>"
        "\n      </text:outline-level-style>"
        "\n      <text:outline-level-style text:level=\"10\" style:num-format=\"\">"
        "\n        <style:list-level-properties text:list-level-position-and-space-mode=\"label-alignment\">"
        "\n          <style:list-level-label-alignment text:label-followed-by=\"listtab\" text:list-tab-stop-position=\"3.048cm\" fo:text-indent=\"-3.048cm\" fo:margin-left=\"3.048cm\"/>"
        "\n        </style:list-level-properties>"
        "\n      </text:outline-level-style>"
        "\n    </text:outline-style>"
        "\n    <!-- /COPIED -->"
    );

    // 1. parse font table
    {
        DocxXmlFontTableReaderContext context(*writers->mainStyles);
        DocxXmlFontTableReader fontTableReader(writers);
        RETURN_IF_ERROR( loadAndParseDocumentIfExists(
            MSOOXML::ContentTypes::wordFontTable, &fontTableReader, writers, errorMessage, &context) )
    }

    QList<QByteArray> partNames = this->partNames(d->mainDocumentContentType());
    if (partNames.count() != 1) {
        errorMessage = i18n("Unable to find part for type %1", d->mainDocumentContentType());
        return KoFilter::WrongFormat;
    }
    const QString documentPathAndFile(partNames.first());
    QString documentPath, documentFile;
    MSOOXML::Utils::splitPathAndFile(documentPathAndFile, &documentPath, &documentFile);

    // 2. parse theme for the document
    MSOOXML::DrawingMLTheme themes;
    const QString docThemePathAndFile(relationships->targetForType(
        documentPath, documentFile,
        QLatin1String(MSOOXML::Schemas::officeDocument::relationships) + "/theme"));
    kDebug() << QLatin1String(MSOOXML::Schemas::officeDocument::relationships) + "/theme";
    kDebug() << "ThemePathAndFile:" << docThemePathAndFile;

    QString docThemePath, docThemeFile;
    MSOOXML::Utils::splitPathAndFile(docThemePathAndFile, &docThemePath, &docThemeFile);

    MSOOXML::MsooXmlThemesReader themesReader(writers);
    MSOOXML::MsooXmlThemesReaderContext themecontext(themes, relationships, (MSOOXML::MsooXmlImport*)this,
        docThemePath, docThemeFile);

    KoFilter::ConversionStatus status
        = loadAndParseDocument(&themesReader, docThemePathAndFile, errorMessage, &themecontext);

    // Main document context, to which we collect footnotes, endnotes, comments
    DocxXmlDocumentReaderContext mainContext(*this, documentPath, documentFile, *relationships, &themes);

    // 3. parse styles
    {
        // get styles path from document's relationships, not from content types; typically returns /word/styles.xml
        // ECMA-376, 11.3.12 Style Definitions Part, p. 65
        // An instance of this part type contains the definition for a set of styles used by this document.
        // A package shall contain at most two Style Definitions parts. One instance of that part shall be
        // the target of an implicit relationship from the Main Document (§11.3.10) part, and the other shall
        // be the target of an implicit relationship in from the Glossary Document (§11.3.8) part.
        const QString stylesPathAndFile(relationships->targetForType(documentPath, documentFile,
            QLatin1String(MSOOXML::Schemas::officeDocument::relationships) + "/styles"));
        DocxXmlStylesReader stylesReader(writers);
        if (!stylesPathAndFile.isEmpty()) {
            QString stylesPath, stylesFile;
            MSOOXML::Utils::splitPathAndFile(stylesPathAndFile, &stylesPath, &stylesFile);
            DocxXmlDocumentReaderContext context(*this, stylesPath, stylesFile, *relationships, &themes);

            RETURN_IF_ERROR( loadAndParseDocumentFromFileIfExists(
                stylesPathAndFile, &stylesReader, writers, errorMessage, &context) )
        }
    }

    // 4. parse numbering
    {
        const QString numberingPathAndFile(relationships->targetForType(documentPath, documentFile,
            QLatin1String(MSOOXML::Schemas::officeDocument::relationships) + "/numbering"));
        DocxXmlNumberingReader numberingReader(writers);
        if (!numberingPathAndFile.isEmpty()) {
            QString numberingPath, numberingFile;
            MSOOXML::Utils::splitPathAndFile(numberingPathAndFile, &numberingPath, &numberingFile);
            DocxXmlDocumentReaderContext context(*this, numberingPath, numberingFile, *relationships, &themes);

            RETURN_IF_ERROR( loadAndParseDocumentFromFileIfExists(
                numberingPathAndFile, &numberingReader, writers, errorMessage, &context) )
        }
    }

    // 5. parse footnotes
    {
        const QString footnotePathAndFile(relationships->targetForType(documentPath, documentFile,
            QLatin1String(MSOOXML::Schemas::officeDocument::relationships) + "/footnotes"));
        //! @todo use m_contentTypes.values() when multiple paths are expected, e.g. for ContentTypes::wordHeader
        DocxXmlFootnoteReader footnoteReader(writers);
        if (!footnotePathAndFile.isEmpty()) {
            QString footnotePath, footnoteFile;
            MSOOXML::Utils::splitPathAndFile(footnotePathAndFile, &footnotePath, &footnoteFile);
            DocxXmlDocumentReaderContext context(*this, footnotePath, footnoteFile, *relationships, &themes);

            RETURN_IF_ERROR( loadAndParseDocumentFromFileIfExists(
                footnotePathAndFile, &footnoteReader, writers, errorMessage, &context) )
            mainContext.m_footnotes = context.m_footnotes;
        }

    // 6. parse comments
        const QString commentPathAndFile(relationships->targetForType(documentPath, documentFile,
           QLatin1String(MSOOXML::Schemas::officeDocument::relationships) + "/comments"));
        DocxXmlCommentReader commentReader(writers);
        if (!commentPathAndFile.isEmpty()) {
            QString commentPath, commentFile;
            MSOOXML::Utils::splitPathAndFile(commentPathAndFile, &commentPath, &commentFile);
            DocxXmlDocumentReaderContext context(*this, commentPath, commentFile, *relationships, &themes);

            RETURN_IF_ERROR( loadAndParseDocumentFromFileIfExists(
                commentPathAndFile, &commentReader, writers, errorMessage, &context) )
            mainContext.m_comments = context.m_comments;
        }

    // 7. parse endnotes
        const QString endnotePathAndFile(relationships->targetForType(documentPath, documentFile,
        QLatin1String(MSOOXML::Schemas::officeDocument::relationships) + "/endnotes"));
        DocxXmlEndnoteReader endnoteReader(writers);
        if (!endnotePathAndFile.isEmpty()) {
            QString endnotePath, endnoteFile;
            MSOOXML::Utils::splitPathAndFile(endnotePathAndFile, &endnotePath, &endnoteFile);
            DocxXmlDocumentReaderContext context(*this, endnotePath, endnoteFile, *relationships, &themes);

            RETURN_IF_ERROR( loadAndParseDocumentFromFileIfExists(
                endnotePathAndFile, &endnoteReader, writers, errorMessage, &context) )
            mainContext.m_endnotes = context.m_endnotes;
        }

    // 8. parse document
        DocxXmlDocumentReader documentReader(writers);
        RETURN_IF_ERROR( loadAndParseDocument(
            d->mainDocumentContentType(), &documentReader, writers, errorMessage, &mainContext) )
    }
    // more here...
    return KoFilter::OK;
}

#include "DocxImport.moc"
