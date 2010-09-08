/* This file is part of the KDE project
   Copyright (C) 2003-2006 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2006 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>
   Copyright (C) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
   Contact: Manikandaprasad Chandrasekar <manikandaprasad.chandrasekar@nokia.com>
   Copyright (c) 2010 Carlos Licea <carlos@kdab.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "ExcelImport.h"
#include <ExcelImport.moc>

#include <QString>
#include <QDate>
#include <QBuffer>
#include <QFontMetricsF>
#include <QPair>
#include <QTextCursor>

#include <kdebug.h>
#include <KoFilterChain.h>
#include <KoGlobal.h>
#include <KoUnit.h>
#include <kgenericfactory.h>

#include <KoXmlWriter.h>
#include <KoOdfWriteStore.h>
#include <KoGenStyles.h>
#include <KoGenStyle.h>
#include <KoOdfNumberStyles.h>
#include <KoXmlNS.h>
#include <KoShapeLoadingContext.h>
#include <KoShapeRegistry.h>
#include <KoOdfStylesReader.h>
#include <KoOdfLoadingContext.h>
#include <KoShape.h>
#include <KoDocumentInfo.h>

#include <part/Doc.h>
#include <CalculationSettings.h>
#include <CellStorage.h>
#include <Map.h>
#include <Sheet.h>
#include <Style.h>
#include <StyleStorage.h>
#include <RowColumnFormat.h>
#include <ValueConverter.h>
#include <ShapeApplicationData.h>
#include <Util.h>

#include <Charting.h>
#include <ChartExport.h>
#include <NumberFormatParser.h>

#include "swinder.h"
#include <iostream>
#include "ODrawClient.h"
#include "ImportUtils.h"

typedef KGenericFactory<ExcelImport> ExcelImportFactory;
K_EXPORT_COMPONENT_FACTORY(libexcelimporttodoc, ExcelImportFactory("kofficefilters"))

using namespace Swinder;
using namespace XlsUtils;

static qreal offset( unsigned long dimension, unsigned long offset, qreal factor ) {
    return (float)dimension * (float)offset / factor;
}

static qreal columnWidth(Sheet* sheet, unsigned long col) {
    if( sheet->column(col, false) )
        return sheet->column(col)->width();

    return sheet->defaultColWidth();
}

static qreal rowHeight(Sheet* sheet, unsigned long row) {
    if( sheet->row(row, false) )
        return sheet->row(row)->height();

    return sheet->defaultRowHeight();
}

// Returns A for 1, B for 2, C for 3, etc.
static QString columnName(uint column)
{
    QString s;
    unsigned digits = 1;
    unsigned offset = 0;
    for (unsigned limit = 26; column >= limit + offset; limit *= 26, digits++)
        offset += limit;
    for (unsigned col = column - offset; digits; --digits, col /= 26)
        s.prepend(QChar('A' + (col % 26)));
    return s;
}

static QString encodeSheetName(const QString& name)
{
    QString sheetName = name;
    if (sheetName.contains(' ') || sheetName.contains('.') || sheetName.contains('\''))
        sheetName = '\'' + sheetName.replace('\'', "''") + '\'';
    return sheetName;
}

static QString encodeAddress(const QString& sheetName, uint column, uint row)
{
    return QString("%1.%2%3").arg(encodeSheetName(sheetName)).arg(columnName(column)).arg(row+1);
}


class ExcelImport::Private
{
public:
    QString inputFile;
    KSpread::Doc* outputDoc;

    Workbook *workbook;

    // for embedded shapes
    KoStore* storeout;
    KoGenStyles *shapeStyles;
    KoGenStyles *dataStyles;
    KoXmlWriter *shapesXml;

    void processMetaData();
    void processSheet(Sheet* isheet, KSpread::Sheet* osheet);
    void processColumn(Sheet* isheet, unsigned column, KSpread::Sheet* osheet);
    void processRow(Sheet* isheet, unsigned row, KSpread::Sheet* osheet);
    void processCell(Cell* icell, KSpread::Cell ocell);
    void processCellObjects(Cell* icell, KSpread::Cell ocell);
    void processEmbeddedObjects(const KoXmlElement& rootElement, KoStore* store);
    void processNumberFormats();

    int convertStyle(const Format* format, const QString& formula = QString());
    QHash<CellFormatKey, int> styleCache;
    QList<KSpread::Style> styleList;
    QHash<QString, KSpread::Style> dataStyleCache;
    QHash<QString, KSpread::Conditions> dataStyleConditions;

    void processFontFormat(const FormatFont& font, KSpread::Style& style);
    QTextCharFormat convertFontToCharFormat(const FormatFont& font);
    QPen convertBorder(const Pen& pen);

    int rowsCountTotal, rowsCountDone;
    void addProgress(int addValue);

    QHash<int, QRegion> cellStyles;
    QHash<int, QRegion> rowStyles;
    QHash<int, QRegion> columnStyles;
    QList<QPair<QRegion, KSpread::Conditions> > cellConditions;

    QList<ChartExport*> charts;
    void processCharts(KoXmlWriter* manifestWriter);

    void addManifestEntries(KoXmlWriter* ManifestWriter);
    void insertPictureManifest(PictureObject* picture);
    QMap<QString,QString> manifestEntries;

    KoXmlWriter* beginMemoryXmlWriter(const char* docElement);
    KoXmlDocument endMemoryXmlWriter(KoXmlWriter* writer);
};

ExcelImport::ExcelImport(QObject* parent, const QStringList&)
        : KoFilter(parent)
{
    d = new Private;
}

ExcelImport::~ExcelImport()
{
    delete d;
}

KoFilter::ConversionStatus ExcelImport::convert(const QByteArray& from, const QByteArray& to)
{
    if (from != "application/vnd.ms-excel")
        return KoFilter::NotImplemented;

    if (to != "application/vnd.oasis.opendocument.spreadsheet")
        return KoFilter::NotImplemented;

    d->inputFile = m_chain->inputFile();

    KoDocument* document = m_chain->outputDocument();
    if (!document)
        return KoFilter::StupidError;

    d->outputDoc = qobject_cast<KSpread::Doc*>(document);
    if (!d->outputDoc) {
        kWarning() << "document isn't a KSpread::Doc but a " << document->metaObject()->className();
        return KoFilter::WrongFormat;
    }

    emit sigProgress(0);
    
    
    QBuffer storeBuffer; // TODO: use temporary file instead
    d->storeout = KoStore::createStore(&storeBuffer, KoStore::Write);

    // open inputFile
    d->workbook = new Swinder::Workbook(d->storeout);
    connect(d->workbook, SIGNAL(sigProgress(int)), this, SIGNAL(sigProgress(int)));
    if (!d->workbook->load(d->inputFile.toLocal8Bit())) {
        delete d->workbook;
        d->workbook = 0;
        delete d->storeout;
        d->storeout = 0;
        return KoFilter::StupidError;
    }

    if (d->workbook->isPasswordProtected()) {
        delete d->workbook;
        d->workbook = 0;
        delete d->storeout;
        d->storeout = 0;
        return KoFilter::PasswordProtected;
    }

    emit sigProgress(-1);
    emit sigProgress(0);

    // count the number of rows in total to provide a good progress value
    d->rowsCountTotal = d->rowsCountDone = 0;
    for (unsigned i = 0; i < d->workbook->sheetCount(); i++) {
        Sheet* sheet = d->workbook->sheet(i);
        d->rowsCountTotal += qMin(maximalRowCount, sheet->maxRow());
    }

    d->shapeStyles = new KoGenStyles();
    d->dataStyles = new KoGenStyles();

    // convert number formats
    d->processNumberFormats();

    d->processMetaData();

    d->shapesXml = d->beginMemoryXmlWriter("table:shapes");

    KSpread::Map* map = d->outputDoc->map();
    for (unsigned i = 0; i < d->workbook->sheetCount(); i++) {
        d->shapesXml->startElement("table:table");
        d->shapesXml->addAttribute("table:id", i);
        Sheet* sheet = d->workbook->sheet(i);
        KSpread::Sheet* ksheet = map->addNewSheet();
        d->processSheet(sheet, ksheet);
        d->shapesXml->endElement();
    }

    QBuffer manifestBuffer;
    KoXmlWriter manifestWriter(&manifestBuffer);
    manifestWriter.startDocument("manifest:manifest");
    manifestWriter.startElement("manifest:manifest");
    manifestWriter.addAttribute("xmlns:manifest", KoXmlNS::manifest);
    manifestWriter.addManifestEntry("/", "application/vnd.oasis.opendocument.spreadsheet");

    d->processCharts(&manifestWriter);
    d->addManifestEntries(&manifestWriter);

    manifestWriter.endElement();
    manifestWriter.endDocument();
    if (d->storeout->open("META-INF/manifest.xml")) {
        d->storeout->write(manifestBuffer.buffer());
        d->storeout->close();
    }

    delete d->storeout;
    storeBuffer.close();

    KoStore *store = KoStore::createStore(&storeBuffer, KoStore::Read);

    KoXmlDocument xmlDoc = d->endMemoryXmlWriter(d->shapesXml);
    d->processEmbeddedObjects(xmlDoc.documentElement(), store);

    // sheet background images
    for (unsigned i = 0; i < d->workbook->sheetCount(); i++) {
        Sheet* sheet = d->workbook->sheet(i);
        KSpread::Sheet* ksheet = map->sheet(i);
        kDebug() << i << sheet->backgroundImage();
        if (sheet->backgroundImage().isEmpty()) continue;

        QByteArray data;
        store->extractFile(sheet->backgroundImage(), data);
        QImage image = QImage::fromData(data);
        if (image.isNull()) continue;

        ksheet->setBackgroundImage(image);
        ksheet->setBackgroundImageProperties(KSpread::Sheet::BackgroundImageProperties());
    }

    delete store;

    delete d->workbook;
    delete d->shapeStyles;
    delete d->dataStyles;
    d->inputFile.clear();
    d->outputDoc = 0;
    d->shapesXml = 0;

    emit sigProgress(100);
    return KoFilter::OK;
}

void ExcelImport::Private::processMetaData()
{
    KoDocumentInfo* info = outputDoc->documentInfo();

    if (workbook->hasProperty(Workbook::PIDSI_TITLE)) {
        info->setAboutInfo("title", workbook->property(Workbook::PIDSI_TITLE).toString());
    }
    if (workbook->hasProperty(Workbook::PIDSI_SUBJECT)) {
        info->setAboutInfo("subject", workbook->property(Workbook::PIDSI_SUBJECT).toString());
    }
    if (workbook->hasProperty(Workbook::PIDSI_AUTHOR)) {
        info->setAuthorInfo("creator", workbook->property(Workbook::PIDSI_AUTHOR).toString());
    }
    if (workbook->hasProperty(Workbook::PIDSI_KEYWORDS)) {
        info->setAboutInfo("keyword", workbook->property(Workbook::PIDSI_KEYWORDS).toString());
    }
    if (workbook->hasProperty(Workbook::PIDSI_COMMENTS)) {
        info->setAboutInfo("comments", workbook->property(Workbook::PIDSI_COMMENTS).toString());
    }
    if (workbook->hasProperty(Workbook::PIDSI_REVNUMBER)) {
        info->setAboutInfo("editing-cycles", workbook->property(Workbook::PIDSI_REVNUMBER).toString());
    }
    if (workbook->hasProperty(Workbook::PIDSI_LASTPRINTED_DTM)) {
        info->setAboutInfo("print-date", workbook->property(Workbook::PIDSI_LASTPRINTED_DTM).toString());
    }
    if (workbook->hasProperty(Workbook::PIDSI_CREATE_DTM)) {
        info->setAboutInfo("creation-date", workbook->property(Workbook::PIDSI_CREATE_DTM).toString());
    }
    if (workbook->hasProperty(Workbook::PIDSI_LASTSAVED_DTM)) {
        info->setAboutInfo("date", workbook->property(Workbook::PIDSI_LASTSAVED_DTM).toString());
    }
    // template
    // lastauthor
    // edittime
}

void ExcelImport::Private::processEmbeddedObjects(const KoXmlElement& rootElement, KoStore* store)
{
    // save styles to xml
    KoXmlWriter *stylesXml = beginMemoryXmlWriter("office:styles");
    shapeStyles->saveOdfStyles(KoGenStyles::DocumentAutomaticStyles, stylesXml);

    KoXmlDocument stylesDoc = endMemoryXmlWriter(stylesXml);

    // Register additional attributes, that identify shapes anchored in cells.
    // Their dimensions need adjustment after all rows are loaded,
    // because the position of the end cell is not always known yet.
    KoShapeLoadingContext::addAdditionalAttributeData(KoShapeLoadingContext::AdditionalAttributeData(
                KoXmlNS::table, "end-cell-address",
                "table:end-cell-address"));
    KoShapeLoadingContext::addAdditionalAttributeData(KoShapeLoadingContext::AdditionalAttributeData(
                KoXmlNS::table, "end-x",
                "table:end-x"));
    KoShapeLoadingContext::addAdditionalAttributeData(KoShapeLoadingContext::AdditionalAttributeData(
                KoXmlNS::table, "end-y",
                "table:end-y"));


    KoOdfStylesReader odfStyles;
    odfStyles.createStyleMap(stylesDoc, false);
    KoOdfLoadingContext odfContext(odfStyles, store);
    KoShapeLoadingContext shapeContext(odfContext, outputDoc->resourceManager());

    KoXmlElement sheetElement;
    forEachElement(sheetElement, rootElement) {
        Q_ASSERT(sheetElement.namespaceURI() == KoXmlNS::table && sheetElement.localName() == "table");
        int sheetId = sheetElement.attributeNS(KoXmlNS::table, "id").toInt();
        KSpread::Sheet* sheet = outputDoc->map()->sheet(sheetId);

        KoXmlElement cellElement;
        forEachElement(cellElement, sheetElement) {
            Q_ASSERT(cellElement.namespaceURI() == KoXmlNS::table && cellElement.localName() == "table-cell");
            int row = cellElement.attributeNS(KoXmlNS::table, "row").toInt();
            int col = cellElement.attributeNS(KoXmlNS::table, "column").toInt();
            KSpread::Cell cell(sheet, col, row);

            KoXmlElement element;
            forEachElement(element, cellElement) {
                cell.loadOdfObject(element, shapeContext);
            }
        }
    }
}

void ExcelImport::Private::processSheet(Sheet* is, KSpread::Sheet* os)
{
    os->setSheetName(is->name());
    os->setHidden(!is->visible());
    //os->setProtected(is->protect());
    os->setAutoCalculationEnabled(is->autoCalc());

    // TODO: page layout

    if(is->password() != 0) {
        //TODO
    }

    const unsigned columnCount = qMin(maximalColumnCount, is->maxColumn());
    for (unsigned i = 0; i <= columnCount; ++i) {
        processColumn(is, i, os);
    }

    cellStyles.clear();
    rowStyles.clear();
    columnStyles.clear();
    cellConditions.clear();
    const unsigned rowCount = qMin(maximalRowCount, is->maxRow());
    for (unsigned i = 0; i <= rowCount; ++i) {
        processRow(is, i, os);
    }

    QList<QPair<QRegion, KSpread::Style> > styles;
    for (QHash<int, QRegion>::const_iterator it = columnStyles.constBegin(); it != columnStyles.constEnd(); ++it) {
        styles.append(qMakePair(it.value(), styleList[it.key()]));
    }
    for (QHash<int, QRegion>::const_iterator it = rowStyles.constBegin(); it != rowStyles.constEnd(); ++it) {
        styles.append(qMakePair(it.value(), styleList[it.key()]));
    }
    for (QHash<int, QRegion>::const_iterator it = cellStyles.constBegin(); it != cellStyles.constEnd(); ++it) {
        styles.append(qMakePair(it.value(), styleList[it.key()]));
    }
    os->cellStorage()->styleStorage()->load(styles);
    os->cellStorage()->loadConditions(cellConditions);
}

void ExcelImport::Private::processColumn(Sheet* is, unsigned columnIndex, KSpread::Sheet* os)
{
    Column* column = is->column(columnIndex, false);

    if (!column) return;

    KSpread::ColumnFormat* oc = os->nonDefaultColumnFormat(columnIndex+1);
    oc->setWidth(column->width());
    oc->setHidden(!column->visible());

    int styleId = convertStyle(&column->format());
    columnStyles[styleId] += QRect(columnIndex+1, 1, 1, KS_rowMax);
}

void ExcelImport::Private::processRow(Sheet* is, unsigned rowIndex, KSpread::Sheet* os)
{
    Row *row = is->row(rowIndex, false);

    if (!row) return;

    KSpread::RowFormat* orf = os->nonDefaultRowFormat(rowIndex+1);
    orf->setHeight(row->height());
    orf->setHidden(!row->visible());
    // TODO default cell style

    // find the column of the rightmost cell (if any)
    const int lastCol = row->sheet()->maxCellsInRow(rowIndex);
    for (int i = 0; i <= lastCol; ++i) {
        Cell* cell = is->cell(i, rowIndex, false);
        if (!cell) continue;
        processCell(cell, KSpread::Cell(os, i+1, rowIndex+1));
    }

    addProgress(1);
}

static QString cellFormulaNamespace(const QString& formula)
{
    if (!formula.isEmpty()) {
        if(formula.startsWith("ROUNDUP(") || formula.startsWith("ROUNDDOWN(") || formula.startsWith("ROUND(") || formula.startsWith("RAND(")) {
            // Special case Excel formulas that differ from OpenFormula
            return "msoxl:";
        } else if (!formula.isEmpty()) {
            return "of:";
        }
    }
    return QString();
}

static QDateTime convertDate(double timestamp)
{
    QDateTime dt(QDate(1899, 12, 30));
    dt = dt.addMSecs((qint64)(timestamp * 86400 * 1000));
    return dt;
}

static QTime convertTime(double timestamp)
{
    QTime tt;
    tt = tt.addMSecs(qRound((timestamp - (qint64)timestamp) * 86400 * 1000));
    return tt;
}

void ExcelImport::Private::processCell(Cell* ic, KSpread::Cell oc)
{
    int colSpan = ic->columnSpan();
    int rowSpan = ic->rowSpan();
    if (colSpan > 1 || rowSpan > 1) {
        oc.mergeCells(oc.column(), oc.row(), colSpan - 1, rowSpan - 1);
    }

    const QString formula = ic->formula();
    const bool isFormula = !formula.isEmpty();
    if (isFormula) {
        const QString nsPrefix = cellFormulaNamespace(formula);
        const QString decodedFormula = KSpread::Odf::decodeFormula(formula, oc.locale(), nsPrefix);
        oc.setUserInput(decodedFormula);
    }

    Value value = ic->value();
    if (value.isBoolean()) {
        oc.setValue(KSpread::Value(value.asBoolean()));
        if (!isFormula)
            oc.setUserInput(oc.sheet()->map()->converter()->asString(oc.value()).asString());
    } else if (value.isNumber()) {
        const QString valueFormat = ic->format().valueFormat();

        if (isPercentageFormat(valueFormat)) {
            KSpread::Value v(value.asFloat());
            v.setFormat(KSpread::Value::fmt_Percent);
            oc.setValue(v);
        } else if (isDateFormat(value, valueFormat)) {
            QDateTime date = convertDate(value.asFloat());
            oc.setValue(KSpread::Value(date, outputDoc->map()->calculationSettings()));
            KLocale* locale = outputDoc->map()->calculationSettings()->locale();
            if (true /* TODO somehow determine if time should be included */) {
                oc.setUserInput(locale->formatDate(date.date()));
            } else {
                oc.setUserInput(locale->formatDateTime(date));
            }
        } else if (isTimeFormat(value, valueFormat)) {
            QTime time = convertTime(value.asFloat());
            oc.setValue(KSpread::Value(time, outputDoc->map()->calculationSettings()));
            KLocale* locale = outputDoc->map()->calculationSettings()->locale();
            oc.setUserInput(locale->formatTime(time, true));
        } else /* fraction or normal */ {
            oc.setValue(KSpread::Value(value.asFloat()));
            if (!isFormula)
                oc.setUserInput(oc.sheet()->map()->converter()->asString(oc.value()).asString());
        }
    } else if (value.isText()) {
        QString txt = value.asString();

        Hyperlink link = ic->hyperlink();
        if (link.isValid) {
            if (!link.location.isEmpty()) {
                oc.setLink(link.location); // not sure if I should strip a leading # like KSpread does
                if (!link.displayName.trimmed().isEmpty())
                    txt = link.displayName.trimmed();
            }
        }

        oc.setValue(KSpread::Value(txt));
        if (!isFormula) {
            if (txt.startsWith('='))
                oc.setUserInput('\'' + txt);
            else
                oc.setUserInput(txt);
        }
        if (value.isRichText() || ic->format().font().subscript() || ic->format().font().superscript()) {
            std::map<unsigned, FormatFont> formatRuns = value.formatRuns();
            // add sentinel to list of format runs
            if (!formatRuns.count(0))
                formatRuns[0] = ic->format().font();
            formatRuns[txt.length()] = ic->format().font();

            QSharedPointer<QTextDocument> doc(new QTextDocument(txt));
            QTextCursor c(doc.data());
            for (std::map<unsigned, FormatFont>::iterator it = formatRuns.begin(); it != formatRuns.end(); ++it) {
                std::map<unsigned, FormatFont>::iterator it2 = it; it2++;
                if (it2 != formatRuns.end()) {
                    // select block
                    c.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, it2->first - it->first);
                    c.setCharFormat(convertFontToCharFormat(it->second));
                    c.clearSelection();
                }
            }
            oc.setRichText(doc);
        }
    } else if (value.isError()) {
        KSpread::Value v(Value::Error);
        v.setError(value.asString());
        oc.setValue(v);
    }

    QString note = ic->note();
    if (!note.isEmpty())
        oc.setComment(note);

    int styleId = convertStyle(&ic->format(), formula);
    cellStyles[styleId] += QRect(oc.column(), oc.row(), 1, 1);
    QHash<QString, KSpread::Conditions>::iterator conds = dataStyleConditions.find(ic->format().valueFormat());
    if (conds != dataStyleConditions.end()) {
        cellConditions.append(qMakePair(QRegion(oc.column(), oc.row(), 1, 1), conds.value()));
    }

    processCellObjects(ic, oc);
}

