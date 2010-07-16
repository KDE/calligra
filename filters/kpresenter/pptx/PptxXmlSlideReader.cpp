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

#include "PptxXmlSlideReader.h"
#include "PptxImport.h"

#include "Charting.h"
#include "ChartExport.h"
#include "XlsxXmlChartReader.h"
#include "PptxXmlCommentsReader.h"

#include <MsooXmlSchemas.h>
#include <MsooXmlUtils.h>
#include <MsooXmlRelationships.h>
#include <MsooXmlUnits.h>

#include <KoXmlWriter.h>
#include <KoGenStyles.h>
#include <KoOdfGraphicStyles.h>
#include <KoUnit.h>

#include <kde_file.h> // for WARNING

#include <QBrush>

#define MSOOXML_CURRENT_NS "p"
#define MSOOXML_CURRENT_CLASS PptxXmlSlideReader
#define BIND_READ_CLASS MSOOXML_CURRENT_CLASS
#define PPTXXMLSLIDEREADER_CPP

#ifdef __GNUC__
#warning Using hardcoded presentation:style-name attributes: pr1; pr2
#else
#pragma WARNING( Using hardcoded presentation:style-name attributes: pr1; pr2 )
#endif
//#define HARDCODED_PRESENTATIONSTYLENAME

#include <MsooXmlReader_p.h>
#include <MsooXmlContentTypes.h>

PptxShapeProperties::PptxShapeProperties()
{
    x = 0;
    y = 0;
    width = -1;
    height = -1;
    rot = 0;
    isPlaceHolder = false;
}

PptxShapeProperties::PptxShapeProperties(const PptxShapeProperties &other)
{
    *this = other;
}

PptxShapeProperties& PptxShapeProperties::operator=(const PptxShapeProperties &other)
{
    x = other.x;
    y = other.y;
    width = other.width;
    height = other.height;
    rot = other.rot;
    isPlaceHolder = other.isPlaceHolder;
    return *this;
}
    
// -------------------

PptxSlideProperties::PptxSlideProperties()
{
}

PptxSlideProperties::~PptxSlideProperties()
{
    qDeleteAll(shapes);
}

void PptxSlideProperties::clear()
{
    qDeleteAll(shapes);
    shapes.clear();
    shapesMap.clear();
}

// -------------------

PptxPlaceholder::PptxPlaceholder()
{
}

PptxPlaceholder::PptxPlaceholder(const PptxShapeProperties &other)
 : x(EMU_TO_CM_STRING(other.x))
 , y(EMU_TO_CM_STRING(other.y))
 , width(EMU_TO_CM_STRING(other.width))
 , height(EMU_TO_CM_STRING(other.height))
{
    kDebug() << x << y << width << height;
}

PptxPlaceholder::~PptxPlaceholder()
{
}

void PptxPlaceholder::writeAttributes(KoXmlWriter* writer)
{
    writer->addAttribute("svg:x", x);
    writer->addAttribute("svg:y", y);
    writer->addAttribute("svg:width", width);
    writer->addAttribute("svg:height", height);
}

// -------------------

PptxSlideMasterListLevelTextStyle::PptxSlideMasterListLevelTextStyle()
{
}

// -------------------

PptxSlideMasterTextStyle::PptxSlideMasterTextStyle()
 : m_listStyles(9)
{
}

PptxSlideMasterTextStyle::~PptxSlideMasterTextStyle()
{
    for (int i = 0; i < m_listStyles.size(); i++) {
        delete m_listStyles[i];
    }
}

PptxSlideMasterListLevelTextStyle* PptxSlideMasterTextStyle::listStyle(uint level)
{
    if (level < 1 || level > (uint)m_listStyles.size())
        return 0;
    PptxSlideMasterListLevelTextStyle* result = m_listStyles.at(level - 1);
    if (!result) {
        m_listStyles[level - 1] = result = new PptxSlideMasterListLevelTextStyle;
    }
    return result;
}

// -------------------

PptxSlideLayoutProperties::PptxSlideLayoutProperties()
{
}

PptxSlideLayoutProperties::~PptxSlideLayoutProperties()
{
    qDeleteAll(shapes);
    qDeleteAll(placeholders);
}

PptxSlideMasterPageProperties::PptxSlideMasterPageProperties()
{
}

void PptxSlideMasterPageProperties::addDrawingPageProperty(
    const QByteArray& property, const QByteArray& value)
{
    kDebug() << "Inserting" << property << "->" << value;
    m_drawingPageProperties.insert(property, value);
}

void PptxSlideMasterPageProperties::saveDrawingPageProperties(KoGenStyle* style)
{
    for (QMap<QByteArray, QByteArray>::ConstIterator it = m_drawingPageProperties.constBegin();
         it != m_drawingPageProperties.constEnd(); ++it)
    {
        style->addProperty(QString(it.key()), it.value().constData());
    }
}

PptxSlideMasterTextStyle* PptxSlideMasterPageProperties::textStyle(const QString& style)
{
    if (style == QLatin1String("title") || style == QLatin1String("ctrTitle")) {
        return &titleStyle;
    }
    else if (style == "body" || style == "subTitle") {
        return &bodyStyle;
    }
    return &otherStyle;
}

