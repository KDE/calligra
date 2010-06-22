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

#ifndef MSOOXMLCOMMONREADERDRAWINGML_IMPL_H
#define MSOOXMLCOMMONREADERDRAWINGML_IMPL_H

#include <math.h>

#ifndef M_PI
#define M_PI 3.1415926535897932384626
#endif

#if !defined DRAWINGML_NS && !defined NO_DRAWINGML_NS
#error missing DRAWINGML_NS define!
#endif
#if !defined DRAWINGML_PIC_NS && !defined NO_DRAWINGML_PIC_NS
#error missing DRAWINGML_PIC_NS define!
#endif

#undef MSOOXML_CURRENT_NS
#ifndef NO_DRAWINGML_PIC_NS
#define MSOOXML_CURRENT_NS DRAWINGML_PIC_NS
#endif

#include <KoXmlWriter.h>
#include <MsooXmlUnits.h>
#include "Charting.h"
#include "ChartExport.h"
#include "XlsxXmlChartReader.h"

// ================================================================

void MSOOXML_CURRENT_CLASS::initDrawingML()
{
    m_currentDoubleValue = 0;
    m_colorType = BackgroundColor;
    m_hyperLink = false;
    m_currentListStyleProperties = 0;
    m_listStylePropertiesAltered = false;
}

KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::copyFile(const QString& sourceName,
                                                           const QString& destinationDir,
                                                           QString& destinationName)
{
    destinationName = destinationDir + sourceName.mid(sourceName.lastIndexOf('/') + 1);
    if (m_copiedFiles.contains(sourceName)) {
        kDebug() << sourceName << "already copied - skipping";
    }
    else {
//! @todo should we check name uniqueness here in case the sourceName can be located in various directories?
        RETURN_IF_ERROR( m_context->import->copyFile(sourceName, destinationName) )
        addManifestEntryForFile(destinationName);
        m_copiedFiles.insert(sourceName);
    }
    return KoFilter::OK;
}

QSize MSOOXML_CURRENT_CLASS::imageSize(const QString& sourceName)
{
    const QMap<QString, QSize>::ConstIterator it(m_imageSizes.constFind(sourceName));
    if (it == m_imageSizes.constEnd()) {
        QSize size;
        const KoFilter::ConversionStatus status = m_context->import->imageSize(sourceName, &size);
        if (status != KoFilter::OK)
            size = QSize(-1, -1);
        m_imageSizes.insert(sourceName, size);
        return size;
    }
    return it.value();
}


// ================================================================
// DrawingML tags
// ================================================================