void ExcelImport::Private::processCellObjects(Cell* ic, KSpread::Cell oc)
{
    bool hasObjects = false;

    // handle pictures
    foreach(PictureObject *picture, ic->pictures()) {
        if (!hasObjects) {
            shapesXml->startElement("table:table-cell");
            shapesXml->addAttribute("table:row", oc.row());
            shapesXml->addAttribute("table:column", oc.column());
            hasObjects = true;
        }

        Sheet* const sheet = ic->sheet();
        const unsigned long colL = picture->m_colL;
        const unsigned long dxL = picture->m_dxL;
        const unsigned long colR = picture->m_colR;
        const unsigned long dxR = picture->m_dxR;
        const unsigned long rwB = picture->m_rwB;
        const unsigned long dyT = picture->m_dyT;
        const unsigned long rwT = picture->m_rwT;
        const unsigned long dyB = picture->m_dyB;

        shapesXml->startElement("draw:frame");
        //xmlWriter->addAttribute("draw:name", "Graphics 1");
        shapesXml->addAttribute("table:end-cell-address", encodeAddress(sheet->name(), picture->m_colR, picture->m_rwB));
        shapesXml->addAttributePt("table:end-x", offset(columnWidth(sheet, colR), dxR, 1024));
        shapesXml->addAttributePt("table:end-y", offset(rowHeight(sheet, rwB), dyB, 256));
        shapesXml->addAttribute("draw:z-index", "0");
        shapesXml->addAttributePt("svg:x", offset(columnWidth(sheet, colL), dxL, 1024) );
        shapesXml->addAttributePt("svg:y", offset(rowHeight(sheet, rwT), dyT, 256));

        shapesXml->startElement("draw:image");
        shapesXml->addAttribute("xlink:href", "Pictures/" + picture->fileName());
        shapesXml->addAttribute("xlink:type", "simple");
        shapesXml->addAttribute("xlink:show", "embed");
        shapesXml->addAttribute("xlink:actuate", "onLoad");
        shapesXml->endElement(); // draw:image
        shapesXml->endElement(); // draw:frame

        insertPictureManifest(picture);
    }

    // handle charts
    foreach(ChartObject *chart, ic->charts()) {
        Sheet* const sheet = ic->sheet();
        if(chart->m_chart->m_impl==0) {
            kDebug() << "Invalid chart to be created, no implementation.";
            continue;
        }

        if (!hasObjects) {
            shapesXml->startElement("table:table-cell");
            shapesXml->addAttribute("table:row", oc.row());
            shapesXml->addAttribute("table:column", oc.column());
            hasObjects = true;
        }

        ChartExport *c = new ChartExport(chart->m_chart);
        c->m_href = QString("Chart%1").arg(this->charts.count()+1);
        c->m_endCellAddress = encodeAddress(sheet->name(), chart->m_colR, chart->m_rwB);
        c->m_notifyOnUpdateOfRanges = "Sheet1.D2:Sheet1.F2";

        const unsigned long colL = chart->m_colL;
        const unsigned long dxL = chart->m_dxL;
        const unsigned long dyT = chart->m_dyT;
        const unsigned long rwT = chart->m_rwT;

        c->m_x = offset(columnWidth(sheet, colL), dxL, 1024);
        c->m_y = offset(rowHeight(sheet, rwT), dyT, 256);

        if (!chart->m_chart->m_cellRangeAddress.isNull() )
            c->m_cellRangeAddress = encodeAddress(sheet->name(), chart->m_chart->m_cellRangeAddress.left(), chart->m_chart->m_cellRangeAddress.top()) + ":" +
                                    encodeAddress(sheet->name(), chart->m_chart->m_cellRangeAddress.right(), chart->m_chart->m_cellRangeAddress.bottom());

        this->charts << c;

        c->saveIndex(shapesXml);
    }



    // handle ODraw objects
    QList<OfficeArtObject*> objects = ic->drawObjects();
    if (!objects.empty()) {
        if (!hasObjects) {
            shapesXml->startElement("table:table-cell");
            shapesXml->addAttribute("table:row", oc.row());
            shapesXml->addAttribute("table:column", oc.column());
            hasObjects = true;
        }
        ODrawClient client = ODrawClient(ic->sheet());
        ODrawToOdf odraw(client);
        Writer writer(*shapesXml, *shapeStyles, false);
        foreach (OfficeArtObject* o,objects) {
            client.setShapeText(o->text());
            odraw.processDrawingObject(o->object(), writer);
        }
    }

    if (hasObjects) {
        shapesXml->endElement();
    }
}

