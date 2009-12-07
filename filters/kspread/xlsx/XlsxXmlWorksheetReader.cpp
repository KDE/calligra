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

#include "XlsxXmlWorksheetReader.h"
#include "XlsxXmlStylesReader.h"
#include <MsooXmlSchemas.h>
#include <MsooXmlUtils.h>

#include <KoUnit.h>
#include <KoXmlWriter.h>
#include <KoGenStyles.h>
#include <KoOdfGraphicStyles.h>
#include <styles/KoCharacterStyle.h>

#include <kspread/Util.h>

#include <QBrush>
#include <QRegExp>

#undef  MSOOXML_CURRENT_NS
#define MSOOXML_CURRENT_CLASS XlsxXmlWorksheetReader
#define BIND_READ_CLASS MSOOXML_CURRENT_CLASS

#include <MsooXmlReader_p.h>

#include <math.h>

XlsxXmlWorksheetReaderContext::XlsxXmlWorksheetReaderContext(
    uint _worksheetNumber,
    const QString& _worksheetName,
    const QMap<QString, MSOOXML::DrawingMLTheme*>& _themes,
    const QVector<QString>& _sharedStrings,
    const XlsxStyles& _styles)
    : worksheetNumber(_worksheetNumber)
    , worksheetName(_worksheetName), themes(&_themes)
    , sharedStrings(&_sharedStrings), styles(&_styles)
{
}

const char* XlsxXmlWorksheetReader::officeValue = "office:value";
const char* XlsxXmlWorksheetReader::officeDateValue = "office:date-value";
const char* XlsxXmlWorksheetReader::officeBooleanValue = "office:boolean-value";

class XlsxXmlWorksheetReader::Private {
public:
    Private()
    {
    }
    ~Private()
    {
    }
};

XlsxXmlWorksheetReader::XlsxXmlWorksheetReader(KoOdfWriters *writers)
    : MSOOXML::MsooXmlCommonReader(writers)
    , m_context(0)
    , d(new Private)
{
    init();
}

XlsxXmlWorksheetReader::~XlsxXmlWorksheetReader()
{
    delete d;
}

void XlsxXmlWorksheetReader::init()
{
    m_defaultNamespace = "";
    m_columnCount = 0;
    m_currentRow = 0;
    m_currentColumn = -1;
}

KoFilter::ConversionStatus XlsxXmlWorksheetReader::read(MSOOXML::MsooXmlReaderContext* context)
{
    m_context = dynamic_cast<XlsxXmlWorksheetReaderContext*>(context);
    Q_ASSERT(m_context);
    const KoFilter::ConversionStatus result = readInternal();
    m_context = 0;
    if (result == KoFilter::OK)
        return KoFilter::OK;
    return result;
}

KoFilter::ConversionStatus XlsxXmlWorksheetReader::readInternal()
{
    kDebug() << "=============================";

    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }

    // worksheet
    readNext();
    kDebug() << *this << namespaceUri();

    if (!expectEl("worksheet")) {
        return KoFilter::WrongFormat;
    }
    if (!expectNS(MSOOXML::Schemas::spreadsheetml)) {
        return KoFilter::WrongFormat;
    }
/*
    const QXmlStreamAttributes attrs( attributes() );
    for (int i=0; i<attrs.count(); i++) {
        kDebug() << "1 NS prefix:" << attrs[i].name() << "uri:" << attrs[i].namespaceUri();
    }*/

    QXmlStreamNamespaceDeclarations namespaces( namespaceDeclarations() );
    for (int i=0; i<namespaces.count(); i++) {
        kDebug() << "NS prefix:" << namespaces[i].prefix() << "uri:" << namespaces[i].namespaceUri();
    }
//! @todo find out whether the namespace returned by namespaceUri()
//!       is exactly the same ref as the element of namespaceDeclarations()
    if (!namespaces.contains( QXmlStreamNamespaceDeclaration( "", MSOOXML::Schemas::spreadsheetml ) )) {
        raiseError(i18n("Namespace \"%1\" not found", MSOOXML::Schemas::spreadsheetml));
        return KoFilter::WrongFormat;
    }
