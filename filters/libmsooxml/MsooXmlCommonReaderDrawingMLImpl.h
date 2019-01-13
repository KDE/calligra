/*
 * This file is part of Office 2007 Filters for Calligra
 *
 * Copyright (C) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * Copyright (C) 2011-2012 Matus Uzak (matus.uzak@gmail.com).
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

#include <QTime>
#include <math.h>

#ifndef M_PI
#define M_PI 3.1415926535897932384626
#endif

//ECMA-376, 20.1.10.68, p.3431 - ST_TextFontSize (Text Font Size)
#define TEXT_FONTSIZE_MIN 1
#define TEXT_FONTSIZE_MAX 4000
#define TEXT_FONTSIZE_DEFAULT 18

#if !defined DRAWINGML_NS && !defined NO_DRAWINGML_NS
#error missing DRAWINGML_NS define!
#endif
#if !defined DRAWINGML_PIC_NS && !defined NO_DRAWINGML_PIC_NS
#error missing DRAWINGML_PIC_NS define!
#endif

// ================================================================
//                     Namespace in {a,pic,p,xdr}
// ================================================================
#undef MSOOXML_CURRENT_NS
#ifndef NO_DRAWINGML_PIC_NS
#define MSOOXML_CURRENT_NS DRAWINGML_PIC_NS
#endif

#include <KoXmlWriter.h>
#include <MsooXmlUnits.h>
#include "Charting.h"
#include "XlsxChartOdfWriter.h"
#include "XlsxXmlChartReader.h"
#include "ComplexShapeHandler.h"

#include <MsooXmlReader.h>
#include <MsooXmlCommonReader.h>
#include <MsooXmlDiagramReader.h>
#include <QScopedPointer>

// ================================================================


void MSOOXML_CURRENT_CLASS::initDrawingML()
{
    m_currentDoubleValue = 0;
    m_hyperLink = false;
    m_listStylePropertiesAltered = false;
    m_inGrpSpPr = false;
    m_insideTable = false;
    m_isLockedCanvas = false;
    qsrand(QTime::currentTime().msec());
}

bool MSOOXML_CURRENT_CLASS::isCustomShape()
{
    if (m_contentType.isEmpty()) {
	return false;
    }
    if (m_contentType == "rect") {
	return false;
    }
    if (unsupportedPredefinedShape()) {
	return false;
    }
    return true;
}

bool MSOOXML_CURRENT_CLASS::unsupportedPredefinedShape()
{
    // TODO: Some conditions are not supported with custom shapes
    // properly yet, remove them when possible.

    // Custom geometry has its own handling
    if (m_contentType == "custom") {
        return false;
    }

    // Lines and connectors are handled elsewhere
    if (m_contentType == "line" || m_contentType == "arc" || m_contentType.contains("Connector")) {
        return false;
    }

    // These shapes are not properly supported atm. some have bugs in
    // predefinedShapes.xml, some might have xml_parser/calligra bugs.
    if (m_contentType == "circularArrow" || m_contentType == "curvedDownArrow" ||
        m_contentType == "curvedLeftArrow" || m_contentType == "curvedUpArrow" ||
        m_contentType == "curvedRightArrow" || m_contentType == "gear6" ||
        m_contentType == "gear9") {
        return true;
    }
    return false;
}

/* bodyPr (Body Properties) defaults
 * ECMA-376, 21.1.2.1.1, p.3556 - DrawingML
 */
void MSOOXML_CURRENT_CLASS::inheritDefaultBodyProperties()
{
    if (m_shapeTextPosition.isEmpty()) {
        m_shapeTextPosition = "top";
    }
    if (m_shapeTextTopOff.isEmpty()) {
        m_shapeTextTopOff = "45720";
    }
    if (m_shapeTextLeftOff.isEmpty()) {
        m_shapeTextLeftOff = "91440";
    }
    if (m_shapeTextRightOff.isEmpty()) {
        m_shapeTextRightOff = "91440";
    }
    if (m_shapeTextBottomOff.isEmpty()) {
        m_shapeTextBottomOff = "45720";
    }
}

// ----------------------------------------------------------------

// ================================================================
// DrawingML tags
// ================================================================

// Don't show this warning: it occurs because gcc gets confused, but
// it _is_ used.
#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

static QString mirrorToOdf(bool flipH, bool flipV)
{
    if (!flipH && !flipV)
        return QString();
    if (flipH && flipV)
        return QLatin1String("horizontal vertical");
    if (flipH)
        return QLatin1String("horizontal");
    if (flipV)
        return QLatin1String("vertical");
    return QLatin1String("none");
}

#undef CURRENT_EL
#define CURRENT_EL pic
//! pic (Picture)
//! ECMA-376, 19.3.1.37, p.2848 (PresentationML)
//! ECMA-376, 20.1.2.2.30, p.3049 (DrawingML)
//! ECMA-376, 20.2.2.5, p.3462
/*! This element specifies the existence of a picture object within
  the document.

  Parent elements:
  ----------------
  PresentationML:
  - control (§19.3.2.1)
  - [done] grpSp (§19.3.1.22)
  - oleObj (§19.3.2.4)
  - [done] spTree (§19.3.1.45)

  DrawingML:
  - [done] grpSp (§20.1.2.2.20)
  - [done] lockedCanvas (§20.3.2.1)

  Child elements:
  ---------------
  PresentationML:
  - [done] blipFill (Picture Fill) §19.3.1.4
  - extLst (Extension List with Modification Flag) §19.3.1.20
  - [done] nvPicPr (Non-Visual Properties for a Picture) §19.3.1.32
  - [done] spPr (Shape Properties) §19.3.1.44
  - [done] style (Shape Style) §19.3.1.46

  DrawingML:
  - [done] blipFill (Picture Fill) §20.1.8.14
  - extLst (Extension List) §20.1.2.2.15
  - [done] nvPicPr (Non-Visual Properties for a Picture) §20.1.2.2.28
  - [done] spPr (Shape Properties) §20.1.2.2.35
  - [done] style (Shape Style) §20.1.2.2.37

  DrawingML/Picture
  - [done] blipFill (Picture Fill) §20.2.2.1
  - [done] nvPicPr (Non-Visual Picture Properties) §20.2.2.4
  - [done] spPr (Shape Properties) §20.2.2.6
*/
//! CASE #P401
//! @todo CASE #P421
//! CASE #P422
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_pic()
{
    if (m_isLockedCanvas) {
        READ_PROLOGUE_IF_NS(a);
    } else {
        READ_PROLOGUE
    }

    // Reset picture properties
    m_xlinkHref.clear();
    m_ignoreLinkHref = false;
    m_cNvPrId.clear();
    m_cNvPrName.clear();
    m_cNvPrDescr.clear();
    m_flipH = false;
    m_flipV = false;
    m_rot = 0;

#ifdef XLSXXMLDRAWINGREADER_CPP
    KoXmlWriter *tempBodyHolder = 0;
    if ( m_currentDrawingObject->isAnchoredToCell() && (m_context->m_groupDepthCounter == 0)) {
        tempBodyHolder = body;
        body = m_currentDrawingObject->pictureWriter();
    }
#endif

#ifndef DOCXXMLDOCREADER_CPP
    // Create a new drawing style for this picture
    pushCurrentDrawStyle(new KoGenStyle(KoGenStyle::GraphicAutoStyle, "graphic"));
#endif

    m_referredFont = KoGenStyle(KoGenStyle::TextAutoStyle, "text");

    if (m_isLockedCanvas) {
        while (!atEnd()) {
            readNext();
            debugMsooXml << *this;
            BREAK_IF_END_OF_WITH_NS(a, CURRENT_EL)
            if (isStartElement()) {
                TRY_READ_IF_NS(a, spPr)
                ELSE_TRY_READ_IF_NS_IN_CONTEXT(a, blipFill)
                ELSE_TRY_READ_IF_NS(a, nvPicPr)
                ELSE_TRY_READ_IF_NS(a, style)
                SKIP_UNKNOWN
            }
        }
    } else {
        while (!atEnd()) {
            readNext();
            debugMsooXml << *this;
            BREAK_IF_END_OF(CURRENT_EL)
            if (isStartElement()) {
                TRY_READ_IF(spPr)
                else if (name() == QLatin1String("blipFill")) {
                    TRY_READ_IN_CONTEXT(blipFill)
                    if (!m_xlinkHref.isEmpty()) {
                        //spPr may also set m_xlinkHref
                        m_ignoreLinkHref = true;
                    }
                }
#ifdef DOCXXMLDOCUMENTREADER_H
                ELSE_TRY_READ_IF_NS_IN_CONTEXT(pic, blipFill)
#endif
                ELSE_TRY_READ_IF(nvPicPr)
                ELSE_TRY_READ_IF(style)
                SKIP_UNKNOWN
            }
        }
    }

    m_ignoreLinkHref = false;

#ifndef DOCXXMLDOCUMENTREADER_H
    body->startElement("draw:frame"); // CASE #P421
#ifdef PPTXXMLSLIDEREADER_CPP
    if (m_context->type == Slide || m_context->type == SlideLayout) {
        body->addAttribute("draw:layer", "layout");
    }
    else { // Slidemaster
        body->addAttribute("draw:layer", "backgroundobjects");
    }
    body->addAttribute("presentation:user-transformed", MsooXmlReader::constTrue);
#endif

#ifdef XLSXXMLDRAWINGREADER_CPP
    if (m_context->m_groupDepthCounter == 0) {
        if (m_currentDrawingObject->m_positions.contains(XlsxDrawingObject::FromAnchor)) {
            XlsxDrawingObject::Position f = m_currentDrawingObject->m_positions[XlsxDrawingObject::FromAnchor];
            // use relative position to the cell's top-left corner
            m_svgX = f.m_colOff;
            m_svgY = f.m_rowOff;

            if(m_currentDrawingObject->m_positions.contains(XlsxDrawingObject::ToAnchor)) {
                f = m_currentDrawingObject->m_positions[XlsxDrawingObject::ToAnchor];
                QString endCellAddress = m_currentDrawingObject->toCellAddress();
                QString end_x = EMU_TO_CM_STRING(f.m_colOff);
                QString end_y = EMU_TO_CM_STRING(f.m_rowOff);

                body->addAttribute("table:end-cell-address", endCellAddress);
                body->addAttribute("table:end-x", end_x); //cm
                body->addAttribute("table:end-y", end_y); //cm
            }
        }
    }
#endif

    if (m_rot == 0) {
        body->addAttribute("svg:x", EMU_TO_CM_STRING(m_svgX));
        body->addAttribute("svg:y", EMU_TO_CM_STRING(m_svgY));
    }
    if (m_svgWidth > 0) {
        body->addAttribute("svg:width", EMU_TO_CM_STRING(m_svgWidth));
    }
    if (m_svgHeight > 0) {
        body->addAttribute("svg:height", EMU_TO_CM_STRING(m_svgHeight));
    }

    if (m_rot != 0) {
        // m_rot is in 1/60,000th of a degree
        qreal angle, xDiff, yDiff;
        MSOOXML::Utils::rotateString(m_rot, m_svgWidth, m_svgHeight, angle, xDiff, yDiff);
        QString rotString = QString("rotate(%1) translate(%2cm %3cm)")
                            .arg(angle).arg((m_svgX + xDiff)/360000, 3, 'f').arg((m_svgY + yDiff)/360000, 3, 'f');
        body->addAttribute("draw:transform", rotString);
    }

    // Add style information
    //! @todo: horizontal-on-{odd,even}?
    const QString mirror(mirrorToOdf(m_flipH, m_flipV));
    if (!mirror.isEmpty()) {
        m_currentDrawStyle->addProperty("style:mirror", mirror);
    }

#ifdef PPTXXMLSLIDEREADER_CPP
    if (m_context->type == SlideMaster || m_context->type == NotesMaster) {
        m_currentDrawStyle->setAutoStyleInStylesDotXml(true);
    }
#elif defined DOCXXMLDOCREADER_CPP
    if (m_moveToStylesXml) {
        m_currentDrawStyle.setAutoStyleInStylesDotXml(true);
    }
#endif
    const QString styleName(mainStyles->insert(*m_currentDrawStyle, "gr"));
    body->addAttribute("draw:style-name", styleName);
#endif

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
        body->endElement(); //draw:image

#ifdef DOCXXMLDOCUMENTREADER_H
        if (!m_cNvPrName.isEmpty() || !m_cNvPrDescr.isEmpty()) {
            body->startElement("svg:title");
            body->addTextSpan(m_cNvPrDescr.isEmpty() ? m_cNvPrName : m_cNvPrDescr);
            body->endElement(); //svg:title
        }
#endif
        m_xlinkHref.clear();
    }

#ifndef DOCXXMLDOCUMENTREADER_H
    body->endElement(); //draw:frame
#endif

#ifdef XLSXXMLDRAWINGREADER_CPP
    // If we anchored to cell, we save odf to different buffer that body operates on
    // Here we restore the original body buffer for next drawing which might be anchored
    // to sheet
    if ( m_currentDrawingObject->isAnchoredToCell() && (m_context->m_groupDepthCounter == 0)) {
        body = tempBodyHolder;
    }
#endif


#ifndef DOCXXMLDOCREADER_CPP
    popCurrentDrawStyle();
#endif

    if (m_isLockedCanvas) {
        READ_EPILOGUE_IF_NS(a)
    } else {
        READ_EPILOGUE
    }
}

#undef CURRENT_EL
#define CURRENT_EL nvPicPr
//! nvPicPr (Non-Visual Properties for a Picture)
//! ECMA-376, 19.3.1.32, p.2845 (PresentationML)
//! ECMA-376, 20.1.2.2.28, p. 3048 (DrawingML)
/*! This element specifies all non-visual properties for a picture.

  Parent elements:
  ----------------
  PresentationML/DrawingML/SpreadsheetML:
  - [done] pic (§19.3.1.37)/(§20.1.2.2.30)/(§20.5.2.25)

  Child elements:
  ---------------
  PresentationML:
  - [done] cNvPicPr (Non-Visual Picture Drawing Properties) §19.3.1.11
  - [done] cNvPr (Non-Visual Drawing Properties) §19.3.1.12
  - [done] nvPr (Non-Visual Properties) §19.3.1.33

  DrawingML:
  - [done] cNvPicPr (Non-Visual Picture Drawing Properties) §20.1.2.2.7
  - [done] cNvPr (Non-Visual Drawing Properties) §20.1.2.2.8

  SpreadsheetML:
  - [done] cNvPicPr (Non-Visual Picture Drawing Properties) §20.5.2.7
  - [done] cNvPr (Non-Visual Drawing Properties) §20.5.2.8
*/
//! @todo support all child elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_nvPicPr()
{
    if (m_isLockedCanvas) {
        READ_PROLOGUE_IF_NS(a);
    } else {
        READ_PROLOGUE
    }

    if (m_isLockedCanvas) {
        while (!atEnd()) {
            readNext();
            debugMsooXml << *this;
            BREAK_IF_END_OF_WITH_NS(a, CURRENT_EL)
            if (isStartElement()) {
                TRY_READ_IF_NS(a, cNvPicPr)
                ELSE_TRY_READ_IF_NS_IN_CONTEXT(a, cNvPr)
                ELSE_WRONG_FORMAT
            }
        }
    } else {
        while (!atEnd()) {
            readNext();
            debugMsooXml << *this;
            BREAK_IF_END_OF(CURRENT_EL)
            if (isStartElement()) {
                TRY_READ_IF(cNvPicPr)
                ELSE_TRY_READ_IF_IN_CONTEXT(cNvPr)
#ifdef PPTXXMLSLIDEREADER_CPP
                ELSE_TRY_READ_IF(nvPr) // only §19.3.1.33
#endif
                ELSE_WRONG_FORMAT
            }
        }
    }

    if (m_isLockedCanvas) {
        READ_EPILOGUE_IF_NS(a)
    } else {
        READ_EPILOGUE
    }
}

#undef CURRENT_EL
#define CURRENT_EL cNvPicPr
//! cNvPicPr handler (Non-Visual Picture Drawing Properties)
//! ECMA-376, 19.3.1.11, p.2823; (PresentationML)
//! ECMA-376, 20.2.2.2, p.3458 (DrawingML)
/*!
 This element specifies the non-visual properties for the picture
 canvas.  These properties are to be used by the generating
 application to determine how certain properties are to be changed for
 the picture object in question.

 Parent elements:
 - [done] nvPicPr (§19.3.1.32)

 Child elements:
 - extLst (Extension List) §20.1.2.2.15
 - picLocks (Picture Locks) §20.1.2.2.31

 Attributes:
 - preferRelativeResize (Relative Resize Preferred)
*/
//! @todo support all child elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_cNvPicPr()
{
    if (m_isLockedCanvas) {
        READ_PROLOGUE_IF_NS(a);
    } else {
        READ_PROLOGUE
    }

    SKIP_EVERYTHING

//     while (!atEnd()) {
//         readNext();
//         debugMsooXml << *this;
//         BREAK_IF_END_OF(CURRENT_EL)
//         if (isStartElement()) {
// //! @todo add ELSE_WRONG_FORMAT
//         }
//     }

    if (m_isLockedCanvas) {
        READ_EPILOGUE_IF_NS(a)
    } else {
        READ_EPILOGUE
    }
}

#undef CURRENT_EL
#define CURRENT_EL cNvPr
//! cNvPr handler (Non-Visual Drawing Properties)
//! ECMA-376, 19.3.1.12, p.2824 (PresentationML)
//! ECMA-376, 20.2.2.3, p.3459 (DrawingML)
/*! This element specifies non-visual canvas properties.  This allows
 for additional information that does not affect the appearance of the
 picture to be stored.

 Parent elements:
 ----------------
 PresentationML/SpreadsheetML:
 - [done] nvCxnSpPr (§19.3.1.29)
 - nvGraphicFramePr (§19.3.1.30)
 - nvGrpSpPr (§19.3.1.31)
 - [done] nvPicPr (§19.3.1.32)
 - [done] nvSpPr (§19.3.1.34)

 DrawingML:
 - [done] nvPicPr (§20.2.2.4)
 //NOTE: Part of nvCxnSpPr child list (20.1.2.2.25)

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
    if (m_isLockedCanvas) {
        READ_PROLOGUE_IF_NS(a);
    } else {
        READ_PROLOGUE
    }

    m_cNvPrId.clear();
    m_cNvPrName.clear();
    m_cNvPrDescr.clear();
    const QXmlStreamAttributes attrs(attributes());

    // for sanity, p:nvGrpSpPr can be also the caller
    if (caller == cNvPr_nvSpPr || caller == cNvPr_nvPicPr) {
        READ_ATTR_WITHOUT_NS_INTO(id, m_cNvPrId)
        debugMsooXml << "id:" << m_cNvPrId;
        TRY_READ_ATTR_WITHOUT_NS_INTO(name, m_cNvPrName)
        debugMsooXml << "name:" << m_cNvPrName;
        TRY_READ_ATTR_WITHOUT_NS_INTO(descr, m_cNvPrDescr)
        debugMsooXml << "descr:" << m_cNvPrDescr;
    }

    SKIP_EVERYTHING

    // while (!atEnd()) {
    //     readNext();
    //     debugMsooXml << *this;
    //     BREAK_IF_END_OF(CURRENT_EL)
    //     if (isStartElement()) {
    //         TRY_READ_IF(...)
    //         //! @todo add ELSE_WRONG_FORMAT
    //     }
    // }

    if (m_isLockedCanvas) {
        READ_EPILOGUE_IF_NS(a)
    } else {
        READ_EPILOGUE
    }
}

#undef CURRENT_EL
#define CURRENT_EL nvSpPr
//! nvSpPr handler (Non-Visual Properties for a Shape)
//! ECMA-376, 19.3.1.34, p. 2846
//! ECMA-376, 20.1.2.2.29, p. 3049
/*!
  This element specifies all non-visual properties for a shape.
  This element is a container for the non-visual identification
  properties, shape properties and application properties that are
  to be associated with a shape.  This allows for additional
  information that does not affect the appearance of the shape to be
  stored.

  Parent elements:
  PresentationML/DrawingML:
  - [done] sp (§19.3.1.43)

  Child elements:
  PresentationML:
  - [done] cNvPr (Non-Visual Drawing Properties) §19.3.1.12
  - [done] cNvSpPr (Non-Visual Drawing Properties for a Shape) §19.3.1.13
  - [done] nvPr (Non-Visual Properties) §19.3.1.33

  DrawingML:
  - [done] cNvPr (Non-Visual Drawing Properties) §20.1.2.2.8
  - [done] cNvSpPr (Non-Visual Drawing Properties for a Shape) §20.1.2.2.9

  SpreadsheetML:
  - [done] cNvPr (Non-Visual Drawing Properties) §20.5.2.8
  - [done] cNvSpPr (Connection Non-Visual Shape Properties) §20.5.2.9
*/
//! @todo support all child elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_nvSpPr()
{
    if (m_isLockedCanvas) {
        READ_PROLOGUE_IF_NS(a);
    } else {
        READ_PROLOGUE
    }

    if (m_isLockedCanvas) {
        while (!atEnd()) {
            readNext();
            debugMsooXml << *this;
            BREAK_IF_END_OF_WITH_NS(a, CURRENT_EL)
            if (isStartElement()) {
                TRY_READ_IF_NS_IN_CONTEXT(a, cNvPr)
                ELSE_TRY_READ_IF_NS(a, cNvSpPr)
                ELSE_WRONG_FORMAT
            }
        }
    } else {
        while (!atEnd()) {
            readNext();
            debugMsooXml << *this;
            BREAK_IF_END_OF(CURRENT_EL)
            if (isStartElement()) {
                TRY_READ_IF_IN_CONTEXT(cNvPr)
                ELSE_TRY_READ_IF(cNvSpPr)
#ifdef PPTXXMLSLIDEREADER_CPP
                ELSE_TRY_READ_IF(nvPr) // only §19.3.1.33
#endif
                ELSE_WRONG_FORMAT

            }
        }
    }

#ifdef PPTXXMLSLIDEREADER_CPP
    inheritShapeGeometry();
#endif

    if (m_isLockedCanvas) {
        READ_EPILOGUE_IF_NS(a)
    } else {
        READ_EPILOGUE
    }
}

