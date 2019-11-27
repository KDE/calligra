/*
 * This file is part of Office 2007 Filters for Calligra
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

// Own
#include "XlsxUtils.h"
#include "XlsxXmlWorksheetReader.h"

#include "XlsxXmlCommentsReader.h"
#include "XlsxXmlStylesReader.h"
#include "XlsxXmlDocumentReader.h"
#include "XlsxXmlDrawingReader.h"
#include "XlsxXmlChartReader.h"
#include "XlsxXmlTableReader.h"
#include "XlsxImport.h"
#include "Charting.h"
#include "XlsxChartOdfWriter.h"
#include "FormulaParser.h"

#include <MsooXmlRelationships.h>
#include <MsooXmlSchemas.h>
#include <MsooXmlUtils.h>
#include <MsooXmlUnits.h>
#include <MsooXmlGlobal.h>

#include <KoUnit.h>
#include <KoXmlWriter.h>
#include <KoGenStyles.h>
#include <styles/KoCharacterStyle.h>

#include <sheets/Util.h>

#include <QBrush>
#include <QRegExp>
#include <QString>
#include <QList>
#include <QCache>

#include "NumberFormatParser.h"

#define XLSXXMLWORKSHEETREADER_CPP

#define UNICODE_EUR 0x20AC
#define UNICODE_GBP 0x00A3
#define UNICODE_JPY 0x00A5

#undef  MSOOXML_CURRENT_NS // tags without namespace
#define MSOOXML_CURRENT_CLASS XlsxXmlWorksheetReader
#define BIND_READ_CLASS MSOOXML_CURRENT_CLASS

#include <MsooXmlReader_p.h>

#include <cmath>
#include <algorithm>

// ----------------------------------------------------------------
// Include implementation of common tags

#include <MsooXmlCommonReaderImpl.h> // this adds p, pPr, t, r, etc.

#undef  MSOOXML_CURRENT_NS // tags without namespace
#define MSOOXML_CURRENT_NS

// ----------------------------------------------------------------

#define NO_DRAWINGML_NS
#define NO_DRAWINGML_PIC_NS // DrawingML/Picture

#include <MsooXmlCommonReaderDrawingMLImpl.h> // this adds pic, etc.

#include "XlsxXmlWorksheetReader_p.h"

XlsxXmlWorksheetReaderContext::XlsxXmlWorksheetReaderContext(
    uint _worksheetNumber,
    uint _numberOfWorkSheets,
    const QString& _worksheetName,
    const QString& _state,
    const QString _path, const QString _file,
    MSOOXML::DrawingMLTheme*& _themes,
    const QVector<QString>& _sharedStrings,
    const XlsxComments& _comments,
    const XlsxStyles& _styles,
    MSOOXML::MsooXmlRelationships& _relationships,
    XlsxImport* _import,
    QMap<QString, QString> _oleReplacements,
    QMap<QString, QString> _oleBeginFrames,
    QVector<XlsxXmlDocumentReaderContext::AutoFilter>& autoFilters)
        : MSOOXML::MsooXmlReaderContext(&_relationships)
        , sheet(new Sheet(_worksheetName))
        , worksheetNumber(_worksheetNumber)
        , numberOfWorkSheets(_numberOfWorkSheets)
        , worksheetName(_worksheetName)
        , state(_state)
        , themes(_themes)
        , sharedStrings(&_sharedStrings)
        , comments(&_comments)
        , styles(&_styles)
        , import(_import)
        , path(_path)
        , file(_file)
        , oleReplacements(_oleReplacements)
        , oleFrameBegins(_oleBeginFrames)
        , autoFilters(autoFilters)
{
}

XlsxXmlWorksheetReaderContext::~XlsxXmlWorksheetReaderContext()
{
    delete sheet;
}

static void splitToRowAndColumn(const char *source, int sourceStart, int sourceLength, QString& row, int& column)
{
    // find the position of the first number
    int pos = 0;
    while (pos < sourceLength) {
        if (source[sourceStart + pos] < 65) {
            break;
        }
        row.append(source[sourceStart + pos]);
        pos++;
    }

    char *pEnd = 0;
    column = strtol(source + sourceStart + pos, &pEnd, 10);
}

//! @return value @a cm with cm suffix
static QString printCm(double cm)
{
    QString string;
    string.sprintf("%3.3fcm", cm);
    return string;
}


QList<QMap<QString, QString> > XlsxXmlWorksheetReaderContext::conditionalStyleForPosition(const QString& positionLetter, int positionNumber)
{
    QString startLetter, endLetter;
    int startNumber, endNumber;

    QList<QMap<QString, QString> > returnMaps;

    // Known positions which are hits/misses for this
    // purpose is to optimize this code part for a large set of conditions
    QList<QString> cachedHits, cachedMisses;

    // We do not wish to add the same condition twice
    QList<QString> addedConditions;

    int index = 0;
    while (index < conditionalStyles.size()) {
        startLetter.clear();
        endLetter.clear();

        QString range = conditionalStyles.at(index).first;
        if (cachedHits.contains(range)) {
            if (!addedConditions.contains(conditionalStyles.at(index).second.value("style:condition"))) {
                returnMaps.push_back(conditionalStyles.at(index).second);
                addedConditions.push_back(conditionalStyles.at(index).second.value("style:condition"));
            }
            ++index;
            continue;
        }
        if (cachedMisses.contains(range)) {
            ++index;
            continue;
        }

        QByteArray ba = range.toLatin1();

        int columnIndex = ba.indexOf(':');
        if (columnIndex < 0) {
            splitToRowAndColumn(ba.constData(), 0, ba.length(), startLetter, startNumber);
            endLetter.clear();
        }
        else {
            splitToRowAndColumn(ba.constData(), 0, columnIndex, startLetter, startNumber);
            splitToRowAndColumn(ba.constData(), columnIndex + 1, ba.size() - (columnIndex + 1), endLetter, endNumber);
        }

        if ((positionLetter == startLetter && positionNumber == startNumber && endLetter.isEmpty()) ||
            (positionLetter >= startLetter && positionNumber >= startNumber &&
             positionLetter <= endLetter && positionNumber <= endNumber)) {
            if (!addedConditions.contains(conditionalStyles.at(index).second.value("style:condition"))) {
                returnMaps.push_back(conditionalStyles.at(index).second);
                addedConditions.push_back(conditionalStyles.at(index).second.value("style:condition"));
            }
            cachedHits.push_back(range);
            ++index;
            continue;
        }
        else {
            cachedMisses.push_back(range);
            ++index;
            continue;
        }
        ++index;
    }

    return returnMaps;
}

const char XlsxXmlWorksheetReader::officeValue[] = "office:value";
const char XlsxXmlWorksheetReader::officeDateValue[] = "office:date-value";
const char XlsxXmlWorksheetReader::officeStringValue[] = "office:string-value";
const char XlsxXmlWorksheetReader::officeTimeValue[] = "office:time-value";
const char XlsxXmlWorksheetReader::officeBooleanValue[] = "office:boolean-value";

class XlsxXmlWorksheetReader::Private
{
public:
    Private( XlsxXmlWorksheetReader* qq )
     : q( qq ),
       warningAboutWorksheetSizeDisplayed(false),
       drawingNumber(0)
    {
    }
    //~Private(){ qDeleteAll( savedStyles ); }

    XlsxXmlWorksheetReader* const q;
    bool warningAboutWorksheetSizeDisplayed;
    int drawingNumber;
    QHash<int, Cell*> sharedFormulas;
    QHash<QString, QString > savedStyles;
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
    qCDebug(lcXlsxImport) << "=============================";
    Q_ASSERT(m_context);

    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }

    // worksheet
    readNext();
    //qCDebug(lcXlsxImport) << *this << namespaceUri();

    if (name() != "worksheet" && name() != "dialogsheet" && name() != "chartsheet") {
        return KoFilter::WrongFormat;
    }
    if (!expectNS(MSOOXML::Schemas::spreadsheetml)) {
        return KoFilter::WrongFormat;
    }

    m_context->sheet->setVisible( m_context->state.toLower() != "hidden" );

    QXmlStreamNamespaceDeclarations namespaces(namespaceDeclarations());
    for (int i = 0; i < namespaces.count(); i++) {
        qCDebug(lcXlsxImport) << "NS prefix:" << namespaces[i].prefix() << "uri:" << namespaces[i].namespaceUri();
    }
//! @todo find out whether the namespace returned by namespaceUri()
//!       is exactly the same ref as the element of namespaceDeclarations()
    if (!namespaces.contains(QXmlStreamNamespaceDeclaration("", MSOOXML::Schemas::spreadsheetml))) {
        raiseError(i18n("Namespace \"%1\" not found", QLatin1String(MSOOXML::Schemas::spreadsheetml)));
        return KoFilter::WrongFormat;
    }
//! @todo expect other namespaces too...

    if (name() == "worksheet") {
        TRY_READ(worksheet)
    }
    else if (name() == "dialogsheet") {
        TRY_READ(dialogsheet)
    }

    qCDebug(lcXlsxImport) << "===========finished============";
    return KoFilter::OK;
}


QString XlsxXmlWorksheetReader::computeColumnWidth(qreal widthNumber) const
{
    //! CASE #S3300
    //! Column width measured as the number of characters of the maximum digit width of the
    //! numbers 0, 1, 2, …, 9 as rendered in the normal style's font. There are 4 pixels of margin
    //! padding (two on each side), plus 1 pixel padding for the gridlines.
    //! For explanation of width, see p. 1778
    //simplified:
    //! @todo hardcoded, not 100% accurate
    qCDebug(lcXlsxImport) << "PT_TO_PX(11.0):" << PT_TO_PX(11.0);
    const double realSize = round(PT_TO_PX(11.0)) * 0.75;
    qCDebug(lcXlsxImport) << "realSize:" << realSize;
    const double averageDigitWidth = realSize * 2.0 / 3.0;
    qCDebug(lcXlsxImport) << "averageDigitWidth:" << averageDigitWidth;

    QString result;
    if (averageDigitWidth * widthNumber == 0) {
        result = QLatin1String("0cm");
    }
    else
    {
        result = printCm(PX_TO_CM(averageDigitWidth * widthNumber));
    }

    return result;
}

void XlsxXmlWorksheetReader::showWarningAboutWorksheetSize()
{
    if (d->warningAboutWorksheetSizeDisplayed)
        return;
    d->warningAboutWorksheetSizeDisplayed = true;
    qCWarning(lcXlsxImport) << i18n("The data could not be loaded completely because the maximum size of "
        "sheet was exceeded.");
}

inline static QString encodeLabelText(int col, int row)
{
    return Calligra::Sheets::Util::encodeColumnLabelText(col) + QString::number(row);
}

void XlsxXmlWorksheetReader::saveAnnotation(int col, int row)
{
    QString ref(encodeLabelText(col + 1, row + 1));
    qCDebug(lcXlsxImport) << ref;
    XlsxComment *comment = m_context->comments->value(ref);
    if (!comment)
        return;
    //qCDebug(lcXlsxImport) << "Saving annotation for cell" << ref;
    body->startElement("office:annotation");
    body->startElement("dc:creator");
    body->addTextNode(comment->author(m_context->comments));
    body->endElement(); // dc:creator
    //! @todo support dc:date
    body->startElement("text:p");
    body->addCompleteElement(comment->texts.toUtf8());
    body->endElement(); // text:p
    body->endElement(); // office:annotation
}

#undef CURRENT_EL
#define CURRENT_EL chartsheet
KoFilter::ConversionStatus XlsxXmlWorksheetReader::read_chartsheet()
{
    READ_PROLOGUE

    return read_sheetHelper("chartsheet");

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL dialogsheet
KoFilter::ConversionStatus XlsxXmlWorksheetReader::read_dialogsheet()
{
    READ_PROLOGUE

    return read_sheetHelper("dialogsheet");

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL worksheet
//! worksheet handler (Worksheet)
/*! ECMA-376, 18.3.1.99, p. 1894.
 Root element of Worksheet parts within a SpreadsheetML document.
 Child elements:
 - [done] autoFilter (AutoFilter Settings) §18.3.1.2
 - cellWatches (Cell Watch Items) §18.3.1.9
 - colBreaks (Vertical Page Breaks) §18.3.1.14
 - [done] cols (Column Information) §18.3.1.17
 - [done] conditionalFormatting (Conditional Formatting) §18.3.1.18
 - [done] controls (Embedded Controls) §18.3.1.21
 - customProperties (Custom Properties) §18.3.1.23
 - customSheetViews (Custom Sheet Views) §18.3.1.27
 - dataConsolidate (Data Consolidate) §18.3.1.29
 - dataValidations (Data Validations) §18.3.1.33
 - dimension (Worksheet Dimensions) §18.3.1.35
 - [done] drawing (Drawing) §18.3.1.36
 - drawingHF (Drawing Reference in Header Footer) §18.3.1.37
 - extLst (Future Feature Data Storage Area) §18.2.10
 - headerFooter (Header Footer Settings) §18.3.1.46
 - [done] hyperlinks (Hyperlinks) §18.3.1.48
 - ignoredErrors (Ignored Errors) §18.3.1.51
 - [done] mergeCells (Merge Cells) §18.3.1.55
 - [done] oleObjects (Embedded Objects) §18.3.1.60
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
 - [done] tableParts (Table Parts) §18.3.1.95
 - webPublishItems (Web Publishing Items) §18.3.1.98

 @todo support all child elements
*/
KoFilter::ConversionStatus XlsxXmlWorksheetReader::read_worksheet()
{
    READ_PROLOGUE

    return read_sheetHelper("worksheet");

    READ_EPILOGUE
}

