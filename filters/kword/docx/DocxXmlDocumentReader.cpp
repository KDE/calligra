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
    MSOOXML::MsooXmlRelationships& _relationships)
        : MSOOXML::MsooXmlReaderContext(&_relationships),
        import(&_import), path(_path), file(_file),
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
    m_defaultNamespace = QLatin1String(MSOOXML_CURRENT_NS ":");
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
//! @todo TRY_READ_IF(background)
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

    QXmlStreamNamespaceDeclarations namespaces = namespaceDeclarations();
    for (int i = 0; i < namespaces.count(); i++) {
        kDebug() << "NS prefix:" << namespaces[i].prefix() << "uri:" << namespaces[i].namespaceUri();
    }

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
 - headerReference (Header Reference) §17.10.5
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

    m_currentPageStyle = KoGenStyle(KoGenStyle::StylePageLayout);
    m_currentPageStyle.setAutoStyleInStylesDotXml(true);
    m_currentPageStyle.addProperty("style:writing-mode", "lr-tb");
//! @todo handle all valued of style:print-orientation
    m_currentPageStyle.addProperty("style:print-orientation", "portrait");

    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            TRY_READ_IF(pgSz)
            ELSE_TRY_READ_IF(pgMar)
            ELSE_TRY_READ_IF(pgBorders)
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    QString pageLayoutStyleName("Mpm");
    pageLayoutStyleName = mainStyles->lookup(
        m_currentPageStyle, pageLayoutStyleName, KoGenStyles::DontForceNumbering);

    KoGenStyle masterStyle(KoGenStyle::StyleMaster);
//! @todo works because paragraphs have Standard style assigned by default; fix for multiple page styles
    QString masterStyleName("Standard");
//! @todo style:display-name
//    masterStyle->addAttribute("style:display-name", masterStyleName);
    masterStyle.addAttribute("style:page-layout-name", pageLayoutStyleName);
    /*masterStyleName =*/ mainStyles->lookup(masterStyle, masterStyleName, KoGenStyles::DontForceNumbering);
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
    bool ok;
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR(w)
    const qreal pageWidth = qreal(TWIP_TO_POINT(w.toUInt(&ok)));
    if (ok)
        m_currentPageStyle.addPropertyPt("fo:page-width", pageWidth);

    TRY_READ_ATTR(h)
    const qreal pageHeight = qreal(TWIP_TO_POINT(h.toUInt(&ok)));
    if (ok)
        m_currentPageStyle.addPropertyPt("fo:page-height", pageHeight);
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
    bool ok;
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR(top)
    const qreal marginTop = qreal(TWIP_TO_POINT(top.toUInt(&ok)));
    if (ok)
        m_currentPageStyle.addPropertyPt("fo:margin-top", marginTop);

    TRY_READ_ATTR(right)
    const qreal marginRight = qreal(TWIP_TO_POINT(right.toUInt(&ok)));
    if (ok)
        m_currentPageStyle.addPropertyPt("fo:margin-right", marginRight);

    TRY_READ_ATTR(bottom)
    const qreal marginBottom = qreal(TWIP_TO_POINT(bottom.toUInt(&ok)));
    if (ok)
        m_currentPageStyle.addPropertyPt("fo:margin-bottom", marginBottom);

    TRY_READ_ATTR(left)
    const qreal marginLeft = qreal(TWIP_TO_POINT(left.toUInt(&ok)));
    if (ok)
        m_currentPageStyle.addPropertyPt("fo:margin-left", marginLeft);

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
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            TRY_READ_IF(top)
            ELSE_TRY_READ_IF(left)
            ELSE_TRY_READ_IF(bottom)
            ELSE_TRY_READ_IF(right)
            ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    if (m_pageBorderStyles.count(m_pageBorderStyles.key(TopBorder)) == 4)
        m_currentPageStyle.addProperty("fo:border", m_pageBorderStyles.key(TopBorder)); // all sides the same
    else {
        if (!m_pageBorderStyles.key(TopBorder).isEmpty())
            m_currentPageStyle.addProperty("fo:border-top", m_pageBorderStyles.key(TopBorder));
        if (!m_pageBorderStyles.key(LeftBorder).isEmpty())
            m_currentPageStyle.addProperty("fo:border-left", m_pageBorderStyles.key(LeftBorder));
        if (!m_pageBorderStyles.key(BottomBorder).isEmpty())
            m_currentPageStyle.addProperty("fo:border-bottom", m_pageBorderStyles.key(BottomBorder));
        if (!m_pageBorderStyles.key(RightBorder).isEmpty())
            m_currentPageStyle.addProperty("fo:border-right", m_pageBorderStyles.key(RightBorder));
    }
    m_pageBorderStyles.clear();

    if (m_pageBorderPaddings.count(m_pageBorderPaddings.key(TopBorder)) == 4)
        m_currentPageStyle.addProperty("fo:padding", m_pageBorderPaddings.key(TopBorder)); // all sides the same
    else {
        if (!m_pageBorderPaddings.key(TopBorder).isEmpty())
            m_currentPageStyle.addProperty("fo:padding-top", m_pageBorderPaddings.key(TopBorder));
        if (!m_pageBorderPaddings.key(LeftBorder).isEmpty())
            m_currentPageStyle.addProperty("fo:padding-left", m_pageBorderPaddings.key(LeftBorder));
        if (!m_pageBorderPaddings.key(BottomBorder).isEmpty())
            m_currentPageStyle.addProperty("fo:padding-bottom", m_pageBorderPaddings.key(BottomBorder));
        if (!m_pageBorderPaddings.key(RightBorder).isEmpty())
            m_currentPageStyle.addProperty("fo:padding-right", m_pageBorderPaddings.key(RightBorder));
    }
    m_pageBorderPaddings.clear();

    READ_EPILOGUE
}

