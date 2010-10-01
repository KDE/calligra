/*
 * This file is part of Office 2007 Filters for KOffice
 *
 * Copyright (C) 2010 Sebastian Sauer <sebsauer@kdab.com>
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

#include "XlsxXmlWorksheetReader.h"
#include "XlsxXmlCommentsReader.h"
#include "XlsxXmlStylesReader.h"
#include "XlsxXmlDocumentReader.h"
#include "XlsxXmlDrawingReader.h"
#include "XlsxXmlChartReader.h"
#include "XlsxImport.h"
#include "Charting.h"
#include "ChartExport.h"
#include "FormulaParser.h"

#include <MsooXmlRelationships.h>
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
#include <QString>

#include "NumberFormatParser.h"

#define UNICODE_EUR 0x20AC
#define UNICODE_GBP 0x00A3
#define UNICODE_JPY 0x00A5

#undef  MSOOXML_CURRENT_NS // tags without namespace
#define MSOOXML_CURRENT_CLASS XlsxXmlWorksheetReader
#define BIND_READ_CLASS MSOOXML_CURRENT_CLASS

#include <MsooXmlReader_p.h>

#include <math.h>

#include <MsooXmlCommonReaderImpl.h> // this adds p, pPr, t, r, etc.

#define NO_DRAWINGML_NS
#define NO_DRAWINGML_PIC_NS // DrawingML/Picture

#include <MsooXmlCommonReaderDrawingMLImpl.h> // this adds pic, etc.

#include "XlsxXmlWorksheetReader_p.h"

XlsxXmlWorksheetReaderContext::XlsxXmlWorksheetReaderContext(
    uint _worksheetNumber,
    const QString& _worksheetName,
    const QString& _state,
    const QString _path, const QString _file,
    /*QMap<QString, */MSOOXML::DrawingMLTheme*/*>*/& _themes,
    const XlsxSharedStringVector& _sharedStrings,
    const XlsxComments& _comments,
    const XlsxStyles& _styles,
    MSOOXML::MsooXmlRelationships& _relationships,
    XlsxImport* _import,
    int& numberOfOleObjects
)
        : MSOOXML::MsooXmlReaderContext(&_relationships)
        , sheet(new Sheet(_worksheetName))
        , worksheetNumber(_worksheetNumber)
        , worksheetName(_worksheetName)
        , state(_state)
        , themes(_themes)
        , sharedStrings(&_sharedStrings)
        , comments(&_comments)
        , styles(&_styles)
        , import(_import)
        , path(_path)
        , file(_file)
        , numberOfOleObjects(numberOfOleObjects)
{
}

XlsxXmlWorksheetReaderContext::~XlsxXmlWorksheetReaderContext()
{
    delete sheet;
}

const char* XlsxXmlWorksheetReader::officeValue = "office:value";
const char* XlsxXmlWorksheetReader::officeDateValue = "office:date-value";
const char* XlsxXmlWorksheetReader::officeStringValue = "office:string-value";
const char* XlsxXmlWorksheetReader::officeTimeValue = "office:time-value";
const char* XlsxXmlWorksheetReader::officeBooleanValue = "office:boolean-value";

class XlsxXmlWorksheetReader::Private
{
public:
    Private( XlsxXmlWorksheetReader* qq )
     : q( qq ),
       warningAboutWorksheetSizeDisplayed(false),
       drawingNumber(0),
       numberOfOleObjects(0)
    {
    }

    XlsxXmlWorksheetReader* const q;
    QString processValueFormat( const QString& valueFormat );
    bool warningAboutWorksheetSizeDisplayed;
    int drawingNumber;
    QHash<int, Cell*> sharedFormulas;
    int numberOfOleObjects;
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
    initInternal(); // MsooXmlCommonReaderImpl.h
    initDrawingML();
    m_defaultNamespace = "";
    m_columnCount = 0;
    m_currentRow = 0;
    m_currentColumn = 0;
}

KoFilter::ConversionStatus XlsxXmlWorksheetReader::read(MSOOXML::MsooXmlReaderContext* context)
{
    m_context = dynamic_cast<XlsxXmlWorksheetReaderContext*>(context);
    Q_ASSERT(m_context);
    const KoFilter::ConversionStatus result = readInternal();
    m_context = 0;

    return result;
}