KoFilter::ConversionStatus XlsxXmlWorksheetReader::read_sheetHelper(const QString& type)
{
    // In the first round we do not wish to output anything
    QBuffer fakeBuffer;
    KoXmlWriter fakeBody(&fakeBuffer);
    KoXmlWriter *oldBody = body;
    if (m_context->firstRoundOfReading) {
        body = &fakeBody;
    }

    body->startElement("table:table");

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
    QBuffer bodyBuffer;
    bodyBuffer.open(QIODevice::ReadWrite);
    body = new KoXmlWriter(&bodyBuffer);

    QBuffer drawingBuffer;
    drawingBuffer.open(QIODevice::ReadWrite);

    while (!atEnd()) {
        readNext();
        qCDebug(lcXlsxImport) << *this;
        if (isEndElement() && name() == type) {
            break;
        }
        if (isStartElement() && !m_context->firstRoundOfReading) {
            TRY_READ_IF(sheetFormatPr)
            ELSE_TRY_READ_IF(cols)
            ELSE_TRY_READ_IF(sheetData) // does fill the m_context->sheet
            ELSE_TRY_READ_IF(mergeCells)
            else if (name() == "drawing") {
                KoXmlWriter *tempBodyHolder = body;
                body = new KoXmlWriter(&drawingBuffer);
                TRY_READ(drawing)
                delete body;
                body = tempBodyHolder;
            }
            ELSE_TRY_READ_IF(legacyDrawing)
            ELSE_TRY_READ_IF(hyperlinks)
            ELSE_TRY_READ_IF(picture)
            ELSE_TRY_READ_IF(oleObjects)
            else if (name() == "controls") {
                KoXmlWriter *tempBodyHolder = body;
                body = new KoXmlWriter(&drawingBuffer);
                TRY_READ(controls)
                delete body;
                body = tempBodyHolder;
            }
            ELSE_TRY_READ_IF(autoFilter)
            SKIP_UNKNOWN
        }
        else if (isStartElement() && m_context->firstRoundOfReading) {
            TRY_READ_IF(conditionalFormatting)
            ELSE_TRY_READ_IF(tableParts)
            SKIP_UNKNOWN
        }
    }

    if (m_context->firstRoundOfReading) {
        // Sorting conditional styles according to the priority

        typedef QPair<int, QMap<QString, QString> > Condition;

        // Transforming to a list for easier handling
        QList<QPair<QPair<QString, QMap<QString, QString> >, int> > diffFormulasList;
        QMapIterator<QString, QList<Condition> > i(m_conditionalStyles);
        while (i.hasNext()) {
            i.next();
            int index = 0;
            QList<Condition> conditions = i.value();
            QPair<QString, QMap<QString, QString> > innerPair;
            QPair<QPair<QString, QMap<QString, QString> >, int> outerPair;

            while (index < conditions.size()) {
                innerPair.first = i.key();
                innerPair.second = conditions.at(index).second;
                outerPair.first = innerPair;
                outerPair.second = conditions.at(index).first;
                diffFormulasList.push_back(outerPair);
                ++index;
            }
        }
        QList<QPair<int, int> > priorityActualIndex;
        int index = 0;
        while (index < diffFormulasList.size()) {
            priorityActualIndex.push_back(QPair<int, int>(diffFormulasList.at(index).second, index));
            ++index;
        }
        std::sort(priorityActualIndex.begin(), priorityActualIndex.end());

        // Finally we have the list sorted and we can store the conditions in right priority order
        index = 0;
        while (index < priorityActualIndex.size()) {
            QPair<QString, QMap<QString, QString> > odfValue;
            odfValue.first = diffFormulasList.at(priorityActualIndex.at(index).second).first.first;
            odfValue.second = diffFormulasList.at(priorityActualIndex.at(index).second).first.second;
            m_context->conditionalStyles.push_back(odfValue);
            ++index;
        }
    }

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
    heldBody->addCompleteElement(&bodyBuffer);
    delete body;
    body = heldBody;

    // Adding drawings, if there are any
    if (drawingBuffer.size() > 0) {
        body->startElement("table:shapes");
        body->addCompleteElement(&drawingBuffer);
        body->endElement(); // table:shapes
    }

    // now we have everything to start writing the actual cells
    int c = 0;
    while (c <= m_context->sheet->maxColumn()) {
        body->startElement("table:table-column");
        int repeatedColumns = 1;
        bool currentColumnHidden = false;
        Column* column = m_context->sheet->column(c, false);
        if (column) {
            if (!column->styleName.isEmpty()) {
                body->addAttribute("table:style-name", column->styleName);
            } else {
                if (m_context->sheet->m_defaultColWidth != -1.0) {
                    saveColumnStyle( computeColumnWidth( m_context->sheet->m_defaultColWidth ) );
                }
            }
        }

        if (column && column->hidden) {
            body->addAttribute("table:visibility", "collapse");
            currentColumnHidden = true;
        }
        ++c;
        while (c <= m_context->sheet->maxColumn()) {
            column = m_context->sheet->column(c, false);
            if (column && column->hidden ) {
                if (currentColumnHidden) {
                    ++repeatedColumns;
                }
                else {
                    break;
                }
            }
            else {
                if (!currentColumnHidden) {
                    ++repeatedColumns;
                }
                else {
                    break;
                }
            }
            ++c;
        }
        if (repeatedColumns > 1) {
           body->addAttribute("table:number-columns-repeated", repeatedColumns);
        }
        body->endElement();  // table:table-column
    }

    const int rowCount = m_context->sheet->maxRow();
    for(int r = 0; r <= rowCount; ++r) {
        const int columnCount = m_context->sheet->maxCellsInRow(r);
        Row* row = m_context->sheet->row(r, false);
        body->startElement("table:table-row");
        if (row) {
            if (!row->styleName.isEmpty()) {
                body->addAttribute("table:style-name", row->styleName);
            } else if (m_context->sheet->m_defaultRowHeight != -1.0) {
                QString styleName = processRowStyle(m_context->sheet->m_defaultRowHeight); // in pt
                body->addAttribute("table:style-name", styleName);
            }

            if (row->hidden) {
                body->addAttribute("table:visibility", "collapse");
            }
            //body->addAttribute("table:number-rows-repeated", QByteArray::number(row->repeated));

            for(int c = 0; c <= columnCount; ++c) {
                body->startElement("table:table-cell");
                if (Cell* cell = m_context->sheet->cell(c, r, false)) {
                    const bool hasHyperlink = ! cell->hyperlink().isEmpty();

                    if (!cell->styleName.isEmpty()) {
                        body->addAttribute("table:style-name", cell->styleName);
                    }
                    //body->addAttribute("table:number-columns-repeated", QByteArray::number(cell->repeated));
                    if (!hasHyperlink) {
                        switch(cell->valueType) {
                            case Cell::ConstNone:
                                break;
                            case Cell::ConstString:
                                body->addAttribute("office:value-type", MsooXmlReader::constString);
                                break;
                            case Cell::ConstBoolean:
                                body->addAttribute("office:value-type", MsooXmlReader::constBoolean);
                                break;
                            case Cell::ConstDate:
                                body->addAttribute("office:value-type", MsooXmlReader::constDate);
                                break;
                            case Cell::ConstFloat:
                                body->addAttribute("office:value-type", MsooXmlReader::constFloat);
                                break;
                        }
                    }

                    if (cell->valueAttrValue) {
                        switch(cell->valueAttr) {
                            case Cell::OfficeNone:
                                break;
                            case Cell::OfficeValue:
                                body->addAttribute(XlsxXmlWorksheetReader::officeValue, *cell->valueAttrValue);
                                break;
                            case Cell::OfficeStringValue:
                                body->addAttribute(XlsxXmlWorksheetReader::officeStringValue, *cell->valueAttrValue);
                                break;
                            case Cell::OfficeBooleanValue:
                                // Treat boolean values specially (ODF1.1 chapter 6.7.1)
                                //! @todo This breaks down if the value is a formula and not constant.
                                body->addAttribute(XlsxXmlWorksheetReader::officeBooleanValue,
                                                *cell->valueAttrValue == "0" ? "false" : "true");
                                break;
                            case Cell::OfficeDateValue:
                                body->addAttribute(XlsxXmlWorksheetReader::officeDateValue, *cell->valueAttrValue);
                                break;
                        }
                    }

                    if (cell->formula) {
                        QString formula;
                        if (cell->formula->isShared()) {
                            Cell *referencedCell = static_cast<SharedFormula*>(cell->formula)->m_referencedCell;
                            Q_ASSERT(referencedCell);
                            formula = MSOOXML::convertFormulaReference(referencedCell, cell);
                        } else  {
                            formula = static_cast<FormulaImpl*>(cell->formula)->m_formula;
                        }
                        if (!formula.isEmpty()) {
                            body->addAttribute("table:formula", formula);
                        }
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
                            body->addAttribute("xlink:href", cell->hyperlink());
                            body->addAttribute("xlink:type", "simple");
                            //body->addAttribute("office:target-frame-name", targetFrameName);
                            if(cell->text.isEmpty()) {
                                body->addTextNode(cell->hyperlink());
                            }
                            else {
                                body->addCompleteElement(cell->text.toUtf8());
                            }
                            body->endElement(); // text:a
                        } else if (!cell->text.isEmpty()) {
                            body->addCompleteElement(cell->text.toUtf8());
                        }
                        if (!cell->charStyleName.isEmpty()) {
                            body->endElement(); // text:span
                        }
                        body->endElement(); // text:p
                    }

                    // handle drawing objects like e.g. charts, diagrams and pictures
                    if ( cell->embedded ) {
                        foreach(XlsxDrawingObject* drawing, cell->embedded->drawings) {
                            drawing->save(body);
                        }

                        typedef QPair<QString,QString> OleObject;
                        int listIndex = 0;
                        foreach( const OleObject& oleObject, cell->embedded->oleObjects ) {
                            const QString olePath = oleObject.first;
                            const QString previewPath = oleObject.second;
                            body->addCompleteElement(cell->embedded->oleFrameBegins.at(listIndex).toUtf8());
                            ++listIndex;

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

                            body->addCompleteElement("</draw:frame>");
                        }
                    }
                }
                body->endElement(); // table:table-cell
            }
        }

        if (!row || columnCount <= 0) {
            // element table:table-row may not be empty
            body->startElement("table:table-cell");
            body->endElement(); // table:table-cell
        }
        body->endElement(); // table:table-row
    }

    body->endElement(); // table:table

    if (m_context->firstRoundOfReading) {
        body = oldBody;
    }



    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL conditionalFormatting
