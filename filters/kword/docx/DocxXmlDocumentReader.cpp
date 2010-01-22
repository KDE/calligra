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
        import(&_import), path(_path), file(_file)
{
}

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
// @todo handle all valued of style:print-orientation
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
        border += MSOOXML::Utils::ST_EighthPointMeasure_to_pt(size) + " ";

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
//! @todo support VML here
        TRY_READ_IF_NS(o, OLEObject)
//! @todo add ELSE_WRONG_FORMAT
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

// ---------------------------------------------------------------------------

#define blipFill_NS "pic"

#include <MsooXmlCommonReaderImpl.h> // this adds a:p, a:pPr, a:t, a:r, etc.

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