KoFilter::ConversionStatus XlsxXmlWorksheetReader::readInternal()
{
    kDebug() << "=============================";
    Q_ASSERT(m_context);

    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }

    // worksheet
    readNext();
    //kDebug() << *this << namespaceUri();

    if (!expectEl("worksheet")) {
        return KoFilter::WrongFormat;
    }
    if (!expectNS(MSOOXML::Schemas::spreadsheetml)) {
        return KoFilter::WrongFormat;
    }

    m_context->sheet->setVisible( m_context->state.toLower() != "hidden" );

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

inline static QString encodeLabelText(int col, int row)
{
    return KSpread::Util::encodeColumnLabelText(col) + QString::number(row);
}

void XlsxXmlWorksheetReader::saveAnnotation(int col, int row)
{
    QString ref(encodeLabelText(col + 1, row + 1));
    kDebug() << ref;
    XlsxComment *comment = m_context->comments->value(ref);
    if (!comment)
        return;
    kDebug() << "Saving annotation for cell" << ref;
    body->startElement("office:annotation");
    body->startElement("dc:creator");
    body->addTextNode(comment->author(m_context->comments));
    body->endElement(); // dc:creator
    //! @todo support dc:date
    foreach (const QString& text, comment->texts) {
        body->startElement("text:p");
        body->addTextSpan(text);
        body->endElement(); // text:p
    }
    body->endElement(); // office:annotation
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
 - [done] drawing (Drawing) §18.3.1.36
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
 - [done] picture (Background Image) §18.3.1.67
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

    m_tableStyle = KoGenStyle(KoGenStyle::TableAutoStyle, "table");
//! @todo hardcoded master page name
    m_tableStyle.addAttribute("style:master-page-name",
                              QString("PageStyle_5f_Test_20_sheet_20__5f_%1").arg(m_context->worksheetNumber));

    m_tableStyle.addProperty("table:display", m_context->sheet->visible());

    //The style might be changed depending on what elements we find,
    //hold the body writer so that we can set the proper style
    KoXmlWriter* heldBody = body;
    QBuffer* bodyBuffer = new QBuffer();
    bodyBuffer->open(QIODevice::ReadWrite);
    body = new KoXmlWriter(bodyBuffer);

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(sheetFormatPr)
            ELSE_TRY_READ_IF(cols)
            ELSE_TRY_READ_IF(sheetData) // does fill the m_context->sheet
            ELSE_TRY_READ_IF(mergeCells)
            ELSE_TRY_READ_IF(drawing)
            ELSE_TRY_READ_IF(hyperlinks)
            ELSE_TRY_READ_IF(picture)
            ELSE_TRY_READ_IF(oleObjects)
        }
    }

    bodyBuffer->close();

    if( !m_context->sheet->pictureBackgroundPath().isNull() ) {
        QBuffer buffer;
        buffer.open(QIODevice::WriteOnly);
        KoXmlWriter writer(&buffer);

        writer.startElement("style:background-image");
        writer.addAttribute("xlink:href", m_context->sheet->pictureBackgroundPath());
        writer.addAttribute("xlink:type", "simple");
        writer.addAttribute("xlink:show", "embed");
        writer.addAttribute("xlink:actuate", "onLoad");
        writer.endElement();

        buffer.close();
        m_tableStyle.addChildElement("style:background-image", QString::fromUtf8(buffer.buffer(), buffer.buffer().size()));
    }

    const QString currentTableStyleName(mainStyles->insert(m_tableStyle, "ta"));
    heldBody->addAttribute("table:style-name", currentTableStyleName);
    heldBody->addCompleteElement(bodyBuffer);
    delete body;
    delete bodyBuffer;
    body = heldBody;

    // now we have everything to start writing the actual cells
    for(int c = 0; c <= m_context->sheet->maxColumn(); ++c) {
        body->startElement("table:table-column");
        if (Column* column = m_context->sheet->column(c, false)) {
            //xmlWriter->addAttribute("table:default-cell-style-name", defaultStyleName);
            if (column->hidden) {
                body->addAttribute("table:visibility", "collapse");
            }
            //xmlWriter->addAttribute("table:number-columns-repeated", );
            //xmlWriter->addAttribute("table:style-name", styleName);
        }
        body->endElement();  // table:table-column
    }
    const int rowCount = m_context->sheet->maxRow();
    for(int r = 0; r <= rowCount; ++r) {
        body->startElement("table:table-row");
        if (Row* row = m_context->sheet->row(r, false)) {

            if (!row->styleName.isEmpty()) {
                body->addAttribute("table:style-name", row->styleName);
            }
            if (row->hidden) {
                body->addAttribute("table:visibility", "collapse");
            }
            //body->addAttribute("table:number-rows-repeated", QByteArray::number(row->repeated));

            const int columnCount = m_context->sheet->maxCellsInRow(r);
            for(int c = 0; c <= columnCount; ++c) {
                body->startElement("table:table-cell");
                if (Cell* cell = m_context->sheet->cell(c, r, false)) {
                    const bool hasHyperlink = ! cell->hyperlink.isEmpty();

                    if (!cell->styleName.isEmpty()) {
                        body->addAttribute("table:style-name", cell->styleName);
                    }
                    //body->addAttribute("table:number-columns-repeated", QByteArray::number(cell->repeated));
                    if (!hasHyperlink && !cell->valueType.isEmpty()) {
                        body->addAttribute("office:value-type", cell->valueType);
                    }
                    if (!cell->valueAttr.isEmpty()) {
                        // Treat boolean values specially (ODF1.1 chapter 6.7.1)
                        if (cell->valueAttr == XlsxXmlWorksheetReader::officeBooleanValue)
                            //! @todo This breaks down if the value is a formula and not constant.
                            body->addAttribute(cell->valueAttr,
                                               cell->valueAttrValue == "0" ? "false" : "true");
                        else
                            body->addAttribute(cell->valueAttr, cell->valueAttrValue);
                    }
                    if (!cell->formula.isEmpty()) {
                        body->addAttribute("table:formula", cell->formula);
                    }
                    if (cell->rowsMerged > 1) {
                        body->addAttribute("table:number-rows-spanned", cell->rowsMerged);
                    }
                    if (cell->columnsMerged > 1) {
                        body->addAttribute("table:number-columns-spanned", cell->columnsMerged);
                    }

                    saveAnnotation(c, r);

                    if (!cell->text.isEmpty() || !cell->charStyleName.isEmpty() || hasHyperlink) {
                        body->startElement("text:p", false);
                        if (!cell->charStyleName.isEmpty()) {
                            body->startElement( "text:span" );
                            body->addAttribute( "text:style-name", cell->charStyleName);
                        }
                        if (hasHyperlink) {
                            body->startElement("text:a");
                            body->addAttribute("xlink:href", cell->hyperlink);
                            //body->addAttribute("office:target-frame-name", targetFrameName);
                            if(cell->text.isEmpty())
                                body->addTextNode(cell->hyperlink);
                            else if(cell->isPlainText)
                                body->addTextNode(cell->text);
                            else 
                                body->addCompleteElement(cell->text.toUtf8());
                            body->endElement(); // text:a
                        } else if (!cell->text.isEmpty()) {
                            if(cell->isPlainText)
                                body->addTextSpan(cell->text);
                            else
                                body->addCompleteElement(cell->text.toUtf8());
                        }
                        if (!cell->charStyleName.isEmpty()) {
                            body->endElement(); // text:span
                        }
                        body->endElement(); // text:p
                    }

                    // handle drawing objects like e.g. charts, diagrams and pictures
                    foreach(XlsxDrawingObject* drawing, cell->drawings) {
                        drawing->save(body);
                    }

                    QPair<QString,QString> oleObject;
                    foreach( oleObject, cell->oleObjects ) {
                        const QString olePath = oleObject.first;
                        const QString previewPath = oleObject.second;
                        body->startElement("draw:frame");
                        //TODO find out the proper values
                        body->addAttribute("svg:x", "1cm");
                        body->addAttribute("svg:y", "1cm");
                        body->addAttribute("svg:width", "5cm");
                        body->addAttribute("svg:height", "5cm");

                        body->startElement("draw:object-ole");
                        body->addAttribute("xlink:href", olePath);
                        body->addAttribute("xlink:type", "simple");
                        body->addAttribute("xlink:show", "embed");
                        body->addAttribute("xlink:actuate", "onLoad");
                        body->endElement(); // draw:object-ole

                        body->startElement("draw:image");
                        body->addAttribute("xlink:href", previewPath);
                        body->addAttribute("xlink:type", "simple");
                        body->addAttribute("xlink:show", "embed");
                        body->addAttribute("xlink:actuate", "onLoad");
                        body->endElement(); // draw:image

                        body->endElement(); // draw:frame
                    }
                }
                body->endElement(); // table:table-cell
            }
        }
        body->endElement(); // table:table-row
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
    TRY_READ_ATTR_WITHOUT_NS(defaultRowHeight) // in pt
    TRY_READ_ATTR_WITHOUT_NS(defaultColWidth)
    TRY_READ_ATTR_WITHOUT_NS(baseColWidth)
    bool ok;
    const double drh = defaultRowHeight.toDouble(&ok);
    if(ok) m_context->sheet->m_defaultRowHeight = drh;
    const double dcw = defaultColWidth.toDouble(&ok);
    if(ok) m_context->sheet->m_defaultColWidth = dcw;
    const double bcw = baseColWidth.toDouble(&ok);
    if(ok) m_context->sheet->m_baseColWidth = bcw;
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
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(col)
            ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE_WITHOUT_RETURN

    // append remaining empty columns
    appendTableColumns(MSOOXML::maximumSpreadsheetColumns() - m_columnCount);
    return KoFilter::OK;
}

