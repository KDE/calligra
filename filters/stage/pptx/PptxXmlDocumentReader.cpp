/*
 * This file is part of Office 2007 Filters for Calligra
 *
 * Copyright (C) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "PptxXmlDocumentReader.h"

#include "PptxXmlCommentAuthorsReader.h"
#include "PptxImport.h"
#include "PptxDebug.h"

#include <VmlDrawingReader.h>
#include <MsooXmlRelationships.h>
#include <MsooXmlSchemas.h>
#include <MsooXmlUtils.h>
#include <MsooXmlUnits.h>
#include <MsooXmlDrawingTableStyleReader.h>

#include <KoXmlWriter.h>
#include <KoGenStyles.h>
#include <KoPageLayout.h>
#include <KoOdfGraphicStyles.h>

#include <KoTable.h>
#include <KoRow.h>
#include <KoColumn.h>
#include <KoCell.h>
#include <KoCellStyle.h>

#define MSOOXML_CURRENT_NS "p"
#define MSOOXML_CURRENT_CLASS PptxXmlDocumentReader
#define BIND_READ_CLASS MSOOXML_CURRENT_CLASS
#define PPTXXMLDOCUMENTREADER_CPP

#include <MsooXmlReader_p.h>


PptxXmlDocumentReaderContext::PptxXmlDocumentReaderContext(
    PptxImport& _import, const QString& _path, const QString& _file,
    MSOOXML::MsooXmlRelationships& _relationships)
        : import(&_import),
          path(_path), file(_file), relationships(&_relationships)
{
    firstReadRound = true;
    numberOfItems = 0;
}

class PptxXmlDocumentReader::Private
{
public:
    Private()
    {
    }
    ~Private() {
        qDeleteAll(slideLayoutPropertiesMap);
    }
    QMap<QString, PptxSlideProperties*> slideLayoutPropertiesMap;
    uint slideNumber; //!< temp., see todo in PptxXmlDocumentReader::read_sldId()
    bool sldSzRead, noteSzRead;
    KoPageLayout pageLayout, notePageLayout;

    // Several because there are several masterpages
    QVector<QString> masterPageDrawStyleNames;
    QVector<KoGenStyle> masterPageStyles;
    QVector<QString> masterPageFrames;

    QMap<QString, PptxSlideProperties> slideMasterPageProperties;
    QMap<QString, PptxSlideProperties> notesMasterPageProperties;

    QMap<int, QString> commentAuthors;
private:
};

PptxXmlDocumentReader::PptxXmlDocumentReader(KoOdfWriters *writers)
        : MSOOXML::MsooXmlCommonReader(writers)
        , m_writers(writers)
        , m_context(0)
        , d(new Private)
{
    init();
}

PptxXmlDocumentReader::~PptxXmlDocumentReader()
{
    delete d;
}

void PptxXmlDocumentReader::init()
{
    m_defaultNamespace = QLatin1String(MSOOXML_CURRENT_NS ":");
}

KoFilter::ConversionStatus PptxXmlDocumentReader::read(MSOOXML::MsooXmlReaderContext* context)
{
    m_context = dynamic_cast<PptxXmlDocumentReaderContext*>(context);
    Q_ASSERT(m_context);
    d->slideNumber = 0;
    d->sldSzRead = false;
    d->noteSzRead = false;
    d->pageLayout = KoPageLayout();
    d->notePageLayout = KoPageLayout();

    const KoFilter::ConversionStatus result = readInternal();

    m_context = 0;
    return result;
}

KoFilter::ConversionStatus PptxXmlDocumentReader::readInternal()
{
    debugPptx << "=============================";
    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }

    // p:presentation
    readNext();
    debugPptx << *this << namespaceUri();

    if (!expectEl("p:presentation")) {
        return KoFilter::WrongFormat;
    }
    if (!expectNS(MSOOXML::Schemas::presentationml)) {
        return KoFilter::WrongFormat;
    }
//     const QXmlStreamAttributes attrs( attributes() );
//     for (int i=0; i<attrs.count(); i++) {
//         debugPptx << "1 NS prefix:" << attrs[i].name() << "uri:" << attrs[i].namespaceUri();

    QXmlStreamNamespaceDeclarations namespaces(namespaceDeclarations());
    for (int i = 0; i < namespaces.count(); i++) {
        debugPptx << "NS prefix:" << namespaces[i].prefix() << "uri:" << namespaces[i].namespaceUri();
    }
//! @todo find out whether the namespace returned by namespaceUri()
//!       is exactly the same ref as the element of namespaceDeclarations()
    if (!namespaces.contains(QXmlStreamNamespaceDeclaration("p", MSOOXML::Schemas::presentationml))) {
        raiseError(i18n("Namespace \"%1\" not found", QLatin1String(MSOOXML::Schemas::presentationml)));
        return KoFilter::WrongFormat;
    }
//! @todo expect other namespaces too...

    {
        PptxXmlCommentAuthorsReader autorsReader(this);
        const QString autorsFilePath = m_context->relationships->targetForType(m_context->path, m_context->file, MSOOXML::Relationships::commentAuthors);
        PptxXmlCommentAuthorsReaderContext autorsContext;
        m_context->import->loadAndParseDocument(&autorsReader, autorsFilePath, &autorsContext);
        d->commentAuthors = autorsContext.authors;
    }

    TRY_READ(presentation)
    debugPptx << "===========finished============";
    return KoFilter::OK;
}

PptxSlideProperties* PptxXmlDocumentReader::slideLayoutProperties(const QString& slidePath, const QString& slideFile)
{
    const QString slideLayoutPathAndFile(m_context->relationships->targetForType(
        slidePath, slideFile,
        QLatin1String(MSOOXML::Schemas::officeDocument::relationships) + "/slideLayout"));
    debugPptx << QLatin1String(MSOOXML::Schemas::officeDocument::relationships) + "/slideLayout";
    debugPptx << "slideLayoutPathAndFile:" << slideLayoutPathAndFile;
    if (slideLayoutPathAndFile.isEmpty())
        return 0;

    QString slideLayoutPath, slideLayoutFile;
    MSOOXML::Utils::splitPathAndFile(slideLayoutPathAndFile, &slideLayoutPath, &slideLayoutFile);

    // load layout or find in cache
    PptxSlideProperties *result = d->slideLayoutPropertiesMap.value(slideLayoutPathAndFile);
    if (result)
        return result;

    QString slideMasterPath, slideMasterFile;
    MSOOXML::Utils::splitPathAndFile(m_context->relationships->targetForType(slidePath, slideFile,
        QLatin1String(MSOOXML::Schemas::officeDocument::relationships) + "/slideLayout"), &slideMasterPath, &slideMasterFile);
    const QString slideMasterPathAndFile = m_context->relationships->targetForType(slideMasterPath, slideMasterFile,
         QLatin1String(MSOOXML::Schemas::officeDocument::relationships) + "/slideMaster");

    result = new PptxSlideProperties();
    result->m_slideMasterName = slideMasterPathAndFile;

    VmlDrawingReader vmlreader(this);
    QString vmlTarget = m_context->relationships->targetForType(slideLayoutPath, slideLayoutFile,
        "http://schemas.openxmlformats.org/officeDocument/2006/relationships/vmlDrawing");

    if (!vmlTarget.isEmpty()) {
        QString errorMessage, vmlPath, vmlFile;

        QString fileName = vmlTarget;
        fileName.remove(0, m_context->path.length());
        MSOOXML::Utils::splitPathAndFile(vmlTarget, &vmlPath, &vmlFile);

        VmlDrawingReaderContext vmlContext(*m_context->import,
            vmlPath, vmlFile, *m_context->relationships);

        const KoFilter::ConversionStatus status =
            m_context->import->loadAndParseDocument(&vmlreader, vmlTarget, errorMessage, &vmlContext);
        if (status != KoFilter::OK) {
            vmlreader.raiseError(errorMessage);
        }
    }

    MSOOXML::Utils::AutoPtrSetter<PptxSlideProperties> slideLayoutPropertiesSetter(result);
    PptxXmlSlideReaderContext context(
        *m_context->import,
        slideLayoutPath, slideLayoutFile,
        0/*unused*/, &d->slideMasterPageProperties[slideMasterPathAndFile].theme,
        PptxXmlSlideReader::SlideLayout,
        result,
        &d->slideMasterPageProperties[slideMasterPathAndFile], //PptxSlideMasterPageProperties
        0,
        *m_context->relationships,
        d->commentAuthors,
        d->slideMasterPageProperties[slideMasterPathAndFile].colorMap,
        vmlreader
    );

    PptxXmlSlideReader slideLayoutReader(this);
    context.firstReadingRound = true;

    KoFilter::ConversionStatus status = m_context->import->loadAndParseDocument(
        &slideLayoutReader, slideLayoutPath + '/' + slideLayoutFile, &context);
    if (status != KoFilter::OK) {
        debugPptx << slideLayoutReader.errorString();
        return 0;
    }

    context.initializeContext(d->slideMasterPageProperties[slideMasterPathAndFile].theme, defaultParagraphStyles,
        defaultTextStyles, defaultListStyles, defaultBulletColors, defaultTextColors, defaultLatinFonts);

    context.firstReadingRound = false;
    status = m_context->import->loadAndParseDocument(
        &slideLayoutReader, slideLayoutPath + '/' + slideLayoutFile, &context);
    if (status != KoFilter::OK) {
        debugPptx << slideLayoutReader.errorString();
        return 0;
    }

    slideLayoutPropertiesSetter.release();
    d->slideLayoutPropertiesMap.insert(slideLayoutPathAndFile, result);
    return result;
}

