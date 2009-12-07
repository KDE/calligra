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

#include "XlsxXmlStylesReader.h"

#include <MsooXmlSchemas.h>
#include <MsooXmlUtils.h>
#include <KoXmlWriter.h>
#include <styles/KoCharacterStyle.h>
#include <KoGenStyles.h>

#undef MSOOXML_CURRENT_NS
#define MSOOXML_CURRENT_CLASS XlsxXmlStylesReader
#define BIND_READ_CLASS MSOOXML_CURRENT_CLASS

#include <MsooXmlReader_p.h>

#include <KGlobal>

#include <QMap>

XlsxFontStyle::XlsxFontStyle()
    : underline(NoUnderline),
    bold(false),
    italic(false),
    strike(false),
    m_size(12.0),
    m_defaultSize(true)
{
}

class ST_UnderlineValue_fromStringMap : public QMap<QString, XlsxFontStyle::ST_UnderlineValue>
{
public:
    ST_UnderlineValue_fromStringMap() {
        insert(QLatin1String("single"), XlsxFontStyle::SingleUnderline);
        insert(QLatin1String("double"), XlsxFontStyle::SingleUnderline);
        insert(QLatin1String("singleAccounting"), XlsxFontStyle::SingleAccountingUnderline);
        insert(QLatin1String("doubleAccounting"), XlsxFontStyle::DoubleAccountingUnderline);
        insert(QLatin1String("none"), XlsxFontStyle::NoUnderline);
    }
};

XlsxFontStyle::ST_UnderlineValue XlsxFontStyle::ST_UnderlineValue_fromString(const QString& s)
{
    K_GLOBAL_STATIC(ST_UnderlineValue_fromStringMap, s_ST_UnderlineValues)
    kDebug() << s;
    const ST_UnderlineValue v = s_ST_UnderlineValues->value(s);
    kDebug() << v;
    if (v == NoUnderline && s != "none")
        return SingleUnderline; // default
    return v;
}

void XlsxFontStyle::setUnderline(const QString& s)
{
    underline = ST_UnderlineValue_fromString(s);
    kDebug() << underline;
}

void XlsxFontStyle::setupCharacterStyle(KoCharacterStyle* characterStyle) const
{
    switch (underline) {
    case SingleUnderline:
    case DoubleUnderline:
    case SingleAccountingUnderline:
    case DoubleAccountingUnderline:
        characterStyle->setUnderlineStyle(KoCharacterStyle::SolidLine);
        break;
    case NoUnderline:
    default:;
    }

    switch (underline) {
    case SingleUnderline:
    case SingleAccountingUnderline:
        characterStyle->setUnderlineType(KoCharacterStyle::SingleLine);
        break;
    case DoubleUnderline:
    case DoubleAccountingUnderline:
        characterStyle->setUnderlineType(KoCharacterStyle::DoubleLine);
        break;
    case NoUnderline:
    default:;
    }

//! @todo underlineWidth
    switch (underline) {
    case SingleUnderline:
    case SingleAccountingUnderline:
    case DoubleUnderline:
    case DoubleAccountingUnderline:
        characterStyle->setUnderlineWidth(KoCharacterStyle::AutoLineWeight, 1.0);
        break;
    case NoUnderline:
    default:;
    }

    if (strike) {
        characterStyle->setStrikeOutStyle(KoCharacterStyle::SolidLine);
        characterStyle->setStrikeOutType(KoCharacterStyle::SingleLine);
    }

    {
        KoCharacterStyle::LineWeight weight;
        qreal width;
        characterStyle->underlineWidth(weight, width);

        kDebug() << "underlineStyle:" << characterStyle->underlineStyle()
            << "underlineType:" << characterStyle->underlineType()
            << "underlineWeight:" << weight;
    }

    if (bold)
        characterStyle->setFontWeight(QFont::Bold);
    if (italic)
        characterStyle->setFontItalic(true);
    if (!m_defaultSize)
        characterStyle->setFontPointSize(m_size);
}

void XlsxFontStyle::setupCellTextStyle(KoGenStyle* cellStyle) const
{
    if (!name.isEmpty()) {
        cellStyle->addProperty("style:font-name", name, KoGenStyle::TextType);
    }
}

XlsxStyles::XlsxStyles()
{
}

XlsxStyles::~XlsxStyles()
{
    for (int i = 0; i<fontStyles.size(); i++)
        delete fontStyles[i];
}

//----------------------------------------------------------