//! Saves information about column style
void XlsxXmlWorksheetReader::saveColumnStyle(const QString& widthString)
{
    KoGenStyle tableColumnStyle(KoGenStyle::TableColumnAutoStyle, "table-column");
    tableColumnStyle.addProperty("style:column-width", widthString);
    tableColumnStyle.addProperty("fo:break-before", "auto");

    const QString currentTableColumnStyleName(mainStyles->insert(tableColumnStyle, "co"));
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

    Column* column = m_context->sheet->column(m_columnCount, true);
    ++m_columnCount;

//moved    body->startElement("table:table-column"); // CASE #S2500?
    int minCol = m_columnCount;
    int maxCol = m_columnCount;
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

    TRY_READ_ATTR_WITHOUT_NS(hidden)
    if (!hidden.isEmpty()) {
        column->hidden = hidden.toInt() > 0;
    }

    SKIP_EVERYTHING

//moved    body->endElement(); // table:table-column
    appendTableColumns(maxCol - minCol + 1, realWidthString);

    m_columnCount += (maxCol - minCol);

    if (m_columnCount > (int)MSOOXML::maximumSpreadsheetColumns()) {
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
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(row)
            ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

QString XlsxXmlWorksheetReader::processRowStyle(const QString& _heightString)
{
    double height = -1.0;
    if(!_heightString.isEmpty()) {
        bool ok;
        height = _heightString.toDouble(&ok);
        if(!ok) height = -1.0;
    } else {
        height = m_context->sheet->m_defaultRowHeight;
    }
    KoGenStyle tableRowStyle(KoGenStyle::TableRowAutoStyle, "table-row");
//! @todo alter fo:break-before?
    tableRowStyle.addProperty("fo:break-before", MsooXmlReader::constAuto);
//! @todo alter style:use-optimal-row-height?
    tableRowStyle.addProperty("style:use-optimal-row-height", MsooXmlReader::constFalse);
    if (height >= 0.0) {
        tableRowStyle.addProperty("style:row-height", printCm(POINT_TO_CM(height)));
    }
    const QString currentTableRowStyleName(mainStyles->insert(tableRowStyle, "ro"));
    return currentTableRowStyleName;
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
    //TRY_READ_ATTR_WITHOUT_NS(spans) // spans are only an optional help
    TRY_READ_ATTR_WITHOUT_NS(ht)
    TRY_READ_ATTR_WITHOUT_NS(customHeight)
    TRY_READ_ATTR_WITHOUT_NS(hidden)

    if (!r.isEmpty()) {
        bool ok;
        m_currentRow = r.toInt(&ok) - 1;
        if (!ok || m_currentRow < 0)
            return KoFilter::WrongFormat;
    }
    if (m_currentRow > (int)MSOOXML::maximumSpreadsheetRows()) {
        showWarningAboutWorksheetSize();
    }

    m_currentColumn = 0;
    Row* row = m_context->sheet->row(m_currentRow, true);
    row->styleName = processRowStyle(ht);

    if (!hidden.isEmpty()) {
        row->hidden = hidden.toInt() > 0;
    }

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(c) // modifies m_currentColumn
            ELSE_WRONG_FORMAT
        }
    }

    ++m_currentRow; // This row is done now. Select the next row.

    READ_EPILOGUE
}