#undef CURRENT_EL
#define CURRENT_EL sldId
//! p:sldId handler (Slide ID)
/*! ECMA-376, 19.2.1.33, p. 2797.

  This element specifies a presentation slide that is available within the
  corresponding presentation.

  Parent elements:
  - [done] sldIdLst (§19.2.1.34)

  Child elements:
  - extLst (Extension List)
*/
KoFilter::ConversionStatus PptxXmlDocumentReader::read_sldId()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(id)
    READ_ATTR_WITH_NS(r, id)
    debugPptx << "id:" << id << "r:id:" << r_id;

    // locate this slide
    const QString slidePathAndFile(m_context->relationships->target(m_context->path, m_context->file, r_id));
    debugPptx << "slidePathAndFile:" << slidePathAndFile;

    QString slidePath, slideFile;
    MSOOXML::Utils::splitPathAndFile(slidePathAndFile, &slidePath, &slideFile);

    PptxSlideProperties *slideLayoutProperties = this->slideLayoutProperties(slidePath, slideFile);
    if (!slideLayoutProperties) {
        raiseError(i18n("Slide layout \"%1\" not found", slidePath + '/' + slideFile));
        return KoFilter::WrongFormat;
    }

    VmlDrawingReader vmlreader(this);
    QString vmlTarget = m_context->relationships->targetForType(slidePath, slideFile,
        "http://schemas.openxmlformats.org/officeDocument/2006/relationships/vmlDrawing");

    if (!vmlTarget.isEmpty()) {
        QString errorMessage, vmlPath, vmlFile;

        QString fileName = vmlTarget;
        fileName.remove(0, m_context->path.length());
        MSOOXML::Utils::splitPathAndFile(vmlTarget, &vmlPath, &vmlFile);

        VmlDrawingReaderContext vmlContext(*m_context->import,
            vmlPath, vmlFile, *m_context->relationships);

        const KoFilter::ConversionStatus status =
            m_context->import->loadAndParseDocument(&vmlreader, vmlTarget, errorMessage, &vmlContext);
        if (status != KoFilter::OK) {
            vmlreader.raiseError(errorMessage);
        }
    }

    QString slideMasterPath, slideMasterFile;
    MSOOXML::Utils::splitPathAndFile(m_context->relationships->targetForType(slidePath, slideFile, QLatin1String(MSOOXML::Schemas::officeDocument::relationships) + "/slideLayout"), &slideMasterPath, &slideMasterFile);
    const QString slideMasterPathAndFile = m_context->relationships->targetForType(slideMasterPath, slideMasterFile, QLatin1String(MSOOXML::Schemas::officeDocument::relationships) + "/slideMaster");

    // Delay the reading of a tableStyle until we find a table as we need the
    // clrMap from the master slide
    const QString tableStylesFilePath = m_context->relationships->targetForType(m_context->path, m_context->file, MSOOXML::Relationships::tableStyles);

    PptxSlideProperties *notes = 0;
    const QString notesTarget(m_context->relationships->targetForType(m_context->path, m_context->file,
        "http://schemas.openxmlformats.org/officeDocument/2006/relationships/notesMaster"));
    if (!notesTarget.isEmpty()) {
        notes = &d->notesMasterPageProperties[notesTarget];
    }

    PptxXmlSlideReaderContext context(
        *m_context->import,
        slidePath, slideFile,
        d->slideNumber,
        &d->slideMasterPageProperties[slideLayoutProperties->m_slideMasterName].theme,
        PptxXmlSlideReader::Slide,
        slideLayoutProperties,
        &d->slideMasterPageProperties[slideLayoutProperties->m_slideMasterName],
        notes,
        *m_context->relationships,
        d->commentAuthors,
        d->slideMasterPageProperties[slideLayoutProperties->m_slideMasterName].colorMap,
        vmlreader,
        tableStylesFilePath
    );

    // 1st reading round - read possible colorMap override
    PptxXmlSlideReader slideReader(this);
    context.firstReadingRound = true;

    KoFilter::ConversionStatus status = m_context->import->loadAndParseDocument(
        &slideReader, slidePath + '/' + slideFile, &context);
    if (status != KoFilter::OK) {
        debugPptx << slideReader.errorString();
        return status;
    }

    context.initializeContext(d->slideMasterPageProperties[slideLayoutProperties->m_slideMasterName].theme, defaultParagraphStyles,
        defaultTextStyles, defaultListStyles, defaultBulletColors, defaultTextColors, defaultLatinFonts);

    // 2nd reading round
    context.firstReadingRound = false;
    status = m_context->import->loadAndParseDocument(&slideReader, slidePath + '/' + slideFile, &context);
    if (status != KoFilter::OK) {
        debugPptx << slideReader.errorString();
        return status;
    }

    ++d->slideNumber;

    SKIP_EVERYTHING
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL notesMasterId
//! p:noteMasterId (Note Master ID)
/*!
 Parent elements:
    - [done] noteMasterIdLst (§19.2.1.37)

 Child elements:
    - extLst (Extension List) §19.2.1.12
*/
KoFilter::ConversionStatus PptxXmlDocumentReader::read_notesMasterId()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    READ_ATTR_WITH_NS(r, id)

    const QString notesMasterPathAndFile(m_context->relationships->target(m_context->path, m_context->file, r_id));
    debugPptx << "notesMasterPathAndFile:" << notesMasterPathAndFile;

    QString notesMasterPath, notesMasterFile;
    MSOOXML::Utils::splitPathAndFile(notesMasterPathAndFile, &notesMasterPath, &notesMasterFile);

    // Reading the notesmaster theme
    PptxSlideProperties notesPageProperties;

    const QString notesThemePathAndFile(m_context->relationships->targetForType(
        notesMasterPath, notesMasterFile,
        QLatin1String(MSOOXML::Schemas::officeDocument::relationships) + "/theme"));
    debugPptx << QLatin1String(MSOOXML::Schemas::officeDocument::relationships) + "/theme";
    debugPptx << "notesThemePathAndFile:" << notesThemePathAndFile;

    QString notesThemePath, notesThemeFile;
    MSOOXML::Utils::splitPathAndFile(notesThemePathAndFile, &notesThemePath, &notesThemeFile);

    MSOOXML::MsooXmlThemesReader themesReader(m_writers);
    MSOOXML::MsooXmlThemesReaderContext themecontext(notesPageProperties.theme, m_context->relationships, m_context->import,
        notesThemePath, notesThemeFile);

    QString errorMessage;

    KoFilter::ConversionStatus status
        = m_context->import->loadAndParseDocument(&themesReader, notesThemePathAndFile, errorMessage, &themecontext);

    //empty map used here as slideMaster is the place where the map is created
    QMap<QString, QString> dummyMap;

    VmlDrawingReader vmlreader(this);
    QString vmlTarget = m_context->relationships->targetForType(notesMasterPath, notesMasterFile,
        "http://schemas.openxmlformats.org/officeDocument/2006/relationships/vmlDrawing");

    if (!vmlTarget.isEmpty()) {
        QString errorMessage, vmlPath, vmlFile;

        QString fileName = vmlTarget;
        fileName.remove(0, m_context->path.length());
        MSOOXML::Utils::splitPathAndFile(vmlTarget, &vmlPath, &vmlFile);

        VmlDrawingReaderContext vmlContext(*m_context->import,
            vmlPath, vmlFile, *m_context->relationships);

        const KoFilter::ConversionStatus status =
            m_context->import->loadAndParseDocument(&vmlreader, vmlTarget, errorMessage, &vmlContext);
        if (status != KoFilter::OK) {
            vmlreader.raiseError(errorMessage);
        }
    }

    PptxXmlSlideReaderContext context(
        *m_context->import,
        notesMasterPath, notesMasterFile,
        0, &notesPageProperties.theme,
        PptxXmlSlideReader::NotesMaster,
        0,
        0,
        &notesPageProperties,
        *m_context->relationships,
        d->commentAuthors,
        dummyMap,
        vmlreader,
        QString()
    );

    PptxXmlSlideReader notesMasterReader(this);
    context.firstReadingRound = true;
    status = m_context->import->loadAndParseDocument(
        &notesMasterReader, notesMasterPath + '/' + notesMasterFile, &context);
    if (status != KoFilter::OK) {
        debugPptx << notesMasterReader.errorString();
        return status;
    }

    context.initializeContext(notesPageProperties.theme, defaultParagraphStyles,
        defaultTextStyles, defaultListStyles, defaultBulletColors, defaultTextColors, defaultLatinFonts);

    // In this context we already have the real colorMap
    context.firstReadingRound = false;

    status = m_context->import->loadAndParseDocument(
        &notesMasterReader, notesMasterPath + '/' + notesMasterFile, &context);
    if (status != KoFilter::OK) {
        debugPptx << notesMasterReader.errorString();
        return status;
    }

    d->notesMasterPageProperties.insert(notesMasterPathAndFile, notesPageProperties);

    SKIP_EVERYTHING
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL sldMasterId
//! p:sldMasterId (Slide Master ID)
/*! This element specifies a slide master that is available within the corresponding presentation.
    A slide master is a slide that is specifically designed to be a template for all related child layout slides.

 ECMA-376, 19.2.1.33, p. 2797.
 Parent elements:
    - [done] sldMasterIdLst (§19.2.1.37)
 Child elements:
    - extLst (Extension List) §19.2.1.12
*/
KoFilter::ConversionStatus PptxXmlDocumentReader::read_sldMasterId()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(id)
    READ_ATTR_WITH_NS(r, id)
    debugPptx << "id:" << id << "r:id:" << r_id;

    const QString slideMasterPathAndFile(m_context->relationships->target(m_context->path, m_context->file, r_id));
    debugPptx << "slideMasterPathAndFile:" << slideMasterPathAndFile;

    QString slideMasterPath, slideMasterFile;
    MSOOXML::Utils::splitPathAndFile(slideMasterPathAndFile, &slideMasterPath, &slideMasterFile);

    // Reading the slidemaster theme

    PptxSlideProperties masterPageProperties;

    const QString slideThemePathAndFile(m_context->relationships->targetForType(
        slideMasterPath, slideMasterFile,
        QLatin1String(MSOOXML::Schemas::officeDocument::relationships) + "/theme"));
    debugPptx << QLatin1String(MSOOXML::Schemas::officeDocument::relationships) + "/theme";
    debugPptx << "slideThemePathAndFile:" << slideThemePathAndFile;

    QString slideThemePath, slideThemeFile;
    MSOOXML::Utils::splitPathAndFile(slideThemePathAndFile, &slideThemePath, &slideThemeFile);

    MSOOXML::MsooXmlThemesReader themesReader(m_writers);
    MSOOXML::MsooXmlThemesReaderContext themecontext(masterPageProperties.theme, m_context->relationships, m_context->import,
        slideThemePath, slideThemeFile);

    QString errorMessage;

    KoFilter::ConversionStatus status
        = m_context->import->loadAndParseDocument(&themesReader, slideThemePathAndFile, errorMessage, &themecontext);

    //empty map used here as slideMaster is the place where the map is created
    QMap<QString, QString> dummyMap;

    VmlDrawingReader vmlreader(this);
    QString vmlTarget = m_context->relationships->targetForType(slideMasterPath, slideMasterFile,
        "http://schemas.openxmlformats.org/officeDocument/2006/relationships/vmlDrawing");

    if (!vmlTarget.isEmpty()) {
        QString errorMessage, vmlPath, vmlFile;

        QString fileName = vmlTarget;
        fileName.remove(0, m_context->path.length());
        MSOOXML::Utils::splitPathAndFile(vmlTarget, &vmlPath, &vmlFile);

        VmlDrawingReaderContext vmlContext(*m_context->import,
            vmlPath, vmlFile, *m_context->relationships);

        const KoFilter::ConversionStatus status =
            m_context->import->loadAndParseDocument(&vmlreader, vmlTarget, errorMessage, &vmlContext);
        if (status != KoFilter::OK) {
            vmlreader.raiseError(errorMessage);
        }
    }

    PptxXmlSlideReaderContext context(
        *m_context->import,
        slideMasterPath, slideMasterFile,
        0/*unused*/, &masterPageProperties.theme,
        PptxXmlSlideReader::SlideMaster,
        0,
        &masterPageProperties,
        0,
        *m_context->relationships,
        d->commentAuthors,
        dummyMap,
        vmlreader,
        QString()
    );

    PptxXmlSlideReader slideMasterReader(this);
    context.firstReadingRound = true;
    status = m_context->import->loadAndParseDocument(
        &slideMasterReader, slideMasterPath + '/' + slideMasterFile, &context);
    if (status != KoFilter::OK) {
        debugPptx << slideMasterReader.errorString();
        return status;
    }

    context.initializeContext(masterPageProperties.theme, defaultParagraphStyles,
        defaultTextStyles, defaultListStyles, defaultBulletColors, defaultTextColors, defaultLatinFonts);

    // In this context we already have the real colorMap
    context.firstReadingRound = false;

    status = m_context->import->loadAndParseDocument(
        &slideMasterReader, slideMasterPath + '/' + slideMasterFile, &context);
    if (status != KoFilter::OK) {
        debugPptx << slideMasterReader.errorString();
        return status;
    }

    d->slideMasterPageProperties.insert(slideMasterPathAndFile, masterPageProperties);
    d->masterPageDrawStyleNames.push_back(context.pageDrawStyleName);
    d->masterPageFrames += context.pageFrames;
    debugPptx << "d->masterPageDrawStyleName:" << d->masterPageDrawStyleNames.back();
    SKIP_EVERYTHING
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL sldIdLst
//! p:sldIdLst handler (List of Slide IDs)
/*! ECMA-376, 19.2.1.34, p. 2798.
 Parent elements:
    - [done] presentation (§19.2.1.26)
 Child elements:
    - [done] sldId (Slide ID)
*/
KoFilter::ConversionStatus PptxXmlDocumentReader::read_sldIdLst()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        debugPptx << *this;
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            if (name() == "sldId") {
                TRY_READ(sldId)
                if (m_context->numberOfItems > 0) {
                    m_context->import->reportProgress(100 / m_context->numberOfItems);
                    m_context->numberOfItems = m_context->numberOfItems - 1;
                }
            }
            ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL notesMasterIdLst