//! @todo expect other namespaces too...

    TRY_READ(worksheet)
    kDebug() << "===========finished============";
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL worksheet
//! worksheet handler (Worksheet)
/*! ECMA-376, 18.3.1.99, p. 1894.
 Root element of Worksheet parts within a SpreadsheetML document.
 Child elements:
 - autoFilter (AutoFilter Settings) §18.3.1.2
 - cellWatches (Cell Watch Items) §18.3.1.9
 - colBreaks (Vertical Page Breaks) §18.3.1.14
 - cols (Column Information) §18.3.1.17
 - conditionalFormatting (Conditional Formatting) §18.3.1.18
 - controls (Embedded Controls) §18.3.1.21
 - customProperties (Custom Properties) §18.3.1.23
 - customSheetViews (Custom Sheet Views) §18.3.1.27
 - dataConsolidate (Data Consolidate) §18.3.1.29
 - dataValidations (Data Validations) §18.3.1.33
 - dimension (Worksheet Dimensions) §18.3.1.35
 - drawing (Drawing) §18.3.1.36
 - drawingHF (Drawing Reference in Header Footer) §18.3.1.37
 - extLst (Future Feature Data Storage Area) §18.2.10
 - headerFooter (Header Footer Settings) §18.3.1.46
 - hyperlinks (Hyperlinks) §18.3.1.48
 - ignoredErrors (Ignored Errors) §18.3.1.51
 - mergeCells (Merge Cells) §18.3.1.55
 - oleObjects (Embedded Objects) §18.3.1.60
 - pageMargins (Page Margins) §18.3.1.62
 - pageSetup (Page Setup Settings) §18.3.1.63
 - phoneticPr (Phonetic Properties) §18.4.3
 - picture (Background Image) §18.3.1.67
 - printOptions (Print Options) §18.3.1.70
 - protectedRanges (Protected Ranges) §18.3.1.72
 - rowBreaks (Horizontal Page Breaks (Row)) §18.3.1.74
 - scenarios (Scenarios) §18.3.1.76
 - sheetCalcPr (Sheet Calculation Properties) §18.3.1.79
 - [done] sheetData (Sheet Data) §18.3.1.80
 - sheetFormatPr (Sheet Format Properties) §18.3.1.81
 - sheetPr (Sheet Properties) §18.3.1.82
 - sheetProtection (Sheet Protection Options) §18.3.1.85
 - sheetViews (Sheet Views) §18.3.1.88
 - smartTags (Smart Tags) §18.3.1.90
 - sortState (Sort State) §18.3.1.92
 - tableParts (Table Parts) §18.3.1.95
 - webPublishItems (Web Publishing Items) §18.3.1.98

 @todo support all child elements
*/
KoFilter::ConversionStatus XlsxXmlWorksheetReader::read_worksheet()
{
    READ_PROLOGUE

    body->startElement("table:table"); // CASE #S232

//! @todo implement CASE #S202 for fixing the name
    body->addAttribute("table:name", m_context->worksheetName);

    m_tableStyle = KoGenStyle(KoGenStyle::StyleAutoTable, "table");
//! @todo hardcoded master page name
    m_tableStyle.addAttribute("style:master-page-name",
        QString("PageStyle_5f_Test_20_sheet_20__5f_%1").arg(m_context->worksheetNumber));
//! @todo table:display="true" hardcoded
    m_tableStyle.addProperty("table:display", XlsxXmlWorksheetReader::constTrue);

    const QString currentTableStyleName( mainStyles->lookup(m_tableStyle) );
    body->addAttribute("table:style-name", currentTableStyleName);

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(sheetFormatPr)
            ELSE_TRY_READ_IF(cols)
            ELSE_TRY_READ_IF(sheetData)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    body->endElement(); // table:table
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL sheetFormatPr
//! sheetFormatPr handler (Sheet Format Properties)
/*! ECMA-376, 18.3.1.81, p. 1866.
 Sheet formatting properties.
 No child elements.
 Parent elements:
 - dialogsheet (§18.3.1.34)
 - [done] worksheet (§18.3.1.99)

 @todo support all attributes and elements
*/
KoFilter::ConversionStatus XlsxXmlWorksheetReader::read_sheetFormatPr()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs( attributes() );
    TRY_READ_ATTR_WITHOUT_NS_INTO(defaultRowHeight, m_defaultRowHeight) // in pt

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL cols
//! cols handler (Column Information)
/*! ECMA-376, 18.3.1.17, p. 1782.
 Information about whole columns of the worksheet.
 Child elements:
 - [done] col (Column Width & Formatting) §18.3.1.13
 Parent elements:
 - [done] worksheet (§18.3.1.99)
*/
KoFilter::ConversionStatus XlsxXmlWorksheetReader::read_cols()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(col)
            ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE_WITHOUT_RETURN

    // add <table:table-column table:number-columns-repeated="..."
    body->startElement("table:table-column");
    body->addAttribute("table:number-columns-repeated", 256 - m_columnCount);
//! @todo hardcoded table:default-cell-style-name
    body->addAttribute("table:default-cell-style-name", "Excel_20_Built-in_20_Normal");

//! @todo hardcoded default style:column-width
    saveColumnStyle("1.707cm");

    body->endElement(); // table:table-column

    return KoFilter::OK;
}