#undef CURRENT_EL
#define CURRENT_EL pic
//! pic handler (Picture)
/*! ECMA-376, 19.3.1.37, p. 2848; 20.1.2.2.30, p.3049 - DrawingML
 This element specifies the existence of a picture object within the document.
*/
//! @todo use it in DrawingML too: ECMA-376, 20.2.2.5, p. 3463
/*!
 Parent elements:
 - control (§19.3.2.1)
 - grpSp (§19.3.1.22)
 - grpSp (§20.1.2.2.20) - DrawingML
 - lockedCanvas (§20.3.2.1) - DrawingML
 - oleObj (§19.3.2.4)
 - [done] spTree (§19.3.1.45)

 Child elements:
 - [done] blipFill (Picture Fill) §19.3.1.4
 - [done] blipFill (Picture Fill) §20.1.8.14 - DrawingML
 - extLst (Extension List with Modification Flag) §19.3.1.20
 - extLst (Extension List) §20.1.2.2.15 - DrawingML
 - [done] nvPicPr (Non-Visual Properties for a Picture) §19.3.1.32
 - [done] nvPicPr (Non-Visual Properties for a Picture) §20.1.2.2.28 - DrawingML
 - [done] spPr (Shape Properties) §19.3.1.44
 - [done] spPr (Shape Properties) §20.1.2.2.35 - DrawingML
 - style (Shape Style) §19.3.1.46
 - style (Shape Style) §20.1.2.2.37 - DrawingML
*/
//! @todo support all elements
//! CASE #P401
//! @todo CASE #P421
//! CASE #P422
//! @todo support all child elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_pic()
{
    READ_PROLOGUE

    // Reset picture properties
    m_xlinkHref.clear();
    m_hasPosOffsetH = false;
    m_hasPosOffsetV = false;
    m_posOffsetH = 0;
    m_posOffsetV = 0;
    m_cNvPrId.clear();
    m_cNvPrName.clear();
    m_cNvPrDescr.clear();
    m_fillImageRenderingStyleStretch = false;
    m_flipH = false;
    m_flipV = false;
    m_rot = 0;
    m_isPlaceHolder = false;

    MSOOXML::Utils::XmlWriteBuffer drawFrameBuf;
    body = drawFrameBuf.setWriter(body);
//    QBuffer drawFrameBuf;
//    KoXmlWriter *origBody = body;
//    body = new KoXmlWriter(&drawFrameBuf);

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(spPr)
            ELSE_TRY_READ_IF_IN_CONTEXT(blipFill)
            ELSE_TRY_READ_IF(nvPicPr)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    body = drawFrameBuf.originalWriter();
//    delete body;
//    body = origBody;
    if (!m_isPlaceHolder) {
        body->startElement("draw:frame"); // CASE #P421
#ifdef PPTXXMLSLIDEREADER_H
        body->addAttribute("draw:layer", "layout");
        body->addAttribute("presentation:user-transformed", MsooXmlReader::constTrue);
#endif
//todo        body->addAttribute("presentation:style-name", styleName);
//! @todo for pptx: maybe use KoGenStyle::PresentationAutoStyle?
        if (m_noFill)
            m_currentDrawStyle.addAttribute("style:fill", constNone);

#ifdef DOCXXMLDOCREADER_H
        QString currentDrawStyleName(mainStyles->insert(m_currentDrawStyle, "gr"));
#endif
#ifdef HARDCODED_PRESENTATIONSTYLENAME
//! @todo hardcoded draw:style-name = grpredef1
        QString currentDrawStyleName("grpredef1");
#endif
#if defined(DOCXXMLDOCREADER_H) || defined(HARDCODED_PRESENTATIONSTYLENAME)
        kDebug() << "currentDrawStyleName:" << currentDrawStyleName;
        body->addAttribute("draw:style-name", currentDrawStyleName);
#endif

//! @todo CASE #1341: images within w:hdr should be anchored as paragraph (!wp:inline) or as-char (wp:inline)
        if (m_drawing_inline) {
            body->addAttribute("text:anchor-type", "as-char");
        }
        else {
            body->addAttribute("text:anchor-type", "char");
        }
        if (!m_docPrName.isEmpty()) { // from docPr/@name
            body->addAttribute("draw:name", m_docPrName);
        }
//! @todo add more cases for text:anchor-type! use m_drawing_inline and see CASE #1343
        int realX = m_svgX;
        int realY = m_svgY;
        if (m_hasPosOffsetH) {
            kDebug() << "m_posOffsetH" << m_posOffsetH;
            realX += m_posOffsetH;
        }
        if (m_hasPosOffsetV) {
            kDebug() << "m_posOffsetV" << m_posOffsetV;
            realY += m_posOffsetV;
        }
        body->addAttribute("svg:x", EMU_TO_CM_STRING(realX));
        body->addAttribute("svg:y", EMU_TO_CM_STRING(realY));
        body->addAttribute("svg:width", EMU_TO_CM_STRING(m_svgWidth));
        body->addAttribute("svg:height", EMU_TO_CM_STRING(m_svgHeight));
        //! @todo: flipH, flipV
        //! @todo: Generalize the generation of draw:transform so it can be used in other places.
        //         ingwa: I'm not so sure.  flipH, flipV are handled below, and generate the
        //                style:mirror attribute.

        if (m_rot != 0) {
            // m_rot is in 1/60,000th of a degree
            body->addAttribute("draw:transform", MSOOXML::Utils::rotateString(m_rot, m_svgX, m_svgY));
        }

        // Now it's time to link to the actual picture.  Only do it if
        // there is an image to link to.  If so, this was created in
        // read_blip().
        if (!m_xlinkHref.isEmpty()) {
            body->startElement("draw:image");
            body->addAttribute("xlink:href", m_xlinkHref);
            //! @todo xlink:type?
            body->addAttribute("xlink:type", "simple");
            //! @todo xlink:show?
            body->addAttribute("xlink:show", "embed");
            //! @todo xlink:actuate?
            body->addAttribute("xlink:actuate", "onLoad");
#ifdef PPTXXMLSLIDEREADER_H
            body->startElement("text:p");
            body->endElement(); //text:p
#endif
            body->endElement(); //draw:image
#ifdef DOCXXMLDOCREADER_H
            if (!m_cNvPrName.isEmpty() || !m_cNvPrDescr.isEmpty()) {
                body->startElement("svg:title");
                body->addTextSpan(m_cNvPrDescr.isEmpty() ? m_cNvPrName : m_cNvPrDescr);
                body->endElement(); //svg:title
            }
#endif
            m_xlinkHref.clear();
        }

        // Add style information
        QString  mirror;
        if (m_flipH || m_flipV) {
            if (m_flipH && m_flipV)
                mirror = "horizontal vertical";
            else if (m_flipH)
                mirror = "horizontal";
            else if (m_flipV)
                mirror = "vertical";
            else
                mirror = "none";
        }
        //! @todo: horizontal-on-{odd,even}?
        m_currentDrawStyle.addProperty("style:mirror", mirror);

        (void)drawFrameBuf.releaseWriter();
//        body->addCompleteElement(&drawFrameBuf);
        body->endElement(); //draw:frame
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL nvPicPr
//! nvPicPr handler (Non-Visual Properties for a Picture)
//! ECMA-376, 19.3.1.32, p. 2845; 20.1.2.2.28, p. 3048 - DrawingML
/*! This element specifies all non-visual properties for a picture.

 Parent elements:
    - [done] pic (§19.3.1.37)
    - [done] pic (§20.1.2.2.30) - DrawingML
 Child elements:
    - cNvPicPr (Non-Visual Picture Drawing Properties) §19.3.1.11
    - [done] cNvPicPr (Non-Visual Picture Drawing Properties) §20.1.2.2.7 - DrawingML
    - [done] cNvPr (Non-Visual Drawing Properties) §19.3.1.12
    - [done] cNvPr (Non-Visual Drawing Properties) §20.1.2.2.8 - DrawingML
    - [done] nvPr (Non-Visual Properties) §19.3.1.33
*/
//! @todo support all child elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_nvPicPr()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(cNvPicPr)
            ELSE_TRY_READ_IF_IN_CONTEXT(cNvPr)
#ifdef PPTXXMLSLIDEREADER_H
            ELSE_TRY_READ_IF(nvPr) // only §19.3.1.33
#endif
            ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL cNvPicPr
//! cNvPicPr handler (Non-Visual Picture Drawing Properties)
//! ECMA-376, 19.3.1.11, p. 2825; 20.1.2.2.7, p. 3027 - DrawingML
/*! This element specifies the non-visual properties for the picture canvas.
 These properties are to be used by the generating application to determine
 how certain properties are to be changed for the picture object in question.

 Parent elements:
    - [done] nvPicPr (§19.3.1.32)
    - [done] nvPicPr (§20.1.2.2.28) - DrawingML
 Child elements:
    - extLst (Extension List) §20.1.2.2.15
    - picLocks (Picture Locks) §20.1.2.2.31
 Attributes:
    - preferRelativeResize (Relative Resize Preferred)
*/
//! @todo support all child elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_cNvPicPr()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL cNvPr
//! cNvPr handler (Non-Visual Drawing Properties)
//! ECMA-376, 19.3.1.12, p. 2826; 20.1.2.2.8, p. 3028 - DrawingML
/*! This element specifies non-visual canvas properties.
 This allows for additional information that does not affect
 the appearance of the picture to be stored.

 Parent elements:
    - nvCxnSpPr (§19.3.1.29)
    - nvCxnSpPr (§20.1.2.2.25) - DrawingML
    - nvGraphicFramePr (§19.3.1.30)
    - nvGraphicFramePr (§20.1.2.2.26) - DrawingML
    - nvGrpSpPr (§19.3.1.31)
    - nvGrpSpPr (§20.1.2.2.27) - DrawingML
    - [done] nvPicPr (§19.3.1.32)
    - [done] nvPicPr (§20.1.2.2.28) - DrawingML
    - [done] nvSpPr (§19.3.1.34)
    - [done] nvSpPr (§20.1.2.2.29) - DrawingML
 Child elements:
    - extLst (Extension List) §20.1.2.2.15
    - hlinkClick (Click Hyperlink) §21.1.2.3.5
    - hlinkHover (Hyperlink for Hover) §20.1.2.2.23
 Attributes:
    - [done] descr (Alternative Text for Object)
    - hidden (Hidden)
    - [done] id (Unique Identifier)
    - [done] name (Name)
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_cNvPr(cNvPrCaller caller)
{
    READ_PROLOGUE

    m_cNvPrId.clear();
    m_cNvPrName.clear();
    m_cNvPrDescr.clear();
    const QXmlStreamAttributes attrs(attributes());
    if (caller == cNvPr_nvSpPr || caller == cNvPr_nvPicPr) { // for sanity, p:nvGrpSpPr can be also the caller
        READ_ATTR_WITHOUT_NS_INTO(id, m_cNvPrId)
        kDebug() << "id:" << m_cNvPrId;
        TRY_READ_ATTR_WITHOUT_NS_INTO(name, m_cNvPrName)
        kDebug() << "name:" << m_cNvPrName;
        TRY_READ_ATTR_WITHOUT_NS_INTO(descr, m_cNvPrDescr)
        kDebug() << "descr:" << m_cNvPrDescr;
    }

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
//            TRY_READ_IF()
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL nvSpPr
//! nvSpPr handler (Non-Visual Properties for a Shape)
//! ECMA-376, 19.3.1.34, p. 2846; 20.1.2.2.29, p. 3049 - DrawingML.
/*! This element specifies all non-visual properties for a shape.
 This element is a container for the non-visual identification properties,
 shape properties and application properties that are to be associated with a shape.
 This allows for additional information that does not affect the appearance of the shape to be stored.

 Parent elements:
    - [done] sp (§19.3.1.43)
    - [done] sp (§20.1.2.2.33)
 Child elements:
    - [done] cNvPr (Non-Visual Drawing Properties) §19.3.1.12
    - [done] cNvPr (Non-Visual Drawing Properties) §20.1.2.2.8 - DrawingML
    - [done] cNvSpPr (Non-Visual Drawing Properties for a Shape) §19.3.1.13
    - [done] cNvSpPr (Non-Visual Drawing Properties for a Shape) §20.1.2.2.9 - DrawingML
    - [done] nvPr (Non-Visual Properties) §19.3.1.33
*/
//! @todo support all child elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_nvSpPr()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF_IN_CONTEXT(cNvPr)
#ifdef PPTXXMLSLIDEREADER_H
            ELSE_TRY_READ_IF(nvPr) // only §19.3.1.33
#endif
            ELSE_TRY_READ_IF(cNvSpPr)
            ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL cNvSpPr
//! cNvSpPr handler (Non-Visual Drawing Properties for a Shape)
//! ECMA-376, 19.3.1.13, p. 2828; 20.1.2.2.9, p. 3030.
/*! This element specifies the non-visual drawing properties for a shape.

 Parent elements:
    - [done] nvSpPr (§19.3.1.34)
    - [done] nvSpPr (§20.1.2.2.29) - DrawingML
 Child elements:
    - extLst (Extension List) §20.1.2.2.15
    - spLocks (Shape Locks) §20.1.2.2.34
 Attributes:
    - [done] txBox (Text Box)
*/
//! @todo support all child elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_cNvSpPr()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    // Read attributes
    // FIXME: Make a member?
    //bool isTextBox = MSOOXML::Utils::convertBooleanAttr(attrs.value("txBox").toString(), false);

    // Read child elements
    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        /*        if (isStartElement()) {
                    TRY_READ_IF(...)
        //! @todo add ELSE_WRONG_FORMAT
                }*/
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL sp
//! sp handler (Shape)
//! ECMA-376, 19.3.1.43, p. 2854; 20.1.2.2.33, p. 3053 - DrawingML.
/*! This element specifies the existence of a single shape.
 A shape can either be a preset or a custom geometry,
 defined using the DrawingML framework.

 Parent elements:
    - grpSp (§19.3.1.22)
    - grpSp (§20.1.2.2.20) - DrawingML
    - lockedCanvas (§20.3.2.1) - DrawingML
    - [done] spTree (§19.3.1.45)
 Child elements:
    - extLst (Extension List with Modification Flag) §19.3.1.20
    - extLst (Extension List) §20.1.2.2.15 - DrawingML
    - [done] nvSpPr (Non-Visual Properties for a Shape) §19.3.1.34
    - [done] nvSpPr (Non-Visual Properties for a Shape) §20.1.2.2.29 - DrawingML
    - [done] spPr (Shape Properties) §19.3.1.44
    - [done] spPr (Shape Properties) §20.1.2.2.35 - DrawingML
    - style (Shape Style) §19.3.1.46
    - style (Shape Style) §20.1.2.2.37 - DrawingML
    - [done] txBody (Shape Text Body) §19.3.1.51 - PML
    - [done] txSp (Text Shape) §20.1.2.2.41 - DrawingML
 Attributes:
 - [unsupported?] useBgFill
*/
//! @todo support all elements
//! CASE #P405
//! CASE #P425
//! CASE #P430
//! CASE #P476
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_sp()
{
    READ_PROLOGUE

    // Reset the position and size
    m_svgX = 0;
    m_svgY = 0;
    m_svgWidth = -1;
    m_svgHeight = -1;

#ifdef PPTXXMLSLIDEREADER_H
    // If called from the pptx converter, handle different contexts
    // (Slide, SlideMaster, SlideLayout)
    kDebug() << "type:" << m_context->type;
    if (m_context->type == Slide) {
    }
    else if (m_context->type == SlideMaster) {
        m_currentShapeProperties = new PptxShapeProperties();
        m_context->slideProperties->shapes.append(m_currentShapeProperties);
        m_currentMasterPageStyle = KoGenStyle(KoGenStyle::MasterPageStyle);
#ifdef __GNUC__
#warning TODO:     m_currentMasterPageStyle.addChildElement(....)
#endif
    }
    else if (m_context->type == SlideLayout) {
        //m_currentShapeProperties = new PptxShapeProperties();
        //m_context->slideLayoutProperties->shapes.append(m_currentShapeProperties);
        m_currentDrawStyle = KoGenStyle(KoGenStyle::GraphicStyle);
#ifdef __GNUC__
#warning TODO:     m_currentMasterPageStyle.addChildElement(....)
#endif
    }
    m_isPlaceHolder = false;
    m_phType.clear();
#endif

    m_cNvPrId.clear();
    m_cNvPrName.clear();
    m_cNvPrDescr.clear();
    m_rot = 0;

    MSOOXML::Utils::XmlWriteBuffer drawFrameBuf; // buffer this draw:frame, because we have
    // to write after the child elements are generated
//    QBuffer drawFrameBuf;
//    KoXmlWriter *origBody = body;
#ifdef PPTXXMLSLIDEREADER_H
    bool outputDrawFrame = m_context->type == Slide || m_context->type == SlideLayout;
#else
    bool outputDrawFrame = true;
#endif
    if (outputDrawFrame) {
        body = drawFrameBuf.setWriter(body);
//        body = new KoXmlWriter(&drawFrameBuf, origBody->indentLevel()+1);
    }

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(nvSpPr)
            ELSE_TRY_READ_IF(spPr)
#ifdef PPTXXMLSLIDEREADER_H
            else if (m_context->type == Slide || m_context->type == SlideLayout) {
                TRY_READ_IF(txBody)
            }
#endif
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

#ifdef PPTXXMLSLIDEREADER_H
    if (m_context->type != Slide && !d->phType.isEmpty() && outputDrawFrame) {
        outputDrawFrame = false;
        body = drawFrameBuf.originalWriter();
        drawFrameBuf.clear();
        kDebug() << "giving up outputDrawFrame for because ph@ is not empty: ph@type=" << d->phType << "m_context->type=" << m_context->type;
    }
#endif

    if (outputDrawFrame) {
#ifdef PPTXXMLSLIDEREADER_H
        kDebug() << "outputDrawFrame for" << (m_context->type == SlideLayout ? "SlideLayout" : "Slide");
//        KoXmlWriter *writer = m_context->type == SlideLayout ? mainStyles : body;
#else
#endif
//        KoXmlWriter *writer = body;
//        delete body;
//        body = origBody;
        body = drawFrameBuf.originalWriter();
        body->startElement("draw:frame"); // CASE #P475

#ifdef PPTXXMLSLIDEREADER_H
        body->addAttribute("draw:layer", "layout");

//todo        body->addAttribute("presentation:style-name", styleName);
# ifdef HARDCODED_PRESENTATIONSTYLENAME
        d->presentationStyleNameCount++;
        body->addAttribute("presentation:style-name",
                           d->presentationStyleNameCount == 1 ? "pr1" : "pr2");
# else
        //body->addAttribute("draw:style-name", );
# endif // HARDCODED_PRESENTATIONSTYLENAME

        if (m_context->type == Slide) {
// CASE #P476
            body->addAttribute("draw:id", m_cNvPrId);
            const QString presentationClass(MSOOXML::Utils::ST_PlaceholderType_to_ODF(d->phType));
            body->addAttribute("presentation:class", presentationClass);
            kDebug() << "presentationClass:" << d->phType << "->" << presentationClass;
            PptxPlaceholder *placeholder;
            if (m_svgWidth > -1 && m_svgHeight > -1) {
                body->addAttribute("presentation:user-transformed", MsooXmlReader::constTrue);
    //! @todo if there's no data in spPr tag, use the one from the slide layout, then from the master slide
                body->addAttribute("svg:x", EMU_TO_CM_STRING(m_svgX));
                body->addAttribute("svg:y", EMU_TO_CM_STRING(m_svgY));
                body->addAttribute("svg:width", EMU_TO_CM_STRING(m_svgWidth));
                body->addAttribute("svg:height", EMU_TO_CM_STRING(m_svgHeight));
            }
            else if (   m_context->slideLayoutProperties
                && ((placeholder = m_context->slideLayoutProperties->placeholders.value(presentationClass))))
            {
                kDebug() << "Copying attributes from slide layout:" << m_context->slideLayoutProperties->styleName;
                placeholder->writeAttributes(body);
            }
            if (m_rot != 0) {
                // m_rot is in 1/60,000th of a degree
                body->addAttribute("draw:transform", MSOOXML::Utils::rotateString(m_rot, m_svgX, m_svgY));
            }
        }
        // FIXME: Adapt this to apply to the current graphics style and use that.
        //if (m_currentColor.isValid()) {
        //  m_currentDrawStyle.addProperty("fo:background-color", m_currentColor.name());
        //  m_currentColor = QColor();
        //}

#else
#ifdef __GNUC__
#warning TODO: docx
#endif
#endif // PPTXXMLSLIDEREADER_H

        (void)drawFrameBuf.releaseWriter();
//        body->addCompleteElement(&drawFrameBuf);
        body->endElement(); //draw:frame
    }
#ifdef PPTXXMLSLIDEREADER_H
    else if (m_context->type == SlideLayout) {
        // presentation:placeholder
        Q_ASSERT(m_placeholderElWriter);
        const QString presentationObject(MSOOXML::Utils::ST_PlaceholderType_to_ODF(d->phType));

        // Keep this placeholder information for reuse in slides because ODF requires
        // not only reference but redundant copy of the properties to be present in slides.
        PptxPlaceholder *placeholder = new PptxPlaceholder;
        placeholder->x = EMU_TO_CM_STRING(m_svgX);
        placeholder->y = EMU_TO_CM_STRING(m_svgY);
        placeholder->width = EMU_TO_CM_STRING(m_svgWidth);
        placeholder->height = EMU_TO_CM_STRING(m_svgHeight);
        m_context->slideLayoutProperties->placeholders.insert(presentationObject, placeholder);

        m_placeholderElWriter->startElement("presentation:placeholder");
        m_placeholderElWriter->addAttribute("presentation:object", presentationObject);
        placeholder->writeAttributes(m_placeholderElWriter);
        m_placeholderElWriter->endElement();
    }
    d->shapeNumber++;
#endif

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL spPr
//! spPr handler (Shape Properties)
/*! ECMA-376, 19.3.1.44, p. 2855; 20.1.2.2.35, p. 3055 (DrawingML)
 This element specifies the visual shape properties that can be applied to a shape.
 These properties include the shape fill, outline, geometry, effects, and 3D orientation.

 Parent elements:
    - cxnSp (§19.3.1.19)
    - cxnSp (§20.1.2.2.10) - DrawingML
    - lnDef (§20.1.4.1.20) - DrawingML
    - [done] pic (§19.3.1.37)
    - [done] pic (§20.1.2.2.30) - DrawingML
    - [done] sp (§19.3.1.43)
    - [done] sp (§20.1.2.2.33) - DrawingML
    - spDef (§20.1.4.1.27) - DrawingML
    - txDef (§20.1.4.1.28) - DrawingML

 Child elements:
    - blipFill (Picture Fill) §20.1.8.14
    - custGeom (Custom Geometry) §20.1.9.8
    - effectDag (Effect Container) §20.1.8.25
    - effectLst (Effect Container) §20.1.8.26
    - extLst (Extension List) §20.1.2.2.15
    - gradFill (Gradient Fill) §20.1.8.33
    - grpFill (Group Fill) §20.1.8.35
    - ln (Outline) §20.1.2.2.24
    - [done] noFill (No Fill) §20.1.8.44
    - pattFill (Pattern Fill) §20.1.8.47
    - prstGeom (Preset geometry) §20.1.9.18
    - scene3d (3D Scene Properties) §20.1.4.1.26
    - [done] solidFill (Solid Fill) §20.1.8.54
    - sp3d (Apply 3D shape properties) §20.1.5.12
    - [done] xfrm (2D Transform for Individual Objects) §20.1.7.6
 Attributes:
    - bwMode (Black and White Mode)
*/
//! @todo support all child elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_spPr()
{
    READ_PROLOGUE
    bool xfrm_read = false;
    bool solidFill_read = false;
    m_noFill = false;
    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            if (qualifiedName() == QLatin1String("a:xfrm")) {
                TRY_READ(xfrm)
                xfrm_read = true;
            }
            else if (qualifiedName() == QLatin1String("a:solidFill")) {
                TRY_READ(solidFill)
                solidFill_read = true;
            }
            else if ( qualifiedName() == QLatin1String("a:ln") ) {
                TRY_READ(ln)
            }
            else if (qualifiedName() == QLatin1String("a:noFill")) {
                SKIP_EVERYTHING // safely skip
                m_noFill = true;
            }
//! @todo a:prstGeom...
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

#ifdef PPTXXMLSLIDEREADER_H
//! @todo
    if (m_context->type == Slide && !xfrm_read) { // loading values from master is needed
        /*if (m_context->slideLayoutProperties) {
            kDebug() << "m_context->slideLayoutProperties->shapes.count()" << m_context->slideLayoutProperties->shapes.count()
                     << "d->shapeNumber" << d->shapeNumber;
            if (m_context->slideProperties->shapes.count() > (int)d->shapeNumber) {
                // for inheritance
                m_currentShapeProperties = m_context->slideProperties->shapes.at(d->shapeNumber);
                kDebug() << QString("Shape #%1 found in master slide").arg(d->shapeNumber);

                m_svgX = m_currentShapeProperties->x;
                m_svgY = m_currentShapeProperties->y;
                kDebug() << "Inherited svg:x/y from master (m_currentShapeProperties)";
                m_svgWidth = m_currentShapeProperties->width;
                m_svgHeight = m_currentShapeProperties->height;
                kDebug() << "Inherited svg:width/height from master (m_currentShapeProperties)";
            }
        }
        else*/
        if (m_context->slideProperties) {
            kDebug() << "m_context->slideProperties->shapes.count()" << m_context->slideProperties->shapes.count()
                     << "d->shapeNumber" << d->shapeNumber;
            if (m_context->slideProperties->shapes.count() > (int)d->shapeNumber) {
                // for inheritance
                m_currentShapeProperties = m_context->slideProperties->shapes.at(d->shapeNumber);
                kDebug() << QString("Shape #%1 found in master slide").arg(d->shapeNumber);

                m_svgX = m_currentShapeProperties->x;
                m_svgY = m_currentShapeProperties->y;
                kDebug() << "Inherited svg:x/y from master (m_currentShapeProperties)";
                m_svgWidth = m_currentShapeProperties->width;
                m_svgHeight = m_currentShapeProperties->height;
                kDebug() << "Inherited svg:width/height from master (m_currentShapeProperties)";
            } else {
                m_currentShapeProperties = 0;
                kWarning() << QString("No shape #%1 found in master slide; shapes count = %2").arg(d->shapeNumber)
                .arg(m_context->slideProperties->shapes.count());
            }
        }
    }
#endif

    if (solidFill_read) {
        
    }

    READ_EPILOGUE
}


// ================================================================
//                             NameSpace "c"
// ================================================================

#undef MSOOXML_CURRENT_NS
#define MSOOXML_CURRENT_NS "c"

#undef CURRENT_EL
#define CURRENT_EL chart
//! chart handler (Charting diagrams)
/*!
@todo documentation
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_chart()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITH_NS(r, id)
    if (!r_id.isEmpty()) {
        const QString filepath = m_context->relationships->target(m_context->path, m_context->file, r_id);

        Charting::Chart* chart = new Charting::Chart;
        ChartExport* chartexport = new ChartExport(chart);
        chartexport->m_drawLayer = true;
        chartexport->m_x = EMU_TO_POINT(qMax(0, m_svgX));
        chartexport->m_y = EMU_TO_POINT(qMax(0, m_svgY));
        chartexport->m_width = m_svgWidth > 0 ? EMU_TO_POINT(m_svgWidth) : 100;
        chartexport->m_height = m_svgHeight > 0 ? EMU_TO_POINT(m_svgHeight) : 100;

        kDebug()<<"r:id="<<r_id<<"filepath="<<filepath<<"position="<<QString("%1:%2").arg(chartexport->m_x).arg(chartexport->m_y)<<"size="<<QString("%1x%2").arg(chartexport->m_width).arg(chartexport->m_height);
        
        KoStore* storeout = m_context->import->outputStore();
        XlsxXmlChartReaderContext context(storeout, chartexport);
        XlsxXmlChartReader reader(this);
        const KoFilter::ConversionStatus result = m_context->import->loadAndParseDocument(&reader, filepath, &context);
        if (result != KoFilter::OK) {
            raiseError(reader.errorString());
            delete chart;
            delete chartexport;
            return result;
        }

        chartexport->saveIndex(body);
        delete chart;
        delete chartexport;
    }

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
    }

    READ_EPILOGUE
}

// ================================================================
//                             NameSpace "a"
// ================================================================


#undef MSOOXML_CURRENT_NS
#define MSOOXML_CURRENT_NS "a"

#undef CURRENT_EL
#define CURRENT_EL p
//! p handler (Text Paragraphs) ECMA-376, DrawingML 21.1.2.2.6, p. 3587.
//!   This element specifies the presence of a paragraph of text within the containing text body.
/*!
 Parent elements:
 - rich (§21.2.2.156)
 - t (§21.4.3.8)
 - txBody (§21.3.2.26)
 - txBody (§20.1.2.2.40)
 - txBody (§20.5.2.34)
 - [done] txBody (§19.3.1.51) - PML
 - txPr (§21.2.2.216)

 Child elements:
 - br (Text Line Break) §21.1.2.2.1
 - endParaRPr (End Paragraph Run Properties) §21.1.2.2.3
 - [done] fld (Text Field) §21.1.2.2.4
 - pPr (Text Paragraph Properties) §21.1.2.2.7
 - [done] r (Text Run) §21.1.2.3.8
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_DrawingML_p()
{
    READ_PROLOGUE2(DrawingML_p)

    const read_p_args args = m_read_DrawingML_p_args;
    m_read_DrawingML_p_args = 0;
    m_paragraphStyleNameWritten = false;
    m_currentListLevel = 0;

    MSOOXML::Utils::XmlWriteBuffer textPBuf;

    if (args & read_p_Skip) {
        kDebug() << "SKIP!";
    } else {
        body = textPBuf.setWriter(body);
        m_currentParagraphStyle = KoGenStyle(KoGenStyle::ParagraphAutoStyle, "paragraph");
    }

    m_currentListStyle = KoGenStyle(KoGenStyle::ListAutoStyle, "list");

    while (!atEnd()) {
        readNext();
        kDebug() << "isStartElement:" << isStartElement();
        if (isStartElement()) {
            if (QUALIFIED_NAME_IS(p)) {
// CASE #301: avoid nested paragaraphs
                kDebug() << "Nested" << qualifiedName() << "detected: skipping the inner element";
                TRY_READ_WITH_ARGS(DrawingML_p, read_p_Skip;)
            }
//            ELSE_TRY_READ_IF(hyperlink)
            //ELSE_TRY_READ_IF(commentRangeEnd)
// CASE #400.1
            else if (QUALIFIED_NAME_IS(pPr)) {
                TRY_READ(DrawingML_pPr)
            }
// CASE #400.2
//! @todo add more conditions testing the parent
            else if (QUALIFIED_NAME_IS(r)) {
                TRY_READ(DrawingML_r)
            }
            ELSE_TRY_READ_IF(fld)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    if (args & read_p_Skip) {
        //nothing
     } else {
         body = textPBuf.originalWriter();
         if (m_lstStyleFound || m_currentListLevel > 0) {
             int listDepth = 0;
             while (listDepth <= m_currentListLevel) {
                 body->startElement("text:list");
                 if (listDepth == 0) {
                     if (m_currentListStyle.isEmpty()) {
#ifdef PPTXXMLSLIDEREADER_H
                         // for now the name is hardcoded...should be maybe fixed
                         if (d->phType == "title" || d->phType == "ctrTitle") {
                             body->addAttribute("text:style-name", "titleList"); 
                         }
                         else if (d->phType == "body" ) {
                             // Fixme? : for now the master slide list style is called bodyList
                             body->addAttribute("text:style-name", "bodyList");
                         }
                         else {
                             // This hardcoded name should maybe changed to something else
                             body->addAttribute("text:style-name", "otherList");
                         }
#else
                         //! @todo ok?
                         body->addAttribute("text:style-name", "otherList");
#endif
                     }
                     else {
                         QString listStyleName = mainStyles->insert(m_currentListStyle);
                         body->addAttribute("text:style-name", listStyleName);
                     }
                 }
                 body->startElement("text:list-item");
                 ++listDepth;                 
             }
         }
         body->startElement("text:p", false);
         setupParagraphStyle();
         (void)textPBuf.releaseWriter();
         if (m_lstStyleFound || m_currentListLevel > 0) {
             int listDepth = 0;
             while (listDepth <= m_currentListLevel) {
                 body->endElement(); // text:list-item
                 body->endElement(); // text:list
                 ++listDepth;
             }
         }
         body->endElement(); //text:p
         kDebug() << "/text:p";
     }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL r
//! r handler (Text Run)
/*! ECMA-376, 21.1.2.3.8, p.3623.

 Parent elements:
 - [done] p (§21.1.2.2.6)

 Child elements:
 - [done] rPr (Text Run Properties) §21.1.2.3.9
 - [done] t (Text String) §21.1.2.3.11
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_DrawingML_r()
{
    READ_PROLOGUE2(DrawingML_r)
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            if (QUALIFIED_NAME_IS(rPr)) {
                TRY_READ(DrawingML_rPr)
            }
            ELSE_TRY_READ_IF(t)
            ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL rPr
//! rPr handler (Text Run Properties) DrawingML ECMA-376, 21.1.2.3.9, p.3624.
//! This element contains all run level text properties for the text runs within a containing paragraph.
/*!
 Parent elements:
 - br (§21.1.2.2.1)
 - fld (§21.1.2.2.4)
 - [done] r (§21.1.2.3.8)
 Attributes:
 - altLang (Alternative Language)
 - b (Bold)
 - baseline (Baseline)
 - bmk (Bookmark Link Target)
 - cap (Capitalization)
 - dirty (Dirty)
 - err (Spelling Error)
 - i (Italics)
 - kern (Kerning)
 - kumimoji (Kumimoji)
 - lang (Language ID)
 - noProof (No Proofing)
 - normalizeH (Normalize Heights)
 - smtClean (SmartTag Clean)
 - smtId (SmartTag ID)
 - spc (Spacing)
 - strike (Strikethrough)
 - sz (Font Size)
 - u (Underline)
 Child elements:
 - [done] blipFill (Picture Fill) §20.1.8.14
 - cs (Complex Script Font) §21.1.2.3.1
 - ea (East Asian Font) §21.1.2.3.3
 - effectDag (Effect Container) §20.1.8.25
 - effectLst (Effect Container) §20.1.8.26
 - extLst (Extension List) §20.1.2.2.15
 - gradFill (Gradient Fill) §20.1.8.33
 - grpFill (Group Fill) §20.1.8.35
 - [done] highlight (Highlight Color) §21.1.2.3.4
 - [done] hlinkClick (Click Hyperlink) §21.1.2.3.5
 - hlinkMouseOver (Mouse-Over Hyperlink) §21.1.2.3.6
 - [done] latin (Latin Font) §21.1.2.3.7
 - [done] ln (Outline) §20.1.2.2.24
 - [done] noFill (No Fill) §20.1.8.44
 - pattFill (Pattern Fill) §20.1.8.47
 - rtl (Right to Left Run) §21.1.2.2.8
 - [done] solidFill (Solid Fill) §20.1.8.54
 - sym (Symbol Font) §21.1.2.3.10
 - uFill (Underline Fill) §21.1.2.3.12
 - uFillTx (Underline Fill Properties Follow Text) §21.1.2.3.13
 - uLn (Underline Stroke) §21.1.2.3.14
 - uLnTx (Underline Follows Text) §21.1.2.3.15
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_DrawingML_rPr()
{    
    READ_PROLOGUE2(DrawingML_rPr)
    m_colorType = TextColor;

    m_hyperLink = false;

    const QXmlStreamAttributes attrs(attributes());

    Q_ASSERT(m_currentTextStyleProperties == 0);
//    delete m_currentTextStyleProperties;
    m_currentTextStyleProperties = new KoCharacterStyle();

    if (!m_currentTextStylePredefined) {
        m_currentTextStyle = KoGenStyle(KoGenStyle::TextAutoStyle, "text");
    }

    // Read child elements
    while (!atEnd()) {
        BREAK_IF_END_OF(CURRENT_EL);
        readNext();
        if (isStartElement()) {
            TRY_READ_IF(latin)
            ELSE_TRY_READ_IF_IN_CONTEXT(blipFill)
            ELSE_TRY_READ_IF(solidFill)
            ELSE_TRY_READ_IF_IN_CONTEXT(noFill) 
            else if (QUALIFIED_NAME_IS(highlight)) {
                TRY_READ(DrawingML_highlight)
            }
            ELSE_TRY_READ_IF(ln)
            ELSE_TRY_READ_IF(hlinkClick)
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    if (m_currentColor.isValid()) {
        m_currentTextStyleProperties->setForeground(m_currentColor);
        m_currentColor = QColor();
    }

    // Read Attributes

    // DrawingML: b, i, strike, u attributes:
    if (attrs.hasAttribute("b")) {
        m_currentTextStyleProperties->setFontWeight(
            MSOOXML::Utils::convertBooleanAttr(attrs.value("b").toString()) ? QFont::Bold : QFont::Normal);
    }
    if (attrs.hasAttribute("i")) {
        m_currentTextStyleProperties->setFontItalic(
            MSOOXML::Utils::convertBooleanAttr(attrs.value("i").toString()));
//kDebug() << "ITALIC:" << m_currentTextStyleProperties->fontItalic();
        }

    if (attrs.hasAttribute("cap")) {
        const QString capValue = attrs.value("cap").toString();
        if (capValue == "small") {
            m_currentTextStyleProperties->setFontCapitalization(QFont::SmallCaps);
        }
        else if (capValue == "all") {
            m_currentTextStyleProperties->setFontCapitalization(QFont::AllUppercase);
        }
    }

    bool ok = false;
    TRY_READ_ATTR_WITHOUT_NS(spc)
    const int pointSize = spc.toInt(&ok)/100;
    if (ok) {
        m_currentTextStyleProperties->setFontLetterSpacing(pointSize);
    }

    if (attrs.hasAttribute("sz")) {
        bool ok = false;
        const qreal pointSize = qreal(attrs.value("sz").toString().toUInt(&ok)) / 100.0;
        if (ok) {
            m_currentTextStyleProperties->setFontPointSize(pointSize);
        }
    }
    // from 20.1.10.79 ST_TextStrikeType (Text Strike Type)
    TRY_READ_ATTR_WITHOUT_NS(strike)
    if (strike == QLatin1String("sngStrike")) {
        m_currentTextStyleProperties->setStrikeOutType(KoCharacterStyle::SingleLine);
        m_currentTextStyleProperties->setStrikeOutStyle(KoCharacterStyle::SolidLine);
    } else if (strike == QLatin1String("dblStrike")) {
        m_currentTextStyleProperties->setStrikeOutType(KoCharacterStyle::DoubleLine);
        m_currentTextStyleProperties->setStrikeOutStyle(KoCharacterStyle::SolidLine);
    } else {
        // empty or "noStrike"
    }
    // from
    TRY_READ_ATTR_WITHOUT_NS(baseline)
    if (!baseline.isEmpty()) {
        int baselineInt;
        STRING_TO_INT(baseline, baselineInt, "rPr@baseline")
        if (baselineInt > 0)
            m_currentTextStyleProperties->setVerticalAlignment( QTextCharFormat::AlignSuperScript );
        else if (baselineInt < 0)
            m_currentTextStyleProperties->setVerticalAlignment( QTextCharFormat::AlignSubScript );
    }

    TRY_READ_ATTR_WITHOUT_NS(u)
    if (!u.isEmpty()) {
        MSOOXML::Utils::setupUnderLineStyle(u, m_currentTextStyleProperties);
    }
    // elements
    m_currentTextStyleProperties->saveOdf(m_currentTextStyle);
    delete m_currentTextStyleProperties;
    m_currentTextStyleProperties = 0;

    READ_EPILOGUE_WITHOUT_RETURN
    // read 't' in one go and insert the contents into text:span
    readNext();
    // Only create text:span if the next el. is 't'. Do not this the next el. is 'drawing', etc.
    if (QUALIFIED_NAME_IS(t)) {
        const QString currentTextStyleName(mainStyles->insert(m_currentTextStyle));
        if (m_hyperLink) {
            body->startElement("text:a");
            body->addAttribute("xlink:type", "simple");
            body->addAttribute("xlink:href", QUrl(m_hyperLinkTarget).toEncoded());
        }
        body->startElement("text:span", false);
        body->addAttribute("text:style-name", currentTextStyleName);
        TRY_READ(t)
        body->endElement(); //text:span
        if (m_hyperLink) {
            body->endElement(); // text:a
        }   
    }
    else {
        undoReadNext();
    }
//kDebug() << "/text:span";

#ifdef __GNUC__
#warning implement read_DrawingML_rPr
#endif

//    READ_EPILOGUE
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL hlinkClick
//! hlinkClick handler
/*!
 Parent elements:
 - cNvPr (§21.3.2.7)
 - cNvPr (§20.1.2.2.8)
 - cNvPr (§20.2.2.3) 
 - cNvPr (§20.5.2.8) 
 - cNvPr (§19.3.1.12)
 - defRPr (§21.1.2.3.2)
 - docPr (§20.4.2.5) 
 - endParaRPr (§21.1.2.2.3)
 - [done] rPr (§21.1.2.3.9)

 Child elements:
 - extLst (§20.1.2.2.15)
 - snd (§20.1.2.2.32)

TODO....
 Attributes..
 Children..
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_hlinkClick()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITH_NS(r, id)

    if (r_id.isEmpty()) {
        m_hyperLinkTarget.clear();
    }
    else {
        m_hyperLink = true;
        m_hyperLinkTarget = m_context->relationships->linkTarget(r_id, m_context->path, m_context->file);
    }

    while (!atEnd()) {
        BREAK_IF_END_OF(CURRENT_EL);
        readNext();
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL pPr
//! pPr handler (Text Paragraph Properties) 21.1.2.2.7, p.3588.
/*!
 Parent elements:
  - fld (§21.1.2.2.4)
  - p (§21.1.2.2.6)
 Attributes:
  - [incomplete] algn (Alignment)
  - defTabSz (Default Tab Size)
  - eaLnBrk (East Asian Line Break)
  - fontAlgn (Font Alignment)
  - hangingPunct (Hanging Punctuation)
  - indent (Indent)
  - latinLnBrk (Latin Line Break)
  - [done] lvl (Level)
  - marL (Left Margin)
  - marR (Right Margin)
  - rtl (Right To Left)
 Child elements:
  - buAutoNum (Auto-Numbered Bullet) §21.1.2.4.1
  - buBlip (Picture Bullet) §21.1.2.4.2
  - [done] buChar (Character Bullet) §21.1.2.4.3
  - buClr (Color Specified) §21.1.2.4.4
  - buClrTx (Follow Text) §21.1.2.4.5
  - buFont (Specified) §21.1.2.4.6
  - buFontTx (Follow text) §21.1.2.4.7
  - [done] buNone (No Bullet) §21.1.2.4.8
  - buSzPct (Bullet Size Percentage) §21.1.2.4.9
  - buSzPts (Bullet Size Points) §21.1.2.4.10
  - buSzTx (Bullet Size Follows Text) §21.1.2.4.11
  - defRPr (Default Text Run Properties) §21.1.2.3.2
  - extLst (Extension List) §20.1.2.2.15
  - [done] lnSpc (Line Spacing) §21.1.2.2.5
  - spcAft (Space After) §21.1.2.2.9
  - [done] spcBef (Space Before) §21.1.2.2.10
  - tabLst (Tab List) §21.1.2.2.14
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_DrawingML_pPr()
{
    READ_PROLOGUE2(DrawingML_pPr)
    const QXmlStreamAttributes attrs(attributes());

    m_currentListStyleProperties = new KoListLevelProperties;
    m_listStylePropertiesAltered = false;

    TRY_READ_ATTR_WITHOUT_NS(algn)
    algnToODF("fo:text-align", algn);

    TRY_READ_ATTR_WITHOUT_NS(lvl)

    if (!lvl.isEmpty()) {
        m_currentListStyleProperties->setLevel(lvl.toInt());
        m_currentListLevel = lvl.toInt();
    }

    while (!atEnd()) {
        if (isStartElement()) {
            TRY_READ_IF(buAutoNum)
            ELSE_TRY_READ_IF(buNone)
            ELSE_TRY_READ_IF(buChar)
            ELSE_TRY_READ_IF(spcBef)
            ELSE_TRY_READ_IF(lnSpc)
        }
        BREAK_IF_END_OF(CURRENT_EL);
        readNext();
    }

    if (m_listStylePropertiesAltered) {
        QBuffer listBuf;
        KoXmlWriter listStyleWriter(&listBuf);
    
        m_currentListStyleProperties->saveOdf(&listStyleWriter);
        const QString elementContents = QString::fromUtf8(listBuf.buffer(),
                                                          listBuf.buffer().size());
        m_currentListStyle.addChildElement("list-style-properties", elementContents);

        delete m_currentListStyleProperties;
        m_currentListStyleProperties = 0;
    }

#ifdef __GNUC__
#warning implement read_DrawingML_pPr
#endif

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL xfrm
//! xfrm handler (2D Transform for Individual Objects)
//! DrawingML ECMA-376, 20.1.7.6, p. 3187.
/*! This element represents 2-D transforms for ordinary shapes.

 Parent elements:
    - graphicFrame (§20.1.2.2.18)
    - spPr (§21.2.2.197)
    - spPr (§21.3.2.23)
    - spPr (§21.4.3.7)
    - [done] spPr (§20.1.2.2.35) - DrawingML
    - spPr (§20.2.2.6)
    - spPr (§20.5.2.30)
    - [done] spPr (§19.3.1.44)
    - txSp (§20.1.2.2.41)
 Child elements:
    - [done] ext (Extents) §20.1.7.3
    - [done] off (Offset) §20.1.7.4
 Attributes:
    - flipH (Horizontal Flip)
    - flipV (Vertical Flip)
    - rot (Rotation)
*/
//! @todo support all child elements
//! CASE #P476
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_xfrm()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    // Read attributes.
    m_flipH = MSOOXML::Utils::convertBooleanAttr(attrs.value("flipH").toString(), false);
    m_flipV = MSOOXML::Utils::convertBooleanAttr(attrs.value("flipV").toString(), false);
    m_rot = 0;
    TRY_READ_ATTR_WITHOUT_NS(rot)
    STRING_TO_INT(rot, m_rot, "xfrm@rot")

    bool off_read = false;
    bool ext_read = false;
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            if (QUALIFIED_NAME_IS(off)) {
                TRY_READ(off);
                off_read = true;
            } else if (QUALIFIED_NAME_IS(ext)) {
                TRY_READ(ext);
                ext_read = true;
            }
        }
//! @todo add ELSE_WRONG_FORMAT
        BREAK_IF_END_OF(CURRENT_EL);
    }

    /*//! @todo
        if (m_context->type == Slide) { // load values from master is needed
            if (!off_read) {
                m_svgX = m_currentShapeProperties->x;
                m_svgY = m_currentShapeProperties->y;
                kDebug() << "Inherited svg:x/y from master (m_currentShapeProperties)";
            }
            if (!ext_read) {
                m_svgWidth = m_currentShapeProperties->width;
                m_svgHeight = m_currentShapeProperties->y;
                kDebug() << "Inherited svg:width/height from master (m_currentShapeProperties)";
            }
        }*/
#ifdef PPTXXMLSLIDEREADER_H
    if (m_context->type == SlideMaster) { // save
        if (!off_read) {
            raiseElNotFoundError("a:off");
            return KoFilter::WrongFormat;
        }
        if (!ext_read) {
            raiseElNotFoundError("a:ext");
            return KoFilter::WrongFormat;
        }
        if (m_currentShapeProperties) {
            m_currentShapeProperties->x = m_svgX;
            m_currentShapeProperties->y = m_svgY;
            m_currentShapeProperties->width = m_svgWidth;
            m_currentShapeProperties->height = m_svgHeight;
            m_currentShapeProperties->rot = m_rot;
            m_currentShapeProperties->isPlaceHolder = m_isPlaceHolder;
            kDebug() << "Saved to m_currentShapeProperties";
        }
    }
#endif

    kDebug()
    << "svg:x" << m_svgX
    << "svg:y" << m_svgY
    << "svg:width" << m_svgWidth
    << "svg:height" << m_svgHeight
    << "rotation" << m_rot;

    READ_EPILOGUE
}