//! @return true if @a v represents an integer or floating-point number
static bool valueIsNumeric(const QString& v)
{
    bool ok;
    v.toDouble(&ok);
    return ok;
}

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
    Row* row = m_context->sheet->row(m_currentRow, false);
    Q_ASSERT(row);

    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(r)
    if (!r.isEmpty()) {
        m_currentColumn = KSpread::Util::decodeColumnLabelText(r) - 1;
        if (m_currentColumn < 0)
            return KoFilter::WrongFormat;
    }

    TRY_READ_ATTR_WITHOUT_NS(s)
    TRY_READ_ATTR_WITHOUT_NS(t)

    m_value.clear();

    Cell* cell = m_context->sheet->cell(m_currentColumn, m_currentRow, true);

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(f)
            ELSE_TRY_READ_IF(v)
            ELSE_WRONG_FORMAT
        }
    }

    bool ok;
    uint styleId = s.toUInt(&ok);
    kDebug() << "styleId:" << styleId;
    const XlsxCellFormat* cellFormat = m_context->styles->cellFormat(styleId);
    const QString numberFormat = cellFormat->applyNumberFormat ? m_context->styles->numberFormatString( cellFormat->numFmtId ) : QString();

    const QString formattedStyle = d->processValueFormat( numberFormat );
    QString charStyleName;

