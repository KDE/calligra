/*
 * This file is part of Office 2007 Filters for Calligra
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

#include "XlsxUtils.h"
#include "XlsxXmlStylesReader.h"
#include "XlsxImport.h"

#include "NumberFormatParser.h"

#include <MsooXmlSchemas.h>
#include <MsooXmlUtils.h>
#include <KoXmlWriter.h>
#include <styles/KoCharacterStyle.h>
#include <KoGenStyles.h>

#undef MSOOXML_CURRENT_NS
#define MSOOXML_CURRENT_CLASS XlsxXmlStylesReader
#define BIND_READ_CLASS MSOOXML_CURRENT_CLASS

#include <MsooXmlReader_p.h>

#include <QMap>
#include <QGlobalStatic>

#include <math.h>

//----------------------------------------------------------

//! @see http://www.w3.org/TR/CSS2/box.html#value-def-border-style
//! @see http://www.w3.org/TR/CSS2/box.html#value-def-border-width
KoFilter::ConversionStatus XlsxXmlStylesReader::readAttributes(const QXmlStreamAttributes& attrs, QString& borderStyle)
{
    QString s;
//! @todo more styles
    TRY_READ_ATTR_WITHOUT_NS_INTO(style, s)
    if (s == QLatin1String("dashed") || s == QLatin1String("dotted") || s == QLatin1String("double")) {
        borderStyle = s;
    }
    else if (s == QLatin1String("medium") || s == QLatin1String("thick") || s == QLatin1String("thin"))
    {
        borderStyle = s + " solid";
    }
    else if (s == QLatin1String("none")) {
        borderStyle = QLatin1String("hidden");
    }
    else if (!s.isEmpty()) {
        borderStyle = QLatin1String("solid"); // fallback
    }
    qCDebug(lcXlsxImport) << "style:" << s << "set to:" << borderStyle;
    return KoFilter::OK;
}

static QColor applyPatternDensity(const QColor& bg, const QColor& fg, qreal percent)
{
    QColor result;

    if (bg.isValid()) {
        result = QColor( bg.red() * percent,
                         bg.green() * percent,
                         bg.blue() * percent,
                         bg.alpha() );
    }
    if (fg.isValid()) {
        result = QColor( result.red()   + fg.red() * ( 1.0 - percent ),
                         result.green() + fg.green() * ( 1.0 - percent ),
                         result.blue()  + fg.blue() * ( 1.0 - percent ),
                         bg.isValid() ? bg.alpha() : fg.alpha() );
    }

    return result;
}

//----------------------------------------------------------

XlsxStyles::XlsxStyles()
{
    // fill the default number formats
    // from Office Open XML Part 4 - Markup Language Reference, p. 1974
    numberFormatStrings[ 1 ] = QLatin1String( "0" );
    numberFormatStrings[ 2 ] = QLatin1String( "0.00" );
    numberFormatStrings[ 3 ] = QLatin1String( "#,##0" );
    numberFormatStrings[ 4 ] = QLatin1String( "#,##0.00" );
    numberFormatStrings[ 9 ] = QLatin1String( "0%" );
    numberFormatStrings[ 10 ] = QLatin1String( "0.00%" );
    numberFormatStrings[ 11 ] = QLatin1String( "0.00E+00" );
    numberFormatStrings[ 12 ] = QLatin1String( "# ?/?" );
    numberFormatStrings[ 13 ] = QLatin1String( "# \?\?/\?\?" );
    numberFormatStrings[ 14 ] = QLatin1String( "mm-dd-yy" );
    numberFormatStrings[ 15 ] = QLatin1String( "d-mmm-yy" );
    numberFormatStrings[ 16 ] = QLatin1String( "d-mmm" );
    numberFormatStrings[ 17 ] = QLatin1String( "mmm-yy" );
    numberFormatStrings[ 18 ] = QLatin1String( "h:mm AM/PM" );
    numberFormatStrings[ 19 ] = QLatin1String( "h:mm:ss AM/PM" );
    numberFormatStrings[ 20 ] = QLatin1String( "h:mm" );
    numberFormatStrings[ 21 ] = QLatin1String( "h:mm:ss" );
    numberFormatStrings[ 22 ] = QLatin1String( "m/d/yy h:mm" );
    numberFormatStrings[ 37 ] = QLatin1String( "#,##0 ;(#,##0)" );
    numberFormatStrings[ 38 ] = QLatin1String( "#,##0 ;[Red](#,##0)" );
    numberFormatStrings[ 39 ] = QLatin1String( "#,##0.00;(#,##0.00)" );
    numberFormatStrings[ 40 ] = QLatin1String( "#,##0.00;[Red](#,##0.00)" );
    numberFormatStrings[ 45 ] = QLatin1String( "mm:ss" );
    numberFormatStrings[ 46 ] = QLatin1String( "[h]:mm:ss" );
    numberFormatStrings[ 47 ] = QLatin1String( "mmss.0" );
    numberFormatStrings[ 48 ] = QLatin1String( "##0.0E+0" );
    numberFormatStrings[ 49 ] = QLatin1String( "@" );
}

XlsxStyles::~XlsxStyles()
{
    for (int i = 0; i < fontStyles.size(); i++)
        delete fontStyles[i];
    for (int i = 0; i < fillStyles.size(); i++)
        delete fillStyles[i];
    for (int i = 0; i < cellFormats.size(); i++)
        delete cellFormats[i];
    for (int i = 0; i < borderStyles.size(); i++)
        delete borderStyles[i];
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
   verticalAlignment(NoVerticalAlignment),
   wrapText(false),
   shrinkToFit(false),
   textRotation(0)
{
}

XlsxCellFormat::~XlsxCellFormat()
{
}

class ST_HorizontalAlignment_fromStringMap : public QMap<QString, XlsxCellFormat::ST_HorizontalAlignment>
{
public:
    ST_HorizontalAlignment_fromStringMap() {
        insert(QString(), XlsxCellFormat::GeneralHorizontalAlignment);
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

Q_GLOBAL_STATIC(ST_HorizontalAlignment_fromStringMap, s_ST_HorizontalAlignmentValues)

void XlsxCellFormat::setHorizontalAlignment(const QString& alignment)
{
    horizontalAlignment = s_ST_HorizontalAlignmentValues->value(alignment);
}

class ST_VerticalAlignment_fromStringMap : public QMap<QString, XlsxCellFormat::ST_VerticalAlignment>
{
public:
    ST_VerticalAlignment_fromStringMap() {
        insert(QString(), XlsxCellFormat::NoVerticalAlignment);
        insert(QLatin1String("bottom"), XlsxCellFormat::BottomVerticalAlignment);
        insert(QLatin1String("center"), XlsxCellFormat::CenterVerticalAlignment);
        insert(QLatin1String("distributed"), XlsxCellFormat::DistributedVerticalAlignment);
        insert(QLatin1String("justify"), XlsxCellFormat::JustifyVerticalAlignment);
        insert(QLatin1String("top"), XlsxCellFormat::TopVerticalAlignment);
    }
};

Q_GLOBAL_STATIC(ST_VerticalAlignment_fromStringMap, s_ST_VerticalAlignmentValues)

void XlsxCellFormat::setVerticalAlignment(const QString& alignment)
{
    verticalAlignment = s_ST_VerticalAlignmentValues->value(alignment);
}

//! CASE #S1600
void XlsxCellFormat::setupCellStyleAlignment(KoGenStyle* cellStyle) const
{
//! @todo FillHorizontalAlignment, JustifyHorizontalAlignment
    int wrapOption = -1; // "don't know"
    if (wrapText)
        wrapOption = 1;
    //special case: 255 indicates vertical rotation without rotated characters (I couldn't find that documented though)
    const bool verticalTtb = textRotation == 255;
    if (verticalTtb)
        cellStyle->addProperty("style:direction", "ttb");
    else if (textRotation != 0) {
        unsigned angle = textRotation;
        if (angle > 90) angle = 360 - (angle - 90);
        cellStyle->addProperty("style:rotation-angle", QString::number(angle));
    }

    if (shrinkToFit)
        cellStyle->addProperty("style:shrink-to-fit", "true");

    switch (horizontalAlignment) {
    case CenterHorizontalAlignment:
    case CenterContinuousHorizontalAlignment:
    case DistributedHorizontalAlignment:
        cellStyle->addProperty("fo:text-align", "center", KoGenStyle::ParagraphType);
        if (horizontalAlignment == DistributedHorizontalAlignment)
            wrapOption = 1;
        break;
    case GeneralHorizontalAlignment: // ok?
        if (verticalTtb) // Excel centers vertical text by default, so mimic that
            cellStyle->addProperty("fo:text-align", "center", KoGenStyle::ParagraphType);
        if (textRotation > 90 && textRotation < 180) // Excel right aligns rotated text for some angles
            cellStyle->addProperty("fo:text-align", "end", KoGenStyle::ParagraphType);
        break;
    case LeftHorizontalAlignment:
        cellStyle->addProperty("fo:text-align", "start", KoGenStyle::ParagraphType);
        break;
    case RightHorizontalAlignment:
        cellStyle->addProperty("fo:text-align", "end", KoGenStyle::ParagraphType);
        break;
    case JustifyHorizontalAlignment:
        cellStyle->addProperty("fo:text-align", "justify", KoGenStyle::ParagraphType);
        wrapOption = 1;
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
    case JustifyVerticalAlignment: // ok?
    case DistributedVerticalAlignment:
        cellStyle->addProperty("style:vertical-align", "top");
        cellStyle->addProperty("calligra:vertical-distributed", "distributed");
        wrapOption = 1;
        break;
    case NoVerticalAlignment:
    case BottomVerticalAlignment:
        cellStyle->addProperty("style:vertical-align", "bottom");
        break;
    default:;
    }

    if (wrapOption == 0 || wrapOption == 1)
        cellStyle->addProperty("fo:wrap-option", wrapOption ? "wrap" : "no-wrap");
}

//! See http://www.w3.org/TR/2001/REC-xsl-20011015/slice7.html#text-align
bool XlsxCellFormat::setupCellStyle(
    const XlsxStyles *styles,
    KoGenStyle* cellStyle) const
{
    qCDebug(lcXlsxImport) << "fontId:" << fontId << "fillId:" << fillId << "borderId:" << borderId;
    if (applyAlignment) {
        setupCellStyleAlignment(cellStyle);
    }
    if (applyFont && fontId >= 0) {
        KoGenStyle* fontStyle = styles->fontStyle(fontId);
        if (!fontStyle) {
            qCWarning(lcXlsxImport) << "No font with ID:" << fontId;
            return false;
        }
        KoGenStyle::copyPropertiesFromStyle(*fontStyle, *cellStyle, KoGenStyle::TextType);
    }
    if (applyFill && fillId >= 0) {
        KoGenStyle *fillStyle = styles->fillStyle(fillId);
        if (!fillStyle) {
            qCWarning(lcXlsxImport) << "No fill with ID:" << fillId;
            return false;
        }
        KoGenStyle::copyPropertiesFromStyle(*fillStyle, *cellStyle, KoGenStyle::TableCellType);
    }
    if (applyBorder && borderId >= 0) {
        KoGenStyle *borderStyle = styles->borderStyle(borderId);
        if (borderStyle) {
            KoGenStyle::copyPropertiesFromStyle(*borderStyle, *cellStyle, KoGenStyle::TableCellType);
        }
    }
    return true;
}

//----------------------------------------------------------

XlsxXmlStylesReaderContext::XlsxXmlStylesReaderContext(XlsxStyles& _styles, bool _skipFirstPart,
    XlsxImport* _import, MSOOXML::DrawingMLTheme* _themes)
        : styles(&_styles), skipFirstPart(_skipFirstPart), import(_import), themes(_themes)
{
    // This is default array of colors from the spec
    colorIndices.push_back("000000");
    colorIndices.push_back("FFFFFF");
    colorIndices.push_back("FF0000");
    colorIndices.push_back("00FF00");
    colorIndices.push_back("0000FF");
    colorIndices.push_back("FFFF00");
    colorIndices.push_back("FF00FF");
    colorIndices.push_back("00FFFF");
    colorIndices.push_back("000000");
    colorIndices.push_back("FFFFFF");
    colorIndices.push_back("FF0000");
    colorIndices.push_back("00FF00");
    colorIndices.push_back("0000FF");
    colorIndices.push_back("FFFF00");
    colorIndices.push_back("FF00FF");
    colorIndices.push_back("00FFFF");
    colorIndices.push_back("800000");
    colorIndices.push_back("008000");
    colorIndices.push_back("000080");
    colorIndices.push_back("808000");
    colorIndices.push_back("800080");
    colorIndices.push_back("008080");
    colorIndices.push_back("C0C0C0");
    colorIndices.push_back("808080");
    colorIndices.push_back("9999FF");
    colorIndices.push_back("993366");
    colorIndices.push_back("FFFFCC");
    colorIndices.push_back("CCFFFF");
    colorIndices.push_back("660066");
    colorIndices.push_back("FF8080");
    colorIndices.push_back("0066CC");
    colorIndices.push_back("CCCCFF");
    colorIndices.push_back("000080");
    colorIndices.push_back("FF00FF");
    colorIndices.push_back("FFFF00");
    colorIndices.push_back("00FFFF");
    colorIndices.push_back("800080");
    colorIndices.push_back("800000");
    colorIndices.push_back("008080");
    colorIndices.push_back("0000FF");
    colorIndices.push_back("00CCFF");
    colorIndices.push_back("CCFFFF");
    colorIndices.push_back("CCFFCC");
    colorIndices.push_back("FFFF99");
    colorIndices.push_back("99CCFF");
    colorIndices.push_back("FF99CC");
    colorIndices.push_back("CC99FF");
    colorIndices.push_back("FFCC99");
    colorIndices.push_back("3366FF");
    colorIndices.push_back("33CCCC");
    colorIndices.push_back("99CC00");
    colorIndices.push_back("FFCC00");
    colorIndices.push_back("FF9900");
    colorIndices.push_back("FF6600");
    colorIndices.push_back("666699");
    colorIndices.push_back("969696");
    colorIndices.push_back("003366");
    colorIndices.push_back("339966");
    colorIndices.push_back("003300");
    colorIndices.push_back("333300");
    colorIndices.push_back("993300");
    colorIndices.push_back("993366");
    colorIndices.push_back("333399");
    colorIndices.push_back("333333");
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
        : XlsxXmlCommonReader(writers)
        , m_context(0)
        , d(new Private)
{
    init();
}

XlsxXmlStylesReader::~XlsxXmlStylesReader()
{
    delete d;
}

void XlsxXmlStylesReader::init()
{
    m_defaultNamespace = "";
    m_cellFormatIndex = 0;
    m_currentFontStyle = 0;
    m_currentFillStyle = 0;
    m_currentCellFormat = 0;
}

KoFilter::ConversionStatus XlsxXmlStylesReader::read(MSOOXML::MsooXmlReaderContext* context)
{
    m_context = dynamic_cast<XlsxXmlStylesReaderContext*>(context);
    Q_ASSERT(m_context);
    m_colorIndices = m_context->colorIndices;
    m_themes = m_context->themes;
    const KoFilter::ConversionStatus result = readInternal();
    m_context = 0;
    if (result == KoFilter::OK)
        return KoFilter::OK;
    return result;
}

KoFilter::ConversionStatus XlsxXmlStylesReader::readInternal()
{
    qCDebug(lcXlsxImport) << "=============================";
    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }

    // sst
    readNext();
    qCDebug(lcXlsxImport) << *this << namespaceUri();

    if (!expectEl("styleSheet")) {
        return KoFilter::WrongFormat;
    }
    if (!expectNS(MSOOXML::Schemas::spreadsheetml)) {
        return KoFilter::WrongFormat;
    }

    QXmlStreamNamespaceDeclarations namespaces(namespaceDeclarations());
    for (int i = 0; i < namespaces.count(); i++) {
        qCDebug(lcXlsxImport) << "NS prefix:" << namespaces[i].prefix() << "uri:" << namespaces[i].namespaceUri();
    }
//! @todo find out whether the namespace returned by namespaceUri()
//!       is exactly the same ref as the element of namespaceDeclarations()
    if (!namespaces.contains(QXmlStreamNamespaceDeclaration(QString(), MSOOXML::Schemas::spreadsheetml))) {
        raiseError(i18n("Namespace \"%1\" not found", QLatin1String(MSOOXML::Schemas::spreadsheetml)));
        return KoFilter::WrongFormat;
    }
//! @todo expect other namespaces too...

    // Read the style-sheet.
    TRY_READ(styleSheet)

    // Once finished reading the style-sheets we can translate the MSExcel number-formattings
    // to ODF number-formattings. We do this here cause m_context->styles->numberFormatStrings
    // is filled now, not allowed to change from here on and it greatly improves the performance
    // if we do the whole conversation only once right now and use the result later.
    for(QMap< int, QString >::ConstIterator it = m_context->styles->numberFormatStrings.constBegin(); it != m_context->styles->numberFormatStrings.constEnd(); ++it) {
        const KoGenStyle style = NumberFormatParser::parse( it.value(), mainStyles );
        //m_context->styles->numberFormatStyles[ it.key() ] = style;
        if ( style.type() != KoGenStyle::ParagraphAutoStyle ) {
            QString styleName = mainStyles->insert( style, "N" );
            m_context->styles->numberFormatStyleNames[ it.key() ] = styleName;
        }
    }

    qCDebug(lcXlsxImport) << "===========finished============";
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
 - [done] colors (Colors) §18.8.11
 - [done] dxfs (Formats) §18.8.15
 - extLst (Future Feature Data Storage Area) §18.2.10
 - [done] fills (Fills) §18.8.21
 - [done] fonts (Fonts) §18.8.23
 - [done] numFmts (Number Formats) §18.8.31
 - tableStyles (Table Styles) §18.8.42
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_styleSheet()
{
    READ_PROLOGUE

    int counter = 0;
    while (!atEnd()) {
        readNext();
        qCDebug(lcXlsxImport) << *this;
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            // In the first round we read potential color overrides
            if (m_context->skipFirstPart) {
                TRY_READ_IF(colors)
                SKIP_UNKNOWN
            }
            else {
                if (counter == 40) {
                    // set the progress by the position of what was read
                    qreal progress = 5 + 25 * device()->pos() / device()->size();
                    m_context->import->reportProgress(progress);
                    counter = 0;
                }
                ++counter;
                TRY_READ_IF(fonts)
                ELSE_TRY_READ_IF(fills)
                ELSE_TRY_READ_IF(numFmts)
                ELSE_TRY_READ_IF(cellXfs)
                ELSE_TRY_READ_IF(borders)
                ELSE_TRY_READ_IF(dxfs)
                SKIP_UNKNOWN
            }
//! @todo add ELSE_WRONG_FORMAT
        }
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
        raiseUnexpectedSecondOccurenceOfElError(STRINGIFY(CURRENT_EL));
    }
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(count)
    uint countNumber = 0;
    STRING_TO_INT(count, countNumber, "styleSheet/fonts@count")
    m_context->styles->fontStyles.resize(countNumber);
    uint fontStyleIndex = 0;

    while (!atEnd()) {
        readNext();
        qCDebug(lcXlsxImport) << *this;
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            if (QUALIFIED_NAME_IS(font)) {
                m_currentFontStyle = new KoGenStyle(KoGenStyle::TextAutoStyle, "text");
                if (fontStyleIndex >= (uint)m_context->styles->fontStyles.size()) {
                    raiseError(i18n("Declared number of font styles too small (%1)", m_context->styles->fontStyles.size()));
                    return KoFilter::WrongFormat;
                }
                TRY_READ(font)
                m_context->styles->fontStyles[fontStyleIndex] = m_currentFontStyle;
                m_currentFontStyle = 0;
                fontStyleIndex++;
            }
            ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL numFmts
//! numFmts handler (Number formats)
/*! ECMA-376, 18.8.31, p. 1981.
 This element defines the number formats in this workbook, consisting of a sequence of numFmt records,
 where each numFmt record defines a particular number format, indicating how to format and render
 the numeric value of a cell.

 Child elements:
 - [done] numFmt (Format Definition) §18.8.30
 Parent elements:
 - [done] styleSheet (§18.8.39)
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_numFmts()
{
    READ_PROLOGUE
#if 0
    //this code just reads the count out of the attributes and convert it to a integer. 
    //it is currently not needed, but maybe we will need it later on.
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS( count )
    int countNumber = 0;
    STRING_TO_INT( count, countNumber, "styleSheet/numFmts@count" );
#endif

    while( !atEnd() )
    {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if( isStartElement() )
        {
            TRY_READ_IF( numFmt )
            ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL numFmt
//! numFmt handler (Number format)
/*! ECMA-376, 18.8.30, p. 1973.
 This element specifies number format properties which indicate how to format and render
 the numeric value of a cell.

 No child elements.

 Parent elements:
 - dxf (§18.8.14)
 - ndxf (§18.11.1.4)
 - [done] numFmts (§18.8.31)
 - odxf (§18.11.1.6)
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_numFmt()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS( numFmtId )
    int id = -1;
    STRING_TO_INT(numFmtId, id, "numFmt@numFmtId")

    TRY_READ_ATTR_WITHOUT_NS( formatCode );
    if (id >= 0 && !formatCode.isEmpty()) {
        m_context->styles->numberFormatStrings[ id ] = formatCode;
    }

    readNext();
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
 - [done] color (Data Bar Color) §18.3.1.15
 - condense (Condense) §18.8.12
 - extend (Extend) §18.8.17
 - family (Font Family) §18.8.18
 - [done] i (Italic) §18.8.26
 - [done] name (Font Name) §18.8.29
 - [done] outline (Outline) §18.4.2
 - [done] scheme (Scheme) §18.8.35
 - shadow (Shadow) §18.8.36
 - [done] strike (Strike Through) §18.4.10
 - [done] sz (Font Size) §18.4.11
 - [done] u (Underline) §18.4.13
 - [done] vertAlign (Vertical Alignment) §18.4.14

 Parent elements:
 - [done] dxf (§18.8.14)
 - [done] fonts (§18.8.23)
 - ndxf (§18.11.1.4)
 - odxf (§18.11.1.6)

 @todo support all child elements
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_font()
{
    READ_PROLOGUE

    MSOOXML::Utils::AutoPtrSetter<KoGenStyle> currentFontStyleSetter(m_currentFontStyle);

    m_currentTextStyleProperties = new KoCharacterStyle;

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(sz)
            ELSE_TRY_READ_IF(name)
            ELSE_TRY_READ_IF(b)
            ELSE_TRY_READ_IF(i)
            else if (name() == "color") {
                m_currentColor = QColor();
                TRY_READ(color)
                if (m_currentColor.isValid()) {
                    m_currentTextStyleProperties->setForeground(QBrush(m_currentColor));
                }
            }
            ELSE_TRY_READ_IF(color)
            ELSE_TRY_READ_IF(strike)
            ELSE_TRY_READ_IF(u)
            ELSE_TRY_READ_IF(vertAlign)
            ELSE_TRY_READ_IF(scheme)
            ELSE_TRY_READ_IF(outline)
            SKIP_UNKNOWN
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    m_currentTextStyleProperties->saveOdf(*m_currentFontStyle);
    delete m_currentTextStyleProperties;
    m_currentTextStyleProperties = 0;

    currentFontStyleSetter.release();

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL name
//! name handler (Font Name)
/*! ECMA-376, 18.8.29, p. 1973.
 This element specifies the face name of this font.

 Parent elements:
 - [done] font (§18.8.22)

 Child elements:
 - none
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_name()
{
    Q_ASSERT(m_currentFontStyle);

    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(val)

    if (!val.isEmpty()) {
        m_currentFontStyle->addProperty("fo:font-family", val);
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL dxfs
/*
 Parent elements:
 - [done] styleSheet (§18.8.39)

 Child elements:
 - [done] dxf (Formatting) §18.8.14

*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_dxfs()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(dxf)
            ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL dxf
/*
 Parent elements:
 - [done] dxfs (§18.8.15);
 - rfmt (§18.11.1.17)

 Child elements:
 - [done] alignment (Alignment) §18.8.1
 - [done] border (Border) §18.8.4
 - extLst (Future Feature Data Storage Area) §18.2.10
 - [done] fill (Fill) §18.8.20
 - [done] font (Font) §18.8.22
 - numFmt (Number Format) §18.8.30
 - protection (Protection Properties) §18.8.33

*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_dxf()
{
    READ_PROLOGUE

    KoGenStyle cellStyle(KoGenStyle::TableCellStyle, "table-cell");

    m_currentFontStyle = new KoGenStyle(KoGenStyle::TextAutoStyle, "text");
    m_currentFillStyle = new KoGenStyle(KoGenStyle::TableCellAutoStyle, "table-cell");
    m_currentBorderStyle = new KoGenStyle(KoGenStyle::TableCellAutoStyle, "table-cell");
    m_currentCellFormat = new XlsxCellFormat;

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(font)
            ELSE_TRY_READ_IF(fill)
            ELSE_TRY_READ_IF(border)
            ELSE_TRY_READ_IF(alignment)
            SKIP_UNKNOWN
        }
    }
    KoGenStyle::copyPropertiesFromStyle(*m_currentFontStyle, cellStyle, KoGenStyle::TextType);
    KoGenStyle::copyPropertiesFromStyle(*m_currentFillStyle, cellStyle, KoGenStyle::TableCellType);
    KoGenStyle::copyPropertiesFromStyle(*m_currentBorderStyle, cellStyle, KoGenStyle::TableCellType);
    m_currentCellFormat->setupCellStyleAlignment(&cellStyle);

    m_context->styles->conditionalStyles.insert(m_context->styles->conditionalStyles.size() + 1,
       mainStyles->insert(cellStyle, "ConditionalStyle"));

    delete m_currentFontStyle;
    m_currentFontStyle = 0;
    delete m_currentFillStyle;
    m_currentFillStyle = 0;
    delete m_currentBorderStyle;
    m_currentBorderStyle = 0;
    delete m_currentCellFormat;
    m_currentCellFormat = 0;

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

 @note cellStyleXfs: the standard states that both the cell style xf records and cell xf records
       must be read to understand the full set of formatting applied to a cell.
       In MSO, only the cell xf record defines the formatting applied to a cell.
       See <a href="http://www.documentinteropinitiative.org/implnotes/implementationnotelist.aspx?id=dd2615fe-aa8d-4a06-a415-13389919cf36&specname=ecma-376">here</a>.
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_cellXfs()
{
    READ_PROLOGUE
    if (!m_context->styles->cellFormats.isEmpty()) {
        raiseUnexpectedSecondOccurenceOfElError(STRINGIFY(CURRENT_EL));
    }
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(count)
    uint countNumber = 0;
    STRING_TO_INT(count, countNumber, "styleSheet/cellXfs@count")
    m_context->styles->cellFormats.resize(countNumber);
    m_cellFormatIndex = 0;

    while (!atEnd()) {
        readNext();
        qCDebug(lcXlsxImport) << *this;
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(xf)
            ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL xf
//! xf handler (Format)
/*! ECMA-376, 18.8.45, p. 1999.
 A single xf element describes all of the formatting for a cell.

 Parent elements:
 - cellStyleXfs (§18.8.9)
 - [done] cellXfs (§18.8.10)
 - extLst (Future Feature Data Storage Area) §18.2.10

 Child elements:
 - [done] alignment (Alignment) §18.8.1
 - protection (Protection Properties) §18.8.33

 Attributes:
 - applyAlignment
 - applyBorder
 - applyFill
 - applyFont
 - applyNumberFormat
 - applyProtection
 - [done] borderId
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

    qCDebug(lcXlsxImport) << "cell format #" << m_cellFormatIndex;
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
        qCDebug(lcXlsxImport) << *this;
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(alignment)
            SKIP_UNKNOWN
        }
    }

    currentCellFormatSetter.release();
    m_context->styles->setCellFormat(m_currentCellFormat, m_cellFormatIndex);
    m_currentCellFormat = 0;
    m_cellFormatIndex++;

    READ_EPILOGUE
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
    qCDebug(lcXlsxImport) << "horizontalAlignment:" << m_currentCellFormat->horizontalAlignment;
    m_currentCellFormat->setVerticalAlignment(attrs.value("vertical").toString());
    qCDebug(lcXlsxImport) << "verticalAlignment:" << m_currentCellFormat->verticalAlignment;
    const bool wrap = readBooleanAttr("wrapText", false);
    m_currentCellFormat->wrapText = wrap;
    const bool shrinkToFit = readBooleanAttr("shrinkToFit", false);
    m_currentCellFormat->shrinkToFit = shrinkToFit;
    const uint textRotation = attributes().value("textRotation").toString().toUInt();
    m_currentCellFormat->textRotation = textRotation;

//! @todo more attributes

    readNext();
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
        raiseUnexpectedSecondOccurenceOfElError(STRINGIFY(CURRENT_EL));
    }
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(count)
    uint countNumber = 0;
    STRING_TO_INT(count, countNumber, "styleSheet/fills@count")
    m_context->styles->fillStyles.resize(countNumber);
    uint fillStyleIndex = 0;

    while (!atEnd()) {
        readNext();
        qCDebug(lcXlsxImport) << *this;
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            if (QUALIFIED_NAME_IS(fill)) {
                m_currentFillStyle = new KoGenStyle(KoGenStyle::TableCellAutoStyle, "table-cell");
                if (fillStyleIndex >= (uint)m_context->styles->fillStyles.size()) {
                    raiseError(i18n("Declared number of fill styles too small (%1)", m_context->styles->fillStyles.size()));
                    return KoFilter::WrongFormat;
                }
                TRY_READ(fill)
                m_context->styles->fillStyles[fillStyleIndex] = m_currentFillStyle;
                m_currentFillStyle = 0;
                fillStyleIndex++;
            }
            ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL fill
//! fill handler (Fill)
/*! ECMA-376, 18.8.20, p. 1962.
 This element specifies fill formatting.

 Child elements:
 - [done] gradientFill (Gradient) §18.8.24
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

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(gradientFill)
            ELSE_TRY_READ_IF(patternFill)
            ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
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

    m_currentBgColor = QColor();
    m_currentFgColor = QColor();

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(bgColor)
            ELSE_TRY_READ_IF(fgColor)
            ELSE_WRONG_FORMAT
        }
    }

    qreal percent = 0;

    const QByteArray p(patternType.toLatin1());
    if (p.isEmpty() || p == MsooXmlReader::constNone) {
        // 100% background
        percent = 1;
    }
    else if (p == "solid") {
        // 100% foreground
        percent = 0;
    }
    else if (p.startsWith("dark")) {
        if (p == "darkDown") {
            percent = 0.5;
        }
        else if (p == "darkGray") {
            percent = 0.25;
        }
        else if (p == "darkGrid") {
            percent = 0.5;
        }
        else if (p == "darkHorizontal") {
            percent = 0.5;
        }
        else if (p == "darkTrellis") {
            percent = 0.25;
        }
        else if (p == "darkUp") {
            percent = 0.5;
        }
        else if (p == "darkVertical") {
            percent = 0.5;
        }
    }
    else if (p.startsWith("light")) {
        if (p == "lightDown") {
            percent = 0.75;
        }
        else if (p == "lightGray") {
            percent = 0.75;
        }
        else if (p == "lightGrid") {
            percent = 0.5625;
        }
        else if (p == "lightHorizontal") {
            percent = 0.75;
        }
        else if (p == "lightTrellis") {
            percent = 0.625;
        }
        else if (p == "lightUp") {
            percent = 0.75;
        }
        else if (p == "lightVertical") {
            percent = 0.5;
        }
    }
    else if (p == "mediumGray") {
        percent = 0.5;
    }
    else if (p == "gray0625") {
        percent = 0.9375;
    }
    else if (p == "gray125") {
        percent = 0.875;
    }
    else {
        qCWarning(lcXlsxImport) << "unknown value" << p << "of patterFill@patternType; defaulting to \"none\"";
        percent = 0;
    }

    QColor color = applyPatternDensity(m_currentBgColor, m_currentFgColor, percent);
    if (color.isValid()) {
        m_currentFillStyle->addProperty("fo:background-color", color.name());
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL bgColor
//! bgColor handler (Background Color)
/*! ECMA-376, 18.8.3, p. 1948.
 Background color of the cell fill pattern. Cell fill patterns operate with two colors:
 a background color and a foreground color. These combine together to make a patterned cell fill.

 Parent elements:
 - [done] patternFill (§18.8.20)

 Child elements:
 - none

 @todo support all elements
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_bgColor()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR_WITHOUT_NS(indexed)
    TRY_READ_ATTR_WITHOUT_NS(rgb)
    TRY_READ_ATTR_WITHOUT_NS(theme)
    TRY_READ_ATTR_WITHOUT_NS(tint)

    if (!indexed.isEmpty()) {
        int index = indexed.toInt();
        if (index >= 0 && index < 64) {
            m_currentBgColor = QString("#%1").arg(m_context->colorIndices.at(index));
        }
    }
    if (!rgb.isEmpty()) {
        m_currentBgColor = QString("#" + rgb.right(rgb.length()-2));
    }
    if (!theme.isEmpty()) {
        // Xlsx seems to switch these indices
        if (theme == "0" ) {
            theme = "1";
        }
        else if (theme == "1" ) {
            theme = "0";
        }
        else if (theme == "2") {
            theme = "3";
        }
        else if (theme == "3") {
            theme = "2";
        }
        MSOOXML::DrawingMLColorSchemeItemBase *colorItemBase = m_context->themes->colorScheme.value(theme);
        if (colorItemBase) {
            m_currentBgColor = colorItemBase->value();
        }
    }
    if (!tint.isEmpty()) {
        m_currentBgColor = tintedColor(m_currentBgColor, tint.toDouble());
    }

    readNext();
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

    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR_WITHOUT_NS(indexed)
    TRY_READ_ATTR_WITHOUT_NS(rgb)
    TRY_READ_ATTR_WITHOUT_NS(theme)
    TRY_READ_ATTR_WITHOUT_NS(tint)

    if (!indexed.isEmpty()) {
        int index = indexed.toInt();
        if (index >= 0 && index < 64) {
            m_currentFgColor = QString("#%1").arg(m_context->colorIndices.at(index));
        }
    }
    if (!rgb.isEmpty()) {
        m_currentFgColor = QString("#" + rgb.right(rgb.length()-2));
    }
    if (!theme.isEmpty()) {
        // Xlsx seems to switch these indices
        if (theme == "0" ) {
            theme = "1";
        }
        else if (theme == "1" ) {
            theme = "0";
        }
        else if (theme == "2") {
            theme = "3";
        }
        else if (theme == "3") {
            theme = "2";
        }
        MSOOXML::DrawingMLColorSchemeItemBase *colorItemBase = m_context->themes->colorScheme.value(theme);
        if (colorItemBase) {
            m_currentFgColor = colorItemBase->value();
        }
    }
    if (!tint.isEmpty()) {
        m_currentFgColor = tintedColor(m_currentFgColor, tint.toDouble());
    }

    readNext();
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
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
//! @todo            TRY_READ_IF(stop)
//todo            ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL borders
//! 18.8.5 borders (Borders), p. 1951
/*
 Parent elements:
 - [done] styleSheet (§18.8.39)

 Child elements:
 - [done] border (Border) §18.8.4

*/
//! @todo support all elements
KoFilter::ConversionStatus XlsxXmlStylesReader::read_borders()
{
    READ_PROLOGUE
    if (!m_context->styles->borderStyles.isEmpty()) {
        raiseUnexpectedSecondOccurenceOfElError(STRINGIFY(CURRENT_EL));
    }
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(count)
    uint countNumber = 0;
    STRING_TO_INT(count, countNumber, "styleSheet/borders@count")
    m_context->styles->borderStyles.resize(countNumber);
    uint borderStyleIndex = 0;

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            if (QUALIFIED_NAME_IS(border)) {
                m_currentBorderStyle = new KoGenStyle(KoGenStyle::TableCellAutoStyle, "table-cell");
                if (borderStyleIndex >= (uint)m_context->styles->borderStyles.size()) {
                    raiseError(i18n("Declared number of fill styles too small (%1)", m_context->styles->borderStyles.size()));
                    return KoFilter::WrongFormat;
                }
                TRY_READ(border)
                m_context->styles->borderStyles[borderStyleIndex] = m_currentBorderStyle;
                m_currentBorderStyle = 0;
                borderStyleIndex++;
            }
            ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL border
//! 18.8.4 border (Border), p. 1949
//! @todo support all elements
KoFilter::ConversionStatus XlsxXmlStylesReader::read_border()
{
    READ_PROLOGUE
    diagonalDirections = 0;

    if (readBooleanAttr("diagonalUp")) {
        diagonalDirections |= XlsxXmlStylesReader::DiagonalUp;
    }
    if (readBooleanAttr("diagonalDown")) {
        diagonalDirections |= XlsxXmlStylesReader::DiagonalDown;
    }

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(bottom)
            ELSE_TRY_READ_IF(diagonal)
//! @todo            ELSE_TRY_READ_IF(end)
//! @todo (dxf only)            ELSE_TRY_READ_IF(horizontal)
            ELSE_TRY_READ_IF(left)
//! @todo            ELSE_TRY_READ_IF(start)
            ELSE_TRY_READ_IF(right)
            ELSE_TRY_READ_IF(top)
            SKIP_UNKNOWN
//! @todo (dxf only)            ELSE_TRY_READ_IF(vertical)
//todo            ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL bottom
//! 18.8.6 bottom (Bottom Border), p. 1952
//! @todo support all elements
KoFilter::ConversionStatus XlsxXmlStylesReader::read_bottom()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    QString borderString;
    RETURN_IF_ERROR(readAttributes(attrs, borderString))

    m_currentColor = QColor();

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(color)
            ELSE_WRONG_FORMAT
        }
    }

    if (m_currentColor.isValid()) {
        borderString += " " + m_currentColor.name();
    }

    if (!borderString.isEmpty()) {
        m_currentBorderStyle->addProperty("fo:border-bottom", borderString);
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL top
//! 18.8.6 bottom (Bottom Border), p. 1952
//! @todo support all elements
KoFilter::ConversionStatus XlsxXmlStylesReader::read_top()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    QString borderString;
    RETURN_IF_ERROR(readAttributes(attrs, borderString))

    m_currentColor = QColor();

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(color)
            ELSE_WRONG_FORMAT
        }
    }

    if (m_currentColor.isValid()) {
        borderString += " " + m_currentColor.name();
    }

    if (!borderString.isEmpty()) {
        m_currentBorderStyle->addProperty("fo:border-top", borderString);
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL left
//! @todo support all elements
KoFilter::ConversionStatus XlsxXmlStylesReader::read_left()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    QString borderString;
    RETURN_IF_ERROR(readAttributes(attrs, borderString))

    m_currentColor = QColor();

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(color)
            ELSE_WRONG_FORMAT
        }
    }

    if (m_currentColor.isValid()) {
        borderString += " " + m_currentColor.name();
    }

    if (!borderString.isEmpty()) {
        m_currentBorderStyle->addProperty("fo:border-left", borderString);
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL right
//! @todo support all elements
KoFilter::ConversionStatus XlsxXmlStylesReader::read_right()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    QString borderString;
    RETURN_IF_ERROR(readAttributes(attrs, borderString))

    m_currentColor = QColor();

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(color)
            ELSE_WRONG_FORMAT
        }
    }

    if (m_currentColor.isValid()) {
        borderString += " " + m_currentColor.name();
    }

    if (!borderString.isEmpty()) {
        m_currentBorderStyle->addProperty("fo:border-right", borderString);
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL diagonal
//! @todo support all elements
KoFilter::ConversionStatus XlsxXmlStylesReader::read_diagonal()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    QString borderString;
    RETURN_IF_ERROR(readAttributes(attrs, borderString))

    m_currentColor = QColor();

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(color)
            ELSE_WRONG_FORMAT
        }
    }

    if (m_currentColor.isValid()) {
        borderString += " " + m_currentColor.name();
    }

    if (!borderString.isEmpty()) {
       if (diagonalDirections & DiagonalUp) {
            m_currentBorderStyle->addProperty("style:diagonal-bl-tr", borderString);
        }
        if (diagonalDirections & DiagonalDown) {
            m_currentBorderStyle->addProperty("style:diagonal-tl-br", borderString);
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL colors
/*
 Parent elements:
 - [done] styleSheet (§18.8.39)

 Child elements:
 - [done] indexedColors (Color Indexes) §18.8.27
 - mruColors (MRU Colors) §18.8.28
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_colors()
{
    READ_PROLOGUE

    m_colorIndex = 0;

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(indexedColors)
            SKIP_UNKNOWN
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL indexedColors
/*
 Parent elements:
 - [done] colors (§18.8.11)

 Child elements:
 - [done] rgbColor (RGB Color) §18.8.34
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_indexedColors()
{
    READ_PROLOGUE

    // In this element, we override the default colors

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(rgbColor)
            ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL rgbColor
/*
 Parent elements:
 - [done] indexedColors (§18.8.27)

 Child elements:
 - none
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_rgbColor()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR_WITHOUT_NS(rgb)
    if (!rgb.isEmpty()) {
        m_context->colorIndices[m_colorIndex] = rgb.right(rgb.length()-2);
    }
    ++m_colorIndex;
    readNext();
    READ_EPILOGUE
}

