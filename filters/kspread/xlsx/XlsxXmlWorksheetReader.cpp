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
#include <MsooXmlUnits.h>
#include <MsooXmlGlobal.h>

#include <KoUnit.h>
#include <KoXmlWriter.h>
#include <KoGenStyles.h>
#include <KoOdfNumberStyles.h>
#include <KoOdfGraphicStyles.h>
#include <styles/KoCharacterStyle.h>

#include <kspread/Util.h>

#include <QBrush>
#include <QRegExp>

#define UNICODE_EUR 0x20AC
#define UNICODE_GBP 0x00A3
#define UNICODE_JPY 0x00A5

#undef  MSOOXML_CURRENT_NS
#define MSOOXML_CURRENT_CLASS XlsxXmlWorksheetReader
#define BIND_READ_CLASS MSOOXML_CURRENT_CLASS

#include <MsooXmlReader_p.h>

#include <math.h>

// Remove via the "\" char escaped characters from the string.
QString removeEscaped(const QString &text, bool removeOnlyEscapeChar = false)
{
    QString s(text);
    int pos = 0;
    while (true) {
        pos = s.indexOf('\\', pos);
        if (pos < 0)
            break;
        if (removeOnlyEscapeChar) {
            s = s.left(pos) + s.mid(pos + 1);
            pos++;
        } else {
            s = s.left(pos) + s.mid(pos + 2);
        }
    }
    return s;
}


static void processNumberText(KoXmlWriter* xmlWriter, QString& text)
{
    if (! text.isEmpty()) {
        xmlWriter->startElement("number:text");
        xmlWriter->addTextNode(removeEscaped(text, true));
        xmlWriter->endElement();  // number:text
        text.clear();
    }
}

// Another form of conditional formats are those that define a different format
// depending on the value. In following examples the different states are
// splittet with a ; char, the first is usually used if the value is positive,
// the second if the value if negavtive, the third if the value is invalid and
// the forth defines a common formatting mask.
// _("$"* #,##0.0000_);_("$"* \(#,##0.0000\);_("$"* "-"????_);_(@_)
// _-[$₩-412]* #,##0.0000_-;\-[$₩-412]* #,##0.0000_-;_-[$₩-412]* "-"????_-;_-@_-
// _ * #,##0_)[$€-1]_ ;_ * #,##0[$€-1]_ ;_ * "-"_)[$€-1]_ ;_ @_ "
QString extractConditional(const QString &_text)
{
    const QString text = removeEscaped(_text);
#if 1
    if ( text.startsWith('_') && text.length() >= 3 ) {
        QChar end;
        if(text[1] == '(') end = ')';
        else if(text[1] == '_') end = '_';
        else if(text[1] == ' ') end = ' ';
        else kDebug() << "Probably unhandled condition=" << text[1] <<"in text=" << text;
        if(! end.isNull()) {
            {
                QString regex = QString( "^_%1(.*\"\\$\".*)%2;.*" ).arg(QString("\\%1").arg(text[1])).arg(QString("\\%1").arg(end));
                QRegExp ex(regex);
                ex.setMinimal(true);
                if (ex.indexIn(text) >= 0) return ex.cap(1);
            }
            {
                QString regex = QString( "^_%1(.*\\[\\$.*\\].*)%2;.*" ).arg(QString("\\%1").arg(text[1])).arg(QString("\\%1").arg(end));
                QRegExp ex(regex);
                ex.setMinimal(true);
                if (ex.indexIn(text) >= 0) return ex.cap(1);
            }
        }
    }
#else
    if ( text.startsWith('_') ) {
        return text.split(";").first();
    }
#endif
    return text;
}

// Currency or accounting format.
// "$"* #,##0.0000_
// [$EUR]\ #,##0.00"
// [$₩-412]* #,##0.0000
// * #,##0_)[$€-1]_
static bool currencyFormat(const QString& valueFormat, QString *currencyVal = 0, QString *formatVal = 0)
{
    QString vf = extractConditional(valueFormat);

    // dollar is special cause it starts with a $
    QRegExp dollarRegEx("^\"\\$\"[*\\-\\s]*([#,]*[\\d]+(|.[#0]+)).*");
    if (dollarRegEx.indexIn(vf) >= 0) {
        if(currencyVal) *currencyVal = "$";
        if(formatVal) *formatVal = dollarRegEx.cap(1);
        return true;
    }

    // every other currency or accounting has a [$...] identifier
    QRegExp crRegEx("\\[\\$(.*)\\]");
    crRegEx.setMinimal(true);
    if (crRegEx.indexIn(vf) >= 0) {
        if(currencyVal) {
            *currencyVal = crRegEx.cap(1);
        }
        if(formatVal) {
            QRegExp vlRegEx("([#,]*[\\d]+(|.[#0]+))");
            *formatVal = vlRegEx.indexIn(vf) >= 0 ? vlRegEx.cap(1) : QString();
        }
        return true;
    }

    return false;
}


QString extractLocale(QString &time)
{
    QString locale;
    if (time.startsWith("[$-")) {
        int pos = time.indexOf(']');
        if (pos > 3) {
            locale = time.mid(3, pos - 3);
            time = time.mid(pos + 1);
            pos = time.lastIndexOf(';');
            if (pos >= 0) {
                time = time.left(pos);
            }
        }
    }
    return locale;
}