XlsxCellFormat::XlsxCellFormat()
 : fontId(0)
 , horizontalAlignment(GeneralHorizontalAlignment)
 , verticalAlignment(NoVerticalAlignment)
{
}

class ST_HorizontalAlignment_fromStringMap : public QMap<QString, XlsxCellFormat::ST_HorizontalAlignment>
{
public:
    ST_HorizontalAlignment_fromStringMap() {
        insert(QLatin1String("general"), XlsxCellFormat::GeneralHorizontalAlignment);
        insert(QLatin1String("center"), XlsxCellFormat::CenterHorizontalAlignment);
        insert(QLatin1String("centerContinuous"), XlsxCellFormat::CenterContinuousHorizontalAlignment);
        insert(QLatin1String("distributed"), XlsxCellFormat::DistributedHorizontalAlignment);
        insert(QLatin1String("fill"), XlsxCellFormat::FillHorizontalAlignment);
        insert(QLatin1String("justify"), XlsxCellFormat::JustifyHorizontalAlignment);
        insert(QLatin1String("left"), XlsxCellFormat::LeftHorizontalAlignment);
        insert(QLatin1String("right"), XlsxCellFormat::RightHorizontalAlignment);
    }
};

void XlsxCellFormat::setHorizontalAlignment(const QString& alignment)
{
    K_GLOBAL_STATIC(ST_HorizontalAlignment_fromStringMap, s_ST_HorizontalAlignmentValues)
    horizontalAlignment = s_ST_HorizontalAlignmentValues->value(alignment);
}

class ST_VerticalAlignment_fromStringMap : public QMap<QString, XlsxCellFormat::ST_VerticalAlignment>
{
public:
    ST_VerticalAlignment_fromStringMap() {
        insert(QLatin1String(""), XlsxCellFormat::NoVerticalAlignment);
        insert(QLatin1String("bottom"), XlsxCellFormat::BottomVerticalAlignment);
        insert(QLatin1String("center"), XlsxCellFormat::CenterVerticalAlignment);
        insert(QLatin1String("distributed"), XlsxCellFormat::DistributedVerticalAlignment);
        insert(QLatin1String("justify"), XlsxCellFormat::JustifyVerticalAlignment);
        insert(QLatin1String("top"), XlsxCellFormat::TopVerticalAlignment);
    }
};

void XlsxCellFormat::setVerticalAlignment(const QString& alignment)
{
    K_GLOBAL_STATIC(ST_VerticalAlignment_fromStringMap, s_ST_VerticalAlignmentValues)
    verticalAlignment = s_ST_VerticalAlignmentValues->value(alignment);
}

// http://www.w3.org/TR/2001/REC-xsl-20011015/slice7.html#text-align
void XlsxCellFormat::setupCellStyle(KoGenStyle* cellStyle) const
{
//! CASE #S1600
//! @todo FillHorizontalAlignment, JustifyHorizontalAlignment
    QByteArray odfHorizontalAlignment;
    bool wrapOption = false;
    switch (horizontalAlignment) {
    case CenterHorizontalAlignment:
    case CenterContinuousHorizontalAlignment:
    case DistributedHorizontalAlignment:
        cellStyle->addProperty("fo:text-align", "center", KoGenStyle::ParagraphType);
        if (horizontalAlignment == DistributedHorizontalAlignment)
            wrapOption = true;
        break;
    case GeneralHorizontalAlignment: // ok?
    case LeftHorizontalAlignment:
        cellStyle->addProperty("fo:text-align", "start", KoGenStyle::ParagraphType);
        break;
    case RightHorizontalAlignment:
        cellStyle->addProperty("fo:text-align", "end", KoGenStyle::ParagraphType);
        break;
    case JustifyHorizontalAlignment:
        cellStyle->addProperty("fo:text-align", "justify", KoGenStyle::ParagraphType);
        break;
    case FillHorizontalAlignment:
    default:;
    }

    switch (verticalAlignment) {
    case CenterVerticalAlignment:
        cellStyle->addProperty("style:vertical-align", "middle");
        break;
    case TopVerticalAlignment:
        cellStyle->addProperty("style:vertical-align", "top");
        break;
    case NoVerticalAlignment:
    case JustifyVerticalAlignment: // ok?
    case DistributedVerticalAlignment:
    case BottomVerticalAlignment:
        if (verticalAlignment == DistributedVerticalAlignment)
            wrapOption = true;
    default:;
    }

//! @todo take alignment/@wrapText into account
    cellStyle->addProperty("fo:wrap-option", wrapOption ? "wrap" : "no-wrap");
}