#undef CURRENT_EL
#define CURRENT_EL grpSp
//! grpSp handler (Group shape)
//! ECMA-376, 19.3.1.22, p.2836 (PresentationML)
//! ECMA-376, 20.1.2.2.20, p.3038 (DrawingML)
/*!
  Parent elements:
  ----------------
  PresentationML:
  - [done] grpSp (§19.3.1.22)
  - [done] spTree (§19.3.1.45)

  DrawingML:
  - [done] grpSp (§20.1.2.2.20)
  - [done] lockedCanvas (§20.3.2.1)

  SpreadsheetML:
  - [done] absoluteAnchor (§20.5.2.1)
  - [done] grpSp (§20.5.2.17)
  - [done] oneCellAnchor (§20.5.2.24)
  - [done] twoCellAnchor (§20.5.2.33)

  Child elements:
  ---------------
  PresentationML:
  - contentPart (Content Part) §19.3.1.14
  - [done] cxnSp (Connection Shape) §19.3.1.19
  - extLst (Extension List with Modification Flag) §19.3.1.20
  - [done] graphicFrame (Graphic Frame) §19.3.1.21
  - [done] grpSp (Group Shape) §19.3.1.22
  - [done] grpSpPr (Group Shape Properties) §19.3.1.23
  - nvGrpSpPr (Non-Visual Properties for a Group Shape) §19.3.1.31
  - [done] pic (Picture) §19.3.1.37
  - [done] sp (Shape) §19.3.1.43

  DrawingML:
  - [done] cxnSp (Connection Shape) §20.1.2.2.10
  - extLst (Extension List) §20.1.2.2.15
  - graphicFrame (Graphic Frame) §20.1.2.2.18
  - [done] grpSp (Group shape) §20.1.2.2.20
  - [done] grpSpPr (Visual Group Shape Properties) §20.1.2.2.22
  - nvGrpSpPr (Non-Visual Properties for a Group Shape) §20.1.2.2.27
  - [done] pic (Picture) §20.1.2.2.30
  - [done] sp (Shape) §20.1.2.2.33
  - [done] txSp (Text Shape) §20.1.2.2.41

  SpreadsheetML:
  - [done] cxnSp (Connection Shape) §20.5.2.13
  - [done] graphicFrame (Graphic Frame) §20.5.2.16
  - [done] grpSp (Group Shape) §20.5.2.17
  - [done] grpSpPr (Group Shape Properties) §20.5.2.18
  - nvGrpSpPr (Non-Visual Properties for a Group Shape) §20.5.2.21
  - [done] pic (Picture) §20.5.2.25
  - [done] sp (Shape) §20.5.2.29
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_grpSp()
{
    if (m_isLockedCanvas) {
        READ_PROLOGUE_IF_NS(a);
    } else {
        READ_PROLOGUE
    }

    pushCurrentDrawStyle(new KoGenStyle(KoGenStyle::GraphicAutoStyle, "graphic"));
    MSOOXML::Utils::XmlWriteBuffer drawFrameBuf; // buffer this draw:g, because we have

    {
        MSOOXML::Utils::AutoRestore<KoXmlWriter> autoRestoreBody(&body);

        // to write after the child elements are generated
        body = drawFrameBuf.setWriter(body);

#ifdef XLSXXMLDRAWINGREADER_CPP
        m_context->m_groupDepthCounter++;
#endif
        if (m_isLockedCanvas) {
            while (!atEnd()) {
                readNext();
                BREAK_IF_END_OF_WITH_NS(a, CURRENT_EL)
                debugMsooXml << *this;
                if (isStartElement()) {
                    TRY_READ_IF_NS(a, grpSp)
                    ELSE_TRY_READ_IF_NS(a, grpSpPr)
                    ELSE_TRY_READ_IF_NS(a, pic)
                    ELSE_TRY_READ_IF_NS(a, sp)
                    ELSE_TRY_READ_IF_NS(a, cxnSp)
                    // ELSE_TRY_READ_IF_NS(a, graphicFrame)
                    ELSE_TRY_READ_IF_NS(a, txSp)
                    SKIP_UNKNOWN
                    //! @todo add ELSE_WRONG_FORMAT
                }
            }
        } else {
            while (!atEnd()) {
                readNext();
                BREAK_IF_END_OF(CURRENT_EL)
                debugMsooXml << *this;
                if (isStartElement()) {
                    TRY_READ_IF(grpSp)
                    ELSE_TRY_READ_IF(grpSpPr)
                    ELSE_TRY_READ_IF(pic)
                    ELSE_TRY_READ_IF(sp)
                    ELSE_TRY_READ_IF(cxnSp)
#if defined PPTXXMLSLIDEREADER_CPP || defined XLSXXMLDRAWINGREADER_CPP
                    ELSE_TRY_READ_IF(graphicFrame)
#endif
                    SKIP_UNKNOWN
                    //! @todo add ELSE_WRONG_FORMAT
                }
            }
        }
#ifdef XLSXXMLDRAWINGREADER_CPP
        m_context->m_groupDepthCounter--;
#endif
    }

    body = drawFrameBuf.originalWriter();
    body->startElement("draw:g");

#ifdef PPTXXMLSLIDEREADER_CPP
    if (m_context->type == SlideMaster || m_context->type == NotesMaster) {
        m_currentDrawStyle->setAutoStyleInStylesDotXml(true);
    }
#elif defined DOCXXMLDOCREADER_CPP
    if (m_moveToStylesXml) {
        m_currentDrawStyle->setAutoStyleInStylesDotXml(true);
    }
#endif
    const QString styleName(mainStyles->insert(*m_currentDrawStyle, "gr"));
    body->addAttribute("draw:style-name", styleName);

    (void)drawFrameBuf.releaseWriter();

    body->endElement(); // draw:g

    // Properties are set in grpSpPr
    if (!m_svgProp.isEmpty()) {
        m_svgProp.pop_back();
    } else {
        warnMsooXml << "Element grpSpPr not processed, empty graphic style assigned to draw:g";
    }

    popCurrentDrawStyle();

    if (m_isLockedCanvas) {
        READ_EPILOGUE_IF_NS(a)
    } else {
        READ_EPILOGUE
    }
}

#undef CURRENT_EL
#define CURRENT_EL grpSpPr
//! grpSpPr (Group Shape Properties)
//! ECMA-376, 19.3.1.23, p.2837 (PresentationML)
//! ECMA-376, 20.1.2.2.22, p.3041 (DrawingML)
/*!
 Parent elements:
 ----------------
 PresentationML:
 - [done] grpSp (§19.3.1.22);
 - [done] spTree (§19.3.1.45)

 DrawingML:
 - [done] grpSp (§20.1.2.2.20)
 - [done] lockedCanvas (§20.3.2.1)

 Child elements:
 - [done] blipFill (Picture Fill) §20.1.8.14
 - effectDag (Effect Container) §20.1.8.25
 - [done] effectLst (Effect Container) §20.1.8.26
 - extLst (Extension List) §20.1.2.2.15
 - [done] gradFill (Gradient Fill) §20.1.8.33
 - grpFill (Group Fill) §20.1.8.35
 - [done] noFill (No Fill) §20.1.8.44
 - pattFill (Pattern Fill) §20.1.8.47
 - scene3d (3D Scene Properties) §20.1.4.1.26
 - [done] solidFill (Solid Fill) §20.1.8.54
 - [done] xfrm (2D Transform for Grouped Objects) §20.1.7.5
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_grpSpPr()
{
    if (m_isLockedCanvas) {
        READ_PROLOGUE_IF_NS(a);
    } else {
        READ_PROLOGUE
    }
    m_inGrpSpPr = true;

    while (!atEnd()) {
        readNext();
        debugMsooXml << *this;
        if (m_isLockedCanvas) {
            BREAK_IF_END_OF_WITH_NS(a, CURRENT_EL)
        } else {
            BREAK_IF_END_OF(CURRENT_EL)
        }
        if (isStartElement()) {
            TRY_READ_IF_NS(a, xfrm)
            else if (qualifiedName() == QLatin1String("a:effectLst")) {
                TRY_READ(effectLst)
            }
            else if (qualifiedName() == QLatin1String("a:solidFill")) {
                TRY_READ(solidFill)
                // We must set the color immediately, otherwise currentColor may be modified by eg. ln
                if (m_currentColor != QColor()) {
                    m_currentDrawStyle->addProperty("draw:fill", QLatin1String("solid"));
                    m_currentDrawStyle->addProperty("draw:fill-color", m_currentColor.name());
                    m_currentColor = QColor();
                }
            }
            else if ( qualifiedName() == QLatin1String("a:ln") ) {
                TRY_READ(ln)
            }
            else if (qualifiedName() == QLatin1String("a:noFill")) {
                m_currentDrawStyle->addProperty("draw:fill", "none");
            }
            else if (qualifiedName() == QLatin1String("a:blipFill")) {
                TRY_READ_IN_CONTEXT(blipFill)
                if (!m_xlinkHref.isEmpty()) {
                    KoGenStyle fillStyle = KoGenStyle(KoGenStyle::FillImageStyle);
                    fillStyle.addProperty("xlink:href", m_xlinkHref);
                    fillStyle.addProperty("xlink:type", "simple");
                    fillStyle.addProperty("xlink:actuate", "onLoad");
                    const QString imageName = mainStyles->insert(fillStyle);
                    m_currentDrawStyle->addProperty("draw:fill", "bitmap");
                    m_currentDrawStyle->addProperty("draw:fill-image-name", imageName);
                    m_xlinkHref.clear();
                }
            }
            else if (qualifiedName() == QLatin1String("a:gradFill")) {
                m_currentGradientStyle = KoGenStyle(KoGenStyle::LinearGradientStyle);
                TRY_READ(gradFill)
                m_currentDrawStyle->addProperty("draw:fill", "gradient");
                const QString gradName = mainStyles->insert(m_currentGradientStyle);
                m_currentDrawStyle->addProperty("draw:fill-gradient-name", gradName);
            }
            SKIP_UNKNOWN
        //! @todo add ELSE_WRONG_FORMAT
        }
    }

    GroupProp prop;
    prop.svgXOld = m_svgX;
    prop.svgYOld = m_svgY;
    prop.svgWidthOld = m_svgWidth;
    prop.svgHeightOld = m_svgHeight;
    prop.svgXChOld = m_svgChX;
    prop.svgYChOld = m_svgChY;
    prop.svgWidthChOld = m_svgChWidth;
    prop.svgHeightChOld = m_svgChHeight;

    m_svgProp.push_back(prop);

    m_inGrpSpPr = false;

    if (m_isLockedCanvas) {
        READ_EPILOGUE_IF_NS(a)
    } else {
        READ_EPILOGUE
    }
}

#undef CURRENT_EL
#define CURRENT_EL nvCxnSpPr
//! nvCxnSpPr (Non-Visual Properties for a Connection Shape)
//! ECMA-376, 19.3.1.29 (PresentationML)
//! ECMA-376, 20.1.2.2.25 (DrawingML)
//! ECMA-376, 20.5.2.19 (SpreadsheetML)
/*!
  Parent Elements:
  ----------------
  PresentationML/DrawingML:
  - [done] cxnSp (§19.3.1.19)/(§20.1.2.2.10)

  Child Elements:
  ---------------
  PresentaionML:
  - cNvCxnSpPr (Non-Visual Connector Shape Drawing Properties) §19.3.1.8
  - [done] cNvPr (Non-Visual Drawing Properties) §19.3.1.12
  - [done] nvPr (Non-Visual Properties) §19.3.1.33

  DrawingML:
  - cNvCxnSpPr (Non-Visual Connector Shape Drawing Properties) §20.1.2.2.4
  - [done] cNvPr (Non-Visual Drawing Properties) §20.1.2.2.8

  SpreadsheetML:
  - cNvCxnSpPr (Non-Visual Connector Shape Drawing Properties) §20.5.2.4
  - [done] cNvPr (Non-Visual Drawing Properties) §20.5.2.8
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_nvCxnSpPr()
{
    if (m_isLockedCanvas) {
        READ_PROLOGUE_IF_NS(a);
    } else {
        READ_PROLOGUE
    }

    if (m_isLockedCanvas) {
        while (!atEnd()) {
            readNext();
            debugMsooXml << *this;
            BREAK_IF_END_OF_WITH_NS(a, CURRENT_EL)
            if (isStartElement()) {
                TRY_READ_IF_NS_IN_CONTEXT(a, cNvPr)
                SKIP_UNKNOWN
            }
        }
    } else {
        while (!atEnd()) {
            readNext();
            debugMsooXml << *this;
            BREAK_IF_END_OF(CURRENT_EL)
            if (isStartElement()) {
                TRY_READ_IF_IN_CONTEXT(cNvPr)
#ifdef PPTXXMLSLIDEREADER_CPP
                ELSE_TRY_READ_IF(nvPr) // only §19.3.1.33
#endif
                SKIP_UNKNOWN
            }
        }
    }

    if (m_isLockedCanvas) {
        READ_EPILOGUE_IF_NS(a)
    } else {
        READ_EPILOGUE
    }
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
    if (m_isLockedCanvas) {
        READ_PROLOGUE_IF_NS(a);
    } else {
        READ_PROLOGUE
    }

    SKIP_EVERYTHING

    // const QXmlStreamAttributes attrs(attributes());

    // Read attributes
    // FIXME: Make a member?
    //bool isTextBox = MSOOXML::Utils::convertBooleanAttr(attrs.value("txBox").toString(), false);

    // while (!atEnd()) {
    //     readNext();
    //     debugMsooXml << *this;
    //     BREAK_IF_END_OF(CURRENT_EL)
    //     if (isStartElement()) {
    //         TRY_READ_IF(...)
    //         //! @todo add ELSE_WRONG_FORMAT
    //     }
    // }

    if (m_isLockedCanvas) {
        READ_EPILOGUE_IF_NS(a)
    } else {
        READ_EPILOGUE
    }
}

void MSOOXML_CURRENT_CLASS::preReadSp()
{
    // Reset the position and size
    m_svgX = 0;
    m_svgY = 0;
    m_svgWidth = -1;
    m_svgHeight = -1;
    m_xfrm_read = false;
    m_flipH = false;
    m_flipV = false;
    m_rot = 0;

#ifdef PPTXXMLSLIDEREADER_CPP
    //We assume that the textbox is empty by default
    d->textBoxHasContent = false;

    m_currentPresentationStyle = KoGenStyle(KoGenStyle::PresentationAutoStyle, "presentation");
    if (m_context->type == SlideMaster || m_context->type == NotesMaster) {
        m_currentPresentationStyle.setAutoStyleInStylesDotXml(true);
    }

    if (m_context->type == SlideMaster || m_context->type == NotesMaster) {
        m_currentShapeProperties = new PptxShapeProperties();
    }
    else if (m_context->type == SlideLayout) {
        // moved down
        m_currentShapeProperties = 0;
    }
#endif

    m_cNvPrId.clear();
    m_cNvPrName.clear();
    m_cNvPrDescr.clear();
}

void MSOOXML_CURRENT_CLASS::generateFrameSp()
{
    inheritDefaultBodyProperties();

#ifdef PPTXXMLSLIDEREADER_CPP
    debugMsooXml << "outputDrawFrame for" << (m_context->type == SlideLayout ? "SlideLayout" : "Slide");

    // Properties may or may not override default ones.
    inheritBodyProperties();

    // FIXME: The draw:fit-to-size attribute specifies whether to stretch the
    // text content of a drawing object to fill an entire object.  The
    // style:shrink-to-fit attribute specifies whether content is reduced in
    // size to fit within a cell or drawing object.  Shrinking means that the
    // font size of the content is decreased to fit the content into a cell or
    // drawing object.  That's needed to be compatible with MS PowerPoint.  Any
    // margin, padding or indent MUST be retained.
    if (m_normAutofit == MSOOXML::Utils::autoFitOn) {
        m_currentPresentationStyle.addProperty("draw:fit-to-size", "true", KoGenStyle::GraphicType);
    }
#endif
    if (m_contentType == "line" || m_contentType == "arc") {
        body->startElement("draw:line");
    }
    else if (m_contentType.contains("Connector")) {
        // This should be maybe draw:connector but calligra doesn't
        // seem to handle that element atm.
        body->startElement("draw:line");
    }
    else if (m_contentType == "custom") {
        body->startElement("draw:custom-shape");
    }
    else if (!isCustomShape()) {
#ifdef PPTXXMLSLIDEREADER_CPP
        if (d->phType == "sldImg") {
	    // Special feature for presentation notes
            body->startElement("draw:page-thumbnail");
        } else {
            body->startElement("draw:frame");
        }
#else
        body->startElement("draw:frame");
#endif
    }
    // For predefined shapes
    else {
        body->startElement("draw:custom-shape");
    }

    if (!m_cNvPrName.isEmpty()) {
        body->addAttribute("draw:name", m_cNvPrName);
    }

    m_currentDrawStyle->addProperty("draw:textarea-vertical-align", m_shapeTextPosition);
    m_currentDrawStyle->addProperty("fo:padding-left", EMU_TO_CM_STRING(m_shapeTextLeftOff.toInt()));
    m_currentDrawStyle->addProperty("fo:padding-right", EMU_TO_CM_STRING(m_shapeTextRightOff.toInt()));
    m_currentDrawStyle->addProperty("fo:padding-top", EMU_TO_CM_STRING(m_shapeTextTopOff.toInt()));
    m_currentDrawStyle->addProperty("fo:padding-bottom", EMU_TO_CM_STRING(m_shapeTextBottomOff.toInt()));

#ifdef PPTXXMLSLIDEREADER_CPP
    if (m_context->type == SlideMaster || m_context->type == NotesMaster) {
        m_currentDrawStyle->setAutoStyleInStylesDotXml(true);
    }

    // NOTE: Workaround: Set padding to ZERO until the fo:wrap-option support
    // arrives and other text on shape related issues get fixed.
    if (isCustomShape()) {
        m_currentDrawStyle->removeProperty("fo:padding-left");
        m_currentDrawStyle->removeProperty("fo:padding-right");
        m_currentDrawStyle->removeProperty("fo:padding-top");
        m_currentDrawStyle->removeProperty("fo:padding-bottom");
        m_currentDrawStyle->addPropertyPt("fo:padding", 0);
    }

#elif defined DOCXXMLDOCREADER_CPP
    if (m_moveToStylesXml) {
        m_currentDrawStyle->setAutoStyleInStylesDotXml(true);
    }
#endif

#ifndef PPTXXMLSLIDEREADER_CPP
    const QString styleName(mainStyles->insert(*m_currentDrawStyle, "gr"));
    body->addAttribute("draw:style-name", styleName);
#else
    const QString presentationClass(MSOOXML::Utils::ST_PlaceholderType_to_ODF(d->phType));

    if (m_context->type == Slide || m_context->type == SlideLayout) {
        body->addAttribute("draw:layer", "layout");
    }
    else {
        body->addAttribute("draw:layer", "backgroundobjects");
        // Phtype will be empty for any text that is in masterslide that is wanted
        // to be shown in the actual slides, such as company names etc.
        if (!d->phType.isEmpty()) {
            body->addAttribute("presentation:placeholder", "true");
            body->addAttribute("presentation:class", presentationClass);
        }
    }

    // only either draw:style-name or presentation:style-name
    // is allowed, but not both.
    if (!m_currentPresentationStyle.isEmpty() || !m_currentPresentationStyle.parentName().isEmpty()) {
        KoGenStyle::copyPropertiesFromStyle(*m_currentDrawStyle, m_currentPresentationStyle, KoGenStyle::GraphicType);
        KoGenStyle::copyPropertiesFromStyle(*m_currentDrawStyle, m_currentPresentationStyle, KoGenStyle::TextType);
        KoGenStyle::copyPropertiesFromStyle(*m_currentDrawStyle, m_currentPresentationStyle, KoGenStyle::ParagraphType);
        const QString presentationStyleName = mainStyles->insert(m_currentPresentationStyle, "pr");
        body->addAttribute("presentation:style-name", presentationStyleName);
    } else {
        const QString styleName(mainStyles->insert(*m_currentDrawStyle, "gr"));
        body->addAttribute("draw:style-name", styleName);
    }
    inheritShapePosition();

    if (m_context->type == Slide) {
        // CASE #P476
        QString id = "slide" + QString::number(m_context->slideNumber) + "item"
                + m_cNvPrId;
        body->addAttribute("draw:id", id);
        body->addAttribute("xml:id", id);
        body->addAttribute("presentation:class", presentationClass);
        debugMsooXml << "presentationClass:" << d->phType << "->" << presentationClass;
        debugMsooXml << "m_svgWidth:" << m_svgWidth << "m_svgHeight:" << m_svgHeight
                 << "m_svgX:" << m_svgX << "m_svgY:" << m_svgY;
    }
#endif

    if (m_svgWidth > -1 && m_svgHeight > -1) {
#ifdef PPTXXMLSLIDEREADER_CPP
        body->addAttribute("presentation:user-transformed", MsooXmlReader::constTrue);
#endif
        if (m_contentType == "line" || m_contentType == "arc" || m_contentType.contains("Connector")) {
#ifdef XLSXXMLDRAWINGREADER_CPP
            XlsxDrawingObject::Position f = m_currentDrawingObject->m_positions[XlsxDrawingObject::FromAnchor];
            body->addAttributePt("svg:x", EMU_TO_POINT(f.m_colOff));
            body->addAttributePt("svg:y", EMU_TO_POINT(f.m_rowOff));
            QString y1 = EMU_TO_CM_STRING(f.m_rowOff);
            QString y2 = EMU_TO_CM_STRING(f.m_rowOff + m_svgHeight);
            QString x1 = EMU_TO_CM_STRING(f.m_colOff);
            QString x2 = EMU_TO_CM_STRING(f.m_colOff + m_svgWidth);
            if (m_rot != 0) {
                qreal angle, xDiff, yDiff;
                if (m_flipH ^ m_flipV) {
                    MSOOXML::Utils::rotateString(-m_rot, m_svgWidth, m_svgHeight, angle, xDiff, yDiff);
                } else {
                    MSOOXML::Utils::rotateString(m_rot, m_svgWidth, m_svgHeight, angle, xDiff, yDiff);
                }
                //! @todo, in case of connector, these should maybe be reversed?
                x1 = EMU_TO_CM_STRING(f.m_colOff + xDiff);
                y1 = EMU_TO_CM_STRING(f.m_rowOff + yDiff);
                x2 = EMU_TO_CM_STRING(f.m_colOff + m_svgWidth - xDiff);
                y2 = EMU_TO_CM_STRING(f.m_rowOff + m_svgHeight - yDiff);
            }
#else
            QString y1 = EMU_TO_CM_STRING(m_svgY);
            QString y2 = EMU_TO_CM_STRING(m_svgY + m_svgHeight);
            QString x1 = EMU_TO_CM_STRING(m_svgX);
            QString x2 = EMU_TO_CM_STRING(m_svgX + m_svgWidth);
            if (m_rot != 0) {
                qreal angle, xDiff, yDiff;
                // handle flipping of lines, logical XOR here
                if (m_flipH ^ m_flipV) {
                    MSOOXML::Utils::rotateString(-m_rot, m_svgWidth, m_svgHeight, angle, xDiff, yDiff);
                } else {
                    MSOOXML::Utils::rotateString(m_rot, m_svgWidth, m_svgHeight, angle, xDiff, yDiff);
                }

                //! @todo, in case of connector, these should maybe be reversed?
                x1 = EMU_TO_CM_STRING(m_svgX + xDiff);
                y1 = EMU_TO_CM_STRING(m_svgY + yDiff);
                x2 = EMU_TO_CM_STRING(m_svgX + m_svgWidth - xDiff);
                y2 = EMU_TO_CM_STRING(m_svgY + m_svgHeight - yDiff);
            }
#endif
            if (m_flipV) {
                QString temp = y2;
                y2 = y1;
                y1 = temp;
            }
            if (m_flipH) {
                QString temp = x2;
                x2 = x1;
                x1 = temp;
            }
            body->addAttribute("svg:x1", x1);
            body->addAttribute("svg:y1", y1);
            body->addAttribute("svg:x2", x2);
            body->addAttribute("svg:y2", y2);
        }
        else {
            if (m_rot == 0) {
                body->addAttribute("svg:x", EMU_TO_CM_STRING(m_svgX));
                body->addAttribute("svg:y", EMU_TO_CM_STRING(m_svgY));
            } else {
                // m_rot is in 1/60,000th of a degree
                qreal angle, xDiff, yDiff;

                // text-box vertical flipping is done with rotation by +180 degrees
                // mirror/flip flag is not available in odf for text-box
                if (m_contentType == "rect" && m_flipV) {
                    MSOOXML::Utils::rotateString(m_rot + (180 * 60000), m_svgWidth, m_svgHeight, angle, xDiff, yDiff);
                } else {
                    MSOOXML::Utils::rotateString(m_rot, m_svgWidth, m_svgHeight, angle, xDiff, yDiff);
                }

                QString rotString = QString("rotate(%1) translate(%2cm %3cm)").
                                    arg(angle).arg((m_svgX + xDiff)/360000, 3, 'f').
                                    arg((m_svgY + yDiff)/360000, 3, 'f');
                body->addAttribute("draw:transform", rotString);
            }
            body->addAttribute("svg:width", EMU_TO_CM_STRING(m_svgWidth));
            body->addAttribute("svg:height", EMU_TO_CM_STRING(m_svgHeight));
        }
    }
}

void MSOOXML_CURRENT_CLASS::writeEnhancedGeometry()
{
    if (!isCustomShape()) {
        return;
    }
    body->startElement("draw:enhanced-geometry");
    body->addAttribute("svg:viewBox", QString("0 0 %1 %2").arg(m_svgWidth).arg(m_svgHeight));

    if (m_flipV) {
        body->addAttribute("draw:mirror-vertical", "true");
    }
    if (m_flipH) {
        body->addAttribute("draw:mirror-horizontal", "true");
    }

    if (m_contentType == "custom") {
        body->addAttribute("draw:enhanced-path", m_customPath);
        if (!m_textareas.isEmpty()) {
            body->addAttribute("draw:text-areas", m_textareas);
        }
        if (!m_customEquations.isEmpty()) {
            body->addCompleteElement(m_customEquations.toUtf8());
        }
    } else {
        body->addAttribute("draw:enhanced-path", m_context->import->m_shapeHelper.attributes.value(m_contentType));

        QString textareas = m_context->import->m_shapeHelper.textareas.value(m_contentType);
        if (!textareas.isEmpty()) {
            body->addAttribute("draw:text-areas", textareas);
        }
        QString equations = m_context->import->m_shapeHelper.equations.value(m_contentType);

        // Some of the values might be overwritten by prstGeom.
        if (m_contentAvLstExists) {
            QMapIterator<QString, QString> i(m_avModifiers);
            while (i.hasNext()) {
                i.next();
                int index = 0;
                index = equations.indexOf(i.key());
                if (index > -1) {
                    // We go forward by name and '" draw:formula="'
                    index += i.key().length() + 16;
                    equations.replace(index, equations.indexOf('\"', index) - index, i.value());
                }
            }
        }
        if (!equations.isEmpty()) {
            body->addCompleteElement(equations.toUtf8());
        }
    }
    body->endElement(); // draw:enhanced-geometry
}

#undef CURRENT_EL
#define CURRENT_EL cxnSp
//! cxnSp (Connection Shape)
//! ECMA-376, 19.3.1.19, p.2833 (PresentationML)
//! ECMA-376, 20.1.2.2.10, p.3029 (DrawingML)
/*
   This element specifies a connection shape that is used to connect
   two sp elements.  Once a connection is specified using a cxnSp, it
   is left to the generating application to determine the exact path
   the connector takes.  That is the connector routing algorithm is
   left up to the generating application as the desired path might be
   different depending on the specific needs of the application.

   Parent Elements:
   ----------------
   PresentationML:
   - [done] grpSp (§19.3.1.22)
   - [done] spTree (§19.3.1.45)

   DrawingML:
   - [done] grpSp (§20.1.2.2.20)
   - [done] lockedCanvas (§20.3.2.1)

   Child Elements:
   ---------------
   PresentationML:
   - extLst (Extension List with Modification Flag) §19.3.1.20
   - [done] nvCxnSpPr (Non-Visual Properties for a Connection Shape) §19.3.1.29
   - [done] spPr (Shape Properties) §19.3.1.44
   - [done] style (Shape Style) §19.3.1.46

   DrawingML:
   - extLst (Extension List) §20.1.2.2.15
   - [done] nvCxnSpPr (Non-Visual Properties for a Connection Shape) §20.1.2.2.25
   - [done] spPr (Shape Properties) §20.1.2.2.35
   - [done] style (Shape Style) §20.1.2.2.37
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_cxnSp()
{
    if (m_isLockedCanvas) {
        READ_PROLOGUE_IF_NS(a);
    } else {
        READ_PROLOGUE
    }

    preReadSp();

    pushCurrentDrawStyle(new KoGenStyle(KoGenStyle::GraphicAutoStyle, "graphic"));

    MSOOXML::Utils::XmlWriteBuffer drawFrameBuf; // buffer this draw:frame, because we have
    // to write after the child elements are generated
    body = drawFrameBuf.setWriter(body);

    m_referredFont = KoGenStyle(KoGenStyle::TextAutoStyle, "text");

    if (m_isLockedCanvas) {
        while (!atEnd()) {
            readNext();
            debugMsooXml << *this;
            BREAK_IF_END_OF_WITH_NS(a, CURRENT_EL)
            if (isStartElement()) {
                TRY_READ_IF_NS(a, nvCxnSpPr)
                ELSE_TRY_READ_IF_NS(a, spPr)
                ELSE_TRY_READ_IF_NS(a, style)
                SKIP_UNKNOWN
            }
        }
    } else {
        while (!atEnd()) {
            readNext();
            debugMsooXml << *this;
            BREAK_IF_END_OF(CURRENT_EL)
            if (isStartElement()) {
                TRY_READ_IF(nvCxnSpPr)
                ELSE_TRY_READ_IF(spPr)
                ELSE_TRY_READ_IF(style)
                SKIP_UNKNOWN
                //! @todo add ELSE_WRONG_FORMAT
            }
        }
    }

    body = drawFrameBuf.originalWriter();

    generateFrameSp();

    (void)drawFrameBuf.releaseWriter();

    if (isCustomShape()) {
	writeEnhancedGeometry();
    }
    body->endElement(); //draw:frame, //draw:line

#ifdef PPTXXMLSLIDEREADER_CPP
    KoFilter::ConversionStatus stat = generatePlaceHolderSp();
    if (stat != KoFilter::OK) {
        return stat;
    }
#endif

    popCurrentDrawStyle();

    if (m_isLockedCanvas) {
        READ_EPILOGUE_IF_NS(a)
    } else {
        READ_EPILOGUE
    }
}

#undef CURRENT_EL
#define CURRENT_EL sp
//! sp handler (Shape)
//! ECMA-376, 19.3.1.43, p.2854 (PresentationML)
//! ECMA-376, 20.1.2.2.33, p.3053 (DrawingML)
/*! This element specifies the existence of a single shape.  A shape
  can either be a preset or a custom geometry, defined using the
  DrawingML framework.

 Parent elements:
 ----------------
 PresentationML:
 - [done] grpSp (§19.3.1.22)
 - [done] spTree (§19.3.1.45)

 DrawingML:
 - [done] grpSp (§20.1.2.2.20)
 - [done] lockedCanvas (§20.3.2.1)

 Child elements:
 PresentationML:
 - extLst (Extension List with Modification Flag) §19.3.1.20
 - [done] nvSpPr (Non-Visual Properties for a Shape) §19.3.1.34
 - [done] spPr (Shape Properties) §19.3.1.44
 - [done] style (Shape Style) §19.3.1.46
 - [done] txBody (Shape Text Body) §19.3.1.51 - PML

 DrawingML:
 - extLst (Extension List) §20.1.2.2.15
 - [done] nvSpPr (Non-Visual Properties for a Shape) §20.1.2.2.29
 - [done] spPr (Shape Properties) §20.1.2.2.35
 - [done] style (Shape Style) §20.1.2.2.37
 - [done] txSp (Text Shape) §20.1.2.2.41

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
    if (m_isLockedCanvas) {
        READ_PROLOGUE_IF_NS(a);
    } else {
        READ_PROLOGUE
    }

    m_contentType.clear();
    m_xlinkHref.clear();

    preReadSp();

    pushCurrentDrawStyle(new KoGenStyle(KoGenStyle::GraphicAutoStyle, "graphic"));

    MSOOXML::Utils::XmlWriteBuffer drawFrameBuf; // buffer this draw:frame, because we have
    // to write after the child elements are generated
    body = drawFrameBuf.setWriter(body);

    m_referredFont = KoGenStyle(KoGenStyle::TextAutoStyle, "text");


    if (m_isLockedCanvas) {
        while (!atEnd()) {
            readNext();
            debugMsooXml << *this;
            BREAK_IF_END_OF_WITH_NS(a, CURRENT_EL)
            if (isStartElement()) {
                TRY_READ_IF_NS(a, nvSpPr)
                ELSE_TRY_READ_IF_NS(a, spPr)
                ELSE_TRY_READ_IF_NS(a, style)
                ELSE_TRY_READ_IF_NS(a, txSp)
                SKIP_UNKNOWN
                //! @todo add ELSE_WRONG_FORMAT
            }
        }
    } else {
        while (!atEnd()) {
            readNext();
            debugMsooXml << *this;
            BREAK_IF_END_OF(CURRENT_EL)
            if (isStartElement()) {
                TRY_READ_IF(nvSpPr)
                ELSE_TRY_READ_IF(spPr)
                ELSE_TRY_READ_IF(style)
#if defined PPTXXMLSLIDEREADER_CPP
                ELSE_TRY_READ_IF(txBody)
#else
                else if (qualifiedName() == QLatin1String(QUALIFIED_NAME(txBody))) {
                    TRY_READ_IN_CONTEXT(DrawingML_txBody)
                }
#endif
                SKIP_UNKNOWN
                //! @todo add ELSE_WRONG_FORMAT
            }
        }
    }

    body = drawFrameBuf.originalWriter();

    generateFrameSp();

    (void)drawFrameBuf.releaseWriter();

    if (isCustomShape()) {
	writeEnhancedGeometry();
    }
    body->endElement(); //draw:frame, //draw:line

#ifdef PPTXXMLSLIDEREADER_CPP
    KoFilter::ConversionStatus stat = generatePlaceHolderSp();
    if (stat != KoFilter::OK) {
        return stat;
    }
#endif

    popCurrentDrawStyle();

    if (m_isLockedCanvas) {
        READ_EPILOGUE_IF_NS(a)
    } else {
        READ_EPILOGUE
    }
}

#undef CURRENT_EL
#define CURRENT_EL style
//! style handler (Shape style)
//! ECMA-376, 21.3.2.24, p.3943 (PresentationML)
//! ECMA-376, 20.1.2.2.37, p.3055 (DrawingML)
/*!
 Parent elements:
 ----------------
 PresentationML/SpreadsheetML:
 - [done] cxnSp (§19.3.1.19);
 - [done] pic (§19.3.1.37);
 - [done] sp (§19.3.1.43)

 DrawingML:
 - [done] cxnSp (§20.1.2.2.10)
 - lnDef (§20.1.4.1.20)
 - [done] pic (§20.1.2.2.30)
 - [done] sp (§20.1.2.2.33)
 - spDef (§20.1.4.1.27)
 - txDef (§20.1.4.1.28)

 Child elements:
 - effectRef (Effect Reference) §20.1.4.2.8
 - [done] fillRef (Fill Reference) §20.1.4.2.10
 - [done] fontRef (Font Reference) §20.1.4.1.17
 - [done] lnRef (Line Reference) §20.1.4.2.19

*/
//! @todo support all child elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_style()
{
    if (m_isLockedCanvas) {
        READ_PROLOGUE_IF_NS(a);
    } else {
        READ_PROLOGUE
    }

    while (!atEnd()) {
        readNext();
        debugMsooXml << *this;
        if (m_isLockedCanvas) {
            BREAK_IF_END_OF_WITH_NS(a, CURRENT_EL)
        } else {
            BREAK_IF_END_OF(CURRENT_EL)
        }
        if (isStartElement()) {
            TRY_READ_IF_NS(a, fillRef)
            ELSE_TRY_READ_IF_NS(a, lnRef)
            else if (qualifiedName() == "a:fontRef") {
                m_currentColor = QColor();
                m_referredFontName.clear();
                TRY_READ(fontRef)
                if (m_currentColor.isValid()) {
                    m_referredFont.addProperty("fo:color", m_currentColor.name());
                    m_currentColor = QColor();
                }
                if (!m_referredFontName.isEmpty()) {
                    m_referredFont.addProperty("fo:font-family", m_referredFontName);
                }
            }
            SKIP_UNKNOWN
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    if (m_isLockedCanvas) {
        READ_EPILOGUE_IF_NS(a)
    } else {
        READ_EPILOGUE
    }
}

#undef CURRENT_EL
#define CURRENT_EL spPr
//! spPr handler (Shape Properties)
//! ECMA-376, 19.3.1.44, p.2855; (PresentationML)
//! ECMA-376, 20.1.2.2.35, p. 3055 (DrawingML)
/*! This element specifies the visual shape properties that can be applied to a shape.
 These properties include the shape fill, outline, geometry, effects, and 3D orientation.

 Parent elements:
 ----------------
 PresentationML:
 - [done] cxnSp (§19.3.1.19)
 - [done] pic (§19.3.1.37)
 - [done] sp (§19.3.1.43)

 DrawingML:
 - [done] cxnSp (§20.1.2.2.10)
 - lnDef (§20.1.4.1.20)
 - [done] pic (§20.1.2.2.30)
 - [done] sp (§20.1.2.2.33)
 - spDef (§20.1.4.1.27)
 - txDef (§20.1.4.1.28)

 Child elements:
 - [done] blipFill (Picture Fill) §20.1.8.14
 - [done] custGeom (Custom Geometry) §20.1.9.8
 - effectDag (Effect Container) §20.1.8.25
 - [done] effectLst (Effect Container) §20.1.8.26
 - extLst (Extension List) §20.1.2.2.15
 - [done] gradFill (Gradient Fill) §20.1.8.33
 - grpFill (Group Fill) §20.1.8.35
 - [done] ln (Outline) §20.1.2.2.24
 - [done] noFill (No Fill) §20.1.8.44
 - pattFill (Pattern Fill) §20.1.8.47
 - [done] prstGeom (Preset geometry) §20.1.9.18
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
    if (m_isLockedCanvas) {
        READ_PROLOGUE_IF_NS(a);
    } else {
        READ_PROLOGUE
    }

    m_contentAvLstExists = false;
    m_customPath.clear();
    m_customEquations.clear();
    m_textareas.clear();

    while (!atEnd()) {
        readNext();
        debugMsooXml << *this;
        if (m_isLockedCanvas) {
            BREAK_IF_END_OF_WITH_NS(a, CURRENT_EL)
        } else {
            BREAK_IF_END_OF(CURRENT_EL)
        }
        if (isStartElement()) {
            if (qualifiedName() == QLatin1String("a:xfrm")) {
                TRY_READ(xfrm)
                m_xfrm_read = true;
            }
            else if (qualifiedName() == "a:custGeom") {
                TRY_READ(custGeom)
                m_contentType = "custom";
            }
            else if (qualifiedName() == QLatin1String("a:solidFill")) {
#ifdef PPTXXMLSLIDEREADER_CPP
                d->textBoxHasContent = true; // We count normal fill and gradient as content
#endif
                TRY_READ(solidFill)
                if (m_currentColor != QColor()) {
                    // We must set the color immediately, otherwise
                    // currentColor may be modified by eg. ln
                    m_currentDrawStyle->addProperty("draw:fill", QLatin1String("solid"));
                    m_currentDrawStyle->addProperty("draw:fill-color", m_currentColor.name());
                    m_currentColor = QColor();
                    if (m_currentAlpha > 0) {
                        m_currentDrawStyle->addProperty("draw:opacity", QString("%1%").arg(m_currentAlpha));
                    }
                }
            }
            else if (qualifiedName() == QLatin1String("a:ln")) {
                TRY_READ(ln)
            }
            else if (qualifiedName() == QLatin1String("a:noFill")) {
                m_currentDrawStyle->addProperty("draw:fill", "none");
            }
            else if (qualifiedName() == QLatin1String("a:prstGeom")) {
                TRY_READ(prstGeom)
            }
            else if (!m_ignoreLinkHref && name() == QLatin1String("blipFill")) {
                TRY_READ_IN_CONTEXT(blipFill)
                if (!m_xlinkHref.isEmpty()) {
                    KoGenStyle fillStyle = KoGenStyle(KoGenStyle::FillImageStyle);
                    fillStyle.addProperty("xlink:href", m_xlinkHref);
                    fillStyle.addProperty("xlink:type", "simple");
                    fillStyle.addProperty("xlink:actuate", "onLoad");
                    const QString imageName = mainStyles->insert(fillStyle);
                    m_currentDrawStyle->addProperty("draw:fill", "bitmap");
                    m_currentDrawStyle->addProperty("draw:fill-image-name", imageName);
                    m_xlinkHref.clear();
                }
            }
            else if (qualifiedName() == QLatin1String("a:effectLst")) {
                TRY_READ(effectLst)
            }
            else if (qualifiedName() == QLatin1String("a:gradFill")) {
#ifdef PPTXXMLSLIDEREADER_CPP
                d->textBoxHasContent = true;
#endif
                m_currentGradientStyle = KoGenStyle(KoGenStyle::LinearGradientStyle);
                TRY_READ(gradFill)
                m_currentDrawStyle->addProperty("draw:fill", "gradient");
                const QString gradName = mainStyles->insert(m_currentGradientStyle);
                m_currentDrawStyle->addProperty("draw:fill-gradient-name", gradName);
            }
            SKIP_UNKNOWN
//! @todo add ELSE_WRONG_FORMAT
        }
    }

#ifdef PPTXXMLSLIDEREADER_CPP
    saveCurrentGraphicStyles();
#endif

    if (m_isLockedCanvas) {
        READ_EPILOGUE_IF_NS(a)
    } else {
        READ_EPILOGUE
    }
}

// ================================================================
//                     Namespace "c"
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
    if (!r_id.isEmpty() && m_context->relationships) {
        const QString filepath = m_context->relationships->target(m_context->path, m_context->file, r_id);

        KoChart::Chart* chart = new KoChart::Chart;
        XlsxChartOdfWriter* chartWriter = new XlsxChartOdfWriter(chart, m_context->themes);
        bool hasStart = false, hasEnd = false;
#if defined(XLSXXMLDRAWINGREADER_CPP)
        chart->m_sheetName = m_context->worksheetReaderContext->worksheetName;
        chartWriter->setSheetReplacement(false);
        if (m_currentDrawingObject->m_positions.contains(XlsxDrawingObject::FromAnchor)) {
            XlsxDrawingObject::Position f = m_currentDrawingObject->m_positions[XlsxDrawingObject::FromAnchor];
            //chartWriter->m_x = columnWidth(f.m_col-1, 0 /*f.m_colOff*/);
            //chartWriter->m_y = rowHeight(f.m_row-1, 0 /*f.m_rowOff*/);
            chartWriter->m_x = EMU_TO_POINT(f.m_colOff);
            chartWriter->m_y = EMU_TO_POINT(f.m_rowOff);
            hasStart = true;
            if (m_currentDrawingObject->m_positions.contains(XlsxDrawingObject::ToAnchor)) {
                f = m_currentDrawingObject->m_positions[XlsxDrawingObject::ToAnchor];
                chartWriter->m_endCellAddress = m_currentDrawingObject->toCellAddress();
                //chartWriter->m_end_x = f.m_colOff;
                //chartWriter->m_end_y = f.m_rowOff;
                chartWriter->m_end_x = EMU_TO_POINT(f.m_colOff);
                chartWriter->m_end_y = EMU_TO_POINT(f.m_rowOff);
                hasEnd = true;
            }
        }