//    const bool addTextPElement = true;//m_value.isEmpty() || t != QLatin1String("s");

    if (!m_value.isEmpty()) {
        KoCharacterStyle cellCharacterStyle;
        cellFormat->setupCharacterStyle(m_context->styles, &cellCharacterStyle);

        if( cellCharacterStyle.verticalAlignment() == QTextCharFormat::AlignSuperScript 
            || cellCharacterStyle.verticalAlignment() == QTextCharFormat::AlignSubScript ) {
            KoGenStyle charStyle( KoGenStyle::TextStyle, "text" );
            cellCharacterStyle.saveOdf( charStyle );
            charStyleName = mainStyles->insert( charStyle, "T" );
        }

        if( !charStyleName.isEmpty() ) {
            cell->charStyleName = charStyleName;
        }

        /* depending on type: 18.18.11 ST_CellType (Cell Type), p. 2679:
            b (Boolean)  Cell containing a boolean.
            d (Date)     Cell contains a date in the ISO 8601 format.
            e (Error)    Cell containing an error.
            inlineStr    (Inline String) Cell containing an (inline) rich string, i.e. 
                         one not in the shared string table. If this cell type is used,
                         then the cell value is in the is element rather than the v
                         element in the cell (c element).
            n (Number)   Cell containing a number.
            s (Shared String) Cell containing a shared string.
            str (String) Cell containing a formula string.

            Converting into values described in ODF1.1: "6.7.1. Variable Value Types and Values".
        */

        if (t == QLatin1String("s")) {
            bool ok;
            const int stringIndex = m_value.toInt(&ok);
            if (!ok || stringIndex < 0 || stringIndex >= m_context->sharedStrings->size()) {
                return KoFilter::WrongFormat;
            }
            XlsxSharedString sharedstring = m_context->sharedStrings->at(stringIndex);
            cell->text = sharedstring.data();
            cell->isPlainText = sharedstring.isPlainText();
            cell->valueType = MsooXmlReader::constString;
            // no valueAttr
        } else if ((t.isEmpty() && !valueIsNumeric(m_value)) || t == QLatin1String("inlineStr")) {
//! @todo handle value properly
            cell->text = m_value;
            cell->valueType = MsooXmlReader::constString;
            // no valueAttr
        } else if (t == QLatin1String("b")) {
            cell->text = m_value;
            cell->valueType = MsooXmlReader::constBoolean;
            cell->valueAttr = XlsxXmlWorksheetReader::officeBooleanValue;
        } else if (t == QLatin1String("d")) {
//! @todo handle value properly
            cell->text = m_value;
            cell->valueType = MsooXmlReader::constDate;
            cell->valueAttr = XlsxXmlWorksheetReader::officeDateValue;
        } else if (t == QLatin1String("str")) {
//! @todo handle value properly
            cell->text = m_value;
            cell->valueType = MsooXmlReader::constString;
            // no valueAttr
        } else if (t == QLatin1String("n") || t.isEmpty() /* already checked if numeric */) {
            if (!t.isEmpty()) { // sanity check
                if (!valueIsNumeric(m_value)) {
                    raiseError(i18n("Expected integer or floating point number"));
                    return KoFilter::WrongFormat;
                }
            }
            const KoGenStyle* const style = mainStyles->style( formattedStyle );
            if( style == 0 || valueIsNumeric(m_value) ) {
//            body->addTextSpan(m_value);
                cell->valueType = MsooXmlReader::constFloat;
                cell->valueAttr = XlsxXmlWorksheetReader::officeValue;
            } else {
                switch( style->type() ) {
                case KoGenStyle::NumericDateStyle:
                    cell->valueType = MsooXmlReader::constDate;
                    cell->valueAttr = XlsxXmlWorksheetReader::officeDateValue;
                    m_value = QDate( 1899, 12, 30 ).addDays( m_value.toInt() ).toString( Qt::ISODate );
                    break;
                case KoGenStyle::NumericTextStyle:
                    cell->valueType = MsooXmlReader::constString;
                    cell->valueAttr = XlsxXmlWorksheetReader::officeStringValue;
                    break;
                default:
                    cell->valueType = MsooXmlReader::constFloat;
                    cell->valueAttr = XlsxXmlWorksheetReader::officeValue;
                    break;
                }
            }
        } else if (t == QLatin1String("e")) {
            if (m_value == QLatin1String("#REF!"))
                cell->text = "#NAME?";
            else
                cell->text = m_value;
//! @todo full parsing needed to retrieve the type
            cell->valueType = MsooXmlReader::constFloat;
            cell->valueAttr = XlsxXmlWorksheetReader::officeValue;
            m_value = QLatin1String("0");
        } else {
            raiseUnexpectedAttributeValueError(t, "c@t");
            return KoFilter::WrongFormat;
        }
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
        KoGenStyle cellStyle(KoGenStyle::TableCellAutoStyle, "table-cell");

        if( charStyleName.isEmpty() ) {
            KoCharacterStyle cellCharacterStyle;
            cellFormat->setupCharacterStyle(m_context->styles, &cellCharacterStyle);
            cellCharacterStyle.saveOdf(cellStyle);
        }
        if (!cellFormat->setupCellStyle(m_context->styles, m_context->themes, &cellStyle)) {
            return KoFilter::WrongFormat;
        }

        if (!formattedStyle.isEmpty()) {
            cellStyle.addAttribute( "style:data-style-name", formattedStyle );
        }

        const QString cellStyleName = mainStyles->insert( cellStyle, "ce" );
        cell->styleName = cellStyleName;
    }

    cell->valueAttrValue = m_value;

    ++m_currentColumn; // This cell is done now. Select the next cell.

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

 @todo support all elements