void ExcelImport::Private::processCharts(KoXmlWriter* manifestWriter)
{
    foreach(ChartExport *c, this->charts) {
        c->saveContent(this->storeout, manifestWriter);
    }
}

int ExcelImport::Private::convertStyle(const Format* format, const QString& formula)
{
    CellFormatKey key(format, formula);
    int& styleId = styleCache[key];
    if (!styleId) {
        KSpread::Style style;
        style.setDefault();

        if (!key.isGeneral) {
            style.merge(dataStyleCache.value(format->valueFormat(), KSpread::Style()));
        } else {
            if (key.decimalCount >= 0) {
                style.setFormatType(KSpread::Format::Number);
                style.setPrecision(key.decimalCount);
                QString format = ".";
                for (int i = 0; i < key.decimalCount; i++) {
                    format += '0';
                }
                style.setCustomFormat(format);
            }
        }

        processFontFormat(format->font(), style);

        FormatAlignment align = format->alignment();
        if (!align.isNull()) {
            switch (align.alignY()) {
            case Format::Top:
                style.setVAlign(KSpread::Style::Top);
                break;
            case Format::Middle:
                style.setVAlign(KSpread::Style::Middle);
                break;
            case Format::Bottom:
                style.setVAlign(KSpread::Style::Bottom);
                break;
            case Format::VJustify:
                style.setVAlign(KSpread::Style::VJustified);
                break;
            case Format::VDistributed:
                style.setVAlign(KSpread::Style::VDistributed);
                break;
            }

            style.setWrapText(align.wrap());

            if (align.rotationAngle()) {
                style.setAngle(align.rotationAngle());
            }

            if (align.stackedLetters()) {
                style.setVerticalText(true);
            }

            if (align.shrinkToFit()) {
                style.setShrinkToFit(true);
            }

            switch (align.alignX()) {
            case Format::Left:
                style.setHAlign(KSpread::Style::Left);
                break;
            case Format::Center:
                style.setHAlign(KSpread::Style::Center);
                break;
            case Format::Right:
                style.setHAlign(KSpread::Style::Right);
                break;
            case Format::Justify:
            case Format::Distributed:
                style.setHAlign(KSpread::Style::Justified);
                break;
            }

            if (align.indentLevel() != 0) {
                style.setIndentation(align.indentLevel() * 10);
            }
        }

        FormatBorders borders = format->borders();
        if (!borders.isNull()) {
            style.setLeftBorderPen(convertBorder(borders.leftBorder()));
            style.setRightBorderPen(convertBorder(borders.rightBorder()));
            style.setTopBorderPen(convertBorder(borders.topBorder()));
            style.setBottomBorderPen(convertBorder(borders.bottomBorder()));
            style.setFallDiagonalPen(convertBorder(borders.topLeftBorder()));
            style.setGoUpDiagonalPen(convertBorder(borders.bottomLeftBorder()));
        }

        FormatBackground back = format->background();
        if (!back.isNull() && back.pattern() != FormatBackground::EmptyPattern) {
            QColor backColor = back.backgroundColor();
            if (back.pattern() == FormatBackground::SolidPattern)
                backColor = back.foregroundColor();
            style.setBackgroundColor(backColor);

            QBrush brush;
            switch (back.pattern()) {
            case FormatBackground::SolidPattern:
                brush.setStyle(Qt::SolidPattern);
                brush.setColor(backColor);
                break;
            case FormatBackground::Dense3Pattern: // 88% gray
                brush.setStyle(Qt::Dense2Pattern);
                brush.setColor(Qt::black);
                break;
            case FormatBackground::Dense4Pattern: // 50% gray
                brush.setStyle(Qt::Dense4Pattern);
                brush.setColor(Qt::black);
                break;
            case FormatBackground::Dense5Pattern: // 37% gray
                brush.setStyle(Qt::Dense5Pattern);
                brush.setColor(Qt::black);
                break;
            case FormatBackground::Dense6Pattern: // 12% gray
                brush.setStyle(Qt::Dense6Pattern);
                brush.setColor(Qt::black);
                break;
            case FormatBackground::Dense7Pattern: // 6% gray
                brush.setStyle(Qt::Dense7Pattern);
                brush.setColor(Qt::black);
                break;

            case FormatBackground::Dense1Pattern:
            case FormatBackground::HorPattern:
                brush.setStyle(Qt::HorPattern);
                brush.setColor(Qt::black);
                break;
            case FormatBackground::VerPattern:
                brush.setStyle(Qt::VerPattern);
                brush.setColor(Qt::black);
                break;
            case FormatBackground::Dense2Pattern:
            case FormatBackground::BDiagPattern:
                brush.setStyle(Qt::BDiagPattern);
                brush.setColor(Qt::black);
                break;
            case FormatBackground::FDiagPattern:
                brush.setStyle(Qt::FDiagPattern);
                brush.setColor(Qt::black);
                break;
            case FormatBackground::CrossPattern:
                brush.setStyle(Qt::CrossPattern);
                brush.setColor(Qt::black);
                break;
            case FormatBackground::DiagCrossPattern:
                brush.setStyle(Qt::DiagCrossPattern);
                brush.setColor(Qt::black);
                break;
            }
            style.setBackgroundBrush(brush);
        }

        styleId = styleList.size();
        styleList.append(style);
    }
    return styleId;
}