#else
        chartWriter->m_drawLayer = true;
#endif
        if (!hasStart) {
            chartWriter->m_x = EMU_TO_POINT(qMax((qint64)0, m_svgX));
            chartWriter->m_y = EMU_TO_POINT(qMax((qint64)0, m_svgY));
        }
        if (!hasEnd) {
            chartWriter->m_width = m_svgWidth > 0 ? EMU_TO_POINT(m_svgWidth) : 100;
            chartWriter->m_height = m_svgHeight > 0 ? EMU_TO_POINT(m_svgHeight) : 100;
        }

        KoStore* storeout = m_context->import->outputStore();
        QScopedPointer<XlsxXmlChartReaderContext> context(new XlsxXmlChartReaderContext(storeout, chartWriter));
        XlsxXmlChartReader reader(this);
        const KoFilter::ConversionStatus result = m_context->import->loadAndParseDocument(&reader, filepath, context.data());
        if (result != KoFilter::OK) {
            raiseError(reader.errorString());
            return result;
        }

#if defined(XLSXXMLDRAWINGREADER_CPP)
        m_currentDrawingObject->setChart(context.take());
#else
        chartWriter->saveIndex(body);
#endif
    }

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
    }

    READ_EPILOGUE
}

// ================================================================
//                     Namespace "dgm"
// ================================================================
#undef MSOOXML_CURRENT_NS
#define MSOOXML_CURRENT_NS "dgm"