/*
 Parent elements:
 - [done] worksheet (§18.3.1.99)

 Child elements:
 - [done] cfRule (Conditional Formatting Rule) §18.3.1.10
 - extLst (Future Feature Data Storage Area) §18.2.10

*/
KoFilter::ConversionStatus XlsxXmlWorksheetReader::read_conditionalFormatting()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(sqref)

    // Getting rid of previously handled conditions
    m_conditionalIndices.clear();

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(cfRule)
            SKIP_UNKNOWN
        }
    }

    QList<QString> areas;
    while (sqref.indexOf(' ') > 0) {
        QString conditionArea = sqref.left(sqref.indexOf(' '));
        sqref.remove(0, conditionArea.length() + 1);
        areas.push_back(conditionArea);
    }
    areas.push_back(sqref);

    typedef QPair<int, QMap<QString, QString> > Condition;

    // Adding conditions to list of conditions and making sure that only the one with highest priority
    // remains if there are multiple conditions with same area & condition
    // This is done because some ooxml files have same condition for some area listed multiple times but
    // with different priorities
    int index = 0;
    while (index < m_conditionalIndices.size()) {
        QString conditionalArea;
        Condition examinedCondition = m_conditionalIndices.at(index);
        QString sqrefOriginal = sqref;
        int areaIndex = 0;
        Condition previousCond;

        while (areaIndex < areas.size()) {
            conditionalArea = areas.at(areaIndex);
            QList<Condition> previousConditions = m_conditionalStyles.value(conditionalArea);
            if (previousConditions.isEmpty()) {
                previousConditions.push_back(examinedCondition);
                m_conditionalStyles[conditionalArea] = previousConditions;
            }
            else {
                int conditionIndex = 0;
                bool hasTheSameCondition = false;
                while (conditionIndex < previousConditions.size()) {
                    // When comparing we only care about the condition, not the style
                    if (previousConditions.at(conditionIndex).second.value("style:condition") ==
                        examinedCondition.second.value("style:condition")) {
                        hasTheSameCondition = true;
                        previousCond = previousConditions.at(conditionIndex);
                        if (previousCond.first > examinedCondition.first) {
                            previousConditions.replace(conditionIndex, examinedCondition);
                            m_conditionalStyles[conditionalArea] = previousConditions;
                        }
                        break;
                    }
                    ++conditionIndex;
                }

                if (!hasTheSameCondition) {
                    previousConditions.push_back(examinedCondition);
                    m_conditionalStyles[conditionalArea] = previousConditions;
                }
            }
            ++areaIndex;
        }
        ++index;
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL cfRule
/*
 Parent elements:
 - [done] conditionalFormatting (§18.3.1.18)

 Child elements:
 - colorScale (Color Scale) §18.3.1.16
 - dataBar (Data Bar) §18.3.1.28
 - extLst (Future Feature Data Storage Area) §18.2.10
 - [done] formula (Formula) §18.3.1.43
 - iconSet (Icon Set) §18.3.1.49

*/
KoFilter::ConversionStatus XlsxXmlWorksheetReader::read_cfRule()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(type)
    TRY_READ_ATTR_WITHOUT_NS(dxfId)
    TRY_READ_ATTR_WITHOUT_NS(priority)
    QString op = attrs.value("operator").toString();

    QList<QString> formulas;

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            if (name() == "formula") {
                TRY_READ(formula)
                formulas.push_back(m_formula);
            }
            SKIP_UNKNOWN
        }
    }

    QMap<QString, QString> odf;
    // TODO, use attributes to really interpret this
    // The default one here is valid for type="cellIs" operator="equal"
    if (op == "equal") {
        odf["style:condition"] = QString("cell-content()=%1").arg(m_formula);
    }
    else if (op == "lessThan") {
        odf["style:condition"] = QString("cell-content()<%1").arg(m_formula);
    }
    else if (op == "greaterThan") {
        odf["style:condition"] = QString("cell-content()>%1").arg(m_formula);
    }
    else if (op == "between") {
        odf["style:condition"] = QString("cell-content-is-between(%1, %2)").arg(formulas.at(0)).arg(formulas.at(1));
    }
    odf["style:apply-style-name"] = m_context->styles->conditionalStyle(dxfId.toInt() + 1);

    m_conditionalIndices.push_back(QPair<int, QMap<QString, QString> >(priority.toInt(), odf));

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL formula
/*
 Parent elements:
 - [done] cfRule (§18.3.1.10)
 - rdn (§18.11.1.13)

 Child elements:
 - none

*/
KoFilter::ConversionStatus XlsxXmlWorksheetReader::read_formula()
{
    READ_PROLOGUE

    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        if (isCharacters()) {
            m_formula = text().toString();
        }
        BREAK_IF_END_OF(CURRENT_EL)
    }
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
    if (ok) {
        m_context->sheet->m_defaultRowHeight = drh;
    }

    const double dcw = defaultColWidth.toDouble(&ok);
    if (ok) {
        m_context->sheet->m_defaultColWidth = dcw;
    }

    const double bcw = baseColWidth.toDouble(&ok);
    if (ok) {
        m_context->sheet->m_baseColWidth = bcw;
    }

    readNext();
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
        qCDebug(lcXlsxImport) << *this;
        BREAK_IF_END_OF(CURRENT_EL)
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
    if ( !d->savedStyles.contains( widthString ) )
    {
        KoGenStyle tableColumnStyle(KoGenStyle::TableColumnAutoStyle, "table-column");
        tableColumnStyle.addProperty("style:column-width", widthString);
        tableColumnStyle.addProperty("fo:break-before", "auto");

        const QString currentTableColumnStyleName(mainStyles->insert(tableColumnStyle, "co"));
        body->addAttribute("table:style-name", currentTableColumnStyleName);
        d->savedStyles[widthString] = currentTableColumnStyleName;
    }
    else
    {
        const QString currentTableColumnStyleName(d->savedStyles[widthString]);
        body->addAttribute("table:style-name", currentTableColumnStyleName);
    }
}