static bool isTimeFormat( const QString& value, const QString& valueFormat)
{
    bool ok = false;
    const double v = value.toDouble( &ok );
    if( !ok )    
        return false;

    QString vf = valueFormat;
    QString locale = extractLocale(vf);
    Q_UNUSED(locale);
    vf = removeEscaped(vf);

    // if( vf == "h:mm AM/PM" ) return true;
    // if( vf == "h:mm:ss AM/PM" ) return true;
    // if( vf == "h:mm" ) return true;
    // if( vf == "h:mm:ss" ) return true;
    // if( vf == "[h]:mm:ss" ) return true;
    // if( vf == "[h]:mm" ) return true;
    // if( vf == "[mm]:ss" ) return true;
    // if( vf == "M/D/YY h:mm" ) return true;
    // if( vf == "[ss]" ) return true;
    // if( vf == "mm:ss" ) return true;
    // if( vf == "mm:ss.0" ) return true;
    // if( vf == "[mm]:ss" ) return true;
    // if( vf == "[ss]" ) return true;

    // if there is still a time formatting picture item that was not escaped
    // and therefore removed above, then we have a time format here.
    QRegExp ex("(h|H|m|s)");
    return (ex.indexIn(vf) >= 0) && v < 1.0;
}




XlsxXmlWorksheetReaderContext::XlsxXmlWorksheetReaderContext(
    uint _worksheetNumber,
    const QString& _worksheetName,
    const QMap<QString, MSOOXML::DrawingMLTheme*>& _themes,
    const XlsxSharedStringVector& _sharedStrings,
    const XlsxStyles& _styles)
        : worksheetNumber(_worksheetNumber)
        , worksheetName(_worksheetName), themes(&_themes)
        , sharedStrings(&_sharedStrings), styles(&_styles)
{
}

const char* XlsxXmlWorksheetReader::officeValue = "office:value";
const char* XlsxXmlWorksheetReader::officeDateValue = "office:date-value";
const char* XlsxXmlWorksheetReader::officeTimeValue = "office:time-value";
const char* XlsxXmlWorksheetReader::officeBooleanValue = "office:boolean-value";

class XlsxXmlWorksheetReader::Private
{
public:
    Private( XlsxXmlWorksheetReader* qq )
     : q( qq ),
       warningAboutWorksheetSizeDisplayed(false)
    {
    }
    ~Private() {
    }
    XlsxXmlWorksheetReader* const q;
    QString processValueFormat( const QString& valueFormat );
    bool warningAboutWorksheetSizeDisplayed;
};

XlsxXmlWorksheetReader::XlsxXmlWorksheetReader(KoOdfWriters *writers)
        : MSOOXML::MsooXmlCommonReader(writers)
        , m_context(0)
        , d(new Private( this ) )
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

    QXmlStreamNamespaceDeclarations namespaces(namespaceDeclarations());
    for (int i = 0; i < namespaces.count(); i++) {
        kDebug() << "NS prefix:" << namespaces[i].prefix() << "uri:" << namespaces[i].namespaceUri();
    }
//! @todo find out whether the namespace returned by namespaceUri()
//!       is exactly the same ref as the element of namespaceDeclarations()
    if (!namespaces.contains(QXmlStreamNamespaceDeclaration("", MSOOXML::Schemas::spreadsheetml))) {
        raiseError(i18n("Namespace \"%1\" not found", MSOOXML::Schemas::spreadsheetml));
        return KoFilter::WrongFormat;
    }
//! @todo expect other namespaces too...

    TRY_READ(worksheet)
    kDebug() << "===========finished============";
    return KoFilter::OK;
}

void XlsxXmlWorksheetReader::showWarningAboutWorksheetSize()
{
    if (d->warningAboutWorksheetSizeDisplayed)
        return;
    d->warningAboutWorksheetSizeDisplayed = true;
    kWarning() << i18n("The data could not be loaded completely because the maximum size of "
        "sheet was exceeded.");
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

    const QString currentTableStyleName(mainStyles->lookup(m_tableStyle));
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
    const QXmlStreamAttributes attrs(attributes());
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

    // append remaining empty columns
    appendTableColumns(MSOOXML::maximumSpreadsheetColumns() - m_columnCount);
    return KoFilter::OK;
}

//! Saves information about column style
void XlsxXmlWorksheetReader::saveColumnStyle(const QString& widthString)
{
    KoGenStyle tableColumnStyle(KoGenStyle::StyleAutoTableColumn, "table-column");
    tableColumnStyle.addProperty("style:column-width", widthString);
    tableColumnStyle.addProperty("fo:break-before", "auto");

    const QString currentTableColumnStyleName(mainStyles->lookup(tableColumnStyle));
    body->addAttribute("table:style-name", currentTableColumnStyleName);
}

//! @return value @a cm with cm suffix
static QString printCm(double cm)
{
    QString string;
    string.sprintf("%3.3fcm", cm);
    return string;
}

void XlsxXmlWorksheetReader::appendTableColumns(int columns, const QString& width)
{
    kDebug() << "columns:" << columns;
    if (columns <= 0)
        return;
    body->startElement("table:table-column");
    if (columns > 1)
        body->addAttribute("table:number-columns-repeated", QByteArray::number(columns));
//! @todo hardcoded table:default-cell-style-name
    body->addAttribute("table:default-cell-style-name", "Excel_20_Built-in_20_Normal");
//! @todo hardcoded default style:column-width
    saveColumnStyle(width.isEmpty() ? QLatin1String("1.707cm") : width);
    body->endElement(); // table:table-column
}

