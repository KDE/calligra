/*
 * This file is part of Office 2007 Filters for Calligra
 *
 * Copyright (C) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2010 KoGmbh (cbo@kogmbh.com).
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

#include "DocxXmlDocumentReader.h"

#include "DocxXmlHeaderReader.h"
#include "DocxXmlFooterReader.h"
#include "DocxImport.h"
#include "DocxDebug.h"

#include <MsooXmlSchemas.h>
#include <MsooXmlUtils.h>
#include <MsooXmlRelationships.h>
#include <MsooXmlUnits.h>

#include <KoXmlWriter.h>
#include <KoGenStyles.h>
#include <KoOdfGraphicStyles.h>
#include <Charting.h>
#include <KoOdfChartWriter.h>
#include <XlsxXmlChartReader.h>

#include <MsooXmlTableStyle.h>

#define MSOOXML_CURRENT_NS "w"
#define MSOOXML_CURRENT_CLASS DocxXmlDocumentReader
#define BIND_READ_CLASS MSOOXML_CURRENT_CLASS
#define DOCXXMLDOCREADER_CPP

//#define DOCXXML_DEBUG_TABLES

#include <MsooXmlReader_p.h>

#include <KoTable.h>
#include <KoCell.h>
#include <KoRow.h>
#include <KoColumn.h>
#include <KoRawCellChild.h>

#include <QTime>

namespace {

class BorderMap : public QMap<QString, KoBorder::BorderStyle>
{
public:
    BorderMap() {
        insert(QString(), KoBorder::BorderNone);
        insert("nil", KoBorder::BorderNone);
        insert("none", KoBorder::BorderSolid);
        insert("single", KoBorder::BorderSolid);
        insert("thick", KoBorder::BorderSolid); //FIXME find a better representation
        insert("double", KoBorder::BorderDouble);
        insert("dotted", KoBorder::BorderDotted);
        insert("dashed", KoBorder::BorderDashed);
        insert("dotDash", KoBorder::BorderDashDot);
        insert("dotDotDash", KoBorder::BorderDashDotDot);
        insert("triple", KoBorder::BorderDouble); //FIXME
        insert("thinThickSmallGap", KoBorder::BorderSolid); //FIXME
        insert("thickThinSmallGap", KoBorder::BorderSolid); //FIXME
        insert("thinThickThinSmallGap", KoBorder::BorderSolid); //FIXME
        insert("thinThickMediumGap", KoBorder::BorderSolid); //FIXME
        insert("thickThinMediumGap", KoBorder::BorderSolid); //FIXME
        insert("thinThickThinMediumGap", KoBorder::BorderSolid); //FIXME
        insert("thinThickLargeGap", KoBorder::BorderSolid); //FIXME
        insert("thickThinLargeGap", KoBorder::BorderSolid); //FIXME
        insert("thinThickThinLargeGap", KoBorder::BorderSolid); //FIXME
        insert("wave", KoBorder::BorderSolid); //FIXME
        insert("dobleWave", KoBorder::BorderSolid); //FIXME
        insert("dashSmallGap", KoBorder::BorderSolid); //FIXME
        insert("dashDotStroked", KoBorder::BorderSolid); //FIXME
        insert("threeDEmboss", KoBorder::BorderSolid); //FIXME
        insert("threeDEngrave", KoBorder::BorderSolid); //FIXME
        insert("outset", KoBorder::BorderOutset);
        insert("inset", KoBorder::BorderInset);
    }
} borderMap;

}

DocxXmlDocumentReaderContext::DocxXmlDocumentReaderContext(
    DocxImport& _import,
    const QString& _path, const QString& _file,
    MSOOXML::MsooXmlRelationships& _relationships,
    MSOOXML::DrawingMLTheme* _themes)
        : MSOOXML::MsooXmlReaderContext(&_relationships),
        import(&_import), path(_path), file(_file),
        themes(_themes)
{
}

// ---------------------------------------------------------------------


class DocxXmlDocumentReader::Private
{
public:
    Private() {
    }
    ~Private() {
    }
};

DocxXmlDocumentReader::DocxXmlDocumentReader(KoOdfWriters *writers)
        : MSOOXML::MsooXmlCommonReader(writers)
        , m_pDocBkgImageWriter(0)
        , m_writers(writers)
        , d(new Private)
{
    init();
}

DocxXmlDocumentReader::~DocxXmlDocumentReader()
{
    if (m_pDocBkgImageWriter) {
        delete m_pDocBkgImageWriter->device();
        delete m_pDocBkgImageWriter;
    }
    doneInternal(); // MsooXmlCommonReaderImpl.h
    delete d;

    delete m_dropCapWriter;
    m_dropCapWriter = 0;
}

void DocxXmlDocumentReader::init()
{
    initInternal(); // MsooXmlCommonReaderImpl.h
    initDrawingML();
    m_defaultNamespace = QLatin1String(MSOOXML_CURRENT_NS ":");
    m_complexCharType = NoComplexFieldCharType;
    m_complexCharStatus = NoneAllowed;
    m_dropCapStatus = NoDropCap;
    m_dropCapWriter = 0;
    m_currentTableNumber = 0;
    m_wasCaption = false;
    m_closeHyperlink = false;
    m_listFound = false;
    m_insideParagraph = false;
    m_createSectionStyle = false;
    m_createSectionToNext = false;
    m_currentVMLProperties.insideGroup = false;
    m_outputFrames = true;
    m_currentNumId.clear();
    m_prevListLevel = 0;
    qsrand(QTime::currentTime().msec());
}

KoFilter::ConversionStatus DocxXmlDocumentReader::read(MSOOXML::MsooXmlReaderContext* context)
{
    m_context = dynamic_cast<DocxXmlDocumentReaderContext*>(context);
    Q_ASSERT(m_context);

    m_createSectionStyle = true;
    debugDocx << "=============================";
    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }

    //w:document
    readNext();
    debugDocx << *this << namespaceUri();

    if (!expectEl("w:document")) {
        return KoFilter::WrongFormat;
    }
    if (!expectNS(MSOOXML::Schemas::wordprocessingml)) {
        return KoFilter::WrongFormat;
    }
    /*
        const QXmlStreamAttributes attrs( attributes() );
        for (int i=0; i<attrs.count(); i++) {
            debugDocx << "1 NS prefix:" << attrs[i].name() << "uri:" << attrs[i].namespaceUri();
        }*/

    QXmlStreamNamespaceDeclarations namespaces(namespaceDeclarations());
    for (int i = 0; i < namespaces.count(); i++) {
        debugDocx << "NS prefix:" << namespaces[i].prefix() << "uri:" << namespaces[i].namespaceUri();
    }
//! @todo find out whether the namespace returned by namespaceUri()
//!       is exactly the same ref as the element of namespaceDeclarations()
    if (!namespaces.contains(QXmlStreamNamespaceDeclaration("w", MSOOXML::Schemas::wordprocessingml))) {
        raiseError(i18n("Namespace \"%1\" not found", QLatin1String(MSOOXML::Schemas::wordprocessingml)));
        return KoFilter::WrongFormat;
    }