*/
KoFilter::ConversionStatus XlsxXmlWorksheetReader::read_f()
{
    Cell* cell = m_context->sheet->cell(m_currentColumn, m_currentRow, false);
    Q_ASSERT(cell);

    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    // Range of cells which the formula applies to. Only required for shared formula, array
    // formula or data table. Only written on the master formula, not subsequent formula's
    // belonging to the same shared group, array, or data table.
    //TRY_READ_ATTR(ref)
    // Type of formula. The possible values defined by the ST_CellFormulaType (§18.18.6), p. 2677
    TRY_READ_ATTR(t)

    // Shared formula groups.
    int sharedGroupIndex = -1;
    if (t == QLatin1String("shared")) {
        TRY_READ_ATTR(si)
        STRING_TO_INT(si, sharedGroupIndex, "f@si")
    }

    while (!atEnd() && !hasError()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isCharacters()) {
            cell->formula = MSOOXML::convertFormula(text().toString());
        }
    }

    if (!t.isEmpty()) {
        if (t == QLatin1String("shared")) {
            if (sharedGroupIndex >= 0) {
                /* Shared Group Index, p. 1815
                Optional attribute to optimize load performance by sharing formulas.
                When a formula is a shared formula (t value is shared) then this value indicates the
                group to which this particular cell's formula belongs. The first formula in a group of
                shared formulas is saved in the f element. This is considered the 'master' formula cell.
                Subsequent cells sharing this formula need not have the formula written in their f
                element. Instead, the attribute si value for a particular cell is used to figure what the
                formula expression should be based on the cell's relative location to the master formula
                cell.
                */
                if (d->sharedFormulas.contains(sharedGroupIndex)) {
                    if (cell->formula.isEmpty()) // don't do anything if the cell already defines a formula
                        cell->formula = MSOOXML::convertFormulaReference(d->sharedFormulas[sharedGroupIndex], cell);
                } else if (!cell->formula.isEmpty()) { // is this cell the master cell?
                    d->sharedFormulas[sharedGroupIndex] = cell;
                }
            }
        }
    }

    /*
    if (!ref.isEmpty()) {
        const int pos = ref.indexOf(':');
        if (pos > 0) {
            const QString fromCell = ref.left(pos);
            const QString toCell = ref.mid(pos + 1);
            const int c1 = KSpread::Util::decodeColumnLabelText(fromCell) - 1;
            const int r1 = KSpread::Util::decodeRowLabelText(fromCell) - 1;
            const int c2 = KSpread::Util::decodeColumnLabelText(toCell) - 1;
            const int r2 = KSpread::Util::decodeRowLabelText(toCell) - 1;
            if (c1 >= 0 && r1 >= 0 && c2 >= c1 && r2 >= r1) {
                for (int col = c1; col <= c2; ++col) {
                    for (int row = r1; row <= r2; ++row) {
                        if (col != m_currentColumn || row != m_currentRow) {
                            if (Cell* c = m_context->sheet->cell(col, row, true))
                                c->formula = convertFormulaReference(cell, c);
                        }
                    }
                }
            }
        }
    }
    */

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
    //kDebug() << m_value;
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
    NumberFormatParser::setStyles( q->mainStyles );
    const KoGenStyle style = NumberFormatParser::parse( valueFormat );
    if( style.type() == KoGenStyle::ParagraphAutoStyle )
        return QString();

    return q->mainStyles->insert( style, "N" );
}