void ExcelImport::Private::processFontFormat(const FormatFont& font, KSpread::Style& style)
{
    if (font.isNull()) return;

    QFont f;
    f.setBold(font.bold());
    f.setItalic(font.italic());
    f.setUnderline(font.underline());
    f.setStrikeOut(font.strikeout());
    f.setFamily(font.fontFamily());
    f.setPointSizeF(font.fontSize());
    style.setFont(f);
    style.setFontColor(font.color());
}

QTextCharFormat ExcelImport::Private::convertFontToCharFormat(const FormatFont& font)
{
    QTextCharFormat frm;
    QFont f;
    f.setBold(font.bold());
    f.setItalic(font.italic());
    f.setUnderline(font.underline());
    f.setStrikeOut(font.strikeout());
    f.setFamily(font.fontFamily());
    f.setPointSizeF(font.fontSize());
    frm.setFont(f);
    frm.setForeground(font.color());
    if (font.subscript())
        frm.setVerticalAlignment(QTextCharFormat::AlignSubScript);
    if (font.superscript())
        frm.setVerticalAlignment(QTextCharFormat::AlignSuperScript);
    return frm;
}

QPen ExcelImport::Private::convertBorder(const Pen& pen)
{
    if (pen.style == Pen::NoLine || pen.width == 0) {
        return QPen(Qt::NoPen);
    } else {
        QPen op;
        if (pen.style == Pen::DoubleLine) {
            op.setWidthF(pen.width * 3);
        } else {
            op.setWidthF(pen.width);
        }

        switch (pen.style) {
        case Pen::SolidLine: op.setStyle(Qt::SolidLine); break;
        case Pen::DashLine: op.setStyle(Qt::DashLine); break;
        case Pen::DotLine: op.setStyle(Qt::DotLine); break;
        case Pen::DashDotLine: op.setStyle(Qt::DashDotLine); break;
        case Pen::DashDotDotLine: op.setStyle(Qt::DashDotDotLine); break;
        case Pen::DoubleLine: op.setStyle(Qt::SolidLine); break; // TODO
        }

        op.setColor(pen.color);

        return op;
    }
}