#undef CURRENT_EL
#define CURRENT_EL col
//! col handler (Column Width & Formatting)
/*! ECMA-376, 18.3.1.13, p. 1777.
 Defines column width and column formatting for one or more columns of the worksheet.
 No child elements.
 Parent elements:
 - [done] cols (§18.3.1.17)

 @todo support more attributes
*/
KoFilter::ConversionStatus XlsxXmlWorksheetReader::read_col()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    m_columnCount++;
//moved    body->startElement("table:table-column"); // CASE #S2500?
    uint minCol = m_columnCount;
    uint maxCol = m_columnCount;
    QString minStr, maxStr;
    TRY_READ_ATTR_WITHOUT_NS_INTO(min, minStr)
    STRING_TO_INT(minStr, minCol, "col@min")
    TRY_READ_ATTR_WITHOUT_NS_INTO(max, maxStr)
    STRING_TO_INT(maxStr, maxCol, "col@min")
    if (minCol > maxCol)
        qSwap(minCol, maxCol);

    if (m_columnCount < minCol) {
        appendTableColumns(minCol - m_columnCount);
        m_columnCount = minCol;
    }

    TRY_READ_ATTR_WITHOUT_NS(width)
    QString realWidthString;
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
//moved        saveColumnStyle(realWidthString);
//! @todo hardcoded table:default-cell-style-name
//moved        body->addAttribute("table:default-cell-style-name", "Excel_20_Built-in_20_Normal");
    }
    // we apparently don't need "customWidth" attr
//! @todo more attrs

    SKIP_EVERYTHING

//moved    body->endElement(); // table:table-column
    appendTableColumns(maxCol - minCol + 1, realWidthString);

    m_columnCount += (maxCol - minCol);

    if (m_columnCount > MSOOXML::maximumSpreadsheetColumns()) {
        showWarningAboutWorksheetSize();
    }

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
    const QString currentTableRowStyleName(mainStyles->lookup(tableRowStyle));
    body->addAttribute("table:style-name", currentTableRowStyleName);
    return KoFilter::OK;
}

