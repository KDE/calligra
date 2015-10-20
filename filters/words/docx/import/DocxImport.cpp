/*
 * This file is part of Office 2007 Filters for Calligra
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
#include "DocxXmlSettingsReader.h"
#include "DocxDebug.h"

#include <QColor>
#include <QFile>
#include <QFont>
#include <QPen>
#include <QRegExp>
#include <QImage>

#include <kpluginfactory.h>

#include <KoEmbeddedDocumentSaver.h>
#include <KoDocumentInfo.h>
#include <KoDocument.h>
#include <KoFilterChain.h>
#include <KoPageLayout.h>
#include <KoXmlWriter.h>

K_PLUGIN_FACTORY_WITH_JSON(DocxImportFactory, "calligra_filter_docx2odt.json",
                           registerPlugin<DocxImport>();)

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
        if (type == DocxTemplate) {
            return MSOOXML::ContentTypes::wordTemplate;
        }
        return MSOOXML::ContentTypes::wordDocument;
    }

    DocxDocumentType type;
    bool macrosEnabled;
    QMap<QString, QVariant> documentSettings;
    QMap<QString, QString> colorMap;
};

DocxImport::DocxImport(QObject* parent, const QVariantList &)
        : MSOOXML::MsooXmlImport(QLatin1String("text"), parent), d(new Private)
{
}

DocxImport::~DocxImport()
{
    delete d;
}

QMap<QString, QVariant> DocxImport::documentSettings() const
{
    return d->documentSettings;
}

QVariant DocxImport::documentSetting(const QString& name) const
{
    return d->documentSettings.value(name);
}

bool DocxImport::acceptsSourceMimeType(const QByteArray& mime) const
{
    debugDocx << "Entering DOCX Import filter: from " << mime;
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
    debugDocx << "Entering DOCX Import filter: to " << mime;
    return mime == "application/vnd.oasis.opendocument.text";
}

KoFilter::ConversionStatus DocxImport::parseParts(KoOdfWriters *writers, MSOOXML::MsooXmlRelationships *relationships,
        QString& errorMessage)
{
    writers->body->addAttribute("text:use-soft-page-breaks", "true");

    // 0. parse settings.xml
    {
        DocxXmlSettingsReaderContext context(d->documentSettings);
        DocxXmlSettingsReader settingsReader(writers);
        d->colorMap = context.colorMap;

        RETURN_IF_ERROR( loadAndParseDocumentIfExists(
            MSOOXML::ContentTypes::wordSettings, &settingsReader, writers, errorMessage, &context) )
    }

    reportProgress(5);

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

    // 2. parse theme for the document if a theme exists
    MSOOXML::DrawingMLTheme themes;
    const QString docThemePathAndFile(relationships->targetForType(
        documentPath, documentFile,
        QLatin1String(MSOOXML::Schemas::officeDocument::relationships) + "/theme"));
    if (!docThemePathAndFile.isEmpty()) {
        debugDocx << QLatin1String(MSOOXML::Schemas::officeDocument::relationships) + "/theme";

        // prepare the themes-reader
        QString docThemePath, docThemeFile;
        MSOOXML::Utils::splitPathAndFile(docThemePathAndFile, &docThemePath, &docThemeFile);

        MSOOXML::MsooXmlThemesReader themesReader(writers);
        MSOOXML::MsooXmlThemesReaderContext themecontext(themes, relationships, (MSOOXML::MsooXmlImport*)this,
            docThemePath, docThemeFile);

        KoFilter::ConversionStatus status
            = loadAndParseDocument(&themesReader, docThemePathAndFile, errorMessage, &themecontext);

        debugDocx << "Reading ThemePathAndFile:" << docThemePathAndFile << "status=" << status;
    }

    reportProgress(15);

    // Main document context, to which we collect footnotes, endnotes,
    // comments, numbering, tablestyles
    DocxXmlDocumentReaderContext mainContext(*this, documentPath, documentFile, *relationships, &themes);

    // 3. parse styles
    {
        // get styles path from document's relationships, not from content
        // types; typically returns /word/styles.xml
        //
        // ECMA-376, 11.3.12 Style Definitions Part, p. 65
        //
        // An instance of this part type contains the definition for a set of
        // styles used by this document.  A package shall contain at most two
        // Style Definitions parts.  One instance of that part shall be the
        // target of an implicit relationship from the Main Document (§11.3.10)
        // part, and the other shall be the target of an implicit relationship
        // in from the Glossary Document (§11.3.8) part.

        const QString stylesPathAndFile(relationships->targetForType(documentPath, documentFile,
            QLatin1String(MSOOXML::Schemas::officeDocument::relationships) + "/styles"));
        DocxXmlStylesReader stylesReader(writers);
        if (!stylesPathAndFile.isEmpty()) {
            QString stylesPath, stylesFile;
            MSOOXML::Utils::splitPathAndFile(stylesPathAndFile, &stylesPath, &stylesFile);
            DocxXmlDocumentReaderContext context(*this, stylesPath, stylesFile, *relationships, &themes);

            RETURN_IF_ERROR( loadAndParseDocumentFromFileIfExists(
                stylesPathAndFile, &stylesReader, writers, errorMessage, &context) )

            mainContext.m_tableStyles = context.m_tableStyles;
            mainContext.m_namedDefaultStyles = context.m_namedDefaultStyles;
            mainContext.m_defaultFontSizePt = context.m_defaultFontSizePt;
        }
    }

    reportProgress(25);

    // 4. parse numbering
    const QString numberingPathAndFile(relationships->targetForType(documentPath, documentFile,
        QLatin1String(MSOOXML::Schemas::officeDocument::relationships) + "/numbering"));
    DocxXmlNumberingReader numberingReader(writers);
    QString numberingPath, numberingFile;
    MSOOXML::Utils::splitPathAndFile(numberingPathAndFile, &numberingPath, &numberingFile);
    DocxXmlDocumentReaderContext numberingContext(*this, numberingPath, numberingFile, *relationships, &themes);

    if (!numberingPathAndFile.isEmpty()) {
        RETURN_IF_ERROR( loadAndParseDocumentFromFileIfExists(
            numberingPathAndFile, &numberingReader, writers, errorMessage, &numberingContext) )
    }
    mainContext.m_bulletStyles = numberingContext.m_bulletStyles;
    mainContext.m_abstractNumIDs = numberingContext.m_abstractNumIDs;

    reportProgress(30);

    {
	// 5. parse footnotes
        const QString footnotePathAndFile(relationships->targetForType(documentPath, documentFile,
            QLatin1String(MSOOXML::Schemas::officeDocument::relationships) + "/footnotes"));
        //! @todo use m_contentTypes.values() when multiple paths are expected, e.g. for ContentTypes::wordHeader
        DocxXmlFootnoteReader footnoteReader(writers);
        if (!footnotePathAndFile.isEmpty()) {
            QString footnotePath, footnoteFile;
            MSOOXML::Utils::splitPathAndFile(footnotePathAndFile, &footnotePath, &footnoteFile);
            DocxXmlDocumentReaderContext context(*this, footnotePath, footnoteFile, *relationships, &themes);
            context.m_tableStyles = mainContext.m_tableStyles;
            context.m_bulletStyles = mainContext.m_bulletStyles;
            context.m_namedDefaultStyles = mainContext.m_namedDefaultStyles;
            context.m_abstractNumIDs = mainContext.m_abstractNumIDs;

            RETURN_IF_ERROR( loadAndParseDocumentFromFileIfExists(
                footnotePathAndFile, &footnoteReader, writers, errorMessage, &context) )
            mainContext.m_footnotes = context.m_footnotes;
        }
        reportProgress(35);

        // 6. parse comments
        const QString commentPathAndFile(relationships->targetForType(documentPath, documentFile,
           QLatin1String(MSOOXML::Schemas::officeDocument::relationships) + "/comments"));
        DocxXmlCommentReader commentReader(writers);
        if (!commentPathAndFile.isEmpty()) {
            QString commentPath, commentFile;
            MSOOXML::Utils::splitPathAndFile(commentPathAndFile, &commentPath, &commentFile);
            DocxXmlDocumentReaderContext context(*this, commentPath, commentFile, *relationships, &themes);
            context.m_tableStyles = mainContext.m_tableStyles;
            context.m_bulletStyles = mainContext.m_bulletStyles;
            //TODO: m_abstractNumIDs and m_namedDefaultStyles might be needed

            RETURN_IF_ERROR( loadAndParseDocumentFromFileIfExists(
                commentPathAndFile, &commentReader, writers, errorMessage, &context) )
            mainContext.m_comments = context.m_comments;
        }

        reportProgress(40);

        // 7. parse endnotes
        const QString endnotePathAndFile(relationships->targetForType(documentPath, documentFile,
            QLatin1String(MSOOXML::Schemas::officeDocument::relationships) + "/endnotes"));
        DocxXmlEndnoteReader endnoteReader(writers);
        if (!endnotePathAndFile.isEmpty()) {
            QString endnotePath, endnoteFile;
            MSOOXML::Utils::splitPathAndFile(endnotePathAndFile, &endnotePath, &endnoteFile);
            DocxXmlDocumentReaderContext context(*this, endnotePath, endnoteFile, *relationships, &themes);
            context.m_tableStyles = mainContext.m_tableStyles;
            context.m_bulletStyles = mainContext.m_bulletStyles;
            context.m_namedDefaultStyles = mainContext.m_namedDefaultStyles;
            context.m_abstractNumIDs = mainContext.m_abstractNumIDs;

            RETURN_IF_ERROR( loadAndParseDocumentFromFileIfExists(
                endnotePathAndFile, &endnoteReader, writers, errorMessage, &context) )
            mainContext.m_endnotes = context.m_endnotes;
        }
        reportProgress(45);

        // 8. parse document
        // Some of the templates MIGHT be defined in numberingreader.
        DocxXmlDocumentReader documentReader(writers);
        documentReader.m_definedShapeTypes = numberingReader.m_definedShapeTypes;
        RETURN_IF_ERROR( loadAndParseDocument(
            d->mainDocumentContentType(), &documentReader, writers, errorMessage, &mainContext) )
    }
    reportProgress(100);

    return KoFilter::OK;
}

void DocxImport::writeConfigurationSettings(KoXmlWriter* settings) const
{
    MsooXmlImport::writeConfigurationSettings(settings);

    // This config item is used in KoTextLayoutArea::handleBordersAndSpacing
    // during layouting.  The defined 'Above paragraph' and 'Below paragraph'
    // paragraph spacing (which is written in the ODF as fo:margin-top for the
    // KoParagraphStyle) are not applied to the first and the last paragraph if
    // this value is true.
    settings->startElement("config:config-item");
    settings->addAttribute("config:name", "AddParaTableSpacingAtStart");
    settings->addAttribute("config:type", "boolean");
    settings->addTextSpan("true");
    settings->endElement();

    // OOo requires this config item to display files produced by this filter
    // correctly.  If true, then the fo:text-indent attribute will be ignored.
    settings->startElement("config:config-item");
    settings->addAttribute("config:name", "IgnoreFirstLineIndentInNumbering");
    settings->addAttribute("config:type", "boolean");
    settings->addTextSpan("false");
    settings->endElement();
}

#include "DocxImport.moc"