#undef CURRENT_EL
#define CURRENT_EL relIds
//! relIds (Explicit Relationships to Diagram Parts)
/*! ECMA-376, 21.4, p.3936

  This element specifies the relationship IDs used to explicitly reference each
  of the four constituent parts of a DrawingML diagram:

  Diagram Colors (cs attribute)
  Diagram Data (dm attribute)
  Diagram Layout Definition (lo attribute)
  Diagram Style (qs attribute)

  ----------
  DrawingML - Diagrams
  ECMA-376, 21.4, p.3936

  A DrawingML diagram allows the definition of diagrams using DrawingML objects
  and constructs. This namespace defines the contents of a DrawingML diagram.
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_relIds()
{
    READ_PROLOGUE

    if (m_context->relationships) {
        const QXmlStreamAttributes attrs(attributes());
        TRY_READ_ATTR_WITH_NS(r, cs) // colors
        TRY_READ_ATTR_WITH_NS(r, dm) // data
        TRY_READ_ATTR_WITH_NS(r, lo) // layout
        TRY_READ_ATTR_WITH_NS(r, qs) // quickStyle
        while (!atEnd()) {
            readNext();
            BREAK_IF_END_OF(CURRENT_EL)
            if (isStartElement()) {
                TRY_READ_IF(spPr)
                ELSE_TRY_READ_IF(style)
            }
        }

/*         const QString colorsfile = r_cs.isEmpty() ? QString() : m_context->relationships->target(m_context->path, m_context->file, r_cs); */
        const QString datafile = r_dm.isEmpty() ? QString() : m_context->relationships->target(m_context->path, m_context->file, r_dm);
        const QString layoutfile = r_lo.isEmpty() ? QString() : m_context->relationships->target(m_context->path, m_context->file, r_lo);
/*         const QString quickstylefile = r_qs.isEmpty() ? QString() : m_context->relationships->target(m_context->path, m_context->file, r_qs); */
        QScopedPointer<MSOOXML::MsooXmlDiagramReaderContext> context(new MSOOXML::MsooXmlDiagramReaderContext(mainStyles));

        // first read the data-model
        MSOOXML::MsooXmlDiagramReader dataReader(this);
        const KoFilter::ConversionStatus dataReaderResult = m_context->import->loadAndParseDocument(&dataReader, datafile, context.data());
        if (dataReaderResult != KoFilter::OK) {
            raiseError(dataReader.errorString());
            return dataReaderResult;
        }

        // then read the layout definition
        MSOOXML::MsooXmlDiagramReader layoutReader(this);
        const KoFilter::ConversionStatus layoutReaderResult = m_context->import->loadAndParseDocument(&layoutReader, layoutfile, context.data());
        if (layoutReaderResult != KoFilter::OK) {
            raiseError(layoutReader.errorString());
            return layoutReaderResult;
        }

        if (context->shapeListSize() > 1) {
            m_context->graphicObjectIsGroup = true;
        }

        // and finally start the process that will produce the ODF
#if defined(XLSXXMLDRAWINGREADER_CPP)
        m_currentDrawingObject->setDiagram(context.take());
#else
        context->saveIndex(body, QRect(EMU_TO_CM(m_svgX), EMU_TO_CM(m_svgY), m_svgHeight > 0 ? EMU_TO_CM(m_svgWidth) : 100, m_svgHeight > 0 ? EMU_TO_CM(m_svgHeight) : 100));
#endif
    }

    READ_EPILOGUE
}

#undef MSOOXML_CURRENT_NS
#define MSOOXML_CURRENT_NS "lc"

// ================================================================
//                     Namespace "lc"
// ================================================================

#undef CURRENT_EL
#define CURRENT_EL lockedCanvas
//! lockedCanvas (Locked Canvas Container)
/*! ECMA-376, 20.3.2.1, p.3464

  The locked canvas element acts as a container for more advanced
  drawing objects.  The notion of a locked canvas comes from the fact
  that the generating application opening the file cannot create this
  object and can thus not perform edits either.  Thus the drawing
  object is locked from all UI adjustments that would normally take
  place.

  Child Elements
  - [done] cxnSp (Connection Shape)
  - extLst (Extension List)
  - graphicFrame (Graphic Frame)
  - [done] grpSp (Group shape)
  - [done] grpSpPr (Visual Group Shape Properties)
  - nvGrpSpPr (Non-Visual Properties for a Group Shape)
  - [done] pic (Picture)
  - [done] sp (Shape)
  - [done] txSp (Text Shape)
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_lockedCanvas()
{
    // NOTE: Child elements have a client specific namespace defined
    // by the DRAWINGML_PIC_NS macro in case of other parent elements.
    // In case of lockedCanvas, child elements have namespace "a".

    READ_PROLOGUE
    m_isLockedCanvas = true;
    m_context->graphicObjectIsGroup = true;

    while (!atEnd()) {
        readNext();
        debugMsooXml << *this;
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(cxnSp)
            ELSE_TRY_READ_IF_NS(a, grpSp)
            ELSE_TRY_READ_IF_NS(a, grpSpPr)
            ELSE_TRY_READ_IF_NS(a, pic)
            ELSE_TRY_READ_IF_NS(a, sp)
            // ELSE_TRY_READ_IF_NS(a, graphicFrame)
            ELSE_TRY_READ_IF_NS(a, txSp)
            SKIP_UNKNOWN
	}
    }

    m_isLockedCanvas = false;
    READ_EPILOGUE
}

// ================================================================
//                     Namespace "a"
// ================================================================
#undef MSOOXML_CURRENT_NS
#define MSOOXML_CURRENT_NS "a"

#undef CURRENT_EL
#define CURRENT_EL lnRef
//! fillRef handler (Line reference)
/*
 Parent elements:
 - [done] style (§21.3.2.24);
 - [done] style (§21.4.2.28);
 - [done] style (§20.1.2.2.37);
 - [done] style (§20.5.2.31);
 - [done] style (§19.3.1.46);
 - tblBg (§20.1.4.2.25);
 - tcStyle (§20.1.4.2.29)

 Child elements:
 - [done] hslClr (Hue, Saturation, Luminance Color Model) §20.1.2.3.13
 - [done] prstClr (Preset Color) §20.1.2.3.22
 - [done] schemeClr (Scheme Color) §20.1.2.3.29
 - [done] scrgbClr (RGB Color Model - Percentage Variant) §20.1.2.3.30
 - [done] srgbClr (RGB Color Model - Hex Variant) §20.1.2.3.32
 - [done] sysClr (System Color) §20.1.2.3.33
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_lnRef()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR_WITHOUT_NS(idx)

    const QList<KoGenStyle> *lst = &m_context->themes->formatScheme.lnStyleLst;
    const KoGenStyle *lnStyle = 0;

    if (!idx.isEmpty() && !lst->empty()) {

        int index = idx.toInt();

        if (index >= lst->size()) {
            index = lst->size() - 1;
        }
        lnStyle = &lst->at(index);
    }

    while (!atEnd()) {
        readNext();
        debugMsooXml << *this;
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(schemeClr)
            ELSE_TRY_READ_IF(srgbClr)
            ELSE_TRY_READ_IF(sysClr)
            ELSE_TRY_READ_IF(scrgbClr)
            ELSE_TRY_READ_IF(prstClr)
            ELSE_TRY_READ_IF(hslClr)
            ELSE_WRONG_FORMAT
        }
    }

    // TODO: Do the following before reading of the ln element.
    // copyPropertiesFromStyle(lnStyle, m_currentDrawStyle, KoGenStyle::GraphicType);

    if (m_currentColor.isValid() && m_currentDrawStyle->property("svg:stroke-color").isEmpty()) {
        m_currentDrawStyle->addProperty("svg:stroke-color", m_currentColor.name());
    }

    if (lnStyle) {
        QString prop;
        if (m_currentDrawStyle->property("draw:stroke").isEmpty()) {
            prop = lnStyle->property("draw:stroke");
            if (!prop.isEmpty()) {
                m_currentDrawStyle->addProperty("draw:stroke", prop);
            } else {
                // MSOOXML default
                m_currentDrawStyle->addProperty("draw:stroke", "none");
            }
        }
        if (m_currentDrawStyle->property("svg:stroke-width").isEmpty()) {
            prop = lnStyle->property("svg:stroke-width");
            if (!prop.isEmpty()) {
                m_currentDrawStyle->addProperty("svg:stroke-width", prop);
            } else {
                // MSOOXML default
                m_currentDrawStyle->addPropertyPt("svg:stroke-width", 0);
            }
        }
        if (m_currentDrawStyle->property("svg:stroke-color").isEmpty()) {
            prop = lnStyle->property("svg:stroke-color");
            if (!prop.isEmpty()) {
                m_currentDrawStyle->addProperty("svg:stroke-color", prop);
            }
        }
        if (m_currentDrawStyle->property("draw:stroke-linejoin").isEmpty()) {
            prop = lnStyle->property("draw:stroke-linejoin");
            if (!prop.isEmpty()) {
                m_currentDrawStyle->addProperty("draw:stroke-linejoin", prop);
            } else {
                // MSOOXML default
                m_currentDrawStyle->addProperty("draw:stroke-linejoin", "round");
            }
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL masterClrMapping
//! masterClrMapping handler (Master Color Mapping)
/* This element is a part of a choice for which color mapping is used within
   the document.  If this element is specified, then we specifically use the
   color mapping defined in the master.

 Parent elements:
 - [done] clrMapOvr (§19.3.1.7)
 */
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_masterClrMapping()
{
    READ_PROLOGUE

    // TODO: Add filter specific stuff.

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL overrideClrMapping
//! overrideClrMapping handler (Override Color Mapping)
/* This element provides an override for the color mapping in a document. When
   defined, this color mapping is used in place of the already defined color
   mapping, or master color mapping. This color mapping is defined in the same
   manner as the other mappings within this document.

 Parent elements:
 - [done] clrMapOvr (§19.3.1.7)
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_overrideClrMapping()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

#ifdef PPTXXMLSLIDEREADER_CPP
    QMap<QString, QString> colorMapBkp;
    if ((m_context->type == SlideLayout) || (m_context->type == Slide)) {
        colorMapBkp = m_context->colorMap;
    }
#endif

    int index = 0;
    while (index < attrs.size()) {
        const QString handledAttr = attrs.at(index).name().toString();
        const QString attrValue = attrs.value(handledAttr).toString();
#ifdef PPTXXMLSLIDEREADER_CPP
        m_context->colorMap[handledAttr] = attrValue;
#endif
        ++index;
    }

    // FIXME: PPTX: Update styles prepared while processing the p:txStyles
    // element (Slide Master Text Styles).
    //
    // NOTE: Workaround!  Inform the pptx filter that the color mapping
    // changed compared to slideMaster.  Theme specific default colors should
    // be used until we get correct style:use-window-font-color support.
#ifdef PPTXXMLSLIDEREADER_CPP
    if (m_context->type == SlideLayout) {
        if (m_context->colorMap != colorMapBkp) {
            m_context->slideLayoutProperties->overrideClrMapping = true;
            m_context->slideLayoutProperties->colorMap = m_context->colorMap;
        }
    }
    // NOTE: Workaround!  Inform the pptx filter that the color mapping
    // changed compared to slideMaster.  Theme specific default colors should
    // be used.
    if (m_context->type == Slide) {
        if (m_context->colorMap != colorMapBkp) {
            m_context->overrideClrMapping = true;
        }
    }
#endif

    while (!atEnd()) {
        readNext();
        debugMsooXml << *this;
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL p
//! p handler (Text Paragraphs) ECMA-376, DrawingML 21.1.2.2.6, p. 3587.
//!   This element specifies the presence of a paragraph of text within the containing text body.
/*!
 Parent elements:
 - rich (§21.2.2.156)
 - txBody (§21.3.2.26)
 - txBody (§20.1.2.2.40)
 - txBody (§20.5.2.34)
 - [done] txBody (§19.3.1.51) - PML
 - txPr (§21.2.2.216)

 Child elements:
 - [done] br (Text Line Break) §21.1.2.2.1
 - [done] endParaRPr (End Paragraph Run Properties) §21.1.2.2.3
 - [done] fld (Text Field) §21.1.2.2.4
 - [done] pPr (Text Paragraph Properties) §21.1.2.2.7
 - [done] r (Text Run) §21.1.2.3.8
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_DrawingML_p()
{
    READ_PROLOGUE2(DrawingML_p)

    // Using TEXT_FONTSIZE_MAX, because the font-size information might not be
    // provided, which would break the margin-top/margin-bottom calculation.
    // NOTE: This might not be the correct approach but it produces the best
    // results at the moment.
    m_minParaFontPt = TEXT_FONTSIZE_MAX;
    m_maxParaFontPt = TEXT_FONTSIZE_MIN;

    m_read_DrawingML_p_args = 0;
    m_paragraphStyleNameWritten = false;
    m_listStylePropertiesAltered = false;

    m_currentCombinedBulletProperties.clear();

#ifdef PPTXXMLSLIDEREADER_CPP
    m_currentListLevel = 1; // By default we're in the first level
    inheritListStyles();
#else
    // TODO: MS Word: There's a different positioning logic for a list inside
    // of a textbox compared to a list in a document body.
/*     m_prevListLevel = m_currentListLevel = 0; */
    m_currentListLevel = 0;
#endif

    MSOOXML::Utils::MSOOXMLFilter currentFilter = MSOOXML::Utils::XlsxFilter;
#ifdef PPTXXMLSLIDEREADER_CPP
    currentFilter = MSOOXML::Utils::PptxFilter;
#elif defined  DOCXXMLDOCREADER_CPP
    currentFilter = MSOOXML::Utils::DocxFilter;
#endif

    QString fontSize;
    QString bulletColor;
    QString listStyleName;

    // Creating a list out of what we have, pPr MAY overwrite the list style
    m_currentListStyle = KoGenStyle(KoGenStyle::ListAutoStyle);
/*     QMapIterator<int, MSOOXML::Utils::ParagraphBulletProperties> i(m_currentCombinedBulletProperties); */
    QMutableMapIterator<int, MSOOXML::Utils::ParagraphBulletProperties> i(m_currentCombinedBulletProperties);
    int index = 0;
    while (i.hasNext()) {
        index++;
        i.next();
        m_currentListStyle.addChildElement(QString("list-style-properties%1").arg(index),
            i.value().convertToListProperties(*mainStyles, currentFilter));
    }

    MSOOXML::Utils::XmlWriteBuffer textPBuf;

    body = textPBuf.setWriter(body);
    m_currentParagraphStyle = KoGenStyle(KoGenStyle::ParagraphAutoStyle, "paragraph");

#ifdef PPTXXMLSLIDEREADER_CPP
    m_currentParagraphStyle.addProperty("fo:line-height", "100%" );
#endif
    bool pprRead = false;
    bool rRead = false;
    bool brLastElement = false;
    QString endParaRPrFontSize;

    while (!atEnd()) {
        readNext();
        debugMsooXml << "isStartElement:" << isStartElement();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
// CASE #400.1
            if (QUALIFIED_NAME_IS(pPr)) {
                TRY_READ(DrawingML_pPr)
                pprRead = true;
            }
            else if (QUALIFIED_NAME_IS(br)) {
                TRY_READ(DrawingML_br)
                brLastElement = true;
            }
// CASE #400.2
//! @todo add more conditions testing the parent
            else if (QUALIFIED_NAME_IS(r)) {
                rRead = true;
#ifdef PPTXXMLSLIDEREADER_CPP
                d->textBoxHasContent = true;
#endif
                TRY_READ(DrawingML_r)
                if (fontSize.isEmpty()) {
                    fontSize = m_currentTextStyle.property("fo:font-size");
                }
                if (bulletColor.isEmpty()) {
                    bulletColor = m_currentTextStyle.property("fo:color");
                }
                brLastElement = false;
            }
            else if (QUALIFIED_NAME_IS(fld)) {
                rRead = true;
                TRY_READ(fld)
                brLastElement = false;
            }
            else if (QUALIFIED_NAME_IS(endParaRPr)) {
                m_currentTextStyleProperties = new KoCharacterStyle();
                m_currentTextStyle = KoGenStyle(KoGenStyle::TextAutoStyle, "text");

#ifdef PPTXXMLSLIDEREADER_CPP
                if (m_context->type == SlideMaster || m_context->type == NotesMaster) {
                    m_currentTextStyle.setAutoStyleInStylesDotXml(true);
                }
#elif defined DOCXXMLDOCREADER_CPP
                if (m_moveToStylesXml) {
                    m_currentTextStyle.setAutoStyleInStylesDotXml(true);
                }
#endif
#ifdef PPTXXMLSLIDEREADER_CPP
                if (!m_insideTable) {
                    inheritTextStyle(m_currentTextStyle);
                }
#endif
                TRY_READ(endParaRPr)
                m_currentTextStyleProperties->saveOdf(m_currentTextStyle);
                delete m_currentTextStyleProperties;
                m_currentTextStyleProperties = 0;

                if (brLastElement || !rRead) {
                    body->startElement("text:span");
                    body->addAttribute("text:style-name", mainStyles->insert(m_currentTextStyle));
                    body->endElement(); //text:span
                }
                endParaRPrFontSize = m_currentTextStyle.property("fo:font-size");
            }
            ELSE_WRONG_FORMAT
        }
    }

#ifdef PPTXXMLSLIDEREADER_CPP
    if (!pprRead) {
        inheritParagraphStyle(m_currentParagraphStyle);
        m_currentBulletProperties = m_currentCombinedBulletProperties[m_currentListLevel];
    }
    if (m_currentParagraphStyle.property("fo:line-height").endsWith('%')) {
        m_currentParagraphStyle.addProperty("style:font-independent-line-spacing", "true");
    }
#else
    Q_UNUSED(pprRead);
#endif

    //---------------------------------------------
    // Empty lines
    //---------------------------------------------
    // The endParaRPr element specifies the text run properties that are to be
    // used if another run is inserted after the last run specified.
    if (!rRead) {
        QString fontSize = endParaRPrFontSize;
        if (!fontSize.isEmpty() && fontSize.endsWith(QLatin1String("pt"))) {
            fontSize.chop(2);
            qreal realSize = fontSize.toDouble();
            if (realSize > m_maxParaFontPt) {
                m_maxParaFontPt = realSize;
            }
            if (realSize < m_minParaFontPt) {
                m_minParaFontPt = realSize;
            }
        }
    }

    //---------------------------------------------
    // Prepare for the List Style
    //---------------------------------------------
#ifdef PPTXXMLSLIDEREADER_CPP
    // MS PowerPoint treats each paragraph as a list-item.
    m_listStylePropertiesAltered = true;
#else
    if (m_currentListLevel == 0) {
	m_listStylePropertiesAltered = false;
    }
#endif

    //required to set size of the picture bullet properly
    if (m_listStylePropertiesAltered && m_currentBulletProperties.bulletSizePt() == "UNUSED") {
        if (!fontSize.isEmpty() && fontSize.endsWith(QLatin1String("pt"))) {
            fontSize.chop(2);
            qreal bulletSize = fontSize.toDouble();

            if (m_currentBulletProperties.bulletRelativeSize() != "UNUSED") {
                bulletSize = (bulletSize * m_currentBulletProperties.bulletRelativeSize().toDouble()) / 100;
            } else {
                m_currentBulletProperties.setBulletRelativeSize(100);
            }
            m_currentBulletProperties.setBulletSizePt(bulletSize);
        }
    }

    //---------------------------------------------
    // List Style
    //---------------------------------------------
    if (m_listStylePropertiesAltered) {
        m_currentListStyle = KoGenStyle(KoGenStyle::ListAutoStyle);
#ifdef PPTXXMLSLIDEREADER_CPP
        if (m_context->type == SlideMaster || m_context->type == NotesMaster) {
            m_currentListStyle.setAutoStyleInStylesDotXml(true);
        }
#elif defined DOCXXMLDOCREADER_CPP
        if (m_moveToStylesXml) {
            m_currentListStyle.setAutoStyleInStylesDotXml(true);
        }
#endif
        m_currentBulletProperties.m_level = m_currentListLevel;
        m_currentListStyle.addChildElement("list-style-properties",
            m_currentBulletProperties.convertToListProperties(*mainStyles, currentFilter));
        listStyleName = mainStyles->insert(m_currentListStyle);
        Q_ASSERT(!listStyleName.isEmpty());

        if (listStyleName != m_prevListStyleName) {
            m_prevListStyleName = listStyleName;
        } else {
            m_listStylePropertiesAltered = false;
        }
    }
    //---------------------------------------------
    // Update Automatic Numbering info
    //---------------------------------------------
    if (m_currentBulletProperties.m_type != MSOOXML::Utils::ParagraphBulletProperties::NumberType) {
        QList<quint16> levels = m_continueListNumbering.keys();
        for (quint16 i = 0; i < levels.size(); i++) {
            if (levels[i] >= m_currentListLevel) {
                m_continueListNumbering.remove(levels[i]);
                m_lvlXmlIdMap.remove(levels[i]);
            }
        }
    }
    if (m_currentBulletProperties.m_type == MSOOXML::Utils::ParagraphBulletProperties::NumberType) {
        if (m_prevListLevel > m_currentListLevel) {
            QList<quint16> levels = m_continueListNumbering.keys();
            for (quint16 i = 0; i < levels.size(); i++) {
                if (levels[i] > m_currentListLevel) {
                    m_continueListNumbering.remove(levels[i]);
                    m_lvlXmlIdMap.remove(levels[i]);
                }
            }
        }
    }

    //---------------------------------------------
    // Prepare for the List
    //---------------------------------------------

    // Empty paragraph is NOT considered to be a list-item at the moment.
    // Prevent stage of displaying a bullet in front of it.
#ifdef PPTXXMLSLIDEREADER_CPP
    if (!rRead) {
        m_listStylePropertiesAltered = true;
        m_prevListStyleName.clear();
        m_currentListLevel = 0;
    }