KoFilter::ConversionStatus DocxXmlDocumentReader::read_border(BorderSide borderSide, const char *borderSideName)
{
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR(val)
    TRY_READ_ATTR(sz)
    TRY_READ_ATTR(color)
    createBorderStyle(sz, color, val, borderSide);
    TRY_READ_ATTR(space)
    if (!space.isEmpty()) {
        int sp;
        STRING_TO_INT(space, sp, QString("w:%1@space").arg(borderSideName));
        m_pageBorderPaddings.insertMulti(QString::number(sp) + "pt", borderSide);
    }
    readNext();
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL top
//! w:top handler (Top Border)
/*! ECMA-376, 17.6.21, p. 676.

 Parent elements:
 - [done] pgBorders (§17.6.10)

 No child elements.
*/
//! @todo support all elements
KoFilter::ConversionStatus DocxXmlDocumentReader::read_top()
{
    READ_PROLOGUE
    RETURN_IF_ERROR(read_border(TopBorder, "top"));
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL left
//! left page border
KoFilter::ConversionStatus DocxXmlDocumentReader::read_left()
{
    READ_PROLOGUE
    RETURN_IF_ERROR(read_border(LeftBorder, "left"));
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL bottom
//! bottom page border
KoFilter::ConversionStatus DocxXmlDocumentReader::read_bottom()
{
    READ_PROLOGUE
    RETURN_IF_ERROR(read_border(BottomBorder, "bottom"));
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL right
//! right page border
KoFilter::ConversionStatus DocxXmlDocumentReader::read_right()
{
    READ_PROLOGUE
    RETURN_IF_ERROR(read_border(RightBorder, "right"));
    READ_EPILOGUE
}

void DocxXmlDocumentReader::createBorderStyle(const QString& size, const QString& color,
    const QString& lineStyle, BorderSide borderSide)
{
    if (lineStyle.isEmpty())
        return;

    QString border;
    if (!size.isEmpty())
        border += MSOOXML::Utils::ST_EighthPointMeasure_to_ODF(size) + " ";

    border.append(lineStyle + " ");

    if (color.startsWith('#'))
        border.append(color);
    else
        border.append(QLatin1String("#000000"));

    m_pageBorderStyles.insertMulti(border, borderSide);
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
    TRY_READ_ATTR(id)
    if (id.isEmpty()) {
        link_target.clear();
    }
    else {
        link_target = m_context->relationships->linkTarget(id);
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
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_p()
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
        m_currentParagraphStyle = KoGenStyle(KoGenStyle::StyleAuto, "paragraph");
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
 - fldChar (Complex Field Character) §17.16.18
 - footnoteRef (Footnote Reference Mark) §17.11.13
 - [done] footnoteReference (Footnote Reference) §17.11.14
 - instrText (Field Code) §17.16.23
 - lastRenderedPageBreak (Position of Last Calculated Page Break) §17.3.3.13
 - monthLong (Date Block - Long Month Format) §17.3.3.15
 - monthShort (Date Block - Short Month Format) §17.3.3.16
 - noBreakHyphen (Non Breaking Hyphen Character) §17.3.3.18
 - [done] object (Embedded Object) §17.3.3.19
 - pgNum (Page Number Block) §17.3.3.22
 - ptab (Absolute Position Tab Character) §17.3.3.23
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
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_r()
{
    READ_PROLOGUE
    while (!atEnd()) {
//kDebug() <<"[0]";
        readNext();
        kDebug() << *this;
//kDebug() <<"[1]";
        if (isStartElement()) {
            TRY_READ_IF(rPr)
            ELSE_TRY_READ_IF(t)
            ELSE_TRY_READ_IF(drawing)
            ELSE_TRY_READ_IF(endnoteReference)
            ELSE_TRY_READ_IF(footnoteReference)
            ELSE_TRY_READ_IF(object)
            ELSE_TRY_READ_IF(pict)
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
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_drawing()
{
    READ_PROLOGUE

    m_currentDrawStyle = KoGenStyle(KoGenStyle::StyleGraphicAuto, "graphic");
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

#undef MSOOXML_CURRENT_NS
#define MSOOXML_CURRENT_NS "pic" // DrawingML/Picture

#include <MsooXmlCommonReaderDrawingMLImpl.h> // this adds pic:pic, etc.
#include <MsooXmlVmlReaderImpl.h> // this adds w:pict, etc.