#undef CURRENT_EL
#define CURRENT_EL mergeCell

KoFilter::ConversionStatus XlsxXmlWorksheetReader::read_mergeCell()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(ref)
    QStringList refList = ref.split(':');
    if (refList.count() >= 2) {
        const QString fromCell = refList[0];
        const QString toCell = refList[1];
        QRegExp rx("([A-Za-z]+)([0-9]+)");
        if(rx.exactMatch(fromCell)) {
            const int fromRow = rx.cap(2).toInt() - 1;
            const int fromCol = KSpread::Util::decodeColumnLabelText(fromCell) - 1;
            if(rx.exactMatch(toCell)) {
                Cell* cell = m_context->sheet->cell(fromCol, fromRow, true);
                cell->rowsMerged = rx.cap(2).toInt() - fromRow;
                cell->columnsMerged = KSpread::Util::decodeColumnLabelText(toCell) - fromCol;

                // correctly take right/bottom borders from the cells that are merged into this one
                const KoGenStyle* origCellStyle = mainStyles->style(cell->styleName);
                KoGenStyle cellStyle;
                if (origCellStyle) {
                    cellStyle = *origCellStyle;
                }
                kDebug() << cell->rowsMerged << cell->columnsMerged << cell->styleName;
                if (cell->rowsMerged > 1) {
                    Cell* lastCell = m_context->sheet->cell(fromCol, fromRow + cell->rowsMerged - 1, false);
                    kDebug() << lastCell;
                    if (lastCell) {
                        const KoGenStyle* style = mainStyles->style(lastCell->styleName);
                        kDebug() << lastCell->styleName;
                        if (style) {
                            QString val = style->property("fo:border-bottom");
                            kDebug() << val;
                            if (!val.isEmpty()) cellStyle.addProperty("fo:border-bottom", val);
                            val = style->property("fo:border-line-width-bottom");
                            if (!val.isEmpty()) cellStyle.addProperty("fo:border-line-width-bottom", val);
                        }
                    }
                }
                if (cell->columnsMerged > 1) {
                    Cell* lastCell = m_context->sheet->cell(fromCol + cell->columnsMerged - 1, fromRow, false);
                    if (lastCell) {
                        const KoGenStyle* style = mainStyles->style(lastCell->styleName);
                        if (style) {
                            QString val = style->property("fo:border-right");
                            if (!val.isEmpty()) cellStyle.addProperty("fo:border-right", val);
                            val = style->property("fo:border-line-width-right");
                            if (!val.isEmpty()) cellStyle.addProperty("fo:border-line-width-right", val);
                        }
                    }
                }
                cell->styleName = mainStyles->insert(cellStyle, "ce");
            }
        }
    }
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL mergeCells

KoFilter::ConversionStatus XlsxXmlWorksheetReader::read_mergeCells()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(mergeCell)
            ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL drawing