#endif

    body = textPBuf.originalWriter();

    // End the previous list in case a new list-style is going to be applied.
    if (m_listStylePropertiesAltered) {
        if (m_prevListLevel > 0) {
            body->endElement(); //text:list
            for (; m_prevListLevel > 1; --m_prevListLevel) {
                body->endElement(); //text:list-item
                body->endElement(); //text:list
            }
            m_prevListLevel = 0;
        }
    }

    //---------------------------------------------
    // Start the List/List-Item
    //---------------------------------------------
    if (m_currentListLevel > 0 || m_prevListLevel > 0) {
/* #ifdef PPTXXMLSLIDEREADER_CPP */
        if (m_listStylePropertiesAltered) {
            Q_ASSERT(m_prevListLevel == 0);

            body->startElement("text:list");
            body->addAttribute("text:style-name", listStyleName);
            m_currentParagraphStyle.addAttribute("style:list-style-name", listStyleName);

            //continue numbering if applicable
            if (m_currentBulletProperties.m_type == MSOOXML::Utils::ParagraphBulletProperties::NumberType) {

                QString xmlId = QString("lvl%1").arg(m_currentListLevel);
                xmlId.append(QString("_%1").arg(qrand()));
                body->addAttribute("xml:id", xmlId);

                if (m_continueListNumbering.contains(m_currentListLevel) &&
                    m_continueListNumbering[m_currentListLevel]) {
                    body->addAttribute("text:continue-list", m_lvlXmlIdMap[m_currentListLevel]);
                }
                m_lvlXmlIdMap[m_currentListLevel] = xmlId;
            }
            body->startElement("text:list-item");
            for (int i = 1; i < m_currentListLevel; i++) {
                body->startElement("text:list");
                body->startElement("text:list-item");
            }

        } else if (m_prevListLevel < m_currentListLevel) {
            if (m_prevListLevel > 0) {
                body->startElement("text:list-item");
            }
            for (int i = m_prevListLevel; i < m_currentListLevel; i++) {
                body->startElement("text:list");
                body->startElement("text:list-item");
            }
        } else if (m_prevListLevel > m_currentListLevel) {
            body->endElement(); //text:list
            for (int i = m_prevListLevel - 1; i > m_currentListLevel; i--) {
                body->endElement(); //text:list-item
                body->endElement(); //text:list
            }
            //starting our own stuff for this level
            if (m_currentListLevel > 0) {
                body->endElement(); //text:list-item
                body->startElement("text:list-item");
            }
        } else { // m_prevListLevel==m_currentListLevel
            body->startElement("text:list-item");
        }
        //restart numbering if applicable
        if (m_currentBulletProperties.m_type == MSOOXML::Utils::ParagraphBulletProperties::NumberType) {
            if (m_continueListNumbering.contains(m_currentListLevel) &&
                (m_continueListNumbering[m_currentListLevel] == false)) {
                body->addAttribute("text:start-value", m_currentBulletProperties.startValue());
            }
        }
/* #else */
/*         for (int i = 0; i < m_currentListLevel; ++i) { */
/*             body->startElement("text:list"); */
/*             // TODO:, should most likely add the name of the current list style */
/*             body->startElement("text:list-item"); */
/*         } */
/* #endif */
        if (m_currentBulletProperties.m_type == MSOOXML::Utils::ParagraphBulletProperties::NumberType) {
            m_continueListNumbering[m_currentListLevel] = true;
        }
    }
    //---------------------------------------------
    // Paragraph Style
    //---------------------------------------------
#ifdef PPTXXMLSLIDEREADER_CPP
    if (m_context->type == SlideMaster || m_context->type == NotesMaster) {
        m_currentParagraphStyle.setAutoStyleInStylesDotXml(true);
    }
#elif defined DOCXXMLDOCREADER_CPP
    if (m_moveToStylesXml) {
        m_currentParagraphStyle.setAutoStyleInStylesDotXml(true);
    }
#endif
    // Position of the list-item defined by fo:margin-left and fo:text-indent
    // in the style:list-level-properties element.  In ODF the paragraph style
    // overrides the list style.
    if (m_currentListLevel > 0) {
        m_currentParagraphStyle.removeProperty("fo:margin-left");
        m_currentParagraphStyle.removeProperty("fo:text-indent");
    }

    // Margins (paragraph spacing) in OOXML MIGHT be defined as percentage.
    // In ODF the value of margin-top/margin-bottom MAY be a percentage that
    // refers to the corresponding margin of a parent style.  Let's convert
    // the percentage value into points to keep it simple.
    QString spcBef = m_currentParagraphStyle.property("fo:margin-top");
    if (spcBef.contains('%')) {
        spcBef.remove('%');
        qreal percentage = spcBef.toDouble();
        qreal margin = 0;
#ifdef PPTXXMLSLIDEREADER_CPP
        margin = processParagraphSpacing(percentage, m_minParaFontPt);
#else
        margin = (percentage * m_maxParaFontPt) / 100.0;
#endif
        m_currentParagraphStyle.addPropertyPt("fo:margin-top", margin);
    }
    QString spcAft = m_currentParagraphStyle.property("fo:margin-bottom");
    if (spcAft.contains('%')) {
        spcAft.remove('%');
        qreal percentage = spcAft.toDouble();
        qreal margin = 0;
#ifdef PPTXXMLSLIDEREADER_CPP
        margin = processParagraphSpacing(percentage, m_minParaFontPt);
#else
        margin = (percentage * m_maxParaFontPt) / 100.0;
#endif
        m_currentParagraphStyle.addPropertyPt("fo:margin-bottom", margin);
    }
    QString currentParagraphStyleName(mainStyles->insert(m_currentParagraphStyle));

    //---------------------------------------------
    // Start Paragraph
    //---------------------------------------------
    body->startElement("text:p", false);
    body->addAttribute("text:style-name", currentParagraphStyleName);

    (void)textPBuf.releaseWriter();
    body->endElement(); //text:p

    //---------------------------------------------
    // End List/List-Item
    //---------------------------------------------
/* #ifdef PPTXXMLSLIDEREADER_CPP */
    if (m_currentListLevel > 0) {
        body->endElement(); //text:list-item
    }
    m_prevListLevel = m_currentListLevel;
/* #else */
/*     // A new list is created for each paragraph rather then nesting the lists */
/*     // cause both word and excel filter still need to be adjusted to properly */
/*     // handle nested lists. */
/*     for(int i = 0; i < m_currentListLevel; ++i) { */
/*         body->endElement(); // text:list-item */
/*         body->endElement(); // text:list */
/*     } */
/*     m_prevListLevel = m_currentListLevel = 0; */
/* #endif */
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

    m_hyperLink = false;

    MSOOXML::Utils::XmlWriteBuffer rBuf;
    body = rBuf.setWriter(body);

    m_currentTextStyleProperties = new KoCharacterStyle();
    m_currentTextStyle = KoGenStyle(KoGenStyle::TextAutoStyle, "text");
#ifdef PPTXXMLSLIDEREADER_CPP
    if (m_context->type == SlideMaster || m_context->type == NotesMaster) {
        m_currentTextStyle.setAutoStyleInStylesDotXml(true);
    }
#elif defined DOCXXMLDOCREADER_CPP
    if (m_moveToStylesXml) {
        m_currentTextStyle.setAutoStyleInStylesDotXml(true);
    }
#endif

#ifdef PPTXXMLSLIDEREADER_CPP
    // FIXME: There's no reason on my mind to NOT inherit the text style. (uzak)
    if (!m_insideTable) {
        inheritTextStyle(m_currentTextStyle);
    }
#endif

    KoGenStyle::copyPropertiesFromStyle(m_referredFont, m_currentTextStyle, KoGenStyle::TextType);

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            if (QUALIFIED_NAME_IS(rPr)) {
                TRY_READ(DrawingML_rPr)
            }
            else if (QUALIFIED_NAME_IS(t)) {
                TRY_READ_WITH_ARGS(t, true;)
            }
            ELSE_WRONG_FORMAT
        }
    }

    m_currentTextStyleProperties->saveOdf(m_currentTextStyle);
    delete m_currentTextStyleProperties;
    m_currentTextStyleProperties = 0;

    // elements

    body = rBuf.originalWriter();

    if (m_hyperLink) {
        body->startElement("text:a", false);
        body->addAttribute("xlink:type", "simple");
        body->addAttribute("xlink:href", QUrl(m_hyperLinkTarget).toEncoded());
    }

    QString fontSize = m_currentTextStyle.property("fo:font-size");

#ifdef PPTXXMLSLIDEREADER_CPP
    if (fontSize.isEmpty()) {
        m_currentTextStyle.addPropertyPt("fo:font-size", TEXT_FONTSIZE_DEFAULT);
        fontSize = QString("%1").arg(TEXT_FONTSIZE_DEFAULT);
    }
#endif
    if (!fontSize.isEmpty()) {
        fontSize.remove("pt");
        qreal realSize = fontSize.toDouble();
        if (realSize > m_maxParaFontPt) {
            m_maxParaFontPt = realSize;
        }
        if (realSize < m_minParaFontPt) {
            m_minParaFontPt = realSize;
        }
    }

    const QString currentTextStyleName(mainStyles->insert(m_currentTextStyle));
    body->startElement("text:span", false);
    body->addAttribute("text:style-name", currentTextStyleName);

    (void)rBuf.releaseWriter();

    body->endElement(); //text:span
    if (m_hyperLink) {
        body->endElement(); // text:a
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL br
//! br (Text Line Break)
//! ECMA-376, 21.1.2.2.1, p.3569
/*
 This element specifies the existence of a vertical line break between two runs
 of text within a paragraph.  In addition to specifying a vertical space
 between two runs of text, this element can also have run properties specified
 via the rPr child element.  This sets the formatting of text for the line
 break so that if text is later inserted there that a new run can be generated
 with the correct formatting.

 Parent elements:
 - [done] p (§21.1.2.2.6)

 Child elements:
 - [done] rPr (Text Run Properties) §21.1.2.3.9
 */
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_DrawingML_br()
{
    READ_PROLOGUE

    m_currentTextStyleProperties = new KoCharacterStyle();
    m_currentTextStyle = KoGenStyle(KoGenStyle::TextAutoStyle, "text");

#ifdef PPTXXMLSLIDEREADER_CPP
    if (m_context->type == SlideMaster || m_context->type == NotesMaster) {
        m_currentTextStyle.setAutoStyleInStylesDotXml(true);
    }
#elif defined DOCXXMLDOCREADER_CPP
    if (m_moveToStylesXml) {
        m_currentTextStyle.setAutoStyleInStylesDotXml(true);
    }
#endif
#ifdef PPTXXMLSLIDEREADER_CPP
    if (!m_insideTable) {
        inheritTextStyle(m_currentTextStyle);
    }
#endif

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            if (QUALIFIED_NAME_IS(rPr)) {
                TRY_READ(DrawingML_rPr)
            }
            ELSE_WRONG_FORMAT
        }
    }
    m_currentTextStyleProperties->saveOdf(m_currentTextStyle);

    // NOTE: workaround: Remove selected properties to get text:line-break
    // applied properly during layout.  I didn't check the layout part (uzak).

    // If fo:text-transform is present then text:line-break is not applied.
    m_currentTextStyle.removeProperty("fo:text-transform");

    // The underline is applied until the end of the line during layout when
    // the text style of text:line-break equals the text style of the chunk.
    m_currentTextStyle.removeProperty("style:text-underline-style");
    m_currentTextStyle.removeProperty("style:text-underline-width");

    body->startElement("text:span", false);
    body->addAttribute("text:style-name", mainStyles->insert(m_currentTextStyle));
    body->startElement("text:line-break");
    body->endElement(); //text:line-break
    body->endElement(); //text:span

    delete m_currentTextStyleProperties;
    m_currentTextStyleProperties = 0;

    READ_EPILOGUE
}


#undef CURRENT_EL
#define CURRENT_EL endParaRPr
//! endParaRPr handler (End Paragraph Run Properties)
/*
 Parent elements:
 - [done] p (§21.1.2.2.6)

 Child elements:
 - blipFill (Picture Fill) §20.1.8.14
 - cs (Complex Script Font) §21.1.2.3.1
 - ea (East Asian Font) §21.1.2.3.3
 - effectDag (Effect Container) §20.1.8.25
 - effectLst (Effect Container) §20.1.8.26
 - extLst (Extension List) §20.1.2.2.15
 - [done] gradFill (Gradient Fill) §20.1.8.33
 - grpFill (Group Fill) §20.1.8.35
 - [done] highlight (Highlight Color) §21.1.2.3.4
 - [done] hlinkClick (Click Hyperlink) §21.1.2.3.5
 - hlinkMouseOver (Mouse-Over Hyperlink) §21.1.2.3.6
 - [done] latin (Latin Font) §21.1.2.3.7
 - ln (Outline) §20.1.2.2.24
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
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_endParaRPr()
{
    READ_PROLOGUE

    m_hyperLink = false;

    const QXmlStreamAttributes attrs(attributes());

    m_currentColor = QColor();

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(latin)
            ELSE_TRY_READ_IF(solidFill)
            else if (QUALIFIED_NAME_IS(highlight)) {
                TRY_READ(DrawingML_highlight)
            }
            else if (name() == "gradFill") {
                TRY_READ(gradFillRpr)
            }
            else if (name() == "noFill") {
                m_currentTextStyleProperties->setTextOutline(QPen(Qt::SolidLine));
            }
            ELSE_TRY_READ_IF(hlinkClick)
            SKIP_UNKNOWN
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    if (m_currentColor.isValid()) {
        m_currentTextStyle.addProperty("fo:color", m_currentColor.name());
        m_currentColor = QColor();
    }

    handleRprAttributes(attrs);

    READ_EPILOGUE
}

void MSOOXML_CURRENT_CLASS::handleRprAttributes(const QXmlStreamAttributes& attrs)
{
    // DrawingML: b, i, strike, u attributes:
    if (attrs.hasAttribute("b")) {
        m_currentTextStyleProperties->setFontWeight(
            MSOOXML::Utils::convertBooleanAttr(attrs.value("b").toString()) ? QFont::Bold : QFont::Normal);
    }
    if (attrs.hasAttribute("i")) {
        m_currentTextStyleProperties->setFontItalic(
            MSOOXML::Utils::convertBooleanAttr(attrs.value("i").toString()));
    }

    TRY_READ_ATTR_WITHOUT_NS(cap);
    if (!cap.isEmpty()) {
        if (cap == QLatin1String("small")) {
            m_currentTextStyle.addProperty("fo:font-variant", "small-caps");
        }
        else if (cap == QLatin1String("all")) {
            m_currentTextStyle.addProperty("fo:text-transform", "uppercase");
        }
    }
    TRY_READ_ATTR_WITHOUT_NS(spc)
    if (!spc.isEmpty()) {
        int spcInt = spc.toInt();
        m_currentTextStyle.addPropertyPt("fo:letter-spacing", qreal(spcInt) / 100.0);
    }

    TRY_READ_ATTR_WITHOUT_NS(sz)
    if (!sz.isEmpty()) {
        int szInt = sz.toInt();
        m_currentTextStyleProperties->setFontPointSize(qreal(szInt) / 100.0);
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
        int baselineInt = baseline.toInt();
        if (baselineInt > 0) {
            m_currentTextStyleProperties->setVerticalAlignment(QTextCharFormat::AlignSuperScript);
        }
        else if (baselineInt < 0) {
            m_currentTextStyleProperties->setVerticalAlignment(QTextCharFormat::AlignSubScript);
        }
    }

    TRY_READ_ATTR_WITHOUT_NS(u)
    if (!u.isEmpty()) {
        MSOOXML::Utils::setupUnderLineStyle(u, m_currentTextStyleProperties);
    }
}

#undef CURRENT_EL
#define CURRENT_EL rPr
//! rPr handler (Text Run Properties) DrawingML ECMA-376, 21.1.2.3.9, p.3624.
//! This element contains all run level text properties for the text runs within a containing paragraph.
/*!
 Parent elements:
 - br (§21.1.2.2.1)
 - [done] fld (§21.1.2.2.4)
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
 - [done] gradFill (Gradient Fill) §20.1.8.33
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

    m_hyperLink = false;

    const QXmlStreamAttributes attrs(attributes());

    m_currentColor = QColor();

    // Read child elements
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(latin)
            //ELSE_TRY_READ_IF_IN_CONTEXT(blipFill)
            ELSE_TRY_READ_IF(solidFill)
            else if (name() == "gradFill") {
                TRY_READ(gradFillRpr)
            }
            else if (name() == "noFill") {
                m_currentTextStyleProperties->setTextOutline(QPen(Qt::SolidLine));
            }
            else if (QUALIFIED_NAME_IS(highlight)) {
                TRY_READ(DrawingML_highlight)
            }
            //ELSE_TRY_READ_IF(ln) // Disabled as this is not supported by odf
            ELSE_TRY_READ_IF(hlinkClick)
            SKIP_UNKNOWN
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    if (m_currentColor.isValid()) {
        m_currentTextStyle.addProperty("fo:color", m_currentColor.name());
        m_currentColor = QColor();
    }

    handleRprAttributes(attrs);

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL pPr
//! pPr handler (Text Paragraph Properties) 21.1.2.2.7, p.3588.
/*!
 Parent elements:
  - [done] fld (§21.1.2.2.4)
  - [done] p (§21.1.2.2.6)

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
  - [done] buAutoNum (Auto-Numbered Bullet) §21.1.2.4.1
  - [done] buBlip (Picture Bullet) §21.1.2.4.2
  - [done] buChar (Character Bullet) §21.1.2.4.3
  - [done] buClr (Color Specified) §21.1.2.4.4
  - [done] buClrTx (Follow Text) §21.1.2.4.5
  - [done] buFont (Specified) §21.1.2.4.6
  - buFontTx (Follow text) §21.1.2.4.7
  - [done] buNone (No Bullet) §21.1.2.4.8
  - [done] buSzPct (Bullet Size Percentage) §21.1.2.4.9
  - [done] buSzPts (Bullet Size Points) §21.1.2.4.10
  - [done] buSzTx (Bullet Size Follows Text) §21.1.2.4.11
  - [done] defRPr (Default Text Run Properties) §21.1.2.3.2
  - extLst (Extension List) §20.1.2.2.15
  - [done] lnSpc (Line Spacing) §21.1.2.2.5
  - [done] spcAft (Space After) §21.1.2.2.9
  - [done] spcBef (Space Before) §21.1.2.2.10
  - tabLst (Tab List) §21.1.2.2.14
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_DrawingML_pPr()
{
    READ_PROLOGUE2(DrawingML_pPr)
    const QXmlStreamAttributes attrs(attributes());

    m_listStylePropertiesAltered = false;

    TRY_READ_ATTR_WITHOUT_NS(lvl)

    if (!lvl.isEmpty()) {
        m_currentListLevel = lvl.toInt() + 1;
    }

    m_currentBulletProperties = m_currentCombinedBulletProperties[m_currentListLevel];

#ifdef PPTXXMLSLIDEREADER_CPP
    inheritParagraphStyle(m_currentParagraphStyle);
#endif

    TRY_READ_ATTR_WITHOUT_NS(algn)
    algnToODF("fo:text-align", algn);

    TRY_READ_ATTR_WITHOUT_NS(marL)
    TRY_READ_ATTR_WITHOUT_NS(marR)
    TRY_READ_ATTR_WITHOUT_NS(indent)
    TRY_READ_ATTR_WITHOUT_NS(defTabSz)

    // Following settings are only applied if defined so they don't overwrite defaults
    // previous defined either in the slideLayout, SlideMaster or the defaultStyles.
    if (!marL.isEmpty()) {
        qreal marLeft;
        STRING_TO_QREAL(marL, marLeft, "attr:marL")
        marLeft = EMU_TO_POINT(marLeft);
        m_currentParagraphStyle.addPropertyPt("fo:margin-left", marLeft);
        m_currentBulletProperties.setMargin(marLeft);
        m_listStylePropertiesAltered = true;
    }
    if (!indent.isEmpty()) {
        qreal firstInd;
        STRING_TO_QREAL(indent, firstInd, "attr:indent")
        firstInd = EMU_TO_POINT(firstInd);
        m_currentParagraphStyle.addPropertyPt("fo:text-indent", firstInd);
        m_currentBulletProperties.setIndent(firstInd);
        m_listStylePropertiesAltered = true;
    }

    if (!marR.isEmpty()) {
        qreal marRight;
        STRING_TO_QREAL(marR, marRight, "attr:marR")
        m_currentParagraphStyle.addPropertyPt("fo:margin-right", EMU_TO_POINT(marRight));
    }
    if (!defTabSz.isEmpty()) {
        qreal tabSize;
        STRING_TO_QREAL(defTabSz, tabSize, "attr:defTabSz")
        m_currentParagraphStyle.addPropertyPt("style:tab-stop-distance", EMU_TO_POINT(tabSize));
    }

    m_currentTextStyleProperties = new KoCharacterStyle();
    m_currentTextStyle = KoGenStyle(KoGenStyle::TextAutoStyle, "text");

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(buAutoNum)
            ELSE_TRY_READ_IF(defRPr)
            ELSE_TRY_READ_IF(buNone)
            ELSE_TRY_READ_IF(buChar)
            ELSE_TRY_READ_IF(buClrTx)
            ELSE_TRY_READ_IF(buClr)
            ELSE_TRY_READ_IF(buFont)
            ELSE_TRY_READ_IF(buBlip)
            ELSE_TRY_READ_IF(buSzPct)
            ELSE_TRY_READ_IF(buSzPts)
            else if (QUALIFIED_NAME_IS(buSzTx)) {
                m_currentBulletProperties.setBulletRelativeSize(100);
            }
            else if (QUALIFIED_NAME_IS(spcBef)) {
                m_currentSpacingType = spacingMarginTop;
                TRY_READ(spcBef)
            }
            else if (QUALIFIED_NAME_IS(spcAft)) {
                m_currentSpacingType = spacingMarginBottom;
                TRY_READ(spcAft)
            }
            else if (QUALIFIED_NAME_IS(lnSpc)) {
                m_currentSpacingType = spacingLines;
                TRY_READ(lnSpc)
            }
            SKIP_UNKNOWN
        }
    }

    m_currentTextStyleProperties->saveOdf(m_currentTextStyle);

    delete m_currentTextStyleProperties;
    m_currentTextStyleProperties = 0;
    KoGenStyle::copyPropertiesFromStyle(m_currentTextStyle, m_currentParagraphStyle, KoGenStyle::TextType);

    READ_EPILOGUE
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

    if (!r_id.isEmpty() && m_context->relationships) {
        m_hyperLink = true;
        m_hyperLinkTarget = m_context->relationships->target(m_context->path, m_context->file, r_id);
        m_hyperLinkTarget.remove(0, m_context->path.length() + 1);
    }

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
    }

#if defined(PPTXXMLSLIDEREADER_CPP)
    // Where there is a hyperlink, hlink value should be used by default
    MSOOXML::DrawingMLColorSchemeItemBase *colorItem = 0;
    QString valTransformed = m_context->colorMap.value("hlink");
    colorItem = m_context->themes->colorScheme.value(valTransformed);
    if (colorItem) {
        m_currentColor = colorItem->value();
    }
#endif

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL custGeom
//! custGeom Handler (Custom Geometry)
/*
 Parent elements:
 - [done] spPr (§21.2.2.197);
 - [done] spPr (§21.3.2.23);
 - [done] spPr (§21.4.3.7);
 - [done] spPr (§20.1.2.2.35);
 - [done] spPr (§20.2.2.6);
 - [done] spPr (§20.5.2.30);
 - [done] spPr (§19.3.1.44)

 Child elements:
 - ahLst (List of Shape Adjust Handles) §20.1.9.1
 - avLst (List of Shape Adjust Values) §20.1.9.5
 - cxnLst (List of Shape Connection Sites) §20.1.9.10
 - gdLst (List of Shape Guides) §20.1.9.12
 - pathLst (List of Shape Paths) §20.1.9.16
 - rect (Shape Text Rectangle) §20.1.9.22

*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_custGeom()
{
    READ_PROLOGUE

    ComplexShapeHandler handler;
    m_customEquations = handler.defaultEquations();

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            if (name() == "avLst") {
                m_customEquations += handler.handle_avLst(this);
            }
            else if (name() == "gdLst") {
                m_customEquations += handler.handle_gdLst(this);
            }
            else if (name() == "pathLst") {
                m_customPath = handler.handle_pathLst(this);
                m_customEquations += handler.pathEquationsCreated();
            }
            else if (name() == "rect") {
                m_textareas = handler.handle_rect(this);
            }

        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL xfrm
//! xfrm  (2D Transform for Graphic Frame)
//! ECMA-376, 19.3.1.53, p.2862 (PresentationML)
//! ECMA-376, 20.5.2.36, p.3548 (SpreadsheetML)
/*! This element specifies the transform to be applied to the
  corresponding graphic frame. This transformation is applied to the
  graphic frame just as it would be for a shape or group shape. */