void ExcelImport::Private::insertPictureManifest(PictureObject* picture)
{
    QString mimeType;
    const QString fileName = picture->fileName();
    const QString extension = fileName.right(fileName.size() - fileName.lastIndexOf('.') - 1);

    if( extension == "gif" ) {
        mimeType = "image/gif";
    }
    else if( extension == "jpg" || extension == "jpeg"
            || extension == "jpe" || extension == "jfif" ) {
        mimeType = "image/jpeg";
    }
    else if( extension == "tif" || extension == "tiff" ) {
        mimeType = "image/tiff";
    }
    else if( extension == "png" ) {
        mimeType = "image/png";
    }
    else if( extension == "emf" ) {
        mimeType = "application/x-openoffice-wmf;windows_formatname=\"Image EMF\"";
    }
    else if( extension == "wmf" ) {
        mimeType = "application/x-openoffice-wmf;windows_formatname=\"Image WMF\"";
    }
    else if( extension == "bmp" ) {
        mimeType = "image/bmp";
    }

    manifestEntries.insert(fileName, mimeType);
}

void ExcelImport::Private::addManifestEntries(KoXmlWriter* manifestWriter)
{
    QMap<QString, QString>::const_iterator iterator = manifestEntries.constBegin();
    QMap<QString, QString>::const_iterator end = manifestEntries.constEnd();
    while( iterator != end ) {
        manifestWriter->addManifestEntry(iterator.key(), iterator.value());
        ++iterator;
    }
}