//! Saves information about column style
void XlsxXmlWorksheetReader::saveColumnStyle(const QString& widthString)
{
    KoGenStyle tableColumnStyle(KoGenStyle::StyleAutoTableColumn, "table-column");
    tableColumnStyle.addProperty("style:column-width", widthString);
    tableColumnStyle.addProperty("fo:break-before", "auto");

    const QString currentTableColumnStyleName( mainStyles->lookup(tableColumnStyle) );
    body->addAttribute("table:style-name", currentTableColumnStyleName);
}

//! @return value @a cm with cm suffix
static QString printCm(double cm)
{
    QString string;
    string.sprintf("%3.3fcm", cm);
    return string;
}

#undef CURRENT_EL
#define CURRENT_EL col
//! col handler (Column Width & Formatting)
/*! ECMA-376, 18.3.1.13, p. 1777.
 Defines column width and column formatting for one or more columns of the worksheet.
 No child elements.
 Parent elements:
 - [done] cols (§18.3.1.17)
*/
KoFilter::ConversionStatus XlsxXmlWorksheetReader::read_col()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs( attributes() );

    m_columnCount++;
    body->startElement("table:table-column"); // CASE #S2500?
    TRY_READ_ATTR_WITHOUT_NS(min)
    TRY_READ_ATTR_WITHOUT_NS(max)
    TRY_READ_ATTR_WITHOUT_NS(width)
    if (!width.isEmpty()) {
        bool ok;
        double widthNumber = width.toDouble(&ok);
        if (!ok)
            return KoFilter::WrongFormat;
        //! CASE #S3300
        //! Column width measured as the number of characters of the maximum digit width of the
        //! numbers 0, 1, 2, …, 9 as rendered in the normal style's font. There are 4 pixels of margin
        //! padding (two on each side), plus 1 pixel padding for the gridlines.
        //! For explanation of width, see p. 1778
//simplified:
//! @todo hardcoded, not 100% accurate
        QString realWidthString;
        kDebug() << "PT_TO_PX(11.0):" << PT_TO_PX(11.0);
        const double realSize = round(PT_TO_PX(11.0)) * 0.75;
        kDebug() << "realSize:" << realSize;
        const double averageDigitWidth = realSize * 2.0 / 3.0;
        kDebug() << "averageDigitWidth:" << averageDigitWidth;
        if (averageDigitWidth * widthNumber == 0)
            realWidthString = QLatin1String("0cm");
        else
            realWidthString = printCm(PX_TO_CM(averageDigitWidth * widthNumber));

        kDebug() << "realWidthString:" << realWidthString;
        saveColumnStyle(realWidthString);
//! @todo hardcoded table:default-cell-style-name
        body->addAttribute("table:default-cell-style-name", "Excel_20_Built-in_20_Normal");
    }
    // we apparently don't need "customWidth" attr