//! xfrm (2D Transform for Grouped Objects)
//! ECMA-376, 20.1.7.5, p.3185 (DrawingML)
/*! This element is nearly identical to the representation of 2-D
  transforms for ordinary shapes (§20.1.7.6). The only addition is a
  member to represent the Child offset and the Child extents. */

//! xfrm (2D Transform for Individual Objects)
//! ECMA-376, 20.1.7.6, p.3186 (DrawingML)
/*! This element represents 2-D transforms for ordinary shapes. */
/*!

 Parent elements:
 ----------------
 PresentationML:
 - [done] graphicFrame (§19.3.1.21)/(§20.5.2.16)

 SpreadsheetML
 - graphicFrame (§20.5.2.16)

 DrawingML:
 - [done] grpSpPr (§21.3.2.14)
 - [done] grpSpPr (§20.1.2.2.22)
 - [done] grpSpPr (§20.5.2.18)
 - [done] grpSpPr (§19.3.1.23)

 - graphicFrame (§20.1.2.2.18)
 - [done] spPr (§21.2.2.197)
 - [done] spPr (§21.3.2.23)
 - [done] spPr (§21.4.3.7)
 - [done] spPr (§20.1.2.2.35)
 - [done] spPr (§20.2.2.6)
 - [done] spPr (§20.5.2.30)
 - [done] spPr (§19.3.1.44)
 - [done] txSp (§20.1.2.2.41)

 Child elements:
 ---------------
 PresentationML/SpreadsheetML:
 - [done] ext (Extents) §20.1.7.3
 - [done] off (Offset) §20.1.7.4

 DrawingML:
 - [done] chExt (Child extends) §20.1.7.1 (grpSpPr only)
 - [done] chOff (Child offset) §20.1.7.2 (grpSpPr only)
 - [done] ext (Extents) §20.1.7.3
 - [done] off (Offset) §20.1.7.4

 Attributes:
 - [done] flipH (Horizontal Flip)
 - [done] flipV (Vertical Flip)
 - [done] rot (Rotation)
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

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            if (QUALIFIED_NAME_IS(off)) {
                TRY_READ(off);
            } else if (QUALIFIED_NAME_IS(ext)) {
                TRY_READ(ext);
            }
            ELSE_TRY_READ_IF(chOff)
            ELSE_TRY_READ_IF(chExt)
            ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL off
//! off handler (Offset)
//! DrawingML ECMA-376, 20.1.7.4, p. 3185.
/*! This element specifies the location of the bounding box of an object.
    Effects on an object are not included in this bounding box.

 Parent elements:
    - [done] xfrm (§21.3.2.28)
    - [done] xfrm (§20.1.7.5)
    - [done] xfrm (§20.1.7.6)
    - [done] xfrm (§20.5.2.36)
    - [done] xfrm (§19.3.1.53)

 No child elements.

 Attributes:
    - [done] x (X-Axis Coordinate)
    - [done] y (Y-Axis Coordinate)
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_off()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    READ_ATTR_WITHOUT_NS(x)
    STRING_TO_LONGLONG(x, m_svgX, "off@x")
    READ_ATTR_WITHOUT_NS(y)
    STRING_TO_LONGLONG(y, m_svgY, "off@y")

    if (!m_inGrpSpPr) {
        int index = 0;
        while (index < m_svgProp.size()) {
            //(a:off(x) - a:chOff(x))/a:chExt(x) * a(p):ext(x) + a(p):off(x)
            GroupProp prop = m_svgProp.at(m_svgProp.size() - 1 - index);
            m_svgX = (m_svgX - prop.svgXChOld) / prop.svgWidthChOld * prop.svgWidthOld + prop.svgXOld;
            m_svgY = (m_svgY - prop.svgYChOld) / prop.svgHeightChOld * prop.svgHeightOld + prop.svgYOld;
            ++index;
        }
    }

    readNext();
    READ_EPILOGUE
}

//! chOff handler (Child offset)
//! Look parent, children
#undef CURRENT_EL
#define CURRENT_EL chOff
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_chOff()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    READ_ATTR_WITHOUT_NS(x)
    STRING_TO_INT(x, m_svgChX, "chOff@x")
    READ_ATTR_WITHOUT_NS(y)
    STRING_TO_INT(y, m_svgChY, "chOff@y")

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL ext
//! ext handler (Extents)
//! DrawingML ECMA-376, 20.1.7.3, p. 3185.
/*! This element specifies the size of the bounding box enclosing the referenced object.
 Parent elements:
 - [done] xfrm (§21.3.2.28)
 - [done] xfrm (§20.1.7.5)
 - [done] xfrm (§20.1.7.6)
 - [done] xfrm (§20.5.2.36)
 - [done] xfrm (§19.3.1.53)
 .
 No child elements.

 Attributes:
 - [done] cx (Extent Length) Specifies the length of the extents rectangle in EMUs. This rectangle shall dictate
      the size of the object as displayed (the result of any scaling to the original object).
 - [done] cy (Extent Width) Specifies the width of the extents rectangle in EMUs.
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_ext()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    READ_ATTR_WITHOUT_NS(cx)
    STRING_TO_INT(cx, m_svgWidth, "ext@cx")
    READ_ATTR_WITHOUT_NS(cy)
    STRING_TO_INT(cy, m_svgHeight, "ext@cy")

    if (!m_inGrpSpPr) {
        int index = 0;
        while (index < m_svgProp.size()) {
            //(a:off(x) - a:chOff(x))/a:chExt(x) * a(p):ext(x) + a(p):off(x)
            GroupProp prop = m_svgProp.at(m_svgProp.size() - 1 - index);

            m_svgWidth = m_svgWidth * prop.svgWidthOld / prop.svgWidthChOld;
            m_svgHeight = m_svgHeight * prop.svgHeightOld / prop.svgHeightChOld;
            ++index;
        }
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL chExt
//! chExt handler (Child extend)
//! Look parent, children
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_chExt()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    READ_ATTR_WITHOUT_NS(cx)
    STRING_TO_INT(cx, m_svgChWidth, "chExt@cx")
    READ_ATTR_WITHOUT_NS(cy)
    STRING_TO_INT(cy, m_svgChHeight, "chExt@cy")

    readNext();
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
    - [done] buBlip (§21.1.2.4.2)

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
    - [done] duotone (Duotone Effect) §20.1.8.23
    - [done] extLst (Extension List) §20.1.2.2.15
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

    // Read attributes.
    const QXmlStreamAttributes attrs(attributes());
//! @todo more attrs
    TRY_READ_ATTR_WITH_NS(r, embed)
    debugMsooXml << "embed:" << r_embed;
    if (!r_embed.isEmpty() && m_context->relationships) {
        const QString sourceName(m_context->relationships->target(m_context->path,
                                                                  m_context->file, r_embed));
        debugMsooXml << "sourceName:" << sourceName;

        //A test file is attached to Bug 286700.
        if (sourceName.endsWith(QLatin1String("NULL"))) {
            skipCurrentElement();
            READ_EPILOGUE
        }

        m_context->import->imageSize(sourceName, m_imageSize);

        if (sourceName.isEmpty()) {
            return KoFilter::FileNotFound;
        }
        QString destinationName = QLatin1String("Pictures/") + sourceName.mid(sourceName.lastIndexOf('/') + 1);

        RETURN_IF_ERROR( m_context->import->copyFile(sourceName, destinationName, false ) )
        addManifestEntryForFile(destinationName);
        m_recentDestName = sourceName;
        addManifestEntryForPicturesDir();
        m_xlinkHref = destinationName;
    }

    // Read child elements
    while (!atEnd()) {
        readNext();
        debugMsooXml << *this;
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(biLevel)
            ELSE_TRY_READ_IF(grayscl)
            ELSE_TRY_READ_IF(lum)
            ELSE_TRY_READ_IF(duotone)
            SKIP_UNKNOWN
//! @todo add ELSE_WRONG_FORMAT
        }
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
    - [done]blipFill (§21.3.2.2) - DrawingML, p. 3919
    - [done] blipFill (§20.1.8.14) - DrawingML, p. 3195
    - [done] blipFill (§20.2.2.1) - DrawingML, p. 3456
    - [done] blipFill (§20.5.2.2) - DrawingML, p. 3518
    - [done] blipFill (§19.3.1.4) - PresentationML, p. 2818

 Child elements:
    - [done] fillRect (Fill Rectangle) §20.1.8.30
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_stretch()
{
    READ_PROLOGUE

    m_currentDrawStyle->addProperty("style:repeat", QLatin1String("stretch"));

    while (!atEnd()) {
        readNext();
        debugMsooXml << *this;
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(fillRect)
            ELSE_WRONG_FORMAT
        }
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

    m_currentDrawStyle->addProperty("draw:color-mode", QLatin1String("mono"));
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

    m_currentDrawStyle->addProperty("draw:color-mode", QLatin1String("greyscale"));

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
 - [done] bright (Brightness)
 - [done] contrast (Contrast)
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_lum()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR_WITHOUT_NS(bright)
    TRY_READ_ATTR_WITHOUT_NS(contrast)

    // Numbers are in format 70000, so we need to remove 3 zeros
    // Adding bright to luminance may not be correct (but better than hardcoding to watermark mode)
    if (!bright.isEmpty()) {
        m_currentDrawStyle->addProperty("draw:luminance", bright.left(bright.length()-3) + '%');
    }

    if (!contrast.isEmpty()) {
        m_currentDrawStyle->addProperty("draw:contrast", contrast.left(contrast.length()-3) + '%');
    }

    readNext();
    READ_EPILOGUE
}


#undef CURRENT_EL
#define CURRENT_EL duotone
//! duotone handler (Duotone effect)
/*! ECMA-376, 20.1.8.23, p 3214

  This element specifies a duotone effect. For each pixel,
  combines clr1 and clr2 through a linear interpolation to
  determine the new color for that pixel.

  In Office, the interpolation is based on the luminance of the pixel.
  http://msdn.microsoft.com/en-us/library/ff534294%28v=office.12%29.aspx

 Parent elements:
 - [done] blip (§20.1.8.13)
 - cont (§20.1.8.20)
 - effectDag (§20.1.8.25)

 Child elements:
    - [done] hslClr (Hue, Saturation, Luminance Color Model) §20.1.2.3.13
    - [done] prstClr (Preset Color) §20.1.2.3.22
    - [done] schemeClr (Scheme Color) §20.1.2.3.29
    - [done] scrgbClr (RGB Color Model - Percentage Variant) §20.1.2.3.30
    - [done] srgbClr (RGB Color Model - Hex Variant) §20.1.2.3.32
    - [done] sysClr (System Color) §20.1.2.3.33

*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_duotone()
{
    READ_PROLOGUE

    int colorCount = 0;
    QColor clr1;
    QColor clr2;

    while (!atEnd()) {
        readNext();

        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(hslClr)
            ELSE_TRY_READ_IF(prstClr)
            ELSE_TRY_READ_IF(schemeClr)
            ELSE_TRY_READ_IF(scrgbClr)
            ELSE_TRY_READ_IF(srgbClr)
            ELSE_TRY_READ_IF(sysClr)
            SKIP_UNKNOWN

            if (colorCount == 0) {
                clr1 = m_currentColor;
            } else {
                clr2 = m_currentColor;
            }
            colorCount++;
        }
    }

    QImage image;
    m_context->import->imageFromFile(m_recentDestName, image);
    // don't apply duotone to unsupported picture formats in QImage like emf, wmf case
    if (!image.isNull()) {

        QColor c1 = clr1.isValid() ? clr1 : Qt::black;
        QColor c2 = clr2.isValid() ? clr2 : Qt::white;

        image = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
        for (int y = 0; y < image.height(); y++) {
            QRgb *scanline = reinterpret_cast<QRgb *>(image.scanLine(y));
            for (int x = 0; x < image.width(); x++) {
                QRgb c = scanline[x];
                int luminosity = (5036060U * quint32(qRed(c)) + 9886846U * quint32(qGreen(c)) + 1920103U * quint32(qBlue(c))) >> 24;
                qreal grayF = (255 - luminosity) / 255.0;
                int r = grayF * c1.red()     + (1.0 - grayF) * c2.red();
                int g = grayF * c1.green()   + (1.0 - grayF) * c2.green();
                int b = grayF * c1.blue()    + (1.0 - grayF) * c2.blue();
                scanline[x] = qRgba(r,g,b,qAlpha(c));
            }
        }

        QString fileName = m_recentDestName.mid(m_recentDestName.lastIndexOf('/') + 1);
        fileName = fileName.left(fileName.lastIndexOf('.'));
        QString destinationName = QLatin1String("Pictures/") + fileName + QString("_duotoned_%1_%2.png").arg(c1.name().mid(1)).arg(c2.name().mid(1));

        RETURN_IF_ERROR( m_context->import->createImage(image, destinationName) )
        addManifestEntryForFile(destinationName);
        m_xlinkHref = destinationName;
    }

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
 - [done] blipFill (§21.3.2.2)
 - [done] blipFill (§20.1.8.14)
 - [done] blipFill (§20.2.2.1)
 - [done] blipFill (§20.5.2.2)
 - [done] blipFill (§19.3.1.4)

 No child elements.
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_tile()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    m_currentDrawStyle->addProperty("style:repeat", QLatin1String("repeat"));
//! @todo algn - convert to "ODF's Fill Image Tile Reference Point"
    m_currentDrawStyle->addProperty("draw:fill-image-ref-point", "top-left");

//! @todo flip
//! @todo sx
//! @todo sy
//! @todo tx
//! @todo ty

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL srcRect
//! srcRect handler (Source Rectangle)
/*
 Parent elements:
 - [done] blipFill (§21.3.2.2);
 - [done] blipFill (§20.1.8.14);
 - [done] blipFill (§20.2.2.1);
 - [done] blipFill (§20.5.2.2);
 - [done] blipFill (§19.3.1.4)

 No child elements.

*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_srcRect()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs( attributes() );
    TRY_READ_ATTR_WITHOUT_NS(b)
    TRY_READ_ATTR_WITHOUT_NS(l)
    TRY_READ_ATTR_WITHOUT_NS(r)
    TRY_READ_ATTR_WITHOUT_NS(t)

    if (!m_recentDestName.endsWith(QLatin1String("wmf")) && !m_recentDestName.endsWith(QLatin1String("emf"))) {
        if (!b.isEmpty() || !l.isEmpty() || !r.isEmpty() || !t.isEmpty()) {
            qreal bReal = b.toDouble() / 100000;
            qreal tReal = t.toDouble() / 100000;
            qreal lReal = l.toDouble() / 100000;
            qreal rReal = r.toDouble() / 100000;

            int rectLeft = m_imageSize.rwidth() * lReal;
            int rectTop = m_imageSize.rheight() * tReal;
            int rectWidth = m_imageSize.rwidth() - m_imageSize.rwidth() * rReal - rectLeft;
            int rectHeight = m_imageSize.rheight() - m_imageSize.rheight() * bReal - rectTop;

            QString fileName = m_recentDestName.mid(m_recentDestName.lastIndexOf('/') + 1);
            fileName = fileName.left(fileName.lastIndexOf('.'));

            QString destinationName = QLatin1String("Pictures/") + fileName + QString("_cropped_%1_%2.png").arg(rectWidth).arg(rectHeight);

            QImage image;
            m_context->import->imageFromFile(m_recentDestName, image);

            // first copy the part we are interested in and then to the conversation what may
            // save us some bytes and circles when the copy is way smaller then the original
            // is in which case the convertToFormat is more cheap.
            image = image.copy(rectLeft, rectTop, rectWidth, rectHeight);
            image = image.convertToFormat(QImage::Format_ARGB32);

            RETURN_IF_ERROR( m_context->import->createImage(image, destinationName) )
            addManifestEntryForFile(destinationName);
            m_xlinkHref = destinationName;
        }
    }

    readNext();
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

    const QXmlStreamAttributes attrs( attributes() );
//! @todo use ST_Percentage_withMsooxmlFix_to_double for attributes b, l, r, t
    TRY_READ_ATTR_WITHOUT_NS(b)
    TRY_READ_ATTR_WITHOUT_NS(l)
    TRY_READ_ATTR_WITHOUT_NS(r)
    TRY_READ_ATTR_WITHOUT_NS(t)
//KOMSOOXML_EXPORT qreal ST_Percentage_withMsooxmlFix_to_double(const QString& val, bool& ok);

    if (!b.isEmpty() || !l.isEmpty() || !r.isEmpty() || !t.isEmpty()) {
        // TODO: One way to approach this would be to first scale the image to the size of the slide
        // then, resize it according to the percentages & make sure there are no black areas
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL graphic
//! graphic handler (Graphic Object)
/*! ECMA-376, 20.1.2.2.16, p.3037.

 This element specifies the existence of a single graphic object.  Document
 authors should refer to this element when they wish to persist a graphical
 object of some kind. The specification for this graphical object is provided
 entirely by the document author and referenced within the graphicData child
 element.

 Parent elements:
 - [done] anchor (§20.4.2.3)
 - [done] graphicFrame (§21.3.2.12)
 - [done] graphicFrame (§20.1.2.2.18)
 - [done] graphicFrame (§20.5.2.16)
 - [done] graphicFrame (§19.3.1.21)
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
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(graphicData)
            ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL graphicData
//! graphicData handler (Graphic Object Data)
/*! ECMA-376, 20.1.2.2.17, p.3038.

 This element specifies the reference to a graphic object within the
 document.  This graphic object is provided entirely by the document
 authors who choose to persist this data within the document.

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

    // TODO: Is it possible to have a group of graphic objects in a chart?
    // It's possible in case of a diagram.
    m_context->graphicObjectIsGroup = false;

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF_NS(pic, pic)
            // Charting diagram
            ELSE_TRY_READ_IF_NS(c, chart)
            // DrawingML diagram
            ELSE_TRY_READ_IF_NS(dgm, relIds)
            ELSE_TRY_READ_IF_NS(lc, lockedCanvas)
#ifdef PPTXXMLSLIDEREADER_CPP
            ELSE_TRY_READ_IF_NS(p, oleObj)
            ELSE_TRY_READ_IF_NS(a, tbl)
#endif
            else if (qualifiedName() == "mc:AlternateContent") {
                TRY_READ(AlternateContent)
            }
            SKIP_UNKNOWN
//! @todo add ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL blipFill
//! blipFill handler (Picture Fill)
//! ECMA-376, 19.3.1.4, p.2818 (PresentationML)
//! ECMA-376, 20.1.8.14, p.3195 (DrawingML)
//! @todo use it in DrawingML, 20.2.2.1, p. 3456
/*! This element specifies the type of picture fill that the picture
 object has.  Because a picture has a picture fill already by default,
 it is possible to have two fills specified for a picture object.

 BLIPs refer to Binary Large Image or Pictures. Blip Fills are made up
 of several components: a Blip Reference, a Source Rectangle, and a
 Fill Mode.  See also M.4.8.4.3 Blip Fills, ECMA-376, p. 5411.

 Parent elements:
     PresentationML:
     - [done] pic (§19.3.1.37)

     DrawingML:
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
     - [done] pic (§20.1.2.2.30)
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
    - [done] srcRect (Source Rectangle) §20.1.8.55
    - [done] stretch (Stretch) §20.1.8.56
    - [done] tile (Tile) §20.1.8.58

 Attributes:
    - dpi (DPI Setting)
    - rotWithShape (Rotate With Shape)
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_blipFill(blipFillCaller caller)
{
    debugMsooXml << "Blip Caller:" << (char)caller;
    QString qn;

    if (m_isLockedCanvas) {
        READ_PROLOGUE
    } else {
        // Do not use READ_PROLOGUE because namespace depends on caller
        PUSH_NAME_INTERNAL

        QString ns;
        // 'p' by default; for dml in docx use 'pic'
#ifdef DOCXXMLDOCREADER_CPP
        if (caller == blipFill_pic) {
            ns = QLatin1String("pic");
        } else {
            ns = QChar((char)caller);
        }
#elif defined XLSXXMLDRAWINGREADER_CPP
        if (caller == blipFill_pic) {
            ns = QLatin1String("xdr");
        } else {
            ns = QChar((char)caller);
        }
#else
        ns = QChar((char)caller);
#endif
        qn = QString(ns + ":" STRINGIFY(CURRENT_EL));
        if (!expectEl(qn)) {
            return KoFilter::WrongFormat;
        }
    }

    while (!atEnd()) {
        readNext();
        debugMsooXml << *this;
        if (m_isLockedCanvas) {
            BREAK_IF_END_OF(CURRENT_EL)
        } else {
            BREAK_IF_END_OF_QSTRING(qn)
        }
        if (isStartElement()) {
            TRY_READ_IF(blip)
            ELSE_TRY_READ_IF(stretch)
            ELSE_TRY_READ_IF(tile)
            ELSE_TRY_READ_IF(srcRect)
            ELSE_WRONG_FORMAT
        }
    }

    if (m_isLockedCanvas) {
        READ_EPILOGUE
    } else {
        // Do not use READ_EPILOGUE because namespace depends on caller
        POP_NAME_INTERNAL

        if (!expectElEnd(qn)) {
            debugMsooXml << "READ_EPILOGUE:" << qn << "not found!";
            return KoFilter::WrongFormat;
        }
        return KoFilter::OK;
    }
}

#undef CURRENT_EL
#define CURRENT_EL txSp
//! txSp (Text Shape)
//! ECMA-376, 20.1.2.2.41, p.3057 (DrawingML)
/*! This element specifies the existence of a text shape within a
  parent shape. This text shape is specifically used for displaying
  text as it has only text related child elements.

  ParentElements:
  - [done] grpSp (§20.1.2.2.20)
  - [done] lockedCanvas (§20.3.2.1)
  - [done] sp (§20.1.2.2.33)

  Child Elements:
  - extLst (Extension List) §20.1.2.2.15
  - [done] txBody (Shape Text Body) §20.1.2.2.40
  - useSpRect (Use Shape Text Rectangle) §20.1.2.2.42
  - [done] xfrm (2D Transform for Individual Objects) §20.1.7.6
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_txSp()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        debugMsooXml << *this;
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
#if defined PPTXXMLSLIDEREADER_CPP
            TRY_READ_IF(txBody)
#else
            if (qualifiedName() == QLatin1String(QUALIFIED_NAME(txBody))) {
                TRY_READ_IN_CONTEXT(DrawingML_txBody)
            }
#endif
            ELSE_TRY_READ_IF(xfrm)
            SKIP_UNKNOWN
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}

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

#include "MsooXmlDrawingMLSharedImpl.h"

// ================================================================
//                     Namespace in {a,wp}
// ================================================================
#undef MSOOXML_CURRENT_NS
#ifndef NO_DRAWINGML_NS
#define MSOOXML_CURRENT_NS DRAWINGML_NS
#endif

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
        m_currentDrawStyle->addProperty(QLatin1String(odfEl), s, KoGenStyle::GraphicType);
    }
}

#undef CURRENT_EL
#define CURRENT_EL lstStyle
//! lstStyle handler (Text List Styles)
//! ECMA-376, DrawingML 21.1.2.4.12, p. 3651.
//! This element specifies the list of styles associated with this body of text.
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
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_lstStyle()
{
    READ_PROLOGUE
    m_currentListStyle = KoGenStyle(KoGenStyle::ListAutoStyle);

    m_currentCombinedBulletProperties.clear();
    m_currentBulletProperties.clear();
    m_currentCombinedTextStyles.clear();
    m_currentCombinedParagraphStyles.clear();

#ifdef PPTXXMLSLIDEREADER_CPP
    inheritListStyles();
    if (m_context->type == SlideMaster || m_context->type == NotesMaster || m_context->type == SlideLayout) {
        inheritAllTextAndParagraphStyles();
    }
#endif

    while (!atEnd()) {
        readNext();
        debugMsooXml << *this;
        BREAK_IF_END_OF(CURRENT_EL)
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
            SKIP_UNKNOWN
//! @todo add ELSE_WRONG_FORMAT
        }
    }

#ifdef PPTXXMLSLIDEREADER_CPP
    saveCurrentListStyles();
    saveCurrentStyles();
#endif

    // Should be zero to not mess anything
    m_currentListLevel = 0;

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL latin
/*! latin handler (Latin Font) ECMA-376, 21.1.2.3.7, p.3621.
 Parent elements:
 - [done] defRPr (§21.1.2.3)
 - [done] endParaRPr (§21.1.2.2.3)
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

#ifdef PPTXXMLDOCUMENTREADER_CPP
    // TODO: Process the pitchFamili attribute.
    defaultLatinFonts[defaultLatinFonts.size() - 1] = typeface;

    // Skip reading because the theme is unknown at time of reading.
    skipCurrentElement();
    READ_EPILOGUE
#endif

    if (!typeface.isEmpty()) {
        QString font = typeface;
        if (typeface.startsWith("+mj")) {
            font = m_context->themes->fontScheme.majorFonts.latinTypeface;
        }
        else if (typeface.startsWith("+mn")) {
            font = m_context->themes->fontScheme.minorFonts.latinTypeface;
        }
        m_currentTextStyleProperties->setFontFamily(font);
    }

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
            //TODO:
            break;
        case 4: //Script
            //TODO:
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
 - [done] defRPr (§21.1.2.3.2)
 - [done] endParaRPr (§21.1.2.2.3)
 - [done] rPr (§21.1.2.3.9)

 Child elements:
 - [done] hslClr (Hue, Saturation, Luminance Color Model) §20.1.2.3.13
 - [done] prstClr (Preset Color) §20.1.2.3.22
 - [done] schemeClr (Scheme Color) §20.1.2.3.29
 - [done] scrgbClr (RGB Color Model - Percentage Variant) §20.1.2.3.30
 - [done] srgbClr (RGB Color Model - Hex Variant) §20.1.2.3.32
 - [done] sysClr (System Color) §20.1.2.3.33
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_DrawingML_highlight()
{
    READ_PROLOGUE2(DrawingML_highlight)

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(schemeClr)
            ELSE_TRY_READ_IF(scrgbClr)
            ELSE_TRY_READ_IF(srgbClr)
            ELSE_TRY_READ_IF(sysClr)
            ELSE_TRY_READ_IF(prstClr)
            ELSE_TRY_READ_IF(hslClr)
            ELSE_WRONG_FORMAT
        }
    }
    // note: paragraph background is unsupported in presentation applications anyway...
    if (m_currentColor.isValid()) {
        m_currentParagraphStyle.addProperty("fo:background-color", m_currentColor.name());
        m_currentColor = QColor();
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL gradFill
//! Gradient Fill
/*
 Parent Elements:
 - bg (§21.4.3.1);
 - [done] bgFillStyleLst (§20.1.4.1.7);
 - [done] bgPr (§19.3.1.2);
 - [elsewhere] defRPr (§21.1.2.3.2);
 - [elsewhere] endParaRPr (§21.1.2.2.3);
 - fill (§20.1.8.28);
 - fill (§20.1.4.2.9);
 - fillOverlay (§20.1.8.29);
 - [done] fillStyleLst (§20.1.4.1.13);
 - [done] grpSpPr (§21.3.2.14);
 - [done] grpSpPr (§20.1.2.2.22);
 - [done] grpSpPr (§20.5.2.18);
 - [done] grpSpPr (§19.3.1.23);
 - ln (§20.1.2.2.24);
 - lnB (§21.1.3.5);
 - lnBlToTr (§21.1.3.6);
 - lnL (§21.1.3.7);
 - lnR (§21.1.3.8);
 - lnT (§21.1.3.9);
 - lnTlToBr (§21.1.3.10);
 - [elsewhere] rPr (§21.1.2.3.9);
 - [done] spPr (§21.2.2.197);
 - [done] spPr (§21.3.2.23);
 - [done] spPr (§21.4.3.7);
 - [done] spPr (§20.1.2.2.35);
 - [done] spPr (§20.2.2.6);
 - [done] spPr (§20.5.2.30);
 - [done] spPr (§19.3.1.44);
 - tblPr (§21.1.3.15);
 - tcPr (§21.1.3.17);
 - uFill (§21.1.2.3.12);
 - uLn (§21.1.2.3.14)

 Child Elements:
 - [done] gsLst (Gradient Stop List) §20.1.8.37
 - [done] lin (Linear Gradient Fill) §20.1.8.41
 - path (Path Gradient) §20.1.8.46
 - tileRect (Tile Rectangle) §20.1.8.59

*/
//! @todo support this properly
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_gradFill()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    bool gradRotation = false;

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(gsLst)
            else if (qualifiedName() == "a:lin") {
                gradRotation = true;
                TRY_READ(lin)
            }
            SKIP_UNKNOWN
        }
    }

    if (gradRotation) {
        qreal angle = -m_gradAngle.toDouble() / 60000.0 / 180.0 * M_PI;
        m_currentGradientStyle.addAttribute("svg:x1", QString("%1%").arg(50 - 50 * cos(angle)));
        m_currentGradientStyle.addAttribute("svg:y1", QString("%1%").arg(50 + 50 * sin(angle)));
        m_currentGradientStyle.addAttribute("svg:x2", QString("%1%").arg(50 + 50 * cos(angle)));
        m_currentGradientStyle.addAttribute("svg:y2", QString("%1%").arg(50 - 50 * sin(angle)));
    } else {
        m_currentGradientStyle.addAttribute("svg:x1", "50%");
        m_currentGradientStyle.addAttribute("svg:y1", "0%");
        m_currentGradientStyle.addAttribute("svg:x2", "50%");
        m_currentGradientStyle.addAttribute("svg:y2", "100%");
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL lin
//! linear gradient fill
/*
 Parent Elements:
 - [done] gradFill (§20.1.8.33)

 Child Elements:
 - none

*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_lin()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR_WITHOUT_NS_INTO(ang, m_gradAngle)

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL gradFill
//! Special gradFill handler for text properties
// Meant to support gradFill as part of rpr as well as can be done as odf does not support
// proper way
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_gradFillRpr()
{
    READ_PROLOGUE2(gradFillRPr)

    QList<QPair<int, QColor> > gradPositions;
    int exactIndex = -1;
    int beforeIndex = -1;
    int afterIndex = -1;

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            if (name() == "gs") {
                TRY_READ(gs)
                gradPositions.push_back(QPair<int, QColor>(m_gradPosition, m_currentColor));
                if (m_gradPosition == 50) {
                    exactIndex = gradPositions.size() - 1;
                } else if (m_gradPosition < 50) {
                    if (beforeIndex < 0) {
                        beforeIndex = gradPositions.size() - 1;
                    } else if (m_gradPosition > gradPositions.at(beforeIndex).first) {
                        beforeIndex = gradPositions.size() - 1;
                    }
                } else {
                    if (afterIndex < 0) {
                        afterIndex = gradPositions.size() - 1;
                    } else if (m_gradPosition < gradPositions.at(afterIndex).first) {
                        afterIndex = gradPositions.size() - 1;
                    }
                }
            }
        }
    }

    // The logic here is to find the color that is in the middle, or if it's not present
    // find the colors before and after it and calculate the middle color

    if (exactIndex > -1) {
        m_currentColor = gradPositions.at(exactIndex).second;
    }
    else {
        if (beforeIndex < 0) {
            beforeIndex = 0; // It is possible that the stops are only listed for aread 50+
        }
        if (afterIndex < 0) {
            afterIndex = beforeIndex; // It is possible that the stops are only listed for areas -50
        }
        int firstDistance = 50 - gradPositions.at(beforeIndex).first;
        int secondDistance = gradPositions.at(afterIndex).first - 50;
        qreal multiplier = 0;
        int red, green, blue;

        if (firstDistance <= secondDistance) {
            multiplier = secondDistance / firstDistance;
            red = multiplier * gradPositions.at(beforeIndex).second.red() + gradPositions.at(afterIndex).second.red();
            green = multiplier * gradPositions.at(beforeIndex).second.green() + gradPositions.at(afterIndex).second.green();
            blue = multiplier * gradPositions.at(beforeIndex).second.blue() + gradPositions.at(afterIndex).second.blue();
        } else {
            multiplier = firstDistance / secondDistance;
            red = multiplier * gradPositions.at(afterIndex).second.red() + gradPositions.at(beforeIndex).second.red();
            green = multiplier * gradPositions.at(afterIndex).second.green() + gradPositions.at(beforeIndex).second.green();
            blue = multiplier * gradPositions.at(afterIndex).second.blue() + gradPositions.at(beforeIndex).second.blue();
        }
        red = red / (multiplier + 1);
        green = green / (multiplier + 1);
        blue = blue / (multiplier + 1);
        m_currentColor = QColor(red, green, blue);
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL gsLst
//! gradient stop list
/*
 Parent Elements:
 - [done] gradFill (§20.1.8.33)

 Child Elements:
 - [done] gs (Gradient stops) §20.1.8.36

*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_gsLst()
{
    READ_PROLOGUE

    int index = 0;

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            if (QUALIFIED_NAME_IS(gs)) {
                TRY_READ(gs)
                qreal alphaLevel = 1;
                if (m_currentAlpha > 0) {
                    alphaLevel = m_currentAlpha/100.0;
                }
                QString contents = QString("<svg:stop svg:offset=\"%1\" svg:stop-color=\"%2\" svg:stop-opacity=\"%3\"/>").arg(m_gradPosition/100.0).arg(m_currentColor.name()).arg(alphaLevel);
                QString name = QString("%1").arg(index);
                m_currentGradientStyle.addChildElement(name, contents);
                ++index;
            }
            ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL gs
//! gradient stops
/*
 Parent Elements:
 - [done] gsLst (§20.1.8.37)

 Child Elements:
 - [done] hslClr (Hue, Saturation, Luminance Color Model) §20.1.2.3.13
 - [done] prstClr (Preset Color) §20.1.2.3.22
 - [done] schemeClr (Scheme Color) §20.1.2.3.29
 - [done] scrgbClr (RGB Color Model - Percentage Variant) §20.1.2.3.30
 - [done] srgbClr (RGB Color Model - Hex Variant) §20.1.2.3.32
 - [done] sysClr (System Color) §20.1.2.3.33
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_gs()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(pos)

    m_gradPosition = pos.toInt() / 1000;

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(schemeClr)
            ELSE_TRY_READ_IF(srgbClr)
            ELSE_TRY_READ_IF(sysClr)
            ELSE_TRY_READ_IF(scrgbClr)
            ELSE_TRY_READ_IF(prstClr)
            ELSE_TRY_READ_IF(hslClr)
            ELSE_WRONG_FORMAT
        }
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
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_noFill()
{
    READ_PROLOGUE
    readNext();
    READ_EPILOGUE
}

// prstGeom (preset geometry)
/*
 Parent elements:
 - [done] spPr (§21.2.2.197)
 - [done] spPr (§21.3.2.23)
 - [done] spPr (§21.4.3.7)
 - [done] spPr (§20.1.2.2.35)
 - [done] spPr (§20.2.2.6)
 - [done] spPr (§20.5.2.30)
 - [done] spPr (§19.3.1.44)

 Child elements:
 - [done] avLst (List of Shape Adjust Values) §20.1.9.5

*/
#undef CURRENT_EL
#define CURRENT_EL prstGeom
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_prstGeom()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(prst)
    m_contentType = prst;

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(avLst)
            ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}