// Updates the displayed progress information
void ExcelImport::Private::addProgress(int addValue)
{
    rowsCountDone += addValue;
    const int progress = int(rowsCountDone / double(rowsCountTotal) * 100.0 + 0.5);
    workbook->emitProgress(progress);
}

KoXmlWriter* ExcelImport::Private::beginMemoryXmlWriter(const char* docElement)
{
    QIODevice* d = new QBuffer;
    d->open(QIODevice::ReadWrite);
    KoXmlWriter* xml = new KoXmlWriter(d);
    xml->startDocument(docElement);
    xml->startElement(docElement);
    xml->addAttribute("xmlns:office", KoXmlNS::office);
    xml->addAttribute("xmlns:meta", KoXmlNS::meta);
    xml->addAttribute("xmlns:config", KoXmlNS::config);
    xml->addAttribute("xmlns:text", KoXmlNS::text);
    xml->addAttribute("xmlns:table", KoXmlNS::table);
    xml->addAttribute("xmlns:draw", KoXmlNS::draw);
    xml->addAttribute("xmlns:presentation", KoXmlNS::presentation);
    xml->addAttribute("xmlns:dr3d", KoXmlNS::dr3d);
    xml->addAttribute("xmlns:chart", KoXmlNS::chart);
    xml->addAttribute("xmlns:form", KoXmlNS::form);
    xml->addAttribute("xmlns:script", KoXmlNS::script);
    xml->addAttribute("xmlns:style", KoXmlNS::style);
    xml->addAttribute("xmlns:number", KoXmlNS::number);
    xml->addAttribute("xmlns:math", KoXmlNS::math);
    xml->addAttribute("xmlns:svg", KoXmlNS::svg);
    xml->addAttribute("xmlns:fo", KoXmlNS::fo);
    xml->addAttribute("xmlns:anim", KoXmlNS::anim);
    xml->addAttribute("xmlns:smil", KoXmlNS::smil);
    xml->addAttribute("xmlns:koffice", KoXmlNS::koffice);
    xml->addAttribute("xmlns:officeooo", KoXmlNS::officeooo);
    xml->addAttribute("xmlns:dc", KoXmlNS::dc);
    xml->addAttribute("xmlns:xlink", KoXmlNS::xlink);
    return xml;
}

