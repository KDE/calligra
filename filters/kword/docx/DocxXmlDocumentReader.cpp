/*
 * This file is part of Office 2007 Filters for KOffice
 *
 * Copyright (C) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2010 KoGmbh (casper.boemann@kogmbh.com).
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
#include "DocxXmlNotesReader.h"
#include "DocxXmlHeaderReader.h"
#include "DocxXmlFooterReader.h"
#include "DocxImport.h"
#include <MsooXmlSchemas.h>
#include <MsooXmlUtils.h>
#include <MsooXmlRelationships.h>
#include <MsooXmlUnits.h>
#include <KoXmlWriter.h>
#include <KoGenStyles.h>

#define MSOOXML_CURRENT_NS "w"
#define MSOOXML_CURRENT_CLASS DocxXmlDocumentReader
#define BIND_READ_CLASS MSOOXML_CURRENT_CLASS
#define DOCXXMLDOCREADER_CPP

#include <MsooXmlReader_p.h>

DocxXmlDocumentReaderContext::DocxXmlDocumentReaderContext(
    DocxImport& _import,
    const QString& _path, const QString& _file,
    MSOOXML::MsooXmlRelationships& _relationships,
    const QMap<QString, MSOOXML::DrawingMLTheme*>& _themes)
        : MSOOXML::MsooXmlReaderContext(&_relationships),
        import(&_import), path(_path), file(_file),
        themes(&_themes)
{
}

// ---------------------------------------------------------------------

//! Column style info, allows to keep information about repeated columns
class ColumnStyleInfo
{
public:
    ColumnStyleInfo(KoGenStyle *s = 0) : count(0), style(s) {}
    uint count;
    KoGenStyle* style; //!< not owned
};

class DocxXmlDocumentReader::Private
{
public:
    Private() {
    }
    ~Private() {
    }

    QList<ColumnStyleInfo> columnStyles; //!< for collecting column styles

    void clearColumnStyles() {
        foreach (const ColumnStyleInfo& info, columnStyles) {
            delete info.style;
        }
        columnStyles.clear();
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
    m_objectRectInitialized = false;
    m_wasCaption = false;
    m_listFound = false;
    m_closeHyperlink = false;
}

KoFilter::ConversionStatus DocxXmlDocumentReader::read(MSOOXML::MsooXmlReaderContext* context)
{
    m_context = dynamic_cast<DocxXmlDocumentReaderContext*>(context);
    kDebug() << "=============================";
    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }

    //w:document
    readNext();
    kDebug() << *this << namespaceUri();

    if (!expectEl("w:document")) {
        return KoFilter::WrongFormat;
    }
    if (!expectNS(MSOOXML::Schemas::wordprocessingml)) {
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
    if (!namespaces.contains(QXmlStreamNamespaceDeclaration("w", MSOOXML::Schemas::wordprocessingml))) {
        raiseError(i18n("Namespace \"%1\" not found", MSOOXML::Schemas::wordprocessingml));
        return KoFilter::WrongFormat;
    }
//! @todo expect other namespaces too...

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(body)
            ELSE_TRY_READ_IF(background)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(document)
    }

    if (!expectElEnd("w:document")) {
        return KoFilter::WrongFormat;
    }
    kDebug() << "===========finished============";
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
 - bookmarkEnd (Bookmark End) §17.13.6.1
 - bookmarkStart (Bookmark Start) §17.13.6.2
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
 - [done] sectPr (Document Final Section Properties) §17.6.17
 - [done] tbl (Table) §17.4.38
*/
//! @todo support all child elements
KoFilter::ConversionStatus DocxXmlDocumentReader::read_body()
{
    READ_PROLOGUE
/*    QXmlStreamNamespaceDeclarations namespaces = namespaceDeclarations();
    for (int i = 0; i < namespaces.count(); i++) {
        kDebug() << "NS prefix:" << namespaces[i].prefix() << "uri:" << namespaces[i].namespaceUri();
    }*/
    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(p)
            ELSE_TRY_READ_IF(sectPr)
            ELSE_TRY_READ_IF(tbl)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
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
 - bidi (Right to Left Section Layout) §17.6.1
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

    while (!atEnd()) {
        readNext();
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
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    // Currently if there are 3 header/footer styles, the one with 'first' is ignored
    if (!m_headers.isEmpty()) {
        bool odd = false;
        if (m_headers["even"] != "") {
            m_masterPageStyle.addChildElement("style:header-left", m_headers["even"]);
        }
        if (m_headers["default"] != "") {
            odd = true;
            m_masterPageStyle.addChildElement("style:header", m_headers["default"]);
        }
        if (!odd) {
           m_masterPageStyle.addChildElement("style:header", m_headers["first"]);
        }
    }

    if (!m_footers.isEmpty()) {
        bool odd = false;
        if (m_footers["even"] != "") {
            m_masterPageStyle.addChildElement("style:footer-left", m_footers["even"]);
        }
        if (m_footers["default"] != "") {
            odd = true;
            m_masterPageStyle.addChildElement("style:footer", m_footers["default"]);
        }
        if (!odd) {
           m_masterPageStyle.addChildElement("style:footer", m_footers["first"]);
        }
    }

    QString pageLayoutStyleName("Mpm");
    pageLayoutStyleName = mainStyles->insert(
        m_currentPageStyle, pageLayoutStyleName, KoGenStyles::DontAddNumberToName);

//! @todo works because paragraphs have Standard style assigned by default; fix for multiple page styles
    QString masterStyleName("Standard");
//! @todo style:display-name
//    masterStyle->addAttribute("style:display-name", masterStyleName);
    m_masterPageStyle.addAttribute("style:page-layout-name", pageLayoutStyleName);
    /*masterStyleName =*/ mainStyles->insert(m_masterPageStyle, masterStyleName, KoGenStyles::DontAddNumberToName);
//    masterStyle = mainStyles->styleForModification(masterStyleName);

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL pgSz
//! w:pgSz handler (Page Size)
/*! ECMA-376, 17.6.13, p. 655.

 Parent elements:
 - [done] sectPr (§17.6.17)
 - sectPr (§17.6.18)
 - sectPr (§17.6.19)

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
        if (!s.isEmpty())
            m_currentPageStyle.addProperty("fo:page-width", s);
    }
    TRY_READ_ATTR(h)
    if (!h.isEmpty()) {
        const QString s(MSOOXML::Utils::TWIP_to_ODF(h));
        if (!s.isEmpty())
            m_currentPageStyle.addProperty("fo:page-height", s);
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
 - sectPr (§17.6.18)
 - sectPr (§17.6.19)

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
        m_currentPageStyle.addProperty("style:writing-mode", left + "-" + right);
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
 - sectPr (§17.6.18)
 - sectPr (§17.6.19)

 No child elements.
*/
//! @todo support all elements
KoFilter::ConversionStatus DocxXmlDocumentReader::read_pgMar()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR(top)
    if (!top.isEmpty()) {
        const QString s(MSOOXML::Utils::TWIP_to_ODF(top));
        if (!s.isEmpty())
            m_currentPageStyle.addProperty("fo:margin-top", s);
    }
    TRY_READ_ATTR(right)
    if (!right.isEmpty()) {
        const QString s(MSOOXML::Utils::TWIP_to_ODF(right));
        if (!s.isEmpty())
            m_currentPageStyle.addProperty("fo:margin-right", s);
    }
    TRY_READ_ATTR(bottom)
    if (!bottom.isEmpty()) {
        const QString s(MSOOXML::Utils::TWIP_to_ODF(bottom));
        if (!s.isEmpty())
            m_currentPageStyle.addProperty("fo:margin-bottom", s);
    }
    TRY_READ_ATTR(left)
    if (!left.isEmpty()) {
        const QString s(MSOOXML::Utils::TWIP_to_ODF(left));
        if (!s.isEmpty())
            m_currentPageStyle.addProperty("fo:margin-left", s);
    }
    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL footerReference
//! w:footerReference handler (Footer Reference)
/*!

 Parent elements:
 -[done] sectPr (§17.6.17)
 - sectPr (§17.6.8)

 Child elements:
 - None

*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_footerReference()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    QString link_target;

    TRY_READ_ATTR_WITH_NS(r, id)
    if (r_id.isEmpty()) {
        link_target.clear();
    }
    else {
        link_target = m_context->relationships->linkTarget(r_id, m_context->path, m_context->file);
    }

    DocxXmlFooterReader reader(this);

    QString errorMessage;

    MSOOXML::MsooXmlRelationships relationships(*m_context->import, m_writers, errorMessage);

    DocxXmlDocumentReaderContext context(*m_context->import, m_context->path, link_target,
        relationships, *m_context->themes);

    const KoFilter::ConversionStatus status
        = m_context->import->loadAndParseDocument(&reader, m_context->path + '/' + link_target, errorMessage, &context);
    if (status != KoFilter::OK) {
        reader.raiseError(errorMessage);
    }

    QString footerContent = "";

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
 -[done] sectPr (§17.6.17)
 - sectPr (§17.6.8)

 Child elements:
 - None

*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_headerReference()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    QString link_target;

    TRY_READ_ATTR_WITH_NS(r, id)
    if (r_id.isEmpty()) {
        link_target.clear();
    }
    else {
        link_target = m_context->relationships->linkTarget(r_id, m_context->path, m_context->file);
    }

    DocxXmlHeaderReader reader(this);

    QString errorMessage;

    MSOOXML::MsooXmlRelationships relationships(*m_context->import, m_writers, errorMessage);

    DocxXmlDocumentReaderContext context(*m_context->import, m_context->path, link_target,
        relationships, *m_context->themes);

    const KoFilter::ConversionStatus status
        = m_context->import->loadAndParseDocument(&reader, m_context->path + '/' + link_target, errorMessage, &context);
    if (status != KoFilter::OK) {
        reader.raiseError(errorMessage);
    }

    QString headerContent = "";

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
 - [done] sectPr (§17.6.18)
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
 - [done] sectPr (§17.6.18)
 - [done] sectPr (§17.6.19)

 Child elements:
 - [done] numFmt (Footnote Numbering Format) §17.11.18
 - numRestart (Footnote and Endnote Numbering Restart Location) §17.11.19
 - numStart (Footnote and Endnote Numbering Starting Value) §17.11.20
 - pos (Footnote Placement) §17.11.21

*/
//! @todo support all elements
KoFilter::ConversionStatus DocxXmlDocumentReader::read_endnotePr()
{
    READ_PROLOGUE

    QBuffer buffer;
    KoXmlWriter *oldBody = body;
    body = new KoXmlWriter(&buffer);

    body->startElement("text:notes-configuration");
    body->addAttribute("text:note-class", "endnote");

    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            TRY_READ_IF(numFmt)
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    body->endElement(); // text:notes-configuration

    QString endStyle = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());

    delete body;
    body = oldBody;

    mainStyles->insertRawOdfStyles(KoGenStyles::DocumentStyles, endStyle.toUtf8());

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL footnotePr
//! w:footnotePr handler (Footnote properties)
/*
 Parent elements:
 - [done] sectPr (§17.6.17)
 - [done] sectPr (§17.6.18)
 - [done] sectPr (§17.6.19)

 Child elements:
 - [done] numFmt (Footnote Numbering Format) §17.11.18
 - numRestart (Footnote and Endnote Numbering Restart Location) §17.11.19
 - numStart (Footnote and Endnote Numbering Starting Value) §17.11.20
 - pos (Footnote Placement) §17.11.21

*/
//! @todo support all elements
KoFilter::ConversionStatus DocxXmlDocumentReader::read_footnotePr()
{
    READ_PROLOGUE

    QBuffer buffer;
    KoXmlWriter *oldBody = body;
    body = new KoXmlWriter(&buffer);

    body->startElement("text:notes-configuration");
    body->addAttribute("text:note-class", "footnote");
    body->addAttribute("text:footnotes-position", "page");
    body->addAttribute("text:start-numbering-at", "document");

    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            TRY_READ_IF(numFmt)
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    body->endElement(); // text:notes-configuration

    QString footStyle = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());

    delete body;
    body = oldBody;

    mainStyles->insertRawOdfStyles(KoGenStyles::DocumentStyles, footStyle.toUtf8());

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL numFmt
//! w:numFmt handler (Footnote Numbering format)
/*
 Parent elements:
 - [done] footnotePr (§17.11.12)
 - [done] footnotePr (§17.11.11)

 Child elements:
 - none

*/
//! @toodo support all elements
KoFilter::ConversionStatus DocxXmlDocumentReader::read_numFmt()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR(val)

    if (!val.isEmpty()) {
        if (val == "upperLetter") {
            body->addAttribute("style:num-format", "A");
        }
        else if (val == "decimal") {
            body->addAttribute("style:num-format", "1");
        }
    }
    else { // For now let's use letter format as the default
        body->addAttribute("style:num-format", "A");
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
        if (isStartElement()) {
        }
        BREAK_IF_END_OF(CURRENT_EL);
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
 - sectPr (§17.6.18)
 - sectPr (§17.6.19)

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
    m_borderStyles.clear();
    m_borderPaddings.clear();
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            if (QUALIFIED_NAME_IS(top)) {
                RETURN_IF_ERROR(readBorderElement(TopBorder, "top"));
            }
            else if (QUALIFIED_NAME_IS(left)) {
                RETURN_IF_ERROR(readBorderElement(LeftBorder, "left"));
            }
            else if (QUALIFIED_NAME_IS(bottom)) {
                RETURN_IF_ERROR(readBorderElement(BottomBorder, "bottom"));
            }
            else if (QUALIFIED_NAME_IS(right)) {
                RETURN_IF_ERROR(readBorderElement(RightBorder, "right"));
            }
            ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    applyBorders(&m_currentPageStyle);
    READ_EPILOGUE
}

void DocxXmlDocumentReader::applyBorders(KoGenStyle *style)
{
    if (m_borderStyles.count(m_borderStyles.key(TopBorder)) == 4) {
        style->addProperty("fo:border", m_borderStyles.key(TopBorder)); // all sides the same
    }
    else {
        if (!m_borderStyles.key(TopBorder).isEmpty())
            style->addProperty("fo:border-top", m_borderStyles.key(TopBorder));
        if (!m_borderStyles.key(LeftBorder).isEmpty())
            style->addProperty("fo:border-left", m_borderStyles.key(LeftBorder));
        if (!m_borderStyles.key(BottomBorder).isEmpty())
            style->addProperty("fo:border-bottom", m_borderStyles.key(BottomBorder));
        if (!m_borderStyles.key(RightBorder).isEmpty())
            style->addProperty("fo:border-right", m_borderStyles.key(RightBorder));
    }
    m_borderStyles.clear();

    if (m_borderPaddings.count(m_borderPaddings.key(TopBorder)) == 4) {
        style->addProperty("fo:padding", m_borderPaddings.key(TopBorder)); // all sides the same
    }
    else {
        if (!m_borderPaddings.key(TopBorder).isEmpty())
            style->addProperty("fo:padding-top", m_borderPaddings.key(TopBorder));
        if (!m_borderPaddings.key(LeftBorder).isEmpty())
            style->addProperty("fo:padding-left", m_borderPaddings.key(LeftBorder));
        if (!m_borderPaddings.key(BottomBorder).isEmpty())
            style->addProperty("fo:padding-bottom", m_borderPaddings.key(BottomBorder));
        if (!m_borderPaddings.key(RightBorder).isEmpty())
            style->addProperty("fo:padding-right", m_borderPaddings.key(RightBorder));
    }
    m_borderPaddings.clear();
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

KoFilter::ConversionStatus DocxXmlDocumentReader::readBorderElement(
    BorderSide borderSide, const char *borderSideName)
{
    const QXmlStreamAttributes attrs(attributes());
    READ_ATTR(val)
    TRY_READ_ATTR(sz)
    TRY_READ_ATTR(color)
    createBorderStyle(sz, color, val, borderSide);
    TRY_READ_ATTR(space)
    if (!space.isEmpty()) {
        int sp;
        STRING_TO_INT(space, sp, QString("w:%1@space").arg(borderSideName));
        m_borderPaddings.insertMulti(QString::number(sp) + "pt", borderSide);
    }
    readNext();
    return KoFilter::OK;
}

void DocxXmlDocumentReader::createBorderStyle(const QString& size, const QString& color,
    const QString& lineStyle, BorderSide borderSide)
{
    const QString odfLineStyle(ST_Border_to_ODF(lineStyle));
    if (odfLineStyle.isEmpty())
        return;

    QString border;
    if (!size.isEmpty())
        border += MSOOXML::Utils::ST_EighthPointMeasure_to_ODF(size) + " ";

    border.append(odfLineStyle + " ");

    if (!color.isEmpty()) {
        border.append('#');
        border.append(color);
    }
    else {
        border.append(QLatin1String("#000000"));
    }

    m_borderStyles.insertMulti(border, borderSide);
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
    kDebug() << "m_currentObjectWidthCm" << m_currentObjectWidthCm;
    TRY_READ_ATTR(dyaOrig)
    m_currentObjectHeightCm = MSOOXML::Utils::ST_TwipsMeasure_to_cm(dyaOrig);
    //! @todo try to get position from object tag...
    m_currentObjectXCm = "0cm";
    m_currentObjectYCm = "0cm";
    m_objectRectInitialized = false;
    m_imagedataPath.clear();
    m_shapeAltText.clear();

    m_currentDrawStyle = new KoGenStyle(KoGenStyle::GraphicAutoStyle, "graphic");

    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            //! @todo support VML here
            TRY_READ_IF_NS(v, shapetype)
            ELSE_TRY_READ_IF_NS(v, shape)
            ELSE_TRY_READ_IF_NS(o, OLEObject)
            //! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    if (m_objectRectInitialized) {
        m_currentDrawStyle->addProperty("draw:fill", "bitmap");
        if (!m_imagedataPath.isEmpty()) {
            // create bitmap fill-style for styles.xml
            KoGenStyle fillImageStyle(KoGenStyle::FillImageStyle);
            fillImageStyle.addAttribute("xlink:href", m_imagedataPath);
            QString displayName = m_shapeAltText;
            //! @todo use m_shapeTitle for mouse-over text
            if (displayName.isEmpty()) {
                displayName = m_shapeTitle;
            }
            if (!displayName.isEmpty()) {
                fillImageStyle.addAttribute("draw:display-name", displayName);
            }
            fillImageStyle.addAttribute("xlink:type", "simple");
            fillImageStyle.addAttribute("xlink:show", "embed");
            fillImageStyle.addAttribute("xlink:actuate", "onLoad");
            const QString fillImageStyleName(mainStyles->insert(fillImageStyle, "FillImage"));
            m_currentDrawStyle->addProperty("draw:fill-image-name", fillImageStyleName);
        }
        writeRect();
    }

    m_currentDrawStyle = new KoGenStyle();
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
 - smartTag (§17.5.1.9)
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

    /*
    # example endnote from odt document converted with OpenOffice
    <text:note text:id="ftn1" text:note-class="endnote">
    <text:note-citation>1</text:note-citation>
    <text:note-body>
    <text:p text:style-name="P2">
    <text:span text:style-name="endnote_20_reference" />studies</text:p>
    <text:p text:style-name="endnote" />
    </text:note-body>
    </text:note>
    */

    body->startElement("text:note");
    body->addAttribute("text:id", QString("endn").append(id));
    body->addAttribute("text:note-class", "endnote");

    body->startElement("text:note-citation");

    // Note, this line is meaningless in the sense that office programs are supposed to autogenerate
    // the value based on the footnote style, it is hardcoded for the moment as koffice has no support
    // for it
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

    /*
    # example endnote from odt document converted with OpenOffice
    <text:note text:id="ftn1" text:note-class="footnote">
    <text:note-citation>1</text:note-citation>
    <text:note-body>
    <text:p text:style-name="P2">
    <text:span text:style-name="footnote_20_reference" />studies</text:p>
    <text:p text:style-name="Footnote" />
    </text:note-body>
    </text:note>
    */

    body->startElement("text:note");
    body->addAttribute("text:id", QString("ftn").append(id));
    body->addAttribute("text:note-class", "footnote");

    body->startElement("text:note-citation");

    // Note, this line is meaningless in the sense that office programs are supposed to autogenerate
    // the value based on the footnote style, it is hardcoded for the moment as koffice has no support
    // for it
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
 - r (§17.3.2.25)
 - r (§22.1.2.87)

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
       }
    }

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
    }

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
 - r (§17.3.2.25)
 - r (§22.1.2.87)