//----------------------------------------------------------

XlsxXmlStylesReaderContext::XlsxXmlStylesReaderContext(XlsxStyles& _styles)
    : styles(&_styles)
{
}

class XlsxXmlStylesReader::Private {
public:
    Private()
    {
    }
    ~Private()
    {
    }
private:
};

XlsxXmlStylesReader::XlsxXmlStylesReader(KoOdfWriters *writers)
    : MSOOXML::MsooXmlReader(writers)
    , m_context(0)
    , d(new Private)
{
    init();
}

XlsxXmlStylesReader::~XlsxXmlStylesReader()
{
    delete d;
    delete m_currentFontStyle;
    delete m_currentCellFormat;
}

void XlsxXmlStylesReader::init()
{
    m_defaultNamespace = "";
    m_fontStyleIndex = 0;
    m_cellFormatIndex = 0;
    m_currentFontStyle = 0;
    m_currentCellFormat = 0;
}

KoFilter::ConversionStatus XlsxXmlStylesReader::read(MSOOXML::MsooXmlReaderContext* context)
{
    m_context = dynamic_cast<XlsxXmlStylesReaderContext*>(context);
    Q_ASSERT(m_context);
    const KoFilter::ConversionStatus result = readInternal();
    m_context = 0;
    if (result == KoFilter::OK)
        return KoFilter::OK;
    return result;
}

KoFilter::ConversionStatus XlsxXmlStylesReader::readInternal()
{
    kDebug() << "=============================";
    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }

    // sst
    readNext();
    kDebug() << *this << namespaceUri();

    if (!expectEl("styleSheet")) {
        return KoFilter::WrongFormat;
    }
    if (!expectNS(MSOOXML::Schemas::spreadsheetml)) {
        return KoFilter::WrongFormat;
    }

    QXmlStreamNamespaceDeclarations namespaces( namespaceDeclarations() );
    for (int i=0; i<namespaces.count(); i++) {
        kDebug() << "NS prefix:" << namespaces[i].prefix() << "uri:" << namespaces[i].namespaceUri();
    }
//! @todo find out whether the namespace returned by namespaceUri()
//!       is exactly the same ref as the element of namespaceDeclarations()
    if (!namespaces.contains( QXmlStreamNamespaceDeclaration( QString(), MSOOXML::Schemas::spreadsheetml ) )) {
        raiseError(i18n("Namespace \"%1\" not found", MSOOXML::Schemas::spreadsheetml));
        return KoFilter::WrongFormat;
    }
