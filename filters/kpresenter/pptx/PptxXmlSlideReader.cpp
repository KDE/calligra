/*
 * This file is part of Office 2007 Filters for KOffice
 *
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

#include "PptxXmlSlideReader.h"
#include "PptxImport.h"
#include <MsooXmlSchemas.h>
#include <MsooXmlUtils.h>
#include <MsooXmlRelationships.h>

#include <KoXmlWriter.h>
#include <KoGenStyles.h>
#include <KoOdfGraphicStyles.h>

#include <kde_file.h> // for WARNING

#include <QBrush>

#define MSOOXML_CURRENT_NS "p"
#define MSOOXML_CURRENT_CLASS PptxXmlSlideReader
#define BIND_READ_CLASS MSOOXML_CURRENT_CLASS

#ifdef __GNUC__
#warning Using hardcoded presentation:style-name attributes: pr1; pr2
#else
#pragma WARNING( Using hardcoded presentation:style-name attributes: pr1; pr2 )
#endif
#define HARDCODED_PRESENTATIONSTYLENAME

#include <MsooXmlReader_p.h>

PptxShapeProperties::PptxShapeProperties()
{
}

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
}


// -------------------

PptxXmlSlideReaderContext::PptxXmlSlideReaderContext(
    PptxImport& _import, const QString& _path, const QString& _file,
    uint _slideNumber, const QMap<QString, MSOOXML::DrawingMLTheme*>& _themes,
    PptxXmlSlideReader::Type _type, PptxSlideProperties& _slideProperties,
    MSOOXML::MsooXmlRelationships& _relationships)
    : MSOOXML::MsooXmlReaderContext(&_relationships),
      import(&_import), path(_path), file(_file),
      slideNumber(_slideNumber), themes(&_themes), type(_type),
      slideProperties(&_slideProperties)
{
}

class PptxXmlSlideReader::Private {
public:
    Private()
    {
    }
    ~Private()
    {
    }
    KoXmlWriter *body; //!< Backup body pointer for SlideMaster mode
#ifdef HARDCODED_PRESENTATIONSTYLENAME
    uint presentationStyleNameCount;
#endif
    //! Used to index shapes in master slide when inheriting properties
    uint shapeNumber;
};

PptxXmlSlideReader::PptxXmlSlideReader(KoOdfWriters *writers)
    : MSOOXML::MsooXmlCommonReader(writers)
    , m_context(0)
    , m_currentDoubleValue(0)
    , m_currentShapeProperties(0)
    , d(new Private)
{
    init();
}

PptxXmlSlideReader::~PptxXmlSlideReader()
{
    delete d;
}

void PptxXmlSlideReader::init()
{
    initInternal();
    m_defaultNamespace = QLatin1String(MSOOXML_CURRENT_NS ":");
}

KoFilter::ConversionStatus PptxXmlSlideReader::read(MSOOXML::MsooXmlReaderContext* context)
{
    m_context = dynamic_cast<PptxXmlSlideReaderContext*>(context);
    Q_ASSERT(m_context);
kDebug() << "m_context->slideProperties->shapes.count()" << m_context->slideProperties->shapes.count();
    if (m_context->type == SlideMaster) { // will be written
        m_context->slideProperties->clear();
    }
    const KoFilter::ConversionStatus result = readInternal();
/*    if (m_context->type == SlideMaster) {
        if (result == KoFilter::OK) {
            m_context->slideProperties->clear();
        }
    }*/
kDebug() << "m_context->slideProperties->shapes.count()" << m_context->slideProperties->shapes.count();
    m_context = 0;
    if (result == KoFilter::OK)
        return KoFilter::OK;
    return result;
}