*/
//! @todo support all attributes etc.
KoFilter::ConversionStatus DocxXmlDocumentReader::read_instrText()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    while (!atEnd()) {
        readNext();
        if (m_complexCharStatus == InstrAllowed) {
            QString instruction = text().toString().trimmed();

            if (instruction.startsWith("HYPERLINK")) {
                // Removes hyperlink, spaces and extra " chars
                instruction.remove(0, 11);
                instruction.truncate(instruction.size() - 1);
                m_complexCharType = HyperlinkComplexFieldCharType;
                m_complexCharValue = instruction;
            }
            else if (instruction.startsWith("PAGEREF")) {
                instruction.remove(0, 8); // removes PAGEREF
                m_complexCharType = ReferenceComplexFieldCharType;
                m_complexCharValue = instruction.left(instruction.indexOf(' '));
            }
            //! @todo: Add rest of the instructions
        }
        BREAK_IF_END_OF(CURRENT_EL);
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
 - fldSimple (§17.16.19)
 - [done] hyperlink (§17.16.22)
 - [done] p (§17.3.1.22)
 - sdtContent (§17.5.2.36)
 - smartTag (§17.5.1.9)

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
 - del (Deleted Run Content) §17.13.5.14
 - dir (Bidirectional Embedding Level) §17.3.2.8
 - fldSimple (Simple Field) §17.16.19
 - [done] hyperlink (Hyperlink) §17.16.22
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
 - [done] r (Text Run) §17.3.2.25
 - sdt (Inline-Level Structured Document Tag) §17.5.2.31
 - smartTag (Inline-Level Smart Tag) §17.5.1.9
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
        link_target = m_context->relationships->linkTarget(r_id, m_context->path, m_context->file);
    }
    kDebug() << "link_target:" << link_target;

    if (link_target.isEmpty()) {
        TRY_READ_ATTR(anchor)
        if (!anchor.isEmpty())
        {
            body->startElement("text:bookmark-ref");
            body->addAttribute("text:reference-format", "page");
            body->addAttribute("text:ref-name", anchor);
        }
    }
    else {
        body->startElement("text:a");
        body->addAttribute("xlink:type", "simple");
        body->addAttribute("xlink:href", QUrl(link_target).toEncoded());
    }

    m_closeHyperlink = true;

    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            TRY_READ_IF(r)
            ELSE_TRY_READ_IF(hyperlink)
            ELSE_TRY_READ_IF(bookmarkStart)
            ELSE_TRY_READ_IF(bookmarkEnd)
            //! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL)
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
 - comment (§17.13.4.2)
 - customXml (§17.5.1.6)
 - docPartBody (§17.12.6)
 - endnote (§17.11.2)
 - footnote (§17.11.10)
 - ftr (§17.10.3)
 - hdr (§17.10.4)
 - sdtContent (§17.5.2.34)
 - tc (§17.4.66)
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
 - del (Deleted Run Content) §17.13.5.14
 - dir (Bidirectional Embedding Level) §17.3.2.8
 - [done] fldSimple (Simple Field) §17.16.19
 - [done] hyperlink (Hyperlink) §17.16.22 - WML only
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
 - [done] pPr (Paragraph Properties) §17.3.1.26
 - proofErr (Proofing Error Anchor) §17.13.8.1
 - [done] r (Text Run) §17.3.2.25
 - sdt (Inline-Level Structured Document Tag) §17.5.2.31
 - smartTag (Inline-Level Smart Tag) §17.5.1.9
 - subDoc (Anchor for Subdocument Location) §17.17.1.1