// -------------------

PptxXmlSlideReaderContext::PptxXmlSlideReaderContext(
    PptxImport& _import, const QString& _path, const QString& _file,
    uint _slideNumber, const QMap<QString, MSOOXML::DrawingMLTheme*>& _themes,
    PptxXmlSlideReader::Type _type, PptxSlideProperties* _slideProperties,
    PptxSlideLayoutProperties* _slideLayoutProperties,
    PptxSlideMasterPageProperties* _slideMasterPageProperties,
    MSOOXML::MsooXmlRelationships& _relationships,
    QMap<int, QString> _commentAuthors)
        : MSOOXML::MsooXmlReaderContext(&_relationships),
        import(&_import), path(_path), file(_file),
        slideNumber(_slideNumber), themes(&_themes), type(_type),
        slideProperties(_slideProperties), slideLayoutProperties(_slideLayoutProperties),
        slideMasterPageProperties(_slideMasterPageProperties),
        commentAuthors(_commentAuthors)
{
}

class PptxXmlSlideReader::Private
{
public:
    Private() {
    }
    ~Private() {
    }
    KoXmlWriter *body; //!< Backup body pointer for SlideMaster mode
#ifdef HARDCODED_PRESENTATIONSTYLENAME
    uint presentationStyleNameCount;
#endif
    //! Used to index shapes in master slide when inheriting properties
    uint shapeNumber;
    QString qualifiedNameOfMainElement;
    QString phType; //!< set by read_ph()
    QString phIdx; //!< set by read_ph()
    QString phStyleId() const
    {
        if (!phType.isEmpty())
            return phType;
        return phIdx;
    }

    PptxSlideMasterTextStyle* currentSlideMasterTextStyle; //!< set by read_*Style()
    //!set by read_t as true whenever some characters are copied to a textbox,
    //!used to figure out if a shape is a placeholder or not
    bool textBoxHasContent;
};

PptxXmlSlideReader::PptxXmlSlideReader(KoOdfWriters *writers)
        : MSOOXML::MsooXmlCommonReader(writers)
        , m_context(0)
        , m_currentShapeProperties(0)
        , m_placeholderElWriter(0)
        , d(new Private)
{
    init();
}

PptxXmlSlideReader::~PptxXmlSlideReader()
{
    doneInternal(); // MsooXmlCommonReaderImpl.h
    delete d;
}

void PptxXmlSlideReader::init()
{
    initInternal(); // MsooXmlCommonReaderImpl.h
    initDrawingML();
    m_defaultNamespace = QLatin1String(MSOOXML_CURRENT_NS ":");
}

KoFilter::ConversionStatus PptxXmlSlideReader::read(MSOOXML::MsooXmlReaderContext* context)
{
    m_context = dynamic_cast<PptxXmlSlideReaderContext*>(context);
    Q_ASSERT(m_context);
    d->currentSlideMasterTextStyle = 0;
    switch (m_context->type) {
    case Slide:
        d->qualifiedNameOfMainElement = "p:sld";
        break;
    case SlideLayout:
        d->qualifiedNameOfMainElement = "p:sldLayout";
        break;
    case SlideMaster:
        d->qualifiedNameOfMainElement = "p:sldMaster";
        break;
    }
#if 0
    if (m_context->slideProperties) {
        kDebug() << "m_context->slideProperties->shapes.count()" << m_context->slideProperties->shapes.count();
        if (m_context->type == SlideMaster) { // will be written
            m_context->slideProperties->clear();
        }
    }
#endif
    const KoFilter::ConversionStatus result = readInternal();
    /*    if (m_context->type == SlideMaster) {
            if (result == KoFilter::OK) {
                m_context->slideProperties->clear();
            }
        }*/
#if 0
    if (m_context->slideProperties) {
        kDebug() << "m_context->slideProperties->shapes.count()" << m_context->slideProperties->shapes.count();
    }
#endif
    m_context = 0;
    return result;
}