//! drawing handler (Drawing)
/*! ECMA-376, 18.3.1.36, p.1804.

 This element indicates that the sheet contains drawing components built
 on the drawingML platform. The relationship Id references the part containing
 the drawingML definitions.

 Parent elements:
 - chartsheet (§18.3.1.12)
 - dialogsheet (§18.3.1.34)
 - [done] worksheet (§18.3.1.99)

 Child elements - see DrawingML.
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_drawing()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITH_NS(r, id)
    if(!r_id.isEmpty() && !this->m_context->path.isEmpty()) {
        //! @todo use MSOOXML::MsooXmlRelationships

        QString path = this->m_context->path;
        const int pos = path.indexOf('/', 1);
        if( pos > 0 ) path = path.left(pos);
        path += "/drawings";
        QString file = QString("drawing%1.xml").arg(++d->drawingNumber);
        QString filepath = path + "/" + file;

        XlsxXmlDrawingReaderContext* context = new XlsxXmlDrawingReaderContext(m_context, m_context->sheet, path, file);
        XlsxXmlDrawingReader reader(this);
        const KoFilter::ConversionStatus result = m_context->import->loadAndParseDocument(&reader, filepath, context);
        if (result != KoFilter::OK) {
            raiseError(reader.errorString());
            delete context;
            return result;
        }

#if 0 //TODO
        if (context->m_positions.contains(XlsxDrawingObject::FromAnchor)) {
            XlsxDrawingObject::Position pos = context->m_positions[XlsxDrawingObject::FromAnchor];
            Cell* cell = m_context->sheet->cell(pos.m_col, pos.m_row, true);
            cell->drawings << context;
        } else {
            delete context;
        }
#endif
    }
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL hyperlink

KoFilter::ConversionStatus XlsxXmlWorksheetReader::read_hyperlink()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(ref)
    TRY_READ_ATTR_WITHOUT_NS(location)
    TRY_READ_ATTR_WITH_NS(r, id)
    if (!ref.isEmpty() && (!r_id.isEmpty() || !location.isEmpty())) {
        const int col = KSpread::Util::decodeColumnLabelText(ref) - 1;
        const int row = KSpread::Util::decodeRowLabelText(ref) - 1;
        if(col >= 0 && row >= 0) {
            QString link = m_context->relationships->target(m_context->path, m_context->file, r_id);
            // it follows a hack to get right of the prepended m_context->path...
            if (link.startsWith(m_context->path))
                link = link.mid(m_context->path.length()+1);

            // append location
            if (!location.isEmpty()) link += '#' + location;

            Cell* cell = m_context->sheet->cell(col, row, true);
            cell->hyperlink = link;
        }
    }
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL hyperlinks

KoFilter::ConversionStatus XlsxXmlWorksheetReader::read_hyperlinks()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(hyperlink)
            ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL picture

KoFilter::ConversionStatus XlsxXmlWorksheetReader::read_picture()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITH_NS(r, id)
    const QString link = m_context->relationships->target(m_context->path, m_context->file, r_id);
    QString fileName = link.right( link.lastIndexOf('/') +1 );
    RETURN_IF_ERROR( copyFile(link, "Pictures/", fileName) )
    m_context->sheet->setPictureBackgroundPath(fileName);
    //NOTE manifest entry is added by copyFile

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL oleObjects

KoFilter::ConversionStatus XlsxXmlWorksheetReader::read_oleObjects()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if( isStartElement() ) {
            TRY_READ_IF(oleObject)
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL oleObject

KoFilter::ConversionStatus XlsxXmlWorksheetReader::read_oleObject()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    READ_ATTR_WITH_NS(r, id)
    READ_ATTR_WITHOUT_NS(progId);

    //For now we just copy the preview emf file and the embeded document,
    //later we might convert it into ODF too.
    //Preview files exist only for some files, please add the program generator
    //for the files we will try to copy the preview image from
    if(progId != "PowerPoint.Slide.12")
        return KoFilter::OK;

    ++(m_context->numberOfOleObjects);

    const QString link = m_context->relationships->target(m_context->path, m_context->file, r_id);
    QString fileName = link.right( link.lastIndexOf('/') +1 );
    RETURN_IF_ERROR( copyFile(link, "", fileName) )

    //In the OOXML specification the preview image would be represented as a relationship
    //it is not specified how exactly that relationship is to be picked.
    //However, to make things worse, the relationship seems that is not saved by MS2007,
    //so, we must assume that the images in the media folder are ordered by appearance in the document
    QString previewFileName = QString("image%1.emf").arg(m_context->numberOfOleObjects);
    QString originalPreviewFilePath = "xl/media/" + previewFileName;
    RETURN_IF_ERROR( copyFile(originalPreviewFilePath, "Pictures/", previewFileName) )

    //TODO find out which cell to pick
    Cell* cell = m_context->sheet->cell(0, 0, true);

    cell->oleObjects << qMakePair<QString,QString>(fileName, previewFileName);

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}