//! @todo more attrs

    SKIP_EVERYTHING

    body->endElement(); // table:table-column

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL sheetData
//! sheetData handler (Sheet Data)
/*! ECMA-376, 18.3.1.80, p. 1866.
 This collection represents the cell table itself. This collection expresses information
 about each cell, grouped together by rows in the worksheet.
 Child elements:
 - [done] row (Row) §18.3.1.73
 Parent elements:
 - [done] worksheet (§18.3.1.99)
*/
KoFilter::ConversionStatus XlsxXmlWorksheetReader::read_sheetData()
{
    READ_PROLOGUE
    m_currentRow = 0;
    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(row)
            ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

KoFilter::ConversionStatus XlsxXmlWorksheetReader::saveRowStyle(const QString& _heightString)
{
    QString heightString(_heightString);
    if (heightString.isEmpty()) {
        heightString = m_defaultRowHeight;
    }
    KoGenStyle tableRowStyle(KoGenStyle::StyleAutoTableRow, "table-row");
//! @todo alter fo:break-before?
    tableRowStyle.addProperty("fo:break-before", MsooXmlReader::constAuto);
//! @todo alter style:use-optimal-row-height?
    tableRowStyle.addProperty("style:use-optimal-row-height", MsooXmlReader::constFalse);
    if (!heightString.isEmpty()) {
        bool ok;
        double height = heightString.toDouble(&ok);
        if (!ok)
            return KoFilter::WrongFormat;
        tableRowStyle.addProperty("style:row-height", printCm(POINT_TO_CM(height)));
    }
    const QString currentTableRowStyleName( mainStyles->lookup(tableRowStyle) );
    body->addAttribute("table:style-name", currentTableRowStyleName);
    return KoFilter::OK;
}

void XlsxXmlWorksheetReader::appendTableCells(uint cells)
{
    if (cells == 0)
        return;
    body->startElement("table:table-cell");
    if (cells > 1)
        body->addAttribute("table:number-columns-repeated", QByteArray::number(cells));
    body->endElement(); // table:table-cell
}

#undef CURRENT_EL
#define CURRENT_EL row
//! row handler (Row)
/*! ECMA-376, 18.3.1.73, p. 1855.
 The element expresses information about an entire row of a worksheet,
 and contains all cell definitions for a particular row in the worksheet.

 Child elements:
 - [done] c (Cell) §18.3.1.4
 - extLst (Future Feature Data Storage Area) §18.2.10
 Parent elements:
 - [done] sheetData (§18.3.1.80)

 @todo support all child elements
*/
KoFilter::ConversionStatus XlsxXmlWorksheetReader::read_row()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs( attributes() );
    TRY_READ_ATTR_WITHOUT_NS(r)
    TRY_READ_ATTR_WITHOUT_NS(spans)
    TRY_READ_ATTR_WITHOUT_NS(ht)
    TRY_READ_ATTR_WITHOUT_NS(customHeight)

    uint rNumber = 0;
    if (!r.isEmpty()) {
        bool ok;
        rNumber = r.toUInt(&ok);
        if (!ok)
            return KoFilter::WrongFormat;
    }
    if (!spans.isEmpty()) {

    }
    if ((m_currentRow + 1) < rNumber) {
        body->startElement("table:table-row");
        const KoFilter::ConversionStatus saveRowStyleStatus = saveRowStyle(QString());
        if (saveRowStyleStatus != KoFilter::OK)
            return saveRowStyleStatus;

        const uint skipRows = rNumber - (m_currentRow + 1);
        if (skipRows > 1) {
            body->addAttribute("table:number-rows-repeated", QString::number(skipRows));
        }
        appendTableCells(256);
        body->endElement(); // table:table-row
    }

    body->startElement("table:table-row");
    const KoFilter::ConversionStatus saveRowStyleStatus = saveRowStyle(ht);
    if (saveRowStyleStatus != KoFilter::OK)
        return saveRowStyleStatus;

    m_currentColumn = -1;
    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(c) // modifies m_currentColumn
            ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    if (m_currentColumn < 255) {
        // output empty cells after the last filled cell
        appendTableCells( 256 - m_currentColumn - 1 );
    }

    body->endElement(); // table:table-row

    m_currentRow = rNumber;

    READ_EPILOGUE
}

//! @return true if @a v represents an integer or floating-point number
static bool valueIsNumeric(const QString& v)
{
    bool ok;
    v.toDouble(&ok);
    return ok;
}

//<SIMPLE SUPPORT FOR FORMULAS>
//! @todo improve support for formulas

//! @return formula with 2-dimentional range
static QByteArray printFormula(const QByteArray& formula, const QByteArray& from, const QByteArray& to)
{
    QString res;
    return res.sprintf("=%s([.%s:.%s])", formula.constData(), from.constData(), to.constData()).toLatin1();
}