KoXmlDocument ExcelImport::Private::endMemoryXmlWriter(KoXmlWriter* writer)
{
    writer->endElement();
    writer->endDocument();
    QBuffer* b = static_cast<QBuffer*>(writer->device());
    delete writer;

    b->seek(0);
    KoXmlDocument doc;
    QString errorMsg; int errorLine, errorColumn;
    if (!doc.setContent(b, true, &errorMsg, &errorLine, &errorColumn)) {
        kDebug() << errorMsg << errorLine << errorColumn;
    }
    delete b;
    return doc;
}

void ExcelImport::Private::processNumberFormats()
{
    static const QString sNoStyle = QString::fromLatin1("NOSTYLE");
    QHash<QString, QString> dataStyleMap;

    NumberFormatParser::setStyles(dataStyles);
    for (int i = 0; i < workbook->formatCount(); i++) {
        Format* f = workbook->format(i);
        QString& styleName = dataStyleMap[f->valueFormat()];
        if (styleName.isEmpty()) {
            KoGenStyle s = NumberFormatParser::parse(f->valueFormat());
            if (s.type() != KoGenStyle::ParagraphAutoStyle) {
                styleName = dataStyles->insert(s, "N");
            } else {
                styleName = sNoStyle; // assign it a name anyway to prevent converting it twice
            }
        }
    }

    KoXmlWriter *stylesXml = beginMemoryXmlWriter("office:styles");
    dataStyles->saveOdfStyles(KoGenStyles::DocumentAutomaticStyles, stylesXml);

    KoXmlDocument stylesDoc = endMemoryXmlWriter(stylesXml);

    KoOdfStylesReader odfStyles;
    odfStyles.createStyleMap(stylesDoc, false);

    for (int i = 0; i < workbook->formatCount(); i++) {
        Format* f = workbook->format(i);
        const QString& styleName = dataStyleMap[f->valueFormat()];
        if (styleName != sNoStyle) {
            KSpread::Style& style = dataStyleCache[f->valueFormat()];
            if (style.isEmpty()) {
                KSpread::Conditions conditions;
                style.loadOdfDataStyle(odfStyles, styleName, conditions, outputDoc->map()->styleManager(), outputDoc->map()->parser());

                if (!conditions.isEmpty())
                    dataStyleConditions[f->valueFormat()] = conditions;
            }
        }
    }
}
