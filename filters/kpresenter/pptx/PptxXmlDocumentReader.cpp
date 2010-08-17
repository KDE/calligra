/*
 * This file is part of Office 2007 Filters for KOffice
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
#include "PptxXmlSlideReader.h"
#include "PptxXmlCommentAuthorsReader.h"
#include "PptxImport.h"
#include <MsooXmlRelationships.h>
#include <MsooXmlSchemas.h>
#include <MsooXmlUtils.h>
#include <MsooXmlUnits.h>
#include <MsooXmlDrawingTableStyleReader.h>
#include <KoXmlWriter.h>
#include <KoGenStyles.h>
#include <KoPageLayout.h>

#define MSOOXML_CURRENT_NS "p"
#define MSOOXML_CURRENT_CLASS PptxXmlDocumentReader
#define BIND_READ_CLASS MSOOXML_CURRENT_CLASS

#include <MsooXmlReader_p.h>

PptxXmlDocumentReaderContext::PptxXmlDocumentReaderContext(
    PptxImport& _import, const QMap<QString, MSOOXML::DrawingMLTheme*>& _themes,
    const QString& _path, const QString& _file,
    MSOOXML::MsooXmlRelationships& _relationships)
        : import(&_import), themes(&_themes),
          path(_path), file(_file), relationships(&_relationships)
{
}

class PptxXmlDocumentReader::Private
{
public:
    Private()
    {
    }
    ~Private() {
        qDeleteAll(masterSlidePropertiesMap);
        qDeleteAll(slideLayoutPropertiesMap);
    }
    QMap<QString, PptxSlideProperties*> masterSlidePropertiesMap;
    QMap<QString, PptxSlideLayoutProperties*> slideLayoutPropertiesMap;
    uint slideNumber; //!< temp., see todo in PptxXmlDocumentReader::read_sldId()
    bool sldSzRead;
    KoPageLayout pageLayout;
    QString masterPageDrawStyleName;
    KoGenStyle masterPageStyle;
    PptxSlideMasterPageProperties slideMasterPageProperties;
    QMap<int,QString> commentAuthors;
    MSOOXML::TableStyleList* tableStyleList;
private:
};

PptxXmlDocumentReader::PptxXmlDocumentReader(KoOdfWriters *writers)
        : MSOOXML::MsooXmlReader(writers)
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
    d->pageLayout = KoPageLayout();
    d->masterPageDrawStyleName.clear();
    d->masterPageStyle = KoGenStyle(KoGenStyle::MasterPageStyle);
    d->slideMasterPageProperties.clear();

    const KoFilter::ConversionStatus result = readInternal();

    m_context = 0;
    return result;
}

KoFilter::ConversionStatus PptxXmlDocumentReader::readInternal()
{
    kDebug() << "=============================";
    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }

    // p:presentation
    readNext();
    kDebug() << *this << namespaceUri();

    if (!expectEl("p:presentation")) {
        return KoFilter::WrongFormat;
    }
    if (!expectNS(MSOOXML::Schemas::presentationml)) {
        return KoFilter::WrongFormat;
    }
    /*
        const QXmlStreamAttributes attrs( attributes() );
        for (int i=0; i<attrs.count(); i++) {
            kDebug() << "1 NS prefix:" << attrs[i].name() << "uri:" << attrs[i].namespaceUri();
        }*/

    QXmlStreamNamespaceDeclarations namespaces(namespaceDeclarations());
    for (int i = 0; i < namespaces.count(); i++) {
        kDebug() << "NS prefix:" << namespaces[i].prefix() << "uri:" << namespaces[i].namespaceUri();
    }
//! @todo find out whether the namespace returned by namespaceUri()
//!       is exactly the same ref as the element of namespaceDeclarations()
    if (!namespaces.contains(QXmlStreamNamespaceDeclaration("p", MSOOXML::Schemas::presentationml))) {
        raiseError(i18n("Namespace \"%1\" not found", MSOOXML::Schemas::presentationml));
        return KoFilter::WrongFormat;
    }