#undef MSOOXML_CURRENT_NS
#define MSOOXML_CURRENT_NS "a"

//! off handler (Offset)
//! DrawingML ECMA-376, 20.1.7.4, p. 3185.
/*! This element specifies the location of the bounding box of an object.
    Effects on an object are not included in this bounding box.

 Parent elements:
    - xfrm (§21.3.2.28)
    - xfrm (§20.1.7.5)
    - [done] xfrm (§20.1.7.6)
    - xfrm (§20.5.2.36)
    - xfrm (§19.3.1.53)

 No child elements.

 Attributes:
    - [done] x (X-Axis Coordinate)
    - [done] y (Y-Axis Coordinate)
*/
//! @todo support all elements
#undef CURRENT_EL
#define CURRENT_EL off
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_off()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    READ_ATTR_WITHOUT_NS(x)
    STRING_TO_INT(x, m_svgX, "off@x")
    READ_ATTR_WITHOUT_NS(y)
    STRING_TO_INT(y, m_svgY, "off@y")

    while (true) {
        BREAK_IF_END_OF(CURRENT_EL);
        readNext();
    }

    READ_EPILOGUE
}

//! ext handler (Extents)
//! DrawingML ECMA-376, 20.1.7.3, p. 3185.
/*! This element specifies the size of the bounding box enclosing the referenced object.
 Parent elements:
 - xfrm (§21.3.2.28)
 - xfrm (§20.1.7.5)
 - [done] xfrm (§20.1.7.6)
 - xfrm (§20.5.2.36)
 - xfrm (§19.3.1.53)

 No child elements.

 Attributes:
 - cx (Extent Length) Specifies the length of the extents rectangle in EMUs. This rectangle shall dictate
      the size of the object as displayed (the result of any scaling to the original object).
 - cy (Extent Width) Specifies the width of the extents rectangle in EMUs.
*/
//! @todo support all child elements
#undef CURRENT_EL
#define CURRENT_EL ext
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_ext()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    READ_ATTR_WITHOUT_NS(cx)
    STRING_TO_INT(cx, m_svgWidth, "ext@cx")
    READ_ATTR_WITHOUT_NS(cy)
    STRING_TO_INT(cy, m_svgHeight, "ext@cy")

    while (true) {
        BREAK_IF_END_OF(CURRENT_EL);
        readNext();
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL blip
//! blip handler (Blip)
//! ECMA-376, 20.1.8.13, p. 3194
/*! This element specifies the existence of an image (binary large image or picture)
    and contains a reference to the image data.

 Parent elements:
    - blipFill (§21.3.2.2) - DrawingML, p. 3919
    - [done] blipFill (§20.1.8.14) - DrawingML, p. 3195
    - blipFill (§20.2.2.1) - DrawingML, p. 3456
    - blipFill (§20.5.2.2) - DrawingML, p. 3518
    - [done] blipFill (§19.3.1.4) - PresentationML, p. 2818
    - buBlip (§21.1.2.4.2)

 Child elements:
    - alphaBiLevel (Alpha Bi-Level Effect) §20.1.8.1
    - alphaCeiling (Alpha Ceiling Effect) §20.1.8.2
    - alphaFloor (Alpha Floor Effect) §20.1.8.3
    - alphaInv (Alpha Inverse Effect) §20.1.8.4
    - alphaMod (Alpha Modulate Effect) §20.1.8.5
    - alphaModFix (Alpha Modulate Fixed Effect) §20.1.8.6
    - alphaRepl (Alpha Replace Effect) §20.1.8.8
    - [done] biLevel (Bi-Level (Black/White) Effect) §20.1.8.11
    - blur (Blur Effect) §20.1.8.15
    - clrChange (Color Change Effect) §20.1.8.16
    - clrRepl (Solid Color Replacement) §20.1.8.18
    - duotone (Duotone Effect) §20.1.8.23
    - extLst (Extension List) §20.1.2.2.15
    - fillOverlay (Fill Overlay Effect) §20.1.8.29
    - [done] grayscl (Gray Scale Effect) §20.1.8.34
    - hsl (Hue Saturation Luminance Effect) §20.1.8.39
    - [done] lum (Luminance Effect) §20.1.8.42
    - tint (Tint Effect) §20.1.8.60

 Attributes:
    - cstate (Compression State)
    - [done] embed (Embedded Picture Reference), 22.8.2.1 ST_RelationshipId (Explicit Relationship ID), p. 4324
    - link (Linked Picture Reference)
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_blip()
{
    READ_PROLOGUE

    m_xlinkHref.clear();

    // Read attributes.
    const QXmlStreamAttributes attrs(attributes());
//! @todo more attrs
    TRY_READ_ATTR_WITH_NS(r, embed)
    kDebug() << "embed:" << r_embed;
    if (!r_embed.isEmpty()) {
        const QString sourceName(m_context->relationships->target(m_context->path,
                                                                  m_context->file, r_embed));
        kDebug() << "sourceName:" << sourceName;
        if (sourceName.isEmpty()) {
            return KoFilter::FileNotFound;
        }

        QString destinationName;
        RETURN_IF_ERROR( copyFile(sourceName, QLatin1String("Pictures/"), destinationName) )

        m_recentSourceName = sourceName;
        addManifestEntryForPicturesDir();
        m_xlinkHref = destinationName;
    }

    // Read child elements
    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(biLevel)
            ELSE_TRY_READ_IF(grayscl)
            ELSE_TRY_READ_IF(lum)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL stretch
//! stretch handler (Stretch)
//! ECMA-376, 20.1.8.56, p. 3233
/*! This element specifies that a BLIP should be stretched
 to fill the target rectangle. The other option is a tile where
 a BLIP is tiled to fill the available area.

 Parent elements:
    - blipFill (§21.3.2.2) - DrawingML, p. 3919
    - [done] blipFill (§20.1.8.14) - DrawingML, p. 3195
    - blipFill (§20.2.2.1) - DrawingML, p. 3456
    - blipFill (§20.5.2.2) - DrawingML, p. 3518
    - [done] blipFill (§19.3.1.4) - PresentationML, p. 2818

 Child elements:
    - [done] fillRect (Fill Rectangle) §20.1.8.30
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_stretch()
{
    READ_PROLOGUE

    m_fillImageRenderingStyleStretch = true;
    m_currentDrawStyle.addProperty("style:repeat", QLatin1String("stretch"));

#ifdef PPTXXMLSLIDEREADER_H
    if (m_context->type == SlideMaster) {
        // pass the properties from master to slides
        m_context->slideMasterPageProperties->addDrawingPageProperty("style:repeat", "stretch");
        m_context->slideMasterPageProperties->addDrawingPageProperty("draw:fill", "bitmap");
        m_context->slideMasterPageProperties->addDrawingPageProperty("draw:fill-image-name", "A1");
        m_context->slideMasterPageProperties->addDrawingPageProperty("presentation:visibility", "visible");
    }
#endif

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(fillRect)
            ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL biLevel
//! biLevel handler (BiLevel (Black/White) Effect)
/*! ECMA-376, 20.1.8.13, p. 3193

  This element specifies a bi-level (black/white) effect. Input colors
  whose luminance is less than the specified threshold value are
  changed to black. Input colors whose luminance are greater than or
  equal the specified value are set to white. The alpha effect values
  are unaffected by this effect.

 Parent elements:
 - [done] blip (§20.1.8.13)
 - cont (§20.1.8.20)
 - effectDag (§20.1.8.25)

 No child elements.

 Attributes
 - thresh (Threshold)
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_biLevel()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    m_currentDrawStyle.addProperty("draw:color-mode", QLatin1String("mono"));
//! @todo thresh attribute (no real counterpoint in ODF)

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL grayscl
//! grayscl handler (Grayscale effect)
/*! ECMA-376, 20.1.8.34, p 3217

  This element specifies a gray scale effect. Converts all effect
  color values to a shade of gray, corresponding to their
  luminance. Effect alpha (opacity) values are unaffected.

 Parent elements:
 - [done] blip (§20.1.8.13)
 - cont (§20.1.8.20)
 - effectDag (§20.1.8.25)

 No child elements.

 No attributes
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_grayscl()
{
    READ_PROLOGUE

    m_currentDrawStyle.addProperty("draw:color-mode", QLatin1String("greyscale"));

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL lum
//! lum handler (luminance effect)
/*! ECMA-376, 20.1.8.34, p 3222

  This element specifies a luminance effect. Brightness linearly
  shifts all colors closer to white or black.  Contrast scales all
  colors to be either closer or further apart

 Parent elements:
 - [done] blip (§20.1.8.13)
 - cont (§20.1.8.20)
 - effectDag (§20.1.8.25)

 No child elements.

 Attributes
 - bright (Brightness)
 - contrast (Contrast)
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_lum()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
//! @todo attributes -- even though there is no counterpart in ODF.
    m_currentDrawStyle.addProperty("draw:color-mode", QLatin1String("watermark"));

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL tile
//! tile handler (Placeholder Shape)
/*! ECMA-376, 19.3.1.36, p. 3234
 This element specifies that a BLIP should be tiled to fill the available space. This element defines a "tile"
 rectangle within the bounding box. The image is encompassed within the tile rectangle, and the tile rectangle
 is tiled across the bounding box to fill the entire area.

 Parent elements:
 - blipFill (§21.3.2.2)
 - blipFill (§20.1.8.14)
 - blipFill (§20.2.2.1)
 - blipFill (§20.5.2.2)
 - blipFill (§19.3.1.4)

 No child elements.
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_tile()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    m_currentDrawStyle.addProperty("style:repeat", QLatin1String("repeat"));
//! @todo algn - convert to "ODF's Fill Image Tile Reference Point"
    m_currentDrawStyle.addProperty("draw:fill-image-ref-point", "top-left");
//! @todo flip
//! @todo sx
//! @todo sy
//! @todo tx
//! @todo ty

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL fillRect
//! fillRect handler (Fill Rectangle)
//! ECMA-376, 20.1.8.30, p. 3212
/*! This element specifies a fill rectangle. When stretching of an image
    is specified, a source rectangle, srcRect, is scaled to fit the specified fill rectangle.

 Parent elements:
    - [done] stretch (§20.1.8.56)

 No child elements.

 Attributes:
    - b (Bottom Offset)
    - l (Left Offset)
    - r (Right Offset)
    - t (Top Offset)

 Complex type: CT_RelativeRect, p. 4545
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_fillRect()
{
    READ_PROLOGUE

//    const QXmlStreamAttributes attrs( attributes() );
//! @todo use ST_Percentage_withMsooxmlFix_to_double for attributes b, l, r, t
    /*    TRY_READ_ATTR_WITHOUT_NS(r, b)
        TRY_READ_ATTR_WITHOUT_NS(r, l)
        TRY_READ_ATTR_WITHOUT_NS(r, r)
        TRY_READ_ATTR_WITHOUT_NS(r, t)*/
//MSOOXML_EXPORT qreal ST_Percentage_withMsooxmlFix_to_double(const QString& val, bool& ok);

    //m_fillImageRenderingStyle = QLatin1String("stretch");
    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL graphic
//! graphic handler (Graphic Object)
/*! ECMA-376, 20.1.2.2.16, p.3037.

 This element specifies the existence of a single graphic object.
 Document authors should refer to this element when they wish to persist
 a graphical object of some kind. The specification for this graphical
 object is provided entirely by the document author and referenced within
 the graphicData child element.

 Parent elements:
 - [done] anchor (§20.4.2.3)
 - graphicFrame (§21.3.2.12)
 - graphicFrame (§20.1.2.2.18)
 - graphicFrame (§20.5.2.16)
 - graphicFrame (§19.3.1.21)
 - [done] inline (§20.4.2.8)

 Child elements:
 - [done] graphicData (Graphic Object Data) §20.1.2.2.17
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_graphic()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            TRY_READ_IF(graphicData)
            ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL graphicData
//! graphicData handler (Graphic Object Data)
/*! ECMA-376, 20.1.2.2.17, p.3038.

 This element specifies the reference to a graphic object within the document.
 This graphic object is provided entirely by the document authors who choose
 to persist this data within the document.

 Parent elements:
 - [done] graphic (§20.1.2.2.16)

 Child elements:
 - Any element in any namespace

 Attributes:
 - uri (Uniform Resource Identifier)
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_graphicData()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            TRY_READ_IF_NS(pic, pic)
            ELSE_TRY_READ_IF_NS(c, chart)
#ifdef PPTXXMLSLIDEREADER_CPP
            ELSE_TRY_READ_IF_NS(p, oleObj)
#endif
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#define MSOOXML_CURRENT_NS "a" // note: osed only for blipFill namespace is parametrized, can be a or p
#undef CURRENT_EL
#define CURRENT_EL blipFill
//! blipFill handler (Picture Fill)
//! ECMA-376, PresentationML, 19.3.1.4, p. 2818; DrawingML, 20.1.8.14, p. 3195
//! @todo use it in DrawingML, 20.2.2.1, p. 3456
/*! This element specifies the type of picture fill that the picture object has.
 Because a picture has a picture fill already by default, it is possible to have
 two fills specified for a picture object.

 BLIPs refer to Binary Large Image or Pictures. Blip Fills are made up of several components: a Blip
 Reference, a Source Rectangle, and a Fill Mode.
 See also M.4.8.4.3 Blip Fills, ECMA-376, p. 5411.

 Parent elements:
    - bg (§21.4.3.1)
    - bgFillStyleLst (§20.1.4.1.7)
    - [done] bgPr (§19.3.1.2)
    - defRPr (§21.1.2.3.2)
    - endParaRPr (§21.1.2.2.3)
    - fill (§20.1.8.28)
    - fill (§20.1.4.2.9)
    - fillOverlay (§20.1.8.29)
    - fillStyleLst (§20.1.4.1.13)
    - grpSpPr (§21.3.2.14)
    - grpSpPr (§20.1.2.2.22)
    - grpSpPr (§20.5.2.18)
    - grpSpPr (§19.3.1.23)
    - [done] pic (§20.1.2.2.30) - DrawingML
    - [done] pic (§19.3.1.37) - PresentationML
    - [done] rPr (§21.1.2.3.9)
    - spPr (§21.2.2.197)
    - spPr (§21.3.2.23)
    - spPr (§21.4.3.7)
    - spPr (§20.1.2.2.35)
    - spPr (§20.2.2.6)
    - spPr (§20.5.2.30)
    - spPr (§19.3.1.44)
    - tblPr (§21.1.3.15)
    - tcPr (§21.1.3.17)
    - uFill (§21.1.2.3.12)

 Child elements:
    - [done] blip (Blip) §20.1.8.13
    - srcRect (Source Rectangle) §20.1.8.55
    - [done] stretch (Stretch) §20.1.8.56
    - [done] tile (Tile) §20.1.8.58

 Attributes:
    - dpi (DPI Setting)
    - rotWithShape (Rotate With Shape)
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_blipFill(blipFillCaller caller)
{
    kDebug() << "Caller:" << (char)caller;
    // we do not use READ_PROLOGUE because namespace depends on caller here
    PUSH_NAME_INTERNAL
    QString ns;
    // 'p' by default; for dml in docx use 'pic'
#ifdef DOCXXMLDOCREADER_CPP
    if (caller == blipFill_pic) {
        ns = QLatin1String("pic");
    }
    else {
        ns = QChar((char)caller);
    }
#else
    ns = QChar((char)caller);
#endif
    const QString qn(ns + ":" STRINGIFY(CURRENT_EL));
    if (!expectEl(qn)) {
        return KoFilter::WrongFormat;
    }

    m_fillImageRenderingStyleStretch = false;

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(blip)
            ELSE_TRY_READ_IF(stretch)
            ELSE_TRY_READ_IF(tile)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF_QSTRING(qn)
    }
    // we do not use READ_EPILOGUE because namespace depends on caller here
    POP_NAME_INTERNAL
    if (!expectElEnd(qn)) {
        kDebug() << "READ_EPILOGUE:" << qn << "not found!";
        return KoFilter::WrongFormat;
    }
    return KoFilter::OK;
}

//! @todo KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_srcRect()
/*
 No child elements.

 Attributes:
    - b (Bottom Offset)
    - l (Left Offset)
    - r (Right Offset)
    - t (Top Offset)

 Complex type: CT_RelativeRect, p. 4545

 const QXmlStreamAttributes attrs( attributes() );
 use qreal ST_Percentage_withMsooxmlFix_to_double(const QString& val, bool& ok)....
*/


#if 0 //todo
#undef CURRENT_EL
#define CURRENT_EL background
//! background handler (Document Background)
/*! ECMA-376, 17.2.1, p. 199.
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_background()
{
}
#endif

// ---- namespace like "a" (pptx) or "wp" (docx)

#undef MSOOXML_CURRENT_NS
#ifndef NO_DRAWINGML_NS
#define MSOOXML_CURRENT_NS DRAWINGML_NS
#endif

void MSOOXML_CURRENT_CLASS::saveStyleWrap(const char * style)
{
    m_currentDrawStyle.addProperty("style:wrap", style, KoGenStyle::GraphicType);
}

void MSOOXML_CURRENT_CLASS::algnToODF(const char * odfEl, const QString& ov)
{
    if (ov.isEmpty())
        return;

    QString v;
    if (ov == QLatin1String("l"))
        v = QLatin1String("start");
    else if (ov == QLatin1String("r"))
        v = QLatin1String("end");
    else if (ov == QLatin1String("just"))
        v = QLatin1String("justify");
    else if (ov == QLatin1String("ctr"))
        v = QLatin1String("center");
    //@todo handle thaiDist, justLow, dist
    if (!v.isEmpty())
        m_currentParagraphStyle.addProperty(odfEl, v);
}

void MSOOXML_CURRENT_CLASS::distToODF(const char * odfEl, const QString& emuValue)
{
    if (emuValue.isEmpty() || emuValue == "0") // skip 0cm which is the default
        return;
    QString s = MSOOXML::Utils::EMU_to_ODF(emuValue);
    if (!s.isEmpty()) {
        m_currentDrawStyle.addProperty(QLatin1String(odfEl), s, KoGenStyle::GraphicType);
    }
}

//! @todo Currently all read_wrap*() uses the same read_wrap(), no idea if they can behave differently
//! CASE #1425
void MSOOXML_CURRENT_CLASS::readWrap()
{
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(wrapText)
    if (wrapText == "bothSides")
        saveStyleWrap("parallel");
    else if (wrapText == "largest")
        saveStyleWrap("dynamic");
    else
        saveStyleWrap(wrapText.toLatin1());
//! @todo Is saveStyleWrap(wrapText) OK?
}

#undef CURRENT_EL
#define CURRENT_EL lstStyle
//! lstStyle handler (Text List Styles) ECMA-376, DrawingML 21.1.2.4.12, p. 3651.
//!          This element specifies the list of styles associated with this body of text.
/*!
 Parent elements:
 - lnDef (§20.1.4.1.20)
 - rich (§21.2.2.156)
 - spDef (§20.1.4.1.27)
 - t (§21.4.3.8)
 - txBody (§21.3.2.26)
 - txBody (§20.1.2.2.40)
 - txBody (§20.5.2.34)
 - [done] txBody (§19.3.1.51)
 - txDef (§20.1.4.1.28)
 - txPr (§21.2.2.216)

 Child elements:
 - defPPr (Default Paragraph Style) §21.1.2.2.2
 - extLst (Extension List) §20.1.2.2.15
 - lvl1pPr (List Level 1 Text Style) §21.1.2.4.13
 - lvl2pPr (List Level 2 Text Style) §21.1.2.4.14
 - lvl3pPr (List Level 3 Text Style) §21.1.2.4.15
 - lvl4pPr (List Level 4 Text Style) §21.1.2.4.16
 - lvl5pPr (List Level 5 Text Style) §21.1.2.4.17
 - lvl6pPr (List Level 6 Text Style) §21.1.2.4.18
 - lvl7pPr (List Level 7 Text Style) §21.1.2.4.19
 - lvl8pPr (List Level 8 Text Style) §21.1.2.4.20
 - lvl9pPr (List Level 9 Text Style) §21.1.2.4.21
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_lstStyle()
{
    READ_PROLOGUE
    m_currentListStyle = KoGenStyle(KoGenStyle::ListAutoStyle, "list");

    while (true) {
        BREAK_IF_END_OF(CURRENT_EL);
        //! @todo read children
        readNext();
    }
    m_lstStyleFound = !m_currentListStyle.isEmpty();

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL latin
/*! latin handler (Latin Font) ECMA-376, 21.1.2.3.7, p.3621.
 Parent elements:
 - defRPr (§21.1.2.3)
 - endParaRPr (§21.1.2.2.3)
 - font (§20.1.4.2.13)
 - majorFont (§20.1.4.1.24)
 - minorFont (§20.1.4.1.25)
 - [done] rPr (§21.1.2.3.9)
 No child elements.

 Attributes:
 - charset (Similar Character Set)
 - panose (Panose Setting)
 - [incomplete] pitchFamily (Similar Font Family)
 - [done] typeface (Text Typeface)
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_latin()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR_WITHOUT_NS(typeface)
    if (!typeface.isEmpty())
        m_currentTextStyleProperties->setFontFamily(typeface);
    TRY_READ_ATTR_WITHOUT_NS(pitchFamily)
    if (!pitchFamily.isEmpty()) {
        int pitchFamilyInt;
        STRING_TO_INT(pitchFamily, pitchFamilyInt, "latin@pitchFamily")
        QFont::StyleHint h = QFont::AnyStyle;
        const int hv = pitchFamilyInt % 0x10;
        switch (hv) {
        case 1: //Roman
            h = QFont::Times;
            break;
        case 2: //Swiss
            h = QFont::SansSerif;
            break;
        case 3: //Modern
            h = QFont::SansSerif;
            //TODO
            break;
        case 4: //Script
            //TODO
            break;
        case 5: //Decorative
            h = QFont::Decorative;
            break;
        }
        const bool fixed = pitchFamilyInt & 0x01; // Fixed Pitch
        m_currentTextStyleProperties->setFontFixedPitch(fixed);
        m_currentTextStyleProperties->setFontStyleHint(h);
    }
    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL highlight
/*! highlight handler  (Highlight Color) ECMA-376, 21.1.2.3.4, p.3616.

 This element specifies the highlight color that is present for a run of text.

 Parent elements:
 - defRPr (§21.1.2.3.2)
 - endParaRPr (§21.1.2.2.3)
 - [done] rPr (§21.1.2.3.9)

 Child elements:
 - hslClr (Hue, Saturation, Luminance Color Model) §20.1.2.3.13
 - prstClr (Preset Color) §20.1.2.3.22
 - [done] schemeClr (Scheme Color) §20.1.2.3.29
 - [done] scrgbClr (RGB Color Model - Percentage Variant) §20.1.2.3.30
 - [done] srgbClr (RGB Color Model - Hex Variant) §20.1.2.3.32
 - sysClr (System Color) §20.1.2.3.33
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_DrawingML_highlight()
{
    READ_PROLOGUE2(DrawingML_highlight)

    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            TRY_READ_IF(schemeClr)
            ELSE_TRY_READ_IF(scrgbClr)
            ELSE_TRY_READ_IF(srgbClr)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
//    m_currentTextStyleProperties->setBackground(m_currentColor);
    // note: paragraph background is unsupported in presentation applications anyway...
    if (m_currentColor.isValid()) {
        m_currentParagraphStyle.addProperty("fo:background-color", m_currentColor.name());
        m_currentColor = QColor();
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL solidFill
//!Solid Fill
//! DrawingML ECMA-376 20.1.8.54, p. 3234.
/*!
This element especifies a solid color fill.
 Parents:
    - bg (§21.4.3.1)
    - bgFillStyleLst (§20.1.4.1.7)
    - bgPr (§19.3.1.2)
    - defRPr (§21.1.2.3.2)
    - endParaRPr (§21.1.2.2.3)
    - fill (§20.1.8.28)
    - fill (§20.1.4.2.9)
    - fillOverlay (§20.1.8.29)
    - fillStyleLst (§20.1.4.1.13)
    - grpSpPr (§21.3.2.14)
    - grpSpPr (§20.1.2.2.22)
    - grpSpPr (§20.5.2.18)
    - grpSpPr (§19.3.1.23)
    - ln (§20.1.2.2.24)
    - lnB (§21.1.3.5)
    - lnBlToTr (§21.1.3.6)
    - lnL (§21.1.3.7)
    - lnR (§21.1.3.8)
    - lnT (§21.1.3.9)
    - lnTlToBr (§21.1.3.10)
    - [done] rPr (§21.1.2.3.9)
    - spPr (§21.2.2.197)
    - spPr (§21.3.2.23)
    - spPr (§21.4.3.7)
    - [done] spPr (§20.1.2.2.35)
    - spPr (§20.2.2.6)
    - spPr (§20.5.2.30)
    - [done] spPr (§19.3.1.44)
    - tblPr (§21.1.3.15)
    - tcPr (§21.1.3.17)
    - uFill (§21.1.2.3.12)
    - uLn (§21.1.2.3.14)

 Child elements:
    - hslClr (Hue, Saturation, Luminance Color Model) §20.1.2.3.13
    - prstClr (Preset Color) §20.1.2.3.22
    - [done] schemeClr (Scheme Color) §20.1.2.3.29
    - [done] scrgbClr (RGB Color Model - Percentage Variant) §20.1.2.3.30
    - [done] srgbClr (RGB Color Model - Hex Variant) §20.1.2.3.32
    - sysClr (System Color) §20.1.2.3.33

 Attributes:
    None.
*/
//! CASE #P121
//! @todo support all child elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_solidFill()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            //scheme color
            TRY_READ_IF(schemeClr)
//             rgb percentage
            ELSE_TRY_READ_IF(scrgbClr)
            //TODO hslClr hue, saturation, luminecence color
            //TODO prstClr preset color
            ELSE_TRY_READ_IF(srgbClr)
            //TODO stsClr system color
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

//noFill 20.1.8.44
/*This element specifies No fill.
Parents:
    - bg (§21.4.3.1)
    - bgFillStyleLst (§20.1.4.1.7)
    - bgPr (§19.3.1.2)
    - defRPr (§21.1.2.3.2)
    - endParaRPr (§21.1.2.2.3)
    - fill (§20.1.8.28)
    - fill (§20.1.4.2.9)
    - fillOverlay (§20.1.8.29)
    - fillStyleLst (§20.1.4.1.13)
    - grpSpPr (§21.3.2.14)
    - grpSpPr (§20.1.2.2.22)
    - grpSpPr (§20.5.2.18)
    - grpSpPr (§19.3.1.23)
    - ln (§20.1.2.2.24)
    - lnB (§21.1.3.5)
    - lnBlToTr(§21.1.3.6)
    - lnL (§21.1.3.7)
    - lnR (§21.1.3.8)
    - lnT (§21.1.3.9)
    - lnTlToBr (§21.1.3.10)
    - [done] rPr (§21.1.2.3(§21.2.2.197)
    - spPr (§21.3.2.23)
    - spPr (§21.4.3.7)
    - spPr (§20.1.2.2.35)
    - spPr (§20.2.2.6)
    - spPr (§20(§19.3.1.44) 
    - tblPr (§21.1.3.15)
    - tcPr (§21.1.3.17)
    - uFill (§21.1.2.3.12)
    - uLn (§21.1.2.3.14)
*/
#undef CURRENT_EL
#define CURRENT_EL noFill
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_noFill(noFillCaller caller)
{
    READ_PROLOGUE
    if (caller == noFill_rPr) {
       m_currentTextStyleProperties->setTextOutline(QPen(Qt::SolidLine));
    }
    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL schemeClr
//! @todo support all child elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_schemeClr()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    READ_ATTR_WITHOUT_NS(val)

    // get color from theme
    if (m_context->themes->isEmpty())
        return KoFilter::WrongFormat;
//! @todo find proper theme, not just any
#ifdef PPTXXMLSLIDEREADER_H
    MSOOXML::DrawingMLColorSchemeItemBase *colorItem = 0;
    if (m_context->type == Slide) {
        MSOOXML::DrawingMLTheme *theme = m_context->themes->constBegin().value();
        colorItem = theme->colorScheme.value(val);
    }
#endif
    MSOOXML::Utils::DoubleModifier lumMod;
    MSOOXML::Utils::DoubleModifier lumOff;
    while (true) {
        BREAK_IF_END_OF(CURRENT_EL);
        readNext();
#ifdef PPTXXMLSLIDEREADER_H
        if (m_context->type == Slide) {
            if (QUALIFIED_NAME_IS(lumMod)) {
                m_currentDoubleValue = &lumMod.value;
                TRY_READ(lumMod);
                lumMod.valid = true;
            } else if (QUALIFIED_NAME_IS(lumOff)) {
                m_currentDoubleValue = &lumOff.value;
                TRY_READ(lumOff);
                lumOff.valid = true;
            }
        }
#endif
    }

#ifdef PPTXXMLSLIDEREADER_H
    if (m_context->type == Slide) {
        //Seems that if the item is not present we should default to black
        QColor col(Qt::black);
        if (colorItem && colorItem->toColorItem())
            col = QColor (colorItem->toColorItem()->color);

        col = MSOOXML::Utils::colorForLuminance(col, lumMod, lumOff);
        switch (m_colorType) {
            case BackgroundColor:
            {
                QBrush brush(col, Qt::SolidPattern);
                KoOdfGraphicStyles::saveOdfFillStyle(m_currentDrawStyle, *mainStyles, brush);
            }
            break;
            case OutlineColor:
            {
                m_currentPen.setColor(col);
                KoOdfGraphicStyles::saveOdfStrokeStyle(m_currentDrawStyle, *mainStyles, m_currentPen);
            }
            break;
            case TextColor:
            {
                if (m_currentTextStyleProperties) {
                    m_currentTextStyleProperties->setForeground(col);
                }
            }
            break;
        }
    }
#endif
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL lumMod
//! 20.1.2.3.20 lumMod (Luminance Modulation)
//! This element specifies the input color with its luminance modulated by the given percentage.
//! @todo support all child elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_lumMod()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    READ_ATTR_WITHOUT_NS(val)

    bool ok;
    Q_ASSERT(m_currentDoubleValue);
    *m_currentDoubleValue = MSOOXML::Utils::ST_Percentage_withMsooxmlFix_to_double(val, ok);
    if (!ok)
        return KoFilter::WrongFormat;

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL lumOff
//! 20.1.2.3.21 lumOff (Luminance Offset)
//! This element specifies the input color with its luminance shifted, but with its hue and saturation unchanged.
//! @todo support all child elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_lumOff()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    READ_ATTR_WITHOUT_NS(val)

    bool ok;
    Q_ASSERT(m_currentDoubleValue);
    *m_currentDoubleValue = MSOOXML::Utils::ST_Percentage_withMsooxmlFix_to_double(val, ok);
    if (!ok)
        return KoFilter::WrongFormat;

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL ln
//! Outline
//! DrawingML ECMA-376, 20.1.2.2.24, p. 3048.
/*! This element specifies an outline style that can be applied to a 
    number of different objects such as shapes and text. 

 Child elements:
    - bevel (Line Join Bevel) §20.1.8.9
    - custDash (Custom Dash) §20.1.8.21
    - extLst (Extension List) §20.1.2.2.15
    - gradFill (Gradient Fill) §20.1.8.33
    - headEnd (Line Head/End Style) §20.1.8.38
    - miter (Miter Line Join) §20.1.8.43
    - noFill (No Fill) §20.1.8.44
    - pattFill (Pattern Fill) §20.1.8.47
    - prstDash (Preset Dash) §20.1.8.48
    - round (Round Line Join) §20.1.8.52
    - solidFill (Solid Fill) §20.1.8.54
    - tailEnd (Tail line end style) §20.1.8.57

 Attributes:
    - algn
    - cap
    - cmpd
    - w
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_ln()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    m_colorType = OutlineColor;
    m_currentPen = QPen();

    //align
    TRY_READ_ATTR_WITHOUT_NS(algn)
    //center
    if (algn.isEmpty() || algn == "ctr") {
    }
    //inset
    else if(algn == "in") {
    }

    //line ending cap
    TRY_READ_ATTR_WITHOUT_NS(cap)
    Qt::PenCapStyle penCap = m_currentPen.capStyle();
    //flat
    if (cap.isEmpty() || cap == "sq") {
       penCap = Qt::SquareCap;
    }
    //round
    else if (cap == "rnd") {
        penCap = Qt::RoundCap;
    }
    //square
    else if (cap == "flat") {
        penCap = Qt::FlatCap;
    }
    m_currentPen.setCapStyle(penCap);

    //TODO
    //compound line type
    TRY_READ_ATTR_WITHOUT_NS(cmpd)
    //double lines
    if( cmpd.isEmpty() || cmpd == "sng" ) {
    }
    //single line
    else if (cmpd == "dbl") {
    }
    //thick thin double lines
    else if (cmpd == "thickThin") {
    }
    //thin thick double lines
    else if (cmpd == "thinThick") {
    }
    //thin thick thin triple lines
    else if (cmpd == "tri") {
    }

    TRY_READ_ATTR_WITHOUT_NS(w) //width
    if(w.isEmpty()) {
        w = "0";
    }
    int wInt = w.toInt();
    m_currentPen.setWidth(wInt);

    while (!atEnd()) {
        BREAK_IF_END_OF(CURRENT_EL);
        readNext();
        if( isStartElement() ) {
            //Line join bevel
//             if(qualifiedName() == QLatin1String("a:bevel")) {
//                 TRY_READ()
//             }
//             //custom dash
//             else if(qualifiedName() == QLatin1String("a:custDash")) {
//             }
//             //extension list
//             else if(qualifiedName() == QLatin1String("a:extLst")) {
//             }
//             //gradient fill
//             else if(qualifiedName() == QLatin1String("a:gradFill")) {
//             }
//             //line head/end style
//             else if(qualifiedName() == QLatin1String("a:headEnd")) {
//             }
//             //miter line join
//             else if(qualifiedName() == QLatin1String("a:miter")) {
//             }
//             //no fill
//             else if(qualifiedName() == QLatin1String("a:noFill")) {
//             }
//             //pattern fill
//             else if(qualifiedName() == QLatin1String("a:pattFill")) {
//             }
//             //preset dash
//             else if(qualifiedName() == QLatin1String("a:prstDash")) {
//             }
//             //round line join
//             else if(qualifiedName() == QLatin1String("a:round")) {
//             }
            //solid fill
            TRY_READ_IF(solidFill)
            //tail line end style
//             else if(qualifiedName() == QLatin1String("a:tailEnd")) {
//             }
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL scrgbClr
//! RGB Color Model - Percentage Variant
//! DrawingML ECMA-376 20.1.2.3.30, p. 3074.
/*!
This element specifies a solid color fill.

 Child elements:
    - alpha (Alpha) §20.1.2.3.1
    - alphaMod (Alpha Modulation) §20.1.2.3.2
    - alphaOff (Alpha Offset) §20.1.2.3.3
    - blue (Blue) §20.1.2.3.4
    - blueMod (Blue Modification) §20.1.2.3.5
    - blueOff (Blue Offset) §20.1.2.3.6
    - comp (Complement) §20.1.2.3.7
    - gamma (Gamma) §20.1.2.3.8
    - gray (Gray) §20.1.2.3.9
    - green (Green) §20.1.2.3.10
    - greenMod (Green Modification) §20.1.2.3.11
    - greenOff (Green Offset) §20.1.2.3.12
    - hue (Hue) §20.1.2.3.14
    - hueMod (Hue Modulate) §20.1.2.3.15
    - hueOff (Hue Offset) §20.1.2.3.16
    - inv (Inverse) §20.1.2.3.17
    - invGamma (Inverse Gamma) §20.1.2.3.18
    - lum (Luminance) §20.1.2.3.19
    - lumMod (Luminance Modulation) §20.1.2.3.20
    - lumOff (Luminance Offset) §20.1.2.3.21
    - red (Red) §20.1.2.3.23
    - redMod (Red Modulation) §20.1.2.3.24
    - redOff (Red Offset) §20.1.2.3.25

 Attributes:
    - [done] b (blue)
    - [done] g (green)
    - [done] r (red)
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_scrgbClr()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    READ_ATTR_WITHOUT_NS(r)
    READ_ATTR_WITHOUT_NS(g)
    READ_ATTR_WITHOUT_NS(b)

    bool okR;
    bool okG;
    bool okB;

    m_currentColor = QColor::fromRgbF(qreal(MSOOXML::Utils::ST_Percentage_to_double(r, okR)),
                                      qreal(MSOOXML::Utils::ST_Percentage_to_double(g, okG)),
                                      qreal(MSOOXML::Utils::ST_Percentage_to_double(b, okB)));

    //TODO: all the color transformations
    while (true) {
        BREAK_IF_END_OF(CURRENT_EL);
        readNext();
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL srgbClr
//! RGB Color Model -Hex Digit Variant
//! DrawingML ECMA-376 20.1.2.3.32, p. 3085.
/*!
This element specifies a color in RGB notation.

 Child elements:
    - alpha (Alpha) §20.1.2.3.1
    - alphaMod (Alpha Modulation) §20.1.2.3.2
    - alphaOff (Alpha Offset) §20.1.2.3.3
    - blue (Blue) §20.1.2.3.4
    - blueMod (Blue Modification) §20.1.2.3.5
    - blueOff (Blue Offset) §20.1.2.3.6
    - comp (Complement) §20.1.2.3.7
    - gamma (Gamma) §20.1.2.3.8
    - gray (Gray) §20.1.2.3.9
    - green (Green) §20.1.2.3.10
    - greenMod (Green Modification) §20.1.2.3.11
    - greenOff (Green Offset) §20.1.2.3.12
    - hue (Hue) §20.1.2.3.14
    - hueMod (Hue Modulate) §20.1.2.3.15
    - hueOff (Hue Offset) §20.1.2.3.16
    - inv (Inverse) §20.1.2.3.17
    - invGamma (Inverse Gamma) §20.1.2.3.18
    - lum (Luminance) §20.1.2.3.19
    - lumMod (Luminance Modulation) §20.1.2.3.20
    - lumOff (Luminance Offset) §20.1.2.3.21
    - red (Red) §20.1.2.3.23
    - redMod (Red Modulation) §20.1.2.3.24
    - redOff (Red Offset) §20.1.2.3.25
    - sat (Saturation) §20.1.2.3.26
    - satMod (Saturation Modulation) §20.1.2.3.27
    - satOff (Saturation Offset) §20.1.2.3.28
    - shade (Shade) §20.1.2.3.31
    - tint (Tint) §20.1.2.3.34
 Attributes:
    - [done] val ("RRGGBB" hex digits)
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_srgbClr()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    READ_ATTR_WITHOUT_NS(val)

    m_currentColor = QColor( QLatin1Char('#') + val );

    //TODO: all the color transformations
    while (true) {
        BREAK_IF_END_OF(CURRENT_EL);
        readNext();
    }
    READ_EPILOGUE
}

KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::lvlHelper(const QString& level) {

    const QXmlStreamAttributes attrs(attributes());

    m_currentListStyleProperties = new KoListLevelProperties;

    // Number 3 makes eg. lvl4 -> 4
    m_currentListStyleProperties->setLevel(QString(level.at(3)).toInt());

    // To prevent the default bullet, as MS2007 does have it
    m_currentListStyleProperties->setBulletCharacter(QChar());
 
    TRY_READ_ATTR_WITHOUT_NS(marL)
    TRY_READ_ATTR_WITHOUT_NS(indent)

    bool marginalOk = true;
    bool indentOk = true;
    float marginal = 0;
    float ind = 0;

//! @todo Check if this conversion is really correct?

    if (!marL.isEmpty()) {
        marginal = int(TWIP_TO_DM(marL.toDouble(&marginalOk)));
    }
    if (!indent.isEmpty()) {
        ind = int(TWIP_TO_DM(indent.toDouble(&indentOk)));
    }

    if (marginalOk || indentOk) {
        m_currentListStyleProperties->setIndent(marginal + ind);
    }

    TRY_READ_ATTR_WITHOUT_NS(algn)
    if (!algn.isEmpty()) {
        if (algn == "l") {
            m_currentListStyleProperties->setAlignment(Qt::AlignLeft);
        }
        else if (algn == "ctr") {
            m_currentListStyleProperties->setAlignment(Qt::AlignHCenter);
        }
        else if (algn == "r") {
            m_currentListStyleProperties->setAlignment(Qt::AlignRight);
        }
//! @todo add rest of the alignments
    }

    TRY_READ_ATTR(defTabSz)

    m_currentTextStyle = KoGenStyle(KoGenStyle::TextAutoStyle, "text");

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(defRPr)
            ELSE_TRY_READ_IF(buNone)
            ELSE_TRY_READ_IF(buAutoNum)
            ELSE_TRY_READ_IF(buChar)
//! @todo add ELSE_WRONG_FORMAT
        }
        if (isEndElement() && qualifiedName() == QString("a:%1").arg(level)) {
            break;
        }
    }

/*
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    KoXmlWriter tempWriter(&buffer);
    m_currentTextStyle.writeStyleProperties(&tempWriter, KoGenStyle::TextType);
    QString content = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());

    listWriter.addCompleteElement(content.toUtf8());

    listWriter.endElement(); // text:list-level-style-number

    const QString elementContents = QString::fromUtf8(listBuf.buffer(), listBuf.buffer().size());

    m_currentListStyle.addChildElement(QString("level %1").arg(level), elementContents);
*/

    QBuffer listBuf;
    KoXmlWriter listStyleWriter(&listBuf);

    m_currentListStyleProperties->saveOdf(&listStyleWriter);

    const QString elementContents = QString::fromUtf8(listBuf.buffer(), listBuf.buffer().size());

    m_currentListStyle.addChildElement("list-style-properties", elementContents);

    delete m_currentListStyleProperties;
    m_currentListStyleProperties = 0;

    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL lvl1pPr
//! List level 1 text style
/*!

 Parent elements:
  - [done] bodyStyle (§19.3.1.5)
  - defaultTextStyle (§19.2.1.8)
  - lstStyle (§21.1.2.4.12)
  - notesStyle (§19.3.1.28)
  - otherStyle (§19.3.1.35)
  - titleStyle (§19.3.1.49)

 Child elements:
  - [done] buAutoNum (Auto-Numbered Bullet)     §21.1.2.4.1
  - buBlip (Picture Bullet)              §21.1.2.4.2
  - [done] buChar (Character Bullet)            §21.1.2.4.3
  - buClr (Color Specified)              §21.1.2.4.4
  - buClrTx (Follow Text)                §21.1.2.4.5
  - buFont (Specified)                   §21.1.2.4.6
  - buFontTx (Follow text)               §21.1.2.4.7
  - [done] buNone (No Bullet)                   §21.1.2.4.8
  - buSzPct (Bullet Size Percentage)     §21.1.2.4.9
  - buSzPts (Bullet Size Points)         §21.1.2.4.10
  - buSzTx (Bullet Size Follows Text)    §21.1.2.4.11
  - [done] defRPr (Default Text Run Properties) §21.1.2.3.2
  - extLst (Extension List)              §20.1.2.2.15
  - lnSpc (Line Spacing)                 §21.1.2.2.5
  - spcAft (Space After)                 §21.1.2.2.9
  - spcBef (Space Before)                §21.1.2.2.10
  - tabLst (Tab List)                    §21.1.2.2.14

*/
//! @todo support all child elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_lvl1pPr()
{
    READ_PROLOGUE
    lvlHelper("lvl1pPr");
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL lvl2pPr
//! Look for lvl1pPr documentation
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_lvl2pPr()
{
    READ_PROLOGUE
    lvlHelper("lvl2pPr");
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL lvl3pPr
//! Look for lvl1pPr documentation  
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_lvl3pPr()
{
    READ_PROLOGUE
    lvlHelper("lvl3pPr");
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL lvl4pPr
//! Look for lvl1pPr documentation
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_lvl4pPr()
{
    READ_PROLOGUE
    lvlHelper("lvl4pPr");
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL lvl5pPr
//! Look for lvl1pPr documentation
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_lvl5pPr()
{
    READ_PROLOGUE
    lvlHelper("lvl5pPr");
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL lvl6pPr
//! Look for lvl1pPr documentation
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_lvl6pPr()
{
    READ_PROLOGUE
    lvlHelper("lvl6pPr");
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL lvl7pPr
//! Look for lvl1pPr documentation
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_lvl7pPr()
{
    READ_PROLOGUE
    lvlHelper("lvl7pPr");
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL lvl8pPr
//! Look for lvl1pPr documentation
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_lvl8pPr()
{
    READ_PROLOGUE
    lvlHelper("lvl8pPr");
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL lvl9pPr
//! Look for lvl1pPr documentation
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_lvl9pPr()
{
    READ_PROLOGUE
    lvlHelper("lvl9pPr");
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL buChar
//! buChar - bullet character
/*!
 Parent elements:
 - defPPr (§21.1.2.2.2)
 - [done] lvl1pPr (§21.1.2.4.13)
 - [done] lvl2pPr (§21.1.2.4.14)
 - [done] lvl3pPr (§21.1.2.4.15)
 - [done] lvl4pPr (§21.1.2.4.16)
 - [done] lvl5pPr (§21.1.2.4.17)
 - [done] lvl6pPr (§21.1.2.4.18)
 - [done] lvl7pPr (§21.1.2.4.19)
 - [done] lvl8pPr (§21.1.2.4.20)
 - [done] lvl9pPr (§21.1.2.4.21)
 - [done] pPr (§21.1.2.2.7)
*/
//! @todo support all attributes
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_buChar()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());   

    if (attrs.hasAttribute("char")) {
        // Effectively converts this to QChar
        m_currentListStyleProperties->setBulletCharacter(attrs.value("char").toString().at(0));
    }

    m_listStylePropertiesAltered = true;

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL fld
//! fld - Text Field
/*!
 Parent elements:
 - [done] p (§21.1.2.2.6)

 Child elements:

 - [done] pPr (Text Paragraph Properties) §21.1.2.2.7
 - [done] rPr (Text Run Properties) §21.1.2.3.9
 - [done] t (Text String) §21.1.2.3.11

*/
//! @todo support all attributes
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_fld()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR(type)

    if (!type.isEmpty()) {
//! @todo support all possible fields here
    }

    while (!atEnd()) {
        if (isStartElement()) {
            if (QUALIFIED_NAME_IS(rPr)) {
                TRY_READ(DrawingML_rPr)
            }
            else if (QUALIFIED_NAME_IS(pPr)) {
                TRY_READ(DrawingML_pPr)
            }
            ELSE_TRY_READ_IF(t)
        }
        BREAK_IF_END_OF(CURRENT_EL);
        readNext();
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL spcBef
//! spcBef - spacing before
/*!
 Parent elements:

 - defPPr (§21.1.2.2.2)
 - [done] lvl1pPr (§21.1.2.4.13)
 - [done] lvl2pPr (§21.1.2.4.14)
 - [done] lvl3pPr (§21.1.2.4.15)
 - [done] lvl4pPr (§21.1.2.4.16)
 - [done] lvl5pPr (§21.1.2.4.17)
 - [done] lvl6pPr (§21.1.2.4.18)
 - [done] lvl7pPr (§21.1.2.4.19)
 - [done] lvl8pPr (§21.1.2.4.20)
 - [done] lvl9pPr (§21.1.2.4.21)
 - [done] pPr (§21.1.2.2.7)

 Child elements:

 - spcPct (Spacing Percent) §21.1.2.2.11
 - [done] spcPts (Spacing Points)  §21.1.2.2.12

*/
//! @todo support all attributes
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_spcBef()
{
    READ_PROLOGUE

    while (!atEnd()) {
        if (isStartElement()) {
            TRY_READ_IF(spcPts)
        }
        BREAK_IF_END_OF(CURRENT_EL);
        readNext();
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL lnSpc
//! lnSpc - line spacing
/*!
 Parent elements:

 - defPPr (§21.1.2.2.2)
 - lvl1pPr (§21.1.2.4.13)
 - lvl2pPr (§21.1.2.4.14)
 - lvl3pPr (§21.1.2.4.15)
 - lvl4pPr (§21.1.2.4.16)
 - lvl5pPr (§21.1.2.4.17)
 - lvl6pPr (§21.1.2.4.18)
 - lvl7pPr (§21.1.2.4.19)
 - lvl8pPr (§21.1.2.4.20)
 - lvl9pPr (§21.1.2.4.21)
 - [done] pPr (§21.1.2.2.7)

 Child elements:

 - [done] spcPct (Spacing Percent) §21.1.2.2.11
 - spcPts (Spacing Points)  §21.1.2.2.12

*/
//! @todo support all attributes
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_lnSpc()
{
    READ_PROLOGUE
    while (!atEnd()) {
        if (isStartElement()) {
            TRY_READ_IF(spcPct)
        }
        BREAK_IF_END_OF(CURRENT_EL);
        readNext();
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL spcPts
//! spcPts - spacing points
/*!
 Parent elements:
 - lnSpc (§21.1.2.2.5)
 - spcAft (§21.1.2.2.9)
 - [done] spcBef (§21.1.2.2.10)
*/
//! @todo support all attributes
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_spcPts()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
   
    TRY_READ_ATTR_WITHOUT_NS(val)

    bool ok = false;
    const int marginTop = val.toDouble(&ok);

    if (ok) {
        m_currentParagraphStyle.addPropertyPt("fo:margin-top", marginTop/100);
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL spcPct
//! spcPct - spacing percentage
/*!
 Parent elements:
 - [done] lnSpc (§21.1.2.2.5)
 - spcAft (§21.1.2.2.9)
 - spcBef (§21.1.2.2.10)
*/
//! @todo support all attributes
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_spcPct()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
   
    TRY_READ_ATTR_WITHOUT_NS(val)
    bool ok = false;
    int lineSpace = val.toDouble(&ok)/1000;
    if (ok) {
        QString space = "%1";
        space = space.arg(lineSpace);
        space.append('%');
        m_currentParagraphStyle.addProperty("fo:line-height", space);
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL buNone
//! buNone - No bullets
/*!
 Parent elements:
 - defPPr (§21.1.2.2.2)
 - [done] lvl1pPr (§21.1.2.4.13)
 - [done] lvl2pPr (§21.1.2.4.14)
 - [done] lvl3pPr (§21.1.2.4.15)
 - [done] lvl4pPr (§21.1.2.4.16)
 - [done] lvl5pPr (§21.1.2.4.17)
 - [done] lvl6pPr (§21.1.2.4.18)
 - [done] lvl7pPr (§21.1.2.4.19)
 - [done] lvl8pPr (§21.1.2.4.20)
 - [done] lvl9pPr (§21.1.2.4.21)
 - [done] pPr (§21.1.2.2.7)
*/
//! @todo support all attributes
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_buNone()
{
    READ_PROLOGUE
    m_currentListStyleProperties->setBulletCharacter(QChar());
    m_listStylePropertiesAltered = true;
    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL buAutoNum
//! buAutoNum - Bullet Auto Numbering
/*!
 Parent elements:
 - defPPr (§21.1.2.2.2)
 - [done] lvl1pPr (§21.1.2.4.13)
 - [done] lvl2pPr (§21.1.2.4.14)
 - [done] lvl3pPr (§21.1.2.4.15)
 - [done] lvl4pPr (§21.1.2.4.16)
 - [done] lvl5pPr (§21.1.2.4.17)
 - [done] lvl6pPr (§21.1.2.4.18)
 - [done] lvl7pPr (§21.1.2.4.19)
 - [done] lvl8pPr (§21.1.2.4.20)
 - [done] lvl9pPr (§21.1.2.4.21)
 - [done] pPr (§21.1.2.2.7)
*/
//! @todo support all attributes
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_buAutoNum()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR_WITHOUT_NS(type)

    if (!type.isEmpty()) {
        if (type == "arabicPeriod") {
            m_currentListStyleProperties->setListItemSuffix(".");
            m_currentListStyleProperties->setStyle(KoListStyle::DecimalItem);
        }
        else if (type == "arabicParenR") {
            m_currentListStyleProperties->setListItemSuffix(")");
            m_currentListStyleProperties->setStyle(KoListStyle::DecimalItem);
        }
        else if (type == "alphaUcPeriod") {
            m_currentListStyleProperties->setListItemSuffix(".");
            m_currentListStyleProperties->setStyle(KoListStyle::UpperAlphaItem);
        }
        else if (type == "alphaLcPeriod") {
            m_currentListStyleProperties->setListItemSuffix(".");
            m_currentListStyleProperties->setStyle(KoListStyle::AlphaLowerItem);
        }
        else if (type == "alphaUcParenR") {
            m_currentListStyleProperties->setListItemSuffix(")");
            m_currentListStyleProperties->setStyle(KoListStyle::UpperAlphaItem);
        }
        else if (type == "alphaLcParenR") {
            m_currentListStyleProperties->setListItemSuffix(")");
            m_currentListStyleProperties->setStyle(KoListStyle::UpperAlphaItem);
        }
        else if (type == "romanUcPeriod") {
            m_currentListStyleProperties->setListItemSuffix(".");
            m_currentListStyleProperties->setStyle(KoListStyle::UpperRomanItem);
        }
        else if (type == "romanLcPeriod") {
            m_currentListStyleProperties->setListItemSuffix(".");
            m_currentListStyleProperties->setStyle(KoListStyle::RomanLowerItem);
        }
        else if (type == "romanUcParenR") {
            m_currentListStyleProperties->setListItemSuffix(")");
            m_currentListStyleProperties->setStyle(KoListStyle::UpperRomanItem);
        }
        else if (type == "romanLcParenR") {
            m_currentListStyleProperties->setListItemSuffix(")");
            m_currentListStyleProperties->setStyle(KoListStyle::RomanLowerItem);
        }
    }

    TRY_READ_ATTR_WITHOUT_NS(startAt)
    if (!startAt.isEmpty()) {
        m_currentListStyleProperties->setStartValue(startAt.toInt());
    }

    m_listStylePropertiesAltered = true;
    readNext();

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL defRPr
//! defRPr - Default Text Run Properties
/*!

 Parent elements:
     - defPPr (§21.1.2.2.2)
     - [done] lvl1pPr (§21.1.2.4.13)
     - [done] lvl2pPr (§21.1.2.4.14)
     - [done] lvl3pPr (§21.1.2.4.15)
     - [done] lvl4pPr (§21.1.2.4.16)
     - [done] lvl5pPr (§21.1.2.4.17)
     - [done] lvl6pPr (§21.1.2.4.18)
     - [done] lvl7pPr (§21.1.2.4.19)
     - [done] lvl8pPr (§21.1.2.4.20)
     - [done] lvl9pPr (§21.1.2.4.21) 
     - pPr (§21.1.2.2.7)

 Child elements:
     - blipFill (Picture Fill)                         §20.1.8.14
     - cs (Complex Script Font)                        §21.1.2.3.1
     - ea (East Asian Font)                            §21.1.2.3.3
     - effectDag (Effect Container)                    §20.1.8.25
     - effectLst (Effect Container)                    §20.1.8.26
     - extLst (Extension List)                         §20.1.2.2.15
     - gradFill (Gradient Fill)                        §20.1.8.33
     - grpFill (Group Fill)                            §20.1.8.35
     - highlight (Highlight Color)                     §21.1.2.3.4
     - hlinkClick (Click Hyperlink)                    §21.1.2.3.5
     - hlinkMouseOver (Mouse-Over Hyperlink)           §21.1.2.3.6
     - latin (Latin Font)                              §21.1.2.3.7
     - ln (Outline)                                    §20.1.2.2.24
     - noFill (No Fill)                                §20.1.8.44
     - pattFill (Pattern Fill)                         §20.1.8.47
     - rtl (Right to Left Run)                         §21.1.2.2.8
     - solidFill (Solid Fill)                          §20.1.8.54
     - sym (Symbol Font)                               §21.1.2.3.10
     - uFill (Underline Fill)                          §21.1.2.3.12
     - uFillTx (Underline Fill Properties Follow Text) §21.1.2.3.13
     - uLn (Underline Stroke)                          §21.1.2.3.14
     - uLnTx (Underline Follows Text)                  §21.1.2.3.15
*/
//! @todo support all child elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_defRPr()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    m_currentTextStyleProperties = new KoCharacterStyle();

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    TRY_READ_ATTR_WITHOUT_NS(sz)
    if (!sz.isEmpty()) {
        bool ok = false;
        const qreal pointSize = qreal(attrs.value("sz").toString().toUInt(&ok)) / 100.0;
        if (ok) {
            m_currentTextStyleProperties->setFontPointSize(pointSize);
        }
    }

    m_currentTextStyleProperties->saveOdf(m_currentTextStyle);
    delete m_currentTextStyleProperties;
    m_currentTextStyleProperties = 0;

    READ_EPILOGUE
}

#endif