static QByteArray convertFormula(const QByteArray& formula)
{
    if (formula.isEmpty())
        return QByteArray();
    QRegExp re("([0-9a-zA-Z]+)\\(([0-9A-Z]+):([0-9A-Z]+)\\)"); // e.g. SUM(B2:B4)
    kDebug() << formula;
    if (re.exactMatch(formula)) {
        kDebug() << re.cap(1) << re.cap(2) << re.cap(3);
        return printFormula(re.cap(1).toLatin1(), re.cap(2).toLatin1(), re.cap(3).toLatin1());
    }
    qDebug() << "Parsing of formula" << formula << "not implemented";
    return QByteArray();
}
//</SIMPLE SUPPORT FOR FORMULAS>

#undef CURRENT_EL
#define CURRENT_EL c
//! c handler (Cell)
/*! ECMA-376, 18.3.1.4, p. 1767.
 This collection represents a cell in the worksheet.
 Information about the cell's location (reference), value, data
 type, formatting, and formula is expressed here.

 Child elements:
 - extLst (Future Feature Data Storage Area) §18.2.10
 - [done] f (Formula) §18.3.1.40
 - is (Rich Text Inline) §18.3.1.53
 - [done] v (Cell Value) §18.3.1.96
 Parent elements:
 - [done] row (§18.3.1.73)

 @todo support all child elements
*/
KoFilter::ConversionStatus XlsxXmlWorksheetReader::read_c()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs( attributes() );
    TRY_READ_ATTR_WITHOUT_NS(r)
    uint referencedColumn = -1;
    if (!r.isEmpty()) {
        referencedColumn = KSpread::Util::decodeColumnLabelText(r) - 1;
        kDebug() << "referencedColumn:" << r << referencedColumn;
        if (m_currentColumn == -1 && referencedColumn > 0) {
            // output empty cells before the first filled cell
            appendTableCells( referencedColumn );
        }
        m_currentColumn = referencedColumn;
    }

    TRY_READ_ATTR_WITHOUT_NS(s)
    TRY_READ_ATTR_WITHOUT_NS(t)
    m_value.clear();
    m_formula.clear();

    // buffer this table:table-cell, because we have to compute style
    // or attributes before details are known
    MSOOXML::Utils::XmlWriteBuffer tableCellBuf;
    body = tableCellBuf.setWriter(body);