//! @todo expect other namespaces too...

    PptxXmlCommentAuthorsReader autorsReader(this);
    const QString autorsFilePath = m_context->relationships->targetForType(m_context->path, m_context->file, MSOOXML::Relationships::commentAuthors);
    PptxXmlCommentAuthorsReaderContext autorsContext;
    m_context->import->loadAndParseDocument(&autorsReader, autorsFilePath, &autorsContext);
    d->commentAuthors = autorsContext.authors;

    MSOOXML::MsooXmlDrawingTableStyleReader tableStyleReader(this);
    d->tableStyleList = new MSOOXML::TableStyleList();
    MSOOXML::MsooXmlDrawingTableStyleContext tableStyleReaderContext;
    tableStyleReaderContext.styleList = d->tableStyleList;
    const QString tableStylesFilePath = m_context->relationships->targetForType(m_context->path, m_context->file, MSOOXML::Relationships::commentAuthors);
    m_context->import->loadAndParseDocument(&tableStyleReader, tableStylesFilePath, &tableStyleReaderContext);

    TRY_READ(presentation)
    kDebug() << "===========finished============";
    return KoFilter::OK;
}

PptxSlideLayoutProperties* PptxXmlDocumentReader::slideLayoutProperties(
    const QString& slidePath, const QString& slideFile)
{
    

    const QString slideLayoutPathAndFile(m_context->relationships->targetForType(
        slidePath, slideFile,
        QLatin1String(MSOOXML::Schemas::officeDocument::relationships) + "/slideLayout"));
    kDebug() << QLatin1String(MSOOXML::Schemas::officeDocument::relationships) + "/slideLayout";
    kDebug() << "slideLayoutPathAndFile:" << slideLayoutPathAndFile;
    if (slideLayoutPathAndFile.isEmpty())
        return 0;

    QString slideLayoutPath, slideLayoutFile;
    MSOOXML::Utils::splitPathAndFile(slideLayoutPathAndFile, &slideLayoutPath, &slideLayoutFile);

    // load layout or find in cache
    PptxSlideLayoutProperties *result = d->slideLayoutPropertiesMap.value(slideLayoutPathAndFile);
    if (result)
        return result;

    QString slideMasterPath, slideMasterFile;
    MSOOXML::Utils::splitPathAndFile(m_context->relationships->targetForType(slidePath, slideFile, QLatin1String(MSOOXML::Schemas::officeDocument::relationships) + "/slideLayout"), &slideMasterPath, &slideMasterFile);
    const QString slideMasterPathAndFile = m_context->relationships->targetForType(slideMasterPath, slideMasterFile, QLatin1String(MSOOXML::Schemas::officeDocument::relationships) + "/slideMaster");
    PptxSlideProperties *masterSlideProperties = d->masterSlidePropertiesMap.contains(slideMasterPathAndFile) ? d->masterSlidePropertiesMap[slideMasterPathAndFile] : 0;

    result = new PptxSlideLayoutProperties();
    MSOOXML::Utils::AutoPtrSetter<PptxSlideLayoutProperties> slideLayoutPropertiesSetter(result);
    PptxXmlSlideReaderContext context(
        *m_context->import,
        slideLayoutPath, slideLayoutFile,
        0/*unused*/, *m_context->themes,
        PptxXmlSlideReader::SlideLayout,
        masterSlideProperties, //PptxSlideProperties
        result,
        &d->slideMasterPageProperties, //PptxSlideMasterPageProperties
        *m_context->relationships,
        d->commentAuthors
    );
    PptxXmlSlideReader slideLayoutReader(this);
    KoFilter::ConversionStatus status = m_context->import->loadAndParseDocument(
        &slideLayoutReader, slideLayoutPath + "/" + slideLayoutFile, &context);
    if (status != KoFilter::OK) {
        kDebug() << slideLayoutReader.errorString();
        return 0;
    }
    slideLayoutPropertiesSetter.release();
    d->slideLayoutPropertiesMap.insert(slideLayoutPathAndFile, result);
    return result;
}