*/
//! @todo support all elements
KoFilter::ConversionStatus DocxXmlDocumentReader::read_p()
{
    READ_PROLOGUE
    const read_p_args args = m_read_p_args;
    m_read_p_args = 0;
    m_paragraphStyleNameWritten = false;
    m_currentStyleName.clear();
    m_currentListStyleName.clear();
    m_listFound = false;
    m_closeHyperlink = false;

    MSOOXML::Utils::XmlWriteBuffer textPBuf;

    bool oldWasCaption = m_wasCaption;
    m_wasCaption = false;
    if (oldWasCaption || (args & read_p_Skip)) {
        kDebug() << "SKIP!";
    } else {
        body = textPBuf.setWriter(body);
        m_currentParagraphStyle = KoGenStyle(KoGenStyle::ParagraphAutoStyle, "paragraph");

#if 0
        // MS2007 has a different way of marking drop cap, it divides them to two paragraphs
        // here we apply the status to current paragraph if previous one had dropCap
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
#endif
    }

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            if (QUALIFIED_NAME_IS(p)) {
                // CASE #301: avoid nested paragaraphs
                kDebug() << "Nested" << qualifiedName() << "detected: skipping the inner element";
                TRY_READ_WITH_ARGS(p, read_p_Skip;)
            }
            //ELSE_TRY_READ_IF(commentRangeEnd)
            ELSE_TRY_READ_IF(hyperlink)
            ELSE_TRY_READ_IF(commentRangeStart)
            ELSE_TRY_READ_IF(bookmarkStart)
            ELSE_TRY_READ_IF(bookmarkEnd)
            ELSE_TRY_READ_IF(pPr) // CASE #400.1
//! @todo add more conditions testing the parent
            ELSE_TRY_READ_IF(r) // CASE #400.2
            ELSE_TRY_READ_IF(fldSimple)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    if (oldWasCaption || (args & read_p_Skip)) {
        //nothing
    } else {
        if (m_dropCapStatus == DropCapRead) {
            body = textPBuf.releaseWriter();
            // If we read a drop cap but there was no text
            // there might as well be no drop cap
            m_dropCapStatus = NoDropCap;
        }
        else {
            body = textPBuf.originalWriter();
            if (m_listFound) {
                body->startElement("text:list");
                if (!m_currentListStyleName.isEmpty()) {
                    body->addAttribute("text:style-name", m_currentListStyleName);
                }
                body->startElement("text:list-item");
                for (int i = 0; i < m_currentListLevel; ++i) {
                    body->startElement("text:list");
                    body->startElement("text:list-item");
                }
            }
            body->startElement("text:p", false);
            if (m_currentStyleName.isEmpty()) {
                setupParagraphStyle();
            }
            else {
                body->addAttribute("text:style-name", m_currentStyleName);
                if (m_currentStyleName=="Caption")
                    m_wasCaption = true;
            }
            /*        if (!m_paragraphStyleNameWritten) {
                    // no style, set default
                    body->addAttribute("text:style-name", "Standard");
                }*/

            (void)textPBuf.releaseWriter();
            body->endElement(); //text:p
            if (m_listFound) {
                for (int i = 0; i < m_currentListLevel; ++i) {
                    body->endElement(); // text:list-item
                    body->endElement(); // text:list
                }
                body->endElement(); //text:list-item
                body->endElement(); //text:list
            }
            kDebug() << "/text:p";
        }
    }
    m_currentStyleName.clear();
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
 - fldSimple (§17.16.19)
 - [done] hyperlink (§17.16.22)
 - ins (§17.13.5.18)
 - moveFrom (§17.13.5.22)
 - moveTo (§17.13.5.25)
 - [done] p (§17.3.1.22)
 - rt (§17.3.3.24)
 - rubyBase (§17.3.3.27)
 - sdtContent (§17.5.2.36)
 - smartTag (§17.5.1.9)

 Child elements:
 - annotationRef (Comment Information Block) §17.13.4.1
 - br (Break) §17.3.3.1
 - commentReference (Comment Content Reference Mark) §17.13.4.5
 - contentPart (Content Part) §17.3.3.2
 - continuationSeparator (Continuation Separator Mark) §17.11.1
 - cr (Carriage Return) §17.3.3.4
 - dayLong (Date Block - Long Day Format) §17.3.3.5
 - dayShort (Date Block - Short Day Format) §17.3.3.6
 - delInstrText (Deleted Field Code) §17.16.13
 - delText (Deleted Text) §17.3.3.7
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

    m_currentRunStyleName.clear();
    m_currentTextStyle = KoGenStyle(KoGenStyle::TextAutoStyle, "text");

    MSOOXML::Utils::XmlWriteBuffer buffer;
    body = buffer.setWriter(body);

    KoXmlWriter* oldWriter = body;

    // DropCapRead means we have read the w:dropCap attribute on this
    //             paragraph and now have to save the text.
    // DropCapDone means we are in the next paragraph and want to add
    //             the saved text to this paragraph.
    if (m_dropCapStatus == DropCapRead) {
       m_dropCapStatus = DropCapDone;
       m_dropCapBuffer.open(QIODevice::ReadWrite);
       m_dropCapWriter = new KoXmlWriter(&m_dropCapBuffer);
       body = m_dropCapWriter;
    }
    else if (m_dropCapStatus == DropCapDone) {
        body->addCompleteElement(&m_dropCapBuffer);
        delete m_dropCapWriter;
        m_dropCapWriter = 0;
        m_dropCapStatus = NoDropCap;
    }

    while (!atEnd()) {
//kDebug() <<"[0]";
        readNext();
        kDebug() << *this;
//kDebug() <<"[1]";
        if (isStartElement()) {
            TRY_READ_IF_IN_CONTEXT(rPr)
            ELSE_TRY_READ_IF(t)
            ELSE_TRY_READ_IF(ptab)
            ELSE_TRY_READ_IF(drawing)
            ELSE_TRY_READ_IF(endnoteReference)
            ELSE_TRY_READ_IF(footnoteReference)
            ELSE_TRY_READ_IF(object)
            ELSE_TRY_READ_IF(pict)
            ELSE_TRY_READ_IF(instrText)
            ELSE_TRY_READ_IF(fldChar)
            ELSE_TRY_READ_IF(lastRenderedPageBreak)
            else  if (qualifiedName() == "w:tab") {
                body->startElement("text:tab");
                body->endElement(); // text:tab
            }
//            else { SKIP_EVERYTHING }
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    if (m_dropCapStatus == DropCapDone) {
        body = oldWriter;
    }
    else if (!m_currentTextStyle.isEmpty() || !m_currentRunStyleName.isEmpty() || 
             m_complexCharType != NoComplexFieldCharType || !m_currentTextStyle.parentName().isEmpty()) {
        // We want to write to the higher body level
        body = buffer.originalWriter();
        QString currentTextStyleName;
        if (!m_currentRunStyleName.isEmpty()) {
            currentTextStyleName = m_currentRunStyleName;
        }
        else {
            currentTextStyleName = mainStyles->insert(m_currentTextStyle);
            if (m_moveToStylesXml) {
                mainStyles->markStyleForStylesXml(currentTextStyleName);
            }
        }
        if (m_complexCharStatus == InstrExecute) {
            if (m_complexCharType == HyperlinkComplexFieldCharType) {
                body->startElement("text:a");
                body->addAttribute("xlink:type", "simple");
                body->addAttribute("xlink:href", QUrl(m_complexCharValue).toEncoded());
            }
            else if (m_complexCharType == ReferenceNextComplexFieldCharType) {
                body->startElement("text:bookmark-ref");
                body->addAttribute("text:reference-format", "page");
                body->addAttribute("text:ref-name", m_complexCharValue);
            }
        }
        body->startElement("text:span", false);
        body->addAttribute("text:style-name", currentTextStyleName);

        // Writing the internal body of read_t now
        body = buffer.releaseWriter();

        body->endElement(); //text:span

        if (m_complexCharStatus == InstrExecute) {
             if (m_complexCharType == ReferenceNextComplexFieldCharType) {
                 body->endElement(); //text:bookmar-ref
                 m_complexCharType = NoComplexFieldCharType;
             }
             else if (m_complexCharType == ReferenceComplexFieldCharType) {
                 m_complexCharType = ReferenceNextComplexFieldCharType;
             }
             else if (m_complexCharType == HyperlinkComplexFieldCharType) {
                 body->endElement(); // text:a
             }
        }
        if (m_closeHyperlink) {
            body->endElement(); //either text:bookmark-ref or text:a
            m_closeHyperlink = false;
        }
    }
    else {
        // Writing the internal body of read_t now 
        body = buffer.releaseWriter();
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
 - bdr (Text Border) §17.3.2.4
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
 - shd (Run Shading) §17.3.2.32
 - [done] smallCaps (Small Caps) §17.3.2.33
 - snapToGrid (Use Document Grid Settings For Inter-Character Spacing) §17.3.2.34
 - spacing (Character Spacing Adjustment) §17.3.2.35
 - specVanish (Paragraph Mark Is Always Hidden) §17.3.2.36
 - [done] strike (Single Strikethrough) §17.3.2.37
 - [done] sz (Non-Complex Script Font Size) §17.3.2.38
 - szCs (Complex Script Font Size) §17.3.2.39
 - [done] u (Underline) §17.3.2.40
 - vanish (Hidden Text) §17.3.2.41
 - [done] vertAlign (Subscript/Superscript Text) §17.3.2.42
 - w (Expanded/Compressed Text) §17.3.2.43
 - [done] webHidden (Web Hidden Text) §17.3.2.44
*/
//! @todo support all elements
KoFilter::ConversionStatus DocxXmlDocumentReader::read_rPr(rPrCaller caller)
{
    Q_UNUSED(caller);
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    Q_ASSERT(m_currentTextStyleProperties == 0);
    m_currentTextStyleProperties = new KoCharacterStyle();

    if (!m_currentTextStylePredefined) {
        m_currentTextStyle = KoGenStyle(KoGenStyle::TextAutoStyle, "text");
    }

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
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
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    m_currentTextStyleProperties->saveOdf(m_currentTextStyle);

    delete m_currentTextStyleProperties;
    m_currentTextStyleProperties = 0;

    READ_EPILOGUE
}

//! CASE #410
void DocxXmlDocumentReader::setParentParagraphStyleName(const QXmlStreamAttributes& attrs)
{
    TRY_READ_ATTR(pStyle)
    if (pStyle.isEmpty()) {
//! CASE #412
//! @todo
    } else {
//! CASE #411
        if (isDefaultTocStyle(pStyle)) {
            pStyle = QLatin1String("Contents") + pStyle.mid(3);
        }
    }

    if (pStyle.isEmpty())
        return;
    kDebug() << "parent paragraph style name set to:" << pStyle;
    m_currentParagraphStyle.setParentName(pStyle);
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
 - outlineLvl (Associated Outline Level) §17.3.1.20
 - overflowPunct (Allow Punctuation to Extend Past Text Extents) §17.3.1.21
 - pageBreakBefore (Start Paragraph on Next Page) §17.3.1.23
 - [done] pBdr (Paragraph Borders) §17.3.1.24
 - pPrChange (Revision Information for Paragraph Properties) §17.13.5.29
 - [done] pStyle (Referenced Paragraph Style) §17.3.1.27
 - [done] rPr (Run Properties for the Paragraph Mark) §17.3.1.29
 - sectPr (Section Properties) §17.6.18
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
    const QXmlStreamAttributes attrs(attributes());
    setParentParagraphStyleName(attrs);

    TRY_READ_ATTR_WITHOUT_NS(lvl)
    m_pPr_lvl = lvl.toUInt(); // 0 (the default) on failure, so ok.

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF_IN_CONTEXT(rPr)
            ELSE_TRY_READ_IF_IN_CONTEXT(shd)
            ELSE_TRY_READ_IF(jc)
            ELSE_TRY_READ_IF(tabs)
            ELSE_TRY_READ_IF(spacing)
            ELSE_TRY_READ_IF(pStyle)
            ELSE_TRY_READ_IF(numPr)
            ELSE_TRY_READ_IF(pBdr)
            ELSE_TRY_READ_IF(framePr)
            ELSE_TRY_READ_IF(ind)
            ELSE_TRY_READ_IF(suppressLineNumbers)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    if (m_listFound) {
        m_currentParagraphStyle.addProperty("style:list-style-name", m_currentListStyleName);
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
 - endnote (§17.11.2)
 - fldSimple (§17.16.19)
 - fName (§22.1.2.37)
 - footnote (§17.11.10)
 - ftr (§17.10.3)
 - hdr (§17.10.4)
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
 - smartTag (§17.5.1.9)
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
        body->startElement("text:bookmark-start");
        body->addAttribute("text:name", name);
        body->endElement(); // text:bookmark-start
        m_bookmarks[id] = name;
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
 - endnote (§17.11.2)
 - fldSimple (§17.16.19)
 - fName (§22.1.2.37)
 - footnote (§17.11.10)
 - ftr (§17.10.3)
 - hdr (§17.10.4)
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
 - smartTag (§17.5.1.9)
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
        body->startElement("text:bookmark-end");
        body->addAttribute("text:name", m_bookmarks[id]);
        body->endElement(); // text:bookmark-end
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL numPr
KoFilter::ConversionStatus DocxXmlDocumentReader::read_numPr()
{
    READ_PROLOGUE

    m_listFound = true;
    m_currentListLevel = 0;

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(numId)
            ELSE_TRY_READ_IF(ilvl)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL ilvl
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
KoFilter::ConversionStatus DocxXmlDocumentReader::read_numId()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR(val)
    // In docx, this value defines a predetermined style from numbering xml,
    // The styles from numbering have to be given some name, NumStyle has been chosen here
    if (!val.isEmpty()) {
        m_currentListStyleName = QString("NumStyle%1").arg(val);
    }

    readNext();
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

    m_currentDrawStyle = new KoGenStyle(KoGenStyle::GraphicAutoStyle, "graphic");
    m_currentDrawStyle->addAttribute("style:parent-style-name", QLatin1String("Graphics"));

    m_drawing_anchor = false;
    m_drawing_inline = false;
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            TRY_READ_IF_NS(wp, anchor)
            ELSE_TRY_READ_IF_NS(wp, inline)
            ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    m_drawing_anchor = false;
    m_drawing_inline = false;
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
 - [done] pPr (§17.3.1.25)
 - [done] pPr (§17.7.5.2)
 - [done] pPr (§17.7.6.1)
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
#define CURRENT_EL b
//! b handler
//! CASE #1112
KoFilter::ConversionStatus DocxXmlDocumentReader::read_b()
{
    READ_PROLOGUE
    m_currentTextStyleProperties->setFontWeight(READ_BOOLEAN_VAL ? QFont::Bold : QFont::Normal);
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
    m_currentTextStyleProperties->setFontItalic(READ_BOOLEAN_VAL);
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
 - rPr (§17.7.9.1)
 - rPr (§17.7.5.4) (within style)
 - [done] rPr (§17.3.2.28)
 - rPr (§17.5.2.27)
 - rPr (§17.7.6.2)
 - rPr (§17.3.2.27)
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
        kDebug() << "pointSize:" << pointSize;
        m_currentTextStyleProperties->setFontPointSize(pointSize);
    }
    SKIP_EVERYTHING
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL jc
//! Paragraph Alignment
/*! ECMA-376, 17.3.1.13, p.239
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_jc()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    READ_ATTR(val)
    // Does ODF support high/low/medium kashida ?
    val = val.toLower();
    if ((val == "both") || (val == "distribute"))
        m_currentParagraphStyle.addProperty("fo:text-align", "justify");
    else if ((val == "start") || (val == "left") || (val == "right") || (val == "center"))
        m_currentParagraphStyle.addProperty("fo:text-align", val);
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
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR(after)
    bool ok;
    const int marginBottom = (TWIP_TO_POINT(after.toDouble(&ok)));

    if (ok) {
        m_currentParagraphStyle.addPropertyPt("fo:margin-bottom", marginBottom);
    }

    TRY_READ_ATTR(before)
    const int marginTop = (TWIP_TO_POINT(before.toDouble(&ok)));

    if (ok) {
        m_currentParagraphStyle.addPropertyPt("fo:margin-top", marginTop);
    }

    // for rPr
    TRY_READ_ATTR(val)

    const int pointSize = (TWIP_TO_POINT(val.toInt(&ok)));

    if (ok) {
        m_currentTextStyleProperties->setFontLetterSpacing(pointSize);
    }

    TRY_READ_ATTR(lineRule)
    TRY_READ_ATTR(line)
    qreal lineSpace = line.toDouble(&ok);

    if (ok) {
        if (lineRule == "atLeast" || lineRule == "exact") {
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

    SKIP_EVERYTHING
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
//kDebug() << m_callsNames;
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
            m_currentTableCellStyle.addProperty("fo:background-color", fillColor);
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
            //! @todo
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

 This element specifies the style ID of the paragraph style which shall be used to format the contents
 of this paragraph. This formatting is applied at the following location in the style hierarchy:
 - Document defaults
 - Table styles
 - Numbering styles
 - Paragraph styles (this element)
 - Character styles
 - Direct Formatting

 This means that all properties specified in the style element (§17.7.4.17) with a styleId which
 corresponds to the value in this element's val attribute are applied to the paragraph at the appropriate
 level in the hierarchy.

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
KoFilter::ConversionStatus DocxXmlDocumentReader::read_pStyle()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR(val)

    m_currentParagraphStyle.setParentName(val);
    SKIP_EVERYTHING
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

 @todo support all elements
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_rStyle()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR(val)

    m_currentTextStyle.setParentName(val);

    SKIP_EVERYTHING
    READ_EPILOGUE
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
 - sdtContent (§17.5.2.36)
 - smartTag (§17.5.1.9)

 Child elements:

 - bdo (Bidirectional Override) §17.3.2.3
 - bookmarkEnd (Bookmark End)   §17.13.6.1
 - bookmarkStart (Bookmark Start)                                                §17.13.6.2
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
 - del (Deleted Run Content)                                                     §17.13.5.14
 - dir (Bidirectional Embedding Level)                                           §17.3.2.8
 - [done] fldSimple (Simple Field)                                               §17.16.19
 - [done] hyperlink (Hyperlink)                                                  §17.16.22
 - ins (Inserted Run Content)                                                    §17.13.5.18
 - moveFrom (Move Source Run Content)                                            §17.13.5.22
 - moveFromRangeEnd (Move Source Location Container - End)                       §17.13.5.23
 - moveFromRangeStart (Move Source Location Container - Start)                   §17.13.5.24
 - moveTo (Move Destination Run Content)                                         §17.13.5.25
 - moveToRangeEnd (Move Destination Location Container - End)                    §17.13.5.27
 - moveToRangeStart (Move Destination Location Container - Start)                §17.13.5.28
 - oMath (Office Math)                                                           §22.1.2.77
 - oMathPara (Office Math Paragraph)                                             §22.1.2.78
 - permEnd (Range Permission End)                                                §17.13.7.1
 - permStart (Range Permission Start)                                            §17.13.7.2
 - proofErr (Proofing Error Anchor)                                              §17.13.8.1
 - [done] r (Text Run)                                                           §17.3.2.25
 - sdt (Inline-Level Structured Document Tag)                                    §17.5.2.31
 - smartTag (Inline-Level Smart Tag)                                             §17.5.1.9
 - subDoc (Anchor for Subdocument Location)                                      §17.17.1.1

 @todo support all elements
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_fldSimple()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR(instr)
    instr = instr.trimmed();

    if (instr.startsWith("PAGE ")) {
        body->startElement("text:page-number");
        body->addAttribute("text:select-page", "current");
    }

// @todo support all attributes

    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            TRY_READ_IF(fldSimple)
            ELSE_TRY_READ_IF(r)
            ELSE_TRY_READ_IF(hyperlink)
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    if (instr.startsWith("PAGE ")) {
        body->endElement(); // text:page-number
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL tabs
//! tabs handler (Set of Custom Tab Stops)
/*!

 Parent elements:
 - pPr (§17.3.1.26)
 - pPr (§17.3.1.25)
 - pPr (§17.7.5.2)
 - pPr (§17.7.6.1)
 - pPr (§17.9.23)
 - pPr (§17.7.8.2)

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
        if (isStartElement()) {
            TRY_READ_IF(tab)
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    elementWriter.addCompleteElement(&buffer);

    delete body;
    body = oldBody;
 
    elementWriter.endElement(); // style-tab-stops

    QString tabStops = QString::fromUtf8(tabs.buffer(), tabs.buffer().size());
    kDebug() << tabStops;
    m_currentParagraphStyle.addChildElement("style:tab-stops", tabStops);

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL tab
//! tab handler (Custom Tab Stop)
/*!

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

    body->startElement("style:tab-stop");
    body->addAttribute("style:type", val);
    bool ok = false;
    const qreal value = qreal(TWIP_TO_POINT(pos.toDouble(&ok)));
    if (ok) {
        body->addAttributePt("style:position", value);
    }
    if (!leader.isEmpty()) {
        if (leader == "dot") {
            body->addAttribute("style:leader-text", ".");
        }
    }
    body->endElement(); // style:tab-stop

//! @todo: support leader attribute

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL ptab
//! ptab handler (absolute tab)
/*!

 Parent elements:
 - r (§22.1.2.87);
 - r (§17.3.2.25)

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
        if (isStartElement()) {
            if (QUALIFIED_NAME_IS(top)) {
                RETURN_IF_ERROR(readBorderElement(TopBorder, "top"));
            }
            else if (QUALIFIED_NAME_IS(left)) {
                RETURN_IF_ERROR(readBorderElement(LeftBorder, "left"));
            } 
            else if (QUALIFIED_NAME_IS(bottom)) {
                RETURN_IF_ERROR(readBorderElement(BottomBorder, "bottom"));
            }
            else if (QUALIFIED_NAME_IS(right)) {
                RETURN_IF_ERROR(readBorderElement(RightBorder, "right"));
            }
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    applyBorders(&m_currentParagraphStyle);
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
    if (READ_BOOLEAN_VAL)
        m_currentTextStyleProperties->setFontCapitalization(QFont::AllUppercase);
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
    if (READ_BOOLEAN_VAL)
        m_currentTextStyleProperties->setFontCapitalization(QFont::SmallCaps);
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
//! @todo set use-window-font-color="true" (currently no way to do this using KoCharacterStyle)
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
    if (val == "superscript")
        m_currentTextStyleProperties->setVerticalAlignment(QTextCharFormat::AlignSuperScript);
    else if (val == "subscript")
        m_currentTextStyleProperties->setVerticalAlignment(QTextCharFormat::AlignSubScript);
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
            kWarning() << "invalid value of \"bidi\" attribute:" << bidi << " - skipping";
        }
    }
    TRY_READ_ATTR(val)
    if (MSOOXML::Utils::ST_Lang_to_languageAndCountry(val, language, country)) {
        m_currentTextStyle.addProperty("fo:language", language, KoGenStyle::TextType);
        m_currentTextStyle.addProperty("fo:country", country, KoGenStyle::TextType);
    } else {
        kWarning() << "invalid value of \"val\" attribute:" << val << " - skipping";
    }

    TRY_READ_ATTR(eastAsia)
    if (!eastAsia.isEmpty()) {
        if (MSOOXML::Utils::ST_Lang_to_languageAndCountry(eastAsia, language, country)) {
            m_currentTextStyle.addProperty("style:language-asian", language, KoGenStyle::TextType);
            m_currentTextStyle.addProperty("style:country-asian", country, KoGenStyle::TextType);
        } else {
            kWarning() << "invalid value of \"eastAsia\" attribute:" << eastAsia << " - skipping";
        }
    }
    kDebug() << "bidi:" << bidi << "val:" << val << "eastAsia:" << eastAsia;

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
 - drawing (§17.3.3.9)
 Attributes:
 - [done] color (Background Color)
 - themeColor (Background Theme Color)
 - themeTint (Border Theme Color Tint)
 - themeShade (Border Theme Color Shade)
*/

KoFilter::ConversionStatus DocxXmlDocumentReader::read_background()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR(color)

    QColor tmpColor(MSOOXML::Utils::ST_HexColorRGB_to_QColor(color));
    if (tmpColor.isValid())
        m_backgroundColor = tmpColor;

    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            if (qualifiedName() == "v:background") {
                TRY_READ(VML_background)
            }
            ELSE_TRY_READ_IF(drawing)
        }
        BREAK_IF_END_OF(CURRENT_EL);
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
 - endnote (§17.11.2);footnote (§17.11.10)
 - ftr (§17.10.3)
 - hdr (§17.10.4)
 - sdtContent (§17.5.2.34)
 - [done] tc (§17.4.66)

 Child elements:
 - bookmarkEnd (Bookmark End) §17.13.6.1
 - bookmarkStart (Bookmark Start) §17.13.6.2
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
    READ_PROLOGUE
    MSOOXML::Utils::XmlWriteBuffer tableBuf;
    body = tableBuf.setWriter(body);

    //const QXmlStreamAttributes attrs(attributes());
    d->clearColumnStyles();
    m_currentTableName = QLatin1String("Table") + QString::number(m_currentTableNumber + 1);
    m_currentTableStyle = KoGenStyle(KoGenStyle::TableAutoStyle, "table");
    m_currentTableWidth = 0.0;
    m_currentTableRowNumber = 0;

    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            TRY_READ_IF(tblPr)
            ELSE_TRY_READ_IF(tblGrid)
            ELSE_TRY_READ_IF(tr)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    body = tableBuf.originalWriter();
    body->startElement("table:table");
    body->addAttribute("table:name", m_currentTableName);
    m_currentTableStyle.addProperty(
        "style:width", QString::number(m_currentTableWidth) + QLatin1String("cm"),
        KoGenStyle::TableType);
    //! @todo fix hardcoded table:align
    m_currentTableStyle.addProperty("table:align", "left");

    //! @todo fix hardcoded style:master-page-name
    m_currentTableStyle.addAttribute("style:master-page-name", "Standard");
    const QString tableStyleName(
        mainStyles->insert(
            m_currentTableStyle,
            m_currentTableName,
            KoGenStyles::DontAddNumberToName)
    );
    body->addAttribute("table:style-name", tableStyleName);
    uint column = 0;
    foreach (const ColumnStyleInfo& columnStyle, d->columnStyles) {
        body->startElement("table:table-column");
        const QString columnStyleName(
            mainStyles->insert(
                *columnStyle.style,
                m_currentTableName + '.' + MSOOXML::Utils::columnName(column),
                KoGenStyles::DontAddNumberToName)
        );
        body->addAttribute("table:style-name", columnStyleName);
        if (columnStyle.count > 1) {
            body->addAttribute("table:number-columns-repeated", columnStyle.count);
        }
        body->endElement(); // table:table-column
        column += columnStyle.count;
    }
    d->clearColumnStyles();

    (void)tableBuf.releaseWriter();
    body->endElement(); // table:table

    m_currentTableNumber++;

    READ_EPILOGUE
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
 - jc (Table Alignment) §17.4.29
 - shd (Table Shading) §17.4.32
 - tblBorders (Table Borders) §17.4.39
 - tblCaption (Table Caption) §17.4.41
 - tblCellMar (Table Cell Margin Defaults) §17.4.43
 - tblCellSpacing (Table Cell Spacing Default) §17.4.46
 - tblDescription (Table Description) §17.4.47
 - tblInd (Table Indent from Leading Margin) §17.4.51
 - tblLayout (Table Layout) §17.4.53
 - tblLook (Table Style Conditional Formatting Settings) §17.4.56
 - tblOverlap (Floating Table Allows Other Tables to Overlap) §17.4.57
 - tblpPr (Floating Table Positioning) §17.4.58
 - tblPrChange (Revision Information for Table Properties) §17.13.5.34
 - tblStyle (Referenced Table Style) §17.4.63
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
        if (isStartElement()) {
//! @todo add tblStyle to get parent table style
//            TRY_READ_IF(..)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
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
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            TRY_READ_IF(gridCol)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
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
    const QString widthCm(MSOOXML::Utils::ST_TwipsMeasure_to_cm(w));
    KoGenStyle *columnStyle = new KoGenStyle(KoGenStyle::TableColumnAutoStyle, "table-column");
    if (!widthCm.isEmpty()) {
        columnStyle->addProperty("style:column-width", widthCm, KoGenStyle::TableColumnType);
        m_currentTableWidth += widthCm.left(widthCm.length()-2).toFloat();
    }
    // only add the style if it different than the previous; else just increate the counter
    if (d->columnStyles.isEmpty() || !(*d->columnStyles.last().style == *columnStyle)) {
        d->columnStyles.append(ColumnStyleInfo(columnStyle));
    }
    d->columnStyles.last().count++;

    while (true) {
        BREAK_IF_END_OF(CURRENT_EL);
        readNext();
    }
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
 - bookmarkEnd (Bookmark End) §17.13.6.1
 - bookmarkStart (Bookmark Start) §17.13.6.2
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
 - sdt (Cell-Level Structured Document Tag) §17.5.2.32
 - tblPrEx (Table-Level Property Exceptions) §17.4.61
 - tc (Table Cell) §17.4.66
 - [done]trPr (Table Row Properties) §17.4.82
*/
//! @todo support all child elements
KoFilter::ConversionStatus DocxXmlDocumentReader::read_tr()
{
    READ_PROLOGUE
    MSOOXML::Utils::XmlWriteBuffer rowBuf;
    body = rowBuf.setWriter(body);
    m_currentTableColumnNumber = 0;
    m_currentTableRowStyle = KoGenStyle(KoGenStyle::TableRowAutoStyle, "table-row");

    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            TRY_READ_IF(tc)
            ELSE_TRY_READ_IF(trPr)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    body = rowBuf.originalWriter();
    body->startElement("table:table-row");

    //! @todo add style:keep-together property
    //! @todo add fo:keep-together
    const QString tableRowStyleName(
        mainStyles->insert(
            m_currentTableRowStyle,
            m_currentTableName + '.' + QString::number(m_currentTableRowNumber + 1),
            KoGenStyles::DontAddNumberToName)
    );
    body->addAttribute("table:style-name", tableRowStyleName);


    (void)rowBuf.releaseWriter();
    body->endElement(); // table:table-row

    m_currentTableRowNumber++;

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL trPr
/*
Parent elements:
 - tr (§17.4.79)

child elements:
 - cantSplit (Table Row Cannot Break Across Pages) §17.4.6
 - cnfStyle (Table Row Conditional Formatting) §17.4.7
 - del (Deleted Table Row)§17.13.5.12
 - divId (Associated HTML div ID)§17.4.9
 - gridAfter (Grid Columns After Last Cell)§17.4.14
 - gridBefore (Grid Columns Before First Cell) §17.4.15
 - hidden (Hidden Table Row Marker)§17.4.20
 - ins (Inserted Table Row) §17.13.5.17
 - jc (Table Row Alignment) §17.4.28
 - tblCellSpacing (Table Row Cell Spacing) §17.4.44
 - tblHeader (Repeat Table Row on Every New Page) §17.4.50
 - [done]trHeight (Table Row Height) §17.4.81
 - trPrChange (Revision Information for Table Row Properties) §17.13.5.37
 - wAfter (Preferred Width After Table Row) §17.4.86 
 - wBefore (Preferred Width Before Table Row) §17.4.87
*/ 
KoFilter::ConversionStatus DocxXmlDocumentReader::read_trPr()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            TRY_READ_IF(trHeight)
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
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
    const QString s(MSOOXML::Utils::TWIP_to_ODF(val));
    if (hRule == QLatin1String("exact")) {
	m_currentTableRowStyle.addProperty("style:row-height",s, KoGenStyle::TableRowType);
    }
    else if (hRule == QLatin1String("atLeast")) {
	    m_currentTableRowStyle.addProperty("style:min-row-height",s, KoGenStyle::TableRowType);
    }
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
 - bookmarkEnd (Bookmark End) §17.13.6.1
 - bookmarkStart (Bookmark Start) §17.13.6.2
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
    MSOOXML::Utils::XmlWriteBuffer cellBuf;
    body = cellBuf.setWriter(body);
    m_currentTableCellStyle = KoGenStyle(KoGenStyle::TableCellAutoStyle, "table-cell");

    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            TRY_READ_IF(p)
            ELSE_TRY_READ_IF(tbl)
            ELSE_TRY_READ_IF(tcPr)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    body = cellBuf.originalWriter();
    body->startElement("table:table-cell");

    //! @todo real border style get from w:tblPr/w:tblStyle@w:val
    m_currentTableCellStyle.addProperty("fo:border", "0.5pt solid #000000");

    const QString tableCellStyleName(
        mainStyles->insert(
            m_currentTableCellStyle,
            m_currentTableName + '.' + MSOOXML::Utils::columnName(m_currentTableColumnNumber)
                + QString::number(m_currentTableRowNumber + 1),
            KoGenStyles::DontAddNumberToName)
    );
    body->addAttribute("table:style-name", tableCellStyleName);
    //! @todo import various cell types
    body->addAttribute("office:value-type", "string");

    (void)cellBuf.releaseWriter();
    body->endElement(); // table:table-cell

    m_currentTableColumnNumber++;

    READ_EPILOGUE
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
 - cnfStyle (Table Cell Conditional Formatting) §17.4.8
 - gridSpan (Grid Columns Spanned by Current Table Cell) §17.4.17
 - headers (Header Cells Associated With Table Cell) §17.4.19
 - hideMark (Ignore End Of Cell Marker In Row Height Calculation) §17.4.21
 - hMerge (Horizontally Merged Cell) §17.4.22
 - noWrap (Don't Wrap Cell Content) §17.4.30
 - [done] shd (Table Cell Shading) §17.4.33
 - tcBorders (Table Cell Borders) §17.4.67
 - tcFitText (Fit Text Within Cell) §17.4.68
 - tcMar (Single Table Cell Margins) §17.4.69
 - tcPrChange (Revision Information for Table Cell Properties) §17.13.5.36
 - tcW (Preferred Table Cell Width) §17.4.72
 - textDirection (Table Cell Text Flow Direction) §17.4.73
 - vAlign (Table Cell Vertical Alignment) §17.4.84
 - vMerge (Vertically Merged Cell) §17.4.85
*/
//! @todo support all child elements
KoFilter::ConversionStatus DocxXmlDocumentReader::read_tcPr()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
//            TRY_READ_IF(..)
              TRY_READ_IF_IN_CONTEXT(shd)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

// ---------------------------------------------------------------------------

#define blipFill_NS "pic"

#include <MsooXmlCommonReaderImpl.h> // this adds w:p, w:pPr, w:t, w:r, etc.

// ---------------------------------------------------------------------------

void DocxXmlDocumentReader::writeRect()
{
    //body->startElement("text:p");
    // //! @todo fix hardcoded text:style-name=Standard?
    //body->addAttribute("text:style-name", "Standard");
    body->startElement("draw:rect");
    if (!m_currentDrawStyle->isEmpty()) {
        const QString drawStyleName( mainStyles->insert(*m_currentDrawStyle, "gr") );
        body->addAttribute("draw:style-name", drawStyleName);
    }

    //! @todo fix hardcoded text:anchor-type=paragraph?
    body->addAttribute("text:anchor-type", "paragraph");
    //! @todo fix hardcoded draw:z-index=0?
    body->addAttribute("draw:z-index", "0");
//! todo    body->addAttribute"draw:style-name", styleName);
/*eg.
   <style:style style:name="gr1" style:family="graphic">
      <style:graphic-properties svg:stroke-color="#000023" draw:fill="bitmap" draw:fill-color="#ffffff" draw:fill-image-name="Bitmape_20_1"
       style:repeat="no-repeat" draw:textarea-horizontal-align="center" draw:textarea-vertical-align="middle" style:run-through="foreground".
       style:wrap="none" style:vertical-pos="from-top" style:vertical-rel="paragraph" style:horizontal-pos="from-left".
       style:horizontal-rel="paragraph" draw:wrap-influence-on-position="once-concurrent" style:flow-with-text="false"/>
    </style:style>*/
    QString x(m_currentObjectXCm);
    if (x.isEmpty()) {
        x = "0cm";
        kWarning() << "No x pos specified! Defaulting to" << x;
    }
    QString y(m_currentObjectYCm);
    if (y.isEmpty()) {
        y = "0cm";
        kWarning() << "No y pos specified! Defaulting to" << y;
    }
    QString width(m_currentObjectWidthCm);
    if (width.isEmpty()) {
        width = "2cm";
        kWarning() << "No width specified! Defaulting to" << width;
    }
    QString height(m_currentObjectHeightCm);
    if (height.isEmpty()) {
        height = "2cm";
        kWarning() << "No height specified! Defaulting to" << height;
    }
    body->addAttribute("svg:x", x);
    body->addAttribute("svg:y", y);
    body->addAttribute("svg:width", width);
    body->addAttribute("svg:height", height);
    body->endElement(); //draw:rect
    //body->endElement(); //text:p
}

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

    TRY_READ_ATTR_WITH_NS(r, id)
    const QString oleName(m_context->relationships->target(m_context->path, m_context->file, r_id));
    kDebug() << "oleName:" << oleName;

    QString destinationName;
//! @todo ooo saves binaries to the root dir; should we?
    RETURN_IF_ERROR( copyFile(oleName, QString(), destinationName) )

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
    }

    m_objectRectInitialized = true;

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
 - allowOverlap (Allow Objects to Overlap)
 - [done] behindDoc (Display Behind Document Text)
 - [done] distB (Distance From Text on Bottom Edge) (see also: inline)
 - [done] distL (Distance From Text on Left Edge) (see also: inline)
 - [done] distR (Distance From Text on Right Edge) (see also: inline)
 - [done] distT (Distance From Text on Top Edge) (see also: inline)
 - hidden (Hidden)
 - layoutInCell (Layout In Table Cell)
 - locked (Lock Anchor)
 - relativeHeight (Relative Z-Ordering Position)
 - simplePos (Page Positioning)
*/
//! @todo support all elements
//! CASE #1340
//! CASE #1410
//! CASE #1420
KoFilter::ConversionStatus DocxXmlDocumentReader::read_anchor()
{
    READ_PROLOGUE
    m_hasPosOffsetH = false;
    m_hasPosOffsetV = false;
    m_docPrName.clear();
    m_docPrDescr.clear();
    m_drawing_anchor = true; // for pic:pic

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

    const bool behindDoc = MSOOXML::Utils::convertBooleanAttr(attrs.value("behindDoc").toString());

    while (!atEnd()) {
        readNext();
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
                if (!expectElEnd(QUALIFIED_NAME(wrapNone)))
                    return KoFilter::WrongFormat;
                saveStyleWrap("run-through");
                m_currentDrawStyle->addProperty(QLatin1String("style:run-through"),
                                                (behindDoc || m_insideHdr || m_insideFtr) ? "background" : "foreground",
                                               KoGenStyle::GraphicType);
            } else if (QUALIFIED_NAME_IS(wrapTopAndBottom)) {
                // 20.4.2.20 wrapTopAndBottom (Top and Bottom Wrapping)
                // This element specifies that text shall wrap around the top
                // and bottom of this object, but not its left or right edges.
                // CASE #1410
                readNext();
                if (!expectElEnd(QUALIFIED_NAME(wrapTopAndBottom)))
                    return KoFilter::WrongFormat;
                saveStyleWrap("none");
            }
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    m_hasPosOffsetH = false;
    m_hasPosOffsetV = false;
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
        if (isStartElement()) {
            TRY_READ_IF_NS(a, graphic)
            ELSE_TRY_READ_IF_NS(wp, extent)
            ELSE_TRY_READ_IF(docPr)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
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
        BREAK_IF_END_OF(CURRENT_EL);
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
 - [done]anchor (§20.4.2.3)
 - inline (§20.4.2.8)

 Child elements:
 - extLst (Extension List) §20.1.2.2.15
 - hlinkClick (Click Hyperlink) §21.1.2.3.5
 - hlinkHover (Hyperlink for Hover) §20.1.2.2.23

 Attributes:
 - descr (Alternative Text for Object)
 - hidden (Hidden)
 - id (Unique Identifier)
 - name (Name)
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
        if (isStartElement()) {
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
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
        if (isStartElement()) {
            TRY_READ_IF_IN_CONTEXT(align)
            ELSE_TRY_READ_IF_IN_CONTEXT(posOffset)
            ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
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
        if (isStartElement()) {
            TRY_READ_IF_IN_CONTEXT(align)
            ELSE_TRY_READ_IF_IN_CONTEXT(posOffset)
            ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
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

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL wrapSquare
//! wrapSquare handler (Square Wrapping)
/*! ECMA-376, 20.4.2.17, p.3497.
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
    readWrap();

    while (!atEnd()) {
        readNext();
//        if (isStartElement()) {
//! @todo effectExtent
//        }
        BREAK_IF_END_OF(CURRENT_EL);
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
    readWrap();

    while (!atEnd()) {
        readNext();
//        if (isStartElement()) {
//! @todo effectExtent
//        }
        BREAK_IF_END_OF(CURRENT_EL);
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
    readWrap();

    while (!atEnd()) {
        readNext();
//        if (isStartElement()) {
//! @todo effectExtent
//        }
        BREAK_IF_END_OF(CURRENT_EL);
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

    SKIP_EVERYTHING
    /*    while (!atEnd()) {
            readNext();
            BREAK_IF_END_OF(CURRENT_EL);
        }*/
    READ_EPILOGUE
}

#define DRAWINGML_NS "a"
#define DRAWINGML_PIC_NS "pic" // DrawingML/Picture

#include <MsooXmlCommonReaderDrawingMLImpl.h> // this adds pic:pic, etc.
#include <MsooXmlVmlReaderImpl.h> // this adds w:pict, etc.
