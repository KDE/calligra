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

#include "DocxXmlDocumentReader.h"
#include "DocxXmlNotesReader.h"
#include "DocxXmlHeaderReader.h"
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
        themes(&_themes), 
        m_commentsLoaded(false), m_endnotesLoaded(false), m_footnotesLoaded(false)
{
}

KoFilter::ConversionStatus DocxXmlDocumentReaderContext::loadComments(KoOdfWriters *writers)
{
    if (m_commentsLoaded)
        return KoFilter::OK;
    m_commentsLoaded = true;
    DocxXmlCommentsReaderContext context(m_comments);
    DocxXmlCommentsReader reader(writers);
    QString errorMessage;
    const KoFilter::ConversionStatus status
        = import->loadAndParseDocument(&reader, "word/comments.xml", errorMessage, &context);
    if (status != KoFilter::OK)
        reader.raiseError(errorMessage);
    return status;
}

KoFilter::ConversionStatus DocxXmlDocumentReaderContext::loadEndnotes(KoOdfWriters *writers)
{
    if (m_endnotesLoaded)
        return KoFilter::OK;
    m_endnotesLoaded = true;
    DocxXmlNotesReaderContext context(m_endnotes);
    DocxXmlNotesReader reader(writers);
    QString errorMessage;
    const KoFilter::ConversionStatus status
        = import->loadAndParseDocument(&reader, "word/endnotes.xml", errorMessage, &context);
    if (status != KoFilter::OK)
        reader.raiseError(errorMessage);
    return status;
}

KoFilter::ConversionStatus DocxXmlDocumentReaderContext::loadFootnotes(KoOdfWriters *writers)
{
    if (m_footnotesLoaded)
        return KoFilter::OK;
    m_footnotesLoaded = true;
    DocxXmlNotesReaderContext context(m_footnotes);
    DocxXmlNotesReader reader(writers);
    QString errorMessage;
    const KoFilter::ConversionStatus status
        = import->loadAndParseDocument(&reader, "word/footnotes.xml", errorMessage, &context);
    if (status != KoFilter::OK)
        reader.raiseError(errorMessage);

    return status;
}

DocxComment DocxXmlDocumentReaderContext::comment(KoOdfWriters *writers, int id)
{
    if (KoFilter::OK != loadComments(writers))
        return DocxComment();
    return m_comments.value(id);
}

DocxNote DocxXmlDocumentReaderContext::endnote(KoOdfWriters *writers, int id)
{
    if (KoFilter::OK != loadEndnotes(writers))
        return DocxNote();
    return m_endnotes.value(id);
}