void XlsxXmlWorksheetReader::appendTableColumns(int columns, const QString& width)
{
    qCDebug(lcXlsxImport) << "columns:" << columns;
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

        realWidthString = computeColumnWidth(widthNumber);
        qCDebug(lcXlsxImport) << "realWidthString:" << realWidthString;
//moved        saveColumnStyle(realWidthString);
//! @todo hardcoded table:default-cell-style-name
//moved        body->addAttribute("table:default-cell-style-name", "Excel_20_Built-in_20_Normal");
    }
    // we apparently don't need "customWidth" attr

    TRY_READ_ATTR_WITHOUT_NS(hidden)
    if (!hidden.isEmpty()) {
        column->hidden = hidden.toInt() > 0;
    }

//moved    body->endElement(); // table:table-column
    appendTableColumns(maxCol - minCol + 1, realWidthString);
    if (d->savedStyles.contains(realWidthString)) {
        column->styleName = d->savedStyles.value(realWidthString);
    }

    m_columnCount += (maxCol - minCol);

    if (m_columnCount > (int)MSOOXML::maximumSpreadsheetColumns()) {
        showWarningAboutWorksheetSize();
    }

    readNext();
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
        qCDebug(lcXlsxImport) << *this;
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(row)
            ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

QString XlsxXmlWorksheetReader::processRowStyle(qreal height)
{
    if (height == -1.0) {
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
    //TRY_READ_ATTR_WITHOUT_NS(customHeight) not used atm
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
    if (!ht.isEmpty()) {
        bool ok;
        qreal height = ht.toDouble(&ok);
        if (ok) {
            row->styleName = processRowStyle(height);
        }
    }

    if (!hidden.isEmpty()) {
        row->hidden = hidden.toInt() > 0;
    }

    qreal range = (55.0/m_context->numberOfWorkSheets);
    int counter = 0;
    while (!atEnd()) {
        readNext();
        qCDebug(lcXlsxImport) << *this;
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            if (counter == 40) {
                // set the progress by the position of what was read
                qreal progress = 45 + range * (m_context->worksheetNumber - 1)
                               + range * device()->pos() / device()->size();
                m_context->import->reportProgress(progress);
                counter = 0;
            }
            ++counter;
            TRY_READ_IF(c) // modifies m_currentColumn
            SKIP_UNKNOWN
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
    Q_UNUSED(row);

    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(r)
    if (!r.isEmpty()) {
        m_currentColumn = Calligra::Sheets::Util::decodeColumnLabelText(r) - 1;
        if (m_currentColumn < 0)
            return KoFilter::WrongFormat;
    }

    TRY_READ_ATTR_WITHOUT_NS(s)
    TRY_READ_ATTR_WITHOUT_NS(t)

    m_value.clear();

    Cell* cell = m_context->sheet->cell(m_currentColumn, m_currentRow, true);

    while (!atEnd()) {
        readNext();
        qCDebug(lcXlsxImport) << *this;
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(f)
            ELSE_TRY_READ_IF(v)
            SKIP_UNKNOWN
        }
    }

    bool ok;
    uint styleId = s.toUInt(&ok);
    const XlsxCellFormat* cellFormat = m_context->styles->cellFormat(styleId);

    QString formattedStyle;
    if (cellFormat->applyNumberFormat)
        formattedStyle = m_context->styles->numberFormatStyleName( cellFormat->numFmtId );

    //qCDebug(lcXlsxImport) << "type=" << t << "styleId=" << styleId << "applyNumberFormat=" << cellFormat->applyNumberFormat << "numberFormat=" << numberFormat << "value=" << m_value;

    QString charStyleName;

//    const bool addTextPElement = true;//m_value.isEmpty() || t != QLatin1String("s");

    if (!m_value.isEmpty()) {
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
            QString sharedstring = m_context->sharedStrings->at(stringIndex);
            cell->text = sharedstring;
            cell->valueType = Cell::ConstString;
            m_value = sharedstring;
            // no valueAttr
        } else if ((t.isEmpty() && !valueIsNumeric(m_value)) || t == QLatin1String("inlineStr")) {
//! @todo handle value properly
            cell->text = m_value;
            cell->valueType = Cell::ConstString;
            // no valueAttr
        } else if (t == QLatin1String("b")) {
            cell->text = m_value;
            cell->valueType = Cell::ConstBoolean;
            cell->valueAttr = Cell::OfficeBooleanValue;
        } else if (t == QLatin1String("d")) {
//! @todo handle value properly
            cell->text = m_value;
            cell->valueType = Cell::ConstDate;
            cell->valueAttr = Cell::OfficeDateValue;
        } else if (t == QLatin1String("str")) {
//! @todo handle value properly
            cell->text = m_value;
            cell->valueType = Cell::ConstString;
            // no valueAttr
        } else if (t == QLatin1String("n") || t.isEmpty() /* already checked if numeric */) {
            if (!t.isEmpty()) { // sanity check
                if (!valueIsNumeric(m_value)) {
                    raiseError(i18n("Expected integer or floating point number"));
                    return KoFilter::WrongFormat;
                }
            }
            const KoGenStyle* const style = mainStyles->style( formattedStyle, "" );
            if( style == 0 || valueIsNumeric(m_value) ) {
//            body->addTextSpan(m_value);
                cell->valueType = Cell::ConstFloat;
                cell->valueAttr = Cell::OfficeValue;
            } else {
                // Tests showed that this code is never executed even when a style was set.
                switch( style->type() ) {
                case KoGenStyle::NumericDateStyle:
                    cell->valueType = Cell::ConstDate;
                    cell->valueAttr = Cell::OfficeDateValue;
                    m_value = QDate( 1899, 12, 30 ).addDays( m_value.toInt() ).toString( Qt::ISODate );
                    break;
                case KoGenStyle::NumericTextStyle:
                    cell->valueType = Cell::ConstString;
                    cell->valueAttr = Cell::OfficeStringValue;
                    break;
                default:
                    cell->valueType = Cell::ConstFloat;
                    cell->valueAttr = Cell::OfficeValue;
                    break;
                }
            }
        } else if (t == QLatin1String("e")) {
            if (m_value == QLatin1String("#REF!"))
                cell->text = "#NAME?";
            else
                cell->text = m_value;
//! @todo full parsing needed to retrieve the type
            cell->valueType = Cell::ConstFloat;
            cell->valueAttr = Cell::OfficeValue;
            m_value = QLatin1String("0");
        } else {
            raiseUnexpectedAttributeValueError(t, "c@t");
            return KoFilter::WrongFormat;
        }
    }

    // cell style
    if (!s.isEmpty()) {
        if (!ok || !cellFormat) {
            raiseUnexpectedAttributeValueError(s, "c@s");
            return KoFilter::WrongFormat;
        }
        KoGenStyle cellStyle(KoGenStyle::TableCellAutoStyle, "table-cell");

        if (charStyleName.isEmpty()) {
            KoGenStyle* fontStyle = m_context->styles->fontStyle(cellFormat->fontId);
            if (!fontStyle) {
                qCWarning(lcXlsxImport) << "No font with ID:" << cellFormat->fontId;
            } else {
                KoGenStyle::copyPropertiesFromStyle(*fontStyle, cellStyle, KoGenStyle::TextType);
            }
        }
        if (!cellFormat->setupCellStyle(m_context->styles, &cellStyle)) {
            return KoFilter::WrongFormat;
        }

        if (!formattedStyle.isEmpty()) {
            cellStyle.addAttribute( "style:data-style-name", formattedStyle );
        }

        if (!m_context->conditionalStyles.isEmpty()) {
            QString positionLetter;
            int positionNumber;
            splitToRowAndColumn(r.toLatin1().constData(), 0, r.size(), positionLetter, positionNumber);
            QList<QMap<QString, QString> > maps = m_context->conditionalStyleForPosition(positionLetter, positionNumber);
            int index = maps.size();
            // Adding the lists in reversed priority order, as KoGenStyle when creating the style
            // adds last added first
            while (index > 0) {
                cellStyle.addStyleMap(maps.at(index - 1));
                --index;
            }
        }

        const QString cellStyleName = mainStyles->insert( cellStyle, "ce" );
        cell->styleName = cellStyleName;
    }

    delete cell->valueAttrValue;
    if (m_value.isEmpty()) {
        cell->valueAttrValue = 0;
    } else {
        cell->valueAttrValue = new QString(m_value);
    }

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
        BREAK_IF_END_OF(CURRENT_EL)
        if (isCharacters()) {
            delete cell->formula;
            cell->formula = new FormulaImpl(Calligra::Sheets::MSOOXML::convertFormula(text().toString()));
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
                    if (!cell->formula /* || cell->formula->isEmpty() */) { // don't do anything if the cell already defines a formula
                        QHash<int, Cell*>::iterator it = d->sharedFormulas.find(sharedGroupIndex);
                        if (it != d->sharedFormulas.end()) {
                            delete cell->formula;
                            cell->formula = new SharedFormula(it.value());
                        }
                    }
                } else if (cell->formula /* && !cell->formula->isEmpty()*/) { // is this cell the master cell?
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
            const int c1 = Calligra::Sheets::Util::decodeColumnLabelText(fromCell) - 1;
            const int r1 = Calligra::Sheets::Util::decodeRowLabelText(fromCell) - 1;
            const int c2 = Calligra::Sheets::Util::decodeColumnLabelText(toCell) - 1;
            const int r2 = Calligra::Sheets::Util::decodeRowLabelText(toCell) - 1;
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

    // It is possible to have empty <v/> element
    if (name() == "v" && isEndElement()) {
        READ_EPILOGUE
    }

    m_value = text().toString();
    m_value.replace('&', "&amp;");
    m_value.replace('<', "&lt;");
    m_value.replace('>', "&gt;");
    m_value.replace('\\', "&apos;");
    m_value.replace('"', "&quot;");

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL mergeCell
/*
 Parent elements:
 - [done] mergeCells (§18.3.1.55)

 Child elements:
 - none
*/
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
            const int fromCol = Calligra::Sheets::Util::decodeColumnLabelText(fromCell) - 1;
            if(rx.exactMatch(toCell)) {
                Cell* cell = m_context->sheet->cell(fromCol, fromRow, true);
                cell->rowsMerged = rx.cap(2).toInt() - fromRow;
                cell->columnsMerged = Calligra::Sheets::Util::decodeColumnLabelText(toCell) - fromCol;

                // correctly take right/bottom borders from the cells that are merged into this one
                const KoGenStyle* origCellStyle = mainStyles->style(cell->styleName, "table-cell");
                KoGenStyle cellStyle;
                if (origCellStyle) {
                    cellStyle = *origCellStyle;
                }
                qCDebug(lcXlsxImport) << cell->rowsMerged << cell->columnsMerged << cell->styleName;
                if (cell->rowsMerged > 1) {
                    Cell* lastCell = m_context->sheet->cell(fromCol, fromRow + cell->rowsMerged - 1, false);
                    qCDebug(lcXlsxImport) << lastCell;
                    if (lastCell) {
                        const KoGenStyle* style = mainStyles->style(lastCell->styleName, "table-cell");
                        qCDebug(lcXlsxImport) << lastCell->styleName;
                        if (style) {
                            QString val = style->property("fo:border-bottom");
                            qCDebug(lcXlsxImport) << val;
                            if (!val.isEmpty()) cellStyle.addProperty("fo:border-bottom", val);
                            val = style->property("fo:border-line-width-bottom");
                            if (!val.isEmpty()) cellStyle.addProperty("fo:border-line-width-bottom", val);
                        }
                    }
                }
                if (cell->columnsMerged > 1) {
                    Cell* lastCell = m_context->sheet->cell(fromCol + cell->columnsMerged - 1, fromRow, false);
                    if (lastCell) {
                        const KoGenStyle* style = mainStyles->style(lastCell->styleName, "table-cell");
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

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL mergeCells
/*
 Parent elements:
 - [done] worksheet (§18.3.1.99)

 Child elements:
 - mergeCell (Merged Cell) §18.3.1.54
*/
KoFilter::ConversionStatus XlsxXmlWorksheetReader::read_mergeCells()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
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
        QString drawingPathAndFile = m_context->relationships->target(m_context->path, m_context->file, r_id);
        QString drawingPath, drawingFile;
        MSOOXML::Utils::splitPathAndFile(drawingPathAndFile, &drawingPath, &drawingFile);

        XlsxXmlDrawingReaderContext context(m_context, m_context->sheet, drawingPath, drawingFile);
        XlsxXmlDrawingReader reader(this);
        const KoFilter::ConversionStatus result = m_context->import->loadAndParseDocument(&reader, drawingPathAndFile, &context);
        if (result != KoFilter::OK) {
            raiseError(reader.errorString());
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
        BREAK_IF_END_OF(CURRENT_EL)
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL hyperlink
/*
 Parent elements:
 - [done] hyperlinks (§18.3.1.48)

 Child elements:
 - none
*/
KoFilter::ConversionStatus XlsxXmlWorksheetReader::read_hyperlink()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(ref)
    TRY_READ_ATTR_WITHOUT_NS(location)
    TRY_READ_ATTR_WITH_NS(r, id)
    if (!ref.isEmpty() && (!r_id.isEmpty() || !location.isEmpty())) {
        const int col = Calligra::Sheets::Util::decodeColumnLabelText(ref) - 1;
        const int row = Calligra::Sheets::Util::decodeRowLabelText(ref) - 1;
        if(col >= 0 && row >= 0) {
            QString link = m_context->relationships->target(m_context->path, m_context->file, r_id);
            // it follows a hack to get right of the prepended m_context->path...
            if (link.startsWith(m_context->path))
                link.remove(0, m_context->path.length()+1);

            // append location
            if (!location.isEmpty()) link += '#' + location;

            Cell* cell = m_context->sheet->cell(col, row, true);
            cell->setHyperLink( link );
        }
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL hyperlinks
/*
 Parent elements:
 - [done] worksheet (§18.3.1.99)

 Child elements:
 - [done] hyperlink (Hyperlink) §18.3.1.47
*/
KoFilter::ConversionStatus XlsxXmlWorksheetReader::read_hyperlinks()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(hyperlink)
            ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL customFilters
/*
 Parent elements:
 - [done] filterColumn (§18.3.2.7)

 Child elements:
 - [done] customFilter (Custom Filter Criteria) §18.3.2.2
*/
KoFilter::ConversionStatus XlsxXmlWorksheetReader::read_customFilters()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    QString andValue = attrs.value("and").toString();

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(customFilter)
            ELSE_WRONG_FORMAT
        }
    }

    if (!m_context->autoFilters.isEmpty()) {
        if (andValue == "1") {
            m_context->autoFilters.last().type = "and";
        } else {
            m_context->autoFilters.last().type = "or";
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL filters
/*
 Parent elements:
 - [done] filterColumn (§18.3.2.7)

 Child elements:
 - dateGroupItem (Date Grouping) §18.3.2.4
 - [done] filter (Filter) §18.3.2.6
*/
KoFilter::ConversionStatus XlsxXmlWorksheetReader::read_filters()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(blank)

    m_context->currentFilterCondition.value = "^(";

    bool hasValueAlready = false;

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            if (name() == "filter") {
                if (hasValueAlready) {
                    m_context->currentFilterCondition.value += "|";
                }
                hasValueAlready = true;
                TRY_READ(filter)
            }
            SKIP_UNKNOWN
        }
    }

    m_context->currentFilterCondition.value += ")$";
    m_context->currentFilterCondition.opField = "match";

    if (blank == "1") {
        m_context->currentFilterCondition.value = "0";
        m_context->currentFilterCondition.opField = "empty";
    }

    if (!m_context->autoFilters.isEmpty()) {
        m_context->autoFilters.last().filterConditions.push_back(m_context->currentFilterCondition);
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL customFilter
/*
 Parent elements:
 - [done] customFilters (§18.3.2.2)

 Child elements:
 - none
*/
KoFilter::ConversionStatus XlsxXmlWorksheetReader::read_customFilter()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    QString opValue = attrs.value("operator").toString();

    TRY_READ_ATTR_WITHOUT_NS(val)
    m_context->currentFilterCondition.value = val;

    if (opValue == "notEqual") {
        m_context->currentFilterCondition.opField = "!=";
    }
    else {
        m_context->currentFilterCondition.opField = "=";
    }

    if (!m_context->autoFilters.isEmpty()) {
        m_context->autoFilters.last().filterConditions.push_back(m_context->currentFilterCondition);
    }

    readNext();

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL filter
/*
 Parent elements:
 - [done] filters (§18.3.2.8)

 Child elements:
 - none
*/
KoFilter::ConversionStatus XlsxXmlWorksheetReader::read_filter()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(val)

    m_context->currentFilterCondition.value += val;

    readNext();

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL filterColumn
/*
 Parent elements:
 - [done] autoFilter (§18.3.1.2)

 Child elements:
 - colorFilter (Color Filter Criteria) §18.3.2.1
 - [done] customFilters (Custom Filters) §18.3.2.3
 - dynamicFilter (Dynamic Filter) §18.3.2.5
 - extLst (Future Feature Data Storage Area) §18.2.10
 - [done] filters (Filter Criteria) §18.3.2.8
 - iconFilter (Icon Filter) §18.3.2.9
 - top10 (Top 10) §18.3.2.10
*/
KoFilter::ConversionStatus XlsxXmlWorksheetReader::read_filterColumn()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(colId)

    m_context->currentFilterCondition.field = colId;

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(filters)
            ELSE_TRY_READ_IF(customFilters)
            SKIP_UNKNOWN
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL autoFilter
/*
 Parent elements:
 - customSheetView (§18.3.1.25)
 - filter (§18.10.1.33)
 - table (§18.5.1.2)
 - [done] worksheet (§18.3.1.99)

 Child elements:
 - extLst (Future Feature Data Storage Area) §18.2.10
 - [done] filterColumn (AutoFilter Column) §18.3.2.7
 - sortState (Sort State) §18.3.1.92
*/
KoFilter::ConversionStatus XlsxXmlWorksheetReader::read_autoFilter()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(ref)

    // take last numbers and replace it with max row
    ref.replace(QRegExp("[0-9]+$"), QString::number(m_context->sheet->maxRow()+1));

    ref.prepend(".");
    QString sheetName = m_context->worksheetName;
    if (sheetName.contains('.') || sheetName.contains(' ') || sheetName.contains('\'')) {
        sheetName = '\'' + sheetName.replace('\'', "''") + '\'';
    }
    ref.prepend(sheetName);

    int colon = ref.indexOf(':');
    if (colon > 0) {
        ref.insert(colon + 1, '.');
        ref.insert(colon + 1, sheetName);
    }

    XlsxXmlDocumentReaderContext::AutoFilter autoFilter;
    autoFilter.area = ref;
    m_context->autoFilters.push_back(autoFilter);

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(filterColumn)
            SKIP_UNKNOWN
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL picture
/*
 Parent elements:
 - chartsheet (§18.3.1.12)
 - [done] worksheet (§18.3.1.99)

 Child elements:
 - none
*/
KoFilter::ConversionStatus XlsxXmlWorksheetReader::read_picture()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITH_NS(r, id)
    const QString link = m_context->relationships->target(m_context->path, m_context->file, r_id);
    QString destinationName = QLatin1String("Pictures/") + link.mid(link.lastIndexOf('/') + 1);
    RETURN_IF_ERROR( m_context->import->copyFile(link, destinationName, false ) )
    addManifestEntryForFile(destinationName);

    m_context->sheet->setPictureBackgroundPath(destinationName);

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL tableParts
/*
 Parent elements:
 - [done] worksheet (§18.3.1.99)

 Child elements:
 - [done] tablePart (Table Part) §18.3.1.94

*/
KoFilter::ConversionStatus XlsxXmlWorksheetReader::read_tableParts()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if( isStartElement() ) {
            TRY_READ_IF(tablePart)
            ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL tablePart
/*
 Parent elements:
 - [done] tableParts (§18.3.1.95)

 Child elements:
 - none
*/
KoFilter::ConversionStatus XlsxXmlWorksheetReader::read_tablePart()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    READ_ATTR_WITH_NS(r, id)
    QString tablePathAndFile = m_context->relationships->target(m_context->path, m_context->file, r_id);

    XlsxXmlTableReaderContext context;
    XlsxXmlTableReader reader(this);
    const KoFilter::ConversionStatus result = m_context->import->loadAndParseDocument(&reader, tablePathAndFile, &context);
    if (result != KoFilter::OK) {
        raiseError(reader.errorString());
        return result;
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL legacyDrawing
// todo
KoFilter::ConversionStatus XlsxXmlWorksheetReader::read_legacyDrawing()
{
    READ_PROLOGUE
    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL controls
/*
 Parent elements:
 - [done] worksheet (§18.3.1.99)

 Child elements:
 - [done] control (Embedded Control) §18.3.1.19
*/
KoFilter::ConversionStatus XlsxXmlWorksheetReader::read_controls()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if( isStartElement() ) {
            TRY_READ_IF(control)
            ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL oleObjects
/*
 Parent elements:
 - [done] dialogsheet (§18.3.1.34)
 - [done] worksheet (§18.3.1.99)

 Child elements:
 - [done] oleObject (Embedded Object) §18.3.1.59
*/
KoFilter::ConversionStatus XlsxXmlWorksheetReader::read_oleObjects()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if( isStartElement() ) {
            TRY_READ_IF(oleObject)
            // It seems that MSO 2010 has a concept of Alternate
            // Content, which it throws in at unexpected times.
            // This is one such time.  So let's try to find the
            // oleObject inside an mc:AlternateContent tag if possible.
            ELSE_TRY_READ_IF_NS(mc, AlternateContent)   // Should be more specialized what we are looking for
            ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL control
/*
 Parent elements:
 - [done] controls (§18.3.1.21)

 Child elements:
 - controlPr (Embedded Control Properties) §18.3.1.20

*/
KoFilter::ConversionStatus XlsxXmlWorksheetReader::read_control()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(shapeId)

    // TODO: Maybe we want to do something with the actual control element.

    // In vmldrawing, the shape identifier has also the extra chars below, therefore
    // we have to add them here for the match
    shapeId = "_x0000_s" + shapeId;

    body->addCompleteElement(m_context->oleFrameBegins.value(shapeId).toUtf8());
    body->startElement("draw:image");
    body->addAttribute("xlink:href", m_context->oleReplacements.value(shapeId));
    body->addAttribute("xlink:type", "simple");
    body->addAttribute("xlink:show", "embed");
    body->addAttribute("xlink:actuate", "onLoad");
    body->endElement(); // draw:image
    body->addCompleteElement("</draw:frame>");

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL oleObject
/*
 Parent elements:
 - [done] oleObjects (§18.3.1.60)

 Child elements:
 - objectPr (Embedded Object Properties) §18.3.1.56

*/
KoFilter::ConversionStatus XlsxXmlWorksheetReader::read_oleObject()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    READ_ATTR_WITH_NS(r, id)
    READ_ATTR_WITHOUT_NS(progId)
    TRY_READ_ATTR_WITHOUT_NS(shapeId)

    // In vmldrawing, the shape identifier has also the extra chars below, therefore
    // we have to add them here for the match
    shapeId = "_x0000_s" + shapeId;

    const QString link = m_context->relationships->target(m_context->path, m_context->file, r_id);
    QString destinationName = QLatin1String("") + link.mid(link.lastIndexOf('/') + 1);
    KoFilter::ConversionStatus status = m_context->import->copyFile(link, destinationName, false);
    if (status == KoFilter::OK) {
        addManifestEntryForFile(destinationName);
    }

    //TODO find out which cell to pick
    Cell* cell = m_context->sheet->cell(0, 0, true);
    cell->appendOleObject( qMakePair<QString,QString>(destinationName, m_context->oleReplacements.value(shapeId)), m_context->oleFrameBegins.value(shapeId));

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
    }
    READ_EPILOGUE
}