//! p:noteMasterIdLst handler (List of Note Master IDs)
/*!

 Parent elements:
 - [done] presentation (§19.2.1.26)

 Child elements:
 - [done] noteMasterId (Note Master ID) §19.2.1.36
*/
KoFilter::ConversionStatus PptxXmlDocumentReader::read_notesMasterIdLst()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            if (name() == "notesMasterId") {
                TRY_READ(notesMasterId)
                if (m_context->numberOfItems > 0) {
                    m_context->import->reportProgress(100 / m_context->numberOfItems);
                    m_context->numberOfItems = m_context->numberOfItems - 1;
                }
            }
            ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL sldMasterIdLst
//! p:sldMasterIdLst handler (List of Slide Master IDs)
/*! ECMA-376, 19.2.1.37, p. 2800
 This element specifies a list of identification information for the slide master slides that
 are available within the corresponding presentation. A slide master is a slide that
 is specifically designed to be a template for all related child layout slides.

 Parent elements:
 - [done] presentation (§19.2.1.26)
 Child elements:
 - [done] sldMasterId (Slide Master ID) §19.2.1.36
*/
KoFilter::ConversionStatus PptxXmlDocumentReader::read_sldMasterIdLst()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            if (name() == "sldMasterId") {
                TRY_READ(sldMasterId)
                if (m_context->numberOfItems > 0) {
                    m_context->import->reportProgress(100 / m_context->numberOfItems);
                    m_context->numberOfItems = m_context->numberOfItems - 1;
                }
            }
            ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL notesSz