//    KoXmlWriter *origBody = body;
//    body = new KoXmlWriter(&tableCellBuf, origBody->indentLevel()+1);

    body->startElement("text:p");

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(f)
            ELSE_TRY_READ_IF(v)
            ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    QByteArray valueType;
    QByteArray valueAttr;
    if (!m_value.isEmpty()) {
        /* depending on type: 18.18.11 ST_CellType (Cell Type), p. 2679:
            b (Boolean) Cell containing a boolean.
            d (Date) Cell contains a date in the ISO 8601 format.
            e (Error) Cell containing an error.
            inlineStr (Inline String) Cell containing an (inline) rich string, i.e., one not in
            the shared string table. If this cell type is used, then
            the cell value is in the is element rather than the v
            element in the cell (c element).
            n (Number) Cell containing a number.
            s (Shared String) Cell containing a shared string.
            str (String) Cell containing a formula string.

            Converting into values described in ODF1.1: "6.7.1. Variable Value Types and Values".
        */
        if (t == QLatin1String("s")) {
            bool ok;
            const uint stringIndex = m_value.toUInt(&ok);
            if (!ok || (int)stringIndex >= m_context->sharedStrings->size()) {
                return KoFilter::WrongFormat;
            }
            body->addTextSpan(
                m_context->sharedStrings->at(stringIndex)
            );
            valueType = MsooXmlReader::constString;
            // no valueAttr
        }
        else if ((t.isEmpty() && !valueIsNumeric(m_value)) || t == QLatin1String("inlineStr")) {
//! @todo handle value properly
            body->addTextSpan(m_value);
            valueType = MsooXmlReader::constString;
            // no valueAttr
        }
        else if (t == QLatin1String("b")) {
            body->addTextSpan(m_value);
            valueType = MsooXmlReader::constBoolean;
            valueAttr = XlsxXmlWorksheetReader::officeBooleanValue;
        }
        else if (t == QLatin1String("d")) {
//! @todo handle value properly
            body->addTextSpan(m_value);
            valueType = MsooXmlReader::constDate;
            valueAttr = XlsxXmlWorksheetReader::officeDateValue;
        }
        else if (t == QLatin1String("str")) {
//! @todo handle value properly
            body->addTextSpan(m_value);
            valueType = MsooXmlReader::constString;
            // no valueAttr
        }
        else if (t == QLatin1String("n") || t.isEmpty() /* already checked if numeric */) {
            if (!t.isEmpty()) { // sanity check
                if (!valueIsNumeric(m_value)) {
                    raiseError( i18n("Expected integer of floating point number") );
                    return KoFilter::WrongFormat;
                }
            }
            body->addTextSpan(m_value);
            valueType = MsooXmlReader::constFloat;
            valueAttr = XlsxXmlWorksheetReader::officeValue;
        }
        else {
            raiseUnexpectedAttributeValueError(t, "c@t");
            return KoFilter::WrongFormat;
        }
    }

    body->endElement(); // text:p
    body = tableCellBuf.originalWriter();
    {
        body->startElement("table:table-cell");
        body->addAttribute("office:value-type", valueType);
        if (!valueAttr.isEmpty()) {
            body->addAttribute(valueAttr, m_value);
        }
        if (!m_formula.isEmpty()) {
            body->addAttribute("table:formula", m_formula);
        }
        // cell style
        if (!s.isEmpty()) {
            bool ok;
            uint styleId = s.toUInt(&ok);
            kDebug() << "styleId:" << styleId;
            const XlsxCellFormat* cellFormat = m_context->styles->cellFormat(styleId);
            if (!ok || !cellFormat) {
                raiseUnexpectedAttributeValueError(s, "c@s");
                return KoFilter::WrongFormat;
            }
            kDebug() << "fontId:" << cellFormat->fontId;
            const XlsxFontStyle* fontStyle = m_context->styles->fontStyle(cellFormat->fontId);
            if (!fontStyle) {
                raiseUnexpectedAttributeValueError(s, "c@s");
                return KoFilter::WrongFormat;
            }
            KoGenStyle cellStyle(KoGenStyle::StyleAutoTableCell, "table-cell");
//! @todo hardcoded master style name
            cellStyle.addAttribute("style:parent-style-name",
                QLatin1String("Excel_20_Built-in_20_Normal"));

            KoCharacterStyle cellCharacterStyle;
            fontStyle->setupCharacterStyle(&cellCharacterStyle);
            cellCharacterStyle.saveOdf(cellStyle);

            fontStyle->setupCellTextStyle(&cellStyle);

            cellFormat->setupCellStyle(&cellStyle);

            const QString cellStyleName( mainStyles->lookup(cellStyle) );
            body->addAttribute("table:style-name", cellStyleName);
        }

        (void)tableCellBuf.releaseWriter();
        body->endElement(); // table:table-cell
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL f
//! f handler (Formula)
/*! ECMA-376, 18.3.1.40, p. 1813.
 Formula for the cell. The formula expression is contained in the character node of this element.

 No child elements.
 Parent elements:
 - [done] c (§18.3.1.4)
 - nc (§18.11.1.3)
 - oc (§18.11.1.5)

 @todo support all parent elements
*/
KoFilter::ConversionStatus XlsxXmlWorksheetReader::read_f()
{
    READ_PROLOGUE
    readNext();
    m_formula = convertFormula( text().toString().toLatin1() );
    kDebug() << m_formula;

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL v
//! v handler (Cell Value)
/*! ECMA-376, 18.3.1.96, p. 1891.
 This element expresses the value contained in a cell.

 No child elements.
 Parent elements:
 - [done] c (§18.3.1.4)
 - cell (§18.14.1)
 - nc (§18.11.1.3)
 - oc (§18.11.1.5)
 - tp (§18.15.3)

 @todo support all parent elements
*/
KoFilter::ConversionStatus XlsxXmlWorksheetReader::read_v()
{
    READ_PROLOGUE
    readNext();
    m_value = text().toString();
    kDebug() << m_value;

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}