//! @todo expect other namespaces too...

    while (!atEnd()) {
        readNext();
        debugDocx << *this;
        BREAK_IF_END_OF(document)
        if (isStartElement()) {
            TRY_READ_IF(body)
            ELSE_TRY_READ_IF(background)
            SKIP_UNKNOWN
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    if (!expectElEnd("w:document")) {
        return KoFilter::WrongFormat;
    }
    debugDocx << "===========finished============";
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL body
//! w:body handler (Document Body)
/*! ECMA-376, 17.2.2, p. 204.
 This element specifies the contents of a main document part in a WordprocessingML document.

 Parent elements:
 - [done] document (§17.2.3)

 Child elements:
 - altChunk (Anchor for Imported External Content) §17.17.2.1
 - [done] bookmarkEnd (Bookmark End) §17.13.6.1
 - [done] bookmarkStart (Bookmark Start) §17.13.6.2
 - commentRangeEnd (Comment Anchor Range End) §17.13.4.3
 - commentRangeStart (Comment Anchor Range Start) §17.13.4.4
 - customXml (Block-Level Custom XML Element) §17.5.1.6
 - customXmlDelRangeEnd (Custom XML Markup Deletion End) §17.13.5.4
 - customXmlDelRangeStart (Custom XML Markup Deletion Start) §17.13.5.5
 - customXmlInsRangeEnd (Custom XML Markup Insertion End) §17.13.5.6
 - customXmlInsRangeStart (Custom XML Markup Insertion Start) §17.13.5.7
 - customXmlMoveFromRangeEnd (Custom XML Markup Move Source End) §17.13.5.8
 - customXmlMoveFromRangeStart (Custom XML Markup Move Source Start) §17.13.5.9
 - customXmlMoveToRangeEnd (Custom XML Markup Move Destination Location End) §17.13.5.10
 - customXmlMoveToRangeStart (Custom XML Markup Move Destination Location Start) §17.13.5.11
 - [done] del (Deleted Run Content) §17.13.5.14
 - [done] ins (Inserted Run Content) §17.13.5.18
 - moveFrom (Move Source Run Content) §17.13.5.22
 - moveFromRangeEnd (Move Source Location Container - End) §17.13.5.23
 - moveFromRangeStart (Move Source Location Container - Start) §17.13.5.24
 - moveTo (Move Destination Run Content) §17.13.5.25
 - moveToRangeEnd (Move Destination Location Container - End) §17.13.5.27
 - moveToRangeStart (Move Destination Location Container - Start) §17.13.5.28
 - oMath (Office Math) §22.1.2.77
 - oMathPara (Office Math Paragraph) §22.1.2.78
 - [done] p (Paragraph) §17.3.1.22
 - permEnd (Range Permission End) §17.13.7.1
 - permStart (Range Permission Start) §17.13.7.2
 - proofErr (Proofing Error Anchor) §17.13.8.1
 - [done] sdt (Block-Level Structured Document Tag) §17.5.2.29
 - [done] sectPr (Document Final Section Properties) §17.6.17
 - [done] tbl (Table) §17.4.38
*/
//! @todo support all child elements
KoFilter::ConversionStatus DocxXmlDocumentReader::read_body()
{
    READ_PROLOGUE
/*    QXmlStreamNamespaceDeclarations namespaces = namespaceDeclarations();
    for (int i = 0; i < namespaces.count(); i++) {
        debugDocx << "NS prefix:" << namespaces[i].prefix() << "uri:" << namespaces[i].namespaceUri();
    }*/

    int counter = 0;

    while (!atEnd()) {
        readNext();
        debugDocx << *this;
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            if (counter == 40) {
                // set the progress by the position of what was read
                qreal progress = 45 + 55 * device()->pos() / device()->size();
                m_context->import->reportProgress(progress);
                counter = 0;
            }
            ++counter;
            TRY_READ_IF(p)
            ELSE_TRY_READ_IF(del)
            ELSE_TRY_READ_IF(ins)
            ELSE_TRY_READ_IF(sdt)
            ELSE_TRY_READ_IF(sectPr)
            ELSE_TRY_READ_IF(tbl)
            ELSE_TRY_READ_IF(bookmarkStart)
            ELSE_TRY_READ_IF(bookmarkEnd)
            SKIP_UNKNOWN
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL sectPr
//! w:sectPr handler (Section Properties)
/*! ECMA-376, 17.6.17, p. 667.

 Parent elements:
 - [done] body (§17.2.2)
 - docPartBody (§17.12.6)

 Chils elements:
 - [done] bidi (Right to Left Section Layout) §17.6.1
 - [done] cols (Column Definitions) §17.6.4
 - docGrid (Document Grid) §17.6.5
 - [done] endnotePr (Section-Wide Endnote Properties) §17.11.5
 - [done] footerReference (Footer Reference) §17.10.2
 - [done] footnotePr (Section-Wide Footnote Properties) §17.11.11
 - formProt (Only Allow Editing of Form Fields) §17.6.6
 - [done] headerReference (Header Reference) §17.10.5
 - [done] lnNumType (Line Numbering Settings) §17.6.8
 - noEndnote (Suppress Endnotes In Document) §17.11.16
 - paperSrc (Paper Source Information) §17.6.9
 - [done] pgBorders (Page Borders) §17.6.10
 - [done] pgMar (Page Margins) §17.6.11
 - pgNumType (Page Numbering Settings) §17.6.12
 - [done] pgSz (Page Size) §17.6.13
 - printerSettings (Reference to Printer Settings Data) §17.6.14
 - rtlGutter (Gutter on Right Side of Page) §17.6.16
 - sectPrChange (Revision Information for Section Properties) §17.13.5.32
 - [done] textDirection (Text Flow Direction) §17.6.20
 - titlePg (Different First Page Headers and Footers) §17.10.6
 - type (Section Type) §17.6.22
 - vAlign (Vertical Text Alignment on Page) §17.6.23
*/
//! @todo support all elements
KoFilter::ConversionStatus DocxXmlDocumentReader::read_sectPr()
{
    READ_PROLOGUE

    m_footerActive = false;
    m_headerActive = false;

    m_currentPageStyle = KoGenStyle(KoGenStyle::PageLayoutStyle);
    m_currentPageStyle.setAutoStyleInStylesDotXml(true);
    m_currentPageStyle.addProperty("style:writing-mode", "lr-tb");
//! @todo handle all valued of style:print-orientation
    m_currentPageStyle.addProperty("style:print-orientation", "portrait");
    if (m_backgroundColor.isValid())
        m_currentPageStyle.addProperty("fo:background-color", m_backgroundColor.name());
    if (m_pDocBkgImageWriter) {
        QString contents = QString::fromUtf8(((QBuffer*)m_pDocBkgImageWriter->device())->buffer(),
                                     ((QBuffer*)m_pDocBkgImageWriter->device())->buffer().size());
        m_currentPageStyle.addChildElement("0", contents);
    }

    m_masterPageStyle = KoGenStyle(KoGenStyle::MasterPageStyle);

    m_footers.clear();
    m_headers.clear();

    m_pageMargins.clear();
    m_pageBorderStyles.clear();
    m_pageBorderPaddings.clear();
    m_pageBorderOffsetFrom = "text";

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(pgSz)
            ELSE_TRY_READ_IF(pgMar)
            ELSE_TRY_READ_IF(pgBorders)
            ELSE_TRY_READ_IF(textDirection)
            ELSE_TRY_READ_IF(headerReference)
            ELSE_TRY_READ_IF(footerReference)
            ELSE_TRY_READ_IF(cols)
            ELSE_TRY_READ_IF(footnotePr)
            ELSE_TRY_READ_IF(endnotePr)
            ELSE_TRY_READ_IF(lnNumType)
            else if (name() == "bidi") {
                m_currentPageStyle.addProperty("style:writing-mode", "rl-tb");
            }
            SKIP_UNKNOWN
        }
    }

    applyPageBorders(m_currentPageStyle, m_pageMargins, m_pageBorderStyles, m_pageBorderPaddings, m_pageBorderOffsetFrom);

    // Currently if there are 3 header/footer styles, the one with 'first' is ignored
    bool useEvenAndOddHeaders = false;
    if (m_context->import->documentSettings().contains("evenAndOddHeaders")) {
        QString val = m_context->import->documentSettings()["evenAndOddHeaders"].toString();
        useEvenAndOddHeaders = (val != "off" && val != "0" && val != "false");
    }
    // the numbering in addChildElement is needed to correctly sort the element when writing out the values.
    if (!m_headers.isEmpty()) {
        bool odd = false;
        if (useEvenAndOddHeaders && !m_headers["even"].isEmpty()) {
            m_masterPageStyle.addChildElement("2 style:header-left", m_headers["even"]);
        }
        if (!m_headers["default"].isEmpty()) {
            odd = true;
            m_masterPageStyle.addChildElement("1 style:header", m_headers["default"]);
        }
        if (!odd) {
            m_masterPageStyle.addChildElement("1 style:header", m_headers["first"]);
        }
    }

    if (!m_footers.isEmpty()) {
        bool odd = false;
        if (useEvenAndOddHeaders && !m_footers["even"].isEmpty()) {
            m_masterPageStyle.addChildElement("4 style:footer-left", m_footers["even"]);
        }
        if (!m_footers["default"].isEmpty()) {
            odd = true;
            m_masterPageStyle.addChildElement("3 style:footer", m_footers["default"]);
        }
        if (!odd) {
           m_masterPageStyle.addChildElement("3 style:footer", m_footers["first"]);
        }
    }

    QString pageLayoutStyleName("Mpm");
    pageLayoutStyleName = mainStyles->insert(m_currentPageStyle, pageLayoutStyleName);

    QString masterStyleName("Standard");
//! @todo style:display-name
//    masterStyle->addAttribute("style:display-name", masterStyleName);
    m_masterPageStyle.addAttribute("style:page-layout-name", pageLayoutStyleName);
    QString currentMasterPageName = mainStyles->insert(m_masterPageStyle, masterStyleName);

    // Because sectPr is always in the last bit of a section, it means that next paragraph/whatever
    // Needs to have a style which is modified by next sectPr
    m_createSectionToNext = true;

    KoGenStyle *sectionStyle = mainStyles->styleForModification(m_currentSectionStyleName, m_currentSectionStyleFamily);
    // There might not be a style to modify if the document is completely empty
    if (sectionStyle) {
        sectionStyle->addAttribute("style:master-page-name", currentMasterPageName);
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL pgSz
//! w:pgSz handler (Page Size)
/*! ECMA-376, 17.6.13, p. 655.

 Parent elements:
 - [done] sectPr (§17.6.17)
 - [done] sectPr (§17.6.18)
 - [done] sectPr (§17.6.19)

 No child elements.
*/
//! @todo support all elements
KoFilter::ConversionStatus DocxXmlDocumentReader::read_pgSz()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR(w)
    if (!w.isEmpty()) {
        const QString s(MSOOXML::Utils::TWIP_to_ODF(w));
        if (!s.isEmpty()) {
            m_currentPageStyle.addProperty("fo:page-width", s);
        }
    }
    TRY_READ_ATTR(h)
    if (!h.isEmpty()) {
        const QString s(MSOOXML::Utils::TWIP_to_ODF(h));
        if (!s.isEmpty()) {
            m_currentPageStyle.addProperty("fo:page-height", s);
        }
    }
    TRY_READ_ATTR(orient)
    if (!orient.isEmpty()) {
        m_currentPageStyle.addProperty("style:print-orientation", orient);
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL textDirection
//! w:textDirection handler (Text Direction)
/*!

 Parent elements:
 - [done] sectPr (§17.6.17)
 - [done] sectPr (§17.6.18)
 - [done] sectPr (§17.6.19)

 No child elements.
*/
//! @todo support all elements
KoFilter::ConversionStatus DocxXmlDocumentReader::read_textDirection()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR(val)

    if (!val.isEmpty() && val.size() == 4) {
        const QString left = val.left(2).toLower();
        const QString right = val.right(2).toLower();
        m_currentPageStyle.addProperty("style:writing-mode", left + '-' + right);
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL pgMar
//! w:pgMar handler (Page Margins)
/*! ECMA-376, 17.6.11, p. 649.

 Parent elements:
 - [done] sectPr (§17.6.17)
 - [done] sectPr (§17.6.18)
 - [done] sectPr (§17.6.19)

 No child elements.
*/
//! @todo support all elements
KoFilter::ConversionStatus DocxXmlDocumentReader::read_pgMar()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR(right)
    if (!right.isEmpty()) {
        int rightNum = 0;
        STRING_TO_INT(right, rightNum, QString("w:right"));
        m_pageMargins.insert(MarginRight,TWIP_TO_POINT(rightNum));
    }
    TRY_READ_ATTR(left)
    if (!left.isEmpty()) {
        int leftNum = 0;
        STRING_TO_INT(left, leftNum, QString("w:left"));
        m_pageMargins.insert(MarginLeft,TWIP_TO_POINT(leftNum));
    }

    TRY_READ_ATTR(footer)
    TRY_READ_ATTR(header)
    TRY_READ_ATTR(top)
    TRY_READ_ATTR(bottom)
    int topNum = 0;
    int bottomNum = 0;
    int headerNum = 0;
    int footerNum = 0;
    topNum = top.toInt();
    bottomNum = bottom.toInt();
    headerNum = header.toInt();
    footerNum = footer.toInt();
    if (!m_headerActive) {
        m_pageMargins.insert(MarginTop, TWIP_TO_POINT(topNum));
    }
    else {
        m_pageMargins.insert(MarginTop, TWIP_TO_POINT(headerNum));
    }
    if (!m_footerActive) {
        m_pageMargins.insert(MarginBottom, TWIP_TO_POINT(bottomNum));
    }
    else {
        m_pageMargins.insert(MarginBottom, TWIP_TO_POINT(footerNum));
    }

    QBuffer headerBuffer;
    headerBuffer.open( QIODevice::WriteOnly );
    KoXmlWriter headerWriter(&headerBuffer, 3);
    headerWriter.startElement("style:header-style");
    headerWriter.startElement("style:header-footer-properties");
    headerWriter.addAttribute("style:dynamic-spacing", "true");
    if (m_headerActive) {
        if (topNum > headerNum) {
            headerWriter.addAttributePt("fo:min-height", TWIP_TO_POINT(topNum - headerNum));
        }
    }
    headerWriter.endElement(); // style:header-footer-properties
    headerWriter.endElement(); // style:header-style
    QString headerContents = QString::fromUtf8(headerBuffer.buffer(), headerBuffer.buffer().size() );
    // the style elements need to be sorted correctly so that the created odf is valid
    // therefore we use footer-header-style-1 and footer-header-style-2 to sort header before footer
    m_currentPageStyle.addStyleChildElement("footer-header-style-1", headerContents);

    QBuffer footerBuffer;
    footerBuffer.open( QIODevice::WriteOnly );
    KoXmlWriter footerWriter(&footerBuffer, 3);
    footerWriter.startElement("style:footer-style");
    footerWriter.startElement("style:header-footer-properties");
    footerWriter.addAttribute("style:dynamic-spacing", "true");
    if (m_footerActive) {
        if (bottomNum > footerNum) {
            footerWriter.addAttributePt("fo:min-height", TWIP_TO_POINT(bottomNum - footerNum));
        }
    }
    footerWriter.endElement(); // style:header-footer-properties
    footerWriter.endElement(); // style:footer-style
    QString footerContents = QString::fromUtf8(footerBuffer.buffer(), footerBuffer.buffer().size() );
    m_currentPageStyle.addStyleChildElement("footer-header-style-2", footerContents);

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL pict
//! pict handler (VML Object)
/*! ECMA-376 Part 4, 9.2.2.2, p.31.

 This element specifies that an object is located at this position
 in the run’s contents. The layout properties of this object are specified using the VML syntax (§14.1).

 Parent elements:
 - r (Part 1, §22.1.2.87) - Shared ML
 - [done] r (Part 1, §17.3.2.25) - Shared ML

 Child elements:
 - control (Floating Embedded Control) §9.2.2.1
 - movie (Embedded Video) Part 1, §17.3.3.17
 - Any element in the urn:schemas-microsoft-com:vml namespace §14.1
 - Any element in the urn:schemas-microsoft-com:office:office namespace §14.2
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_pict()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF_NS(v, rect)
            ELSE_TRY_READ_IF_NS(v, roundrect)
            ELSE_TRY_READ_IF_NS(v, shapetype)
            ELSE_TRY_READ_IF_NS(v, shape)
            ELSE_TRY_READ_IF_NS(v, group)
            ELSE_TRY_READ_IF_NS(v, oval)
            ELSE_TRY_READ_IF_NS(v, line)
            SKIP_UNKNOWN
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL footerReference
//! w:footerReference handler (Footer Reference)
/*!

 Parent elements:
 - [done] sectPr (§17.6.17)
 - [done] sectPr (§17.6.8)

 Child elements:
 - None

*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_footerReference()
{
    READ_PROLOGUE

    m_footerActive = true;

    const QXmlStreamAttributes attrs(attributes());

    QString link_target;

    TRY_READ_ATTR_WITH_NS(r, id)
    if (r_id.isEmpty()) {
        link_target.clear();
    }
    else {
        link_target = m_context->relationships->target(m_context->path, m_context->file, r_id);
    }

    DocxXmlFooterReader reader(this);

    QString errorMessage;

    MSOOXML::MsooXmlRelationships relationships(*m_context->import, m_writers, errorMessage);

    QString fileName = link_target;
    fileName.remove(0, m_context->path.length());

    DocxXmlDocumentReaderContext context(*m_context->import, m_context->path, fileName,
        relationships, m_context->themes);
    context.m_tableStyles = m_context->m_tableStyles;
    context.m_bulletStyles = m_context->m_bulletStyles;
    context.m_namedDefaultStyles = m_context->m_namedDefaultStyles;

    const KoFilter::ConversionStatus status
        = m_context->import->loadAndParseDocument(&reader, link_target, errorMessage, &context);
    if (status != KoFilter::OK) {
        reader.raiseError(errorMessage);
    }

    QString footerContent;

    TRY_READ_ATTR(type)
    if (!type.isEmpty()) {
        if (type == "even") {
             footerContent = "<style:footer-left>";
             footerContent.append(reader.content());
             footerContent.append("</style:footer-left>");
        }
        else {
            footerContent = "<style:footer>";
            footerContent.append(reader.content());
            footerContent.append("</style:footer>");
        }
        m_footers[type] = footerContent;
    }
    else {
        footerContent = "<style:footer>";
        footerContent.append(reader.content());
        footerContent.append("</style:footer>");
        m_footers["default"] = footerContent;
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL headerReference
//! w:headerReference handler (Header Reference)
/*!

 Parent elements:
 - [done] sectPr (§17.6.17)
 - [done] sectPr (§17.6.8)

 Child elements:
 - None

*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_headerReference()
{
    READ_PROLOGUE

    m_headerActive = true;

    const QXmlStreamAttributes attrs(attributes());

    QString link_target;

    TRY_READ_ATTR_WITH_NS(r, id)
    if (r_id.isEmpty()) {
        link_target.clear();
    }
    else {
        link_target = m_context->relationships->target(m_context->path, m_context->file, r_id);
    }

    DocxXmlHeaderReader reader(this);

    QString errorMessage;

    MSOOXML::MsooXmlRelationships relationships(*m_context->import, m_writers, errorMessage);

    QString fileName = link_target;
    fileName.remove(0, m_context->path.length());

    DocxXmlDocumentReaderContext context(*m_context->import, m_context->path, fileName,
        relationships, m_context->themes);
    context.m_tableStyles = m_context->m_tableStyles;
    context.m_bulletStyles = m_context->m_bulletStyles;
    context.m_namedDefaultStyles = m_context->m_namedDefaultStyles;

    const KoFilter::ConversionStatus status
        = m_context->import->loadAndParseDocument(&reader, link_target, errorMessage, &context);
    if (status != KoFilter::OK) {
        reader.raiseError(errorMessage);
    }

    QString headerContent;

    TRY_READ_ATTR(type)
    if (!type.isEmpty()) {
        if (type == "even") {
             headerContent = "<style:header-left>";
             headerContent.append(reader.content());
             headerContent.append("</style:header-left>");
        }
        else {
            headerContent = "<style:header>";
            headerContent.append(reader.content());
            headerContent.append("</style:header>");
        }
        m_headers[type] = headerContent;
    }
    else {
        headerContent = "<style:header>";
        headerContent.append(reader.content());
        headerContent.append("</style:header>");
        m_headers["default"] = headerContent;
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL lnNumType
//! w:lnNumType handler (Line Numbering type)
/*
 Parents elements:
 - [done] sectPr (§17.6.17)
 - [done] sectPr (§17.6.18)
 - [done] sectPr (§17.6.19)

 Child elements:
 - none

*/
//! @todo support all elements
KoFilter::ConversionStatus DocxXmlDocumentReader::read_lnNumType()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR(countBy)
    //TRY_READ_ATTR(restart)
    //TRY_READ_ATTR(start)

    QBuffer buffer;
    KoXmlWriter temp(&buffer);

    temp.startElement("text:linenumbering-configuration");

    // These should maybe be somehow determined from attributes
    temp.addAttribute("text:number-position", "left");
    temp.addAttribute("style:num-format", "1");
    temp.addAttribute("text:offset", "0.1965in");

    if (!countBy.isEmpty()) {
        temp.addAttribute("text:increment", countBy);
    }

    temp.endElement(); // text:linenumbering-configuration

    QString lineStyle = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());
    mainStyles->insertRawOdfStyles(KoGenStyles::DocumentStyles, lineStyle.toUtf8());

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL endnotePr
//! w:endonotePr handler (Endnote properties)
/*
 Parent elements:
 - [done] sectPr (§17.6.17)
 - [done] sectPr (§17.6.18)
 - [done] sectPr (§17.6.19)

 Child elements:
 - [done] numFmt (Footnote Numbering Format) §17.11.18
 - [done] numRestart (Footnote and Endnote Numbering Restart Location) §17.11.19
 - [done] numStart (Footnote and Endnote Numbering Starting Value) §17.11.20
 - pos (Footnote Placement) §17.11.21

*/
//! @todo support all elements
KoFilter::ConversionStatus DocxXmlDocumentReader::read_endnotePr()
{
    READ_PROLOGUE

    MSOOXML::Utils::XmlWriteBuffer endBuf;
    body = endBuf.setWriter(body);
    body->startElement("text:notes-configuration");
    body->addAttribute("text:note-class", "endnote");

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(numFmt)
            ELSE_TRY_READ_IF(numRestart)
            ELSE_TRY_READ_IF(numStart)
            SKIP_UNKNOWN
        }
    }

    body->endElement(); // text:notes-configuration
    QString endStyle;
    body = endBuf.releaseWriter(endStyle);
    mainStyles->insertRawOdfStyles(KoGenStyles::DocumentStyles, endStyle.toUtf8());

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL footnotePr
//! w:footnotePr handler (Footnote properties)
/*
 Parent elements:
 - [done] sectPr (§17.6.17)
 - [done] sectPr (§17.6.18)
 - [done] sectPr (§17.6.19)

 Child elements:
 - [done] numFmt (Footnote Numbering Format) §17.11.18
 - [done] numRestart (Footnote and Endnote Numbering Restart Location) §17.11.19
 - [done] numStart (Footnote and Endnote Numbering Starting Value) §17.11.20
 - [done] pos (Footnote Placement) §17.11.21

*/
//! @todo support all elements
KoFilter::ConversionStatus DocxXmlDocumentReader::read_footnotePr()
{
    READ_PROLOGUE

    MSOOXML::Utils::XmlWriteBuffer footBuf;
    body = footBuf.setWriter(body);
    body->startElement("text:notes-configuration");
    body->addAttribute("text:note-class", "footnote");

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(numFmt)
            ELSE_TRY_READ_IF(numRestart)
            ELSE_TRY_READ_IF(numStart)
            ELSE_TRY_READ_IF(pos)
            SKIP_UNKNOWN
        }
    }

    body->endElement(); // text:notes-configuration
    QString footStyle;
    body = footBuf.releaseWriter(footStyle);
    mainStyles->insertRawOdfStyles(KoGenStyles::DocumentStyles, footStyle.toUtf8());

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL numFmt
//! numFmt handler (Footnote Numbering format)
/*
 Parent elements:
 - [done] endnotePr (§17.11.4)
 - [done] endnotePr (§17.11.5)
 - [done] footnotePr (§17.11.12)
 - [done] footnotePr (§17.11.11)

 Child elements:
 - none
*/
//! @todo support all attributes
KoFilter::ConversionStatus DocxXmlDocumentReader::read_numFmt()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR(val)

    if (!val.isEmpty()) {
        if (val == "upperLetter") {
            body->addAttribute("style:num-format", "A");
        }
        else if (val == "lowerLetter") {
            body->addAttribute("style:num-format", "a");
        }
        else if (val == "upperRoman") {
            body->addAttribute("style:num-format", "I");
        }
        else if (val == "lowerRoman") {
            body->addAttribute("style:num-format", "i");
        }
        else if (val == "none") {
            body->addAttribute("style:num-format", "");
        }
        else {
            body->addAttribute("style:num-format", "1");
        }
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL numRestart
//! numRestart (Footnote and Endnote Numbering Restart Location)
/*
 Parent elements:
 - [done] endnotePr (§17.11.4)
 - [done] endnotePr (§17.11.5)
 - [done] footnotePr (§17.11.12)
 - [done] footnotePr (§17.11.11)

 Child elements:
 - none
*/
//! @todo support all attributes
KoFilter::ConversionStatus DocxXmlDocumentReader::read_numRestart()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR(val)

    if (!val.isEmpty()) {
        if (val == "eachPage") {
            body->addAttribute("text:start-numbering-at", "page");
        }
        else if (val == "eachSect") {
            body->addAttribute("text:start-numbering-at", "chapter");
        }
        else { //continuous
            body->addAttribute("text:start-numbering-at", "document");
        }
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL numStart
//! numStart (Footnote and Endnote Numbering Starting Value)
/*
 Parent elements:
 - [done] endnotePr (§17.11.4)
 - [done] endnotePr (§17.11.5)
 - [done] footnotePr (§17.11.12)
 - [done] footnotePr (§17.11.11)

 Child elements:
 - none
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_numStart()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR(val)

    if (!val.isEmpty()) {
        body->addAttribute("text:start-value", val);
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL pos
//! pos (Footnote Placement)
/*
 Parent elements:
 - [not applicable] endnotePr (§17.11.4)
 - [not applicable] endnotePr (§17.11.5)
 - [done] footnotePr (§17.11.12)
 - [done] footnotePr (§17.11.11)

 Child elements:
 - none
 */
KoFilter::ConversionStatus DocxXmlDocumentReader::read_pos()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR(val)

    if (!val.isEmpty()) {
        if (val == "beneathText") {
            body->addAttribute("text:footnotes-position", "text");
        }
        else if (val == "docEnd") {
            body->addAttribute("text:footnotes-position", "document");
        }
        else if (val == "sectEnd") {
            body->addAttribute("text:footnotes-position", "section");
        }
        else { // pageBottom
            body->addAttribute("text:footnotes-position", "page");
        }
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL cols
//! w:cols handler (Column definitions)
/*
 Parents elements:
 - [done] sectPr (§17.6.17)
 - [done] sectPr (§17.6.18)
 - [done] sectPr (§17.6.19)

 Child Elements:
 - col (Single Column Definition) §17.6.3
*/
//! @todo support all elements
KoFilter::ConversionStatus DocxXmlDocumentReader::read_cols()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR(num)
    TRY_READ_ATTR(space)

    QBuffer columnBuffer;
    KoXmlWriter columnWriter(&columnBuffer);

    columnWriter.startElement("style:columns");
    if (!num.isEmpty()) {
        columnWriter.addAttribute("fo:column-count", num);
    }
    if (!space.isEmpty()) {
        bool ok;
        const qreal distance = qreal(TWIP_TO_POINT(space.toDouble(&ok)));
        if (ok) {
            columnWriter.addAttributePt("fo:column-gap", distance);
        }
    }

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
        }
    }

    columnWriter.endElement(); // style:columns;

    const QString elementContents = QString::fromUtf8(columnBuffer.buffer(), columnBuffer.buffer().size());
    if (!num.isEmpty()) {
        m_currentPageStyle.addChildElement("style:columns", elementContents);
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL pgBorders
//! w:pgBorders handler (Page Borders)
/*! ECMA-376, 17.6.10, p. 646.

 Parent elements:
 - [done] sectPr (§17.6.17)
 - [done] sectPr (§17.6.18)
 - [done] sectPr (§17.6.19)

 Child elements:
 - [done] bottom (Bottom Border) §17.6.2
 - [done] left (Left Border) §17.6.7
 - [done] right (Right Border) §17.6.15
 - [done] top (Top Border) §17.6.21
*/
//! @todo support all elements
KoFilter::ConversionStatus DocxXmlDocumentReader::read_pgBorders()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR(offsetFrom)
    m_pageBorderOffsetFrom = offsetFrom;

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            if (QUALIFIED_NAME_IS(top)) {
                RETURN_IF_ERROR(readBorderElement(TopBorder, "top", m_pageBorderStyles, m_pageBorderPaddings));
            }
            else if (QUALIFIED_NAME_IS(left)) {
                RETURN_IF_ERROR(readBorderElement(LeftBorder, "left", m_pageBorderStyles, m_pageBorderPaddings));
            }
            else if (QUALIFIED_NAME_IS(bottom)) {
                RETURN_IF_ERROR(readBorderElement(BottomBorder, "bottom", m_pageBorderStyles, m_pageBorderPaddings));
            }
            else if (QUALIFIED_NAME_IS(right)) {
                RETURN_IF_ERROR(readBorderElement(RightBorder, "right", m_pageBorderStyles, m_pageBorderPaddings));
            }
            ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

void DocxXmlDocumentReader::applyBorders(KoGenStyle *style, QMap<BorderSide, QString> sourceBorder, QMap<BorderSide, qreal> sourcePadding)
{
    const QString topBorder = sourceBorder.value(TopBorder,QString());
    const QString leftBorder = sourceBorder.value(LeftBorder,QString());
    const QString bottomBorder = sourceBorder.value(BottomBorder,QString());
    const QString rightBorder = sourceBorder.value(RightBorder,QString());
    if (!topBorder.isEmpty() && leftBorder == topBorder && bottomBorder == topBorder && rightBorder == topBorder) {
        style->addProperty("fo:border", topBorder); // all sides the same
    }
    else {
        if (!topBorder.isEmpty()) {
            style->addProperty("fo:border-top", topBorder);
        }
        if (!leftBorder.isEmpty()) {
            style->addProperty("fo:border-left", leftBorder);
        }
        if (!bottomBorder.isEmpty()) {
            style->addProperty("fo:border-bottom", bottomBorder);
        }
        if (!rightBorder.isEmpty()) {
            style->addProperty("fo:border-right", rightBorder);
        }
    }
    sourceBorder.clear();

    const qreal topPadding = sourcePadding.value(TopBorder);
    const qreal leftPadding = sourcePadding.value(LeftBorder);
    const qreal bottomPadding = sourcePadding.value(BottomBorder);
    const qreal rightPadding = sourcePadding.value(RightBorder);
    if (sourcePadding.contains(TopBorder) && leftPadding == topPadding && bottomPadding == topPadding && rightPadding == topPadding) {
        style->addPropertyPt("fo:padding", topPadding); // all sides the same
    }
    else {
        if (sourcePadding.contains(TopBorder)) {
            style->addPropertyPt("fo:padding-top", topPadding);
        }
        if (sourcePadding.contains(LeftBorder)) {
            style->addPropertyPt("fo:padding-left", leftPadding);
        }
        if (sourcePadding.contains(BottomBorder)) {
            style->addPropertyPt("fo:padding-bottom", bottomPadding);
        }
        if (sourcePadding.contains(RightBorder)) {
            style->addPropertyPt("fo:padding-right", rightPadding);
        }
    }
    sourcePadding.clear();
}

void DocxXmlDocumentReader::applyPageBorders(KoGenStyle &style, QMap<PageMargin, qreal> &pageMargins,
        QMap<BorderSide,QString> &pageBorder, QMap<BorderSide, qreal> &pagePadding, QString & offsetFrom)
{
    if (pageMargins.contains(MarginTop)) {
        if (pagePadding.contains(TopBorder)) {
            qreal margin = pageMargins.value(MarginTop);
            qreal padding = pagePadding.value(TopBorder);
            if(offsetFrom == "page") {
                style.addPropertyPt("fo:margin-top", padding);
                style.addPropertyPt("fo:padding-top", margin - padding);
            }
            else {
                style.addPropertyPt("fo:margin-top", margin - padding);
                style.addPropertyPt("fo:padding-top", padding);
            }
        }
        else {
            style.addPropertyPt("fo:margin-top", pageMargins.value(MarginTop));
        }
    }

    if (pageMargins.contains(MarginBottom)) {
        if (pagePadding.contains(BottomBorder)) {
            qreal margin = pageMargins.value(MarginBottom);
            qreal padding = pagePadding.value(BottomBorder);
            if(offsetFrom == "page") {
                style.addPropertyPt("fo:margin-bottom", padding);
                style.addPropertyPt("fo:padding-bottom", margin - padding);
            }
            else {
                style.addPropertyPt("fo:margin-bottom", margin - padding);
                style.addPropertyPt("fo:padding-bottom", padding);
            }
        }
        else {
            style.addPropertyPt("fo:margin-bottom", pageMargins.value(MarginBottom));
        }
    }

    if (pageMargins.contains(MarginLeft)) {
        if (pagePadding.contains(LeftBorder)) {
            qreal margin = pageMargins.value(MarginLeft);
            qreal padding = pagePadding.value(LeftBorder);
            if(offsetFrom == "page") {
                style.addPropertyPt("fo:margin-left", padding);
                style.addPropertyPt("fo:padding-left", margin - padding);
            }
            else {
                style.addPropertyPt("fo:margin-left", margin - padding);
                style.addPropertyPt("fo:padding-left", padding);
            }
        }
        else {
            style.addPropertyPt("fo:margin-left", pageMargins.value(MarginLeft));
        }
    }

    if (pageMargins.contains(MarginRight)) {
        if (pagePadding.contains(RightBorder)) {
            qreal margin = pageMargins.value(MarginRight);
            qreal padding = pagePadding.value(RightBorder);
            if(offsetFrom == "page") {
                style.addPropertyPt("fo:margin-right", padding);
                style.addPropertyPt("fo:padding-right", margin - padding);
            }
            else {
                style.addPropertyPt("fo:margin-right", margin - padding);
                style.addPropertyPt("fo:padding-right", padding);
            }
        }
        else {
            style.addPropertyPt("fo:margin-right", pageMargins.value(MarginRight));
        }
    }

    pageMargins.clear();
    pagePadding.clear();

    const QString topBorder = pageBorder.value(TopBorder,QString());
    const QString leftBorder = pageBorder.value(LeftBorder,QString());
    const QString bottomBorder = pageBorder.value(BottomBorder,QString());
    const QString rightBorder = pageBorder.value(RightBorder,QString());
    if (!topBorder.isEmpty() && leftBorder == topBorder && bottomBorder == topBorder && rightBorder == topBorder) {
        style.addProperty("fo:border", topBorder); // all sides the same
    }
    else {
        if (!topBorder.isEmpty()) {
            style.addProperty("fo:border-top", topBorder);
        }
        if (!leftBorder.isEmpty()) {
            style.addProperty("fo:border-left", leftBorder);
        }
        if (!bottomBorder.isEmpty()) {
            style.addProperty("fo:border-bottom", bottomBorder);
        }
        if (!rightBorder.isEmpty()) {
            style.addProperty("fo:border-right", rightBorder);
        }
    }
    pageBorder.clear();
}

//! Converts 17.18.2 ST_Border (Border Styles, p. 1462, 4357) value to W3C CSS2 border-style value
//! @see http://www.w3.org/TR/CSS2/box.html#value-def-border-style
//! @see http://www.w3.org/TR/CSS2/box.html#value-def-border-width
static QString ST_Border_to_ODF(const QString& s)
{
    if (s == "nil" || s == "none" || s.isEmpty())
        return QString();
    else if (s == "thick")
        return QLatin1String("solid thick");
    else if (s == "single")
        return QLatin1String("solid");
    else if (s == "dashed" || s == "dotted" || s == "double")
        return s;
    return QLatin1String("solid");
}

KoFilter::ConversionStatus DocxXmlDocumentReader::readBorderElement(BorderSide borderSide,
        const char *borderSideName, QMap<BorderSide, QString> &sourceBorder, QMap<BorderSide, qreal> &sourcePadding)
{
    const QXmlStreamAttributes attrs(attributes());
    READ_ATTR(val)
    TRY_READ_ATTR(sz)
    TRY_READ_ATTR(color)
    createBorderStyle(sz, color, val, borderSide, sourceBorder);
    TRY_READ_ATTR(space)
    if (!space.isEmpty()) {
        int sp;
        STRING_TO_INT(space, sp, QString("w:%1@space").arg(borderSideName));
        sourcePadding.insertMulti(borderSide, sp);
    }
    readNext();
    return KoFilter::OK;
}

void DocxXmlDocumentReader::createBorderStyle(const QString& size, const QString& color,
    const QString& lineStyle, BorderSide borderSide, QMap<BorderSide, QString> &sourceBorder)
{
    const QString odfLineStyle(ST_Border_to_ODF(lineStyle));
    if (odfLineStyle.isEmpty())
        return;

    QString border;
    if (!size.isEmpty())
        border += MSOOXML::Utils::ST_EighthPointMeasure_to_ODF(size) + ' ';

    border.append(odfLineStyle + ' ');

    if (!color.isEmpty()) {
        if (color == "auto") {
            // The documentation for auto value says that it leaves the color up to the application
            // to decide, here we make a decision to use window color text
            MSOOXML::DrawingMLColorSchemeItemBase *colorItem = 0;
            colorItem = m_context->themes->colorScheme.value("dk1");
            QColor col = Qt::black;
            if (colorItem) {
                col = colorItem->value();
            }
            border.append(col.name());
        } else {
            border.append('#');
            border.append(color);
        }
    }
    else {
        border.append(QLatin1String("#000000"));
    }

    sourceBorder.insertMulti(borderSide,border);
}

#undef CURRENT_EL
#define CURRENT_EL object
//! object handler (Embedded Object)
/*! ECMA-376, 17.3.3.19, p.371.
*/
//! @todo support all elements
KoFilter::ConversionStatus DocxXmlDocumentReader::read_object()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR(dxaOrig)
    m_currentObjectWidthCm = MSOOXML::Utils::ST_TwipsMeasure_to_cm(dxaOrig);
    TRY_READ_ATTR(dyaOrig)
    m_currentObjectHeightCm = MSOOXML::Utils::ST_TwipsMeasure_to_cm(dyaOrig);

    // Protecting in case the object is inside a textbox inside a shape
    VMLShapeProperties oldProperties = m_currentVMLProperties;

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF_NS(v, shapetype)
            else if (name() == "shape") {
                m_outputFrames = false;
                TRY_READ(shape)
                m_outputFrames = true;
            }
            ELSE_TRY_READ_IF_NS(o, OLEObject)
            ELSE_TRY_READ_IF(control)
            SKIP_UNKNOWN
            //! @todo add ELSE_WRONG_FORMAT
        }
    }

    m_currentVMLProperties = oldProperties;

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL commentRangeStart
//! commentRangeStart handler
/*!

 Parent elements:
 - bdo (§17.3.2.3)
 - body (§17.2.2)
 - comment (§17.13.4.2)
 - customXml (§17.5.1.6)
 - customXml (§17.5.1.4)
 - customXml (§17.5.1.5)
 - customXml (§17.5.1.3)
 - deg (§22.1.2.26)
 - del (§17.13.5.14)
 - den (§22.1.2.28)
 - dir (§17.3.2.8)
 - docPartBody (§17.12.6)
 - e (§22.1.2.32)
 - endnote (§17.11.2)
 - fldSimple (§17.16.19)
 - fName (§22.1.2.37)
 - footnote (§17.11.10)
 - ftr (§17.10.3)
 - hdr (§17.10.4)
 - hyperlink (§17.16.22)
 - ins (§17.13.5.18)
 - lim (§22.1.2.52)
 - moveFrom (§17.13.5.22)
 - moveTo (§17.13.5.25)
 - num (§22.1.2.75)
 - oMath (§22.1.2.77)
 - [done] p (§17.3.1.22)
 - rt (§17.3.3.24)
 - rubyBase (§17.3.3.27)
 - sdtContent (§17.5.2.34)
 - sdtContent (§17.5.2.33)
 - sdtContent (§17.5.2.35)
 - sdtContent (§17.5.2.36)
 - [done] smartTag (§17.5.1.9)
 - sub (§22.1.2.112)
 - sup (§22.1.2.114)
 - tbl (§17.4.38)
 - tc (§17.4.66)
 - tr (§17.4.79)

 Child elements:
 - none

*/
//! @todo support all attributes etc.
KoFilter::ConversionStatus DocxXmlDocumentReader::read_commentRangeStart()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    READ_ATTR(id)

    body->startElement("office:annotation");

    body->addCompleteElement(m_context->m_comments[id].toUtf8());

    body->endElement(); // office:annotation

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL endnoteReference
//! endnoteReference handler
/*!

 Parent elements:
 - [done] r (§17.3.2.25)
 - [done] r (§22.1.2.87)

 Child elements:
 - none

*/
//! @todo support all attributes etc.
KoFilter::ConversionStatus DocxXmlDocumentReader::read_endnoteReference()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    READ_ATTR(id)

    // # example endnote from odt document converted with OpenOffice
    // <text:note xml:id="ftn1" text:id="ftn1" text:note-class="endnote">
    // <text:note-citation>1</text:note-citation>
    // <text:note-body>
    // <text:p text:style-name="P2">
    // <text:span text:style-name="endnote_20_reference" />studies</text:p>
    // <text:p text:style-name="endnote" />
    // </text:note-body>
    // </text:note>

    body->startElement("text:note");
    body->addAttribute("text:id", QString("endn").append(id));
    body->addAttribute("text:note-class", "endnote");
    body->startElement("text:note-citation");

    // NOTE: This line is meaningless in the sense that office
    // programs are supposed to autogenerate the value based on the
    // footnote style, it is hardcoded for the moment as calligra has
    // no support for it.
    body->addTextSpan(id);

    body->endElement(); // text:note-citation
    body->startElement("text:note-body");
    body->addCompleteElement(m_context->m_endnotes[id].toUtf8());
    body->endElement(); // text:note-body
    body->endElement(); // text:note

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL footnoteReference
//! footnoteReference handler
/*!

 Parent elements:
 - [done] r (§17.3.2.25)
 - [done] r (§22.1.2.87)

 Child elements:
 - none

*/
//! @todo support all attributes etc.
KoFilter::ConversionStatus DocxXmlDocumentReader::read_footnoteReference()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    READ_ATTR(id)

    // # example endnote from odt document converted with OpenOffice
    // <text:note text:id="ftn1" xml:id="ftn1" text:note-class="footnote">
    // <text:note-citation>1</text:note-citation>
    // <text:note-body>
    // <text:p text:style-name="P2">
    // <text:span text:style-name="footnote_20_reference" />studies</text:p>
    // <text:p text:style-name="Footnote" />
    // </text:note-body>
    // </text:note>

    body->startElement("text:note");
    body->addAttribute("text:id", QString("ftn").append(id));
    body->addAttribute("text:note-class", "footnote");
    body->startElement("text:note-citation");

    // NOTE: This line is meaningless in the sense that office
    // programs are supposed to autogenerate the value based on the
    // footnote style, it is hardcoded for the moment as calligra has
    // no support for it.
    body->addTextSpan(id);

    body->endElement(); // text:note-citation
    body->startElement("text:note-body");
    body->addCompleteElement(m_context->m_footnotes[id].toUtf8());
    body->endElement(); // text:note-body
    body->endElement(); // text:note

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL fldChar
//! fldChar handler
/*! Complex field character

 Parent elements:
 - [done] r (§17.3.2.25)
 - [done] r (§22.1.2.87)

 Child elements:
 - ffData (Form Field Properties) §17.16.17

*/
//! @todo support all attributes etc.
KoFilter::ConversionStatus DocxXmlDocumentReader::read_fldChar()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR(fldCharType)

    if (!fldCharType.isEmpty()) {
       if (fldCharType == "begin") {
           m_complexCharStatus = InstrAllowed;
       }
       else if (fldCharType == "separate") {
           m_complexCharStatus = InstrExecute;
       }
       else if (fldCharType == "end") {
           m_complexCharStatus = NoneAllowed;
           m_complexCharType = NoComplexFieldCharType;
           m_complexCharValue.clear();
       }
    }

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL br
//! br handler
/*
 Parent elements:
 - [done] r (§17.3.2.25)
 - [done] r (§22.1.2.87)
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_br()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR(type)

    if (type == "column") {
        m_currentParagraphStyle.addProperty("fo:break-before", "column");
    }
    else if (type == "page") {
        m_currentParagraphStyle.addProperty("fo:break-before", "page");
    }
    else {
        body->startElement("text:line-break");
        body->endElement();
    }
    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL lastRenderedPageBreak
//! lastRenderedPageBreak handler
/*
 Parent elements:
 - [done] r (§17.3.2.25)
 - [done] r (§22.1.2.87)
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_lastRenderedPageBreak()
{
    READ_PROLOGUE
    body->startElement("text:soft-page-break");
    body->endElement(); // text:soft-page-break
    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL instrText
//! instrText handler
/*
 Parent elements:
 - [done] r (§17.3.2.25)
 - [done] r (§22.1.2.87)
*/
//! @todo support all attributes etc.
KoFilter::ConversionStatus DocxXmlDocumentReader::read_instrText()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (m_complexCharStatus == InstrAllowed) {
            QString instruction = text().toString().trimmed();

            if (instruction.startsWith(QLatin1String("HYPERLINK"))) {
                // Removes hyperlink, spaces and extra " chars
                instruction.remove(0, 11);
                instruction.truncate(instruction.size() - 1);
                m_complexCharType = HyperlinkComplexFieldCharType;
                m_complexCharValue = instruction;
            }
            else if (instruction.startsWith(QLatin1String("PAGEREF"))) {
                instruction.remove(0, 8); // removes PAGEREF
                m_complexCharType = ReferenceComplexFieldCharType;
                m_complexCharValue = instruction.left(instruction.indexOf(' '));
            }
            else if (instruction.startsWith(QLatin1String("GOTOBUTTON"))) {
                instruction.remove(0, 12); // removes GOTOBUTTON
                m_complexCharType = InternalHyperlinkComplexFieldCharType;
                m_complexCharValue = instruction;
            }
            else if (instruction.startsWith(QLatin1String("MACROBUTTON"))) {
                m_complexCharType = MacroButtonFieldCharType;
                m_complexCharValue = '[';
            }
            else {
                m_complexCharValue = instruction;
            }
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL hyperlink
//! hyperlink handler (Hyperlink)
/*! ECMA-376, 17.3.3.31, p.1431.

 Parent elements:
 - bdo (§17.3.2.3)
 - customXml (§17.5.1.3)
 - dir (§17.3.2.8)
 - [done] fldSimple (§17.16.19)
 - [done] hyperlink (§17.16.22)
 - [done] p (§17.3.1.22)
 - [done] sdtContent (§17.5.2.36)
 - [done] smartTag (§17.5.1.9)

 Child elements:
 - bdo (Bidirectional Override) §17.3.2.3
 - [done] bookmarkEnd (Bookmark End) §17.13.6.1
 - [done] bookmarkStart (Bookmark Start) §17.13.6.2
 - commentRangeEnd (Comment Anchor Range End) §17.13.4.3
 - commentRangeStart (Comment Anchor Range Start) §17.13.4.4
 - customXml (Inline-Level Custom XML Element) §17.5.1.3
 - customXmlDelRangeEnd (Custom XML Markup Deletion End) §17.13.5.4
 - customXmlDelRangeStart (Custom XML Markup Deletion Start) §17.13.5.5
 - customXmlInsRangeEnd (Custom XML Markup Insertion End) §17.13.5.6
 - customXmlInsRangeStart (Custom XML Markup Insertion Start) §17.13.5.7
 - customXmlMoveFromRangeEnd (Custom XML Markup Move Source End) §17.13.5.8
 - customXmlMoveFromRangeStart (Custom XML Markup Move Source Start) §17.13.5.9
 - customXmlMoveToRangeEnd (Custom XML Markup Move Destination Location End) §17.13.5.10
 - customXmlMoveToRangeStart (Custom XML Markup Move Destination Location Start) §17.13.5.11
 - [done] del (Deleted Run Content) §17.13.5.14
 - dir (Bidirectional Embedding Level) §17.3.2.8
 - [done] fldSimple (Simple Field) §17.16.19
 - [done] hyperlink (Hyperlink) §17.16.22
 - [done] ins (Inserted Run Content) §17.13.5.18
 - moveFrom (Move Source Run Content) §17.13.5.22
 - moveFromRangeEnd (Move Source Location Container - End) §17.13.5.23
 - moveFromRangeStart (Move Source Location Container - Start) §17.13.5.24
 - moveTo (Move Destination Run Content) §17.13.5.25
 - moveToRangeEnd (Move Destination Location Container - End) §17.13.5.27
 - moveToRangeStart (Move Destination Location Container - Start) §17.13.5.28
 - oMath (Office Math) §22.1.2.77
 - oMathPara (Office Math Paragraph) §22.1.2.78
 - permEnd (Range Permission End) §17.13.7.1
 - permStart (Range Permission Start) §17.13.7.2
 - proofErr (Proofing Error Anchor) §17.13.8.1
 - [done] r (Text Run) §17.3.2.25
 - [done] sdt (Inline-Level Structured Document Tag) §17.5.2.31
 - [done] smartTag (Inline-Level Smart Tag) §17.5.1.9
 - subDoc (Anchor for Subdocument Location) §17.17.1.1
*/
//! @todo support all elements
KoFilter::ConversionStatus DocxXmlDocumentReader::read_hyperlink()
{
    READ_PROLOGUE

    QString link_target;

    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITH_NS(r, id)
    if (r_id.isEmpty()) {
        link_target.clear();
    }
    else {
        link_target = m_context->relationships->target(m_context->path, m_context->file, r_id);
        // The return value also contains the path in the start and directory separator, we need to remove them
        link_target.remove(0, m_context->path.size() + 1);
    }

    bool closeTag = false;

    TRY_READ_ATTR(anchor)

    MSOOXML::Utils::XmlWriteBuffer hlinkBuffer;
    body = hlinkBuffer.setWriter(body);

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(r)
            ELSE_TRY_READ_IF(sdt)
            ELSE_TRY_READ_IF(hyperlink)
            ELSE_TRY_READ_IF(bookmarkStart)
            ELSE_TRY_READ_IF(bookmarkEnd)
            ELSE_TRY_READ_IF(fldSimple)
            ELSE_TRY_READ_IF(del)
            ELSE_TRY_READ_IF(ins)
            ELSE_TRY_READ_IF(smartTag)
            SKIP_UNKNOWN
            //! @todo add ELSE_WRONG_FORMAT
        }
    }
    body = hlinkBuffer.originalWriter();

    //NOTE: Workaround until text:display="none" support.
    if ((m_currentParagraphStyle.property("text:display", KoGenStyle::TextType) == "none") &&
        hlinkBuffer.isEmpty()) {
        READ_EPILOGUE
    }

    if (!link_target.isEmpty() || !anchor.isEmpty()) {
        body->startElement("text:a", false);
        body->addAttribute("xlink:type", "simple");
        closeTag = true;
        if (!anchor.isEmpty())
        {
            body->addAttribute("xlink:href", QString("#%1").arg(anchor));
        }
        else {
            body->addAttribute("xlink:href", QUrl(link_target).toEncoded());
        }
    }

    hlinkBuffer.releaseWriter();

    if (closeTag) {
        body->endElement(); // text:bookmark, text:a
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL del
//! del (Deleted Run Content)
/*! ECMA-376, 17.13.5.18, p.959.

  This element specifies that the inline-level content contained within it
  shall be treated as deleted content which has been tracked as a revision.

  Parent elements, Child elements: sync to ins (Inserted Run Content)

 */
//! @todo: read more attributes and child elements
KoFilter::ConversionStatus DocxXmlDocumentReader::read_del()
{
    READ_PROLOGUE

    m_changeTrackingState.push(DeletedRunContent);

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(r)
            ELSE_TRY_READ_IF(bookmarkStart)
            ELSE_TRY_READ_IF(bookmarkEnd)
            ELSE_TRY_READ_IF(del)
            ELSE_TRY_READ_IF(ins)
            ELSE_TRY_READ_IF_NS(m, oMath)
            ELSE_TRY_READ_IF_NS(m, oMathPara)
            ELSE_TRY_READ_IF(sdt)
            ELSE_TRY_READ_IF(smartTag)
            SKIP_UNKNOWN
        }
    }

    if (m_changeTrackingState.isEmpty()) {
        warnDocx << "Error: ChangeTrackingState stack is corrupt!";
    } else {
        m_changeTrackingState.pop();
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL ins
//! ins (Inserted Run Content)
/*! ECMA-376, 17.13.5.18, p.959.

  This element specifies that the inline-level content contained within it shall
  be treated as inserted content which has been tracked as a revision.

  Parent elements:
  - bdo (§17.3.2.3)
  - [done] body (§17.2.2)
  - comment (§17.13.4.2)
  - customXml (§17.5.1.3)
  - deg (§22.1.2.26)
  - del (§17.13.5.14)
  - den (§22.1.2.28)
  - dir (§17.3.2.8)
  - docPartBody (§17.12.6)
  - e (§22.1.2.32)
  - [done] endnote (§17.11.2)
  - [done] fldSimple (§17.16.19)
  - fName (§22.1.2.37)
  - [done] footnote (§17.11.10)
  - [done] ftr (§17.10.3)
  - [done] hdr (§17.10.4)
  - [done] hyperlink (§17.16.22)
  - lim (§22.1.2.52
  - moveFrom (§17.13.5.22)
  - moveTo (§17.13.5.25)
  - num (§22.1.2.75)
  - [done] oMath (§22.1.2.77)
  - [done] p (§17.3.1.22)
  - rt (§17.3.3.24
  - rubyBase (§17.3.3.27)
  - sdtContent (§17.5.2.36)
  - [done] smartTag (§17.5.1.9)
  - sub (§22.1.2.112)
  - sup (§22.1.2.114)
  - tbl (§17.4.38)
  - tc (§17.4.66)
  - tr (§17.4.79)

  Child elements:
  - acc (Accent) §22.1.2.1
  - bar (Bar) §22.1.2.7
  - bdo (Bidirectional Override) §17.3.2.3
  - [done] bookmarkEnd (Bookmark End) §17.13.6.1
  - [done] bookmarkStart (Bookmark Start) §17.13.6.2
  - borderBox (Border-Box Object) §22.1.2.11
  - box (Box Object) §22.1.2.13
  - commentRangeEnd (Comment Anchor Range End) §17.13.4.3
  - commentRangeStart (Comment Anchor Range Start) §17.13.4.4
  - customXml (Inline-Level Custom XML Element) §17.5.1.3
  - customXmlDelRangeEnd (Custom XML Markup Deletion End) §17.13.5.4
  - customXmlDelRangeStart (Custom XML Markup Deletion Start) §17.13.5.5
  - customXmlInsRangeEnd (Custom XML Markup Insertion End) §17.13.5.6
  - customXmlInsRangeStart (Custom XML Markup Insertion Start) §17.13.5.7
  - customXmlMoveFromRangeEnd (Custom XML Markup Move Source End) §17.13.5.8
  - customXmlMoveFromRangeStart (Custom XML Markup Move Source Start) §17.13.5.9
  - customXmlMoveToRangeEnd (Custom XML Markup Move Destination Location End) §17.13.5.10
  - customXmlMoveToRangeStart (Custom XML Markup Move Destination Location Start) §17.13.5.11
  - d (Delimiter Object) §22.1.2.24
  - [done] del (Deleted Run Content) §17.13.5.14
  - dir (Bidirectional Embedding Level) §17.3.2.8
  - eqArr (Array Object) §22.1.2.34
  - f (Fraction Object) §22.1.2.36
  - func (Function Apply Object) §22.1.2.39
  - groupChr (Group-Character Object) §22.1.2.41
  - [done] ins (Inserted Run Content) §17.13.5.18
  - limLow (Lower-Limit Object) §22.1.2.54
  - limUpp (Upper-Limit Object) §22.1.2.56
  - m (Matrix Object) §22.1.2.60
  - moveFrom (Move Source Run Content) §17.13.5.22
  - moveFromRangeEnd (Move Source Location Container - End) §17.13.5.23
  - moveFromRangeStart (Move Source Location Container - Start) §17.13.5.24
  - moveTo (Move Destination Run Content) §17.13.5.25
  - moveToRangeEnd (Move Destination Location Container - End) §17.13.5.27
  - moveToRangeStart (Move Destination Location Container - Start) §17.13.5.28
  - nary (n-ary Operator Object) §22.1.2.70
  - [done] oMath (Office Math) §22.1.2.77
  - [done] oMathPara (Office Math Paragraph) §22.1.2.78
  - permEnd (Range Permission End) §17.13.7.1
  - permStart (Range Permission Start) §17.13.7.2
  - phant (Phantom Object) §22.1.2.81
  - proofErr (Proofing Error Anchor) §17.13.8.1
  - [done] r (Run) §22.1.2.87
  - [done] r (Text Run) §17.3.2.25
  - rad (Radical Object) §22.1.2.88
  - [done] sdt (Inline-Level Structured Document Tag) §17.5.2.31
  - [done] smartTag (Inline-Level Smart Tag) §17.5.1.9
  - sPre (Pre-Sub-Superscript Object) §22.1.2.99
  - sSub (Subscript Object) §22.1.2.101
  - sSubSup (Sub-Superscript Object) §22.1.2.103
  - sSup (Superscript Object) §22.1.2.105
*/
//! @todo: read more attributes and child elements
KoFilter::ConversionStatus DocxXmlDocumentReader::read_ins()
{
    READ_PROLOGUE

    m_changeTrackingState.push(InsertedRunContent);

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(r)
            ELSE_TRY_READ_IF(bookmarkStart)
            ELSE_TRY_READ_IF(bookmarkEnd)
            ELSE_TRY_READ_IF(del)
            ELSE_TRY_READ_IF(ins)
            ELSE_TRY_READ_IF_NS(m, oMath)
            ELSE_TRY_READ_IF_NS(m, oMathPara)
            ELSE_TRY_READ_IF(sdt)
            ELSE_TRY_READ_IF(smartTag)
            SKIP_UNKNOWN
        }
    }

    if (m_changeTrackingState.isEmpty()) {
        warnDocx << "Error: ChangeTrackingState stack is corrupt!";
    } else {
        m_changeTrackingState.pop();
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL sdtContent
/*! sdt handler
//! @todo support properly */
KoFilter::ConversionStatus DocxXmlDocumentReader::read_sdtContent()
{
    READ_PROLOGUE

    // FIXME: This is not properly supported at all atm.

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(p)
            ELSE_TRY_READ_IF(tbl)
            ELSE_TRY_READ_IF(sdt)
            ELSE_TRY_READ_IF(fldSimple)
            ELSE_TRY_READ_IF(hyperlink)
            ELSE_TRY_READ_IF_NS(m, oMath)
            ELSE_TRY_READ_IF_NS(m, oMathPara)
            ELSE_TRY_READ_IF(r)
            ELSE_TRY_READ_IF(bookmarkStart)
            ELSE_TRY_READ_IF(bookmarkEnd)
            ELSE_TRY_READ_IF(smartTag)
            ELSE_TRY_READ_IF(tc)
            SKIP_UNKNOWN
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL sdt
/*! sdt handler
//! @todo support properly */
KoFilter::ConversionStatus DocxXmlDocumentReader::read_sdt()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(sdtContent)
            SKIP_UNKNOWN
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL txbxContent
/*! txbxContent handler (Textbox content)

 Parent elements:
 - [done] textbox (§14.1.2.19)

*/
//! @todo support all elements
KoFilter::ConversionStatus DocxXmlDocumentReader::read_txbxContent()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(p)
            ELSE_TRY_READ_IF(tbl)
            ELSE_TRY_READ_IF(sdt)
            SKIP_UNKNOWN
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL p
//! [1] p handler (Paragraph) ECMA-376, WML 17.3.1.22, p. 251,
//!       This element specifies a paragraph of content in the document.

//! [2] p handler (Text Paragraphs) ECMA-376, DrawingML 21.1.2.2.6, p. 3587.
//!       This element specifies the presence of a paragraph of text within the containing text body.
/*!
 Parent elements:
 - [done] body (§17.2.2)
 - [done] comment (§17.13.4.2)
 - customXml (§17.5.1.6)
 - docPartBody (§17.12.6)
 - [done] endnote (§17.11.2)
 - [done] footnote (§17.11.10)
 - [done] ftr (§17.10.3)
 - [done] hdr (§17.10.4)
 - [done] sdtContent (§17.5.2.34)
 - [done] tc (§17.4.66)
 - [done] p (§17.3.1.22)

 Child elements:
 - bdo (Bidirectional Override) §17.3.2.3
 - [done] bookmarkEnd (Bookmark End) §17.13.6.1
 - [done] bookmarkStart (Bookmark Start) §17.13.6.2
 - commentRangeEnd (Comment Anchor Range End) §17.13.4.3
 - [done] commentRangeStart (Comment Anchor Range Start) §17.13.4.4 - WML only
 - customXml (Inline-Level Custom XML Element) §17.5.1.3
 - customXmlDelRangeEnd (Custom XML Markup Deletion End) §17.13.5.4
 - customXmlDelRangeStart (Custom XML Markup Deletion Start) §17.13.5.5
 - customXmlInsRangeEnd (Custom XML Markup Insertion End) §17.13.5.6
 - customXmlInsRangeStart (Custom XML Markup Insertion Start) §17.13.5.7
 - customXmlMoveFromRangeEnd (Custom XML Markup Move Source End) §17.13.5.8
 - customXmlMoveFromRangeStart (Custom XML Markup Move Source Start) §17.13.5.9
 - customXmlMoveToRangeEnd (Custom XML Markup Move Destination Location End) §17.13.5.10
 - customXmlMoveToRangeStart (Custom XML Markup Move Destination Location Start) §17.13.5.11
 - [done] del (Deleted Run Content) §17.13.5.14
 - dir (Bidirectional Embedding Level) §17.3.2.8
 - [done] fldSimple (Simple Field) §17.16.19
 - [done] hyperlink (Hyperlink) §17.16.22 - WML only
 - [done] ins (Inserted Run Content) §17.13.5.18
 - moveFrom (Move Source Run Content) §17.13.5.22
 - moveFromRangeEnd (Move Source Location Container - End) §17.13.5.23
 - moveFromRangeStart (Move Source Location Container - Start) §17.13.5.24
 - moveTo (Move Destination Run Content) §17.13.5.25
 - moveToRangeEnd (Move Destination Location Container - End) §17.13.5.27
 - moveToRangeStart (Move Destination Location Container - Start) §17.13.5.28
 - [done] oMath (Office Math) §22.1.2.77
 - [done] oMathPara (Office Math Paragraph) §22.1.2.78
 - permEnd (Range Permission End) §17.13.7.1
 - permStart (Range Permission Start) §17.13.7.2
 - [done] pPr (Paragraph Properties) §17.3.1.26
 - proofErr (Proofing Error Anchor) §17.13.8.1
 - [done] r (Text Run) §17.3.2.25
 - [done] sdt (Inline-Level Structured Document Tag) §17.5.2.31
 - [done] smartTag (Inline-Level Smart Tag) §17.5.1.9
 - subDoc (Anchor for Subdocument Location) §17.17.1.1
*/
//! @todo support all elements
KoFilter::ConversionStatus DocxXmlDocumentReader::read_p()
{
    READ_PROLOGUE
    m_insideParagraph = true;
    m_paragraphStyleNameWritten = false;
    m_currentStyleName.clear();
    m_listFound = false;
    m_closeHyperlink = false;

    // It is possible that one of the child elements of p has p element
    // Therefore we push the current style to vector and pop it out when we
    // come out To make sure in that case we don't lose the previous style
    QVector<KoGenStyle> activeStyles;
    activeStyles.push_back(m_currentParagraphStyle);

    MSOOXML::Utils::XmlWriteBuffer textPBuf;

    bool oldWasCaption = m_wasCaption;
    m_wasCaption = false;
    if (oldWasCaption) {
        debugDocx << "SKIP!";
    } else {
        body = textPBuf.setWriter(body);
        m_currentParagraphStyle = KoGenStyle(KoGenStyle::ParagraphAutoStyle, "paragraph");
        if (m_moveToStylesXml) {
            m_currentParagraphStyle.setAutoStyleInStylesDotXml(true);
        }

        // MS2007 has a different way of marking drop cap, it divides them to
        // two paragraphs here we apply the status to current paragraph if
        // previous one had dropCap
        if (m_dropCapStatus == DropCapDone) {
            QBuffer frameBuffer;
            frameBuffer.open(QIODevice::WriteOnly);
            KoXmlWriter elementWriter(&frameBuffer);
            elementWriter.startElement("style:drop-cap");
            elementWriter.addAttribute("style:lines", m_dropCapLines);
            elementWriter.addAttributePt("style:distance", m_dropCapDistance);
            elementWriter.endElement(); // style-drop-cap
            QString drop = QString::fromUtf8(frameBuffer.buffer(), frameBuffer.buffer().size());
            m_currentParagraphStyle.addChildElement("style:tab-stops", drop);
        }
    }

    // Whether section starts from this paragraph
    bool sectionAdded = false;

    if (m_createSectionStyle) {
        // To avoid the style to become a duplicate or being duplicated.
        m_currentParagraphStyle.addAttribute("style:master-page-name", "placeholder");
        m_createSectionStyle = false;
        sectionAdded = true;
    }

    while (!atEnd()) {
        readNext();
        debugDocx << *this;
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            //ELSE_TRY_READ_IF(commentRangeEnd)
            TRY_READ_IF(sdt)
            ELSE_TRY_READ_IF(hyperlink)
            ELSE_TRY_READ_IF(del)
            ELSE_TRY_READ_IF(ins)
            ELSE_TRY_READ_IF(commentRangeStart)
            ELSE_TRY_READ_IF(bookmarkStart)
            ELSE_TRY_READ_IF(bookmarkEnd)
            ELSE_TRY_READ_IF(pPr) // CASE #400.1
            ELSE_TRY_READ_IF(r) // CASE #400.2
            ELSE_TRY_READ_IF(fldSimple)
            ELSE_TRY_READ_IF_NS(m, oMathPara)
            ELSE_TRY_READ_IF_NS(m, oMath)
            ELSE_TRY_READ_IF(smartTag)
            SKIP_UNKNOWN
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    //---------------------------------------------
    // Prepare for List Style
    //---------------------------------------------
    if (m_listFound && m_currentBulletList.size() > m_currentListLevel) {
        m_currentBulletProperties = m_currentBulletList.at(m_currentListLevel);

        KoGenStyle textStyle = KoGenStyle(KoGenStyle::TextStyle, "text");
        KoGenStyle::copyPropertiesFromStyle(m_currentParagraphStyle, textStyle, KoGenStyle::TextType);

        if (!textStyle.isEmpty()) {

            //MSWord: A bullet/picture label does not inherit font information
            //from the paragraph mark.
            if (m_currentBulletProperties.m_type != MSOOXML::Utils::ParagraphBulletProperties::NumberType) {
                textStyle.removeProperty("style:font-name");
                textStyle.removeProperty("style:font-name-complex");
            }
            //MSWord: A label does NOT inherit Underline from text-properties
            //of the paragraph style.  A bullet/picture does not inherit
            //{Italics, Bold}.
            if (m_currentBulletProperties.m_type != MSOOXML::Utils::ParagraphBulletProperties::NumberType) {
                textStyle.removeProperty("fo:font-style");
                textStyle.removeProperty("fo:font-weight");
            }
            textStyle.removeProperty("style:text-underline-color");
            textStyle.removeProperty("style:text-underline-mode");
            textStyle.removeProperty("style:text-underline-style");
            textStyle.removeProperty("style:text-underline-type");
            textStyle.removeProperty("style:text-underline-width");

            KoGenStyle::copyPropertiesFromStyle(m_currentBulletProperties.textStyle(), textStyle);
            m_currentBulletProperties.setTextStyle(textStyle);
        }
    }

    //---------------------------------------------
    // Fine-tune Paragraph Style
    //---------------------------------------------
    if (m_currentParagraphStyle.parentName().isEmpty()) {
        if (m_context->m_namedDefaultStyles.contains("paragraph")) {
            m_currentParagraphStyle.setParentName(m_context->m_namedDefaultStyles.value("paragraph"));
        }
    }

    // take outline level from style's default-outline-level,
    // there is no text:outline-level equivalent in OOXML
    QString outlineLevelAttribute;
    const KoGenStyle* pstyle = &m_currentParagraphStyle;
    do {
        outlineLevelAttribute = pstyle->attribute("style:default-outline-level");
        // use isNull, empty string value is valid here
        if (! outlineLevelAttribute.isNull()) {
            break;
        }
        // next in hierarchy
        pstyle = mainStyles->style(pstyle->parentName(), "paragraph");
    } while (pstyle);

    const uint outlineLevel = outlineLevelAttribute.toUInt();

    //NOTE: Workaround until text:display="none" support.
    bool isHidden = (m_currentParagraphStyle.property("text:display", KoGenStyle::TextType) == "none");

    // rPr (Run Properties for the Paragraph Mark), ECMA-376, 17.3.1.29, p.253
    // This element specifies the set of run properties applied to the
    // glyph used to represent the physical location of the paragraph
    // mark for this paragraph.  ODF: The paragraph mark formatting
    // does not affect other runs of text so it can NOT be saved into
    // text-properties of the paragraph style.
    //
    if (!textPBuf.isEmpty()) {
        m_currentParagraphStyle.removeAllProperties(KoGenStyle::TextType);
    }

    //---------------------------------------------
    // Process Paragraph Content
    //---------------------------------------------
    if (oldWasCaption || (isHidden && textPBuf.isEmpty())) {
        //nothing
        body = textPBuf.originalWriter();
    } else {
        if (m_dropCapStatus == DropCapRead) {
            body = textPBuf.releaseWriter();
            // If we read a drop cap but there was no text there might as well
            // be no drop cap
            m_dropCapStatus = NoDropCap;
        }
        else if (m_dropCapStatus == DropCapDone) {
            // In case of drop cap, we do not wish there to be a paragraph at
            // this point, this because two ooxml paragraphs with drop cap in
            // first, transfer to one paragraph in odt with drop cap as style.
            body = textPBuf.releaseWriter();
        }
        else {
            body = textPBuf.originalWriter();
            // In ooxml it seems that nothing should be created if sectPr was present
            if (!m_createSectionToNext) {
                if (m_listFound) {

                    // update the size of a bullet picture
                    if ((m_currentBulletProperties.m_type ==
                         MSOOXML::Utils::ParagraphBulletProperties::PictureType) &&
                        (m_currentBulletProperties.bulletSizePt() == "UNUSED")) {

                        int percent = 100;
                        if (m_currentBulletProperties.bulletRelativeSize() != "UNUSED") {
                            STRING_TO_INT(m_currentBulletProperties.bulletRelativeSize(), percent,
                                          QString("PictureType: processing bulletRelativeSize"));
                        }
                        QString fontSize = m_currentParagraphStyle.property("fo:font-size",KoGenStyle::TextType);
                        // Using the default font size at the moment
                        if (fontSize.isEmpty()) {
                            fontSize = m_context->m_defaultFontSizePt;
                        }
                        qreal base = 10; //fair enough
                        if (!fontSize.isEmpty() && fontSize.endsWith(QLatin1String("pt"))) {
                            fontSize.chop(2);
                            STRING_TO_QREAL(fontSize, base, QString("PictureType: processing font-size"));
                        }
                        m_currentBulletProperties.setBulletSizePt(percent * base / 100);
                    }

                    // update automatic numbering info
                    if (m_currentBulletProperties.m_type == MSOOXML::Utils::ParagraphBulletProperties::NumberType) {

                        bool listOpen = false;

                        if (m_continueListNum.contains(m_currentNumId)) {
                            listOpen = true;
                        }
                        else if (!m_currentBulletProperties.startOverride()) {

                            // TODO: The most recent one is required to check
                            // against the correct prevListLevel.
                            //
                            // Check if any of the lists that inherit numbering
                            // from the abstract numbering definition was opened.
                            QStringList numIDs = m_context->m_abstractNumIDs.keys(m_context->m_abstractNumIDs[m_currentNumId]);
                            QStringList::const_iterator i;
                            for (i = numIDs.constBegin(); i != numIDs.constEnd(); ++i) {
                                if (m_continueListNum.contains(*i)) {
                                    listOpen = true;
                                    m_currentNumId = *i;
                                    break;
                                }
                            }
                        }
                        if (listOpen) {
                            if (m_currentListLevel <= m_continueListNum[m_currentNumId].first) {
                                m_continueListNum[m_currentNumId].second = true;
                            } else {
                                m_continueListNum[m_currentNumId].second = false;

                                QString key;
                                int i = m_continueListNum[m_currentNumId].first;
                                while (i > m_currentListLevel) {
                                    key = m_currentNumId;
                                    key.append(QString(".lvl%1").arg(i));
                                    m_numIdXmlId.remove(key);
                                    --i;
                                }
                            }
                        }
                    }

                    m_currentListStyle = KoGenStyle(KoGenStyle::ListAutoStyle);
                    if (m_moveToStylesXml) {
                        m_currentListStyle.setAutoStyleInStylesDotXml(true);
                    }

                    m_currentListStyle.addChildElement("list-style-properties",
                        m_currentBulletProperties.convertToListProperties(*mainStyles, MSOOXML::Utils::DocxFilter));

                    QString listStyleName = mainStyles->insert(m_currentListStyle, QString());
                    Q_ASSERT(!listStyleName.isEmpty());
                    //TODO: continue an opened list based on this information
//                     m_usedListStyles.insertMulti(m_currentNumId, listStyleName);

                    // Start a new list
                    body->startElement("text:list");
                    body->addAttribute("text:style-name", listStyleName);

                    // continue numbering if applicable
                    if (m_currentBulletProperties.m_type == MSOOXML::Utils::ParagraphBulletProperties::NumberType) {

                        QString key = m_currentNumId;
                        key.append(QString(".lvl%1").arg(m_currentListLevel));

                        // Keeping the id readable for debugging purpose
                        QString xmlId = key;
                        xmlId.append(QString("_%1").arg(m_numIdXmlId[key].first)).prepend("lst");
                        xmlId.append(QString("_%1").arg(qrand()));
                        body->addAttribute("xml:id", xmlId);

                        if (m_continueListNum.contains(m_currentNumId)) {
                            if (m_continueListNum[m_currentNumId].second) {
                                body->addAttribute("text:continue-list", m_numIdXmlId[key].second);
                            }
                        }
                        m_numIdXmlId[key].first++;
                        m_numIdXmlId[key].second = xmlId;
                    }
                    body->startElement("text:list-item");
                    for (int i = 0; i < m_currentListLevel; ++i) {
                        body->startElement("text:list");
                        body->startElement("text:list-item");
                    }
                    // restart numbering if applicable
                    if (m_currentBulletProperties.m_type == MSOOXML::Utils::ParagraphBulletProperties::NumberType) {
                        if (!m_continueListNum.contains(m_currentNumId) ||
                            (m_continueListNum.contains(m_currentNumId) &&
                             !m_continueListNum[m_currentNumId].second))
                        {
                            body->addAttribute("text:start-value", m_currentBulletProperties.startValue());
                        }
                        m_continueListNum[m_currentNumId] = qMakePair(m_currentListLevel, false);
                    }
                    m_currentParagraphStyle.addAttribute("style:list-style-name", listStyleName);
                }

                body->startElement((outlineLevel > 0) ? "text:h" : "text:p", false);
                if (outlineLevel > 0) {
                    body->addAttribute("text:outline-level", outlineLevel);
                }
                if (m_currentStyleName.isEmpty()) {
                    QString currentParagraphStyleName;
                    currentParagraphStyleName = (mainStyles->insert(m_currentParagraphStyle));
                    if (sectionAdded) {
                        m_currentSectionStyleName = currentParagraphStyleName;
                        m_currentSectionStyleFamily = m_currentParagraphStyle.familyName();
                    }
                    body->addAttribute("text:style-name", currentParagraphStyleName);
                }
                else {
                    body->addAttribute("text:style-name", m_currentStyleName);
                    if (m_currentStyleName == "Caption") {
                        m_wasCaption = true;
                    }
                }
                //insert the bookmark-start/bookmark-end XML snippet
                if (!m_bookmarkSnippet.isEmpty()) {
                    body->addCompleteElement(m_bookmarkSnippet.toUtf8());
                }

                //insert the floating table XML snippet
                if (!m_floatingTable.isEmpty()) {
                    body->addCompleteElement(m_floatingTable.toUtf8());
                    m_floatingTable.clear();
                }

                (void)textPBuf.releaseWriter();
                body->endElement(); //text:p or text:h

                if (m_listFound) {
                    for (int i = 0; i <= m_currentListLevel; ++i) {
                        body->endElement(); //text:list-item
                        body->endElement(); //text:list
                    }
                }
                debugDocx << "/text:p";
            }
        }
    }

    // True if this was last paragraph of the section, if we were then it means
    // that next paragraph/table should have a new section identifier
    if (m_createSectionToNext) {
        m_createSectionStyle = true;
        m_createSectionToNext = false;
    }

    m_currentStyleName.clear();
    m_insideParagraph = false;

    m_currentParagraphStyle = activeStyles.last();
    activeStyles.pop_back();

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL smartTag
//! smartTag Handler (Smart Tag)
/*
 Parent elements:
 - bdo (§17.3.2.3);
 - customXml (§17.5.1.3);
 - del (§17.13.5.14); dir (§17.3.2.8);
 - [done] fldSimple (§17.16.19);
 - [done] hyperlink (§17.16.22);
 - ins (§17.13.5.18);
 - moveFrom (§17.13.5.22);
 - moveTo (§17.13.5.25);
 - [done] p (§17.3.1.22);
 - [done] sdtContent (§17.5.2.36);
 - [done] smartTag (§17.5.1.9)

 Child elements:
 - bdo (Bidirectional Override) §17.3.2.3
 - [done] bookmarkEnd (Bookmark End) §17.13.6.1
 - [done] bookmarkStart (Bookmark Start) §17.13.6.2
 - commentRangeEnd (Comment Anchor Range End) §17.13.4.3
 - [done] commentRangeStart (Comment Anchor Range Start) §17.13.4.4
 - customXml (Inline-Level Custom XML Element) §17.5.1.3
 - customXmlDelRangeEnd (Custom XML Markup Deletion End) §17.13.5.4
 - customXmlDelRangeStart (Custom XML Markup Deletion Start) §17.13.5.5
 - customXmlInsRangeEnd (Custom XML Markup Insertion End) §17.13.5.6
 - customXmlInsRangeStart (Custom XML Markup Insertion Start) §17.13.5.7
 - customXmlMoveFromRangeEnd (Custom XML Markup Move Source End) §17.13.5.8
 - customXmlMoveFromRangeStart (Custom XML Markup Move Source Start) §17.13.5.9
 - customXmlMoveToRangeEnd (Custom XML Markup Move Destination Location End) §17.13.5.10
 - customXmlMoveToRangeStart (Custom XML Markup Move Destination Location Start) §17.13.5.11
 - [done] del (Deleted Run Content) §17.13.5.14
 - dir (Bidirectional Embedding Level) §17.3.2.8
 - [done] fldSimple (Simple Field) §17.16.19
 - [done] hyperlink (Hyperlink) §17.16.22
 - [done] ins (Inserted Run Content) §17.13.5.18
 - moveFrom (Move Source Run Content) §17.13.5.22
 - moveFromRangeEnd (Move Source Location Container - End) §17.13.5.23
 - moveFromRangeStart (Move Source Location Container - Start) §17.13.5.24
 - moveTo (Move Destination Run Content) §17.13.5.25
 - moveToRangeEnd (Move Destination Location Container - End) §17.13.5.27
 - moveToRangeStart (Move Destination Location Container - Start) §17.13.5.28
 - [done] oMath (Office Math) §22.1.2.77
 - [done] oMathPara (Office Math Paragraph) §22.1.2.78
 - permEnd (Range Permission End) §17.13.7.1
 - permStart (Range Permission Start) §17.13.7.2
 - proofErr (Proofing Error Anchor) §17.13.8.1
 - [done] r (Text Run) §17.3.2.25
 - [done] sdt (Inline-Level Structured Document Tag) §17.5.2.31
 - [done] smartTag (Inline-Level Smart Tag) §17.5.1.9
 - smartTagPr (Smart Tag Properties) §17.5.1.10
 - subDoc (Anchor for Subdocument Location) §17.17.1.1
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_smartTag()
{
    READ_PROLOGUE

    // todo, use as RDF info?

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(r)
            ELSE_TRY_READ_IF(smartTag)
            ELSE_TRY_READ_IF(bookmarkStart)
            ELSE_TRY_READ_IF(bookmarkEnd)
            ELSE_TRY_READ_IF(fldSimple)
            ELSE_TRY_READ_IF(hyperlink)
            ELSE_TRY_READ_IF(commentRangeStart)
            ELSE_TRY_READ_IF(r)
            ELSE_TRY_READ_IF_NS(m, oMathPara)
            ELSE_TRY_READ_IF_NS(m, oMath)
            ELSE_TRY_READ_IF(del)
            ELSE_TRY_READ_IF(ins)
            ELSE_TRY_READ_IF(sdt)
            SKIP_UNKNOWN
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL r
//! r handler (Text Run)
/*! ECMA-376, 17.3.2.25, p.320.

 Parent elements:
 - bdo (§17.3.2.3)
 - customXml (§17.5.1.3)
 - del (§17.13.5.14)
 - dir (§17.3.2.8)
 - [done] fldSimple (§17.16.19)
 - [done] hyperlink (§17.16.22)
 - ins (§17.13.5.18)
 - moveFrom (§17.13.5.22)
 - moveTo (§17.13.5.25)
 - [done] p (§17.3.1.22)
 - rt (§17.3.3.24)
 - rubyBase (§17.3.3.27)
 - [done] sdtContent (§17.5.2.36)
 - [done] smartTag (§17.5.1.9)

 Child elements:
 - annotationRef (Comment Information Block) §17.13.4.1
 - [done] br (Break) §17.3.3.1
 - commentReference (Comment Content Reference Mark) §17.13.4.5
 - contentPart (Content Part) §17.3.3.2
 - continuationSeparator (Continuation Separator Mark) §17.11.1
 - cr (Carriage Return) §17.3.3.4
 - dayLong (Date Block - Long Day Format) §17.3.3.5
 - dayShort (Date Block - Short Day Format) §17.3.3.6
 - delInstrText (Deleted Field Code) §17.16.13
 - [done] delText (Deleted Text) §17.3.3.7
 - [done] drawing (DrawingML Object) §17.3.3.9
 - endnoteRef (Endnote Reference Mark) §17.11.6
 - [done] endnoteReference (Endnote Reference) §17.11.7
 - [done] fldChar (Complex Field Character) §17.16.18
 - footnoteRef (Footnote Reference Mark) §17.11.13
 - [done] footnoteReference (Footnote Reference) §17.11.14
 - [done] instrText (Field Code) §17.16.23
 - [done] lastRenderedPageBreak (Position of Last Calculated Page Break) §17.3.3.13
 - monthLong (Date Block - Long Month Format) §17.3.3.15
 - monthShort (Date Block - Short Month Format) §17.3.3.16
 - noBreakHyphen (Non Breaking Hyphen Character) §17.3.3.18
 - [done] object (Embedded Object) §17.3.3.19
 - pgNum (Page Number Block) §17.3.3.22
 - [done] ptab (Absolute Position Tab Character) §17.3.3.23
 - [done] rPr (Run Properties) §17.3.2.28
 - ruby (Phonetic Guide) §17.3.3.25
 - separator (Footnote/Endnote Separator Mark) §17.11.23
 - softHyphen (Optional Hyphen Character) §17.3.3.29
 - sym (Symbol Character) §17.3.3.30
 - [done] t (Text) §17.3.3.31
 - [done] tab (Tab Character) §17.3.3.32
 - yearLong (Date Block - Long Year Format) §17.3.3.33
 - yearShort (Date Block - Short Year Format) §17.3.3.34

 VML child elements (see Part 4):
 - [done] pict (VML Object) §9.2.2.2
*/
//! @todo support all elements
KoFilter::ConversionStatus DocxXmlDocumentReader::read_r()
{
    READ_PROLOGUE

    m_currentTextStyle = KoGenStyle(KoGenStyle::TextAutoStyle, "text");
    if (m_moveToStylesXml) {
        m_currentTextStyle.setAutoStyleInStylesDotXml(true);
    }

    KoXmlWriter* oldWriter = body;

    // DropCapRead means we have read the w:dropCap attribute on this
    //             paragraph and now have to save the text.
    // DropCapDone means we are in the next paragraph and want to add
    //             the saved text to this paragraph.
    if (m_dropCapStatus == DropCapRead) {
       m_dropCapStatus = DropCapDone;
       m_dropCapBuffer = new QBuffer;
       m_dropCapBuffer->open(QIODevice::ReadWrite);
       m_dropCapWriter = new KoXmlWriter(m_dropCapBuffer);
       body = m_dropCapWriter;
    }
    else if (m_dropCapStatus == DropCapDone) {
        body->addCompleteElement(m_dropCapBuffer);
        delete m_dropCapWriter;
        delete m_dropCapBuffer;
        m_dropCapBuffer = 0;
        m_dropCapWriter = 0;
        m_dropCapStatus = NoDropCap;
    }

    MSOOXML::Utils::XmlWriteBuffer buffer;
    body = buffer.setWriter(body);

    while (!atEnd()) {
        readNext();
        debugDocx << *this;
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(rPr)
            ELSE_TRY_READ_IF(t)
            ELSE_TRY_READ_IF(ptab)
            ELSE_TRY_READ_IF(delText)
            ELSE_TRY_READ_IF(drawing)
            ELSE_TRY_READ_IF(endnoteReference)
            ELSE_TRY_READ_IF(footnoteReference)
            ELSE_TRY_READ_IF(object)
            else if (name() == "pict") {
                // Protecting in case the object is inside a textbox inside a shape
                VMLShapeProperties oldProperties = m_currentVMLProperties;
                TRY_READ(pict)
                m_currentVMLProperties = oldProperties;
            }
            ELSE_TRY_READ_IF(instrText)
            ELSE_TRY_READ_IF(fldChar)
            ELSE_TRY_READ_IF(lastRenderedPageBreak)
            ELSE_TRY_READ_IF(br)
            ELSE_TRY_READ_IF_NS(mc, AlternateContent)
            else  if (qualifiedName() == "w:tab") {
                body->startElement("text:tab");
                body->endElement(); // text:tab
            }
            SKIP_UNKNOWN
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    //NOTE: Workaround until text:display="none" support.
    if (m_currentTextStyle.property("text:display") == "none") {
        body = buffer.originalWriter();
        READ_EPILOGUE
    }

    if (m_currentTextStyle.parentName().isEmpty()) {
        if (m_context->m_namedDefaultStyles.contains("text")) {
            m_currentTextStyle.setParentName(m_context->m_namedDefaultStyles.value("text"));
        }
    }

    // inserted/deleted run content, use a different color and
    // underline/line-through
    if (!m_changeTrackingState.isEmpty()) {
        if (m_changeTrackingState.top() == InsertedRunContent) {
            m_currentTextStyle.addProperty("style:text-underline-mode", "continuous");
            m_currentTextStyle.addProperty("style:text-underline-style", "solid");
            m_currentTextStyle.addProperty("style:text-underline-type", "single");
            m_currentTextStyle.addProperty("style:text-underline-width", "auto");
        } else {
            m_currentTextStyle.addProperty("style:text-line-through-mode", "continuous");
            m_currentTextStyle.addProperty("style:text-line-through-style", "solid");
            m_currentTextStyle.addProperty("style:text-line-through-type", "single");
            m_currentTextStyle.addProperty("style:text-line-through-width", "auto");
        }
        m_currentTextStyle.addProperty("style:text-underline-color", "#800080");
        m_currentTextStyle.addProperty("fo:color", "#800080");
    }

    // We want to write to the higher body level
    body = buffer.originalWriter();
    QString currentTextStyleName;
    if (!m_currentTextStyle.isEmpty() || !m_currentTextStyle.parentName().isEmpty()) {
        currentTextStyleName = mainStyles->insert(m_currentTextStyle);
    }
    if (m_complexCharStatus == ExecuteInstrNow || m_complexCharType == InternalHyperlinkComplexFieldCharType) {
        if (m_complexCharType == HyperlinkComplexFieldCharType || m_complexCharType == InternalHyperlinkComplexFieldCharType) {
            body->startElement("text:a", false);
            body->addAttribute("xlink:type", "simple");
            if (m_complexCharType == HyperlinkComplexFieldCharType) {
                body->addAttribute("xlink:href", QUrl(m_complexCharValue).toEncoded());
            }
            else {
                int spacePosition = m_complexCharValue.indexOf(' ');
                QString textValue(QLatin1Char('#'));
                textValue.append(m_complexCharValue.left(spacePosition));
                m_complexCharValue.remove(0, textValue.length());
                body->addAttribute("xlink:href", QUrl(textValue).toEncoded());
            }
        }
    }

    if (!currentTextStyleName.isEmpty()) {
        body->startElement("text:span", false);
        body->addAttribute("text:style-name", currentTextStyleName);
    }

    m_closeHyperlink = handleSpecialField();

    if (m_complexCharStatus == ExecuteInstrNow) {
        if (m_complexCharType == ReferenceNextComplexFieldCharType) {
            body->startElement("text:bookmark-ref");
            body->addAttribute("text:reference-format", "page");
            body->addAttribute("text:ref-name", m_complexCharValue);
            m_closeHyperlink = true;
        }
        else {
            m_specialCharacters = m_complexCharValue;
            m_closeHyperlink = handleSpecialField();
        }
    }

    if (m_complexCharType == InternalHyperlinkComplexFieldCharType ||
        m_complexCharType == MacroButtonFieldCharType) {
        body->addTextSpan(m_complexCharValue);
    }

    // Writing the internal body of read_t now
    body = buffer.releaseWriter();

    if (m_closeHyperlink) {
        body->endElement(); //either text:bookmark-ref or text:a or text:page-number etc.
        m_closeHyperlink = false;
    }

    if (!currentTextStyleName.isEmpty()) {
        body->endElement(); //text:span
    }

    if (m_complexCharStatus == InstrExecute) {
         if (m_complexCharType == ReferenceComplexFieldCharType) {
             m_complexCharType = ReferenceNextComplexFieldCharType;
         }
    }
    if (m_complexCharStatus == ExecuteInstrNow && m_complexCharType == HyperlinkComplexFieldCharType) {
        body->endElement(); // text:a
    }
    else if (m_complexCharType == InternalHyperlinkComplexFieldCharType) {
        body->endElement(); // text:a
    }

    // Case where there's hyperlink with read_instrText, we only want to use the link
    // with the next r element, not this.
    if (m_complexCharStatus == InstrExecute) {
        m_complexCharStatus = ExecuteInstrNow;
    }

    if (m_dropCapStatus == DropCapDone) {
        body = oldWriter;
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL rPr
//! [1] rPr handler (Run Properties for the Paragraph Mark) WML ECMA-376, 17.3.1.29, p.263,
//!         This element specifies a set of run properties which shall be applied to the contents
//!         of the parent run after all style formatting has been applied to the text.

//! [2] rPr handler (Run Properties) WML ECMA-376, 17.3.2.28, p.331,
//!         This element specifies a set of run properties which shall be applied to the contents
//!         of the parent run after all style formatting has been applied to the text.
/*!
 Parent elements:
 - [1] pPr (§17.3.1.26)
 - [2] ctrlPr (§22.1.2.23)
 - [2] r (§22.1.2.87) - Shared ML
 - [2] r (§17.3.2.25)

 Child elements:
 - [done] b (Bold) §17.3.2.1
 - bCs (Complex Script Bold) §17.3.2.2
 - [done] bdr (Text Border) §17.3.2.4
 - [done] caps (Display All Characters As Capital Letters) §17.3.2.5
 - [done] color (Run Content Color) §17.3.2.6
 - cs (Use Complex Script Formatting on Run) §17.3.2.7
 - del (Deleted Paragraph) §17.13.5.15
 - [done] dstrike (Double Strikethrough) §17.3.2.9
 - eastAsianLayout (East Asian Typography Settings) §17.3.2.10
 - effect (Animated Text Effect) §17.3.2.11
 - em (Emphasis Mark) §17.3.2.12
 - emboss (Embossing) §17.3.2.13
 - fitText (Manual Run Width) §17.3.2.14
 - [done] highlight (Text Highlighting) §17.3.2.15
 - [done] i (Italics) §17.3.2.16
 - iCs (Complex Script Italics) §17.3.2.17
 - imprint (Imprinting) §17.3.2.18
 - ins (Inserted Paragraph) §17.13.5.20
 - kern (Font Kerning) §17.3.2.19
 - [done] lang (Languages for Run Content) §17.3.2.20
 - moveFrom (Move Source Paragraph) §17.13.5.21
 - moveTo (Move Destination Paragraph) §17.13.5.26
 - noProof (Do Not Check Spelling or Grammar) §17.3.2.21
 - oMath (Office Open XML Math) §17.3.2.22
 - [done] outline (Display Character Outline) §17.3.2.23
 - position (Vertically Raised or Lowered Text) §17.3.2.24
 - [done] rFonts (Run Fonts) §17.3.2.26
 - rPrChange (Revision Information for Run Properties on the Paragraph Mark) §17.13.5.30
 - [done] rStyle (Referenced Character Style) §17.3.2.29
 - rtl (Right To Left Text) §17.3.2.30
 - shadow (Shadow) §17.3.2.31
 - [done] shd (Run Shading) §17.3.2.32
 - [done] smallCaps (Small Caps) §17.3.2.33
 - snapToGrid (Use Document Grid Settings For Inter-Character Spacing) §17.3.2.34
 - [done] spacing (Character Spacing Adjustment) §17.3.2.35
 - specVanish (Paragraph Mark Is Always Hidden) §17.3.2.36
 - [done] strike (Single Strikethrough) §17.3.2.37
 - [done] sz (Non-Complex Script Font Size) §17.3.2.38
 - szCs (Complex Script Font Size) §17.3.2.39
 - [done] u (Underline) §17.3.2.40
 - [done] vanish (Hidden Text) §17.3.2.41
 - [done] vertAlign (Subscript/Superscript Text) §17.3.2.42
 - [done] w (Expanded/Compressed Text) §17.3.2.43
 - [done] webHidden (Web Hidden Text) §17.3.2.44
*/
//! @todo support all elements
KoFilter::ConversionStatus DocxXmlDocumentReader::read_rPr()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    Q_ASSERT(m_currentTextStyleProperties == 0);
    m_currentTextStyleProperties = new KoCharacterStyle();

    while (!atEnd()) {
        readNext();
        debugDocx << *this;
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(b)
            ELSE_TRY_READ_IF(i)
            ELSE_TRY_READ_IF(u)
            ELSE_TRY_READ_IF(sz)
            ELSE_TRY_READ_IF(strike)
            ELSE_TRY_READ_IF(dstrike)
            ELSE_TRY_READ_IF(rStyle)
            ELSE_TRY_READ_IF(color)
            ELSE_TRY_READ_IF(highlight)
            ELSE_TRY_READ_IF(lang)
            ELSE_TRY_READ_IF_IN_CONTEXT(shd)
            ELSE_TRY_READ_IF(vertAlign)
            ELSE_TRY_READ_IF(rFonts)
            ELSE_TRY_READ_IF(spacing)
            ELSE_TRY_READ_IF(outline)
            ELSE_TRY_READ_IF(caps)
            ELSE_TRY_READ_IF(smallCaps)
            ELSE_TRY_READ_IF(w)
            ELSE_TRY_READ_IF(webHidden)
            ELSE_TRY_READ_IF(bdr)
            ELSE_TRY_READ_IF(vanish)
            SKIP_UNKNOWN
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    m_currentTextStyleProperties->saveOdf(m_currentTextStyle);

    delete m_currentTextStyleProperties;
    m_currentTextStyleProperties = 0;

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL pPr
//! pPr handler (Paragraph Properties)
/*!
 1. pPr (Paragraph Properties) 17.3.1.26, p.259.
 Parent elements:
 - [done] p (§17.3.1.22) - common reader

 2. pPr (Paragraph Properties) 17.7.5.2, p.725.
 Parent elements:
 - [done] pPrDefault - DocxXmlStylesReader

 3. pPr (Table Style Conditional Formatting Paragraph Properties)  17.7.6.1, p.733.
 Parent elements:
 - tblStylePr (§17.7.6.6)

 4. pPr (Style Paragraph Properties) 17.7.8.2, p.752.
 Parent elements:
 - [done] style (§17.7.4.17) - DocxXmlStylesReader

 5. pPr (Numbering Level Associated Paragraph Properties) 17.9.23, p.818.
 Parent elements:
 - lvl (§17.9.6)
 - lvl (§17.9.7)

 Child elements:
 - adjustRightInd (Automatically Adjust Right Indent When Using Document Grid) §17.3.1.1
 - autoSpaceDE (Automatically Adjust Spacing of Latin and East Asian Text) §17.3.1.2
 - autoSpaceDN (Automatically Adjust Spacing of East Asian Text and Numbers) §17.3.1.3
 - bidi (Right to Left Paragraph Layout) §17.3.1.6
 - cnfStyle (Paragraph Conditional Formatting) §17.3.1.8
 - contextualSpacing (Ignore Spacing Above and Below When Using Identical Styles) §17.3.1.9
 - divId (Associated HTML div ID) §17.3.1.10
 - [done] framePr (Text Frame Properties) §17.3.1.11
 - ind (Paragraph Indentation) §17.3.1.12
 - [done] jc (Paragraph Alignment) §17.3.1.13
 - keepLines (Keep All Lines On One Page) §17.3.1.14
 - keepNext (Keep Paragraph With Next Paragraph) §17.3.1.15
 - kinsoku (Use East Asian Typography Rules for First and Last Character per Line) §17.3.1.16
 - mirrorIndents (Use Left/Right Indents as Inside/Outside Indents) §17.3.1.18
 - [done] numPr (Numbering Definition Instance Reference) §17.3.1.19
 - [done] outlineLvl (Associated Outline Level) §17.3.1.20
 - overflowPunct (Allow Punctuation to Extend Past Text Extents) §17.3.1.21
 - pageBreakBefore (Start Paragraph on Next Page) §17.3.1.23
 - [done] pBdr (Paragraph Borders) §17.3.1.24
 - pPrChange (Revision Information for Paragraph Properties) §17.13.5.29
 - [done] pStyle (Referenced Paragraph Style) §17.3.1.27
 - [done] rPr (Run Properties for the Paragraph Mark) §17.3.1.29
 - [done] sectPr (Section Properties) §17.6.18
 - [done] shd (Paragraph Shading) §17.3.1.31
 - snapToGrid (Use Document Grid Settings for Inter-Line Paragraph Spacing) §17.3.1.32
 - [done] spacing (Spacing Between Lines and Above/Below Paragraph) §17.3.1.33
 - suppressAutoHyphens (Suppress Hyphenation for Paragraph) §17.3.1.34
 - [done] suppressLineNumbers (Suppress Line Numbers for Paragraph) §17.3.1.35
 - suppressOverlap (Prevent Text Frames From Overlapping) §17.3.1.36
 - [done] tabs (Set of Custom Tab Stops) §17.3.1.38
 - textAlignment (Vertical Character Alignment on Line) §17.3.1.39
 - textboxTightWrap (Allow Surrounding Paragraphs to Tight Wrap to Text Box Contents) §17.3.1.40
 - textDirection (Paragraph Text Flow Direction) §17.3.1.41
 - topLinePunct (Compress Punctuation at Start of a Line) §17.3.1.43
 - widowControl (Allow First/Last Line to Display on a Separate Page) §17.3.1.44
 - wordWrap (Allow Line Breaking At Character Level) §17.3.1.45
*/
//! CASE #850 -> CASE #853
//! @todo support all elements
KoFilter::ConversionStatus DocxXmlDocumentReader::read_pPr()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        debugDocx << *this;
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            if (name() == "rPr") {
                m_currentTextStyle = KoGenStyle(KoGenStyle::TextAutoStyle, "text");
                TRY_READ(rPr)
                KoGenStyle::copyPropertiesFromStyle(m_currentTextStyle, m_currentParagraphStyle, KoGenStyle::TextType);
            }
            ELSE_TRY_READ_IF_IN_CONTEXT(shd)
            ELSE_TRY_READ_IF_IN_CONTEXT(jc)
            ELSE_TRY_READ_IF(tabs)
            ELSE_TRY_READ_IF(spacing)
            ELSE_TRY_READ_IF(pStyle)
            ELSE_TRY_READ_IF(numPr)
            ELSE_TRY_READ_IF(pBdr)
            ELSE_TRY_READ_IF(framePr)
            ELSE_TRY_READ_IF(ind)
            ELSE_TRY_READ_IF(suppressLineNumbers)
            ELSE_TRY_READ_IF(sectPr)
            ELSE_TRY_READ_IF(outlineLvl)
            SKIP_UNKNOWN
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL webHidden
//! webHidden handler (Web Hidden Text)
/*!

 Parent elements:
 - [done] rPr (§17.3.1.29)
 - [done] rPr (§17.3.1.30)
 - [done] rPr (§17.5.2.28)
 - [done] rPr (§17.9.25)
 - [done] rPr (§17.7.9.1)
 - [done] rPr (§17.7.5.4)
 - [done] rPr (§17.3.2.28)
 - [done] rPr (§17.5.2.27)
 - [done] rPr (§17.7.6.2)
 - [done]  rPr (§17.3.2.27)

 Child elements:
 - none
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_webHidden()
{
    READ_PROLOGUE

//! TODO: implement hidden

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL bookmarkStart
//! bookmarkStart handler (Bookmark Start)
/*!

 Parent elements:
 - bdo (§17.3.2.3)
 - body (§17.2.2)
 - comment (§17.13.4.2)
 - customXml (§17.5.1.6)
 - customXml (§17.5.1.4)
 - customXml (§17.5.1.5)
 - customXml (§17.5.1.3)
 - deg (§22.1.2.26)
 - del (§17.13.5.14)
 - den (§22.1.2.28)
 - dir (§17.3.2.8)
 - docPartBody (§17.12.6)
 - e (§22.1.2.32)
 - [done] endnote (§17.11.2)
 - fldSimple (§17.16.19)
 - fName (§22.1.2.37)
 - [done] footnote (§17.11.10)
 - [done] ftr (§17.10.3)
 - [done] hdr (§17.10.4)
 - [done] hyperlink (§17.16.22)
 - ins (§17.13.5.18)
 - lim (§22.1.2.52)
 - moveFrom (§17.13.5.22)
 - moveTo (§17.13.5.25)
 - num (§22.1.2.75)
 - oMath (§22.1.2.77)
 - [done p (§17.3.1.22)
 - rt (§17.3.3.24)
 - rubyBase (§17.3.3.27)
 - sdtContent (§17.5.2.34)
 - sdtContent (§17.5.2.33)
 - sdtContent (§17.5.2.35)
 - sdtContent (§17.5.2.36)
 - [done] smartTag (§17.5.1.9)
 - sub (§22.1.2.112)
 - sup (§22.1.2.114)
 - tbl (§17.4.38)
 - tc (§17.4.66)
 - tr (§17.4.79)

 Child elements:
 - none
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_bookmarkStart()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR(name)
    TRY_READ_ATTR(id)

    if (!name.isEmpty() && !id.isEmpty()) {
        MSOOXML::Utils::XmlWriteBuffer buffer;

        //bookmark presence limitation by ODF 1.2
        if (!m_insideParagraph) {
            body = buffer.setWriter(body);
        }
        body->startElement("text:bookmark-start");
        body->addAttribute("text:name", name);
        body->endElement(); // text:bookmark-start
        m_bookmarks[id] = name;

        if (!m_insideParagraph) {
            body = buffer.releaseWriter(m_bookmarkSnippet);
        }
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL bookmarkEnd
//! bookmarkEnd handler (Bookmark End)
/*!

 Parent elements:
 - bdo (§17.3.2.3)
 - body (§17.2.2)
 - comment (§17.13.4.2)
 - customXml (§17.5.1.6)
 - customXml (§17.5.1.4)
 - customXml (§17.5.1.5)
 - customXml (§17.5.1.3)
 - deg (§22.1.2.26)
 - del (§17.13.5.14)
 - den (§22.1.2.28)
 - dir (§17.3.2.8)
 - docPartBody (§17.12.6)
 - e (§22.1.2.32)
 - [done] endnote (§17.11.2)
 - fldSimple (§17.16.19)
 - fName (§22.1.2.37)
 - [done] footnote (§17.11.10)
 - [done] ftr (§17.10.3)
 - [done] hdr (§17.10.4)
 - [done] hyperlink (§17.16.22)
 - ins (§17.13.5.18)
 - lim (§22.1.2.52)
 - moveFrom (§17.13.5.22)
 - moveTo (§17.13.5.25)
 - num (§22.1.2.75)
 - oMath (§22.1.2.77)
 - [done p (§17.3.1.22)
 - rt (§17.3.3.24)
 - rubyBase (§17.3.3.27)
 - sdtContent (§17.5.2.34)
 - sdtContent (§17.5.2.33)
 - sdtContent (§17.5.2.35)
 - sdtContent (§17.5.2.36)
 - [done] smartTag (§17.5.1.9)
 - sub (§22.1.2.112)
 - sup (§22.1.2.114)
 - tbl (§17.4.38)
 - tc (§17.4.66)
 - tr (§17.4.79)

 Child elements:
 - none
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_bookmarkEnd()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR(id)
    if (!id.isEmpty()) {
        MSOOXML::Utils::XmlWriteBuffer buffer;

        //bookmark presence limitation by ODF 1.2
        if (!m_insideParagraph) {
            body = buffer.setWriter(body);
        }
        body->startElement("text:bookmark-end");
        body->addAttribute("text:name", m_bookmarks[id]);
        body->endElement(); // text:bookmark-end

        if (!m_insideParagraph) {
            body = buffer.releaseWriter(m_bookmarkSnippet);
        }
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL numPr
//! numPr handler (Numbering Definition Instance Reference)
/*!
 This element specifies that the current paragraph references a numbering definition instance in the current
 document.

 The presence of this element specifies that the paragraph will inherit the properties specified by the numbering
 definition in the num element (§2.9.16) at the level specified by the level specified in the lvl element (§2.9.7)
 and shall have an associated number positioned before the beginning of the text flow in this paragraph. When
 this element appears as part of the paragraph formatting for a paragraph style, then any numbering level
 defined using the ilvl element shall be ignored, and the pStyle element (§2.9.25) on the associated abstract
 numbering definition shall be used instead.

 Parent elements:
 - [done] pPr (§17.3.1.26);
 - [done] pPr (§17.3.1.25);
 - [done] pPr (§17.7.5.2);
 - [done] pPr (§17.7.6.1);
 - [done] pPr (§17.9.23);
 - [done] pPr (§17.7.8.2)

 Child elements:
 - [done] ilvl (Numbering Level Reference) §17.9.3
 - ins (Inserted Numbering Properties) §17.13.5.19
 - [done] numId (Numbering Definition Instance Reference) §17.9.19
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_numPr()
{
    READ_PROLOGUE

    m_listFound = true;
    m_currentListLevel = 0;

    while (!atEnd()) {
        readNext();
        debugDocx << *this;
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(numId)
            ELSE_TRY_READ_IF(ilvl)
            SKIP_UNKNOWN
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL ilvl
//! ilvl handler (Numbering Level Reference)
/*!
 This element specifies the numbering level of the numbering definition instance which shall be applied to the
 parent paragraph.
 This numbering level is specified on either the abstract numbering definition's lvl element (§2.9.7), and may be
 overridden by a numbering definition instance level override's lvl element (§2.9.6).
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_ilvl()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR(val)
    if (!val.isEmpty()) {
        bool ok = false;
        uint listValue = val.toUInt(&ok);
        if (ok) {
            m_currentListLevel = listValue;
        }
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL numId
//! numPr handler (Numbering Definition Instance Reference)
/*!
 This element specifies that the current paragraph references a numbering
 definition instance in the current document.

 The presence of this element specifies that the paragraph will inherit the
 properties specified by the numbering definition in the num element (§2.9.16)
 at the level specified by the level specified in the lvl element (§2.9.7) and
 shall have an associated number positioned before the beginning of the text
 flow in this paragraph. When this element appears as part of the paragraph
 formatting for a paragraph style, then any numbering level defined using the
 ilvl element shall be ignored, and the pStyle element (§2.9.25) on the
 associated abstract numbering definition shall be used instead.

 Parent elements:
 - [done] numPr (§17.3.1.19)

 Child elements:
 - none
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_numId()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR(val)
    // In docx, this value defines a predetermined style from numbering xml,
    // The styles from numbering have to be given some name, NumStyle has been
    // chosen here
    if (!val.isEmpty()) {
        if (val == "0") {
            m_listFound = false; // spec says that this means deleted list
        } else {
            m_currentBulletList = m_context->m_bulletStyles[val];
            m_currentNumId = val;
        }
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL delText
//! delText (Deleted Text)
/*! ECMA-376, 17.3.3.7, p.351.

  This element specifies that this run contains literal text which shall be
  displayed in the document.  The delText element shall be used for all text
  runs which are part of a region of text that is contained in a deleted region
  using the del element.

  Parent Elements:
  - [done] r (§22.1.2.87)
  - [done] r (§17.3.2.25)

  @todo: attribute xml:space
 */
KoFilter::ConversionStatus DocxXmlDocumentReader::read_delText()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        if (isCharacters()) {
            body->addTextSpan(text().toString());
        }
        BREAK_IF_END_OF(CURRENT_EL)
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL drawing
//! drawing handler (DrawingML Object)
/*! ECMA-376, 17.3.3.9, p.362.

 This element specifies that a DrawingML object is located at this position
 in the run’s contents. The layout properties of this DrawingML object
 are specified using the WordprocessingML Drawing syntax (§20.4, p. 3466).

 Parent elements:
 - background (§17.2.1)
 - numPicBullet (§17.9.21)
 - object (§17.3.3.19)
 - r (§22.1.2.87) - Shared ML
 - [done] r (§17.3.2.25)

 Child elements:
 - [done] anchor (Anchor for Floating DrawingML Object) §20.4.2.3
 - [done] inline (Inline DrawingML Object) §20.4.2.8
*/
//! CASE #1300
//! CASE #1301
//! CASE #1380
KoFilter::ConversionStatus DocxXmlDocumentReader::read_drawing()
{
    READ_PROLOGUE

    m_hyperLink = false;
    m_hasPosOffsetH = false;
    m_hasPosOffsetV = false;
    m_rot = 0;
    m_z_index = 0;

    pushCurrentDrawStyle(new KoGenStyle(KoGenStyle::GraphicAutoStyle, "graphic"));
    if (m_moveToStylesXml) {
        m_currentDrawStyle->setAutoStyleInStylesDotXml(true);
    }

    applyBorders(m_currentDrawStyle, m_textBorderStyles, m_textBorderPaddings);

    m_drawing_anchor = false;
    m_drawing_inline = false;

    MSOOXML::Utils::XmlWriteBuffer buffer;
    body = buffer.setWriter(body);

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF_NS(wp, anchor)
            ELSE_TRY_READ_IF_NS(wp, inline)
            ELSE_WRONG_FORMAT
        }
    }

    body = buffer.originalWriter();

    if (m_hyperLink) {
        body->startElement("text:a");
        body->addAttribute("xlink:type", "simple");
        body->addAttribute("xlink:href", QUrl(m_hyperLinkTarget).toEncoded());
    }

    if (m_context->graphicObjectIsGroup) {
        body->startElement("draw:g");
    } else {
        body->startElement("draw:frame");
        body->addAttribute("draw:layer", "layout");

        if (m_hasPosOffsetH) {
            debugDocx << "m_posOffsetH" << m_posOffsetH;
            m_svgX += m_posOffsetH;
        }
        if (m_hasPosOffsetV) {
            debugDocx << "m_posOffsetV" << m_posOffsetV;
            m_svgY += m_posOffsetV;
        }

        if (!m_docPrName.isEmpty()) { // from docPr/@name
            body->addAttribute("draw:name", m_docPrName);
        }

        if (m_rot == 0) {
            body->addAttribute("svg:x", EMU_TO_CM_STRING(m_svgX));
            body->addAttribute("svg:y", EMU_TO_CM_STRING(m_svgY));
        } else {
            // m_rot is in 1/60,000th of a degree
            qreal angle, xDiff, yDiff;
            MSOOXML::Utils::rotateString(m_rot, m_svgWidth, m_svgHeight, angle, xDiff, yDiff);
            QString rotString = QString("rotate(%1) translate(%2cm %3cm)")
                                .arg(angle).arg((m_svgX + xDiff)/360000).arg((m_svgY + yDiff)/360000);
            body->addAttribute("draw:transform", rotString);
        }

        body->addAttribute("svg:width", EMU_TO_CM_STRING(m_svgWidth));
        body->addAttribute("svg:height", EMU_TO_CM_STRING(m_svgHeight));
    }

//! @todo add more cases for text:anchor-type! use m_drawing_inline and see CASE #1343
    if (m_drawing_inline) {
        body->addAttribute("text:anchor-type", "as-char");
        m_currentDrawStyle->addProperty("style:vertical-rel", "baseline");
    }
    else {
        body->addAttribute("draw:z-index", m_z_index);

        if (m_alignH.isEmpty()) {
            m_currentDrawStyle->addProperty("style:horizontal-pos", "from-left");
        } else {
            m_currentDrawStyle->addProperty("style:horizontal-pos", m_alignH);
        }
        if (m_alignV.isEmpty()) {
            m_currentDrawStyle->addProperty("style:vertical-pos", "from-top");
        } else if (m_alignV == "center") {
            m_currentDrawStyle->addProperty("style:vertical-pos", "middle");
        } else if (m_alignV == "bottom") {
            m_currentDrawStyle->addProperty("style:vertical-pos", "bottom");
        } else if (m_alignV == "top") {
            m_currentDrawStyle->addProperty("style:vertical-pos", "top");
        } else if (m_alignV == "outside") {
            m_currentDrawStyle->addProperty("style:vertical-pos", "bottom");
        } else if (m_alignV == "inside") {
            m_currentDrawStyle->addProperty("style:vertical-pos", "top");
        }

        if (m_relativeFromV == "column") {
            m_currentDrawStyle->addProperty("style:vertical-rel", "paragraph");
        }
        else  if (m_relativeFromV == "bottomMargin") {
            // Not supported properly by ODF, making a best guess
            m_currentDrawStyle->addProperty("style:vertical-rel", "page");
            m_currentDrawStyle->addProperty("style:vertical-pos", "bottom");
        }
        else  if (m_relativeFromV == "insideMargin") {
            // Not supported properly by ODF, making a best guess
            m_currentDrawStyle->addProperty("style:vertical-rel", "page");
            m_currentDrawStyle->addProperty("style:vertical-pos", "top");
        }
        else  if (m_relativeFromV == "line") {
            m_currentDrawStyle->addProperty("style:vertical-rel", "paragraph");
        }
        else  if (m_relativeFromV == "margin") {
            if (m_headerActive || m_footerActive) {
                m_currentDrawStyle->addProperty("style:vertical-rel", "frame-content");
            } else {
                m_currentDrawStyle->addProperty("style:vertical-rel", "page-content");
            }
        }
        else  if (m_relativeFromV == "outsideMargin") {
            // Not supported properly by ODF, making a best guess
            m_currentDrawStyle->addProperty("style:vertical-rel", "page");
            m_currentDrawStyle->addProperty("style:vertical-pos", "bottom");
        }
        else  if (m_relativeFromV == "page") {
            if (m_headerActive || m_footerActive) {
                m_currentDrawStyle->addProperty("style:vertical-rel", "frame");
            } else {
                m_currentDrawStyle->addProperty("style:vertical-rel", "page");
            }
        }
        else  if (m_relativeFromV == "paragraph") {
            m_currentDrawStyle->addProperty("style:vertical-rel", "paragraph");
        }
        else  if (m_relativeFromV == "topMargin") {
            m_currentDrawStyle->addProperty("style:vertical-rel", "page");
            m_currentDrawStyle->addProperty("style:vertical-pos", "top");
        }

        if (m_relativeFromH == "character") {
            m_currentDrawStyle->addProperty("style:horizontal-rel", "char");
            body->addAttribute("text:anchor-type", "char");
        }
        else if (m_relativeFromH == "column") {
            m_currentDrawStyle->addProperty("style:horizontal-rel", "paragraph");
            body->addAttribute("text:anchor-type", "paragraph");
        }
        else if (m_relativeFromH == "insideMargin") {
            m_currentDrawStyle->addProperty("style:horizontal-rel", "page-start-margin");
            body->addAttribute("text:anchor-type", "paragraph");
        }
        else if (m_relativeFromH == "leftMargin") {
            m_currentDrawStyle->addProperty("style:horizontal-rel", "page-start-margin");
            body->addAttribute("text:anchor-type", "paragraph");
        }
        else if (m_relativeFromH == "margin") {
            m_currentDrawStyle->addProperty("style:horizontal-rel", "page-content");
            body->addAttribute("text:anchor-type", "paragraph");
        }
        else if (m_relativeFromH == "outsideMargin") {
            m_currentDrawStyle->addProperty("style:horizontal-rel", "page-end-margin");
            body->addAttribute("text:anchor-type", "paragraph");
        }
        else if (m_relativeFromH == "page") {
            m_currentDrawStyle->addProperty("style:horizontal-rel", "page");
            body->addAttribute("text:anchor-type", "paragraph");
        }
        else if (m_relativeFromH == "rightMargin") {
            m_currentDrawStyle->addProperty("style:horizontal-rel", "page-end-margin");
            body->addAttribute("text:anchor-type", "paragraph");
        }
    }

    const QString styleName(mainStyles->insert(*m_currentDrawStyle, "gr"));
    body->addAttribute("draw:style-name", styleName);

    popCurrentDrawStyle();

    (void)buffer.releaseWriter();

    body->endElement(); // draw:frame/draw:g

    if (m_hyperLink) {
        body->endElement(); // text:a
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL framePr
//! framePr handler (Text Frame Properties)
/*!
 Parent elements:
 - [done] pPr (§17.3.1.26)
 - [done] pPr (§17.3.1.25)
 - [done] pPr (§17.7.5.2)
 - [done] pPr (§17.7.6.1)
 - [done] pPr (§17.9.23)
 - [done] pPr (§17.7.8.2)

 No child elements.
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_framePr()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR(dropCap)
    TRY_READ_ATTR(lines)
    TRY_READ_ATTR(hSpace)

    // MS 2007 behaves so that it ignores text and paragraph styles in
    // case of drop cap for the first letter(s), here we set a
    // variable to show we have encountered a drop cap.
    if (!dropCap.isEmpty()) {
        m_dropCapStatus = DropCapRead;
        m_dropCapDistance = 0;
        m_dropCapLines.clear();
        if (!lines.isEmpty()) {
            m_dropCapLines = lines;
        }

        if (!hSpace.isEmpty()) {
            bool ok;
            const qreal distance = qreal(TWIP_TO_POINT(hSpace.toDouble(&ok)));

            if (ok) {
                m_dropCapDistance = distance;
            }
        }
    }

//! @todo more attributes

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL suppressLineNumbers
//! w:suppressLineNumbers handler
/*
 Parent elements:
 - [done] pPr (§17.3.1.26)
 - [done] pPr (§17.3.1.25)
 - [done] pPr (§17.7.5.2)
 - [done] pPr (§17.7.6.1)
 - [done] pPr (§17.9.23)
 - [done] pPr (§17.7.8.2)

*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_suppressLineNumbers()
{
    READ_PROLOGUE

    m_currentParagraphStyle.addProperty("text:number-lines", "false");

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL ind
//! ind handler
//! CASE
KoFilter::ConversionStatus DocxXmlDocumentReader::read_ind()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR(left)
    bool ok = false;
    const qreal leftInd = qreal(TWIP_TO_POINT(left.toDouble(&ok)));
    if (ok) {
        m_currentParagraphStyle.addPropertyPt("fo:margin-left", leftInd);
    }

    // TODO: Values in {none, first-line, hanging) are from the
    // "Special" field of the Paragraph dialog in MS Word.  The
    // tab-stop position in case of a list item and the implicit
    // tab-stop for a paragraph with hanging indent depend on this.
    // Check the MsooXmlUtils::convertToListProperties function.
    TRY_READ_ATTR(firstLine)
    TRY_READ_ATTR(hanging)
    if (!hanging.isEmpty()) {
        const qreal firstInd = qreal(TWIP_TO_POINT(hanging.toDouble(&ok)));
        if (ok) {
           m_currentParagraphStyle.addPropertyPt("fo:text-indent", -firstInd);
        }
    }
    else if (!firstLine.isEmpty()) {
        const qreal firstInd = qreal(TWIP_TO_POINT(firstLine.toDouble(&ok)));
        if (ok) {
           m_currentParagraphStyle.addPropertyPt("fo:text-indent", firstInd);
        }
    }

    TRY_READ_ATTR(right)
    const int rightInd = qreal(TWIP_TO_POINT(right.toDouble(&ok)));
    if (ok) {
        m_currentParagraphStyle.addPropertyPt("fo:margin-right", rightInd);
    }
//! @todo more attributes

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL outlineLvl
//! ind handler
//! CASE
KoFilter::ConversionStatus DocxXmlDocumentReader::read_outlineLvl()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR(val)
    if (!val.isEmpty()) {
        bool ok = false;
        const uint outlineLevelValue = val.toUInt(&ok);
        if (ok && outlineLevelValue <= 9) {
            // ooxml's levels starts at 0, odf's at 1, ooxml 9 means: no level
            // Set non-null QString for no level, to allow seeing the difference
            // between non-set and set-but-empty value with KoGenStyle::attribute
            // ODF §19.470 style:default-outline-level says:
            // The style:default-outline-level attribute value can be empty.
            // If empty, this attribute does not inherit a list style value from a parent style.
            const QString odfOutlineLevelValue = (outlineLevelValue == 9) ?
                 QString("") : QString::number(outlineLevelValue + 1);
            m_currentParagraphStyle.addAttribute("style:default-outline-level", odfOutlineLevelValue);
        }
    }

    readNext();
    READ_EPILOGUE
}


#undef CURRENT_EL
#define CURRENT_EL b
//! b handler
//! CASE #1112
KoFilter::ConversionStatus DocxXmlDocumentReader::read_b()
{
    READ_PROLOGUE
    if (READ_BOOLEAN_VAL) {
        m_currentTextStyle.addProperty("fo:font-weight", "bold");
    } else {
        m_currentTextStyle.addProperty("fo:font-weight", "normal");
    }
    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL i
//! i handler
//! CASE #1112
KoFilter::ConversionStatus DocxXmlDocumentReader::read_i()
{
    READ_PROLOGUE
    if (READ_BOOLEAN_VAL) {
        m_currentTextStyle.addProperty("fo:font-style", "italic");
    } else {
        m_currentTextStyle.addProperty("fo:font-style", "normal");
    }
    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL u
//! u handler
//! CASE #1149
KoFilter::ConversionStatus DocxXmlDocumentReader::read_u()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR(val)
//! @todo more styles
    MSOOXML::Utils::setupUnderLineStyle(val, m_currentTextStyleProperties);

    TRY_READ_ATTR(color)
    QColor c(MSOOXML::Utils::ST_HexColorRGB_to_QColor(color));
    if (c.isValid()) {
        m_currentTextStyleProperties->setUnderlineColor(c);
    }
    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL sz
//! sz handler (Non-Complex Script Font Size) ECMA-376, 17.3.2.38, p.342
/*!    This element specifies the font size which shall be applied to all
       non complex script characters in the contents of this run when displayed.
*/
/*!
 Parent elements:
 - [done] rPr (§17.3.1.29)
 - rPr (§17.3.1.30)
 - rPr (§17.5.2.28)
 - rPr (§17.9.25)
 - [done] rPr (§17.7.9.1)
 - [done] rPr (§17.7.5.4) (within style)
 - [done] rPr (§17.3.2.28)
 - [done] rPr (§17.5.2.27)
 - [done] rPr (§17.7.6.2)
 - [done] rPr (§17.3.2.27)
 No child elements.
*/
//! @todo support all elements
//! CASE #1162
KoFilter::ConversionStatus DocxXmlDocumentReader::read_sz()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    READ_ATTR(val)
    bool ok;
    const qreal pointSize = qreal(val.toUInt(&ok)) / 2.0; /* half-points */
    if (ok) {
        // In case of drop cap, text size should not be read
        if (m_dropCapStatus != DropCapDone) {
            m_currentTextStyleProperties->setFontPointSize(pointSize);
        }
    }
    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL jc
//! Paragraph Alignment
/*! ECMA-376, 17.3.1.13, p.239
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_jc(jcCaller caller)
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    READ_ATTR(val)
    // Does ODF support high/low/medium kashida ?
    val = val.toLower();
    if ((val == "both") || (val == "distribute")) {
        if (caller == jc_pPr) {
            m_currentParagraphStyle.addProperty("fo:text-align", "justify");
        }
        else {
            m_tableMainStyle->setHorizontalAlign(KoTblStyle::CenterAlign);
        }
    }
    else if (val == "center") {
        if (caller == jc_pPr) {
            m_currentParagraphStyle.addProperty("fo:text-align", val);
        }
        else {
            m_tableMainStyle->setHorizontalAlign(KoTblStyle::CenterAlign);
        }
    }
    else if ((val == "start") || (val == "left")) {
        if (caller == jc_pPr) {
            m_currentParagraphStyle.addProperty("fo:text-align", "start");
        }
        else {
            m_tableMainStyle->setHorizontalAlign(KoTblStyle::LeftAlign);
        }
    }
    else if ((val == "right") || (val == "end")) {
        if (caller == jc_pPr) {
            m_currentParagraphStyle.addProperty("fo:text-align", "end");
        }
        else {
            m_tableMainStyle->setHorizontalAlign(KoTblStyle::RightAlign);
        }
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL spacing
//! spacing handler (Spacing Between Lines and Above/Below Paragraph)
/*! ECMA-376, 17.3.1.33, p.269.

 This element specifies the inter-line and inter-paragraph spacing which shall be applied
 to the contents of this paragraph when it is displayed by a consumer.

 Parent elements:
 - [done] pPr (§17.3.1.26)
 - [done] pPr (§17.3.1.25)
 - [done] pPr (§17.7.5.2)
 - [done] pPr (§17.7.6.1)
 - [done] pPr (§17.9.23)
 - [done] pPr (§17.7.8.2)

 No child elements.

 @todo support all elements
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_spacing()
{
    //TODO: afterLines, beforeLines are critical.

    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    bool ok = true;
    int marginBottom = 10;
    bool afterAutospacing = MSOOXML::Utils::convertBooleanAttr(attrs.value("w:afterAutospacing").toString());
    if (!afterAutospacing) {
        TRY_READ_ATTR(after)
        marginBottom = TWIP_TO_POINT(after.toDouble(&ok));
    }
    if (ok) {
        m_currentParagraphStyle.addPropertyPt("fo:margin-bottom", marginBottom);
    }

    ok = true;
    int marginTop = 5;
    bool beforeAutospacing = MSOOXML::Utils::convertBooleanAttr(attrs.value("w:beforeAutospacing").toString());
    if (!beforeAutospacing) {
        TRY_READ_ATTR(before)
        marginTop = TWIP_TO_POINT(before.toDouble(&ok));
    }
    if (ok) {
        m_currentParagraphStyle.addPropertyPt("fo:margin-top", marginTop);
    }

    // for rPr
    TRY_READ_ATTR(val)

    const qreal pointSize = (TWIP_TO_POINT(val.toDouble(&ok)));

    if (ok) {
        m_currentTextStyle.addPropertyPt("fo:letter-spacing", qreal(pointSize) / 100.0);
    }

    TRY_READ_ATTR(lineRule)
    TRY_READ_ATTR(line)
    qreal lineSpace = line.toDouble(&ok);

    if (ok) {
        if (lineRule == "atLeast") {
            lineSpace = TWIP_TO_POINT(lineSpace);
            m_currentParagraphStyle.addPropertyPt("style:line-height-at-least", lineSpace);
        } else if (lineRule == "exact") {
            lineSpace = TWIP_TO_POINT(lineSpace);
            m_currentParagraphStyle.addPropertyPt("fo:line-height", lineSpace);
        }
        else {
            lineSpace = lineSpace / 2.4; // converting to percentage
            QString space = "%1";
            space = space.arg(lineSpace);
            space.append('%');

            m_currentParagraphStyle.addProperty("fo:line-height", space);
        }
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL shd
//! Shading handler (object's shading attributes)
/*! ECMA-376, 17.3.5, p.399
 Parent Elements:
 - [done] pPr (Paragraph Properties) §17.3.1.26
 - [done] tcPr (Table Cell Properties) §17.4.70
 Attributes:
 - color (Shading Pattern Color)
 - fill (Shading Background Color)
 - themeColor (Shading Pattern Theme Color)
 - themeFill (Shading Background Theme Color)
 - themeFillShade (Shading Background Theme Color Shade)
 - themeFillTint (Shading Background Theme Color Tint)
 - themeShade (Shading Pattern Theme Color Shade)
 - themeTint (Shading Pattern Theme Color Tint)
 - [done] val (Shading Pattern)
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_shd(shdCaller caller)
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    READ_ATTR(val)
    val = val.toLower();
    TRY_READ_ATTR(color)
//debugDocx << m_callsNames;
    if (!color.isEmpty() && color != MsooXmlReader::constAuto) {
        QColor clr(MSOOXML::Utils::ST_HexColorRGB_to_QColor(color));
        if (caller == shd_rPr && clr.isValid() && val == "solid") {
            m_currentTextStyleProperties->setBackground(clr);
        }
    }

    TRY_READ_ATTR(fill)
    QString fillColor = fill.toLower();
    if (!fillColor.isEmpty() && fillColor != MsooXmlReader::constAuto) {
        fillColor.prepend("#");
        if (caller == shd_pPr) {
            m_currentParagraphStyle.addProperty("fo:background-color", fillColor);
        }
        if (caller == shd_tcPr) {
            m_currentTableStyleProperties->backgroundColor = fillColor;
            m_currentTableStyleProperties->setProperties |= MSOOXML::TableStyleProperties::BackgroundColor;
        }
        if (caller == shd_rPr && val == "clear") {
            if (m_currentTextStyleProperties->background() == QBrush()) {
                QColor clr(fillColor);
                m_currentTextStyleProperties->setBackground(clr);
            }
        }
    }
    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL rFonts
//! w:lang handler (Run Fonts)
/*! ECMA-376, 17.3.2.26, p.323.
 Parent elements:
 - rPr (§17.3.1.29) (within p)
 - rPr (§17.3.1.30)
 - rPr (§17.5.2.28)
 - rPr (§17.9.25)
 - rPr (§17.7.9.1)
 - rPr (§17.7.5.4) (within style)
 - rPr (§17.3.2.28)
 - rPr (§17.5.2.27)
 - rPr (§17.7.6.2)
 - rPr (§17.3.2.27)
 No child elements.

 CASE #850 -> CASE #858 -> CASE #861 -> CASE #1150

 @todo support all elements
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_rFonts()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
// CASE #1153
    TRY_READ_ATTR(ascii)
    if (!ascii.isEmpty()) {
        m_currentTextStyle.addProperty("style:font-name", ascii, KoGenStyle::TextType);
    }
// CASE #1152
    if (ascii.isEmpty()) {
        TRY_READ_ATTR(asciiTheme)
        if (!asciiTheme.isEmpty()) {
            QString font = asciiTheme;
            if (font.startsWith(QLatin1String("major"))) {
                font = m_context->themes->fontScheme.majorFonts.latinTypeface;
            }
            else if (font.startsWith(QLatin1String("minor"))) {
               font = m_context->themes->fontScheme.minorFonts.latinTypeface;
            }
            m_currentTextStyle.addProperty("style:font-name", font);
        }
    }
// CASE #1155
    TRY_READ_ATTR(cs)
    if (!cs.isEmpty()) {
        m_currentTextStyle.addProperty("style:font-name-complex", cs, KoGenStyle::TextType);
    }
// CASE #1154
    if (cs.isEmpty()) {
        TRY_READ_ATTR(cstheme)
        if (!cstheme.isEmpty()) {
            //! @todo
        }
    }
// CASE #1157
    TRY_READ_ATTR(eastAsia)
    if (!eastAsia.isEmpty()) {
        m_currentTextStyle.addProperty("style:font-name-asian", eastAsia, KoGenStyle::TextType);
    }
// CASE #1156
    if (eastAsia.isEmpty()) {
        TRY_READ_ATTR(eastAsiaTheme)
        if (!eastAsiaTheme.isEmpty()) {
            //! @todo
        }
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL pStyle
//! pStyle handler (Referenced Paragraph Style)
/*! ECMA-376, WML, 17.3.1.27, p.260.

 This element specifies the style ID of the paragraph style which shall be used
 to format the contents of this paragraph.  This formatting is applied at the
 following location in the style hierarchy:

 - Document defaults
 - Table styles
 - Numbering styles
 - Paragraph styles (this element)
 - Character styles
 - Direct Formatting

 This means that all properties specified in the style element (§17.7.4.17)
 with a styleId which corresponds to the value in this element's val attribute
 are applied to the paragraph at the appropriate level in the hierarchy.

 Parent elements:
 - [done] pPr (§17.3.1.26)
 - [done] pPr (§17.3.1.25)
 - [done] pPr (§17.7.5.2)
 - [done] pPr (§17.7.6.1)
 - [done] pPr (§17.9.23)
 - [done] pPr (§17.7.8.2)

 No child elements.
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_pStyle()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR(val)

    m_currentParagraphStyle.setParentName(val);
    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL tcMar
//! tcMar (cell margin)
/*!
 Parent elements:
 - ....

 Child elements:
 - ...

 @todo support all elements
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_tcMar()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            const QXmlStreamAttributes attrs(attributes());
            if (QUALIFIED_NAME_IS(top)) {
                READ_ATTR(w)
                m_currentTableStyleProperties->topMargin = TWIP_TO_POINT(w.toDouble());
                m_currentTableStyleProperties->setProperties |= MSOOXML::TableStyleProperties::TopMargin;
            }
            else if (QUALIFIED_NAME_IS(left)) {
                READ_ATTR(w)
                m_currentTableStyleProperties->leftMargin = TWIP_TO_POINT(w.toDouble());
                m_currentTableStyleProperties->setProperties |= MSOOXML::TableStyleProperties::LeftMargin;
            }
            else if (QUALIFIED_NAME_IS(bottom)) {
                READ_ATTR(w)
                m_currentTableStyleProperties->bottomMargin = TWIP_TO_POINT(w.toDouble());
                m_currentTableStyleProperties->setProperties |= MSOOXML::TableStyleProperties::BottomMargin;
            }
            else if (QUALIFIED_NAME_IS(right)) {
                READ_ATTR(w)
                m_currentTableStyleProperties->rightMargin = TWIP_TO_POINT(w.toDouble());
                m_currentTableStyleProperties->setProperties |= MSOOXML::TableStyleProperties::RightMargin;
            }
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL tblCellMar
//! tblCellMar (cell margin defaults)
/*!
 Parent elements:
 - [done] tblPr (§17.4.60)
 - [done] tblPr (§17.4.59)
 - [done] tblPr (§17.7.6.4)
 - [done] tblPr (§17.7.6.3)

 Child elements:
 - ...

 @todo support all elements
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_tblCellMar()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            const QXmlStreamAttributes attrs(attributes());
            if (QUALIFIED_NAME_IS(top)) {
                READ_ATTR(w)
                m_currentTableStyleProperties->topMargin = TWIP_TO_POINT(w.toDouble());
                m_currentTableStyleProperties->setProperties |= MSOOXML::TableStyleProperties::TopMargin;
            }
            else if (QUALIFIED_NAME_IS(left)) {
                READ_ATTR(w)
                m_currentTableStyleProperties->leftMargin = TWIP_TO_POINT(w.toDouble());
                m_currentTableStyleProperties->setProperties |= MSOOXML::TableStyleProperties::LeftMargin;
            }
            else if (QUALIFIED_NAME_IS(bottom)) {
                READ_ATTR(w)
                m_currentTableStyleProperties->bottomMargin = TWIP_TO_POINT(w.toDouble());
                m_currentTableStyleProperties->setProperties |= MSOOXML::TableStyleProperties::BottomMargin;
            }
            else if (QUALIFIED_NAME_IS(right)) {
                READ_ATTR(w)
                m_currentTableStyleProperties->rightMargin = TWIP_TO_POINT(w.toDouble());
                m_currentTableStyleProperties->setProperties |= MSOOXML::TableStyleProperties::RightMargin;
            }
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL tblBorders
//! tblBorders handler (Table borders)
/*!
 Parent elements:
 - [done] tblPr (§17.4.60)
 - [done] tblPr (§17.4.59)
 - [done] tblPr (§17.7.6.4)
 - [done] tblPr (§17.7.6.3)

 Child elements:
 - [done] bottom (Table Bottom Border) §17.4.4
 - [done] end (Table Trailing Edge Border) §17.4.13
 - [done] insideH (Table Inside Horizontal Edges Border) §17.4.23
 - [done] insideV (Table Inside Vertical Edges Border) §17.4.25
 - [done] start (Table Leading Edge Border) §17.4.37
 - [done] top (Table Top Border) §17.4.77
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_tblBorders()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            if (QUALIFIED_NAME_IS(top)) {
                m_currentTableStyleProperties->top = getBorderData();
                m_currentTableStyleProperties->setProperties |= MSOOXML::TableStyleProperties::TopBorder;
            }
            else if (QUALIFIED_NAME_IS(bottom)) {
                m_currentTableStyleProperties->bottom = getBorderData();
                m_currentTableStyleProperties->setProperties |= MSOOXML::TableStyleProperties::BottomBorder;
            }
            else if (QUALIFIED_NAME_IS(left)) {
                m_currentTableStyleProperties->left = getBorderData();
                m_currentTableStyleProperties->setProperties |= MSOOXML::TableStyleProperties::LeftBorder;
            }
            else if (QUALIFIED_NAME_IS(right)) {
                m_currentTableStyleProperties->right = getBorderData();
                m_currentTableStyleProperties->setProperties |= MSOOXML::TableStyleProperties::RightBorder;
            }
            else if (QUALIFIED_NAME_IS(insideV)) {
                m_currentTableStyleProperties->insideV = getBorderData();
                m_currentTableStyleProperties->setProperties |= MSOOXML::TableStyleProperties::InsideVBorder;
            }
            else if (QUALIFIED_NAME_IS(insideH)) {
                m_currentTableStyleProperties->insideH = getBorderData();
                m_currentTableStyleProperties->setProperties |= MSOOXML::TableStyleProperties::InsideHBorder;
            }
        }
    }

    READ_EPILOGUE
}

KoBorder::BorderData DocxXmlDocumentReader::getBorderData()
{
    const QXmlStreamAttributes attrs(attributes());

    KoBorder::BorderData borderData;

    TRY_READ_ATTR(val)
    borderData.style = borderMap.value(val);

    TRY_READ_ATTR(themeColor)
    TRY_READ_ATTR(color)

    if (!color.isEmpty()) {
        QString colorString = QString("#").append(color);
        borderData.innerPen.setColor(QColor(colorString));
        borderData.outerPen.setColor(QColor(colorString));
    }

    // Fallback to theme
    if (!borderData.innerPen.color().isValid() && !themeColor.isEmpty()) {

        MSOOXML::DrawingMLColorSchemeItemBase *colorItem = 0;
        colorItem = m_context->themes->colorScheme.value(themeColor);
        if (colorItem) {
            borderData.innerPen.setColor(colorItem->value());
            borderData.outerPen.setColor(colorItem->value());
        }
    }

    TRY_READ_ATTR(sz)
    borderData.outerPen.setWidthF(sz.toDouble() / 8.0);

    return borderData;
}

#undef CURRENT_EL
#define CURRENT_EL tblpPr
//! tblpPr handler (Floating Table Positioning)
/*! ECMA-376, 17.4.58, p. 475.

 Parent elements:
 - [done] tblPr (§17.4.60)
 - [done] tblPr (§17.4.59)
 - [done] tblPr (§17.7.6.4)
 - [done] tblPr (§17.7.6.3)

 Child elements:
 - none
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_tblpPr()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR(bottomFromText)
    TRY_READ_ATTR(leftFromText)
    TRY_READ_ATTR(rightFromText)
    TRY_READ_ATTR(topFromText)

    TRY_READ_ATTR(horzAnchor)
    TRY_READ_ATTR(vertAnchor)

    TRY_READ_ATTR(tblpX)
    TRY_READ_ATTR(tblpXSpec)
    TRY_READ_ATTR(tblpY)
    TRY_READ_ATTR(tblpYSpec)

    //If the tblpYSpecX/tblpYSpec attribute is also specified, then tblpX/tblpY
    //value is ignored.
    m_svgX = m_svgY = 0;
    if (tblpXSpec.isEmpty()) {
        STRING_TO_INT(tblpX, m_svgX, QString("w:tblpX"));
    }
    if (tblpYSpec.isEmpty()) {
        STRING_TO_INT(tblpY, m_svgY, QString("w:tblpY"));
    }

    int bottom = 0, left =0, right = 0, top = 0;
    STRING_TO_INT(bottomFromText, bottom, QString("w:bottomFromText"));
    STRING_TO_INT(leftFromText, left, QString("w:leftFromText"));
    STRING_TO_INT(rightFromText, right, QString("w:rightFromText"));
    STRING_TO_INT(topFromText, top, QString("w:topFromText"));

    pushCurrentDrawStyle(new KoGenStyle(KoGenStyle::GraphicAutoStyle, "graphic"));
    if (m_moveToStylesXml) {
        m_currentDrawStyle->setAutoStyleInStylesDotXml(true);
    }
    //fo:margin
    m_currentDrawStyle->addPropertyPt("fo:margin-bottom", TWIP_TO_POINT(bottom));
    m_currentDrawStyle->addPropertyPt("fo:margin-left", TWIP_TO_POINT(left));
    m_currentDrawStyle->addPropertyPt("fo:margin-right", TWIP_TO_POINT(right));
    m_currentDrawStyle->addPropertyPt("fo:margin-top", TWIP_TO_POINT(top));
    //style:horizontal-rel
    if (horzAnchor.isEmpty()) {
        m_currentDrawStyle->addProperty("style:horizontal-rel", "page");
    }
    else if (horzAnchor == "margin") {
        m_currentDrawStyle->addProperty("style:horizontal-rel", "page-content");
    }
    else if (horzAnchor == "page") {
        m_currentDrawStyle->addProperty("style:horizontal-rel", "page");
    }
    else if (horzAnchor == "text") {
        m_currentDrawStyle->addProperty("style:horizontal-rel", "paragraph");
    }
    //style:vertical-rel
    if (vertAnchor.isEmpty()) {
        if (m_headerActive || m_footerActive) {
            m_currentDrawStyle->addProperty("style:vertical-rel", "frame");
        } else {
           m_currentDrawStyle->addProperty("style:vertical-rel", "page");
        }
    }
    else if (vertAnchor == "margin") {
        m_currentDrawStyle->addProperty("style:vertical-rel", "page-content");
    }
    else if (vertAnchor == "page") {
        if (m_headerActive || m_footerActive) {
            m_currentDrawStyle->addProperty("style:vertical-rel", "frame");
        } else {
            m_currentDrawStyle->addProperty("style:vertical-rel", "page");
        }
    }
    else if (vertAnchor == "text") {
        m_currentDrawStyle->addProperty("style:vertical-rel", "paragraph");
    }
    //style:horizontal-pos
    if (tblpXSpec.isEmpty()) {
        m_currentDrawStyle->addProperty("style:horizontal-pos", "from-left");
    } else {
        m_currentDrawStyle->addProperty("style:horizontal-pos", tblpXSpec);
    }
    //style:vertical-pos
    if (tblpYSpec.isEmpty()) {
        m_currentDrawStyle->addProperty("style:vertical-pos", "from-top");
    }
    else if (tblpYSpec == "center") {
        m_currentDrawStyle->addProperty("style:vertical-pos", "middle");
    }
    else if (tblpYSpec == "top") {
        m_currentDrawStyle->addProperty("style:vertical-pos", "top");
    }
    else if (tblpYSpec == "inside") {
        m_currentDrawStyle->addProperty("style:vertical-pos", "top");
    }
    else if (tblpYSpec == "bottom") {
        m_currentDrawStyle->addProperty("style:vertical-pos", "bottom");
    }
    else if (tblpYSpec == "outside") {
        m_currentDrawStyle->addProperty("style:vertical-pos", "bottom");
    }
    //style:wrap
    if (tblpXSpec.isEmpty() || (tblpXSpec == "left")) {
        m_currentDrawStyle->addProperty("style:wrap", "right");
    }
    else if (tblpXSpec == "right") {
        m_currentDrawStyle->addProperty("style:wrap", "left");
    } else {
        m_currentDrawStyle->addProperty("style:wrap", "parallel");
    }
    m_currentDrawStyle->addProperty("style:number-wrapped-paragraphs", "no-limit");
    //draw:auto-grow-height
    m_currentDrawStyle->addProperty("draw:auto-grow-height", "true");

    m_currentDrawStyleName = mainStyles->insert(*m_currentDrawStyle);
    popCurrentDrawStyle();

    readNext();

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL tblStyle
//! tblStyle handler (Referenced table style)
/*!
 Parent elements:
 - [done] tblPr (§17.4.60)
 - [done] tblPr (§17.4.59)
 - [done] tblPr (§17.7.6.4)
 - [done] tblPr (§17.7.6.3)

 Child elements:
 - none
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_tblStyle()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR(val)

    m_currentTableStyleName = val;

    //Inheriting values from the defined style
    MSOOXML::DrawingTableStyle* tableStyle = m_context->m_tableStyles.value(m_currentTableStyleName);
    Q_ASSERT(tableStyle);
    if (tableStyle) {
        m_tableMainStyle->setHorizontalAlign(tableStyle->mainStyle->horizontalAlign());
    }

    readNext();

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL rStyle
//! rStyle handler (Referenced run Style)
/*!

 Parent elements:
 - rPr (§17.3.1.29)
 - rPr (§17.3.1.30)
 - rPr (§17.5.2.28)
 - rPr (§17.9.25)
 - rPr (§17.7.9.1)
 - rPr (§17.7.5.4)
 - rPr (§17.3.2.28)
 - rPr (§17.5.2.27)
 - rPr (§17.7.6.2)
 - rPr (§17.3.2.27)

 No child elements.
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_rStyle()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR(val)

    m_currentTextStyle.setParentName(val);

    readNext();
    READ_EPILOGUE
}

bool DocxXmlDocumentReader::handleSpecialField()
{
    if (m_specialCharacters.isEmpty()) {
        return false;
    }
    QString instr = m_specialCharacters.trimmed();
    m_specialCharacters.clear();
    QVector<QString> instructions;
    while (instr.indexOf(' ') > 0) {
        int place = instr.indexOf(' ');
        instructions.push_back(instr.left(place));
        instr.remove(0, place + 1);
    }
    instructions.push_back(instr);
    QString command = instructions.at(0);

    bool returnTrue = true;

    if (command == "AUTHOR") {
        body->startElement("text:author-name");
    }
    else if (command == "CREATEDATE") {
        body->startElement("text:creation-date");
    }
    else if (command == "DATE") {
        body->startElement("text:date");
    }
    else if (command == "EDITIME") {
        body->startElement("text:modification-time");
    }
    else if (command == "FILENAME") {
        body->startElement("text:file-name");
    }
    else if (command == "NUMPAGES") {
        body->startElement("text:page-count");
    }
    else if (command == "NUMWORDS") {
        body->startElement("text:word-count");
    }
    else if (command == "PAGE") {
        body->startElement("text:page-number");
        body->addAttribute("text:select-page", "current");
    }
    else if (command == "PRINTDATE") {
        body->startElement("text:print-date");
    }
    else if (command == "REF") {
        if ((instructions.size() > 3) && instructions.contains("\\h")) {
            body->startElement("text:bookmark-ref");
            body->addAttribute("text:reference-format", "page");
            body->addAttribute("text:ref-name", instructions.at(1));
        }
        else {
            returnTrue = false;
        }
    }
    else if (command == "TIME") {
        body->startElement("text:time");
    }
    else if (command == "SAVEDATE") {
        body->startElement("text:modification-date");
    }
    else {
        returnTrue = false;
    }

    return returnTrue;
}

#undef CURRENT_EL
#define CURRENT_EL fldSimple
//! fldSimple handler (Simple Field)
/*!

 Parent elements:
 - bdo (§17.3.2.3)
 - customXml (§17.5.1.3)
 - dir (§17.3.2.8)
 - [done] fldSimple (§17.16.19)
 - hyperlink (§17.16.22)
 - [done] p (§17.3.1.22)
 - [done] sdtContent (§17.5.2.36)
 - [done] smartTag (§17.5.1.9)

 Child elements:

 - bdo (Bidirectional Override) §17.3.2.3
 - [done] bookmarkEnd (Bookmark End)   §17.13.6.1
 - [done] bookmarkStart (Bookmark Start)                                         §17.13.6.2
 - commentRangeEnd (Comment Anchor Range End)                                    §17.13.4.3
 - commentRangeStart (Comment Anchor Range Start)                                §17.13.4.4
 - customXml (Inline-Level Custom XML Element)                                   §17.5.1.3
 - customXmlDelRangeEnd (Custom XML Markup Deletion End)                         §17.13.5.4
 - customXmlDelRangeStart (Custom XML Markup Deletion Start)                     §17.13.5.5
 - customXmlInsRangeEnd (Custom XML Markup Insertion End)                        §17.13.5.6
 - customXmlInsRangeStart (Custom XML Markup Insertion Start)                    §17.13.5.7
 - customXmlMoveFromRangeEnd (Custom XML Markup Move Source End)                 §17.13.5.8
 - customXmlMoveFromRangeStart (Custom XML Markup Move Source Start)             §17.13.5.9
 - customXmlMoveToRangeEnd (Custom XML Markup Move Destination Location End)     §17.13.5.10
 - customXmlMoveToRangeStart (Custom XML Markup Move Destination Location Start) §17.13.5.11
 - [done] del (Deleted Run Content)                                              §17.13.5.14
 - dir (Bidirectional Embedding Level)                                           §17.3.2.8
 - [done] fldSimple (Simple Field)                                               §17.16.19
 - [done] hyperlink (Hyperlink)                                                  §17.16.22
 - [done] ins (Inserted Run Content)                                             §17.13.5.18
 - moveFrom (Move Source Run Content)                                            §17.13.5.22
 - moveFromRangeEnd (Move Source Location Container - End)                       §17.13.5.23
 - moveFromRangeStart (Move Source Location Container - Start)                   §17.13.5.24
 - moveTo (Move Destination Run Content)                                         §17.13.5.25
 - moveToRangeEnd (Move Destination Location Container - End)                    §17.13.5.27
 - moveToRangeStart (Move Destination Location Container - Start)                §17.13.5.28
 - [done] oMath (Office Math)                                                    §22.1.2.77
 - [done] oMathPara (Office Math Paragraph)                                      §22.1.2.78
 - permEnd (Range Permission End)                                                §17.13.7.1
 - permStart (Range Permission Start)                                            §17.13.7.2
 - proofErr (Proofing Error Anchor)                                              §17.13.8.1
 - [done] r (Text Run)                                                           §17.3.2.25
 - [done] sdt (Inline-Level Structured Document Tag)                             §17.5.2.31
 - [done] smartTag (Inline-Level Smart Tag)                                      §17.5.1.9
 - subDoc (Anchor for Subdocument Location)                                      §17.17.1.1

 @todo support all elements
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_fldSimple()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR(instr)

    m_specialCharacters = instr;

// @todo support all attributes

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(fldSimple)
            ELSE_TRY_READ_IF(r)
            ELSE_TRY_READ_IF(sdt)
            ELSE_TRY_READ_IF(hyperlink)
            ELSE_TRY_READ_IF(del)
            ELSE_TRY_READ_IF(ins)
            ELSE_TRY_READ_IF(bookmarkStart)
            ELSE_TRY_READ_IF(bookmarkEnd)
            ELSE_TRY_READ_IF_NS(m, oMathPara)
            ELSE_TRY_READ_IF_NS(m, oMath)
            ELSE_TRY_READ_IF(smartTag)
            SKIP_UNKNOWN
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL tabs
//! tabs handler (Set of Custom Tab Stops)
/*! ECMA-376, 17.3.1.38, p.269

 This element specifies a sequence of custom tab stops which shall be
 used for any tab characters in the current paragraph.

 If this element is omitted on a given paragraph, its value is
 determined by the setting previously set at any level of the style
 hierarchy (i.e. that previous setting remains unchanged).  If this
 setting is never specified in the style hierarchy, then no custom tab
 stops shall be used for this paragraph.

 As well, this property is additive - tab stops at each level in the
 style hierarchy are added to each other to determine the full set of
 tab stops for the paragraph.  A hanging indent specified via the
 hanging attribute on the ind element (§17.3.1.12) shall also always
 implicitly create a custom tab stop at its location.

 Parent elements:
 - [done] pPr (§17.3.1.26)
 - [done] pPr (§17.3.1.25)
 - [done] pPr (§17.7.5.2)
 - [done] pPr (§17.7.6.1)
 - [done] pPr (§17.9.23)
 - [done] pPr (§17.7.8.2)

 Child elements:
 - [done] tab (§17.3.137)

*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_tabs()
{
    READ_PROLOGUE

    QBuffer tabs;
    tabs.open(QIODevice::WriteOnly);
    KoXmlWriter elementWriter(&tabs, 4 /*proper indentation*/);
    elementWriter.startElement("style:tab-stops");

    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    KoXmlWriter *oldBody = body;
    body = new KoXmlWriter(&buffer);

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(tab)
            ELSE_WRONG_FORMAT
        }
    }

    elementWriter.addCompleteElement(&buffer);

    delete body;
    body = oldBody;

    elementWriter.endElement(); // style-tab-stops

    QString tabStops = QString::fromUtf8(tabs.buffer(), tabs.buffer().size());
    debugDocx << tabStops;
    m_currentParagraphStyle.addChildElement("style:tab-stops", tabStops);

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL tab
//! tab (Custom Tab Stop)
/*! ECMA-376, 17.3.1.37, p.267

 Specifies a single custom tab stop defined within a set of paragraph
 properties in a document. A tab stop location shall always be
 measured relative to the leading edge of the paragraph in which it
 is used (that is, the left edge for a left-to-right paragraph, and
 the right edge for a right-to-left paragraph).

 Parent elements:
 - [done] tabs (§17.3.1.38)

 Child elements:
 - None

*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_tab()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR(leader)
    TRY_READ_ATTR(pos)
    TRY_READ_ATTR(val)

    // "clear" - Specifies that the current tab stop is cleared and
    // shall be removed and ignored when processing the contents of
    // this document.
    //
    // NOTE: This is a workaround!  The correct approach would be to
    // clear the tab-stop inherited from the parent named style at the
    // specified position.  But this must be done during ODF loading
    // and it's not supported by ODF.  The solution for a viewer would
    // be to not save tab-stop elements to the named style and only
    // save the final set into each "child" style.
    if (val == "clear") {
        readNext();
        READ_EPILOGUE
    }

    body->startElement("style:tab-stop");

    // ST_TabJc (Custom Tab Stop Type) in {bar, center, clear,
    // decimal, end, num, start} - there's (left, right) instead of
    // (start, end) according to test files.
    //
    // ODF: The default value for this attribute is left.
    if (!val.isEmpty()) {
        if (val == "center") {
            body->addAttribute("style:type", "center");
        }
        else if (val == "decimal") {
            body->addAttribute("style:type", "char");
            body->addAttribute("style:char", "." );
        }
        else if (val == "end" || val == "right") {
            body->addAttribute("style:type", "right");
        }
        else if (val == "bar" || val == "num") {
            debugDocx << "Unhandled tab justification code:" << val;
        }
    }

    bool ok = false;
    const qreal value = qreal(TWIP_TO_POINT(pos.toDouble(&ok)));
    if (ok) {
        body->addAttributePt("style:position", value);
    }

    // ST_TabTlc (Custom Tab Stop Leader Character) in {dot, heavy,
    // hyphen, middleDot, none, underscore}
    //
    // ODF: The default value for this attribute is “ “ (U+0020).
    QChar text;
    if (!leader.isEmpty()) {
        if (leader == "dot" || leader == "middleDot") {
            text = QChar('.');
        }
        else if (leader == "hyphen") {
            text = QChar('-');
        }
        else if (leader == "underscore" || leader == "heavy") {
            text = QChar('_');
        }
        else if (leader == "none") {
            text = QChar();
        }
    }
    if (!text.isNull()) {
        body->addAttribute("style:leader-text", text);
    }
    body->endElement(); // style:tab-stop

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL ptab
//! ptab handler (absolute tab)
/*!

 Parent elements:
 - [done] r (§22.1.2.87);
 - [done] r (§17.3.2.25)

 Child elements:
 - none

 @todo support all attributes
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_ptab()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
//! @todo: support all attributes properly

    body->startElement("text:tab");
    body->endElement(); // text:tab

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL pBdr
//! pBdr handler (Paragraph Borders)
/*! ECMA-376, WML, 17.3.1.24, p.256.

 This element specifies the borders for the parent paragraph. Each child element shall specify a specific kind
 of border (left, right, bottom, top, and between).

 Parent elements:
 - [done] pPr (§17.3.1.26)
 - [done] pPr (§17.3.1.25)
 - [done] pPr (§17.7.5.2)
 - [done] pPr (§17.7.6.1)
 - [done] pPr (§17.9.23)
 - [done] pPr (§17.7.8.2)

 Child elements:
 - bar (Paragraph Border Between Facing Pages) §17.3.1.4
 - between (Paragraph Border Between Identical Paragraphs) §17.3.1.5
 - [done] bottom (Paragraph Border Below Identical Paragraphs) §17.3.1.7
 - [done] left (Left Paragraph Border) §17.3.1.17
 - [done] right (Right Paragraph Border) §17.3.1.28
 - [done] top (Paragraph Border Above Identical Paragraphs) §17.3.1.42

 @todo support all elements
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_pBdr()
{
    READ_PROLOGUE
    m_borderStyles.clear();
    m_borderPaddings.clear();
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            if (QUALIFIED_NAME_IS(top)) {
                RETURN_IF_ERROR(readBorderElement(TopBorder, "top", m_borderStyles, m_borderPaddings));
            }
            else if (QUALIFIED_NAME_IS(left)) {
                RETURN_IF_ERROR(readBorderElement(LeftBorder, "left", m_borderStyles, m_borderPaddings));
            }
            else if (QUALIFIED_NAME_IS(bottom)) {
                RETURN_IF_ERROR(readBorderElement(BottomBorder, "bottom", m_borderStyles, m_borderPaddings));
            }
            else if (QUALIFIED_NAME_IS(right)) {
                RETURN_IF_ERROR(readBorderElement(RightBorder, "right", m_borderStyles, m_borderPaddings));
            }
            SKIP_UNKNOWN
//! @todo add ELSE_WRONG_FORMAT
        }
    }
    applyBorders(&m_currentParagraphStyle, m_borderStyles, m_borderPaddings);
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL bdr
//! bbdr handler (Text Border)
/*!

 Parent elements:
 - [done] rPr (§17.3.1.29);
 - [done] rPr (§17.3.1.30);
 - [done] rPr (§17.5.2.28);
 - [done] rPr (§17.9.25);
 - [done] rPr (§17.7.9.1);
 - [done] rPr (§17.7.5.4);
 - [done] rPr (§17.3.2.28);
 - [done] rPr (§17.5.2.27);
 - [done] rPr (§17.7.6.2);
 - [done] rPr (§17.3.2.27)

 Child elements:
 - none (?)

 @todo support all elements
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_bdr()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    m_textBorderStyles.clear();
    m_textBorderPaddings.clear();

    READ_ATTR(val)
    TRY_READ_ATTR(sz)
    TRY_READ_ATTR(color)
    createBorderStyle(sz, color, val, TopBorder, m_textBorderStyles);
    createBorderStyle(sz, color, val, LeftBorder, m_textBorderStyles);
    createBorderStyle(sz, color, val, BottomBorder, m_textBorderStyles);
    createBorderStyle(sz, color, val, RightBorder, m_textBorderStyles);
    TRY_READ_ATTR(space)
    if (!space.isEmpty()) {
        bool ok = false;
        const qreal sp = qreal(TWIP_TO_POINT(space.toDouble(&ok)));
        if (ok) {
            m_textBorderPaddings.insertMulti(TopBorder, sp);
            m_textBorderPaddings.insertMulti(LeftBorder, sp);
            m_textBorderPaddings.insertMulti(RightBorder, sp);
            m_textBorderPaddings.insertMulti(BottomBorder, sp);
        }
    }

    // Note that styles are not applied to anything, odf does not support
    // border around normal text run, but ooxml uses this element also to determine
    // borders for example to pictures

    readNext();
    READ_EPILOGUE
}

void DocxXmlDocumentReader::readStrikeElement(KoCharacterStyle::LineType type)
{
    const QXmlStreamAttributes attrs(attributes());
    if (READ_BOOLEAN_VAL) {
        m_currentTextStyleProperties->setStrikeOutType(type);
        m_currentTextStyleProperties->setStrikeOutStyle(KoCharacterStyle::SolidLine);
//! @todo m_currentTextStyleProperties->strikeOutWidth() ??
//! @todo m_currentTextStyleProperties->setStrikeOutColor() ??
//! @todo m_currentTextStyleProperties->setStrikeOutMode() ??
//! @todo m_currentTextStyleProperties->setStrikeOutText() ??
    }
}

#undef CURRENT_EL
#define CURRENT_EL strike
//! strike handler
//! CASE #1050
KoFilter::ConversionStatus DocxXmlDocumentReader::read_strike()
{
    READ_PROLOGUE
    readStrikeElement(KoCharacterStyle::SingleLine);
    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL dstrike
//! dstrike handler
//! CASE #1051
KoFilter::ConversionStatus DocxXmlDocumentReader::read_dstrike()
{
    READ_PROLOGUE
    readStrikeElement(KoCharacterStyle::DoubleLine);
    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL caps
//! caps handler
/*! Parent elements:
 - [done] rPr (many)
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_caps()
{
    READ_PROLOGUE
    if (READ_BOOLEAN_VAL) {
        m_currentTextStyleProperties->setFontCapitalization(QFont::AllUppercase);
    }
    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL outline
//! outline handler
/*! Parent Elements:
 - [done] rPr (many)
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_outline()
{
    READ_PROLOGUE
    m_currentTextStyleProperties->setTextOutline(QPen(Qt::SolidLine));
    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL smallCaps
//! smallCaps handler
/*! Parent elements:
 - [done] rPr (many)
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_smallCaps()
{
    READ_PROLOGUE
    if (READ_BOOLEAN_VAL) {
        m_currentTextStyleProperties->setFontCapitalization(QFont::SmallCaps);
    }
    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL w
//! w handler (Expanded/Compressed Text)
/*! ECMA-376, 17.3.2.43, p.350
 This element specifies the amount by which each character shall be expanded or when the character
 is rendered in the document.

 Parent elements:
 - [done] rPr (§17.3.1.29)
 - rPr (§17.3.1.30)
 - rPr (§17.5.2.28)
 - rPr (§17.9.25)
 - rPr (§17.7.9.1)
 - rPr (§17.7.5.4)
 - [done] rPr (§17.3.2.28)
 - rPr (§17.5.2.27)
 - rPr (§17.7.6.2)
 - rPr (§17.3.2.27)

 No child elements.
*/
//! @todo support all elements
KoFilter::ConversionStatus DocxXmlDocumentReader::read_w()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    READ_ATTR(val)
    if (!val.isEmpty()) {
        int wNumber;
        STRING_TO_INT(val, wNumber, "w@val")
        m_currentTextStyleProperties->setTextScale(wNumber);
    }
    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL vanish
//! vanish handler (Hidden text)
/*

 Parent elements:
 - [done] rPr (§17.3.1.29);
 - [done] rPr (§17.3.1.30);
 - [done] rPr (§17.5.2.28);
 - [done] rPr (§17.9.25);
 . [done] rPr (§17.7.9.1);
 - [done] rPr (§17.7.5.4);
 - [done] rPr (§17.3.2.28);
 - [done] rPr (§17.5.2.27);
 - [done] rPr (§17.7.6.2);
 - [done] rPr (§17.3.2.27)

 No child elements

*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_vanish()
{
    READ_PROLOGUE

    m_currentTextStyle.addProperty("text:display", "none");

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL color
//! color handler
//! CASE #1158
KoFilter::ConversionStatus DocxXmlDocumentReader::read_color()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    READ_ATTR(val)
//! @todo more styles
    if (val == MsooXmlReader::constAuto) {
        m_currentTextStyle.addProperty("style:use-window-font-color", "true");
    } else {
        QColor color(MSOOXML::Utils::ST_HexColorRGB_to_QColor(val));
        if (color.isValid()) {
            m_currentTextStyleProperties->setForeground(QBrush(color));
        }
    }
    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL highlight
//! highlight handler (Text Highlighting)
/*! ECMA-376, 17.3.2.15, p.310
*/
//! CASE #1158
KoFilter::ConversionStatus DocxXmlDocumentReader::read_highlight()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    READ_ATTR(val)
    m_currentTextStyleProperties->setBackground(MSOOXML::Utils::ST_HighlightColor_to_QColor(val));
    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL vertAlign
//! vertAlign handler (Subscript/Superscript Text) ECMA-376, 17.3.2.42, p.349
/*!    This element specifies the alignment which shall be applied to
       the contents of this run when displayed.
*/
/*!
 Parent elements:
 - [done] rPr
 No child elements.
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_vertAlign()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR(val)
    val = val.toLower();
    if (val == "superscript") {
        m_currentTextStyleProperties->setVerticalAlignment(QTextCharFormat::AlignSuperScript);
    }
    else if (val == "subscript") {
        m_currentTextStyleProperties->setVerticalAlignment(QTextCharFormat::AlignSubScript);
    }
    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL lang
//! w:lang handler (Languages for Run Content)
/*! ECMA-376, 17.3.2.20, p.314.
 Parent elements:
 - [done] rPr (§17.3.1.29) (within pPr §17.3.1.26)
 - rPr (§17.3.1.30) (within rPrChange §17.13.5.30)
 - rPr (§17.3.2.27) (within del §17.13.5.13, ins §17.13.5.16, rPrChange §17.13.5.31)
 - [done] rPr (§17.3.2.28) (witin ctrlPr §22.1.2.23, r §22.1.2.87, r §17.3.2.25)
 - rPr (§17.5.2.27) (within sdtPr §17.5.2.38)
 - rPr (§17.5.2.28) (within sdtEndPr §17.5.2.37)
 - [done] rPr (§17.7.9.1) (within style §17.7.4.17 - styles)
 - [done] rPr (§17.7.5.4) (within rPrDefault §17.7.5.5 - styles)
 - rPr (§17.7.6.2) (within tblStylePr §17.7.6.6 - styles)
 - rPr (§17.9.25) (within lvl §17.9.6, lvl §17.9.7 - numbering)
 No child elements.

 CASE #850 -> CASE #858 -> CASE #861 -> CASE #1100

 @todo support all elements
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_lang()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
// CASE #1100
    TRY_READ_ATTR(bidi)
    QString language, country;
    if (!bidi.isEmpty()) {
        if (MSOOXML::Utils::ST_Lang_to_languageAndCountry(bidi, language, country)) {
            m_currentTextStyle.addProperty("style:language-complex", language, KoGenStyle::TextType);
            m_currentTextStyle.addProperty("style:country-complex", country, KoGenStyle::TextType);
        } else {
            warnDocx << "invalid value of \"bidi\" attribute:" << bidi << " - skipping";
        }
    }
    TRY_READ_ATTR(val)
    if (MSOOXML::Utils::ST_Lang_to_languageAndCountry(val, language, country)) {
        m_currentTextStyle.addProperty("fo:language", language, KoGenStyle::TextType);
        m_currentTextStyle.addProperty("fo:country", country, KoGenStyle::TextType);
    } else {
        warnDocx << "invalid value of \"val\" attribute:" << val << " - skipping";
    }

    TRY_READ_ATTR(eastAsia)
    if (!eastAsia.isEmpty()) {
        if (MSOOXML::Utils::ST_Lang_to_languageAndCountry(eastAsia, language, country)) {
            m_currentTextStyle.addProperty("style:language-asian", language, KoGenStyle::TextType);
            m_currentTextStyle.addProperty("style:country-asian", country, KoGenStyle::TextType);
        } else {
            warnDocx << "invalid value of \"eastAsia\" attribute:" << eastAsia << " - skipping";
        }
    }
    debugDocx << "bidi:" << bidi << "val:" << val << "eastAsia:" << eastAsia;

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL background

//! w:background handler (Document Background)
/*! ECMA-376, 17.2.1, p. 189.
 This element specifies the background for every page of the document
 containing the background element.

 Child element:
 - [done] drawing (§17.3.3.9)

 Attributes:
 - [done] color (Background Color)
 - themeColor (Background Theme Color)
 - themeTint (Border Theme Color Tint)
 - themeShade (Border Theme Color Shade)
*/

KoFilter::ConversionStatus DocxXmlDocumentReader::read_background()
{
    READ_PROLOGUE

    // The specs say that the background will not be shown in the default view but only in the fullscreen-view except the
    // displayBackgroundShape is defined. Since we cannot (and don't want to) make such assumptions about the available
    // views we only take over whatever is displayed in that default view. That's inline with what OO.org does too.
    bool displayBackgroundShape = m_context->import->documentSettings().contains("displayBackgroundShape");
    if (displayBackgroundShape) {
        QString val = m_context->import->documentSetting("displayBackgroundShape").toString();
        displayBackgroundShape = (val != "off" && val != "0" && val != "false");
    }

    if (displayBackgroundShape) {
        const QXmlStreamAttributes attrs(attributes());
        TRY_READ_ATTR(color)

        QColor tmpColor(MSOOXML::Utils::ST_HexColorRGB_to_QColor(color));
        if (tmpColor.isValid())
            m_backgroundColor = tmpColor;

        while (!atEnd()) {
            readNext();
            BREAK_IF_END_OF(CURRENT_EL)
            if (isStartElement()) {
                if (qualifiedName() == "v:background") {
                    TRY_READ(VML_background)
                }
                ELSE_TRY_READ_IF(drawing)
                SKIP_UNKNOWN
            }
        }
    } else {
        while (!atEnd()) {
            readNext();
            BREAK_IF_END_OF(CURRENT_EL)
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL tbl
//! tbl handler (Table)
/*! ECMA-376, 17.4.38, p. 460.
 This element specifies the contents of a table present in the document. A table is a set of paragraphs
 (and other block-level content) arranged in rows and columns.

 Parent elements:
 - [done] body (§17.2.2)
 - comment (§17.13.4.2)
 - customXml (§17.5.1.6)
 - docPartBody (§17.12.6)
 - [done] endnote (§17.11.2);
 - [done] footnote (§17.11.10)
 - [done] ftr (§17.10.3)
 - [done] hdr (§17.10.4)
 - sdtContent (§17.5.2.34)
 - [done] tc (§17.4.66)

 Child elements:
 - [done] bookmarkEnd (Bookmark End) §17.13.6.1
 - [done] bookmarkStart (Bookmark Start) §17.13.6.2
 - commentRangeEnd (Comment Anchor Range End) §17.13.4.3
 - commentRangeStart (Comment Anchor Range Start) §17.13.4.4
 - customXml (Row-Level Custom XML Element) §17.5.1.5
 - customXmlDelRangeEnd (Custom XML Markup Deletion End) §17.13.5.4
 - customXmlDelRangeStart (Custom XML Markup Deletion Start) §17.13.5.5
 - customXmlInsRangeEnd (Custom XML Markup Insertion End) §17.13.5.6
 - customXmlInsRangeStart (Custom XML Markup Insertion Start) §17.13.5.7
 - customXmlMoveFromRangeEnd (Custom XML Markup Move Source End) §17.13.5.8
 - customXmlMoveFromRangeStart (Custom XML Markup Move Source Start) §17.13.5.9
 - customXmlMoveToRangeEnd (Custom XML Markup Move Destination Location End) §17.13.5.10
 - customXmlMoveToRangeStart (Custom XML Markup Move Destination Location Start) §17.13.5.11
 - del (Deleted Run Content) §17.13.5.14
 - ins (Inserted Run Content) §17.13.5.18
 - moveFrom (Move Source Run Content) §17.13.5.22
 - moveFromRangeEnd (Move Source Location Container - End) §17.13.5.23
 - moveFromRangeStart (Move Source Location Container - Start) §17.13.5.24
 - moveTo (Move Destination Run Content) §17.13.5.25
 - moveToRangeEnd (Move Destination Location Container - End) §17.13.5.27
 - moveToRangeStart (Move Destination Location Container - Start) §17.13.5.28
 - oMath (Office Math) §22.1.2.77
 - oMathPara (Office Math Paragraph) §22.1.2.78
 - permEnd (Range Permission End) §17.13.7.1
 - permStart (Range Permission Start) §17.13.7.2
 - proofErr (Proofing Error Anchor) §17.13.8.1
 - sdt (Row-Level Structured Document Tag) §17.5.2.30
 - [done] tblGrid (Table Grid) §17.4.49
 - [done] tblPr (Table Properties) §17.4.60
 - [done] tr (Table Row) §17.4.79
*/
//! @todo support all child elements
KoFilter::ConversionStatus DocxXmlDocumentReader::read_tbl()
{
    m_table = KoTable::create();

    READ_PROLOGUE

    // save current state of lists processing
    saveState();

    m_table->setName(QLatin1String("Table") + QString::number(++m_currentTableNumber));
    m_currentTableRowNumber = 0;
    m_currentTableColumnNumber = 0;

    m_currentDefaultCellStyle = 0;
    m_currentTableStyleProperties = 0;
    m_currentLocalTableStyles = new MSOOXML::LocalTableStyles;

    m_currentTableStyleName.clear();
    m_currentDrawStyleName.clear();

    m_tableMainStyle = KoTblStyle::create();
    if (m_moveToStylesXml) {
        m_tableMainStyle->setAutoStyleInStylesDotXml(true);
    }
    //Disable to get a style of type TableAutoStyle.
//     m_tableMainStyle->setName("TableMainStyle" + QString::number(m_currentTableNumber - 1));

    bool sectionAdded = false;
    if (m_createSectionStyle) {
        m_createSectionStyle = false;
        sectionAdded = true;

        // To avoid the style to become a duplicate or being duplicated.
        m_tableMainStyle->setMasterPageName("placeholder");
    }
    // Fix me, for tables inside tables this might not work
    m_activeRoles = 0;

    MSOOXML::Utils::XmlWriteBuffer buffer;
    body = buffer.setWriter(body);

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            if(QUALIFIED_NAME_IS(tblPr)) {

                //tblPr/tblBorders
                m_currentTableStyleProperties = new MSOOXML::TableStyleProperties;
                TRY_READ(tblPr)
                m_currentDefaultCellStyle = m_currentTableStyleProperties;
                m_currentTableStyleProperties = 0;

                //debug border information
#ifdef DOCXXML_DEBUG_TABLES
		debugDocx << "==> [tblBorders] information: <====================";
		debugDocx << "top:" << m_currentDefaultCellStyle->top.style;
		debugDocx << "bottom:" << m_currentDefaultCellStyle->bottom.style;
		debugDocx << "left:" << m_currentDefaultCellStyle->left.style;
		debugDocx << "right:" << m_currentDefaultCellStyle->right.style;
		debugDocx << "insideH:" << m_currentDefaultCellStyle->insideH.style;
		debugDocx << "insideV:" << m_currentDefaultCellStyle->insideV.style;
#endif
            }
            ELSE_TRY_READ_IF(tblGrid)
            ELSE_TRY_READ_IF(tr)
            SKIP_UNKNOWN
//             ELSE_TRY_READ_IF(bookmarkStart)
//             ELSE_TRY_READ_IF(bookmarkEnd)
//             ELSE_WRONG_FORMAT
        }
    }

    //reference to the default parent style from styles.xml
    if (m_currentTableStyleName.isEmpty() &&
        m_context->m_namedDefaultStyles.contains("table"))
    {
        m_currentTableStyleName = m_context->m_namedDefaultStyles.value("table");
        MSOOXML::DrawingTableStyle* tableStyle = m_context->m_tableStyles.value(m_currentTableStyleName);
        Q_ASSERT(tableStyle);
        if (tableStyle) {
            m_tableMainStyle->setHorizontalAlign(tableStyle->mainStyle->horizontalAlign());
        }
    }

    //Floating Table
    if (!m_currentDrawStyleName.isEmpty()) {
        body->startElement("draw:frame");
        body->addAttribute("draw:style-name", m_currentDrawStyleName.toUtf8());
        body->addAttribute("text:anchor-type", "paragraph");
        body->addAttributePt("svg:width", TWIP_TO_POINT(m_svgWidth));
        if (m_svgX != 0) {
            body->addAttributePt("svg:x", TWIP_TO_POINT(m_svgX));
        }
        if (m_svgY != 0) {
            body->addAttributePt("svg:y", TWIP_TO_POINT(m_svgY));
        }
        body->startElement("draw:text-box");
    }

    m_table->setTableStyle(m_tableMainStyle);

    defineTableStyles();

#ifdef DOCXXML_DEBUG_TABLES
    debugDocx << "----------------------------------------";
#endif

    m_table->saveOdf(*body, *mainStyles);

    if (sectionAdded) {
        m_currentSectionStyleName = m_table->tableStyle()->name();
        m_currentSectionStyleFamily = "table";
    }

    delete m_currentLocalTableStyles;

    //Floating Table
    if (!m_currentDrawStyleName.isEmpty()) {
        body->endElement(); //draw:text-box
        body->endElement(); //draw:frame
        body = buffer.releaseWriter(m_floatingTable);
    } else {
        body = buffer.releaseWriter();
    }

    // restore previous state of lists processing
    restoreState();

    READ_EPILOGUE
}

void DocxXmlDocumentReader::defineTableStyles()
{
    const int rowCount = m_table->rowCount();
    const int columnCount = m_table->columnCount();

    MSOOXML::DrawingTableStyleConverterProperties converterProperties;
    converterProperties.setRowCount(rowCount);
    converterProperties.setColumnCount(columnCount);
    converterProperties.setRoles(m_activeRoles);
    converterProperties.setLocalStyles(*m_currentLocalTableStyles);
    converterProperties.setLocalDefaulCelltStyle(m_currentDefaultCellStyle);
    MSOOXML::DrawingTableStyle* tableStyle = m_context->m_tableStyles.value(m_currentTableStyleName);
    MSOOXML::DrawingTableStyleConverter styleConverter(converterProperties, tableStyle);
    QPair<int, int> spans;

    for(int row = 0; row < rowCount; ++row ) {
#ifdef DOCXXML_DEBUG_TABLES
        debugDocx << "----- [ROW" << row +1 << "] ----------";
#endif
        for(int column = 0; column < columnCount; ++column ) {
#ifdef DOCXXML_DEBUG_TABLES
            debugDocx << "----- [COLUMN" << column +1 << "] ----------";
#endif
            spans.first = m_table->cellAt(row, column)->rowSpan();
            spans.second = m_table->cellAt(row, column)->columnSpan();
            KoCellStyle::Ptr style = styleConverter.style(row, column, spans);
            if (m_moveToStylesXml) {
                style->setAutoStyleInStylesDotXml(true);
            }
            m_table->cellAt(row, column)->setStyle(style);
#ifdef DOCXXML_DEBUG_TABLES
            style = m_table->cellAt(row, column)->style();
            debugDocx << "[check] top:" << style->borders()->topBorderData().style;
            debugDocx << "[check] bottom:" << style->borders()->bottomBorderData().style;
            debugDocx << "[check] left:" << style->borders()->leftBorderData().style;
            debugDocx << "[check] right:" << style->borders()->rightBorderData().style;
#endif
        }
    }
}

#undef CURRENT_EL
#define CURRENT_EL tblPr
//! tblPr handler (Table Properties)
/*! ECMA-376, 17.4.60, p. 492.

 This element specifies the set of table-wide properties applied to the current table.
 These properties affect the appearance of all rows and cells within the parent table,
 but can be overridden by individual table-level exception, row, and cell level properties
 as defined by each property.

 Parent elements:
 - [done] tbl (§17.4.38)

 Child elements:
 - bidiVisual (Visually Right to Left Table) §17.4.1
 - [done] jc (Table Alignment) §17.4.29
 - shd (Table Shading) §17.4.32
 - [done] tblBorders (Table Borders) §17.4.39
 - tblCaption (Table Caption) §17.4.41
 - [done] tblCellMar (Table Cell Margin Defaults) §17.4.43
 - tblCellSpacing (Table Cell Spacing Default) §17.4.46
 - tblDescription (Table Description) §17.4.47
 - tblInd (Table Indent from Leading Margin) §17.4.51
 - tblLayout (Table Layout) §17.4.53
 - tblLook (Table Style Conditional Formatting Settings) §17.4.56
 - tblOverlap (Floating Table Allows Other Tables to Overlap) §17.4.57
 - [done] tblpPr (Floating Table Positioning) §17.4.58
 - tblPrChange (Revision Information for Table Properties) §17.13.5.34
 - [done] tblStyle (Referenced Table Style) §17.4.63
 - tblStyleColBandSize (Number of Columns in Column Band) §17.7.6.5
 - tblStyleRowBandSize (Number of Rows in Row Band) §17.7.6.7
 - tblW (Preferred Table Width) §17.4.64
*/
//! @todo support all child elements
KoFilter::ConversionStatus DocxXmlDocumentReader::read_tblPr()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(tblStyle)
            ELSE_TRY_READ_IF(tblpPr)
            ELSE_TRY_READ_IF(tblBorders)
            ELSE_TRY_READ_IF(tblCellMar)
            ELSE_TRY_READ_IF_IN_CONTEXT(jc)
            SKIP_UNKNOWN
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL tblPrEx
//! tblPrEx (Table-Level Property Exceptions)
/*! ECMA-376, 17.4.61, p. 484.

  Parent elements:
  - [done] tr (§17.4.79)

  Child elements
  - jc (Table Alignment Exception) §17.4.27
  - shd (Table Shading Exception) §17.4.31
  - [done] tblBorders (Table Borders Exceptions) §17.4.40
  - tblCellMar (Table Cell Margin Exceptions) §17.4.42
  - tblCellSpacing (Table Cell Spacing Exception) §17.4.45
  - tblInd (Table Indent from Leading Margin Exception) §17.4.52
  - tblLayout (Table Layout Exception) §17.4.54
  - tblLook (Table Style Conditional Formatting Settings Exception) §17.4.55
  - tblPrExChange (Revision Information for Table-Level Property Exceptions) §17.13.5.35
  - tblW (Preferred Table Width Exception) §17.4.65
 */
KoFilter::ConversionStatus DocxXmlDocumentReader::read_tblPrEx()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            if (QUALIFIED_NAME_IS(tblBorders)) {
                m_currentTableStyleProperties = new MSOOXML::TableStyleProperties;
                TRY_READ_IF(tblBorders)
                m_currentTableStyleProperties->target = MSOOXML::TableStyleProperties::TableRow;
                m_currentLocalTableStyles->setLocalStyle(m_currentTableStyleProperties,
                    m_currentTableRowNumber, -1);
#ifdef DOCXXML_DEBUG_TABLES
		debugDocx << "====> [Row" << m_currentTableRowNumber << " Column (-1)" <<
                            "] Border information:";
		debugDocx << "top:" << m_currentTableStyleProperties->top.style;
		debugDocx << "bottom:" << m_currentTableStyleProperties->bottom.style;
		debugDocx << "left:" << m_currentTableStyleProperties->left.style;
		debugDocx << "right:" << m_currentTableStyleProperties->right.style;
#endif
                m_currentTableStyleProperties = 0;

            }
            SKIP_UNKNOWN
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL tblGrid
//! tblGrid handler (Table Grid)
/*! ECMA-376, 17.4.49, p. 474.
 This element specifies the table grid for the current table. The table grid is a definition
 of the set of grid columns which define all of the shared vertical edges of the table,
 as well as default widths for each of these grid columns. These grid column widths are then
 used to determine the size of the table based on the table layout algorithm used (§17.4.53;§17.4.54).

 Parent elements:
 - [done] tbl (§17.4.38)

 Child elements:
 - [done] gridCol (Grid Column Definition) §17.4.16
 - tblGridChange (Revision Information for Table Grid Column Definitions) §17.13.5.33
*/
//! @todo support all child elements
KoFilter::ConversionStatus DocxXmlDocumentReader::read_tblGrid()
{
    READ_PROLOGUE

    m_currentTableColumnNumber = 0;
    m_svgWidth = 0;

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(gridCol)
            SKIP_UNKNOWN
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL gridCol
//! gridCol handler (Grid Column Definition)
/*! ECMA-376, 17.4.16, p. 433.
 This element specifies the presence and details about a single grid column within a table grid.
 A grid column is a logical column in a table used to specify the presence of a shared vertical
 edge in the table. When table cells are then added to this table, these shared edges
 (or grid columns, looking at the column between those shared edges) determine how table cells
 are placed into the table grid.

 Parent elements:
 - tblGrid (§17.4.48)
 - [done] tblGrid (§17.4.49)

 No child elements.
*/
//! @todo support all child elements
KoFilter::ConversionStatus DocxXmlDocumentReader::read_gridCol()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR(w)

    int width = 0;
    STRING_TO_INT(w, width, QString("w:w"));
    m_svgWidth += width;

    const qreal columnWidth = width / 20.0;
    KoColumn* column = m_table->columnAt(m_currentTableColumnNumber++);
    KoColumnStyle::Ptr style = KoColumnStyle::create();
    if (m_moveToStylesXml) {
        style->setAutoStyleInStylesDotXml(true);
    }
    style->setWidth(columnWidth);
    column->setStyle(style);

    readNext();

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL tr
//! tr handler (Table Row)
/*! ECMA-376, 17.4.79, p. 492.
 This element specifies a single table row, which contains the table’s cells.
 Table rows in WordprocessingML are analogous to HTML tr elements.

 Parent elements:
 - customXml (§17.5.1.5)
 - sdtContent (§17.5.2.35)
 - [done] tbl (§17.4.38)

 Child elements:
 - [done] bookmarkEnd (Bookmark End) §17.13.6.1
 - [done] bookmarkStart (Bookmark Start) §17.13.6.2
 - commentRangeEnd (Comment Anchor Range End) §17.13.4.3
 - commentRangeStart (Comment Anchor Range Start) §17.13.4.4
 - customXml (Cell-Level Custom XML Element) §17.5.1.4
 - customXmlDelRangeEnd (Custom XML Markup Deletion End) §17.13.5.4
 - customXmlDelRangeStart (Custom XML Markup Deletion Start) §17.13.5.5
 - customXmlInsRangeEnd (Custom XML Markup Insertion End) §17.13.5.6
 - customXmlInsRangeStart (Custom XML Markup Insertion Start) §17.13.5.7
 - customXmlMoveFromRangeEnd (Custom XML Markup Move Source End) §17.13.5.8
 - customXmlMoveFromRangeStart (Custom XML Markup Move Source Start) §17.13.5.9
 - customXmlMoveToRangeEnd (Custom XML Markup Move Destination Location End) §17.13.5.10
 - customXmlMoveToRangeStart (Custom XML Markup Move Destination Location Start) §17.13.5.11
 - del (Deleted Run Content) §17.13.5.14
 - ins (Inserted Run Content) §17.13.5.18
 - moveFrom (Move Source Run Content) §17.13.5.22
 - moveFromRangeEnd (Move Source Location Container - End) §17.13.5.23
 - moveFromRangeStart (Move Source Location Container - Start) §17.13.5.24
 - moveTo (Move Destination Run Content) §17.13.5.25
 - moveToRangeEnd (Move Destination Location Container - End) §17.13.5.27
 - moveToRangeStart (Move Destination Location Container - Start) §17.13.5.28
 - oMath (Office Math) §22.1.2.77
 - oMathPara (Office Math Paragraph) §22.1.2.78
 - permEnd (Range Permission End) §17.13.7.1
 - permStart (Range Permission Start) §17.13.7.2
 - proofErr (Proofing Error Anchor) §17.13.8.1
 - [done] sdt (Cell-Level Structured Document Tag) §17.5.2.32
 - [done] tblPrEx (Table-Level Property Exceptions) §17.4.61
 - tc (Table Cell) §17.4.66
 - [done]trPr (Table Row Properties) §17.4.82
*/
//! @todo support all child elements
KoFilter::ConversionStatus DocxXmlDocumentReader::read_tr()
{
    READ_PROLOGUE

    m_currentTableColumnNumber = 0;

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(tc)
            ELSE_TRY_READ_IF(trPr)
            ELSE_TRY_READ_IF(bookmarkStart)
            ELSE_TRY_READ_IF(bookmarkEnd)
            ELSE_TRY_READ_IF(sdt)
            ELSE_TRY_READ_IF(tblPrEx)
            SKIP_UNKNOWN
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    m_currentTableRowNumber++;

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL trPr
/*
Parent elements:
 - [done] tr (§17.4.79)

child elements:
 - cantSplit (Table Row Cannot Break Across Pages) §17.4.6
 - [done] cnfStyle (Table Row Conditional Formatting) §17.4.7
 - del (Deleted Table Row)§17.13.5.12
 - divId (Associated HTML div ID)§17.4.9
 - gridAfter (Grid Columns After Last Cell)§17.4.14
 - gridBefore (Grid Columns Before First Cell) §17.4.15
 - hidden (Hidden Table Row Marker)§17.4.20
 - ins (Inserted Table Row) §17.13.5.17
 - jc (Table Row Alignment) §17.4.28
 - tblCellSpacing (Table Row Cell Spacing) §17.4.44
 - tblHeader (Repeat Table Row on Every New Page) §17.4.50
 - [done] trHeight (Table Row Height) §17.4.81
 - trPrChange (Revision Information for Table Row Properties) §17.13.5.37
 - wAfter (Preferred Width After Table Row) §17.4.86
 - wBefore (Preferred Width Before Table Row) §17.4.87
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_trPr()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(trHeight)
            ELSE_TRY_READ_IF(cnfStyle)
            SKIP_UNKNOWN
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL cnfStyle
/*
Parent elements:
 - many, eg. tr, tc, ppr

child elements:
 - none
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_cnfStyle()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    // Note this parameter does not follow ooxml spec at all at least in office2007 but
    // produces values from office 2003 ie. val with binary
    // FirstRow, LastRow, FirstColumn, LastColumn, Band1Vertical, Band2Vertical,
    // Band1Horizontal, Band2Horizontal, NE Cell, NW Cell, SE Cell, SW Cell.

    TRY_READ_ATTR(val)

    if (val.length() == 12) {
        if (val.at(0) == '1') {
            m_activeRoles |= MSOOXML::DrawingTableStyleConverterProperties::FirstRow;
        }
        if (val.at(1) == '1') {
            m_activeRoles |= MSOOXML::DrawingTableStyleConverterProperties::LastRow;
        }
        if (val.at(2) == '1') {
            m_activeRoles |= MSOOXML::DrawingTableStyleConverterProperties::FirstCol;
        }
        if (val.at(3) == '1') {
            m_activeRoles |= MSOOXML::DrawingTableStyleConverterProperties::LastCol;
        }
        if (val.at(4) == '1') {
            m_activeRoles |= MSOOXML::DrawingTableStyleConverterProperties::ColumnBanded;
        }
        if (val.at(5) == '1') {
            m_activeRoles |= MSOOXML::DrawingTableStyleConverterProperties::ColumnBanded;
        }
        if (val.at(6) == '1') {
            m_activeRoles |= MSOOXML::DrawingTableStyleConverterProperties::RowBanded;
        }
        if (val.at(7) == '1') {
            m_activeRoles |= MSOOXML::DrawingTableStyleConverterProperties::RowBanded;
        }
        if (val.at(8) == '1') {
            m_activeRoles |= MSOOXML::DrawingTableStyleConverterProperties::NeCell;
        }
        if (val.at(9) == '1') {
            m_activeRoles |= MSOOXML::DrawingTableStyleConverterProperties::NwCell;
        }
        if (val.at(10) == '1') {
            m_activeRoles |= MSOOXML::DrawingTableStyleConverterProperties::SeCell;
        }
        if (val.at(11) == '1') {
            m_activeRoles |= MSOOXML::DrawingTableStyleConverterProperties::SwCell;
        }
    }

    readNext();
    READ_EPILOGUE
}

/*
Parent elements:
 - [done]trPr (§17.4.82)
 - trPr (§17.7.6.10)
 - trPr (§17.7.6.11)
 - trPr (§17.4.83)
*/
#undef CURRENT_EL
#define CURRENT_EL trHeight
KoFilter::ConversionStatus DocxXmlDocumentReader::read_trHeight()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR(val)
    TRY_READ_ATTR(hRule)

    KoRow* row = m_table->rowAt(m_currentTableRowNumber);
    KoRowStyle::Ptr style = KoRowStyle::create();
    if (m_moveToStylesXml) {
        style->setAutoStyleInStylesDotXml(true);
    }

    style->setHeight(TWIP_TO_POINT(val.toFloat()));

    if (hRule == QLatin1String("exact")) {
        style->setHeightType(KoRowStyle::ExactHeight);
    }
    else if (hRule == QLatin1String("atLeast")) {
        style->setHeightType(KoRowStyle::MinimumHeight);
    }
    else {
        style->setHeightType(KoRowStyle::MinimumHeight);
    }

    row->setStyle(style);

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL tc
//! tc handler (Table Cell)
/*! ECMA-376, 17.4.66, p. 502.
 This element specifies a single cell in a table row, which contains the table’s content.
 Table cells in WordprocessingML are analogous to HTML td elements.

 Parent elements:
 - customXml (§17.5.1.4)
 - sdtContent (§17.5.2.33)
 - [done] tr (§17.4.79)

 Child elements:
 - altChunk (Anchor for Imported External Content) §17.17.2.1
 - [done] bookmarkEnd (Bookmark End) §17.13.6.1
 - [done] bookmarkStart (Bookmark Start) §17.13.6.2
 - commentRangeEnd (Comment Anchor Range End) §17.13.4.3
 - commentRangeStart (Comment Anchor Range Start) §17.13.4.4
 - customXml (Block-Level Custom XML Element) §17.5.1.6
 - customXmlDelRangeEnd (Custom XML Markup Deletion End) §17.13.5.4
 - customXmlDelRangeStart (Custom XML Markup Deletion Start) §17.13.5.5
 - customXmlInsRangeEnd (Custom XML Markup Insertion End) §17.13.5.6
 - customXmlInsRangeStart (Custom XML Markup Insertion Start) §17.13.5.7
 - customXmlMoveFromRangeEnd (Custom XML Markup Move Source End) §17.13.5.8
 - customXmlMoveFromRangeStart (Custom XML Markup Move Source Start) §17.13.5.9
 - customXmlMoveToRangeEnd (Custom XML Markup Move Destination Location End) §17.13.5.10
 - customXmlMoveToRangeStart (Custom XML Markup Move Destination Location Start) §17.13.5.11
 - del (Deleted Run Content) §17.13.5.14
 - ins (Inserted Run Content) §17.13.5.18
 - moveFrom (Move Source Run Content) §17.13.5.22
 - moveFromRangeEnd (Move Source Location Container - End) §17.13.5.23
 - moveFromRangeStart (Move Source Location Container - Start) §17.13.5.24
 - moveTo (Move Destination Run Content) §17.13.5.25
 - moveToRangeEnd (Move Destination Location Container - End) §17.13.5.27
 - moveToRangeStart (Move Destination Location Container - Start) §17.13.5.28
 - oMath (Office Math) §22.1.2.77
 - oMathPara (Office Math Paragraph) §22.1.2.78
 - [done] p (Paragraph) §17.3.1.22
 - permEnd (Range Permission End) §17.13.7.1
 - permStart (Range Permission Start) §17.13.7.2
 - proofErr (Proofing Error Anchor) §17.13.8.1
 - sdt (Block-Level Structured Document Tag) §17.5.2.29
 - [done] tbl (Table) §17.4.38
 - [done] tcPr (Table Cell Properties) §17.4.70
*/
//! @todo support all child elements
KoFilter::ConversionStatus DocxXmlDocumentReader::read_tc()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            if(qualifiedName() == "w:p") {
                KoCell* cell = m_table->cellAt(m_currentTableRowNumber, m_currentTableColumnNumber);

                QBuffer buffer;
                KoXmlWriter* oldBody = body;
                KoXmlWriter newBody(&buffer, oldBody->indentLevel()+1);
                body = &newBody;
                TRY_READ(p)

                KoRawCellChild* textChild = new KoRawCellChild(buffer.data());
                cell->appendChild(textChild);

                body = oldBody;
            }
            else if(QUALIFIED_NAME_IS(tbl)) {
                KoTable::Ptr currentTable = m_table;
                int currentRow =  m_currentTableRowNumber;
                int currentColumn = m_currentTableColumnNumber;
                MSOOXML::TableStyleProperties* currentDefaultCellStyle = m_currentDefaultCellStyle;
                MSOOXML::TableStyleProperties* currentStyleProperties = m_currentTableStyleProperties;
                MSOOXML::LocalTableStyles* currentLocalStyles = m_currentLocalTableStyles;
                QString currentTableStyle = m_currentTableStyleName;
                KoCell* cell = m_table->cellAt(m_currentTableRowNumber, m_currentTableColumnNumber);

                QBuffer buffer;
                KoXmlWriter* oldBody = body;
                KoXmlWriter newBody(&buffer, oldBody->indentLevel()+1);
                body = &newBody;

                TRY_READ(tbl)

                KoRawCellChild* textChild = new KoRawCellChild(buffer.data());
                cell->appendChild(textChild);

                body = oldBody;

                m_table = currentTable;
                m_currentTableRowNumber = currentRow;
                m_currentTableColumnNumber = currentColumn;
                m_currentDefaultCellStyle = currentDefaultCellStyle;
                m_currentTableStyleProperties = currentStyleProperties;
                m_currentLocalTableStyles = currentLocalStyles;
                m_currentTableStyleName = currentTableStyle;
            }
            else if(QUALIFIED_NAME_IS(tcPr)) {
                m_currentTableStyleProperties = new MSOOXML::TableStyleProperties;
                TRY_READ(tcPr)
                m_currentTableStyleProperties->target = MSOOXML::TableStyleProperties::TableCell;
                m_currentLocalTableStyles->setLocalStyle(m_currentTableStyleProperties,
                    m_currentTableRowNumber, m_currentTableColumnNumber);

#ifdef DOCXXML_DEBUG_TABLES
		debugDocx << "====> [Row" << m_currentTableRowNumber << " Column" <<
                            m_currentTableColumnNumber << "] Border information:";
		debugDocx << "top:" << m_currentTableStyleProperties->top.style;
		debugDocx << "bottom:" << m_currentTableStyleProperties->bottom.style;
		debugDocx << "left:" << m_currentTableStyleProperties->left.style;
		debugDocx << "right:" << m_currentTableStyleProperties->right.style;
#endif
                m_currentTableStyleProperties = 0;
            }
//             ELSE_TRY_READ_IF(bookmarkStart)
//             ELSE_TRY_READ_IF(bookmarkEnd)
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    // Setting cell covers in case of cell span in horizontal
    KoCell* const cell = m_table->cellAt(m_currentTableRowNumber, m_currentTableColumnNumber);
    int columnSpan = cell->columnSpan();
    if (columnSpan > 1) {
        int columnIndex = 1;
        while (columnIndex < columnSpan) {
            ++m_currentTableColumnNumber;
            KoCell* coveredCell = m_table->cellAt(m_currentTableRowNumber, m_currentTableColumnNumber);
            coveredCell->setCovered(true);
            ++columnIndex;
        }
    }

    m_currentTableColumnNumber++;

    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL tcPr
//! tcPr handler  (Table Cell Properties)
/*! ECMA-376, 17.4.70, p. 510.
 This element specifies the set of properties which shall be applied a specific table cell.
 Each unique property is specified by a child element of this element. In any instance where there
 is a conflict between the table level, table-level exception, or row level properties with
 a corresponding table cell property, these properties shall overwrite the table or row wide properties.

 Parent elements:
 - [done] tc (§17.4.66)

 Child elements:
 - cellDel (Table Cell Deletion) §17.13.5.1
 - cellIns (Table Cell Insertion) §17.13.5.2
 - cellMerge (Vertically Merged/Split Table Cells) §17.13.5.3
 - [done] cnfStyle (Table Cell Conditional Formatting) §17.4.8
 - [done] gridSpan (Grid Columns Spanned by Current Table Cell) §17.4.17
 - headers (Header Cells Associated With Table Cell) §17.4.19
 - hideMark (Ignore End Of Cell Marker In Row Height Calculation) §17.4.21
 - hMerge (Horizontally Merged Cell) §17.4.22
 - noWrap (Don't Wrap Cell Content) §17.4.30
 - [done] shd (Table Cell Shading) §17.4.33
 - [done] tcBorders (Table Cell Borders) §17.4.67
 - tcFitText (Fit Text Within Cell) §17.4.68
 - [done] tcMar (Single Table Cell Margins) §17.4.69
 - tcPrChange (Revision Information for Table Cell Properties) §17.13.5.36
 - tcW (Preferred Table Cell Width) §17.4.72
 - [done] textDirection (Table Cell Text Flow Direction) §17.4.73
 - [done] vAlign (Table Cell Vertical Alignment) §17.4.84
 - [done] vMerge (Vertically Merged Cell) §17.4.85
*/
//! @todo support all child elements
KoFilter::ConversionStatus DocxXmlDocumentReader::read_tcPr()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(gridSpan)
            ELSE_TRY_READ_IF(cnfStyle)
            ELSE_TRY_READ_IF_IN_CONTEXT(shd)
            ELSE_TRY_READ_IF(tcBorders)
            ELSE_TRY_READ_IF(tcMar)
            ELSE_TRY_READ_IF(vMerge)
            ELSE_TRY_READ_IF(vAlign)
            else if (name() == "textDirection") {
                TRY_READ(textDirectionTc)
            }
            SKIP_UNKNOWN
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL vAlign
//! vAlign Handler (Table Cell Vertical Alignment)
/*! ECMA-376, 17.4.84, p. 519.

 Parent elements:
 - [done] tcPr (§17.7.6.8);
 - [done] tcPr (§17.7.6.9);
 - [done] tcPr (§17.4.70);
 - [done] tcPr (§17.4.71)

 Child elements:
 - none
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_vAlign()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR(val)
    if (!val.isEmpty()) {
        if (val == "both" || val == "center") {
            m_currentTableStyleProperties->verticalAlign = "middle";
        }
        else if (val == "top" || val == "bottom") {
            m_currentTableStyleProperties->verticalAlign = val;
        } else {
            m_currentTableStyleProperties->verticalAlign = "automatic";
        }
        m_currentTableStyleProperties->setProperties |= MSOOXML::TableStyleProperties::VerticalAlign;
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL textDirection
//! textDirection Handler (Table Cell Text Flow Direction)
/*
 Parent elements:
 - [done] tcPr (§17.7.6.8);
 - [done] tcPr (§17.7.6.9);
 - [done] tcPr (§17.4.70);
 - [done] tcPr (§17.4.71)

 Child elements:
 - none
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_textDirectionTc()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR(val)
    if (!val.isEmpty()) {
        m_currentTableStyleProperties->glyphOrientation = false;
        m_currentTableStyleProperties->setProperties |= MSOOXML::TableStyleProperties::GlyphOrientation;
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL vMerge
//! vMerge Handler (vertically merged cell)
/*
 Parent elements:
 - [done] tcPr (§17.7.6.8);
 - [done] tcPr (§17.7.6.9);
 - [done] tcPr (§17.4.70);
 - [done] tcPr (§17.4.71)

 Child elements:
 - none
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_vMerge()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR(val)
    if (!val.isEmpty()) { // Not empty value marks the start of the vertical merging
        KoCell* cell = m_table->cellAt(m_currentTableRowNumber, m_currentTableColumnNumber);
        cell->setRowSpan(1);
    } else {
        // In this case, it should continue from which ever cell above was the starter
        KoCell* cell = m_table->cellAt(m_currentTableRowNumber, m_currentTableColumnNumber);
        cell->setCovered(true);
        int previousRow = m_currentTableRowNumber - 1;
        while (previousRow > -1) {
            KoCell* previousRowCell = m_table->cellAt(previousRow, m_currentTableColumnNumber);
            if (!previousRowCell->isCovered()) {
                previousRowCell->setRowSpan(previousRowCell->rowSpan() + 1);
                // This current cell should be replaced with <table:covered-table-cell>
                cell->setCovered(true);
                break;
            }
            --previousRow;
        }
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL tcBorders
//! tcBorders handlers (table cell borders)
/*! ECMA-376, 17.4.67, p.494.

 Parent elements:
 - [done] tcPr (§17.7.6.8);
 - [done] tcPr (§17.7.6.9);
 - [done] tcPr (§17.4.70);
 - [done] tcPr (§17.4.71)

 Child elements:
 - [done] bottom (Table Cell Bottom Border) §17.4.3
 - [done] end (Table Cell Trailing Edge Border) §17.4.12
 - [done] insideH (Table Cell Inside Horizontal Edges Border) §17.4.24
 - [done] insideV (Table Cell Inside Vertical Edges Border) §17.4.26
 - [done] start (Table Cell Leading Edge Border) §17.4.34
 - [done] tl2br (Table Cell Top Left to Bottom Right Diagonal Border) §17.4.74
 - [done] top (Table Cell Top Border) §17.4.75
 - [done] tr2bl (Table Cell Top Right to Bottom Left Diagonal Border) §17.4.80
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_tcBorders()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            if (QUALIFIED_NAME_IS(top)) {
                m_currentTableStyleProperties->top = getBorderData();
                m_currentTableStyleProperties->setProperties |= MSOOXML::TableStyleProperties::TopBorder;
            }
            else if (QUALIFIED_NAME_IS(bottom)) {
                m_currentTableStyleProperties->bottom = getBorderData();
                m_currentTableStyleProperties->setProperties |= MSOOXML::TableStyleProperties::BottomBorder;
            }
            else if (QUALIFIED_NAME_IS(left)) {
                m_currentTableStyleProperties->left = getBorderData();
                m_currentTableStyleProperties->setProperties |= MSOOXML::TableStyleProperties::LeftBorder;
            }
            else if (QUALIFIED_NAME_IS(right)) {
                m_currentTableStyleProperties->right = getBorderData();
                m_currentTableStyleProperties->setProperties |= MSOOXML::TableStyleProperties::RightBorder;
            }
            else if (QUALIFIED_NAME_IS(insideV)) {
                m_currentTableStyleProperties->insideV = getBorderData();
                m_currentTableStyleProperties->setProperties |= MSOOXML::TableStyleProperties::InsideVBorder;
            }
            else if (QUALIFIED_NAME_IS(insideH)) {
                m_currentTableStyleProperties->insideH = getBorderData();
                m_currentTableStyleProperties->setProperties |= MSOOXML::TableStyleProperties::InsideHBorder;
            }
            else if (QUALIFIED_NAME_IS(tl2br)) {
                m_currentTableStyleProperties->tl2br = getBorderData();
                m_currentTableStyleProperties->setProperties |= MSOOXML::TableStyleProperties::Tl2brBorder;
            }
            else if (QUALIFIED_NAME_IS(tr2bl)) {
                m_currentTableStyleProperties->tr2bl = getBorderData();
                m_currentTableStyleProperties->setProperties |= MSOOXML::TableStyleProperties::Tr2blBorder;
            }
            SKIP_UNKNOWN
        }
    }
    m_currentTableStyleProperties->target = MSOOXML::TableStyleProperties::TableCell;

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL gridSpan
//! gridSpan handler  (Grid Columns Spanned by Current Table Cell)
/*!
 Parent elements:
 - [done]tcPr (§17.7.6.8);
 - [done] tcPr (§17.7.6.9);
 - [done] tcPr (§17.4.70);
 - [done] tcPr (§17.4.71)

 Child elements:
 - none
*/
//! @todo support all child elements
KoFilter::ConversionStatus DocxXmlDocumentReader::read_gridSpan()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR(val)
    if (!val.isEmpty()) {
        int span = 0;
        STRING_TO_INT(val, span, "gridSpan");
        KoCell* const cell = m_table->cellAt(m_currentTableRowNumber, m_currentTableColumnNumber);
        cell->setColumnSpan(span);
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL control
//! Reads a activeX control object
/*
 Parent elements:
 - [done] object (§17.3.3.19)

 Child elements:
 - none
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_control()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    body->startElement("draw:frame");
    body->addAttribute("svg:width", m_currentObjectWidthCm);
    body->addAttribute("svg:height", m_currentObjectHeightCm);
    body->addAttribute("text:anchor-type", "as-char"); // default for these

    // Do we even want to try anything with activeX controls?
    /*
    TRY_READ_ATTR_WITH_NS(r, id)
    const QString oleName(m_context->relationships->target(m_context->path, m_context->file, r_id));
    debugDocx << "oleName:" << oleName;
    */

    // Replacement image
    body->startElement("draw:image");
    body->addAttribute("xlink:type", "simple");
    body->addAttribute("xlink:show", "embed");
    body->addAttribute("xlink:actuate", "onLoad");
    body->addAttribute("xlink:href", m_currentVMLProperties.imagedataPath);
    body->endElement(); // draw:image

    body->endElement(); // draw:frame

    readNext();

    READ_EPILOGUE
}

// ---------------------------------------------------------------------------

#include <MsooXmlCommonReaderImpl.h> // this adds w:p, w:pPr, w:t, w:r, etc.

// ---------------------------------------------------------------------------

#undef MSOOXML_CURRENT_NS
#define MSOOXML_CURRENT_NS "o" // urn:schemas-microsoft-com:office:office

#undef CURRENT_EL
#define CURRENT_EL OLEObject
//! Reads an OLE object
KoFilter::ConversionStatus DocxXmlDocumentReader::read_OLEObject()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
// example:
// <o:OLEObject Type="Embed" ProgID="Visio.Drawing.11" ShapeID="_x0000_i1025"
//              DrawAspect="Content" ObjectID="_1240488905" r:id="rId10"/>

    body->startElement("draw:frame");
    body->addAttribute("svg:width", m_currentObjectWidthCm);
    body->addAttribute("svg:height", m_currentObjectHeightCm);
    body->addAttribute("text:anchor-type", "as-char"); // default for these

    TRY_READ_ATTR_WITH_NS(r, id)
    const QString oleName(m_context->relationships->target(m_context->path, m_context->file, r_id));
    debugDocx << "oleName:" << oleName;

    QString destinationName = oleName.mid(oleName.lastIndexOf('/') + 1);
    KoFilter::ConversionStatus stat = m_context->import->copyFile(oleName, destinationName, false);
    if (stat == KoFilter::OK) {
        body->startElement("draw:object-ole");
        addManifestEntryForFile(destinationName);
        body->addAttribute("xlink:href", destinationName);
        body->addAttribute("xlink:type", "simple");
        body->endElement(); // draw:object-ole
    }

    // Replacement image
    body->startElement("draw:image");
    body->addAttribute("xlink:type", "simple");
    body->addAttribute("xlink:show", "embed");
    body->addAttribute("xlink:actuate", "onLoad");
    body->addAttribute("xlink:href", m_currentVMLProperties.imagedataPath);
    body->endElement(); // draw:image

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
    }

    body->endElement(); // draw:frame

    READ_EPILOGUE
}

#undef MSOOXML_CURRENT_NS
#define MSOOXML_CURRENT_NS "wp"

#undef CURRENT_EL
#define CURRENT_EL anchor
//! anchor handler (Anchor for Floating DrawingML Object)
/*! ECMA-376, 20.4.2.3, p.3469.

 This element specifies that the DrawingML object located at this position
 in the document is a floating object.
 Within a WordprocessingML document, drawing objects can exist in two states:
 - Inline - The drawing object is in line with the text, and affects the line
   height and layout of its line (like a character glyph of similar size).
 - Floating - The drawing object is anchored within the text, but can be
   absolutely positioned in the document relative to the page.

 When this element encapsulates the DrawingML object's information,
 then all child elements shall dictate the positioning of this object
 as a floating object on the page.

 Parent elements:
 - [done] drawing (§17.3.3.9)

 Child elements:
 - cNvGraphicFramePr (Common DrawingML Non-Visual Properties) §20.4.2.4
 - [done] docPr (Drawing Object Non-Visual Properties) §20.4.2.5
 - effectExtent (Object Extents Including Effects) §20.4.2.6
 - extent (Drawing Object Size) §20.4.2.7
 - [done] graphic (Graphic Object) §20.1.2.2.16
 - [done] positionH (Horizontal Positioning) §20.4.2.10
 - [done] positionV (Vertical Positioning) §20.4.2.11
 - simplePos (Simple Positioning Coordinates) §20.4.2.13
 - [done] wrapNone (No Text Wrapping) §20.4.2.15
 - [done] wrapSquare (Square Wrapping) §20.4.2.17
 - [done] wrapThrough (Through Wrapping) §20.4.2.18
 - [done] wrapTight (Tight Wrapping) §20.4.2.19
 - [done] wrapTopAndBottom (Top and Bottom Wrapping) §20.4.2.20

 Attributes:
 - [done] allowOverlap (Allow Objects to Overlap)
 - [done] behindDoc (Display Behind Document Text)
 - [done] distB (Distance From Text on Bottom Edge) (see also: inline)
 - [done] distL (Distance From Text on Left Edge) (see also: inline)
 - [done] distR (Distance From Text on Right Edge) (see also: inline)
 - [done] distT (Distance From Text on Top Edge) (see also: inline)
 - hidden (Hidden)
 - [done] layoutInCell (Layout In Table Cell)
 - locked (Lock Anchor)
 - [done] relativeHeight (Relative Z-Ordering Position)
 - simplePos (Page Positioning)
*/
//! @todo support all elements
//! CASE #1340
//! CASE #1410
//! CASE #1420
KoFilter::ConversionStatus DocxXmlDocumentReader::read_anchor()
{
    READ_PROLOGUE

    m_docPrName.clear();
    m_docPrDescr.clear();
    m_drawing_anchor = true; // for pic:pic
    bool behindDoc = false;
    bool allowOverlap = false;
    m_alignH.clear();
    m_alignV.clear();

    const QXmlStreamAttributes attrs(attributes());
//! @todo parse 20.4.3.4 ST_RelFromH (Horizontal Relative Positioning), p. 3511
    READ_ATTR_WITHOUT_NS(distT)
    distToODF("fo:margin-top", distT);
    READ_ATTR_WITHOUT_NS(distB)
    distToODF("fo:margin-bottom", distB);
    READ_ATTR_WITHOUT_NS(distL)
    distToODF("fo:margin-left", distL);
    READ_ATTR_WITHOUT_NS(distR)
    distToODF("fo:margin-right", distR);

    TRY_READ_ATTR_WITHOUT_NS(relativeHeight)
    if (!relativeHeight.isEmpty()) {
        m_z_index = relativeHeight.toInt();
    }

    TRY_READ_ATTR_WITHOUT_NS(layoutInCell)
    m_currentDrawStyle->addProperty("style:flow-with-text",
                                    MSOOXML::Utils::convertBooleanAttr(layoutInCell, false));

    behindDoc = MSOOXML::Utils::convertBooleanAttr(attrs.value("behindDoc").toString());
    allowOverlap = MSOOXML::Utils::convertBooleanAttr(attrs.value("allowOverlap").toString());

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF_NS(a, graphic)
            ELSE_TRY_READ_IF(positionH)
            ELSE_TRY_READ_IF(positionV)
            ELSE_TRY_READ_IF(docPr)
            ELSE_TRY_READ_IF(wrapSquare)
            ELSE_TRY_READ_IF(wrapTight)
            ELSE_TRY_READ_IF(wrapThrough)
            else if (QUALIFIED_NAME_IS(wrapNone)) {
                // wrapNone (No Text Wrapping), ECMA-376, 20.4.2.15
                // This element specifies that the parent DrawingML object shall
                // not cause any text wrapping within the contents of the host
                // WordprocessingML document based on its display location.
                // CASE #1410
                readNext();
                if (!expectElEnd(QUALIFIED_NAME(wrapNone))) {
                    return KoFilter::WrongFormat;
                }
                if (allowOverlap) {
                    m_currentDrawStyle->addProperty("style:wrap", "run-through");
                    if (behindDoc) {
                        m_currentDrawStyle->addProperty("style:run-through", "background");
                    }
                    else {
                        m_currentDrawStyle->addProperty("style:run-through", "foreground");
                    }
                }
                else {
                    m_currentDrawStyle->addProperty("style:wrap", "none");
                }
            } else if (QUALIFIED_NAME_IS(wrapTopAndBottom)) {
                // 20.4.2.20 wrapTopAndBottom (Top and Bottom Wrapping)
                // This element specifies that text shall wrap around the top
                // and bottom of this object, but not its left or right edges.
                // CASE #1410
                readNext();
                if (!expectElEnd(QUALIFIED_NAME(wrapTopAndBottom))) {
                    return KoFilter::WrongFormat;
                }
                m_currentDrawStyle->addProperty("style:wrap", "none");
            }
            SKIP_UNKNOWN
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}


#undef CURRENT_EL
#define CURRENT_EL inline
//! inline handler (Inline DrawingML Object)
/*! ECMA-376, 20.4.2.8, p.3485.

 This element specifies that the DrawingML object located at this position
 in the document is a floating object.
 Within a WordprocessingML document, drawing objects can exist in two states:
 - Inline - The drawing object is in line with the text, and affects the line
   height and layout of its line (like a character glyph of similar size).
 - Floating - The drawing object is anchored within the text, but can be
   absolutely positioned in the document relative to the page.

 When this element encapsulates the DrawingML object's information,
 then all child elements shall dictate the positioning of this object
 as a floating object on the page.

 Parent elements:
 - [done] drawing (§17.3.3.9)

 Child elements:
 - cNvGraphicFramePr (Common DrawingML Non-Visual Properties) §20.4.2.4
 - [done] docPr (Drawing Object Non-Visual Properties) §20.4.2.5
 - effectExtent (Object Extents Including Effects) §20.4.2.6
 - [done] extent (Drawing Object Size) §20.4.2.7
 - [done] graphic (Graphic Object) §20.1.2.2.16

 Attributes:
 - distB (Distance From Text on Bottom Edge)
 - distL (Distance From Text on Left Edge)
 - distR (Distance From Text on Right Edge)
 - distT (Distance From Text on Top Edge)
*/
//! @todo support all elements
KoFilter::ConversionStatus DocxXmlDocumentReader::read_inline()
{
    READ_PROLOGUE

    m_docPrName.clear();
    m_docPrDescr.clear();
    m_drawing_inline = true; // for pic
    m_svgX = m_svgY = m_svgWidth = m_svgHeight = 0;
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF_NS(wp, extent)
            ELSE_TRY_READ_IF(docPr)
            ELSE_TRY_READ_IF_NS(a, graphic)
            SKIP_UNKNOWN
//! @todo add ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL extent
KoFilter::ConversionStatus DocxXmlDocumentReader::read_extent()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    READ_ATTR_WITHOUT_NS(cx)
    STRING_TO_INT(cx, m_svgWidth, "ext@cx")
    READ_ATTR_WITHOUT_NS(cy)
    STRING_TO_INT(cy, m_svgHeight, "ext@cy")

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL docPr
//! docPr handler (Drawing Object Non-Visual Properties)
/*! ECMA-376, 20.4.2.5, p.3478.

 This element specifies non-visual object properties for the parent DrawingML object.
 These properties are specified as child elements of this element.

 Parent elements:
 - [done] anchor (§20.4.2.3)
 - [done] inline (§20.4.2.8)

 Child elements:
 - extLst (Extension List) §20.1.2.2.15
 - [done] hlinkClick (Click Hyperlink) §21.1.2.3.5
 - hlinkHover (Hyperlink for Hover) §20.1.2.2.23

 Attributes:
 - [done] descr (Alternative Text for Object)
 - hidden (Hidden)
 - id (Unique Identifier)
 - [done] name (Name)
*/
//! CASE #1340
//! @todo support all elements
KoFilter::ConversionStatus DocxXmlDocumentReader::read_docPr()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS_INTO(name, m_docPrName)
    TRY_READ_ATTR_WITHOUT_NS_INTO(descr, m_docPrDescr)
//! @todo support docPr/@hidden (maybe to style:text-properties/@text:display)

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF_NS(a, hlinkClick)
            SKIP_UNKNOWN
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL positionH
//! positionH handler (Horizontal Positioning)
/*! ECMA-376, 20.4.2.10, p.3490.
 This element specifies the horizontal positioning of a floating
 DrawingML object within a WordprocessingML document.
 This positioning is specified in two parts:

 - Positioning Base - The relativeFrom attribute on this element
   specifies the part of the document from which the positioning
   shall be calculated.
 - Positioning - The child element of this element (align
   or posOffset) specifies how the object is positioned relative
   to that base.

 Parent elements:
 - [done] anchor (§20.4.2.3)

 Child elements:
 - [done] align (Relative Horizontal Alignment) §20.4.2.1
 - [done] posOffset (Absolute Position Offset) §20.4.2.12

 Attributes:
 - [done] relativeFrom (Horizontal Position Relative Base)
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_positionH()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
//! @todo parse 20.4.3.4 ST_RelFromH (Horizontal Relative Positioning), p. 3511
    READ_ATTR_WITHOUT_NS_INTO(relativeFrom, m_relativeFromH)

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF_IN_CONTEXT(align)
            ELSE_TRY_READ_IF_IN_CONTEXT(posOffset)
            ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL positionV
//! positionV handler (Vertical Positioning)
/*! ECMA-376, 20.4.2.11, p.3491.
 This element specifies the vertical positioning of a floating
 DrawingML object within a WordprocessingML document.
 This positioning is specified in two parts:

 - Positioning Base - The relativeFrom attribute on this element
   specifies the part of the document from which the positioning
   shall be calculated.
 - Positioning - The child element of this element (align
   or posOffset) specifies how the object is positioned relative
   to that base.

 Parent elements:
 - [done] anchor (§20.4.2.3)

 Child elements:
 - [done] align (Relative Vertical Alignment) §20.4.2.2
 - [done] posOffset (Absolute Position Offset) §20.4.2.12

 Attributes:
 - [done] relativeFrom (Horizontal Position Relative Base)
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_positionV()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
//! @todo parse 20.4.3.5 ST_RelFromV (Vertical Relative Positioning), p. 3512
    READ_ATTR_WITHOUT_NS_INTO(relativeFrom, m_relativeFromV)

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF_IN_CONTEXT(align)
            ELSE_TRY_READ_IF_IN_CONTEXT(posOffset)
            ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL posOffset
//! posOffset handler (Absolute Position Offset)
/*! ECMA-376, 20.4.2.12, p.3492.
 This element specifies an absolute measurement for the positioning
 of a floating DrawingML object within a WordprocessingML document.
 This measurement shall be calculated relative to the top left edge
 of the positioning base specified by the parent element's
 relativeFrom attribute.

 Parent elements:
 - [done] positionH (§20.4.2.10)
 - [done] positionV (§20.4.2.11)

 No child elements.
*/
//! CASE #1360
KoFilter::ConversionStatus DocxXmlDocumentReader::read_posOffset(posOffsetCaller caller)
{
    READ_PROLOGUE

    readNext();
    if (isCharacters()) {
        switch (caller) {
        case posOffset_positionH:
            STRING_TO_INT(text().toString(), m_posOffsetH, "positionH/posOffset text")
            m_hasPosOffsetH = true;
            break;
        case posOffset_positionV:
            STRING_TO_INT(text().toString(), m_posOffsetV, "positionV/posOffset text")
            m_hasPosOffsetV = true;
            break;
        default:
            return KoFilter::WrongFormat;
        }
    }
    ELSE_WRONG_FORMAT

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL

void DocxXmlDocumentReader::readWrapAttrs()
{
    if (QUALIFIED_NAME_IS(wrapTight)) {
        m_currentDrawStyle->addProperty("style:wrap-contour", "true");
        m_currentDrawStyle->addProperty("style:wrap-contour-mode", "outside");
    }
    else if (QUALIFIED_NAME_IS(wrapThrough)) {
        m_currentDrawStyle->addProperty("style:wrap-contour", "true");
        m_currentDrawStyle->addProperty("style:wrap-contour-mode", "full");
    }
    m_currentDrawStyle->addProperty("style:number-wrapped-paragraphs", "no-limit");

    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(wrapText)

    if (wrapText == "bothSides") {
        m_currentDrawStyle->addProperty("style:wrap", "parallel");
    }
    else if (wrapText == "largest") {
        m_currentDrawStyle->addProperty("style:wrap", "biggest");
    } else {
        m_currentDrawStyle->addProperty("style:wrap", wrapText);
    }
}

#define CURRENT_EL wrapPolygon
//! wrapPolygon (Wrapping Polygon)
/*! ECMA-376, 20.4.2.16, p.3495
 This element specifies the wrapping polygon which shall be used to determine
 the extents to which text can wrap around the specified object in the
 document. This polygon shall be defined by the following:

 - The start element defines the coordinates of the origin of the wrap polygon
 - Two or more lineTo elements define the point of the wrap polygon

 TODO: If the set of child elements does not result in a closed polygon (the
 last lineTo element does not return to the position specified by the start
 element), then a single additional line shall be inferred as needed to close
 the wrapping polygon.

 Parent elements:
 - [done] wrapThrough (§20.4.2.18)
 - [done] wrapTight (§20.4.2.19)

 Child elements:
 - lineTo (Wrapping Polygon Line End Position)
 - start (Wrapping Polygon Start)

 Attributes:
 - edited (Wrapping Points Modified)
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_wrapPolygon()
{
    READ_PROLOGUE
    QString points;

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            if (QUALIFIED_NAME_IS(start) || QUALIFIED_NAME_IS(lineTo)) {
                const QXmlStreamAttributes attrs(attributes());
                READ_ATTR_WITHOUT_NS(x)
                READ_ATTR_WITHOUT_NS(y)

                bool ok;
                int _x = x.toInt(&ok);
                int _y = y.toInt(&ok);

                // EMUs - TODO: Which units do I have to use?
                if (ok) {
                    // x = QString::number(PT_TO_PX(EMU_TO_POINT(_x)), 'f');
                    // y = QString::number(PT_TO_PX(EMU_TO_POINT(_y)), 'f');
                    x = QString::number(EMU_TO_POINT(_x), 'f');
                    y = QString::number(EMU_TO_POINT(_y), 'f');
                }
                //TODO: else a number followed immediately by a unit identifier.

                //TODO: find max and min to set svg:width, svg:height and svg:viewBox

                points.append(x);
                points.append(",");
                points.append(y);
                points.append(" ");

                skipCurrentElement();
            }
            SKIP_UNKNOWN
        }
    }

    if (!points.isEmpty()) {
        points.chop(1); //remove last space
        body->startElement("draw:contour-polygon");
        body->addAttribute("draw:points", points);
        body->endElement(); //draw:contour-polygon
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL wrapSquare
//! wrapSquare handler (Square Wrapping)
/*! ECMA-376, 20.4.2.17, p.3496.
 This element specifies that text shall wrap around a virtual rectangle bounding
 this object. The bounds of the wrapping rectangle shall be dictated by the extents
 including the addition of the effectExtent element as a child of this element
 (if present) or the effectExtent present on the parent element.

 Parent elements:
 - [done] anchor (§20.4.2.3)

 Child elements:
 - effectExtent (Object Extents Including Effects)

 Attributes:
 - distB (Distance From Text on Bottom Edge)
 - distL (Distance From Text on Left Edge)
 - distR (Distance From Text on Right Edge)
 - distT (Distance From Text (Top))
 - [done] wrapText (Text Wrapping Location)
*/
//! CASE #1410
KoFilter::ConversionStatus DocxXmlDocumentReader::read_wrapSquare()
{
    READ_PROLOGUE
    readWrapAttrs();

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
//        if (isStartElement()) {
//! @todo effectExtent
//        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL wrapTight
//! wrapTight handler (Tight Wrapping)
/*! ECMA-376, 20.4.2.17, p.3497.
 This element specifies that text shall wrap around the wrapping polygon
 bounding this object as defined by the child wrapPolygon element.
 When this element specifies a wrapping polygon, it shall not allow text
 to wrap within the object's maximum left and right extents.

 Parent elements:
 - [done] anchor (§20.4.2.3)

 Child elements:
 - wrapPolygon (Wrapping Polygon)

 Attributes:
 - distL (Distance From Text on Left Edge)
 - distR (Distance From Text on Right Edge)
 - [done] wrapText (Text Wrapping Location)
*/
//! CASE #1410
KoFilter::ConversionStatus DocxXmlDocumentReader::read_wrapTight()
{
    READ_PROLOGUE
    readWrapAttrs();

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        // if (isStartElement()) {
        //     TRY_READ_IF(wrapPolygon)
        //     SKIP_UNKNOWN
        // }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL wrapThrough
//! wrapThrough handler (Through Wrapping)
/*! ECMA-376, 20.4.2.18, p.3500.
 This element specifies that text shall wrap around the wrapping polygon
 bounding this object as defined by the child wrapPolygon element.
 When this element specifies a wrapping polygon, it shall allow text
 to wrap within the object's maximum left and right extents.

 Parent elements:
 - [done] anchor (§20.4.2.3)

 Child elements:
 - wrapPolygon (Wrapping Polygon)

 Attributes:
 - distL (Distance From Text on Left Edge)
 - distR (Distance From Text on Right Edge)
 - [done] wrapText (Text Wrapping Location)
*/
//! CASE #1410
KoFilter::ConversionStatus DocxXmlDocumentReader::read_wrapThrough()
{
    READ_PROLOGUE
    readWrapAttrs();

    // NOTE: The name wrapThrough is misleading, it does not mean run-through it
    // just means that wrapping happens potentially inside of the container.

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        // if (isStartElement()) {
        //     TRY_READ_IF(wrapPolygon)
        //     SKIP_UNKNOWN
        // }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL align
//! align handler (Relative Horizontal Alignment, Relative Vertical Alignment)
/*! ECMA-376, 20.4.2.1, 20.4.2.2, p.3468, 3469.
 This element specifies how a DrawingML object shall be horizontally/vertically
 aligned relative to the horizontal alignment base defined
 by the parent element. Once an alignment base is defined,
 this element shall determine how the DrawingML object shall
 be aligned relative to that location.

 Parent elements:
 - [done] positionH (§20.4.2.10)
 - [done] positionV (§20.4.2.11)

 No child elements.
*/
//! CASE #1340
KoFilter::ConversionStatus DocxXmlDocumentReader::read_align(alignCaller caller)
{
    READ_PROLOGUE
    readNext();

    if (!isEndElement()) {// was text
       switch (caller) {
       case align_positionH:
//! 20.4.3.1 ST_AlignH (Relative Horizontal Alignment Positions), p. 3508.
            /*center
            inside
            left
            outside
            right*/
            m_alignH = text().toString();
            break;
        case align_positionV:
//! 20.4.3.2 ST_AlignV (Vertical Alignment Definition), p. 3509.
            /*bottom
            center
            inside
            outside
            top*/
            m_alignV = text().toString();
        break;
        }
        readNext();
    }

    READ_EPILOGUE
}

#undef MSOOXML_CURRENT_NS
#define MSOOXML_CURRENT_NS "m"

#undef CURRENT_EL
#define CURRENT_EL oMathPara
//! oMathPara handler
// @todo implement...
KoFilter::ConversionStatus DocxXmlDocumentReader::read_oMathPara()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(oMath)
            ELSE_TRY_READ_IF(oMathParaPr)
            SKIP_UNKNOWN
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL oMathParaPr
//! oMathParaPr handler (paragraph properties)
// @todo support fully
KoFilter::ConversionStatus DocxXmlDocumentReader::read_oMathParaPr()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            if (qualifiedName() == "m:jc") {
                TRY_READ(jc_m)
            }
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL oMath
//! oMath handler
// @todo implement...
KoFilter::ConversionStatus DocxXmlDocumentReader::read_oMath()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            if (qualifiedName() == "m:r") {
                TRY_READ(r_m)
            }
            ELSE_TRY_READ_IF(del)
            ELSE_TRY_READ_IF(ins)
            SKIP_UNKNOWN
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL jc
//! jc handler for math
// @todo implement..
KoFilter::ConversionStatus DocxXmlDocumentReader::read_jc_m()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR(val)

    if (!val.isEmpty()) {
        if (val == "centerGroup") {
            m_currentParagraphStyle.addProperty("fo:text-align", "center");
        }
        else {
            // Fix this based on possible values,
            m_currentParagraphStyle.addProperty("fo:text-align", "left");
        }
    }

    readNext();

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL t
//! t handler for math
// @todo implement...
KoFilter::ConversionStatus DocxXmlDocumentReader::read_t_m()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        debugDocx << *this;
        if (isCharacters()) {
            body->addTextSpan(text().toString());
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL r
//! r handler in math
// @todo implement fully.
KoFilter::ConversionStatus DocxXmlDocumentReader::read_r_m()
{
    READ_PROLOGUE

    m_currentTextStyle = KoGenStyle(KoGenStyle::TextAutoStyle, "text");
    if (m_moveToStylesXml) {
        m_currentTextStyle.setAutoStyleInStylesDotXml(true);
    }

    MSOOXML::Utils::XmlWriteBuffer buffer;
    body = buffer.setWriter(body);

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF_NS(w, rPr)
            else if (qualifiedName() == "m:t") {
                TRY_READ(t_m)
            }
            SKIP_UNKNOWN
        }
    }

    body = buffer.originalWriter();

    QString currentTextStyleName = mainStyles->insert(m_currentTextStyle);

    body->startElement("text:span", false);
    body->addAttribute("text:style-name", currentTextStyleName);

    bool closeHyperLink = handleSpecialField();

    // Writing the internal body of read_t now
    body = buffer.releaseWriter();

    if (closeHyperLink) {
        body->endElement(); // some special field
    }

    body->endElement(); //text:span

    READ_EPILOGUE
}

// ************************************************
//  State
// ************************************************
void DocxXmlDocumentReader::saveState()
{
    DocumentReaderState state(m_usedListStyles, m_continueListNum, m_numIdXmlId);
    m_statesBkp.push(state);

    m_usedListStyles.clear();
    m_continueListNum.clear();
    m_numIdXmlId.clear();
}

void DocxXmlDocumentReader::restoreState()
{
    if (m_statesBkp.isEmpty()) {
        warnDocx << "Error: DocumentReaderState stack is corrupt!";
        return;
    }
    DocumentReaderState s = m_statesBkp.pop();
    m_usedListStyles = s.usedListStyles;
    m_continueListNum = s.continueListNum;
    m_numIdXmlId = s.numIdXmlId;
}

#define DRAWINGML_NS "a"
#define DRAWINGML_PIC_NS "pic" // DrawingML/Picture

#include <MsooXmlCommonReaderDrawingMLImpl.h> // this adds pic:pic, etc.
#include <MsooXmlVmlReaderImpl.h> // this adds w:pict, etc.