//! p:noteSz handler
/*!

 Parent elements:
 - [done] presentation (§19.2.1.26)

 No child elements.
*/
KoFilter::ConversionStatus PptxXmlDocumentReader::read_notesSz()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    READ_ATTR_WITHOUT_NS(cx)
    READ_ATTR_WITHOUT_NS(cy)
    int intCx = 0;
    int intCy = 0;
    STRING_TO_INT(cx, intCx, "notesSz@cx")
    STRING_TO_INT(cy, intCy, "notesSz@cy")
    //! @todo check "type" attr, e.g. 4x3

    d->notePageLayout.width = EMU_TO_POINT(qreal(intCx));
    d->notePageLayout.height = EMU_TO_POINT(qreal(intCy));
    d->notePageLayout.leftMargin = 0.0;
    d->notePageLayout.rightMargin = 0.0;
    d->notePageLayout.topMargin = 0.0;
    d->notePageLayout.bottomMargin = 0.0;

    d->notePageLayout.orientation = d->notePageLayout.width > d->notePageLayout.height
        ? KoPageFormat::Landscape : KoPageFormat::Portrait;

    readNext();

    d->noteSzRead = true;
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL sldSz
//! p:sldSz handler (Presentation)
/*! ECMA-376, 19.2.1.39, p. 2801.
 This element specifies the size of the presentation slide surface.

 Parent elements:
 - [done] presentation (§19.2.1.26)

 No child elements.
*/
KoFilter::ConversionStatus PptxXmlDocumentReader::read_sldSz()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    READ_ATTR_WITHOUT_NS(cx)
    READ_ATTR_WITHOUT_NS(cy)
    int intCx = 0;
    int intCy = 0;
    STRING_TO_INT(cx, intCx, "sldSz@cx")
    STRING_TO_INT(cy, intCy, "sldSz@cy")
    //! @todo check "type" attr, e.g. 4x3

    d->pageLayout.width = EMU_TO_POINT(qreal(intCx));
    d->pageLayout.height = EMU_TO_POINT(qreal(intCy));
    d->pageLayout.leftMargin = 0.0;
    d->pageLayout.rightMargin = 0.0;
    d->pageLayout.topMargin = 0.0;
    d->pageLayout.bottomMargin = 0.0;
    //! @todo orientation heuristics - OK?
    d->pageLayout.orientation = d->pageLayout.width > d->pageLayout.height
        ? KoPageFormat::Landscape : KoPageFormat::Portrait;

    readNext();

    d->sldSzRead = true;
    READ_EPILOGUE
}

