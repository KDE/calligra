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

XlsxColorStyle::XlsxColorStyle()
        : automatic(false), indexed(-1)
{
}

XlsxFontStyle::XlsxFontStyle()
        : underline(NoUnderline),
        bold(false),
        italic(false),
        strike(false),
        m_size(12.0),
        m_defaultSize(true)
{
}

XlsxFillStyle::XlsxFillStyle()
        : patternType(NonePatternType)
{
}

const XlsxColorStyle* XlsxFillStyle::realBackgroundColor() const
{
    switch (patternType) {
    case NonePatternType:
        return 0;
    case SolidPatternType:
        return &fgColor;
    default:;
//! @todo support other patterns
    }
    return &bgColor;
}

void XlsxFillStyle::setupCellStyle(KoGenStyle* cellStyle) const
{
//! @todo implement more styling;
//!       use XlsxColorStyle::automatic, XlsxColorStyle::indexed, XlsxColorStyle::theme, XlsxColorStyle::tint...
    const XlsxColorStyle* realBackgroundColor = this->realBackgroundColor();
    if (realBackgroundColor) {
kDebug() << patternType << realBackgroundColor->rgb;
        if (realBackgroundColor->rgb.isValid()) {
            cellStyle->addProperty("fo:background-color", realBackgroundColor->rgb.name());
        }
    }
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
    if (color.rgb.isValid()) {
        cellStyle->addProperty("fo:color", color.rgb.name(), KoGenStyle::TextType);
    }
    //! @todo implement more styling
}

XlsxStyles::XlsxStyles()
{
}

XlsxStyles::~XlsxStyles()
{
    for (int i = 0; i < fontStyles.size(); i++)
        delete fontStyles[i];
    for (int i = 0; i < fillStyles.size(); i++)
        delete fillStyles[i];
    for (int i = 0; i < cellFormats.size(); i++)
        delete cellFormats[i];
}

void XlsxStyles::setCellFormat(XlsxCellFormat *format, int cellFormatIndex)
{
//    format->styles = this;
    delete cellFormats[cellFormatIndex];
    cellFormats[cellFormatIndex] = format;
}

//----------------------------------------------------------
//! default values based on Annex A, p. 4476
XlsxCellFormat::XlsxCellFormat()
 : applyAlignment(true), applyBorder(true), applyFill(true), applyFont(true),
   applyNumberFormat(true), applyProtection(true),
   borderId(-1), fillId(-1), fontId(-1), numFmtId(-1),
   pivotButton(false), quotePrefix(false), xfId(-1),
   horizontalAlignment(GeneralHorizontalAlignment),
   verticalAlignment(NoVerticalAlignment)
{
}

XlsxCellFormat::~XlsxCellFormat()
{
}