// avLst handler (List of Shape Adjust Values)
/*
 Parent elements:
 - [done - special handling in customGeom] custGeom (§20.1.9.8);
 . [done] prstGeom (§20.1.9.18);
 -  prstTxWarp (§20.1.9.19)

 Child elements:
 - [done] gd (Shape Guide) §20.1.9.11

*/
#undef CURRENT_EL
#define CURRENT_EL avLst
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_avLst()
{
    READ_PROLOGUE

    m_contentAvLstExists = true;
    m_avModifiers.clear();

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(gd)
            ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}

// gd handler (Shape guide)
/*
 Parent elements:
 - [done] avLst (§20.1.9.5);
 - [done - elsewhere] gdLst (§20.1.9.12)

 Child elements:
 - none

*/
#undef CURRENT_EL
#define CURRENT_EL gd
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_gd()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(name)
    TRY_READ_ATTR_WITHOUT_NS(fmla)

    // In theory we should interpret all possible values here, not just "val"
    // in practice it does not happen
    if (fmla.startsWith("val ")) {
        fmla.remove(0, 4);
    }

    m_avModifiers[name] = fmla;

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL effectLst
//! Effect list
/*
 Parent elements:
 - bg (§21.4.3.1);
 - [done] bgPr (§19.3.1.2);
 - defRPr (§21.1.2.3.2);
 - effect (§20.1.4.2.7);
 - effectStyle (§20.1.4.1.11);
 - endParaRPr (§21.1.2.2.3);
 - [done] grpSpPr (§21.3.2.14);
 - [done] grpSpPr (§20.1.2.2.22);
 - [done] grpSpPr (§20.5.2.18);
 - [done] grpSpPr (§19.3.1.23);
 - rPr (§21.1.2.3.9);
 - [done] spPr (§21.2.2.197);
 - [done] spPr (§21.3.2.23);
 - [done] spPr (§21.4.3.7);
 - [done] spPr (§20.1.2.2.35);
 - [done] spPr (§20.2.2.6);
 - [done] spPr (§20.5.2.30);
 - [done] spPr (§19.3.1.44);
 - tblPr (§21.1.3.15);
 - whole (§21.4.3.9)

 Child elements:
 - blur (Blur Effect) §20.1.8.15
 - fillOverlay (Fill Overlay Effect) §20.1.8.29
 - glow (Glow Effect) §20.1.8.32
 - innerShdw (Inner Shadow Effect) §20.1.8.40
 - [done] outerShdw (Outer Shadow Effect) §20.1.8.45
 - prstShdw (Preset Shadow) §20.1.8.49
 - reflection (Reflection Effect) §20.1.8.50
 - softEdge (Soft Edge Effect) §20.1.8.53

*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_effectLst()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(outerShdw)
            SKIP_UNKNOWN
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL outerShdw
//! Outer shadow
/*
 Parent elements:
 - cont (§20.1.8.20);
 - effectDag (§20.1.8.25);
 - [done] effectLst (§20.1.8.26)

 Child elements:
 - [done] hslClr (Hue, Saturation, Luminance Color Model) §20.1.2.3.13
 - [done] prstClr (Preset Color) §20.1.2.3.22
 - [done] schemeClr (Scheme Color) §20.1.2.3.29
 - [done] scrgbClr (RGB Color Model - Percentage Variant) §20.1.2.3.30
 - [done] srgbClr (RGB Color Model - Hex Variant) §20.1.2.3.32
 - [done] sysClr (System Color) §20.1.2.3.33
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_outerShdw()
{
    READ_PROLOGUE

    QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR_WITHOUT_NS(dir)
    TRY_READ_ATTR_WITHOUT_NS(dist)

    qreal angle = (qreal)dir.toDouble() * ((qreal)(M_PI) / (qreal)180.0)/ (qreal)60000.0;
    qreal xDist = EMU_TO_CM(dist.toInt() / 2) * cos(angle);
    qreal yDist = EMU_TO_CM(dist.toInt() / 2) * sin(angle);

    m_currentDrawStyle->addProperty("draw:shadow-offset-x", QString("%1cm").arg(xDist, 3, 'f'));
    m_currentDrawStyle->addProperty("draw:shadow-offset-y", QString("%1cm").arg(yDist, 3, 'f'));

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(srgbClr)
            ELSE_TRY_READ_IF(schemeClr)
            ELSE_TRY_READ_IF(scrgbClr)
            ELSE_TRY_READ_IF(sysClr)
            ELSE_TRY_READ_IF(prstClr)
            ELSE_TRY_READ_IF(hslClr)
            ELSE_WRONG_FORMAT
        }
    }

    if (m_currentColor != QColor()) {
        m_currentDrawStyle->addProperty("draw:shadow", "visible");
        m_currentDrawStyle->addProperty("draw:shadow-color", m_currentColor.name());
        m_currentColor = QColor();
        if (m_currentAlpha > 0) {
            m_currentDrawStyle->addProperty("draw:shadow-opacity", QString("%1%").arg(m_currentAlpha));
        }
    }

    READ_EPILOGUE
}

KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::lvlHelper(const QString& level) {

    const QXmlStreamAttributes attrs(attributes());

    Q_ASSERT(m_currentTextStyleProperties == 0);
    m_currentTextStyleProperties = new KoCharacterStyle();

    // Number 3 makes eg. lvl4 -> 4
    m_currentListLevel = QString(level.at(3)).toInt();

    m_currentBulletProperties = m_currentCombinedBulletProperties[m_currentListLevel];
    Q_ASSERT(m_currentListLevel > 0);
    m_currentBulletProperties.m_level = m_currentListLevel;

    TRY_READ_ATTR_WITHOUT_NS(marL)
    TRY_READ_ATTR_WITHOUT_NS(marR)
    TRY_READ_ATTR_WITHOUT_NS(indent)
    TRY_READ_ATTR_WITHOUT_NS(defTabSz)

    m_currentParagraphStyle = KoGenStyle(KoGenStyle::ParagraphAutoStyle, "paragraph");
    m_currentTextStyle = KoGenStyle(KoGenStyle::TextAutoStyle, "text");

#ifdef PPTXXMLSLIDEREADER_CPP
    inheritTextStyle(m_currentTextStyle);
#endif

    // Following settings are only applied if defined so they don't overwrite
    // defaults defined in {slideLayout, slideMaster, defaultStyles}.
    if (!marL.isEmpty()) {
        qreal marLeft;
        STRING_TO_QREAL(marL, marLeft, "attr:marL")
        marLeft = EMU_TO_POINT(marLeft);
        m_currentParagraphStyle.addPropertyPt("fo:margin-left", marLeft);
        m_currentBulletProperties.setMargin(marLeft);
    }
    if (!indent.isEmpty()) {
        qreal firstInd;
        STRING_TO_QREAL(indent, firstInd, "attr:indent")
        firstInd = EMU_TO_POINT(firstInd);
        m_currentParagraphStyle.addPropertyPt("fo:text-indent", firstInd);
        m_currentBulletProperties.setIndent(firstInd);
    }
    if (!marR.isEmpty()) {
        qreal marRight;
        STRING_TO_QREAL(marR, marRight, "attr:marR")
        m_currentParagraphStyle.addPropertyPt("fo:margin-right", EMU_TO_POINT(marRight));
    }
    if (!defTabSz.isEmpty()) {
        qreal tabSize;
        STRING_TO_QREAL(defTabSz, tabSize, "attr:defTabSz")
        m_currentParagraphStyle.addPropertyPt("style:tab-stop-distance", EMU_TO_POINT(tabSize));
    }

    TRY_READ_ATTR_WITHOUT_NS(algn)
    algnToODF("fo:text-align", algn);

    while (!atEnd()) {
        readNext();
        debugMsooXml << *this;
        if (isEndElement() && qualifiedName() == QString("a:%1").arg(level)) {
            break;
        }
        if (isStartElement()) {
            TRY_READ_IF(defRPr) // fills m_currentTextStyleProperties
            ELSE_TRY_READ_IF(buNone)
            ELSE_TRY_READ_IF(buAutoNum)
            ELSE_TRY_READ_IF(buChar)
            ELSE_TRY_READ_IF(buFont)
            ELSE_TRY_READ_IF(buBlip)
            ELSE_TRY_READ_IF(buClr)
            ELSE_TRY_READ_IF(buClrTx)
            ELSE_TRY_READ_IF(buSzPct)
            ELSE_TRY_READ_IF(buSzPts)
            else if (QUALIFIED_NAME_IS(buSzTx)) {
                m_currentBulletProperties.setBulletRelativeSize(100);
            }
            else if (QUALIFIED_NAME_IS(spcBef)) {
                m_currentSpacingType = spacingMarginTop;
                TRY_READ(spcBef)
            }
            else if (QUALIFIED_NAME_IS(spcAft)) {
                m_currentSpacingType = spacingMarginBottom;
                TRY_READ(spcAft)
            }
            else if (QUALIFIED_NAME_IS(lnSpc)) {
                m_currentSpacingType = spacingLines;
                TRY_READ(lnSpc)
            }
            SKIP_UNKNOWN
        }
    }

    m_currentTextStyleProperties->saveOdf(m_currentTextStyle);

    m_currentCombinedParagraphStyles[m_currentListLevel] = m_currentParagraphStyle;
    m_currentCombinedTextStyles[m_currentListLevel] = m_currentTextStyle;
    m_currentCombinedBulletProperties[m_currentListLevel] = m_currentBulletProperties;

    delete m_currentTextStyleProperties;
    m_currentTextStyleProperties = 0;

    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL lvl1pPr
//! List level 1 text style
/*!

 Parent elements:
  - [done] bodyStyle (§19.3.1.5)
  - defaultTextStyle (§19.2.1.8)
  - [done] lstStyle (§21.1.2.4.12)
  - notesStyle (§19.3.1.28)
  - [done] otherStyle (§19.3.1.35)
  - [done] titleStyle (§19.3.1.49)

 Child elements:
  - [done] buAutoNum (Auto-Numbered Bullet)     §21.1.2.4.1
  - [done] buBlip (Picture Bullet)              §21.1.2.4.2
  - [done] buChar (Character Bullet)            §21.1.2.4.3
  - [done] buClr (Color Specified)              §21.1.2.4.4
  - [done] buClrTx (Follow Text)                §21.1.2.4.5
  - [done] buFont (Specified)                   §21.1.2.4.6
  - buFontTx (Follow text)               §21.1.2.4.7
  - [done] buNone (No Bullet)                   §21.1.2.4.8
  - [done] buSzPct (Bullet Size Percentage)     §21.1.2.4.9
  - [done] buSzPts (Bullet Size Points)         §21.1.2.4.10
  - [done] buSzTx (Bullet Size Follows Text)    §21.1.2.4.11
  - [done] defRPr (Default Text Run Properties) §21.1.2.3.2
  - extLst (Extension List)              §20.1.2.2.15
  - [done] lnSpc (Line Spacing)                 §21.1.2.2.5
  - [done] spcAft (Space After)                 §21.1.2.2.9
  - [done] spcBef (Space Before)                §21.1.2.2.10
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
#define CURRENT_EL buBlip
//! buBlip - bullet picture
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
 - [done] blip
*/
//! @todo support all attributes
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_buBlip()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    m_xlinkHref.clear();

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(blip)
            ELSE_WRONG_FORMAT
        }
    }

    if (!m_xlinkHref.isEmpty()) {
        m_currentBulletProperties.setPicturePath(m_xlinkHref);
        m_listStylePropertiesAltered = true;
    }

    m_xlinkHref.clear();

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
        m_currentBulletProperties.setBulletChar(attrs.value("char").toString());
        // if such a char is defined then we have actually a list-item even if OOXML doesn't handle them as such
    }

    m_listStylePropertiesAltered = true;

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL buClr
//! buClr - bullet color
/*!
 Parent elements:
 - defPPr  (§21.1.2.2.2)
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
 - [done] hslClr (Hue, Saturation, Luminance Color Model) §20.1.2.3.13
 - [done] prstClr (Preset Color) §20.1.2.3.22
 - [done]schemeClr (Scheme Color) §20.1.2.3.29
 - [done] scrgbClr (RGB Color Model - Percentage Variant) §20.1.2.3.30
 - [done]srgbClr (RGB Color Model - Hex Variant) §20.1.2.3.32
 - [done] sysClr (System Color) §20.1.2.3.33
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_buClr()
{
    READ_PROLOGUE

#ifdef PPTXXMLDOCUMENTREADER_CPP
    m_colorState = PptxXmlDocumentReader::buClrState;
#endif

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(srgbClr)
            ELSE_TRY_READ_IF(schemeClr)
            ELSE_TRY_READ_IF(scrgbClr)
            ELSE_TRY_READ_IF(sysClr)
            ELSE_TRY_READ_IF(prstClr)
            ELSE_TRY_READ_IF(hslClr)
            ELSE_WRONG_FORMAT
        }
    }
    if (m_currentColor.isValid()) {
	m_currentBulletProperties.setBulletColor(m_currentColor.name());
        m_currentColor = QColor();
    	m_listStylePropertiesAltered = true;
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL buClrTx
//! buClrTx - follow text
/*!
 Parent elements:
 - defPPr  (§21.1.2.2.2)
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
 - none
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_buClrTx()
{
    READ_PROLOGUE
    m_currentBulletProperties.setBulletColor("UNUSED");
    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL buSzPct
//! buSzPct (Bullet Size Percentage) ECMA-376, 21.1.2.4.9, p.3638
/*!
 Parent elements:
 - defPPr  (§21.1.2.2.2)
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
 - none
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_buSzPct()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(val)

    if (!val.isEmpty()) {
        m_currentBulletProperties.setBulletRelativeSize(val.toInt()/1000);
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL buSzPts
//! buSzPts (Bullet Size Points) ECMA-376, 21.1.2.4.10, p.3639
/*!
 Parent elements:
 - defPPr  (§21.1.2.2.2)
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
 - none
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_buSzPts()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(val)

    if (!val.isEmpty()) {
        m_currentBulletProperties.setBulletSizePt(val.toInt()/1000);
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL buFont
//! buFont - bullet font
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
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_buFont()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR_WITHOUT_NS(typeface)

    if (!typeface.isEmpty()) {
        m_currentBulletProperties.setBulletFont(attrs.value("typeface").toString());
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
    m_currentBulletProperties.setBulletChar("");
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
        if (type == "alphaLcParenBoth") {
            m_currentBulletProperties.setPrefix("(");
            m_currentBulletProperties.setSuffix(")");
            m_currentBulletProperties.setNumFormat("a");
        }
        else if (type == "alphaLcParenR") {
            m_currentBulletProperties.setSuffix(")");
            m_currentBulletProperties.setNumFormat("a");
        }
        else if (type == "alphaLcPeriod") {
            m_currentBulletProperties.setSuffix(".");
            m_currentBulletProperties.setNumFormat("a");
        }
        else if (type == "alphaUcParenBoth") {
            m_currentBulletProperties.setPrefix("(");
            m_currentBulletProperties.setSuffix(")");
            m_currentBulletProperties.setNumFormat("A");
        }
        else if (type == "alphaUcParenR") {
            m_currentBulletProperties.setSuffix(")");
            m_currentBulletProperties.setNumFormat("A");
        }
        else if (type == "alphaUcPeriod") {
            m_currentBulletProperties.setSuffix(".");
            m_currentBulletProperties.setNumFormat("A");
        }
        else if (type == "arabicParenBoth") {
            m_currentBulletProperties.setPrefix("(");
            m_currentBulletProperties.setSuffix(")");
            m_currentBulletProperties.setNumFormat("1");
        }
        else if (type == "arabicParenR") {
            m_currentBulletProperties.setSuffix(")");
            m_currentBulletProperties.setNumFormat("1");
        }
        else if (type == "arabicPeriod") {
            m_currentBulletProperties.setSuffix(".");
            m_currentBulletProperties.setNumFormat("1");
        }
        else if (type == "arabicPlain") {
            m_currentBulletProperties.setNumFormat("1");
        }
        else if (type == "romanLcParenBoth") {
            m_currentBulletProperties.setPrefix("(");
            m_currentBulletProperties.setSuffix(")");
            m_currentBulletProperties.setNumFormat("i");
        }
        else if (type == "romanLcParenR") {
            m_currentBulletProperties.setSuffix(")");
            m_currentBulletProperties.setNumFormat("i");
        }
        else if (type == "romanLcPeriod") {
            m_currentBulletProperties.setSuffix(".");
            m_currentBulletProperties.setNumFormat("i");
        }
        else if (type == "romanUcParenBoth") {
            m_currentBulletProperties.setPrefix("(");
            m_currentBulletProperties.setSuffix(")");
            m_currentBulletProperties.setNumFormat("I");
        }
        else if (type == "romanUcParenR") {
            m_currentBulletProperties.setSuffix(")");
            m_currentBulletProperties.setNumFormat("I");
        }
        else if (type == "romanUcPeriod") {
            m_currentBulletProperties.setSuffix(".");
            m_currentBulletProperties.setNumFormat("I");
        } else {
            m_currentBulletProperties.setSuffix(".");
            m_currentBulletProperties.setNumFormat("i");
        }
    }

    TRY_READ_ATTR_WITHOUT_NS(startAt)
    if (!startAt.isEmpty()) {
        m_currentBulletProperties.setStartValue(startAt);
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

    TRY_READ_ATTR_WITHOUT_NS(type)

    MSOOXML::Utils::XmlWriteBuffer fldBuf;
    body = fldBuf.setWriter(body);

    QString textStyleName;

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            if (QUALIFIED_NAME_IS(rPr)) {
                m_currentTextStyleProperties = new KoCharacterStyle();
                m_currentTextStyle = KoGenStyle(KoGenStyle::TextAutoStyle, "text");

#ifdef PPTXXMLSLIDEREADER_CPP
                if (m_context->type == SlideMaster || m_context->type == NotesMaster) {
                    m_currentTextStyle.setAutoStyleInStylesDotXml(true);
                }
#elif defined DOCXXMLDOCREADER_CPP
                if (m_moveToStylesXml) {
                    m_currentTextStyle.setAutoStyleInStylesDotXml(true);
                }
#endif
#ifdef PPTXXMLSLIDEREADER_CPP
                inheritTextStyle(m_currentTextStyle);
#endif
                KoGenStyle::copyPropertiesFromStyle(m_referredFont, m_currentTextStyle, KoGenStyle::TextType);

                TRY_READ(DrawingML_rPr)

                m_currentTextStyleProperties->saveOdf(m_currentTextStyle);
                textStyleName = mainStyles->insert(m_currentTextStyle);

                delete m_currentTextStyleProperties;
                m_currentTextStyleProperties = 0;
            }
            else if (QUALIFIED_NAME_IS(pPr)) {
                TRY_READ(DrawingML_pPr)
            }
            ELSE_TRY_READ_IF(t)
            ELSE_WRONG_FORMAT
        }
    }

    QString fontSize = m_currentTextStyle.property("fo:font-size");
#ifdef PPTXXMLSLIDEREADER_CPP
    if (fontSize.isEmpty()) {
        m_currentTextStyle.addPropertyPt("fo:font-size", TEXT_FONTSIZE_DEFAULT);
        fontSize = QString("%1").arg(TEXT_FONTSIZE_DEFAULT);
    }
#endif
    if (!fontSize.isEmpty()) {
        fontSize.remove("pt");
        qreal realSize = fontSize.toDouble();
        if (realSize > m_maxParaFontPt) {
            m_maxParaFontPt = realSize;
        }
        if (realSize < m_minParaFontPt) {
            m_minParaFontPt = realSize;
        }
    }

    body = fldBuf.originalWriter();

//! @todo support all possible fields here

    body->startElement("text:span", false);
    body->addAttribute("text:style-name", textStyleName);

    if (type == "slidenum") {
        body->startElement("text:page-number");
        body->addAttribute("text:select-page", "current");
    } else {
        body->startElement("text:date");
    }

    (void)fldBuf.releaseWriter();

    body->endElement(); //text:page-number, some date format
    body->endElement(); //text:span

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

 - [done] spcPct (Spacing Percent) §21.1.2.2.11
 - [done] spcPts (Spacing Points)  §21.1.2.2.12

*/
//! @todo support all attributes
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_spcBef()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(spcPts)
            ELSE_TRY_READ_IF(spcPct)
            ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL spcAft