//! @todo expect other namespaces too...

    TRY_READ(styleSheet)
    kDebug() << "===========finished============";
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL styleSheet
//! styleSheet handler (Style Sheet)
/*! ECMA-376, 18.8.39, p. 1993.
 This is the root element of the Styles part.

 Child elements:
 - borders (Borders) §18.8.5
 - cellStyles (Cell Styles) §18.8.8
 - cellStyleXfs (Formatting Records) §18.8.9
 - [done] cellXfs (Cell Formats) §18.8.10
 - colors (Colors) §18.8.11
 - dxfs (Formats) §18.8.15
 - extLst (Future Feature Data Storage Area) §18.2.10
 - fills (Fills) §18.8.21
 - [done] fonts (Fonts) §18.8.23
 - numFmts (Number Formats) §18.8.31
 - tableStyles (Table Styles) §18.8.42
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_styleSheet()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(fonts)
            TRY_READ_IF(cellXfs)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL fonts
//! fonts handler (Fonts)
/*! ECMA-376, 18.8.23, p. 1964.
 This element contains all font definitions for this workbook.

 Child elements:
 - [done] font (Font) §18.8.22
 Parent elements:
 - [done] styleSheet (§18.8.39)
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_fonts()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs( attributes() );
    TRY_READ_ATTR_WITHOUT_NS(count)

    bool ok;
    const uint countNumber = count.toUInt(&ok);
    if (!ok) {
        raiseUnexpectedAttributeValueError(count, "styleSheet@fonts");
        return KoFilter::WrongFormat;
    }
    m_context->styles->fontStyles.resize(countNumber);
    m_fontStyleIndex = 0;

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(font)
            ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL font
//! font handler (Font)
/*! ECMA-376, 18.8.22, p. 1964.
 This element defines the properties for one of the fonts used in this workbook.

 Child elements:
 - [done] b (Bold) §18.8.2
 - charset (Character Set) §18.4.1
 - color (Data Bar Color) §18.3.1.15
 - condense (Condense) §18.8.12
 - extend (Extend) §18.8.17
 - family (Font Family) §18.8.18
 - [done] i (Italic) §18.8.26
 - [done] name (Font Name) §18.8.29
 - outline (Outline) §18.4.2
 - scheme (Scheme) §18.8.35
 - shadow (Shadow) §18.8.36
 - [done] strike (Strike Through) §18.4.10
 - [done] sz (Font Size) §18.4.11
 - [done] u (Underline) §18.4.13
 - vertAlign (Vertical Alignment) §18.4.14
 Parent elements:
 - dxf (§18.8.14)
 - [done] fonts (§18.8.23)
 - ndxf (§18.11.1.4)
 - odxf (§18.11.1.6)

 @todo support all child elements
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_font()
{
    READ_PROLOGUE

    if (m_fontStyleIndex >= (uint)m_context->styles->fontStyles.size()) {
        raiseError(i18n("Declared number of font styles too small (%1)", m_context->styles->fontStyles.size()));
        return KoFilter::WrongFormat;
    }

    kDebug() << "font #" << m_fontStyleIndex;
    m_currentFontStyle = new XlsxFontStyle;

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(sz)
            ELSE_TRY_READ_IF(name)
            ELSE_TRY_READ_IF(b)
            ELSE_TRY_READ_IF(i)
            ELSE_TRY_READ_IF(strike)
            ELSE_TRY_READ_IF(u)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    READ_EPILOGUE_WITHOUT_RETURN

    m_context->styles->fontStyles[m_fontStyleIndex] = m_currentFontStyle;
    m_currentFontStyle = 0;
    m_fontStyleIndex++;

    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL sz
//! sz handler (Font Size)
/*! ECMA-376, 18.4.11, p. 1913.
 This element represents the point size (1/72 of an inch) of the Latin and East Asian text.

 No child elements.
 Parent elements:
 - [done] font (§18.8.22)
 - rPr (§18.4.7)

 @todo support all elements
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_sz()
{
    Q_ASSERT(m_currentFontStyle);

    READ_PROLOGUE
    const QXmlStreamAttributes attrs( attributes() );
    TRY_READ_ATTR_WITHOUT_NS(val)
    bool ok;
    m_currentFontStyle->setSize(val.toDouble(&ok));
    if (!ok) {
        raiseUnexpectedAttributeValueError(val, "sz");
        return KoFilter::WrongFormat;
    }

    while (true) {
        BREAK_IF_END_OF(CURRENT_EL);
        readNext();
        break;
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL name
//! name handler (Font Name)
/*! ECMA-376, 18.8.29, p. 1973.
 This element specifies the face name of this font.

 No child elements.
 Parent elements:
 - [done] font (§18.8.22)
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_name()
{
    Q_ASSERT(m_currentFontStyle);

    READ_PROLOGUE
    const QXmlStreamAttributes attrs( attributes() );
    TRY_READ_ATTR_WITHOUT_NS_INTO(val, m_currentFontStyle->name)

    while (true) {
        BREAK_IF_END_OF(CURRENT_EL);
        readNext();
        break;
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL b
//! b handler (Bold)
/*! ECMA-376, 18.8.2, p. 1947.
 Displays characters in bold face font style.

 No child elements.
 Parent elements:
 - [done] font (§18.8.22)
 - rPr (§18.4.7)
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_b()
{
    Q_ASSERT(m_currentFontStyle);

    READ_PROLOGUE
    const QXmlStreamAttributes attrs( attributes() );
    m_currentFontStyle->bold = readBooleanAttr("val");

    while (true) {
        BREAK_IF_END_OF(CURRENT_EL);
        readNext();
        break;
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL i
//! i handler (Italic)
/*! ECMA-376, 18.8.26, p. 1969.
 Displays characters in italic font style.

 No child elements.
 Parent elements:
 - [done] font (§18.8.22)
 - rPr (§18.4.7)
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_i()
{
    Q_ASSERT(m_currentFontStyle);

    READ_PROLOGUE
    const QXmlStreamAttributes attrs( attributes() );
    m_currentFontStyle->italic = readBooleanAttr("val");

    while (true) {
        BREAK_IF_END_OF(CURRENT_EL);
        readNext();
        break;
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL strike
//! strike handler (Strike Through)
/*! ECMA-376, 18.4.10, p. 1913.
 This element draws a strikethrough line through the horizontal middle of the text.

 No child elements.
 Parent elements:
 - [done] font (§18.8.22)
 - rPr (§18.4.7)
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_strike()
{
    Q_ASSERT(m_currentFontStyle);

    READ_PROLOGUE
    const QXmlStreamAttributes attrs( attributes() );
    m_currentFontStyle->strike = readBooleanAttr("val");

    while (true) {
        BREAK_IF_END_OF(CURRENT_EL);
        readNext();
        break;
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL u
//! u handler (Underline)
/*! ECMA-376, 18.4.13, p. 1914.
 This element represents the underline formatting style.

 No child elements.
 Parent elements:
 - [done] font (§18.8.22)
 - rPr (§18.4.7)
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_u()
{
    Q_ASSERT(m_currentFontStyle);

    READ_PROLOGUE
    const QXmlStreamAttributes attrs( attributes() );
    m_currentFontStyle->setUnderline(attrs.value("val").toString());

    while (true) {
        BREAK_IF_END_OF(CURRENT_EL);
        readNext();
        break;
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL cellXfs
//! cellXfs handler (Cell Formats)
/*! ECMA-376, 18.8.10, p. 1956.
 This element contains the master formatting records (xf) which define
 the formatting applied to cells in this workbook.

 Child elements:
 - [done] xf (Format) §18.8.45
 Parent elements:
 - [done] styleSheet (§18.8.39)
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_cellXfs()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs( attributes() );
    TRY_READ_ATTR_WITHOUT_NS(count)

    bool ok;
    const uint countNumber = count.toUInt(&ok);
    if (!ok) {
        raiseUnexpectedAttributeValueError(count, "styleSheet@cellXfs");
        return KoFilter::WrongFormat;
    }
    m_context->styles->cellFormats.resize(countNumber);
    m_cellFormatIndex = 0;

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(xf)
            ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL xf
//! xf handler (Format)
/*! ECMA-376, 18.8.45, p. 1999.
 A single xf element describes all of the formatting for a cell.

 Child elements:
 - [done] alignment (Alignment) §18.8.1
 Parent elements:
 - cellStyleXfs (§18.8.9)
 - [done] cellXfs (§18.8.10)
 - extLst (Future Feature Data Storage Area) §18.2.10
 - protection (Protection Properties) §18.8.33
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_xf()
{
    READ_PROLOGUE

    if (m_cellFormatIndex >= (uint)m_context->styles->cellFormats.size()) {
        raiseError(i18n("Declared number of cell formats too small (%1)", m_context->styles->cellFormats.size()));
        return KoFilter::WrongFormat;
    }

    const QXmlStreamAttributes attrs( attributes() );
    TRY_READ_ATTR_WITHOUT_NS(fontId)
    bool ok;
    const uint fontIdNumber = fontId.toUInt(&ok);
    if (!ok) {
        raiseUnexpectedAttributeValueError(fontId, "xf@fontId");
        return KoFilter::WrongFormat;
    }


    kDebug() << "cell format #" << m_cellFormatIndex;
    m_currentCellFormat = new XlsxCellFormat;
    kDebug() << "- fontId:" << fontId;
    m_currentCellFormat->fontId = fontIdNumber;

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(alignment)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    READ_EPILOGUE_WITHOUT_RETURN

    m_context->styles->cellFormats[m_cellFormatIndex] = m_currentCellFormat;
    m_currentCellFormat = 0;
    m_cellFormatIndex++;

    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL alignment
//! alignment handler (Alignment)
/*! ECMA-376, 18.8.1, p. 1944.
 Formatting information pertaining to text alignment in cells.

 No child elements.
 Parent elements:
 - dxf (§18.8.14)
 - ndxf (§18.11.1.4)
 - odxf (§18.11.1.6)
 - [done] xf (§18.8.45)
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_alignment()
{
    Q_ASSERT(m_currentCellFormat);

    READ_PROLOGUE
    const QXmlStreamAttributes attrs( attributes() );
    m_currentCellFormat->setHorizontalAlignment(attrs.value("horizontal").toString());
    kDebug() << "horizontalAlignment:" << m_currentCellFormat->horizontalAlignment;
    m_currentCellFormat->setVerticalAlignment(attrs.value("vertical").toString());
    kDebug() << "verticalAlignment:" << m_currentCellFormat->verticalAlignment;
//! @todo more attributes

    while (true) {
        BREAK_IF_END_OF(CURRENT_EL);
        readNext();
        break;
    }
    READ_EPILOGUE
}