bool XlsxCellFormat::setupCharacterStyle(const XlsxStyles *styles, KoCharacterStyle* characterStyle) const
{
    XlsxFontStyle* fontStyle = styles->fontStyle(fontId);
    if (!fontStyle) {
        kDebug() << "No font with ID:" << fontId;
        return false;
    }
    fontStyle->setupCharacterStyle(characterStyle);
    return true;
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

//! CASE #S1600
void XlsxCellFormat::setupCellStyleAlignment(KoGenStyle* cellStyle) const
{
//! @todo FillHorizontalAlignment, JustifyHorizontalAlignment
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

//! See http://www.w3.org/TR/2001/REC-xsl-20011015/slice7.html#text-align
bool XlsxCellFormat::setupCellStyle(const XlsxStyles *styles, KoGenStyle* cellStyle) const
{
    if (applyAlignment) {
        setupCellStyleAlignment(cellStyle);
    }
    if (applyFont && fontId >= 0) {
        XlsxFontStyle* fontStyle = styles->fontStyle(fontId);
        if (!fontStyle) {
            kWarning() << "No font with ID:" << fontId;
            return false;
        }
        fontStyle->setupCellTextStyle(cellStyle);
    }
    if (applyFill && fillId >= 0) {
kDebug() << "fillId:" << fillId;
        XlsxFillStyle *fillStyle = styles->fillStyle(fillId);
        if (!fillStyle) {
            kWarning() << "No fill with ID:" << fillId;
            return false;
        }
        fillStyle->setupCellStyle(cellStyle);
    }
    return true;
}

//----------------------------------------------------------

XlsxXmlStylesReaderContext::XlsxXmlStylesReaderContext(XlsxStyles& _styles)
        : styles(&_styles)
{
}

class XlsxXmlStylesReader::Private
{
public:
    Private() {
    }
    ~Private() {
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
    delete m_currentFillStyle;
    delete m_currentCellFormat;
}

void XlsxXmlStylesReader::init()
{
    m_defaultNamespace = "";
    m_fontStyleIndex = 0;
    m_fillStyleIndex = 0;
    m_cellFormatIndex = 0;
    m_currentFontStyle = 0;
    m_currentFillStyle = 0;
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

    QXmlStreamNamespaceDeclarations namespaces(namespaceDeclarations());
    for (int i = 0; i < namespaces.count(); i++) {
        kDebug() << "NS prefix:" << namespaces[i].prefix() << "uri:" << namespaces[i].namespaceUri();
    }
//! @todo find out whether the namespace returned by namespaceUri()
//!       is exactly the same ref as the element of namespaceDeclarations()
    if (!namespaces.contains(QXmlStreamNamespaceDeclaration(QString(), MSOOXML::Schemas::spreadsheetml))) {
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
 - [done] borders (Borders) §18.8.5
 - cellStyles (Cell Styles) §18.8.8
 - cellStyleXfs (Formatting Records) §18.8.9
 - [done] cellXfs (Cell Formats) §18.8.10
 - colors (Colors) §18.8.11
 - dxfs (Formats) §18.8.15
 - extLst (Future Feature Data Storage Area) §18.2.10
 - [done] fills (Fills) §18.8.21
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
            TRY_READ_IF(fills)
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
    if (!m_context->styles->fontStyles.isEmpty()) {
        ERROR_UNEXPECTED_SECOND_OCCURENCE(CURRENT_EL);
    }
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(count)
    uint countNumber;
    STRING_TO_INT(count, countNumber, "styleSheet/fonts@count")
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
    MSOOXML::Utils::AutoPtrSetter<XlsxFontStyle> currentFontStyleSetter(m_currentFontStyle);

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
            ELSE_TRY_READ_IF(color)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    READ_EPILOGUE_WITHOUT_RETURN

    currentFontStyleSetter.release();
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
    const QXmlStreamAttributes attrs(attributes());
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
    const QXmlStreamAttributes attrs(attributes());
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
    const QXmlStreamAttributes attrs(attributes());
    m_currentFontStyle->bold = readBooleanAttr("val", true);

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
    const QXmlStreamAttributes attrs(attributes());
    m_currentFontStyle->italic = readBooleanAttr("val", true);

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
    const QXmlStreamAttributes attrs(attributes());
    m_currentFontStyle->strike = readBooleanAttr("val", true);

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
    const QXmlStreamAttributes attrs(attributes());
    m_currentFontStyle->setUnderline(attrs.value("val").toString());

    while (true) {
        BREAK_IF_END_OF(CURRENT_EL);
        readNext();
        break;
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL color
//! color handler (Data Bar Color)
/*! ECMA-376, 18.3.1.15, p. 1780.
 One of the colors associated with the data bar or color scale.
 The auto attribute shall not be used in the context of data bars.

 Child elements:
 - stop (Gradient Stop) §18.8.38
 Parent elements:
 - bottom (§18.8.6)
 - colorScale (§18.3.1.16)
 - dataBar (§18.3.1.28)
 - diagonal (§18.8.13)
 - end (§18.8.16)
 - [done] font (§18.8.22)
 - horizontal (§18.8.25)
 - mruColors (§18.8.28)
 - rPr (§18.4.7)
 - start (§18.8.37)
 - stop (§18.8.38)
 - top (§18.8.43)
 - vertical (§18.8.44)

 @todo support all elements
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_color()
{
    Q_ASSERT(m_currentFontStyle);

    READ_PROLOGUE
    RETURN_IF_ERROR( readColorStyle(&m_currentFontStyle->color, "color") )
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
    if (!m_context->styles->cellFormats.isEmpty()) {
        ERROR_UNEXPECTED_SECOND_OCCURENCE(CURRENT_EL);
    }
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(count)
    uint countNumber;
    STRING_TO_INT(count, countNumber, "styleSheet/cellXfs@count")
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

 Attributes:
 - applyAlignment
 - applyBorder
 - applyFill
 - applyFont
 - applyNumberFormat
 - applyProtection
 - borderId
 - fillId
 - [done] fontId
 - numFmtId
 - pivotButton
 - quotePrefix
 - xfId

 @todo support all attributes and elements
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_xf()
{
    READ_PROLOGUE

    if (m_cellFormatIndex >= (uint)m_context->styles->cellFormats.size()) {
        raiseError(i18n("Declared number of cell formats too small (%1)", m_context->styles->cellFormats.size()));
        return KoFilter::WrongFormat;
    }

    kDebug() << "cell format #" << m_cellFormatIndex;
    m_currentCellFormat = new XlsxCellFormat;
    MSOOXML::Utils::AutoPtrSetter<XlsxCellFormat> currentCellFormatSetter(m_currentCellFormat);

    // -- read attrs --
    const QXmlStreamAttributes attrs(attributes());
    m_currentCellFormat->applyAlignment = readBooleanAttr("applyAlignment", true);
    m_currentCellFormat->applyBorder = readBooleanAttr("applyBorder", true);
    m_currentCellFormat->applyFill = readBooleanAttr("applyFill", true);
    m_currentCellFormat->applyFont = readBooleanAttr("applyFont", true);
    m_currentCellFormat->applyNumberFormat = readBooleanAttr("applyNumberFormat", true);
    m_currentCellFormat->applyProtection = readBooleanAttr("applyProtection", true);

    TRY_READ_ATTR_WITHOUT_NS(borderId)
    STRING_TO_INT(borderId, m_currentCellFormat->borderId, "xf@borderId")

    TRY_READ_ATTR_WITHOUT_NS(fillId)
kDebug() << "fillId:" << fillId;
    STRING_TO_INT(fillId, m_currentCellFormat->fillId, "xf@fillId")

    TRY_READ_ATTR_WITHOUT_NS(fontId)
    STRING_TO_INT(fontId, m_currentCellFormat->fontId, "xf@fontId")

    TRY_READ_ATTR_WITHOUT_NS(numFmtId)
    STRING_TO_INT(numFmtId, m_currentCellFormat->numFmtId, "xf@numFmtId")

    m_currentCellFormat->pivotButton = readBooleanAttr("pivotButton");
    m_currentCellFormat->quotePrefix = readBooleanAttr("quotePrefix");

    TRY_READ_ATTR_WITHOUT_NS(xfId)
    STRING_TO_INT(xfId, m_currentCellFormat->xfId, "xf@xfId")

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

    currentCellFormatSetter.release();
    m_context->styles->setCellFormat(m_currentCellFormat, m_cellFormatIndex);
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
    const QXmlStreamAttributes attrs(attributes());
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

#undef CURRENT_EL
#define CURRENT_EL fills
//! fills handler (Fills)
/*! ECMA-376, 18.8.21, p. 1963.
 This element defines the cell fills portion of the Styles part, consisting of a sequence of fill records.
 A cell fill consists of a background color, foreground color, and pattern to be applied across the cell.

 Child elements:
 - [done] fill (Fill) §18.8.20
 Parent elements:
 - [done] styleSheet (§18.8.39)
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_fills()
{
    READ_PROLOGUE
    if (!m_context->styles->fillStyles.isEmpty()) {
        ERROR_UNEXPECTED_SECOND_OCCURENCE(CURRENT_EL);
    }
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(count)
    uint countNumber;
    STRING_TO_INT(count, countNumber, "styleSheet/fills@count")
    m_context->styles->fillStyles.resize(countNumber);
    m_fillStyleIndex = 0;

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(fill)
            ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL fill
//! fill handler (Fill)
/*! ECMA-376, 18.8.20, p. 1962.
 This element specifies fill formatting.

 Child elements:
 - gradientFill (Gradient) §18.8.24
 - [done] patternFill (Pattern) §18.8.32
 Parent elements:
 - dxf (§18.8.14)
 - [done] fills (§18.8.21)
 - ndxf (§18.11.1.4)
 - odxf (§18.11.1.6)
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_fill()
{
    READ_PROLOGUE

    if (m_fillStyleIndex >= (uint)m_context->styles->fillStyles.size()) {
        raiseError(i18n("Declared number of fill styles too small (%1)", m_context->styles->fillStyles.size()));
        return KoFilter::WrongFormat;
    }

    kDebug() << "fill #" << m_fillStyleIndex;
    m_currentFillStyle = new XlsxFillStyle;
    MSOOXML::Utils::AutoPtrSetter<XlsxFillStyle> currentFillStyleSetter(m_currentFillStyle);

    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            TRY_READ_IF(gradientFill)
            ELSE_TRY_READ_IF(patternFill)
            ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE_WITHOUT_RETURN

    currentFillStyleSetter.release();
    m_context->styles->fillStyles[m_fillStyleIndex] = m_currentFillStyle;
    m_currentFillStyle = 0;
    m_fillStyleIndex++;

    return KoFilter::OK;
}

//! Handles patternType
void XlsxXmlStylesReader::handlePatternType(const QString& patternType)
{
    const QByteArray p(patternType.toLatin1());
kDebug() << p;
    if (p.isEmpty() || p == MsooXmlReader::constNone) {
        // 100% background
        m_currentFillStyle->patternType = XlsxFillStyle::NonePatternType;
    }
    else if (p == "solid") {
        // 100% foreground
        m_currentFillStyle->patternType = XlsxFillStyle::SolidPatternType;
    }
    else if (p.startsWith("dark")) {
        if (p == "darkDown") {
        }
        else if (p == "darkGray") {
        }
        else if (p == "darkGrid") {
        }
        else if (p == "darkHorizontal") {
        }
        else if (p == "darkTrellis") {
        }
        else if (p == "darkUp") {
        }
        else if (p == "darkVertical") {
        }
    }
    else if (p.startsWith("light")) {
        if (p == "lightDown") {
        }
        else if (p == "lightGray") {
        }
        else if (p == "lightGrid") {
        }
        else if (p == "lightHorizontal") {
        }
        else if (p == "lightTrellis") {
        }
        else if (p == "lightUp") {
        }
        else if (p == "lightVertical") {
        }
    }
    else if (p == "mediumGray") {
    }
    else if (p == "gray0625") {
    }
    else if (p == "gray125") {
    }
    else {
        kWarning() << "unknown value" << p
            << "of patterFill@patternType; defaulting to \"none\"";
        m_currentFillStyle->patternType = XlsxFillStyle::NonePatternType;
    }
}

#undef CURRENT_EL
#define CURRENT_EL patternFill
//! patternFill handler (Pattern)
/*! ECMA-376, 18.8.22, p. 1990.
 This element is used to specify cell fill information for pattern and solid color cell fills.

 Child elements:
 - [done] bgColor (Background Color) §18.8.3
 - [done] fgColor (Foreground Color) §18.8.19
 Parent elements:
 - [done] fill (§18.8.20)
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_patternFill()
{
    Q_ASSERT(m_currentFillStyle);
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR_WITHOUT_NS(patternType)
    handlePatternType(patternType);

    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            TRY_READ_IF(bgColor)
            ELSE_TRY_READ_IF(fgColor)
            ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

// private
QColor XlsxXmlStylesReader::readRgbAttribute(const QXmlStreamAttributes& attrs) const
{
    TRY_READ_ATTR_WITHOUT_NS(rgb)
    if (rgb.isEmpty())
        return QColor();
    return ST_UnsignedIntHex_to_QColor(rgb);
}

KoFilter::ConversionStatus XlsxXmlStylesReader::readColorStyle(
    XlsxColorStyle* colorStyle, const char* debugElement)
{
    const QXmlStreamAttributes attrs(attributes());
    colorStyle->automatic = readBooleanAttr("auto", false);
    TRY_READ_ATTR_WITHOUT_NS(indexed)
    STRING_TO_INT(indexed, colorStyle->indexed, QLatin1String(debugElement) + "@indexed")
    colorStyle->rgb = readRgbAttribute(attrs);
 //! @todo theme
 //! @todo tint
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL bgColor
//! bgColor handler (Background Color)
/*! ECMA-376, 18.8.3, p. 1948.
 Background color of the cell fill pattern. Cell fill patterns operate with two colors:
 a background color and a foreground color. These combine together to make a patterned cell fill.

 No child elements.
 Parent elements:
 - [done] patternFill (§18.8.20)

 @todo support all elements
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_bgColor()
{
    Q_ASSERT(m_currentFillStyle);
    READ_PROLOGUE
    RETURN_IF_ERROR( readColorStyle(&m_currentFillStyle->bgColor, "bgColor") )

    while (true) {
        BREAK_IF_END_OF(CURRENT_EL);
        readNext();
        break;
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL fgColor
//! fgColor handler (Foreground  Color)
/*! ECMA-376, 18.8.19, p. 1961.
 Foreground color of the cell fill pattern. Cell fill patterns operate with two colors:
 a background color and a foreground color. These combine together to make a patterned cell fill.

 No child elements.
 Parent elements:
 - [done] patternFill (§18.8.20)

 @todo support all elements
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_fgColor()
{
    READ_PROLOGUE
    RETURN_IF_ERROR( readColorStyle(&m_currentFillStyle->fgColor, "fgColor") )

    while (true) {
        BREAK_IF_END_OF(CURRENT_EL);
        readNext();
        break;
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL gradientFill
//! gradientFill handler (Pattern)
/*! ECMA-376, 18.8.24, p. 1965.
 This element defines a gradient-style cell fill.
 Gradient cell fills can use one or two colors as the end points of color interpolation.

 Child elements:
 - stop (Gradient Stop) §18.8.38
 Parent elements:
 - [done] fill (§18.8.20)

 @todo support all elements
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_gradientFill()
{
    Q_ASSERT(m_currentFillStyle);
    READ_PROLOGUE

//! @todo read attributes for gradientFill

    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
//! @todo            TRY_READ_IF(stop)
//todo            ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}