void XlsxXmlWorksheetReader::appendTableCells(int cells)
{
    if (cells <= 0)
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
    const QXmlStreamAttributes attrs(attributes());
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
    if (rNumber > MSOOXML::maximumSpreadsheetRows()) {
        showWarningAboutWorksheetSize();
    }
    if (!spans.isEmpty()) {
        //?
    }
    if ((m_currentRow + 1) < rNumber) {
        body->startElement("table:table-row");
        RETURN_IF_ERROR( saveRowStyle(QString()) )

        const uint skipRows = rNumber - (m_currentRow + 1);
        if (skipRows > 1) {
            body->addAttribute("table:number-rows-repeated", QString::number(skipRows));
        }
        appendTableCells(MSOOXML::maximumSpreadsheetColumns());
        body->endElement(); // table:table-row
    }

    body->startElement("table:table-row");
    RETURN_IF_ERROR( saveRowStyle(ht) )

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
    const int remainingColumns = MSOOXML::maximumSpreadsheetColumns() - m_currentColumn - 1;
    if (remainingColumns > 0) { // output empty cells after the last filled cell
        appendTableCells(remainingColumns);
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
static QString printFormula(const QString& formula, const QString& from, const QString& to)
{
    return QString("=%1([.%2:.%3])").arg(formula).arg(from).arg(to);
}

static QString convertFormula(const QString& formula)
{
    if (formula.isEmpty())
        return QString();
    QRegExp re("([0-9a-zA-Z]+)\\(([A-Z]+[0-9]+):([A-Z]+[0-9]+)\\)"); // e.g. SUM(B2:B4)
    kDebug() << formula;
    if (re.exactMatch(formula)) {
        kDebug() << re.cap(1) << re.cap(2) << re.cap(3);
        return printFormula(re.cap(1).toLatin1(), re.cap(2).toLatin1(), re.cap(3).toLatin1());
    }
    QString res(QLatin1String("=") + formula);
    res.replace(QRegExp("([A-Z]+[0-9]+)"), "[.\\1]");
    return res;
//    qDebug() << "Parsing of formula" << formula << "not implemented";
//    return QString();
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
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(r)
    uint referencedColumn = -1;
    if (!r.isEmpty()) {
        referencedColumn = KSpread::Util::decodeColumnLabelText(r) - 1;
        kDebug() << "referencedColumn:" << r << referencedColumn;
        if (m_currentColumn == -1 && referencedColumn > 0) {
            // output empty cells before the first filled cell
            appendTableCells(referencedColumn);
        }
        m_currentColumn = referencedColumn;
    }

    TRY_READ_ATTR_WITHOUT_NS(s)
    TRY_READ_ATTR_WITHOUT_NS(t)
    m_convertFormula = true; // t != QLatin1String("e");

    m_value.clear();
    m_formula.clear();

    // buffer this table:table-cell, because we have to compute style
    // or attributes before details are known
    MSOOXML::Utils::XmlWriteBuffer tableCellBuf;
    body = tableCellBuf.setWriter(body);
//    KoXmlWriter *origBody = body;
//    body = new KoXmlWriter(&tableCellBuf, origBody->indentLevel()+1);

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

    bool ok = false;
    uint styleId = s.toUInt(&ok);
    kDebug() << "styleId:" << styleId;
    const XlsxCellFormat* cellFormat = m_context->styles->cellFormat(styleId);
    const QString numberFormat = cellFormat->applyNumberFormat ? m_context->styles->numberFormatString( cellFormat->numFmtId ) : QString();

//    const bool addTextPElement = true;//m_value.isEmpty() || t != QLatin1String("s");

    QByteArray valueType;
    QByteArray valueAttr;

    if (!m_value.isEmpty()) {
        body->startElement("text:p", false);

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
        if( isTimeFormat( m_value, numberFormat ) )
        {
            const QTime time = QTime( 0, 0 ).addMSecs( qRound( m_value.toDouble() * 24 * 60 * 60 * 1000 ) );
            body->addTextSpan( time.toString() );
            m_value = time.toString( QLatin1String( "'PT'HH'H'mm'M'ss'S'" ) );
            valueType = MsooXmlReader::constTime;
            valueAttr = XlsxXmlWorksheetReader::officeTimeValue;
        }
        else if (t == QLatin1String("s")) {
            bool ok;
            const uint stringIndex = m_value.toUInt(&ok);
            if (!ok || (int)stringIndex >= m_context->sharedStrings->size()) {
                return KoFilter::WrongFormat;
            }
            m_context->sharedStrings->at(stringIndex).saveXml(body);
            valueType = MsooXmlReader::constString;
            // no valueAttr
        } else if ((t.isEmpty() && !valueIsNumeric(m_value)) || t == QLatin1String("inlineStr")) {
//! @todo handle value properly
            body->addTextSpan(m_value);
            valueType = MsooXmlReader::constString;
            // no valueAttr
        } else if (t == QLatin1String("b")) {
            body->addTextSpan(m_value);
            valueType = MsooXmlReader::constBoolean;
            valueAttr = XlsxXmlWorksheetReader::officeBooleanValue;
        } else if (t == QLatin1String("d")) {
//! @todo handle value properly
            body->addTextSpan(m_value);
            valueType = MsooXmlReader::constDate;
            valueAttr = XlsxXmlWorksheetReader::officeDateValue;
        } else if (t == QLatin1String("str")) {
//! @todo handle value properly
            body->addTextSpan(m_value);
            valueType = MsooXmlReader::constString;
            // no valueAttr
        } else if (t == QLatin1String("n") || t.isEmpty() /* already checked if numeric */) {
            if (!t.isEmpty()) { // sanity check
                if (!valueIsNumeric(m_value)) {
                    raiseError(i18n("Expected integer or floating point number"));
                    return KoFilter::WrongFormat;
                }
            }
            body->addTextSpan(m_value);
            valueType = MsooXmlReader::constFloat;
            valueAttr = XlsxXmlWorksheetReader::officeValue;
        } else if (t == QLatin1String("e")) {
            if (m_value == QLatin1String("#REF!"))
                body->addTextSpan("#NAME?");
            else
                body->addTextSpan(m_value);
//! @todo full parsing needed to retrieve the type
            valueType = MsooXmlReader::constFloat;
            valueAttr = XlsxXmlWorksheetReader::officeValue;
            m_value = QLatin1String("0");
        } else {
            raiseUnexpectedAttributeValueError(t, "c@t");
            return KoFilter::WrongFormat;
        }

        body->endElement(); // text:p
    }

    body = tableCellBuf.originalWriter();
    {
        body->startElement("table:table-cell");
        if (!valueType.isEmpty()) {
            body->addAttribute("office:value-type", valueType);
        }
        if (!valueAttr.isEmpty()) {
            body->addAttribute(valueAttr, m_value);
        }
        if (!m_formula.isEmpty()) {
            body->addAttribute("table:formula", m_formula);
        }
        // cell style
        if (!s.isEmpty()) {
            bool ok;
            const uint styleId = s.toUInt(&ok);
            kDebug() << "styleId:" << styleId;
            const XlsxCellFormat* cellFormat = m_context->styles->cellFormat(styleId);
            if (!ok || !cellFormat) {
                raiseUnexpectedAttributeValueError(s, "c@s");
                return KoFilter::WrongFormat;
            }
            KoGenStyle cellStyle(KoGenStyle::StyleAutoTableCell, "table-cell");
//! @todo hardcoded master style name
            cellStyle.addAttribute("style:parent-style-name",
                                   QLatin1String("Excel_20_Built-in_20_Normal"));

            KoCharacterStyle cellCharacterStyle;
            cellFormat->setupCharacterStyle(m_context->styles, &cellCharacterStyle);
            cellCharacterStyle.saveOdf(cellStyle);
            if (!cellFormat->setupCellStyle(m_context->styles, m_context->themes, &cellStyle)) {
                return KoFilter::WrongFormat;
            }
            QString cellStyleName(mainStyles->lookup(cellStyle));

/*
            if( !numberFormat.isEmpty() )
            {
                if( valueType == "time" )
                {
                    KoGenStyle timeStyle( KoGenStyle::StyleNumericTime );
                    timeStyle.addAttribute("style:parent-style-name", cellStyleName );
                    timeStyle.addChildElement( "number", "<number:hours/>" );
                    cellStyleName = mainStyles->lookup( timeStyle, "N" );
                }
            }*/

            const QString formattedStyle = d->processValueFormat( numberFormat );

            body->addAttribute("table:style-name", formattedStyle.isEmpty() ? cellStyleName : formattedStyle );
        }

        if (m_value.isEmpty()) {
            tableCellBuf.clear(); // do not output
        }
        else {
            (void)tableCellBuf.releaseWriter();
        }
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
    m_formula = m_convertFormula ? convertFormula(text().toString()) : text().toString();
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

// 3.8.31 numFmts
QString XlsxXmlWorksheetReader::Private::processValueFormat(const QString& valueFormat)
{
    // number
    QRegExp numberRegEx("(0+)(\\.0+)?(E\\+0+)?");
    if (numberRegEx.exactMatch(valueFormat)) {
        if (numberRegEx.cap(3).length())
            return KoOdfNumberStyles::saveOdfScientificStyle(*q->mainStyles, valueFormat, "", "");
        else
            return KoOdfNumberStyles::saveOdfNumberStyle(*q->mainStyles, valueFormat, "", "");
    }

    // percent
    QRegExp percentageRegEx("(0+)(\\.0+)?%");
    if (percentageRegEx.exactMatch(valueFormat)) {
        return KoOdfNumberStyles::saveOdfPercentageStyle(*q->mainStyles, valueFormat, "", "");
    }

    // text
    if (valueFormat.startsWith("@")) {
        KoGenStyle style(KoGenStyle::StyleNumericText);
        QBuffer buffer;
        buffer.open(QIODevice::WriteOnly);
        KoXmlWriter xmlWriter(&buffer);    // TODO pass indentation level

        xmlWriter.startElement("number:text-content");
        xmlWriter.endElement(); // text-content

        QString elementContents = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());
        style.addChildElement("number", elementContents);
        return q->mainStyles->lookup(style, "N");
    }
    
    // fraction
    const QString escapedValueFormat = removeEscaped(valueFormat);
    QRegExp fractionRegEx("^#([?]+)/([0-9?]+)$");
    if (fractionRegEx.indexIn(escapedValueFormat) >= 0) {
        const int minlength = fractionRegEx.cap(1).length(); // numerator
        const QString denominator = fractionRegEx.cap(2); // denominator
        bool hasDenominatorValue = false;
        const int denominatorValue = denominator.toInt(&hasDenominatorValue);

        KoGenStyle style(KoGenStyle::StyleNumericFraction);
        QBuffer buffer;
        buffer.open(QIODevice::WriteOnly);
        KoXmlWriter xmlWriter(&buffer);    // TODO pass indentation level

        xmlWriter.startElement("number:fraction");
        xmlWriter.addAttribute("number:min-numerator-digits", minlength);
        if (hasDenominatorValue) {
            QRegExp rx("/[?]*([0-9]*)[?]*$");
            if (rx.indexIn(escapedValueFormat) >= 0)
                xmlWriter.addAttribute("number:min-integer-digits", rx.cap(1).length());
            xmlWriter.addAttribute("number:number:denominator-value", denominatorValue);
        } else {
            xmlWriter.addAttribute("number:min-denominator-digits", denominator.length());
        }
        xmlWriter.endElement(); // number:fraction

        QString elementContents = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());
        style.addChildElement("number", elementContents);
        return q->mainStyles->lookup(style, "N");
    }

    // currency
    QString currencyVal, formatVal;
    if (currencyFormat(valueFormat, &currencyVal, &formatVal)) {
        KoGenStyle style(KoGenStyle::StyleNumericCurrency);
        QBuffer buffer;
        buffer.open(QIODevice::WriteOnly);
        KoXmlWriter xmlWriter(&buffer);    // TODO pass indentation level

        QRegExp symbolRegEx("^([^a-zA-Z0-9\\-_\\s]+)");
        if(symbolRegEx.indexIn(currencyVal) >= 0) {
            xmlWriter.startElement("number:currency-symbol");

            QString language, country;
            QRegExp countryRegExp("^[^a-zA-Z0-9\\s]+\\-[\\s]*([0-9]+)[\\s]*$");
            if(countryRegExp.indexIn(currencyVal) >= 0) {
                //TODO
                //bool ok = false;
                //int languageCode = countryRegExp.cap(1).toInt(&ok);
                //if(ok) language = languageName(languageCode);
            } else if(currencyVal[0] == '$') {
                language = "en";
                country = "US";
            } else if(currencyVal[0] == QChar(UNICODE_EUR)) {
                // should not be possible cause there is no single "euro-land"
            } else if(currencyVal[0] == QChar(UNICODE_GBP)) {
                language = "en";
                country = "GB";
            } else if(currencyVal[0] == QChar(UNICODE_JPY)) {
                language = "ja";
                country = "JP";
            } else {
                // nothing we can do here...
            }

            if(!language.isEmpty())
                xmlWriter.addAttribute("number:language", language);
            if(!country.isEmpty())
                xmlWriter.addAttribute("number:country", country);

            xmlWriter.addTextNode(symbolRegEx.cap(1));
            xmlWriter.endElement();
        }

        QString elementContents = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());
        style.addChildElement("number", elementContents);
        return q->mainStyles->lookup(style, "N");
    }

    QString vf = valueFormat;
    QString locale = extractLocale(vf);
    Q_UNUSED(locale);
    const QString _vf = removeEscaped(vf);

    // dates
    QRegExp dateRegEx("(d|M|y)");   // we don't check for 'm' cause this can be 'month' or 'minute' and if nothing else is defined we assume 'minute'...
    if (dateRegEx.indexIn(_vf) >= 0) {
        KoGenStyle style(KoGenStyle::StyleNumericDate);
        QBuffer buffer;
        buffer.open(QIODevice::WriteOnly);
        KoXmlWriter xmlWriter(&buffer);    // TODO pass indentation level

        QString numberText;
        int lastPos = -1;
        while (++lastPos < vf.count()) {
            if (vf[lastPos] == 'd' || vf[lastPos] == 'm' || vf[lastPos] == 'M' || vf[lastPos] == 'y') break;
            numberText += vf[lastPos];
        }
        processNumberText(&xmlWriter, numberText);

        while (++lastPos < vf.count()) {
            if (vf[lastPos] == 'd') { // day
                processNumberText(&xmlWriter, numberText);
                const bool isLong = lastPos + 1 < vf.count() && vf[lastPos + 1] == 'd';
                if (isLong) ++lastPos;
                xmlWriter.startElement("number:day");
                xmlWriter.addAttribute("number:style", isLong ? "long" : "short");
                xmlWriter.endElement();  // number:day
            } else if (vf[lastPos] == 'm' || vf[lastPos] == 'M') { // month
                processNumberText(&xmlWriter, numberText);
                const int length = (lastPos + 2 < vf.count() && (vf[lastPos + 2] == 'm' || vf[lastPos + 2] == 'M')) ? 2
                                   : (lastPos + 1 < vf.count() && (vf[lastPos + 1] == 'm' || vf[lastPos + 1] == 'M')) ? 1
                                   : 0;
                xmlWriter.startElement("number:month");
                xmlWriter.addAttribute("number:textual", length == 2 ? "true" : "false");
                xmlWriter.addAttribute("number:style", length == 1 ? "long" : "short");
                xmlWriter.endElement();  // number:month
                lastPos += length;
            } else if (vf[lastPos] == 'y') { // year
                processNumberText(&xmlWriter, numberText);
                const int length = (lastPos + 3 < vf.count() && vf[lastPos + 3] == 'y') ? 3
                                   : (lastPos + 2 < vf.count() && vf[lastPos + 2] == 'y') ? 2
                                   : (lastPos + 1 < vf.count() && vf[lastPos + 1] == 'y') ? 1 : 0;
                xmlWriter.startElement("number:year");
                xmlWriter.addAttribute("number:style", length >= 3 ? "long" : "short");
                xmlWriter.endElement();  // number:year
                lastPos += length;
            } else {
                numberText += vf[lastPos];
            }
        }
        processNumberText(&xmlWriter, numberText);

        QString elementContents = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());
        style.addChildElement("number", elementContents);

        qDebug() << elementContents;
        return q->mainStyles->lookup(style, "N");
    }

    /*
    QRegExp dateRegEx("(m{1,3}|d{1,2}|yy|yyyy)(/|-|\\\\-)(m{1,3}|d{1,2}|yy|yyyy)(?:(/|-|\\\\-)(m{1,3}|d{1,2}|yy|yyyy))?");
    if( dateRegEx.exactMatch(valueFormat) )
    {
      KoGenStyle style(KoGenStyle::StyleNumericDate);
      QBuffer buffer;
      buffer.open(QIODevice::WriteOnly);
      KoXmlWriter elementWriter(&buffer);    // TODO pass indentation level

      processDateFormatComponent( &elementWriter, dateRegEx.cap(1) );
      processDateFormatSeparator( &elementWriter, dateRegEx.cap(2) );
      processDateFormatComponent( &elementWriter, dateRegEx.cap(3) );
      if( dateRegEx.cap(4).length() )
      {
        processDateFormatSeparator( &elementWriter, dateRegEx.cap(4) );
        processDateFormatComponent( &elementWriter, dateRegEx.cap(5) );
      }

      QString elementContents = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());
      style.addChildElement("number", elementContents);

      return q->mainStyles->lookup(style, "N");
    }
    */

    // times
    QRegExp timeRegEx("(h|hh|H|HH|m|s)");
    if (timeRegEx.indexIn(_vf) >= 0) {
        KoGenStyle style(KoGenStyle::StyleNumericTime);
        QBuffer buffer;
        buffer.open(QIODevice::WriteOnly);
        KoXmlWriter xmlWriter(&buffer);    // TODO pass indentation level

        // look for hours, minutes or seconds. Not for AM/PM cause we need at least one value before.
        QString numberText;
        int lastPos = -1;
        while (++lastPos < vf.count()) {
            if (vf[lastPos] == 'h' || vf[lastPos] == 'H' || vf[lastPos] == 'm' || vf[lastPos] == 's') break;
            numberText += vf[lastPos];
        }
        if (! numberText.isEmpty()) {
            xmlWriter.startElement("number:text");
            xmlWriter.addTextNode(numberText);
            xmlWriter.endElement();  // number:text
            numberText.clear();
        }
        if (lastPos < vf.count()) {
            // take over hours if defined
            if (vf[lastPos] == 'h' || vf[lastPos] == 'H') {
                const bool isLong = ++lastPos < vf.count() && (vf[lastPos] == 'h' || vf[lastPos] == 'H');
                if (! isLong) --lastPos;
                xmlWriter.startElement("number:hours");
                xmlWriter.addAttribute("number:style", isLong ? "long" : "short");
                xmlWriter.endElement();  // number:hours

                // look for minutes, seconds or AM/PM definition
                while (++lastPos < vf.count()) {
                    if (vf[lastPos] == 'm' || vf[lastPos] == 's') break;
                    const QString s = vf.mid(lastPos);
                    if (s.startsWith("AM/PM") || s.startsWith("am/pm")) break;
                    numberText += vf[lastPos];
                }
                if (! numberText.isEmpty()) {
                    xmlWriter.startElement("number:text");
                    xmlWriter.addTextNode(numberText);
                    xmlWriter.endElement();  // number:text

                    numberText.clear();
                }
            }
        }

        if (lastPos < vf.count()) {

            // taker over minutes if defined
            if (vf[lastPos] == 'm') {
                const bool isLong = ++lastPos < vf.count() && vf[lastPos] == 'm';
                if (! isLong) --lastPos;
                xmlWriter.startElement("number:minutes");
                xmlWriter.addAttribute("number:style", isLong ? "long" : "short");
                xmlWriter.endElement();  // number:hours

                // look for seconds or AM/PM definition
                while (++lastPos < vf.count()) {
                    if (vf[lastPos] == 's') break;
                    const QString s = vf.mid(lastPos);
                    if (s.startsWith("AM/PM") || s.startsWith("am/pm")) break;
                    numberText += vf[lastPos];
                }
                if (! numberText.isEmpty()) {
                    xmlWriter.startElement("number:text");
                    xmlWriter.addTextNode(numberText);
                    xmlWriter.endElement();  // number:text
                    numberText.clear();
                }
            }
        }

        if (lastPos < vf.count()) {
            // taker over seconds if defined
            if (vf[lastPos] == 's') {
                const bool isLong = ++lastPos < vf.count() && vf[lastPos] == 's';
                if (! isLong) --lastPos;
                xmlWriter.startElement("number:seconds");
                xmlWriter.addAttribute("number:style", isLong ? "long" : "short");
                xmlWriter.endElement();  // number:hours

                // look for AM/PM definition
                while (++lastPos < vf.count()) {
                    const QString s = vf.mid(lastPos);
                    if (s.startsWith("AM/PM") || s.startsWith("am/pm")) break;
                    numberText += vf[lastPos];
                }
                if (! numberText.isEmpty()) {
                    xmlWriter.startElement("number:text");
                    xmlWriter.addTextNode(numberText);
                    xmlWriter.endElement();  // number:text
                    numberText.clear();
                }
            }

            // take over AM/PM definition if defined
            const QString s = vf.mid(lastPos);
            if (s.startsWith("AM/PM") || s.startsWith("am/pm")) {
                xmlWriter.startElement("number:am-pm");
                xmlWriter.endElement();  // number:am-pm
                lastPos += 4;
            }
        }

        // and take over remaining text
        if (++lastPos < vf.count()) {
            xmlWriter.startElement("number:text");
            xmlWriter.addTextNode(vf.mid(lastPos));
            xmlWriter.endElement();  // number:text
        }

        QString elementContents = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());
        style.addChildElement("number", elementContents);
        return q->mainStyles->lookup(style, "N");
    }


    /*
    else if( valueFormat == "h:mm AM/PM" )
    {
      KoGenStyle style(KoGenStyle::StyleNumericTime);
      QBuffer buffer;
      buffer.open(QIODevice::WriteOnly);
      KoXmlWriter xmlWriter(&buffer);    // TODO pass indentation level

      xmlWriter.startElement( "number:hours" );
      xmlWriter.addAttribute( "number:style", "short" );
      xmlWriter.endElement();  // number:hour

      xmlWriter.startElement( "number:text");
      xmlWriter.addTextNode( ":" );
      xmlWriter.endElement();  // number:text

      xmlWriter.startElement( "number:minutes" );
      xmlWriter.addAttribute( "number:style", "long" );
      xmlWriter.endElement();  // number:minutes

      xmlWriter.startElement( "number:text");
      xmlWriter.addTextNode( " " );
      xmlWriter.endElement();  // number:text

      xmlWriter.startElement( "number:am-pm" );
      xmlWriter.endElement();  // number:am-pm

      QString elementContents = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());
      style.addChildElement("number", elementContents);

      return q->mainStyles->lookup(style, "N");
    }
    else if( valueFormat == "h:mm:ss AM/PM" )
    {
      KoGenStyle style(KoGenStyle::StyleNumericTime);
      QBuffer buffer;
      buffer.open(QIODevice::WriteOnly);
      KoXmlWriter xmlWriter(&buffer);    // TODO pass indentation level

      xmlWriter.startElement( "number:hours" );
      xmlWriter.addAttribute( "number:style", "short" );
      xmlWriter.endElement();  // number:hour

      xmlWriter.startElement( "number:text");
      xmlWriter.addTextNode( ":" );
      xmlWriter.endElement();  // number:text

      xmlWriter.startElement( "number:minutes" );
      xmlWriter.addAttribute( "number:style", "long" );
      xmlWriter.endElement();  // number:minutes

      xmlWriter.startElement( "number:text");
      xmlWriter.addTextNode( ":" );
      xmlWriter.endElement();  // number:text

      xmlWriter.startElement( "number:seconds" );
      xmlWriter.addAttribute( "number:style", "long" );
      xmlWriter.endElement();  // number:minutes

      xmlWriter.startElement( "number:text");
      xmlWriter.addTextNode( " " );
      xmlWriter.endElement();  // number:text

      xmlWriter.startElement( "number:am-pm" );
      xmlWriter.endElement();  // number:am-pm

      QString elementContents = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());
      style.addChildElement("number", elementContents);

      return q->mainStyles->lookup(style, "N");
    }
    else if( valueFormat == "h:mm" )
    {
      KoGenStyle style(KoGenStyle::StyleNumericTime);
      QBuffer buffer;
      buffer.open(QIODevice::WriteOnly);
      KoXmlWriter xmlWriter(&buffer);    // TODO pass indentation level

      xmlWriter.startElement( "number:hours" );
      xmlWriter.addAttribute( "number:style", "short" );
      xmlWriter.endElement();  // number:hour

      xmlWriter.startElement( "number:text");
      xmlWriter.addTextNode( ":" );
      xmlWriter.endElement();  // number:text

      xmlWriter.startElement( "number:minutes" );
      xmlWriter.addAttribute( "number:style", "long" );
      xmlWriter.endElement();  // number:minutes

      QString elementContents = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());
      style.addChildElement("number", elementContents);

      return q->mainStyles->lookup(style, "N");
    }
    else if( valueFormat == "h:mm:ss" )
    {
      KoGenStyle style(KoGenStyle::StyleNumericTime);
      QBuffer buffer;
      buffer.open(QIODevice::WriteOnly);
      KoXmlWriter xmlWriter(&buffer);    // TODO pass indentation level

      xmlWriter.startElement( "number:hours" );
      xmlWriter.addAttribute( "number:style", "short" );
      xmlWriter.endElement();  // number:hour

      xmlWriter.startElement( "number:text");
      xmlWriter.addTextNode( ":" );
      xmlWriter.endElement();  // number:text

      xmlWriter.startElement( "number:minutes" );
      xmlWriter.addAttribute( "number:style", "long" );
      xmlWriter.endElement();  // number:minutes

      xmlWriter.startElement( "number:text");
      xmlWriter.addTextNode( ":" );
      xmlWriter.endElement();  // number:text

      xmlWriter.startElement( "number:seconds" );
      xmlWriter.addAttribute( "number:style", "long" );
      xmlWriter.endElement();  // number:minutes

      QString elementContents = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());
      style.addChildElement("number", elementContents);

      return q->mainStyles->lookup(style, "N");
    }
    else if( valueFormat == "[h]:mm" )
    {
      KoGenStyle style(KoGenStyle::StyleNumericTime);
      QBuffer buffer;
      buffer.open(QIODevice::WriteOnly);
      KoXmlWriter xmlWriter(&buffer);    // TODO pass indentation level

      style.addAttribute( "number:truncate-on-overflow", "false" );

      xmlWriter.startElement( "number:hours" );
      xmlWriter.addAttribute( "number:style", "short" );
      xmlWriter.endElement();  // number:hour

      xmlWriter.startElement( "number:text");
      xmlWriter.addTextNode( ":" );
      xmlWriter.endElement();  // number:text

      xmlWriter.startElement( "number:minutes" );
      xmlWriter.addAttribute( "number:style", "long" );
      xmlWriter.endElement();  // number:minutes

      QString elementContents = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());
      style.addChildElement("number", elementContents);

      return q->mainStyles->lookup(style, "N");
    }
    else if( valueFormat == "[h]:mm:ss" )
    {
      KoGenStyle style(KoGenStyle::StyleNumericTime);
      QBuffer buffer;
      buffer.open(QIODevice::WriteOnly);
      KoXmlWriter xmlWriter(&buffer);    // TODO pass indentation level

      style.addAttribute( "number:truncate-on-overflow", "false" );

      xmlWriter.startElement( "number:hours" );
      xmlWriter.addAttribute( "number:style", "short" );
      xmlWriter.endElement();  // number:hour

      xmlWriter.startElement( "number:text");
      xmlWriter.addTextNode( ":" );
      xmlWriter.endElement();  // number:text

      xmlWriter.startElement( "number:minutes" );
      xmlWriter.addAttribute( "number:style", "long" );
      xmlWriter.endElement();  // number:minutes

      xmlWriter.startElement( "number:text");
      xmlWriter.addTextNode( ":" );
      xmlWriter.endElement();  // number:text

      xmlWriter.startElement( "number:seconds" );
      xmlWriter.addAttribute( "number:style", "long" );
      xmlWriter.endElement();  // number:minutes

      QString elementContents = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());
      style.addChildElement("number", elementContents);

      return q->mainStyles->lookup(style, "N");
    }
    else if( valueFormat == "mm:ss" )
    {
      KoGenStyle style(KoGenStyle::StyleNumericTime);
      QBuffer buffer;
      buffer.open(QIODevice::WriteOnly);
      KoXmlWriter xmlWriter(&buffer);    // TODO pass indentation level

      xmlWriter.startElement( "number:minutes" );
      xmlWriter.addAttribute( "number:style", "long" );
      xmlWriter.endElement();  // number:minutes

      xmlWriter.startElement( "number:text");
      xmlWriter.addTextNode( ":" );
      xmlWriter.endElement();  // number:text

      xmlWriter.startElement( "number:seconds" );
      xmlWriter.addAttribute( "number:style", "long" );
      xmlWriter.endElement();  // number:minutes

      QString elementContents = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());
      style.addChildElement("number", elementContents);

      return q->mainStyles->lookup(style, "N");
    }
    else if( valueFormat == "mm:ss.0" )
    {
      KoGenStyle style(KoGenStyle::StyleNumericTime);
      QBuffer buffer;
      buffer.open(QIODevice::WriteOnly);
      KoXmlWriter xmlWriter(&buffer);    // TODO pass indentation level

      xmlWriter.startElement( "number:minutes" );
      xmlWriter.addAttribute( "number:style", "long" );
      xmlWriter.endElement();  // number:minutes

      xmlWriter.startElement( "number:text");
      xmlWriter.addTextNode( ":" );
      xmlWriter.endElement();  // number:text

      xmlWriter.startElement( "number:seconds" );
      xmlWriter.addAttribute( "number:style", "long" );

      xmlWriter.endElement();  // number:minutes
      xmlWriter.startElement( "number:text");
      xmlWriter.addTextNode( ".0" );
      xmlWriter.endElement();  // number:text


      QString elementContents = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());
      style.addChildElement("number", elementContents);

      return q->mainStyles->lookup(style, "N");
    }
    else if( valueFormat == "[mm]:ss" )
    {
      KoGenStyle style(KoGenStyle::StyleNumericTime);
      QBuffer buffer;
      buffer.open(QIODevice::WriteOnly);
      KoXmlWriter xmlWriter(&buffer);    // TODO pass indentation level

      style.addAttribute( "number:truncate-on-overflow", "false" );

      xmlWriter.startElement( "number:minutes" );
      xmlWriter.addAttribute( "number:style", "long" );
      xmlWriter.endElement();  // number:minutes

      xmlWriter.startElement( "number:text");
      xmlWriter.addTextNode( ":" );
      xmlWriter.endElement();  // number:textexactMatch

      xmlWriter.startElement( "number:seconds" );
      xmlWriter.addAttribute( "number:style", "long" );
      xmlWriter.endElement();  // number:minutes

      QString elementContents = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());
      style.addChildElement("number", elementContents);

      return q->mainStyles->lookup(style, "N");
    }
    else if( valueFormat == "[ss]" )
    {
      KoGenStyle style(KoGenStyle::StyleNumericTime);
      QBuffer buffer;
      buffer.open(QIODevice::WriteOnly);
      KoXmlWriter xmlWriter(&buffer);    // TODO pass indentation level

      style.addAttribute( "number:truncate-on-overflow", "false" );

      xmlWriter.startElement( "number:seconds" );
      xmlWriter.addAttribute( "number:style", "long" );
      xmlWriter.endElement();  // number:minutes

      QString elementContents = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());
      style.addChildElement("number", elementContents);

      return q->mainStyles->lookup(style, "N");
    }
    else
    {
    }
    */

    return ""; // generic
}