DocxNote DocxXmlDocumentReaderContext::footnote(KoOdfWriters *writers, int id)
{
    if (KoFilter::OK != loadFootnotes(writers))
        return DocxNote();
    return m_footnotes.value(id);
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
        , d(new Private)
{
    init();
}

DocxXmlDocumentReader::~DocxXmlDocumentReader()
{
    doneInternal(); // MsooXmlCommonReaderImpl.h
    delete d;
}

void DocxXmlDocumentReader::init()
{
    initInternal(); // MsooXmlCommonReaderImpl.h
    initDrawingML();
    m_defaultNamespace = QLatin1String(MSOOXML_CURRENT_NS ":");
    m_complexCharType = NoComplexFieldCharType;
    m_complexCharStatus = NoneAllowed;
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
            TRY_READ_IF(background)
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
 - cols (Column Definitions) §17.6.4
 - docGrid (Document Grid) §17.6.5
 - endnotePr (Section-Wide Endnote Properties) §17.11.5
 - footerReference (Footer Reference) §17.10.2
 - footnotePr (Section-Wide Footnote Properties) §17.11.11
 - formProt (Only Allow Editing of Form Fields) §17.6.6
 - [done] headerReference (Header Reference) §17.10.5
 - lnNumType (Line Numbering Settings) §17.6.8
 - noEndnote (Suppress Endnotes In Document) §17.11.16
 - paperSrc (Paper Source Information) §17.6.9
 - [done] pgBorders (Page Borders) §17.6.10
 - [done] pgMar (Page Margins) §17.6.11
 - pgNumType (Page Numbering Settings) §17.6.12
 - [done] pgSz (Page Size) §17.6.13
 - printerSettings (Reference to Printer Settings Data) §17.6.14
 - rtlGutter (Gutter on Right Side of Page) §17.6.16
 - sectPrChange (Revision Information for Section Properties) §17.13.5.32
 - textDirection (Text Flow Direction) §17.6.20
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

    m_masterPageStyle = KoGenStyle(KoGenStyle::MasterPageStyle);
  
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            TRY_READ_IF(pgSz)
            ELSE_TRY_READ_IF(pgMar)
            ELSE_TRY_READ_IF(pgBorders)
            ELSE_TRY_READ_IF(headerReference)
        }
        BREAK_IF_END_OF(CURRENT_EL);
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
    const KoFilter::ConversionStatus status
        = m_context->import->loadAndParseDocument(&reader, m_context->path + '/' + link_target, errorMessage, m_context);
    if (status != KoFilter::OK) {
        reader.raiseError(errorMessage);
    }

//! @todo: support type attribute

    m_masterPageStyle.addChildElement("style:header", reader.content());

    readNext();
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

/*    if (color ==  MsooXmlReader::constAuto) { // default
    }
    else*/ if (color.startsWith('#')) {
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
//! @todo    TRY_READ_ATTR(dxaOrig)?
//! @todo    TRY_READ_ATTR(dyaOrig)?
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            //! @todo support VML here
            TRY_READ_IF_NS(o, OLEObject)
            //! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL commentRangeStart
KoFilter::ConversionStatus DocxXmlDocumentReader::read_commentRangeStart()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    READ_ATTR(id)
    int idNumber = -1;
    STRING_TO_INT(id, idNumber, "commentRangeStart@id")
    const DocxComment comment(m_context->comment(this, idNumber));
    if (comment.isNull()) {
        raiseError(i18n("Comment \"%1\" not found", id));
        return KoFilter::WrongFormat;
    }

    body->startElement("office:annotation");

    body->startElement("dc:creator");
    body->addTextSpan(comment.author());
    body->endElement(); // dc:creator

    body->startElement("dc:date");
    //! @todo date ok?
    body->addTextSpan(comment.dateTime().toString(Qt::ISODate));
    body->endElement(); // dc:date

    body->startElement("text:p");
    //! @todo hardcoded style
    body->addAttribute("text:style-name", "P1");
    body->startElement("text:span");
    body->addTextSpan(comment.text());
    body->endElement(); // text:span
    body->endElement(); // text:p

    body->endElement(); // office:annotation
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL endnoteReference
KoFilter::ConversionStatus DocxXmlDocumentReader::read_endnoteReference()
{
    /*
    # example endnote from odt document converted with OpenOffice
    <text:note text:id="ftn1" text:note-class="endnote">
    <text:note-citation>i</text:note-citation>
    <text:note-body>
    <text:p text:style-name="Endnote">Tassa olisi endnote!</text:p>
    </text:note-body>
    </text:note></text:p>
    */
    const QXmlStreamAttributes attrs(attributes());
    READ_ATTR(id)
    int idNumber = -1;
    STRING_TO_INT(id, idNumber, "endnoteReference@id")
    const DocxNote note(m_context->endnote(this, idNumber));
    if (note.isNull()) {
        raiseError(i18n("Endnote \"%1\" not found", id));
        return KoFilter::WrongFormat;
    }
    body->startElement("text:note");
    body->addAttribute("text:id", "ftn1");
    body->addAttribute("text:note-class", "endnote");

    body->startElement("text:note-citation");
    body->addTextSpan(QString::number(note.number)); // this needs to be improved in future!
    body->endElement(); // text:note-citation

    body->startElement("text:note-body");
    body->startElement("text:p");
    body->addAttribute("text:style-name", "Endnote");
    body->addTextSpan(note.text);
    body->endElement(); // text:p
    body->endElement(); // text:note-body

    body->endElement(); // text:note
    return KoFilter::OK;
}



#undef CURRENT_EL
#define CURRENT_EL footnoteReference
KoFilter::ConversionStatus DocxXmlDocumentReader::read_footnoteReference()
{
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
    const QXmlStreamAttributes attrs(attributes());
    READ_ATTR(id)
    int idNumber = -1;
    STRING_TO_INT(id, idNumber, "footnoteReference@id")
    const DocxNote note(m_context->footnote(this, idNumber));
    if (note.isNull()) {
        raiseError(i18n("Footnote \"%1\" not found", id));
        return KoFilter::WrongFormat;
    }
    body->startElement("text:note");
    body->addAttribute("text:id", "ftn1");
    body->addAttribute("text:note-class", "footnote");

    body->startElement("text:note-citation");
    body->addTextSpan(QString::number(note.number)); // this needs to be improved in future!
    body->endElement(); // text:note-citation

    body->startElement("text:note-body");
    body->startElement("text:p");
    body->addAttribute("text:style-name", "Footnote");
    body->addTextSpan(note.text);
    body->endElement(); // text:p
    body->endElement(); // text:note-body

    body->endElement(); // text:note
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL fldChar
//! fldChar handler
/*! Complex field character

 Parent elements:
 - r (§17.3.2.25)
 - r (§22.1.2.87)
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

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL lastRenderedPageBreak
//! lastRenderedPageBreak handler
/*
 Parent elements:
 - r (§17.3.2.25)
 - r (§22.1.2.87)
*/
KoFilter::ConversionStatus DocxXmlDocumentReader::read_lastRenderedPageBreak()
{
    READ_PROLOGUE
    m_currentParagraphStyle.addProperty("fo:break-before", "page");
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
 - hyperlink (§17.16.22)
 - p (§17.3.1.22)
 - sdtContent (§17.5.2.36)
 - smartTag (§17.5.1.9)

 Child elements:
 - bdo (Bidirectional Override) §17.3.2.3
 - bookmarkEnd (Bookmark End) §17.13.6.1
 - bookmarkStart (Bookmark Start) §17.13.6.2
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
 - r (Text Run) §17.3.2.25
 - sdt (Inline-Level Structured Document Tag) §17.5.2.31
 - smartTag (Inline-Level Smart Tag) §17.5.1.9
 - subDoc (Anchor for Subdocument Location) §17.17.1.1
*/
//! @todo support all elements
KoFilter::ConversionStatus DocxXmlDocumentReader::read_hyperlink()
{
    READ_PROLOGUE

    QString link_target;
    MSOOXML::Utils::XmlWriteBuffer linkBuf;
    body = linkBuf.setWriter(body);

    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITH_NS(r, id)
    if (r_id.isEmpty()) {
        link_target.clear();
    }
    else {
        link_target = m_context->relationships->linkTarget(r_id, m_context->path, m_context->file);
    }
    kDebug() << "link_target:" << link_target;

    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            TRY_READ_IF(t)
            ELSE_TRY_READ_IF(r)
            ELSE_TRY_READ_IF(hyperlink)
            //! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL)
    }

    body = linkBuf.originalWriter();
    body->startElement("text:a");
    body->addAttribute("xlink:type", "simple");
    body->addAttribute("xlink:href", QUrl(link_target).toEncoded());
    (void)linkBuf.releaseWriter();
    body->endElement(); // text:a

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
 - bookmarkEnd (Bookmark End) §17.13.6.1
 - bookmarkStart (Bookmark Start) §17.13.6.2
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
 - fldSimple (Simple Field) §17.16.19
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

    MSOOXML::Utils::XmlWriteBuffer textPBuf;

    if (args & read_p_Skip) {
        kDebug() << "SKIP!";
    } else {
        body = textPBuf.setWriter(body);
        m_currentParagraphStyle = KoGenStyle(KoGenStyle::ParagraphAutoStyle, "paragraph");
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
            ELSE_TRY_READ_IF(pPr) // CASE #400.1
//! @todo add more conditions testing the parent
            ELSE_TRY_READ_IF(r) // CASE #400.2
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    if (args & read_p_Skip) {
        //nothing
    } else {
        body = textPBuf.originalWriter();
        body->startElement("text:p", false);
        if (m_currentStyleName.isEmpty()) {
            setupParagraphStyle();
        }
        else {
            body->addAttribute("text:style-name", m_currentStyleName);
        }
        /*        if (!m_paragraphStyleNameWritten) {
                    // no style, set default
                    body->addAttribute("text:style-name", "Standard");
                }*/
        (void)textPBuf.releaseWriter();
        body->endElement(); //text:p
        kDebug() << "/text:p";
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
 - tab (Tab Character) §17.3.3.32
 - yearLong (Date Block - Long Year Format) §17.3.3.33
 - yearShort (Date Block - Short Year Format) §17.3.3.34

 VML child elements (see Part 4):
 - pict (VML Object) §9.2.2.2
*/
//! @todo support all elements
KoFilter::ConversionStatus DocxXmlDocumentReader::read_r()
{
    READ_PROLOGUE
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
//            else { SKIP_EVERYTHING }
//! @todo add ELSE_WRONG_FORMAT
//kDebug() <<"[1.5]";
        }
//kDebug() <<"[2]";
        BREAK_IF_END_OF(CURRENT_EL);
//kDebug() <<"[2.5]";
    }
//kDebug() <<"[3]";
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
 - outline (Display Character Outline) §17.3.2.23
 - position (Vertically Raised or Lowered Text) §17.3.2.24
 - [done] rFonts (Run Fonts) §17.3.2.26
 - rPrChange (Revision Information for Run Properties on the Paragraph Mark) §17.13.5.30
 - rStyle (Referenced Character Style) §17.3.2.29
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
 - webHidden (Web Hidden Text) §17.3.2.44
*/
//! @todo support all elements
KoFilter::ConversionStatus DocxXmlDocumentReader::read_rPr(rPrCaller caller)
{
    READ_PROLOGUE

    const bool setupTextStyle = caller == rPr_r;
    kDebug() << "setupTextStyle:" << setupTextStyle;

    const QXmlStreamAttributes attrs(attributes());

    Q_ASSERT(m_currentTextStyleProperties == 0);
//    delete m_currentTextStyleProperties;
    m_currentTextStyleProperties = new KoCharacterStyle();

    if (!m_currentTextStylePredefined) {
        m_currentTextStyle = KoGenStyle(KoGenStyle::TextAutoStyle, "text");
    }

    MSOOXML::Utils::XmlWriteBuffer textSpanBuf;
    if (setupTextStyle) {
//kDebug() << "text:span...";
        body = textSpanBuf.setWriter(body);
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
            ELSE_TRY_READ_IF(color)
            ELSE_TRY_READ_IF(highlight)
            ELSE_TRY_READ_IF(lang)
            ELSE_TRY_READ_IF_IN_CONTEXT(shd)
            ELSE_TRY_READ_IF(vertAlign)
            ELSE_TRY_READ_IF(rFonts)
            ELSE_TRY_READ_IF(spacing)
            ELSE_TRY_READ_IF(caps)
            ELSE_TRY_READ_IF(smallCaps)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    if (setupTextStyle) {
        m_currentTextStyleProperties->saveOdf(m_currentTextStyle);

        READ_EPILOGUE_WITHOUT_RETURN
        // read 't' in one go and insert the contents into text:span
        readNext();
        // Only create text:span if the next el. is 't'. Do not this the next el. is 'drawing', etc.
        if (QUALIFIED_NAME_IS(t)) {
            const QString currentTextStyleName(mainStyles->insert(m_currentTextStyle));
            if (m_moveToStylesXml) {
                mainStyles->markStyleForStylesXml(currentTextStyleName);
            }
            if (m_complexCharStatus == InstrExecute && m_complexCharType == HyperlinkComplexFieldCharType) {
                body->startElement("text:a");
                body->addAttribute("xlink:type", "simple");
                body->addAttribute("xlink:href", QUrl(m_complexCharValue).toEncoded());
            }
            body->startElement("text:span", false);
            body->addAttribute("text:style-name", currentTextStyleName);
            TRY_READ(t)
            body->endElement(); //text:span
            if (m_complexCharStatus == InstrExecute && m_complexCharType == HyperlinkComplexFieldCharType) {
                body->endElement(); // text:a
            }
        }
//kDebug() << "currentTextStyleName:" << currentTextStyleName;
        else {
            undoReadNext();
        }
        //already checked expectElEnd(MSOOXML_CURRENT_NS ":t");
//kDebug() << "/text:span";
        body = textSpanBuf.releaseWriter();
    }

    m_currentTextStyleProperties->saveOdf(m_currentTextStyle);
    delete m_currentTextStyleProperties;
    m_currentTextStyleProperties = 0;

    if (setupTextStyle) {
        return KoFilter::OK;
    }
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
 - framePr (Text Frame Properties) §17.3.1.11
 - ind (Paragraph Indentation) §17.3.1.12
 - [done] jc (Paragraph Alignment) §17.3.1.13
 - keepLines (Keep All Lines On One Page) §17.3.1.14
 - keepNext (Keep Paragraph With Next Paragraph) §17.3.1.15
 - kinsoku (Use East Asian Typography Rules for First and Last Character per Line) §17.3.1.16
 - mirrorIndents (Use Left/Right Indents as Inside/Outside Indents) §17.3.1.18
 - numPr (Numbering Definition Instance Reference) §17.3.1.19
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
 - suppressLineNumbers (Suppress Line Numbers for Paragraph) §17.3.1.35
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

    if (!m_currentParagraphStylePredefined) {
        m_currentParagraphStyle = KoGenStyle(KoGenStyle::ParagraphAutoStyle, "text");
    }

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
            ELSE_TRY_READ_IF(pBdr)
            ELSE_TRY_READ_IF(ind)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
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

    m_currentDrawStyle = KoGenStyle(KoGenStyle::GraphicAutoStyle, "graphic");
    m_currentDrawStyle.addAttribute("style:parent-style-name", QLatin1String("Graphics"));

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
    const char *ns = 0;
#ifdef MSOOXML_CURRENT_NS
    ns = MSOOXML_CURRENT_NS;
#endif
    RETURN_IF_ERROR(MSOOXML::MsooXmlReader::read_sz(ns, m_currentTextStyleProperties))
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

    TRY_READ_ATTR(line)
    const int lineSpace = (TWIP_TO_POINT(line.toDouble(&ok)));

    if (ok) {
        m_currentParagraphStyle.addPropertyPt("fo:line-height", lineSpace);
    }

    TRY_READ_ATTR(lineRule)

    SKIP_EVERYTHING
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL shd
//! Shading handler (object's shading attributes)
/*! ECMA-376, 17.3.5, p.399
 Parent Elements:
 - [done] pPr (Paragraph Properties) §17.3.1.26
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

    //READ_ATTR_INTO(val, m_currentStyleName)
    SKIP_EVERYTHING
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
    KoXmlWriter elementWriter(&tabs);
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
            TRY_READ_IF(drawing)
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}
// ---------------------------------------------------------------------------

#define blipFill_NS "pic"

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

    TRY_READ_ATTR_WITH_NS(r, id)
    const QString oleName(m_context->relationships->target(m_context->path, m_context->file, r_id));
    kDebug() << "oleName:" << oleName;

    QString destinationName;
//! @todo ooo saves binaries to the root dir; should we?
    RETURN_IF_ERROR( copyFile(oleName, QString(), destinationName) )

    body->startElement("text:p");
    body->startElement("draw:rect");
    body->addAttribute("text:anchor-type", "paragraph");
    body->addAttribute("draw:z-index", "0");
//! todo    body->addAttribute"draw:style-name", styleName);
/*eg.
   <style:style style:name="gr1" style:family="graphic">
      <style:graphic-properties svg:stroke-color="#000023" draw:fill="bitmap" draw:fill-color="#ffffff" draw:fill-image-name="Bitmape_20_1"
       style:repeat="no-repeat" draw:textarea-horizontal-align="center" draw:textarea-vertical-align="middle" style:run-through="foreground".
       style:wrap="none" style:vertical-pos="from-top" style:vertical-rel="paragraph" style:horizontal-pos="from-left".
       style:horizontal-rel="paragraph" draw:wrap-influence-on-position="once-concurrent" style:flow-with-text="false"/>
    </style:style>*/
//! todo size!
    body->addAttribute("svg:width", "14.179cm");
    body->addAttribute("svg:height", "10.97cm");
    body->endElement(); //draw:rect
    body->endElement(); //text:p

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
    }
    READ_EPILOGUE
}

#define DRAWINGML_NS "wp"
#define DRAWINGML_PIC_NS "pic" // DrawingML/Picture

#include <MsooXmlCommonReaderDrawingMLImpl.h> // this adds pic:pic, etc.
#include <MsooXmlVmlReaderImpl.h> // this adds w:pict, etc.