KoFilter::ConversionStatus PptxXmlSlideReader::readInternal()
{
    kDebug() << "=============================";
    if (m_context->type == SlideMaster) {
        //! Clear body pointer for SlideMaster mode: avoid writting to body by mistake in this mode
        d->body = body;
        body = 0;
    }

    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }

    // p:sld{Master}
    readNext();
    kDebug() << *this << namespaceUri();

    if (!expectEl(m_context->type == Slide ? "p:sld" : "p:sldMaster")) {
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

    QXmlStreamNamespaceDeclarations namespaces( namespaceDeclarations() );
    for (int i=0; i<namespaces.count(); i++) {
        kDebug() << "NS prefix:" << namespaces[i].prefix() << "uri:" << namespaces[i].namespaceUri();
    }
//! @todo find out whether the namespace returned by namespaceUri()
//!       is exactly the same ref as the element of namespaceDeclarations()
    if (!namespaces.contains( QXmlStreamNamespaceDeclaration( "p", MSOOXML::Schemas::presentationml ) )) {
        raiseError(i18n("Namespace \"%1\" not found", MSOOXML::Schemas::presentationml));
        return KoFilter::WrongFormat;
    }
//! @todo expect other namespaces too...

    if (m_context->type == Slide) {
        TRY_READ(sld)
    }
    else {
        TRY_READ(sldMaster)
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

    KoFilter::ConversionStatus result = read_sldInternal();
    if (result != KoFilter::OK)
        return result;

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL sldMaster
//! sldMster handler (Slide Master)
/*! This element specifies an instance of a slide master slide.
 ECMA-376, 19.3.1.42, p. 2853.
 This element specifies an instance of a slide master slide.
 Child elements:
    - clrMap (Color Scheme Map) §19.3.1.6
    - [done] cSld (Common Slide Data) §19.3.1.16
    - extLst (Extension List with Modification Flag) §19.3.1.20
    - hf (Header/Footer information for a slide master) §19.3.1.25
    - sldLayoutIdLst (List of Slide Layouts) §19.3.1.41
    - timing (Slide Timing Information for a Slide Layout) §19.3.1.48
    - transition (Slide Transition for a Slide Layout) §19.3.1.50
    - txStyles (Slide Master Text Styles) §19.3.1.52
*/
//! @todo support all child elements
KoFilter::ConversionStatus PptxXmlSlideReader::read_sldMaster()
{
    READ_PROLOGUE

    KoFilter::ConversionStatus result = read_sldInternal();
    if (result != KoFilter::OK)
        return result;

    READ_EPILOGUE
}

//! CASE #P300
KoFilter::ConversionStatus PptxXmlSlideReader::read_sldInternal()
{
    QXmlStreamNamespaceDeclarations namespaces = namespaceDeclarations();
    for (int i=0; i<namespaces.count(); i++) {
        kDebug() << "NS prefix:" << namespaces[i].prefix() << "uri:" << namespaces[i].namespaceUri();
    }

    if (m_context->type == Slide) {
        m_currentPageStyle = KoGenStyle(KoGenStyle::StyleDrawingPage, "drawing-page"); // CASE #P109
        m_currentPageStyle.addProperty( "presentation:background-visible", true ); // CASE #P111
        m_currentPageStyle.addProperty( "presentation:background-objects-visible", true ); // CASE #P112
    }

    {
        MSOOXML::Utils::XmlWriteBuffer drawPageBuf; // buffer this draw:page, because we have to compute
                                                    // style before style name is known
//        KoXmlWriter *origBody = body;
        if (m_context->type == Slide) {
            body = drawPageBuf.setWriter(body);
//            body = new KoXmlWriter(&drawPageBuf, origBody->indentLevel()+1);
        }
        while (!atEnd()) {
            readNext();
            kDebug() << *this;
            if (isStartElement()) {
                TRY_READ_IF(cSld)
//! @todo add ELSE_WRONG_FORMAT
            }
            if (m_context->type == Slide) {
                BREAK_IF_END_OF(sld);
            }
            else {
                BREAK_IF_END_OF(sldMaster);
            }
        }

        if (m_context->type == Slide) {
            body = drawPageBuf.originalWriter();
//            delete body;
//            body = origBody;
            {
                body->startElement("draw:page"); // CASE #P300
        //! @todo draw:master-page-name is hardcoded for now
                body->addAttribute("draw:master-page-name", "Default"); // required; CASE #P301
        //! @todo draw:name can be pulled out of docProps/app.xml (TitlesOfParts)
                body->addAttribute("draw:name", QString("page%1").arg(m_context->slideNumber)); //optional; CASE #P303
                body->addAttribute("draw:id", QString("pid%1").arg(m_context->slideNumber)); //optional; unique ID; CASE #P305, #P306
        //! @todo presentation:use-date-time-name //optional; CASE #P304
        //! @todo body->addAttribute("presentation:presentation-page-layout-name", ...); //optional; CASE #P308

                const QString currentPageStyleName( mainStyles->lookup(m_currentPageStyle) );
                body->addAttribute("draw:style-name", currentPageStyleName); // CASE #P302
                kDebug() << "currentPageStyleName:" << currentPageStyleName;

//                body->addCompleteElement(&drawPageBuf);
                (void)drawPageBuf.releaseWriter();
                body->endElement(); //draw:page
            }
        }
    }
    return KoFilter::OK;
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
    - bg (Slide Background) §19.3.1.1
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
KoFilter::ConversionStatus PptxXmlSlideReader::read_bgPr()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF_NS(a, solidFill)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
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
    - graphicFrame (Graphic Frame) §19.3.1.21
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
    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(sp)
            ELSE_TRY_READ_IF(pic)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
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

    const QXmlStreamAttributes attrs( attributes() );
    // Specifies the placeholder index. This is used when applying templates or changing
    // layouts to match a placeholder on one template/master to another.
    TRY_READ_ATTR_WITHOUT_NS(idx)
    kDebug() << "idx:" << idx;

    // Specifies the size of a placeholder.
    // The possible values for this attribute are defined by the ST_PlaceholderSize simple type (§19.7.9), p.2987.
    TRY_READ_ATTR_WITHOUT_NS(size)
    kDebug() << "size:" << size;

    // Specifies what content type a placeholder is intended to contain.
    // The possible values for this attribute are defined by the ST_PlaceholderType simple type (§19.7.10), p.2988.
    TRY_READ_ATTR_WITHOUT_NS_INTO(type, m_phType)
    kDebug() << "type:" << m_phType;

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
 - bodyPr (Body Properties) §21.1.2.1.1
 - [done] lstStyle (Text List Styles) §21.1.2.4.12
 - [done] p (Text Paragraphs) §21.1.2.2.6

 Only used for Slide type.
*/
//! @todo support all child elements
KoFilter::ConversionStatus PptxXmlSlideReader::read_txBody()
{
    READ_PROLOGUE
    body->startElement("draw:text-box"); // CASE #P436

    m_lstStyleFound = false;
    m_pPr_lvl = 0;
    const bool isOutline = MSOOXML::Utils::ST_PlaceholderType_to_ODF(m_phType.toLatin1()) == "outline";

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF_NS(a, lstStyle)
//            else TRY_READ_IF_NS(a, p)
            else if (qualifiedName() == QLatin1String("a:p")) {
                // buffer each text:p, because we have to write after the child elements aregenerated
                MSOOXML::Utils::XmlWriteBuffer paragraphBuf;
//                KoXmlWriter *origBody = body;
                if (isOutline) {
//                    body = new KoXmlWriter(&paragraphBuf, origBody->indentLevel()+1);
                    body = paragraphBuf.setWriter(body);
                }
                TRY_READ(p);
                if (isOutline) { // CASE #P612
//                    delete body;
//                    body = origBody;
                    body = paragraphBuf.originalWriter();
                    for (uint i=0; i<=m_pPr_lvl; i++) {
                        body->startElement("text:list");
                        if (i==0) {
//! @todo L2 hardcoded
                            body->addAttribute("text:style-name", "L2");
                        }
                        body->startElement("text:list-item");
                    }
//                    body->addCompleteElement(&paragraphBuf);
                    (void)paragraphBuf.releaseWriter();
                    for (uint i=0; i<=m_pPr_lvl; i++) {
                        body->endElement(); //text:list-item
                        body->endElement(); //text:list
                    }
                }
            }
//! @todo a:bodyPr
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    body->endElement(); // draw:text-box
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
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_nvPr()
{
    READ_PROLOGUE
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

#define blipFill_NS "p"

// END NAMESPACE p

// BEGIN NAMESPACE a
//! @todo move to the themes reader or so...

#undef MSOOXML_CURRENT_NS
#define MSOOXML_CURRENT_NS "a"

// in PPTX we do not have pPr, so p@text:style-name should be added earlier
#define SETUP_PARA_STYLE_IN_READ_P

#include <MsooXmlCommonReaderImpl.h> // this adds a:p, a:pPr, a:t, a:r, etc.

#undef MSOOXML_CURRENT_NS
#define MSOOXML_CURRENT_NS "p"

#include <MsooXmlCommonReaderDrawingMLImpl.h> // this adds p:pic, etc.

#undef MSOOXML_CURRENT_NS
#define MSOOXML_CURRENT_NS "a"

#undef CURRENT_EL
#define CURRENT_EL solidFill
//! CASE #P121
//! @todo support all child elements
KoFilter::ConversionStatus PptxXmlSlideReader::read_solidFill()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(schemeClr)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL schemeClr
//! @todo support all child elements
KoFilter::ConversionStatus PptxXmlSlideReader::read_schemeClr()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs( attributes() );
    READ_ATTR_WITHOUT_NS(val)

    // get color from theme
    if (m_context->themes->isEmpty())
        return KoFilter::WrongFormat;
//! @todo find proper theme, not just any
    MSOOXML::DrawingMLColorSchemeItemBase *colorItem = 0;
    if (m_context->type == Slide) {
       MSOOXML::DrawingMLTheme *theme = m_context->themes->constBegin().value();
        colorItem = theme->colorScheme.value(val);
        if (!colorItem || !colorItem->toColorItem())
            return KoFilter::WrongFormat;
    }
    MSOOXML::Utils::DoubleModifier lumMod;
    MSOOXML::Utils::DoubleModifier lumOff;
    while (true) {
        BREAK_IF_END_OF(CURRENT_EL);
        readNext();
        kDebug() << *this;
        if (m_context->type == Slide) {
            if (QUALIFIED_NAME_IS(lumMod)) {
                m_currentDoubleValue = &lumMod.value;
                TRY_READ(lumMod);
                lumMod.valid = true;
            }
            else if (QUALIFIED_NAME_IS(lumOff)) {
                m_currentDoubleValue = &lumOff.value;
                TRY_READ(lumOff);
                lumOff.valid = true;
            }
        }
    }

    if (m_context->type == Slide) {
        QColor col(colorItem->toColorItem()->color);
        col = MSOOXML::Utils::colorForLuminance(col, lumMod, lumOff);
        QBrush brush(col, Qt::SolidPattern);
        KoOdfGraphicStyles::saveOdfFillStyle(m_currentPageStyle, *mainStyles, brush);
    }

    READ_EPILOGUE
}

//! 20.1.2.3.20 lumMod (Luminance Modulation)
//! This element specifies the input color with its luminance modulated by the given percentage.
//! @todo support all child elements
#undef CURRENT_EL
#define CURRENT_EL lumMod
KoFilter::ConversionStatus PptxXmlSlideReader::read_lumMod()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs( attributes() );
    READ_ATTR_WITHOUT_NS(val)

    bool ok;
    Q_ASSERT(m_currentDoubleValue);
    *m_currentDoubleValue = MSOOXML::Utils::ST_Percentage_withMsooxmlFix_to_double(val, ok);
    if (!ok)
        return KoFilter::WrongFormat;

    readNext();
    READ_EPILOGUE
}

//! 20.1.2.3.21 lumOff (Luminance Offset)
//! This element specifies the input color with its luminance shifted, but with its hue and saturation unchanged.
//! @todo support all child elements
#undef CURRENT_EL
#define CURRENT_EL lumOff
KoFilter::ConversionStatus PptxXmlSlideReader::read_lumOff()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs( attributes() );
    READ_ATTR_WITHOUT_NS(val)

    bool ok;
    Q_ASSERT(m_currentDoubleValue);
    *m_currentDoubleValue = MSOOXML::Utils::ST_Percentage_withMsooxmlFix_to_double(val, ok);
    if (!ok)
        return KoFilter::WrongFormat;

    readNext();
    READ_EPILOGUE
}

// END NAMESPACE a