//! spcAft - spacing after
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

 - [done] spcPct (Spacing Percent) §21.1.2.2.11
 - [done] spcPts (Spacing Points)  §21.1.2.2.12

*/
//! @todo support all attributes
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_spcAft()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(spcPts)
            ELSE_TRY_READ_IF(spcPct)
            ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL lnSpc
//! lnSpc - line spacing
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

 - [done] spcPct (Spacing Percent) §21.1.2.2.11
 - [done] spcPts (Spacing Points)  §21.1.2.2.12

*/
//! @todo support all attributes
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_lnSpc()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(spcPct)
            ELSE_TRY_READ_IF(spcPts)
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL spcPts
//! spcPts (Spacing Points), ECMA-376, DrawingML 21.1.2.2.12, p.3600
/*!
 Parent elements:
 - [done] lnSpc (§21.1.2.2.5)
 - [done] spcAft (§21.1.2.2.9)
 - [done] spcBef (§21.1.2.2.10)
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_spcPts()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR_WITHOUT_NS(val)

    int margin = 0;
    STRING_TO_INT(val, margin, "attr:val")

    switch (m_currentSpacingType) {
    case (spacingMarginTop):
        m_currentParagraphStyle.addPropertyPt("fo:margin-top", margin/100.0);
        break;
    case (spacingMarginBottom):
        m_currentParagraphStyle.addPropertyPt("fo:margin-bottom", margin/100.0);
        break;
    case (spacingLines):
        m_currentParagraphStyle.addPropertyPt("fo:line-height", margin/100.0);
        break;
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL spcPct
//! spcPct (Spacing Percent), ECMA-376, DrawingML 21.1.2.2.11, p.3599
/*!
 Parent elements:
 - [done] lnSpc (§21.1.2.2.5)
 - [done] spcAft (§21.1.2.2.9)
 - [done] spcBef (§21.1.2.2.10)
*/
//! @todo support all attributes
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_spcPct()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR_WITHOUT_NS(val)

    int lineSpace = 0;
    STRING_TO_INT(val, lineSpace, "attr:val")

    QString space = "%1";
    space = space.arg(lineSpace/1000.0);
    space.append('%');

    switch (m_currentSpacingType) {
    case (spacingMarginTop):
        m_currentParagraphStyle.addProperty("fo:margin-top", space);
        break;
    case (spacingMarginBottom):
        m_currentParagraphStyle.addProperty("fo:margin-bottom", space);
        break;
    case (spacingLines):
        m_currentParagraphStyle.addProperty("fo:line-height", space);
        break;
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL defRPr
//! defRPr - Default Text Run Properties
/*! ECMA-376, 21.1.2.3.2, p.3597

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
     - [done] gradFill (Gradient Fill)                 §20.1.8.33
     - grpFill (Group Fill)                            §20.1.8.35
     - highlight (Highlight Color)                     §21.1.2.3.4
     - hlinkClick (Click Hyperlink)                    §21.1.2.3.5
     - hlinkMouseOver (Mouse-Over Hyperlink)           §21.1.2.3.6
     - [done] latin (Latin Font)                       §21.1.2.3.7
     - ln (Outline)                                    §20.1.2.2.24
     - [done] noFill (No Fill)                         §20.1.8.44
     - pattFill (Pattern Fill)                         §20.1.8.47
     - rtl (Right to Left Run)                         §21.1.2.2.8
     - [done] solidFill (Solid Fill)                   §20.1.8.54
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

    m_currentColor = QColor();

#ifdef PPTXXMLDOCUMENTREADER_CPP
    m_colorState = PptxXmlDocumentReader::defRPrState;
#endif

    while (!atEnd()) {
        readNext();
        debugMsooXml << *this;
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(solidFill)
            else if (name() == "gradFill") {
                TRY_READ(gradFillRpr)
            }
            else if (name() == "noFill") {
                m_currentTextStyleProperties->setTextOutline(QPen(Qt::SolidLine));
            }
            ELSE_TRY_READ_IF(latin)
            SKIP_UNKNOWN
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    if (m_currentColor.isValid()) {
        m_currentTextStyle.addProperty("fo:color", m_currentColor.name());
        m_currentColor = QColor();
    }

    handleRprAttributes(attrs);

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL bodyPr
//! bodyPr handler (Body Properties)
/*! ECMA-376, 21.1.2.1.1, p.3556 - DrawingML

 This element defines the body properties for the text body within a
 shape.

 Parent elements:
 - lnDef (§20.1.4.1.20)
 - rich (§21.2.2.156)
 - spDef (§20.1.4.1.27)
 - t (§21.4.3.8)
 - txBody (§21.3.2.26)
 - txBody(§20.1.2.2.40)
 - txBody (§20.5.2.34)
 - [done] txBody (§19.3.1.51)
 - txDef (§20.1.4.1.28)
 - txPr (§21.2.2.216)

 Child elements:
 - extLst (Extension List) §20.1.2.2.15
 - flatTx (No text in 3D scene) §20.1.5.8
 - noAutofit (No AutoFit) §21.1.2.1.2
 - [done] normAutofit (Normal AutoFit) §21.1.2.1.3
 - [done] prstTxWarp (Preset Text Warp) §20.1.9.19
 - scene3d (3D Scene Properties) §20.1.4.1.26
 - sp3d (Apply 3D shape properties) §20.1.5.12
 - [done] spAutoFit (Shape AutoFit) §21.1.2.1.4

*/
//! @todo support all attributes
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_bodyPr()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR_WITHOUT_NS(anchor)
    TRY_READ_ATTR_WITHOUT_NS(lIns)
    TRY_READ_ATTR_WITHOUT_NS(rIns)
    TRY_READ_ATTR_WITHOUT_NS(bIns)
    TRY_READ_ATTR_WITHOUT_NS(tIns)
    TRY_READ_ATTR_WITHOUT_NS(vert)
    TRY_READ_ATTR_WITHOUT_NS(wrap)

    //TODO:
    /* TRY_READ_ATTR_WITHOUT_NS(fontAlgn) */

    //TODO:
    /* if (!vert.isEmpty()) { */
    /*     if (vert == "vert270") { */
    /*     } */
    /* } */

    m_shapeTextPosition.clear();
    m_shapeTextTopOff.clear();
    m_shapeTextBottomOff.clear();
    m_shapeTextLeftOff.clear();
    m_shapeTextRightOff.clear();

    if (!lIns.isEmpty()) {
        m_shapeTextLeftOff = lIns;
    }
    if (!rIns.isEmpty()) {
        m_shapeTextRightOff = rIns;
    }
    if (!tIns.isEmpty()) {
        m_shapeTextTopOff = tIns;
    }
    if (!bIns.isEmpty()) {
        m_shapeTextBottomOff = bIns;
    }

    if (!anchor.isEmpty()) {
        if (anchor == "t") {
            m_shapeTextPosition = "top";
        }
        else if (anchor == "b") {
            m_shapeTextPosition = "bottom";
        }
        else if (anchor == "ctr") {
            m_shapeTextPosition = "middle";
        }
        else if (anchor == "just") {
            m_shapeTextPosition = "justify";
        }
    }

//! @todo more attributes

    m_normAutofit =  MSOOXML::Utils::autoFitUnUsed;

    bool spAutoFit = false;
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            if (qualifiedName() == QLatin1String("a:spAutoFit")) {
                TRY_READ(spAutoFit)
                spAutoFit = true;
                m_normAutofit = MSOOXML::Utils::autoFitOn;
            }
            else if (qualifiedName() == QLatin1String("a:normAutofit")) {
                TRY_READ(normAutofit)
                m_normAutofit = MSOOXML::Utils::autoFitOn;
            }
            else if (qualifiedName() == QLatin1String("a:prstTxWarp")) {
                // This element describes text transformation, todo:
            }
            SKIP_UNKNOWN
        }
    }

#ifdef PPTXXMLSLIDEREADER_CPP
    saveBodyProperties();

    const KoGenStyle::PropertyType gt = KoGenStyle::GraphicType;

    m_currentPresentationStyle.addProperty("draw:auto-grow-height",
            spAutoFit ? MsooXmlReader::constTrue : MsooXmlReader::constFalse, gt);

    // If the wrap attribute is omitted, then a value of square is implied.
    if (!spAutoFit || (wrap == QLatin1String("square") || wrap.isEmpty())) {
	m_currentPresentationStyle.addProperty("draw:auto-grow-width", MsooXmlReader::constFalse, gt);
    } else {
	m_currentPresentationStyle.addProperty("draw:auto-grow-width", MsooXmlReader::constTrue, gt);
    }
    // text in shape
    if (wrap == QLatin1String("none")) {
	m_currentPresentationStyle.addProperty("fo:wrap-option", "no-wrap", gt);
    } else {
	m_currentPresentationStyle.addProperty("fo:wrap-option", "wrap", gt);
    }
#else
    Q_UNUSED(spAutoFit);
#endif
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL normAutofit
//! Normal autofit handler (Normal AutoFit)
/*! ECMA-376, 21.1.2.1.3, p.3555 - DrawingML

 Parent elements:
 - [done] bodyPr (§21.1.2.1.1)

 No child elements.
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_normAutofit()
{
    READ_PROLOGUE
    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL spAutoFit
//! spAutoFit handler (Shape AutoFit)
/*! ECMA-376, 21.1.2.1.4, p.3567 - DrawingML

 This element specifies that a shape should be auto-fit to fully contain the text described within it.
 Auto-fitting is when text within a shape is scaled in order to contain all the text inside.
 If this element is omitted, then noAutofit or auto-fit off is implied.

 Parent elements:
 - [done] bodyPr (§21.1.2.1.1)

 No child elements.
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_spAutoFit()
{
    READ_PROLOGUE
    readNext();
    READ_EPILOGUE
}

// ================================================================
//                     Namespace in {a,xdr}
// ================================================================
#undef MSOOXML_CURRENT_NS
#ifdef DRAWINGML_TXBODY_NS
#define MSOOXML_CURRENT_NS DRAWINGML_TXBODY_NS
#else
#define MSOOXML_CURRENT_NS DRAWINGML_NS
#endif

#undef CURRENT_EL
#define CURRENT_EL txBody
//! txBody handler (Shape Text Body)
//! ECMA-376, 20.1.2.2.40, p. 3058
/* This element specifies the existence of text to be contained within
   the corresponding shape.  All visible text and visible text related
   properties are contained within this element.

   Parent Elements:
   ----------------
   PresentationML/SpreadsheetML:
   - [done] sp (§19.3.1.43)/(§20.5.2.29)

   DrawingML:
   - [done] tc (§21.1.3.16)
   - [done] txSp (§20.1.2.2.41)

   Child Elements:
   - [done] bodyPr (Body Properties) §21.1.2.1.1
   - [done] lstStyle (Text List Styles) §21.1.2.4.12
   - [done] p (Text Paragraphs) §21.1.2.2.6

   TODO: There's a separate implementation in the PPTX filter which
   should be merge with this one.

*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_DrawingML_txBody(txBodyCaller caller)
{
    READ_PROLOGUE2(DrawingML_txBody)

    m_prevListLevel = 0;
    m_currentListLevel = 0;
    m_pPr_lvl = 0;
    m_continueListNumbering.clear();
    m_prevListStyleName.clear();

    bool textBoxCreated = false;
    if (caller != DrawingML_txBody_tc) {
        if (!isCustomShape()) {
            body->startElement("draw:text-box");
            textBoxCreated = true;
        }
    }

    while (!atEnd()) {
        readNext();
        debugMsooXml << *this;
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF_NS(a, bodyPr)
            ELSE_TRY_READ_IF_NS(a, lstStyle)
            else if (qualifiedName() == QLatin1String("a:p")) {
                TRY_READ(DrawingML_p)
            }
            SKIP_UNKNOWN
        }
    }
    if (m_prevListLevel > 0) {
        // Ending our current level
        body->endElement(); // text:list
        // Ending any additional levels needed
        for(; m_prevListLevel > 1; --m_prevListLevel) {
            body->endElement(); // text:list-item
            body->endElement(); // text:list
        }
        m_prevListLevel = 0;
    }

    if (textBoxCreated) {
        body->endElement(); //draw:text-box
    }

    READ_EPILOGUE
}

#endif