//! defaultTextStyle (Presentation Default Text Style)
/*! ECMA-376, 19.2.1.8, p. 2773.

 This element specifies the default text styles that are to be used within the
 presentation.  The text style defined here can be referenced when inserting a
 new slide if that slide is not associated with a master slide or if no styling
 information has been otherwise specified for the text within the presentation
 slide.

 Parent element:
 - [done] presentation (§19.2.1.26)

 Child elements:
 - defPPr (§21.1.2.2.2)
 - extLst (§20.1.2.2.15)
 - [done] lvl1pPr (§21.1.2.4.13)
 - [done] lvl2pPr (§21.1.2.4.14)
 - [done] lvl3pPr (§21.1.2.4.15)
 - [done] lvl4pPr (§21.1.2.4.16)
 - [done] lvl5pPr (§21.1.2.4.17)
 - [done] lvl6pPr (§21.1.2.4.18)
 - [done] lvl7pPr (§21.1.2.4.19)
 - [done] lvl8pPr (§21.1.2.4.20)
 - [done] lvl9pPr (§21.1.2.4.21)
*/
#undef CURRENT_EL
#define CURRENT_EL defaultTextStyle
KoFilter::ConversionStatus PptxXmlDocumentReader::read_defaultTextStyle()
{
    READ_PROLOGUE
    m_currentListStyle = KoGenStyle(KoGenStyle::ListStyle);

    while (!atEnd()) {
        readNext();
        debugPptx << *this;
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            // Initializing the default style for the level.  At the end, there
            // should be 9 levels
            if (qualifiedName().toString().startsWith(QLatin1String("a:lvl"))) {
                defaultTextColors.push_back(QString());
                defaultLatinFonts.push_back(QString());
                defaultBulletColors.push_back(QString());
            }
        }
        if (isStartElement()) {
            TRY_READ_IF_NS(a, lvl1pPr)
            ELSE_TRY_READ_IF_NS(a, lvl2pPr)
            ELSE_TRY_READ_IF_NS(a, lvl3pPr)
            ELSE_TRY_READ_IF_NS(a, lvl4pPr)
            ELSE_TRY_READ_IF_NS(a, lvl5pPr)
            ELSE_TRY_READ_IF_NS(a, lvl6pPr)
            ELSE_TRY_READ_IF_NS(a, lvl7pPr)
            ELSE_TRY_READ_IF_NS(a, lvl8pPr)
            ELSE_TRY_READ_IF_NS(a, lvl9pPr)
//! @todo add ELSE_WRONG_FORMAT
        }
        if (isEndElement()) {
            if (qualifiedName().toString().startsWith(QLatin1String("a:lvl"))) {
                defaultParagraphStyles.push_back(m_currentParagraphStyle);
                defaultTextStyles.push_back(m_currentTextStyle);
                defaultListStyles.push_back(m_currentBulletProperties);
            }
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL presentation
//! p:presentation handler (Presentation)
/*! ECMA-376, 19.2.1.26, p. 2790.
 Root element.
 Child elements:
    - ustDataLst (Customer Data List) §19.3.1.18
    - custShowLst (List of Custom Shows) §19.2.1.7
    - [done] defaultTextStyle (Presentation Default Text Style) §19.2.1.8
    - embeddedFontLst (Embedded Font List) §19.2.1.10
    - extLst (Extension List) §19.2.1.12
    - handoutMasterIdLst (List of Handout Master IDs) §19.2.1.15
    - kinsoku (Kinsoku Settings) §19.2.1.17
    - modifyVerifier (Modification Verifier) §19.2.1.19
    - [done] notesMasterIdLst (List of Notes Master IDs) §19.2.1.21
    - [done] notesSz (Notes Slide Size) §19.2.1.22
    - photoAlbum (Photo Album Information) §19.2.1.24
    - [done] sldIdLst (List of Slide IDs) §19.2.1.34
    - [done] sldMasterIdLst (List of Slide Master IDs) §19.2.1.37
    - [done] sldSz (Presentation Slide Size) §19.2.1.39
    - smartTags (Smart Tags) §19.2.1.40
*/
//! @todo support all child elements
KoFilter::ConversionStatus PptxXmlDocumentReader::read_presentation()
{
    READ_PROLOGUE

    QXmlStreamNamespaceDeclarations namespaces = namespaceDeclarations();
    for (int i = 0; i < namespaces.count(); i++) {
        debugPptx << "NS prefix:" << namespaces[i].prefix() << "uri:" << namespaces[i].namespaceUri();
    }

    if (!m_context->firstReadRound) {
        debugPptx << "======> Second reading round <======";
        while (!atEnd()) {
            readNext();
            debugPptx << *this;
            BREAK_IF_END_OF(CURRENT_EL)
            if (isStartElement()) {
                TRY_READ_IF(sldMasterIdLst)
                ELSE_TRY_READ_IF(sldIdLst)
                ELSE_TRY_READ_IF(sldSz)
                ELSE_TRY_READ_IF(notesMasterIdLst)
                ELSE_TRY_READ_IF(notesSz)
                SKIP_UNKNOWN
//! @todo add ELSE_WRONG_FORMAT
            }
        }
    }
    else {
        while (!atEnd()) {
            readNext();
            BREAK_IF_END_OF(CURRENT_EL)
            if (isStartElement()) {
                TRY_READ_IF(defaultTextStyle)
                SKIP_UNKNOWN
            }
        }
        // TODO: Discuss the font-size logic in case it's not provided in the
        // presentation at the Office Open XML File Format Implementation
        // forum.  The 18pt value is a result of test files analysis.
        KoGenStyle style(KoGenStyle::ParagraphStyle, "paragraph");
        style.setDefaultStyle(true);
        style.addPropertyPt("fo:font-size", 18, KoGenStyle::TextType);

        // TODO: Add all defaults defined by the spec. Values defined by
        // defaultTextStyle/defPPr come on top.
        style.addPropertyPt("fo:margin-left", EMU_TO_POINT(347663));
        style.addPropertyPt("fo:margin-top", 0);
        style.addPropertyPt("fo:margin-right", 0);
        style.addPropertyPt("fo:margin-bottom", 0);
        style.addPropertyPt("fo:text-indent", EMU_TO_POINT(-342900));

        mainStyles->insert(style);

        style = KoGenStyle(KoGenStyle::TextStyle, "text");
        style.setDefaultStyle(true);
        style.addPropertyPt("fo:font-size", 18, KoGenStyle::TextType);
        mainStyles->insert(style);
    }

    if (!m_context->firstReadRound) {
        // There are double the amount of masterPage frames because we read slideMaster always twice
        // This means that first frame of the set is always empty and is skipped in the loop
        unsigned frameCount = d->masterPageFrames.size() / 2;
        unsigned index = 0;
        while (index < frameCount) {
            d->masterPageStyles.push_back(KoGenStyle(KoGenStyle::MasterPageStyle));
            if (d->sldSzRead) {
                KoGenStyle pageLayoutStyle(d->pageLayout.saveOdf());
                pageLayoutStyle.setAutoStyleInStylesDotXml(true);
                const QString pageLayoutStyleName(mainStyles->insert(pageLayoutStyle, "PM"));
                debugPptx << "pageLayoutStyleName:" << pageLayoutStyleName;

                d->masterPageStyles[index].addAttribute("style:page-layout-name", pageLayoutStyleName);
            }
            if (!d->masterPageDrawStyleNames.at(index).isEmpty()) {
               d->masterPageStyles[index].addAttribute("draw:style-name", d->masterPageDrawStyleNames.at(index));
            }
            d->masterPageStyles[index].addChildElement(QString("frame-2-%1").arg(index), d->masterPageFrames.at((1+index)*2-1));
            const QString masterPageStyleName(mainStyles->insert(d->masterPageStyles.at(index), "slideMaster"));
            ++index;
        }
    } else {
        m_context->numberOfItems = m_context->relationships->targetCountWithWord("slideMasters") +
                                   m_context->relationships->targetCountWithWord("notesMasters") +
                                   m_context->relationships->targetCountWithWord("slides");
    }

    READ_EPILOGUE
}

#define blipFill_NS "a"

// END NAMESPACE p

// BEGIN NAMESPACE a

#undef MSOOXML_CURRENT_NS
#define MSOOXML_CURRENT_NS "a"

#include <MsooXmlCommonReaderImpl.h> // this adds a:p, a:pPr, a:t, a:r, etc.

#define DRAWINGML_NS "a"
#define DRAWINGML_PIC_NS "p" // DrawingML/Picture

#include <MsooXmlCommonReaderDrawingMLImpl.h> // this adds p:pic, etc.