KoFilter::ConversionStatus PptxXmlSlideReader::readInternal()
{
    kDebug() << "=============================";
    if (m_context->type == SlideMaster) {
        //! Clear body pointer for SlideMaster mode: avoid writting to body by mistake in this mode
        d->body = body;
      //  body = 0;
    }

    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }

    // p:sld{Master}
    readNext();
    kDebug() << *this << namespaceUri();

    if (!expectEl(d->qualifiedNameOfMainElement)) {
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

    switch (m_context->type) {
    case Slide:
        TRY_READ(sld)
        break;
    case SlideLayout:
        TRY_READ(sldLayout)
        break;
    case SlideMaster:
        TRY_READ(sldMaster)
        break;
    }
    kDebug() << "===========finished============";
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL sld
//! sld handler (Presentation Slide)
/*! This element specifies a slide.
 ECMA-376, 19.3.1.38, p. 2849.
 Root element of PresentationML Slide part.

 Child elements:
    - clrMapOvr (Color Scheme Map Override) §19.3.1.7
    - [done] cSld (Common Slide Data) §19.3.1.16
    - extLst (Extension List with Modification Flag) §19.3.1.20
    - timing (Slide Timing Information for a Slide Layout) §19.3.1.48
    - transition (Slide Transition for a Slide Layout) §19.3.1.50
*/
//! @todo support all child elements
//! CASE #P300
KoFilter::ConversionStatus PptxXmlSlideReader::read_sld()
{
    READ_PROLOGUE
    RETURN_IF_ERROR( read_sldInternal() )
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL sldMaster
//! sldMaster handler (Slide Master)
/*! ECMA-376, 19.3.1.42, p. 2853.
 This element specifies an instance of a slide master slide.

 Child elements:
    - clrMap (Color Scheme Map) §19.3.1.6
    - [done] cSld (Common Slide Data) §19.3.1.16
    - extLst (Extension List with Modification Flag) §19.3.1.20
    - hf (Header/Footer information for a slide master) §19.3.1.25
    - sldLayoutIdLst (List of Slide Layouts) §19.3.1.41
    - timing (Slide Timing Information for a Slide Layout) §19.3.1.48
    - transition (Slide Transition for a Slide Layout) §19.3.1.50
    - [done] txStyles (Slide Master Text Styles) §19.3.1.52
*/
//! @todo support all child elements
KoFilter::ConversionStatus PptxXmlSlideReader::read_sldMaster()
{
    READ_PROLOGUE
    RETURN_IF_ERROR( read_sldInternal() )
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL sldLayout
//! sldLayout handler (Slide Layout)
/*! ECMA-376, 19.3.1.39, p. 2851.
 This element specifies an instance of a slide layout. The slide layout contains
 in essence a template slide design that can be applied to any existing slide.
 When applied to an existing slide all corresponding content should be mapped to the new slide layout.

 Root element of PresentationML Slide Layout part.

 Child elements:
 - clrMapOvr (Color Scheme Map Override) §19.3.1.7
 - [done] cSld (Common Slide Data) §19.3.1.16
 - extLst (Extension List with Modification Flag) §19.3.1.20
 - hf (Header/Footer information for a slide master) §19.3.1.25
 - timing (Slide Timing Information for a Slide Layout) §19.3.1.48
 - transition (Slide Transition for a Slide Layout) §19.3.1.50
*/
//! @todo support all child elements
KoFilter::ConversionStatus PptxXmlSlideReader::read_sldLayout()
{
    READ_PROLOGUE
    RETURN_IF_ERROR( read_sldInternal() )
    READ_EPILOGUE
}

//! CASE #P300
KoFilter::ConversionStatus PptxXmlSlideReader::read_sldInternal()
{
    QXmlStreamNamespaceDeclarations namespaces = namespaceDeclarations();
    for (int i = 0; i < namespaces.count(); i++) {
        kDebug() << "NS prefix:" << namespaces[i].prefix() << "uri:" << namespaces[i].namespaceUri();
    }

    if (m_context->type == Slide) {
#if 0
    QString slidePath, slideFile;
    MSOOXML::Utils::splitPathAndFile(masterSlidePathAndFile, &slidePath, &slideFile);

    PptxXmlSlideReaderContext context(
        *m_context->import,
        slidePath, slideFile,
        0/*unused*/, *m_context->themes,
        PptxXmlSlideReader::SlideMaster,
        0, /*slideProperties*/
        *m_context->relationships
    );
    PptxXmlSlideReader slideReader(this);
    KoFilter::ConversionStatus status = m_context->import->loadAndParseDocument(
        &slideReader, slidePath + "/" + slideFile, &context);
    if (status != KoFilter::OK) {
        kDebug() << slideReader.errorString();
        return status;
    }
        
        

#endif

        // m_currentDrawStyle defined in "MsooXmlCommonReaderDrawingMLMethods.h"
        m_currentDrawStyle = new KoGenStyle(KoGenStyle::DrawingPageAutoStyle, "drawing-page"); // CASE #P109
        m_currentDrawStyle->addProperty("presentation:background-visible", true);   // CASE #P111
        m_currentDrawStyle->addProperty("presentation:background-objects-visible", true);   // CASE #P112
        m_context->slideMasterPageProperties->saveDrawingPageProperties(m_currentDrawStyle);
    }
    else if (m_context->type == SlideMaster) {
        m_currentDrawStyle = new KoGenStyle(KoGenStyle::DrawingPageAutoStyle, "drawing-page");
        m_currentDrawStyle->addProperty("presentation:visibility", "visible");
        m_currentDrawStyle->addProperty("presentation:background-objects-visible", false);
    }
    else if (m_context->type == SlideLayout) {
        m_currentPresentationPageLayoutStyle = KoGenStyle(KoGenStyle::PresentationPageLayoutStyle);
    }

    MSOOXML::Utils::XmlWriteBuffer drawPageBuf; // buffer this draw:page, because we have to compute
    // style before style name is known
//        KoXmlWriter *origBody = body;
    if (m_context->type == Slide) {
        body = drawPageBuf.setWriter(body);
//            body = new KoXmlWriter(&drawPageBuf, origBody->indentLevel()+1);
    }
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            TRY_READ_IF(cSld)
            else if (m_context->type == SlideMaster && QUALIFIED_NAME_IS(txStyles)) {
                TRY_READ(txStyles)
            }
//! @todo add ELSE_WRONG_FORMAT
        }
        if (isEndElement()) {
            if (d->qualifiedNameOfMainElement == qualifiedName()) {
                break;
            }
        }
    }

    if (m_context->type == Slide) {
        body = drawPageBuf.originalWriter();
//            delete body;
//            body = origBody;

        body->startElement("draw:page"); // CASE #P300
        //! @todo draw:master-page-name is hardcoded for now
        body->addAttribute("draw:master-page-name", "Default"); // required; CASE #P301
        //! @todo draw:name can be pulled out of docProps/app.xml (TitlesOfParts)
        body->addAttribute("draw:name", i18n("Slide %1",m_context->slideNumber+1)); //optional; CASE #P303
        body->addAttribute("draw:id", QString("pid%1").arg(m_context->slideNumber)); //optional; unique ID; CASE #P305, #P306
        //! @todo presentation:use-date-time-name //optional; CASE #P304

/*
<style:style style:family="drawing-page" style:name="a393">
    <style:drawing-page-properties draw:fill="bitmap" draw:fill-image-name="a392" style:repeat="readerh".
    presentation:visibility="visible" draw:background-size="border" presentation:background-objects-visible="true".
    presentation:background-visible="true" presentation:display-header="false" presentation:display-footer="false" presentation:display-page-number="false".
    presentation:display-date-time="false"/>
</style:style>
*/
            
        const QString currentPageStyleName(mainStyles->insert(*m_currentDrawStyle, "dp"));
        body->addAttribute("draw:style-name", currentPageStyleName); // CASE #P302
        kDebug() << "currentPageStyleName:" << currentPageStyleName;

        if (!m_context->slideLayoutProperties->styleName.isEmpty()) {
            // CASE #P308
            kDebug() << "presentation:presentation-page-layout-name=" <<
                                m_context->slideLayoutProperties->styleName;
            body->addAttribute("presentation:presentation-page-layout-name",
                                m_context->slideLayoutProperties->styleName);
        }

//                body->addCompleteElement(&drawPageBuf);
        (void)drawPageBuf.releaseWriter();

        {
            PptxXmlCommentsReader commentsReader(this);
            const QString filepath = m_context->relationships->targetForType(m_context->path, m_context->file, MSOOXML::Relationships::comments);
            PptxXmlCommentsReaderContext commentsContext;
            commentsContext.authors = m_context->commentAuthors;
            (void)m_context->import->loadAndParseDocument(&commentsReader, filepath, &commentsContext);
        }

        body->endElement(); //draw:page
    }
    else if (m_context->type == SlideMaster) {
        m_context->pageDrawStyleName = mainStyles->insert(*m_currentDrawStyle, "dp");
        mainStyles->markStyleForStylesXml(m_context->pageDrawStyleName);
        kDebug() << "m_context->pageDrawStyleName:" << m_context->pageDrawStyleName
            << "m_context->type:" << m_context->type;
    }
    else if (m_context->type == SlideLayout) {
        m_context->slideLayoutProperties->styleName = mainStyles->insert(m_currentPresentationPageLayoutStyle);
        kDebug() << "slideLayoutProperties->styleName:" << m_context->slideLayoutProperties->styleName;
    }

    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL txStyles
//! txStyles handler (Slide Master Text Styles)
/*!
 Parent elements:
    - [done] sldMaster (§19.3.1.42)

 Child elements:
    - [done] bodyStyle (Slide Master Body Text Style)   §19.3.1.5
    - extLst (Extension List)                    §19.2.1.12
    - [done] otherStyle (Slide Master Other Text Style) §19.3.1.35
    - [done] titleStyle (Slide Master Title Text Style) §19.3.1.49
*/
//! @todo support all child elements
KoFilter::ConversionStatus PptxXmlSlideReader::read_txStyles()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(bodyStyle)
            ELSE_TRY_READ_IF(titleStyle)
            ELSE_TRY_READ_IF(otherStyle)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL bodyStyle
//! bodyStyle handler (Slide Master Body Text)
/*!
 Parent elements:
    - [done] txStyles (§19.3.1.52)

 Child elements:
    - defPPr (Default Paragraph Style)  §21.1.2.2.2
    - extLst (Extension List)           §20.1.2.2.15
    - [done] lvl1pPr (List Level 1 Text Style) §21.1.2.4.13
    - [done] lvl2pPr (List Level 2 Text Style) §21.1.2.4.14
    - [done] lvl3pPr (List Level 3 Text Style) §21.1.2.4.15
    - [done] lvl4pPr (List Level 4 Text Style) §21.1.2.4.16
    - [done] lvl5pPr (List Level 5 Text Style) §21.1.2.4.17
    - [done] lvl6pPr (List Level 6 Text Style) §21.1.2.4.18
    - [done] lvl7pPr (List Level 7 Text Style) §21.1.2.4.19
    - [done] lvl8pPr (List Level 8 Text Style) §21.1.2.4.20
    - [done] lvl9pPr (List Level 9 Text Style) §21.1.2.4.21
*/
//! @todo support all child elements
KoFilter::ConversionStatus PptxXmlSlideReader::read_bodyStyle()
{
    READ_PROLOGUE

    QString currentListStyleName = "bodyList";
    m_currentListStyle = KoGenStyle(KoGenStyle::ListStyle, "list");
    d->currentSlideMasterTextStyle = &m_context->slideMasterPageProperties->bodyStyle;
    MSOOXML::Utils::Setter<PptxSlideMasterTextStyle*> currentSlideMasterTextStyleSetter(&d->currentSlideMasterTextStyle, 0);

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
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
        BREAK_IF_END_OF(CURRENT_EL);
    }

    mainStyles->insert(m_currentListStyle, currentListStyleName, KoGenStyles::DontAddNumberToName);

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL oleObj
//! oleObj handler (Global Element for Embedded objects and Controls)
/*!
 Parent elements:

 Child elements:
 - embed (Embedded Object or Control) §19.3.2.2
 - link (Linked Object or Control) §19.3.2.3
 - pic (Picture) §19.3.1.37

*/
//! @todo support all child elements
KoFilter::ConversionStatus PptxXmlSlideReader::read_oleObj()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs( attributes() );
    TRY_READ_ATTR_WITH_NS(r, id);
    TRY_READ_ATTR_WITHOUT_NS(imgW);
    TRY_READ_ATTR_WITHOUT_NS(imgH);

    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    if (!r_id.isEmpty()) {
        // The file which we get here is oble object, it should be unpacked.
        // @todo : implement this
        /*
        body->startElement("draw:image");
        const QString sourceName(m_context->relationships->target(m_context->path, m_context->file, r_id));
        if (sourceName.isEmpty()) {
            return KoFilter::FileNotFound;
        }
        QString destinationName;
        RETURN_IF_ERROR( copyFile(sourceName, QLatin1String("Pictures/"), destinationName) )
        addManifestEntryForPicturesDir();
        body->addAttribute("xlink:href", destinationName);
        body->endElement(); //draw:image
        */
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL titleStyle
//! titleStyle handler (Slide Master Title Text)
/*!
 Parent elements:
    - [done] txStyles (§19.3.1.52)

 Child elements:
    - defPPr (Default Paragraph Style)  §21.1.2.2.2
    - extLst (Extension List)           §20.1.2.2.15
    - [done] lvl1pPr (List Level 1 Text Style) §21.1.2.4.13
    - [done] lvl2pPr (List Level 2 Text Style) §21.1.2.4.14
    - [done] lvl3pPr (List Level 3 Text Style) §21.1.2.4.15
    - [done] lvl4pPr (List Level 4 Text Style) §21.1.2.4.16
    - [done] lvl5pPr (List Level 5 Text Style) §21.1.2.4.17
    - [done] lvl6pPr (List Level 6 Text Style) §21.1.2.4.18
    - [done] lvl7pPr (List Level 7 Text Style) §21.1.2.4.19
    - [done] lvl8pPr (List Level 8 Text Style) §21.1.2.4.20
    - [done] lvl9pPr (List Level 9 Text Style) §21.1.2.4.21
*/
//! @todo support all child elements
KoFilter::ConversionStatus PptxXmlSlideReader::read_titleStyle()
{
    READ_PROLOGUE

    QString currentListStyleName = "titleList";
    m_currentListStyle = KoGenStyle(KoGenStyle::ListStyle, "list");
    d->currentSlideMasterTextStyle = &m_context->slideMasterPageProperties->titleStyle;
    MSOOXML::Utils::Setter<PptxSlideMasterTextStyle*> currentSlideMasterTextStyleSetter(&d->currentSlideMasterTextStyle, 0);

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
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
        BREAK_IF_END_OF(CURRENT_EL);
    }

    mainStyles->insert(m_currentListStyle, currentListStyleName, KoGenStyles::DontAddNumberToName);

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL otherStyle
//! otherStyle handler (Slide Master Other Text)
/*!
 Parent elements:
    - [done] txStyles (§19.3.1.52)

 Child elements:
    - defPPr (Default Paragraph Style)  §21.1.2.2.2
    - extLst (Extension List)           §20.1.2.2.15
    - [done] lvl1pPr (List Level 1 Text Style) §21.1.2.4.13
    - [done] lvl2pPr (List Level 2 Text Style) §21.1.2.4.14
    - [done] lvl3pPr (List Level 3 Text Style) §21.1.2.4.15
    - [done] lvl4pPr (List Level 4 Text Style) §21.1.2.4.16
    - [done] lvl5pPr (List Level 5 Text Style) §21.1.2.4.17
    - [done] lvl6pPr (List Level 6 Text Style) §21.1.2.4.18
    - [done] lvl7pPr (List Level 7 Text Style) §21.1.2.4.19
    - [done] lvl8pPr (List Level 8 Text Style) §21.1.2.4.20
    - [done] lvl9pPr (List Level 9 Text Style) §21.1.2.4.21
*/
//! @todo support all child elements
KoFilter::ConversionStatus PptxXmlSlideReader::read_otherStyle()
{
    READ_PROLOGUE

    QString currentListStyleName = "otherList";
    m_currentListStyle = KoGenStyle(KoGenStyle::ListStyle, "list");
    d->currentSlideMasterTextStyle = &m_context->slideMasterPageProperties->otherStyle;
    MSOOXML::Utils::Setter<PptxSlideMasterTextStyle*> currentSlideMasterTextStyleSetter(&d->currentSlideMasterTextStyle, 0);

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
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
        BREAK_IF_END_OF(CURRENT_EL);
    }

    mainStyles->insert(m_currentListStyle, currentListStyleName, KoGenStyles::DontAddNumberToName);

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL cSld
//! cSld handler (Common Slide Data)
/*! ECMA-376, 19.3.1.16, p. 2833.
 This element specifies a container for slide information that is relevant to all of the slide types.

 Parent elements:
    - [done] presentation (§19.2.1.26)
    - handoutMaster (§19.3.1.24)
    - notes (§19.3.1.26)
    - notesMaster (§19.3.1.27)
    - [done] sld (§19.3.1.38)
    - sldLayout (§19.3.1.39)
    - sldMaster (§19.3.1.42)
 Child elements:
    - [done] bg (Slide Background) §19.3.1.1
    - controls (List of controls) §19.3.1.15
    - custDataLst (Customer Data List) §19.3.1.18
    - extLst (Extension List) §19.2.1.12
    - [done] spTree (Shape Tree) §19.3.1.45
*/
//! @todo support all child elements
KoFilter::ConversionStatus PptxXmlSlideReader::read_cSld()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(bg)
            ELSE_TRY_READ_IF(spTree)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL bg
// bg handler (Slide Background)
/*! ECMA-376, 19.3.1.1, p. 2815.
 This element specifies the background appearance information for a
 slide. The slide background covers the entire slide and is visible
 where no objects exist and as the background for transparent objects.

 Parent elements:
    - cSld (§19.3.1.16)
 Attributes:
    - bwMode (Black and White Mode)
 Child elements:
    - [done] bgPr (Background Properties) §19.3.1.2
    - bgRef (Background Style Reference) §19.3.1.3
*/
//! @todo support all elements
KoFilter::ConversionStatus PptxXmlSlideReader::read_bg()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(bgPr)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL bgPr
//! 19.3.1.2 bgPr (Background Properties)
/*! ECMA-376, 19.3.1.2, p. 2815.
 This element specifies visual effects used to render the slide
 background. This includes any fill, image, or effects that are to
 make up the background of the slide.

 Parent elements:
 - bg (§19.3.1.1)
 Attributes:
 - shadeToTitle
 Child elements:
 - [done] blipFill (Picture Fill) §20.1.8.14
 - effectDag (Effect Container) §20.1.8.25
 - effectLst (Effect Container) §20.1.8.26
 - extLst (Extension List) §19.2.1.12
 - gradFill (Gradient Fill) §20.1.8.33
 - grpFill (Group Fill) §20.1.8.35
 - noFill (No Fill) §20.1.8.44
 - pattFill (Pattern Fill) §20.1.8.47
 - [done] solidFill (Solid Fill) §20.1.8.54
*/
//! @todo support all elements
KoFilter::ConversionStatus PptxXmlSlideReader::read_bgPr()
{
    READ_PROLOGUE

    m_colorType = BackgroundColor;
    QString fillImageName;

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF_NS(a, solidFill)
            else if (qualifiedName() == QLatin1String("a:blipFill")) {
                TRY_READ_IF_NS_IN_CONTEXT(a, blipFill)
                KoGenStyle fillImageStyle(KoGenStyle::FillImageStyle);
                fillImageStyle.addAttribute("xlink:href", m_xlinkHref);
                //! @todo draw:name="???"
                fillImageStyle.addAttribute("xlink:type", "simple");
                fillImageStyle.addAttribute("xlink:show", "embed");
                fillImageStyle.addAttribute("xlink:actuate", "onLoad");
                fillImageName = mainStyles->insert(fillImageStyle);
                kDebug() << fillImageName;
            }
/*            else if (qualifiedName() == QLatin1String("a:tile")) {
                TRY_READ(tile)
                foundTile = true;
            }*/
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    if (!fillImageName.isEmpty()) {
        //! Setup slide's bitmap fill
        m_currentDrawStyle->addProperty("draw:fill", "bitmap");
        m_currentDrawStyle->addProperty("draw:fill-image-name", fillImageName);
        if (m_context->type != SlideMaster) {
            if (!m_recentSourceName.isEmpty()) {
                const QSize size(imageSize(m_recentSourceName));
                kDebug() << "SIZE:" << size;
                if (size.isValid()) {
                    m_currentDrawStyle->addProperty("draw:fill-image-width",
                        MSOOXML::Utils::cmString(POINT_TO_CM(size.width())));
                    m_currentDrawStyle->addProperty("draw:fill-image-height",
                        MSOOXML::Utils::cmString(POINT_TO_CM(size.height())));
                }
            }
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL spTree
//! spTree handler (Shape Tree)
/*! ECMA-376, 19.3.1.45, p. 2856
 This element specifies all shape-based objects, either grouped or not, that can be referenced on a given slide.

 Parent elements:
    - [done] cSld (§19.3.1.16)
 Child elements:
    - contentPart (Content Part) §19.3.1.14
    - cxnSp (Connection Shape) §19.3.1.19
    - extLst (Extension List with Modification Flag) §19.3.1.20
    - [done] graphicFrame (Graphic Frame) §19.3.1.21
    - grpSp (Group Shape) §19.3.1.22
    - grpSpPr (Group Shape Properties) §19.3.1.23
    - nvGrpSpPr (Non-Visual Properties for a Group Shape) §19.3.1.31
    - [done] pic (Picture) §19.3.1.37
    - [done] sp (Shape) §19.3.1.43
*/
//! @todo support all child elements
KoFilter::ConversionStatus PptxXmlSlideReader::read_spTree()
{
    READ_PROLOGUE

#ifdef HARDCODED_PRESENTATIONSTYLENAME
    // we will expect two draw:frame elements:
    // - the first will have presentation:style-name="pr1"
    // - the second presentation:style-name="pr2"
    d->presentationStyleNameCount = 0;
#endif
    d->shapeNumber = 0;

    QByteArray placeholderEl;
    QBuffer placeholderElBuffer(&placeholderEl);
    placeholderElBuffer.open(QIODevice::WriteOnly);
    delete m_placeholderElWriter;
    m_placeholderElWriter = new KoXmlWriter(&placeholderElBuffer, 0/*indentation*/);
    MSOOXML::Utils::AutoPtrSetter<KoXmlWriter> placeholderElWriterSetter(m_placeholderElWriter);
    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(sp)
            ELSE_TRY_READ_IF(pic)
            ELSE_TRY_READ_IF(graphicFrame)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    placeholderElBuffer.close();
    m_currentPresentationPageLayoutStyle.addProperty(
        QString(), QString::fromUtf8(placeholderEl), KoGenStyle::StyleChildElement);
    placeholderElWriterSetter.release();
    delete m_placeholderElWriter;
    m_placeholderElWriter = 0;

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL ph
//! ph handler (Placeholder Shape)
/*! ECMA-376, 19.3.1.36, p. 2848
 This element specifies that the corresponding shape should be represented
 by the generating application as a placeholder.

 Parent elements:
 - [done] nvPr (§19.3.1.33)
 Child elements:
 - extLst (Extension List with Modification Flag) §19.3.1.20
 Attributes:
 - hasCustomPrompt (Placeholder has custom prompt)
 - [done] idx (Placeholder Index)
 - orient (Placeholder Orientation)
 - [done] sz (Placeholder Size)
 - [done] type (Placeholder Type)
*/
//! @todo support all child elements
KoFilter::ConversionStatus PptxXmlSlideReader::read_ph()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    // Specifies the placeholder index. This is used when applying templates or changing
    // layouts to match a placeholder on one template/master to another.
    TRY_READ_ATTR_WITHOUT_NS_INTO(idx, d->phIdx)
    kDebug() << "idx:" << d->phIdx;

    // Specifies the size of a placeholder.
    // The possible values for this attribute are defined by the ST_PlaceholderSize simple type (§19.7.9), p.2987.
    TRY_READ_ATTR_WITHOUT_NS(size)
    kDebug() << "size:" << size;

    // Specifies what content type a placeholder is intended to contain.
    // The possible values for this attribute are defined by the ST_PlaceholderType simple type (§19.7.10), p.2988.
    TRY_READ_ATTR_WITHOUT_NS_INTO(type, d->phType)
    kDebug() << "type:" << d->phType;

    // Mark this shape as a place holder.
    m_isPlaceHolder = true;

    const QString styleId(d->phStyleId());
    kDebug() << "styleId:" << styleId;
    if (m_context->type == Slide) {
        if (m_context->slideLayoutProperties->styles.contains(styleId)) {
            m_currentParagraphStyle = m_context->slideLayoutProperties->styles[styleId];
            m_currentParagraphStylePredefined = true;
        }
    }
    else if (m_context->type == SlideLayout) {
    }
    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL txBody
//! txBody handler (Shape Text Body)
/*! ECMA-376, 19.3.1.51, p. 2862
 This element specifies the existence of text to be contained within the corresponding shape.
 Parent elements:
 - [done] sp (§19.3.1.43)
 Child elements:
 - [done] bodyPr (Body Properties) §21.1.2.1.1
 - [done] lstStyle (Text List Styles) §21.1.2.4.12
 - [done] p (Text Paragraphs) §21.1.2.2.6

 Only used for Slide type.
*/
//! @todo support all child elements
//! CASE #P526
KoFilter::ConversionStatus PptxXmlSlideReader::read_txBody()
{
    READ_PROLOGUE
    kDebug() << "m_context->type:" << m_context->type;

/*#ifdef __GNUC__
#warning remove m_context->type != Slide
#endif
    if (m_context->type != Slide) {
        SKIP_EVERYTHING_AND_RETURN
    }*/

    body->startElement("draw:text-box"); // CASE #P436

    m_lstStyleFound = false;
    m_prevListLevel = 0;
    m_currentListLevel = 0;
    m_pPr_lvl = 0;

    MSOOXML::Utils::XmlWriteBuffer listBuf;
    body = listBuf.setWriter(body);

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF_NS(a, bodyPr)
            ELSE_TRY_READ_IF_NS(a, lstStyle)
            else if (qualifiedName() == QLatin1String("a:p")) {
                TRY_READ(DrawingML_p);
            }
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    if (m_prevListLevel > 0) {
        for(; m_prevListLevel > 0; --m_prevListLevel) {
            body->endElement(); // text:list-item
            body->endElement(); // text:list
        }
    }

    if (m_lstStyleFound) {
        body = listBuf.originalWriter();
        body->startElement("text:list");
        const QString currentListStyleName(mainStyles->insert(m_currentListStyle));

        //! @todo currently hardcoded
        body->addAttribute("text:style-name", "bodyList");
        (void)listBuf.releaseWriter();
        body->endElement(); // text:list
    }
    else {
        body = listBuf.releaseWriter();
    }

    body->endElement(); // draw:text-box
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL graphicFrame
//! graphicFrame 
/*!
  This element specifies the existence of a graphics frame. This frame contains a graphic that was generated
  by an external source and needs a container in which to be displayed on the slide surface.

  Parent Elements:
    - grpSp (§4.4.1.19); spTree (§4.4.1.42)
  Child Elements:
    - extLst (Extension List with Modification Flag) (§4.2.4)
    - graphic (Graphic Object) (§5.1.2.1.16)
    - nvGraphicFramePr (Non-Visual Properties for a Graphic Frame) (§4.4.1.27)
    - xfrm (2D Transform for Graphic Frame)
*/
KoFilter::ConversionStatus PptxXmlSlideReader::read_graphicFrame()
{
    READ_PROLOGUE
    m_svgX = m_svgY = m_svgWidth = m_svgHeight = 0;
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            TRY_READ_IF_NS(a, graphic)
            else if (qualifiedName() == "p:xfrm") {
                read_xfrm_p();
            }
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL nvPr
//! nvPr handler (Non-Visual Properties)
/*! ECMA-376, 19.3.1.33, p. 2845
 This element specifies non-visual properties for objects.

 Parent elements:
    - nvCxnSpPr (§19.3.1.29)
    - nvGraphicFramePr (§19.3.1.30)
    - nvGrpSpPr (§19.3.1.31)
    - [done] nvPicPr (§19.3.1.32)
    - [done] nvSpPr (§19.3.1.34)
 Child elements:
    - audioCd (Audio from CD) §20.1.3.1
    - audioFile (Audio from File) §20.1.3.2
    - custDataLst (Customer Data List) §19.3.1.18
    - extLst (Extension List) §19.2.1.12
    - [done] ph (Placeholder Shape) §19.3.1.36
    - quickTimeFile (QuickTime from File) §20.1.3.4
    - videoFile (Video from File) §20.1.3.6
    - wavAudioFile (Audio from WAV File) §20.1.3.7
*/
//! @todo support all child elements
KoFilter::ConversionStatus PptxXmlSlideReader::read_nvPr()
{
    READ_PROLOGUE
    d->phType.clear();
    d->phIdx.clear();
    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(ph)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef MSOOXML_CURRENT_NS
#define MSOOXML_CURRENT_NS "p"

#undef CURRENT_EL
#define CURRENT_EL xfrm
//! p:xfrm handler that redirects to a:xfrm
KoFilter::ConversionStatus PptxXmlSlideReader::read_xfrm_p()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            if (qualifiedName() == QLatin1String("a:off")) {
                TRY_READ(off);
            } else if (qualifiedName() == QLatin1String("a:ext")) {
                TRY_READ(ext);
            }
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#define blipFill_NS "a"

// END NAMESPACE p

// BEGIN NAMESPACE a

#undef MSOOXML_CURRENT_NS
#define MSOOXML_CURRENT_NS "a"

// in PPTX we do not have pPr, so p@text:style-name should be added earlier
#define SETUP_PARA_STYLE_IN_READ_P

#include <MsooXmlCommonReaderImpl.h> // this adds a:p, a:pPr, a:t, a:r, etc.

#define DRAWINGML_NS "a"
#define DRAWINGML_PIC_NS "p" // DrawingML/Picture

#include <MsooXmlCommonReaderDrawingMLImpl.h> // this adds p:pic, etc.