#undef CURRENT_EL
#define CURRENT_EL sldId
//! p:sldId handler (Slide ID)
/*! This element specifies a presentation slide that is available within the corresponding presentation.
 ECMA-376, 19.2.1.33, p. 2797.
 Parent elements:
    - [done] sldIdLst (§19.2.1.34)
 Child elements:
    - extLst (Extension List)
*/
KoFilter::ConversionStatus PptxXmlDocumentReader::read_sldId()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    READ_ATTR_WITHOUT_NS(id)
    READ_ATTR_WITH_NS(r, id)
    kDebug() << "id:" << id << "r:id:" << r_id;

    // locate this slide
    const QString slidePathAndFile(m_context->relationships->target(m_context->path, m_context->file, r_id));
    kDebug() << "slidePathAndFile:" << slidePathAndFile;

    QString slidePath, slideFile;
    MSOOXML::Utils::splitPathAndFile(slidePathAndFile, &slidePath, &slideFile);

    PptxSlideLayoutProperties *slideLayoutProperties = this->slideLayoutProperties(slidePath, slideFile);
    if (!slideLayoutProperties) {
        raiseError(i18n("Slide layout \"%1\" not found", slidePath + '/' + slideFile));
        return KoFilter::WrongFormat;
    }
    
    QString slideMasterPath, slideMasterFile;
    MSOOXML::Utils::splitPathAndFile(m_context->relationships->targetForType(slidePath, slideFile, QLatin1String(MSOOXML::Schemas::officeDocument::relationships) + "/slideLayout"), &slideMasterPath, &slideMasterFile);
    const QString slideMasterPathAndFile = m_context->relationships->targetForType(slideMasterPath, slideMasterFile, QLatin1String(MSOOXML::Schemas::officeDocument::relationships) + "/slideMaster");
    PptxSlideProperties *masterSlideProperties = d->masterSlidePropertiesMap.contains(slideMasterPathAndFile) ? d->masterSlidePropertiesMap[slideMasterPathAndFile] : 0;
    
    PptxXmlSlideReaderContext context(
        *m_context->import,
        slidePath, slideFile,
        d->slideNumber,
        *m_context->themes,
        PptxXmlSlideReader::Slide,
        masterSlideProperties,
        slideLayoutProperties,
        &d->slideMasterPageProperties,
        *m_context->relationships,
        d->commentAuthors
    );
    PptxXmlSlideReader slideReader(this);
    KoFilter::ConversionStatus status = m_context->import->loadAndParseDocument(
        &slideReader, slidePath + '/' + slideFile, &context);
    if (status != KoFilter::OK) {
        kDebug() << slideReader.errorString();
        return status;
    }
    ++d->slideNumber;

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
    READ_ATTR_WITHOUT_NS(id)
    READ_ATTR_WITH_NS(r, id)
    kDebug() << "id:" << id << "r:id:" << r_id;

    const QString slideMasterPathAndFile(m_context->relationships->target(m_context->path, m_context->file, r_id));
    kDebug() << "slideMasterPathAndFile:" << slideMasterPathAndFile;

    QString slideMasterPath, slideMasterFile;
    MSOOXML::Utils::splitPathAndFile(slideMasterPathAndFile, &slideMasterPath, &slideMasterFile);

    PptxSlideProperties *masterSlideProperties = new PptxSlideProperties();
    MSOOXML::Utils::AutoPtrSetter<PptxSlideProperties> masterSlidePropertiesSetter(masterSlideProperties);
    PptxXmlSlideReaderContext context(
        *m_context->import,
        slideMasterPath, slideMasterFile,
        0/*unused*/, *m_context->themes,
        PptxXmlSlideReader::SlideMaster,
        masterSlideProperties,
        0,
        &d->slideMasterPageProperties,
        *m_context->relationships,
        d->commentAuthors
    );
    PptxXmlSlideReader slideMasterReader(this);
    KoFilter::ConversionStatus status = m_context->import->loadAndParseDocument(
        &slideMasterReader, slideMasterPath + "/" + slideMasterFile, &context);
    if (status != KoFilter::OK) {
        kDebug() << slideMasterReader.errorString();
        return status;
    }
    d->masterSlidePropertiesMap.insert(slideMasterPathAndFile, masterSlideProperties);
    masterSlidePropertiesSetter.release();
    d->masterPageDrawStyleName = context.pageDrawStyleName;
    kDebug() << "d->masterPageDrawStyleName:" << d->masterPageDrawStyleName;
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
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(sldId)
            ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
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
        if (isStartElement()) {
            TRY_READ_IF(sldMasterId)
            ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

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
//! @todo support all child elements
KoFilter::ConversionStatus PptxXmlDocumentReader::read_sldSz()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    READ_ATTR_WITHOUT_NS(cx)
    READ_ATTR_WITHOUT_NS(cy)
    int intCx, intCy;
    STRING_TO_INT(cx, intCx, "sldSz@cx")
    STRING_TO_INT(cy, intCy, "sldSz@cy")
    //! @todo check "type" attr, e.g. 4x3

    d->pageLayout.width = EMU_TO_POINT(qreal(intCx));
    d->pageLayout.height = EMU_TO_POINT(qreal(intCy));
    d->pageLayout.leftMargin = 0.0;
    d->pageLayout.rightMargin = 0.0;
    d->pageLayout.topMargin = 0.0;
    d->pageLayout.bottomMargin = 0.0;
    //! @todo orientation heristics - OK?
    d->pageLayout.orientation = d->pageLayout.width > d->pageLayout.height
        ? KoPageFormat::Landscape : KoPageFormat::Portrait;

    while (true) {
        BREAK_IF_END_OF(CURRENT_EL);
        readNext();
    }

    d->sldSzRead = true;
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
    - defaultTextStyle (Presentation Default Text Style) §19.2.1.8
    - embeddedFontLst (Embedded Font List) §19.2.1.10
    - extLst (Extension List) §19.2.1.12
    - handoutMasterIdLst (List of Handout Master IDs) §19.2.1.15
    - kinsoku (Kinsoku Settings) §19.2.1.17
    - modifyVerifier (Modification Verifier) §19.2.1.19
    - notesMasterIdLst (List of Notes Master IDs) §19.2.1.21
    - notesSz (Notes Slide Size) §19.2.1.22
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
        kDebug() << "NS prefix:" << namespaces[i].prefix() << "uri:" << namespaces[i].namespaceUri();
    }

    while (!atEnd()) {
//        kDebug() << "!!" << qualifiedName() << JOIN(MSOOXML_CURRENT_NS ":", CURRENT_EL);
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(sldMasterIdLst)
            ELSE_TRY_READ_IF(sldIdLst)
            ELSE_TRY_READ_IF(sldSz)
            //! @todo ELSE_TRY_READ_IF(notesSz)
            //! @todo ELSE_TRY_READ_IF(defaultTextStyle)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    if (d->sldSzRead) {
        KoGenStyle pageLayoutStyle(d->pageLayout.saveOdf());
        const QString pageLayoutStyleName(mainStyles->insert(pageLayoutStyle, "PM"));
        mainStyles->markStyleForStylesXml(pageLayoutStyleName);
        kDebug() << "pageLayoutStyleName:" << pageLayoutStyleName;

        //! @todo insert draw:frame child elements read from master slide
        //<style:master-page style:name="Default" style:page-layout-name="PMpredef1" draw:style-name="dppredef1">
        // <draw:frame presentation:style-name=...
        d->masterPageStyle.addAttribute("style:page-layout-name", pageLayoutStyleName);
    }
    if (!d->masterPageDrawStyleName.isEmpty()) {
        d->masterPageStyle.addAttribute("draw:style-name", d->masterPageDrawStyleName);
    }
    const QString masterPageStyleName(
        mainStyles->insert(d->masterPageStyle, "Default", KoGenStyles::DontAddNumberToName));
    //mainStyles->markStyleForStylesXml(masterPageStyleName);
    kDebug() << "masterPageStyleName:" << masterPageStyleName;
    READ_EPILOGUE
}
