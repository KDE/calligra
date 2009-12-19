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

#ifndef MSOOXMLCOMMONREADERDRAWINGML_IMPL_H
#define MSOOXMLCOMMONREADERDRAWINGML_IMPL_H

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
    m_xlinkHref.clear();

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
            ELSE_TRY_READ_IF(blipFill)
            ELSE_TRY_READ_IF(nvPicPr)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    body = drawFrameBuf.originalWriter();
//    delete body;
//    body = origBody;
    {
        body->startElement("draw:frame"); // CASE #P421
#ifdef PPTXXMLSLIDEREADER_H
        body->addAttribute("draw:layer", "layout");
        body->addAttribute("presentation:user-transformed", MsooXmlReader::constTrue);
#endif
//todo        body->addAttribute("presentation:style-name", styleName);
//! @todo for pptx: maybe use KoGenStyle::StylePresentationAuto?
#ifdef DOCXXMLDOCREADER_H
        QString currentDrawStyleName(mainStyles->lookup(m_currentDrawStyle));
#endif
#ifdef HARDCODED_PRESENTATIONSTYLENAME
//! @todo hardcoded draw:style-name = gr1
        QString currentDrawStyleName("gr1");
#endif
        kDebug() << "currentDrawStyleName:" << currentDrawStyleName;
        body->addAttribute("draw:style-name", currentDrawStyleName);
        body->addAttribute("text:anchor-type", "char");
        if (!m_docPrName.isEmpty()) { // from docPr/@name
            body->addAttribute("draw:name", m_docPrName);
        }
//! @todo text:anchor-type hardcoded!
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
            if (!m_cNvPrName.isEmpty()) {
                body->startElement("svg:title");
                body->addTextSpan(m_cNvPrName);
                body->endElement(); //svg:title
            }
#endif
            m_xlinkHref.clear();
        }

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
            ELSE_TRY_READ_IF(cNvPr)
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
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_cNvPr()
{
    ReadMethod caller = m_calls.top();
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    if (CALLER_IS(nvSpPr) || CALLER_IS(nvPicPr)) { // for sanity, p:nvGrpSpPr can be also the caller
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
            TRY_READ_IF(cNvPr)
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
    - txBox (Text Box)
*/
//! @todo support all child elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_cNvSpPr()
{
    READ_PROLOGUE
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
    - [done] txBody (Shape Text Body) §19.3.1.51
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

#ifdef PPTXXMLSLIDEREADER_H
    if (m_context->type == Slide) {
    } else if (m_context->type == SlideMaster) {
        m_currentShapeProperties = new PptxShapeProperties();
        m_context->slideProperties->shapes.append(m_currentShapeProperties);
    }
    m_phType.clear();
#endif
    m_cNvPrId.clear();
    m_cNvPrName.clear();

    MSOOXML::Utils::XmlWriteBuffer drawFrameBuf; // buffer this draw:frame, because we have
    // to write after the child elements are generated
//    QBuffer drawFrameBuf;
//    KoXmlWriter *origBody = body;
#ifdef PPTXXMLSLIDEREADER_H
    const bool outputDrawFrame = m_context->type == Slide;
#else
    const bool outputDrawFrame = true;
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
            else if (m_context->type == Slide) {
                TRY_READ_IF(txBody)
            }
#endif
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    if (outputDrawFrame) {
//        delete body;
//        body = origBody;
        body = drawFrameBuf.originalWriter();

        body->startElement("draw:frame"); // CASE #P475
#ifdef PPTXXMLSLIDEREADER_H
        body->addAttribute("draw:layer", "layout");
        body->addAttribute("presentation:user-transformed", MsooXmlReader::constTrue);
//todo        body->addAttribute("presentation:style-name", styleName);
# ifdef HARDCODED_PRESENTATIONSTYLENAME
        d->presentationStyleNameCount++;
        body->addAttribute("presentation:style-name",
                           d->presentationStyleNameCount == 1 ? "pr1" : "pr2");
# endif

// CASE #P476
        body->addAttribute("draw:id", m_cNvPrId);
        body->addAttribute("presentation:class",
                           MSOOXML::Utils::ST_PlaceholderType_to_ODF(m_phType.toLatin1()));
//! @todo if there's no data in spPr tag, use the one from the slide layout, then from the master slide
        body->addAttribute("svg:x", EMU_TO_CM_STRING(m_svgX));
        body->addAttribute("svg:y", EMU_TO_CM_STRING(m_svgY));
        body->addAttribute("svg:width", EMU_TO_CM_STRING(m_svgWidth));
        body->addAttribute("svg:height", EMU_TO_CM_STRING(m_svgHeight));
#else
#ifdef __GNUC__
#warning TODO: docx
#endif
#endif

        (void)drawFrameBuf.releaseWriter();
//        body->addCompleteElement(&drawFrameBuf);
        body->endElement(); //draw:frame
    }

#ifdef PPTXXMLSLIDEREADER_H
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
    - noFill (No Fill) §20.1.8.44
    - pattFill (Pattern Fill) §20.1.8.47
    - prstGeom (Preset geometry) §20.1.9.18
    - scene3d (3D Scene Properties) §20.1.4.1.26
    - solidFill (Solid Fill) §20.1.8.54
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
    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            if (qualifiedName() == QLatin1String("a:xfrm")) {
                TRY_READ(xfrm)
                xfrm_read = true;
            }
//! @todo a:prstGeom...
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

#ifdef PPTXXMLSLIDEREADER_H
//! @todo
    if (m_context->type == Slide && !xfrm_read) { // loading values from master is needed
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
#endif

    READ_EPILOGUE
}

#undef MSOOXML_CURRENT_NS
#define MSOOXML_CURRENT_NS "a"

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

    bool off_read = false, ext_read = false;
    while (true) {
        BREAK_IF_END_OF(CURRENT_EL);
        readNext();
        kDebug() << *this;
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
            ERROR_NO_ELEMENT("a:off")
        }
        if (!ext_read) {
            ERROR_NO_ELEMENT("a:ext")
        }
        m_currentShapeProperties->x = m_svgX;
        m_currentShapeProperties->y = m_svgY;
        m_currentShapeProperties->width = m_svgWidth;
        m_currentShapeProperties->height = m_svgHeight;
        kDebug() << "Saved to m_currentShapeProperties";
    }
#endif
    kDebug()
    << "svg:x" << m_svgX
    << "svg:y" << m_svgY
    << "svg:width" << m_svgWidth
    << "svg:height" << m_svgHeight;

    while (true) {
        BREAK_IF_END_OF(CURRENT_EL);
        readNext();
    }

    READ_EPILOGUE
}

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
//! 20.1.7.4 off (Offset)
#undef CURRENT_EL
#define CURRENT_EL off
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_off()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    READ_ATTR_WITHOUT_NS(x)
    STRING_TO_INT(x, m_svgX, "off@x")
//    m_svgX = MSOOXML::Utils::EMU_to_ODF_CM(x);
    READ_ATTR_WITHOUT_NS(y)
    STRING_TO_INT(y, m_svgY, "off@y")
//    m_svgY = MSOOXML::Utils::EMU_to_ODF_CM(y);
//    if (m_svgX.isEmpty() || m_svgY.isEmpty())
//        return KoFilter::WrongFormat;

    while (true) {
        BREAK_IF_END_OF(CURRENT_EL);
        readNext();
        break;
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
        break;
    }

    READ_EPILOGUE
}

#endif
