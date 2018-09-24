/* This file is part of the KDE project
   Copyright (C) 2003-2006 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2006 Marijn Kruisselbrink <mkruisselbrink@kde.org>
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

#include <excelimporttoods.h>

#include <QString>
#include <QDate>
#include <QBuffer>
#include <QFontMetricsF>
#include <QPair>
#include <KoFilterChain.h>
#include <kpluginfactory.h>

#include <KoXmlWriter.h>
#include <KoOdfWriteStore.h>
#include <KoGenStyles.h>
#include <KoGenStyle.h>

#include <Charting.h>
#include <KoOdfChartWriter.h>
#include <NumberFormatParser.h>

#include "swinder.h"
#include "objects.h"
#include <iostream>
#include "ODrawClient.h"
#include "ImportUtils.h"
#include "writeodf/writeodfofficedc.h"
#include "writeodf/writeodfofficemeta.h"
#include "writeodf/writeodfofficestyle.h"
#include "writeodf/writeodfofficetable.h"
#include "writeodf/writeodftext.h"
#include "writeodf/writeodfnumber.h"
#include "writeodf/helpers.h"

K_PLUGIN_FACTORY(ExcelImportFactory, registerPlugin<ExcelImport>();)
K_EXPORT_PLUGIN(ExcelImportFactory("calligrafilters"))

#define UNICODE_EUR 0x20AC
#define UNICODE_GBP 0x00A3
#define UNICODE_JPY 0x00A5

using namespace writeodf;

QUrl urlFromArg(const QString& arg)
{
#if QT_VERSION >= 0x050400
    return QUrl::fromUserInput(arg, QDir::currentPath(), QUrl::AssumeLocalFile);
#else
    // Logic from QUrl::fromUserInput(QString, QString, UserInputResolutionOptions)
    return (QUrl(arg, QUrl::TolerantMode).isRelative() && !QDir::isAbsolutePath(arg))
           ? QUrl::fromLocalFile(QDir::current().absoluteFilePath(arg))
           : QUrl::fromUserInput(arg);
#endif
}

namespace Swinder
{
// qHash function to support hashing by Swinder::FormatFont instances.
static inline uint qHash(const Swinder::FormatFont& font)
{
    // TODO: make this a better hash
    return qHash(font.fontFamily()) ^ qRound(font.fontSize() * 100);
}

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

}

using namespace Swinder;
using namespace XlsUtils;

class ExcelImport::Private
{
public:
    QString inputFile;
    QString outputFile;

    KoStore* storeout;
    Workbook *workbook;

    KoGenStyles *styles;
    KoGenStyles *mainStyles;
    QList<QString> cellStyles;
    QList<QString> rowStyles;
    QList<QString> colStyles;
    QList<QString> colCellStyles;
    QList<QString> sheetStyles;
    QHash<FormatFont, QString> fontStyles;
    QString subScriptStyle, superScriptStyle;
    QHash<QString, KoGenStyle> valueFormatCache;
    QHash<CellFormatKey, QString> cellFormatCache;
    QList<KoOdfChartWriter*> charts;
    QHash<Cell*, QByteArray> cellShapes;
    QHash<Sheet*, QByteArray> sheetShapes;

    struct CellValue {
        Value value;
        QString str;
        QString linkName;
        QString linkLocation;
        Hyperlink link;
    };

    QHash<Row*,int> rowsRepeatedHash;
    int rowsRepeated(Row* row, int rowIndex);

    int rowsCountTotal, rowsCountDone;
    void addProgress(int addValue);

    bool createStyles(KoStore* store, KoXmlWriter* manifestWriter, KoGenStyles* mainStyles);
    bool createContent(KoOdfWriteStore* store);
    bool createMeta(KoOdfWriteStore* store);
    bool createSettings(KoOdfWriteStore* store);

    int sheetFormatIndex;
    int columnFormatIndex;
    int rowFormatIndex;
    int cellFormatIndex;

    void processWorkbookForBody(Workbook* workbook, KoXmlWriter* xmlWriter, office_body& body);
    void processWorkbookForStyle(Workbook* workbook, KoXmlWriter* xmlWriter);
    void processSheetForBody(Sheet* sheet, KoXmlWriter* xmlWriter, office_spreadsheet& spreadsheet);
    void processSheetForStyle(Sheet* sheet, KoXmlWriter* xmlWriter);
    void processSheetForHeaderFooter(Sheet* sheet, KoXmlWriter* writer);
    void processHeaderFooterStyle(const QString& text, text_p& p);
    void processColumnForBody(Sheet* sheet, int columnIndex, group_table_columns_and_groups& table, unsigned& outlineLevel);
    void processColumnForStyle(Sheet* sheet, int columnIndex, KoXmlWriter* xmlWriter);
    int processRowForBody(Sheet* sheet, int rowIndex, KoXmlWriter* xmlWriter, group_table_rows_and_groups& table, unsigned& outlineLevel);
    int processRowForStyle(Sheet* sheet, int rowIndex, KoXmlWriter* xmlWriter);
    void processCellForBody(Cell* cell, KoXmlWriter* xmlWriter, table_table_row& row);
    void processCellAttributesForBody(Cell* cell, group_table_table_cell_attlist& c, CellValue& cellValue);
    void processCellText(Cell* cell, group_paragraph_content& content, CellValue& cellValue);
    void processCellContentForBody(Cell* cell, KoXmlWriter* xmlWriter, group_table_table_cell_content& c, CellValue& cellValue);
    void processCellForStyle(Cell* cell, KoXmlWriter* xmlWriter);
    QString processCellFormat(const Format* format, const QString& formula = QString());
    QString processRowFormat(Format* format, const QString& breakBefore = QString(), int rowRepeat = 1, double rowHeight = -1);
    void processFormat(const Format* format, KoGenStyle& style);
    QString processValueFormat(const QString& valueFormat);
    void processFontFormat(const FormatFont& font, KoGenStyle& style, bool allProps = false);
    void processCharts(KoXmlWriter* manifestWriter);

    void createDefaultColumnStyle( Sheet* sheet );
    void processSheetBackground(Sheet* sheet, KoGenStyle& style);
    void addManifestEntries(KoXmlWriter* ManifestWriter);
    void insertPictureManifest(const QString &fileName);

    bool isDateFormat(const QString& valueFormat);

    QList<QString> defaultColumnStyles;
    int defaultColumnStyleIndex;
    QMap<QString,QString> manifestEntries;
};

ExcelImport::ExcelImport(QObject* parent, const QVariantList&)
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
    d->outputFile = m_chain->outputFile();

    // create output store
    d->storeout = KoStore::createStore(d->outputFile, KoStore::Write,
                                    "application/vnd.oasis.opendocument.spreadsheet", KoStore::Zip);
    if (!d->storeout || d->storeout->bad()) {
        qCWarning(lcExcelImport) << "Couldn't open the requested file.";
        delete d->workbook;
        delete d->storeout;
        return KoFilter::FileNotFound;
    }

    emit sigProgress(0);

    // open inputFile
    d->workbook = new Swinder::Workbook(d->storeout);
    connect(d->workbook, SIGNAL(sigProgress(int)), this, SIGNAL(sigProgress(int)));
    if (!d->workbook->load(d->inputFile.toLocal8Bit())) {
        delete d->workbook;
        d->workbook = 0;
        return KoFilter::StupidError;
    }

    if (d->workbook->isPasswordProtected()) {
        delete d->workbook;
        d->workbook = 0;
        return KoFilter::PasswordProtected;
    }

    emit sigProgress(-1);
    emit sigProgress(0);

    d->styles = new KoGenStyles();
    d->mainStyles = new KoGenStyles();

    KoOdfWriteStore oasisStore(d->storeout);
    KoXmlWriter* manifestWriter = oasisStore.manifestWriter("application/vnd.oasis.opendocument.spreadsheet");

    // header and footer are read from each sheet and saved in styles
    // So creating content before styles
    // store document content
    if (!d->createContent(&oasisStore)) {
        qCWarning(lcExcelImport) << "Couldn't open the file 'content.xml'.";
        delete d->workbook;
        delete d->storeout;
        return KoFilter::CreationError;
    }

    // store document styles
    if (!d->createStyles(d->storeout, manifestWriter, d->mainStyles)) {
        qCWarning(lcExcelImport) << "Couldn't open the file 'styles.xml'.";
        delete d->workbook;
        delete d->storeout;
        return KoFilter::CreationError;
    }

    // store meta content
    if (!d->createMeta(&oasisStore)) {
        qCWarning(lcExcelImport) << "Couldn't open the file 'meta.xml'.";
        delete d->workbook;
        delete d->storeout;
        return KoFilter::CreationError;
    }

    // store settings
    if (!d->createSettings(&oasisStore)) {
        qCWarning(lcExcelImport) << "Couldn't open the file 'settings.xml'.";
        delete d->workbook;
        delete d->storeout;
        return KoFilter::CreationError;
    }

    manifestWriter->addManifestEntry("meta.xml", "text/xml");
    manifestWriter->addManifestEntry("styles.xml", "text/xml");
    manifestWriter->addManifestEntry("content.xml", "text/xml");
    manifestWriter->addManifestEntry("settings.xml", "text/xml");

    d->processCharts(manifestWriter);
    d->addManifestEntries(manifestWriter);
    oasisStore.closeManifestWriter();

    // we are done!
    delete d->workbook;
    delete d->styles;
    delete d->mainStyles;
    delete d->storeout;
    d->inputFile.clear();
    d->outputFile.clear();
    d->workbook = 0;
    d->styles = 0;
    d->mainStyles = 0;
    d->cellStyles.clear();
    d->rowStyles.clear();
    d->colStyles.clear();
    d->colCellStyles.clear();
    d->sheetStyles.clear();

    emit sigProgress(100);
    return KoFilter::OK;
}

// Updates the displayed progress information
void ExcelImport::Private::addProgress(int addValue)
{
    rowsCountDone += addValue;
    const int progress = int(rowsCountDone / double(rowsCountTotal) * 100.0 + 0.5);
    workbook->emitProgress(progress);
}

int ExcelImport::Private::rowsRepeated(Row* row, int rowIndex)
{
    if(rowsRepeatedHash.contains(row))
        return rowsRepeatedHash[row];
    // a row does usually at least repeat itself
    int repeat = 1;
    // find the column of the rightmost cell (if any)
    int lastCol = row->sheet()->maxCellsInRow(rowIndex);
    // find repeating rows by forward searching
    const unsigned rowCount = qMin(maximalRowCount, row->sheet()->maxRow());
    for (unsigned i = rowIndex + 1; i <= rowCount; ++i) {
        Row *nextRow = row->sheet()->row(i, false);
        if(!nextRow) break;
        if (*row != *nextRow) break; // do the rows have the same properties?
        const int nextLastCol = row->sheet()->maxCellsInRow(i);
        if (lastCol != nextLastCol) break;
        bool cellsAreSame = true;
        for(int c = 0; c <= lastCol; ++c) {
            Cell* c1 = row->sheet()->cell(c, row->index(), false);
            Cell* c2 = nextRow->sheet()->cell(c, nextRow->index(), false);
            if (!c1 != !c2 || (c1 && *c1 != *c2)) {
                cellsAreSame = false;
                break; // job done, abort loop
            }
        }
        if (!cellsAreSame) break;
        ++repeat;
    }
    rowsRepeatedHash[row] = repeat; // cache the result
    return repeat;
}

// Writes the spreadsheet content into the content.xml
bool ExcelImport::Private::createContent(KoOdfWriteStore* store)
{
    KoXmlWriter* bodyWriter = store->bodyWriter();
    KoXmlWriter* contentWriter = store->contentWriter();
    if (!bodyWriter || !contentWriter)
        return false;

    if(workbook->password() != 0) {
        contentWriter->addAttribute("table:structure-protected-excel", "true");
        contentWriter->addAttribute("table:protection-key-excel" , uint(workbook->password()));
    }

    // FIXME this is dummy and hardcoded, replace with real font names
    office_font_face_decls decls(contentWriter);
    style_font_face font(decls.add_style_font_face("Arial"));
    font.set_svg_font_family("Arial");
    style_font_face font2(decls.add_style_font_face("Times New Roman"));
    font2.set_svg_font_family("&apos;Times New Roman&apos;");
    decls.end();

    defaultColumnStyleIndex = 0;
    // office:automatic-styles
    processWorkbookForStyle(workbook, contentWriter);
    styles->saveOdfStyles(KoGenStyles::DocumentAutomaticStyles, contentWriter);

    // important: reset all indexes
    sheetFormatIndex = 0;
    columnFormatIndex = 0;
    rowFormatIndex = 0;
    cellFormatIndex = 0;


    // office:body
    office_body body(bodyWriter);
    processWorkbookForBody(workbook, bodyWriter, body);
    body.end();

    return store->closeContentWriter();
}



// Writes the styles.xml
bool ExcelImport::Private::createStyles(KoStore* store, KoXmlWriter* manifestWriter, KoGenStyles* mainStyles)
{
    Q_UNUSED(manifestWriter);
    if (!store->open("styles.xml"))
        return false;
    KoStoreDevice dev(store);
    KoXmlWriter* stylesWriter = new KoXmlWriter(&dev);

    stylesWriter->startDocument("office:document-styles");
    office_document_styles styles(stylesWriter);
    styles.addAttribute("xmlns:office", "urn:oasis:names:tc:opendocument:xmlns:office:1.0");
    styles.addAttribute("xmlns:style", "urn:oasis:names:tc:opendocument:xmlns:style:1.0");
    styles.addAttribute("xmlns:text", "urn:oasis:names:tc:opendocument:xmlns:text:1.0");
    styles.addAttribute("xmlns:table", "urn:oasis:names:tc:opendocument:xmlns:table:1.0");
    styles.addAttribute("xmlns:draw", "urn:oasis:names:tc:opendocument:xmlns:drawing:1.0");
    styles.addAttribute("xmlns:fo", "urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0");
    styles.addAttribute("xmlns:svg", "urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0");
    styles.addAttribute("xmlns:xlink", "http://www.w3.org/1999/xlink");
    styles.addAttribute("xmlns:chart", "urn:oasis:names:tc:opendocument:xmlns:chart:1.0");
    styles.addAttribute("xmlns:dc", "http://purl.org/dc/elements/1.1/");
    styles.addAttribute("xmlns:meta", "urn:oasis:names:tc:opendocument:xmlns:meta:1.0");
    styles.addAttribute("xmlns:number", "urn:oasis:names:tc:opendocument:xmlns:datastyle:1.0");
    //styles.addAttribute("xmlns:dr3d", "urn:oasis:names:tc:opendocument:xmlns:dr3d:1.0");
    styles.addAttribute("xmlns:math", "http://www.w3.org/1998/Math/MathML");
    styles.addAttribute("xmlns:of", "urn:oasis:names:tc:opendocument:xmlns:of:1.2");

    mainStyles->saveOdfStyles(KoGenStyles::MasterStyles, stylesWriter);
    mainStyles->saveOdfStyles(KoGenStyles::DocumentStyles, stylesWriter); // office:style
    mainStyles->saveOdfStyles(KoGenStyles::DocumentAutomaticStyles, stylesWriter); // office:automatic-styles

    styles.end();
    stylesWriter->endDocument();

    delete stylesWriter;
    return store->close();
}

// Writes meta-information into the meta.xml
bool ExcelImport::Private::createMeta(KoOdfWriteStore* store)
{
    if (!store->store()->open("meta.xml"))
        return false;

    KoStoreDevice dev(store->store());
    KoXmlWriter* metaWriter = new KoXmlWriter(&dev);
    metaWriter->startDocument("office:document-meta");

    office_document_meta metadoc(metaWriter);
    metadoc.addAttribute("xmlns:office", "urn:oasis:names:tc:opendocument:xmlns:office:1.0");
    metadoc.addAttribute("xmlns:xlink", "http://www.w3.org/1999/xlink");
    metadoc.addAttribute("xmlns:dc", "http://purl.org/dc/elements/1.1/");
    metadoc.addAttribute("xmlns:meta", "urn:oasis:names:tc:opendocument:xmlns:meta:1.0");
    office_meta meta(metadoc.add_office_meta());

    if (workbook->hasProperty(Workbook::PIDSI_TITLE)) {
        meta.add_dc_title().addTextNode(
                    workbook->property(Workbook::PIDSI_TITLE).toString());
    }
    if (workbook->hasProperty(Workbook::PIDSI_SUBJECT)) {
        meta.add_dc_subject().addTextNode(
                    workbook->property(Workbook::PIDSI_SUBJECT).toString());
    }
    if (workbook->hasProperty(Workbook::PIDSI_AUTHOR)) {
        meta.add_dc_creator().addTextNode(
                    workbook->property(Workbook::PIDSI_AUTHOR).toString());
    }
    if (workbook->hasProperty(Workbook::PIDSI_KEYWORDS)) {
        meta.add_meta_keyword().addTextNode(
                    workbook->property(Workbook::PIDSI_KEYWORDS).toString());
    }
    if (workbook->hasProperty(Workbook::PIDSI_COMMENTS)) {
        meta_user_defined c(meta.add_meta_user_defined("comments"));
        c.set_meta_value_type("string");
        c.addTextNode(
                    workbook->property(Workbook::PIDSI_COMMENTS).toString());
    }
    if (workbook->hasProperty(Workbook::PIDSI_REVNUMBER)) {
        meta.add_meta_editing_cycles().addTextNode(
                    workbook->property(Workbook::PIDSI_REVNUMBER).toString());
    }
    if (workbook->hasProperty(Workbook::PIDSI_LASTPRINTED_DTM)) {
        meta.add_meta_print_date().addTextNode(
                    workbook->property(Workbook::PIDSI_LASTPRINTED_DTM).toString());
    }
    if (workbook->hasProperty(Workbook::PIDSI_CREATE_DTM)) {
        meta.add_meta_creation_date().addTextNode(
                    workbook->property(Workbook::PIDSI_CREATE_DTM).toString());
    }
    if (workbook->hasProperty(Workbook::PIDSI_LASTSAVED_DTM)) {
        meta.add_dc_date().addTextNode(
                    workbook->property(Workbook::PIDSI_LASTSAVED_DTM).toString());
    }

    //if( workbook->hasProperty( Workbook::PIDSI_TEMPLATE )  ) metaWriter->addAttribute( "dc:", workbook->property( Workbook::PIDSI_TEMPLATE ).toString() );
    //if( workbook->hasProperty( Workbook::PIDSI_LASTAUTHOR )  ) metaWriter->addAttribute( "dc:", workbook->property( Workbook::PIDSI_LASTAUTHOR ).toString() );
    //if( workbook->hasProperty( Workbook::PIDSI_EDITTIME )  ) metaWriter->addAttribute( "dc:date", workbook->property( Workbook::PIDSI_EDITTIME ).toString() );

    metadoc.end();
    metaWriter->endDocument();

    delete metaWriter;
    return store->store()->close();
}

// Writes configuration-settings into the settings.xml
bool ExcelImport::Private::createSettings(KoOdfWriteStore* store)
{
    if (!store->store()->open("settings.xml"))
        return false;

    KoStoreDevice dev(store->store());
    KoXmlWriter* settingsWriter = KoOdfWriteStore::createOasisXmlWriter(&dev, "office:document-settings");
    {
    office_settings settings(settingsWriter);
    config_config_item_set set(settings.add_config_config_item_set("view-settings"));

    // units...

    // settings
    config_config_item_map_indexed map(set.add_config_config_item_map_indexed("Views"));
    config_config_item_map_entry entry(map.add_config_config_item_map_entry());

    addConfigItem(entry, "ViewId", QString::fromLatin1("View1"));
    if(Sheet *sheet = workbook->sheet(workbook->activeTab()))
            addConfigItem(entry, "ActiveTable", sheet->name());

    config_config_item_map_named named(entry.add_config_config_item_map_named("Tables"));
    for(uint i = 0; i < workbook->sheetCount(); ++i) {
        Sheet* sheet = workbook->sheet(i);
        config_config_item_map_entry entry(named.add_config_config_item_map_entry());
        entry.set_config_name(sheet->name());
        QPoint point = sheet->firstVisibleCell();
        addConfigItem(entry, "CursorPositionX", point.x());
        addConfigItem(entry, "CursorPositionY", point.y());
        //TODO how should we replace these settings?
//         settingsWriter->addConfigItem("xOffset", columnWidth(sheet,point.x()));
//         settingsWriter->addConfigItem("yOffset", rowHeight(sheet,point.y()));
        addConfigItem(entry, "ShowZeroValues", sheet->showZeroValues());
        addConfigItem(entry, "ShowGrid", sheet->showGrid());
        addConfigItem(entry, "FirstLetterUpper", false);
        addConfigItem(entry, "ShowFormulaIndicator", false);
        addConfigItem(entry, "ShowCommentIndicator", true);
        addConfigItem(entry, "ShowPageOutline", sheet->isPageBreakViewEnabled()); // best match Sheets provides
        addConfigItem(entry, "lcmode", false);
        addConfigItem(entry, "autoCalc", sheet->autoCalc());
        addConfigItem(entry, "ShowColumnNumber", false);
    }
    } // end of block closes all elements
    settingsWriter->endDocument();
    delete settingsWriter;
    return store->store()->close();
}

// Processes the workbook content. The workbook is the top-level element for content.
void ExcelImport::Private::processWorkbookForBody(Workbook* workbook, KoXmlWriter* xmlWriter, office_body& body)
{
    if (!workbook) return;
    if (!xmlWriter) return;

    office_spreadsheet spreadsheet(body.add_office_spreadsheet());

    table_calculation_settings calcsettings = spreadsheet.add_table_calculation_settings();
    calcsettings.set_table_case_sensitive(false);
    calcsettings.set_table_automatic_find_labels(false);
    calcsettings.set_table_use_regular_expressions(false);
    calcsettings.set_table_use_wildcards(true);

    // count the number of rows in total to provide a good progress value
    rowsCountTotal = rowsCountDone = 0;
    for (unsigned i = 0; i < workbook->sheetCount(); ++i) {
        Sheet* sheet = workbook->sheet(i);
        rowsCountTotal += qMin(maximalRowCount, sheet->maxRow()) * 2; // double cause we will count them 2 times, once for styles and once for content
    }

    // now start the whole work
    for (unsigned i = 0; i < workbook->sheetCount(); ++i) {
        Sheet* sheet = workbook->sheet(i);
        processSheetForBody(sheet, xmlWriter, spreadsheet);
    }

    std::map<std::pair<unsigned, QString>, QString> &namedAreas = workbook->namedAreas();
    if(namedAreas.size() > 0) {
        table_named_expressions exprs(spreadsheet.add_table_named_expressions());
        for(std::map<std::pair<unsigned, QString>, QString>::iterator it = namedAreas.begin(); it != namedAreas.end(); ++it) {
            QString range = it->second;
            if(range.startsWith(QLatin1Char('[')) && range.endsWith(QLatin1Char(']'))) {
                range.remove(0, 1).chop(1);
            }
            table_named_range(exprs.add_table_named_range(range, it->first.second));
        }
    }

    table_database_ranges ranges(spreadsheet.add_table_database_ranges());
    int rangeId = 1;
    for (unsigned i = 0; i < workbook->sheetCount(); ++i) {
        QList<QRect> filters = workbook->filterRanges(i);
        QString sheetName = workbook->sheet(i)->name();
        if (filters.size()) {
            foreach (const QRect& filter, filters) {
                QString sRange(encodeAddress(sheetName, filter.left(), filter.top()));
                sRange.append(":");
                sRange.append(encodeAddress(sheetName, filter.right(), workbook->sheet(i)->maxRow()));
                table_database_range range(ranges.add_table_database_range(sRange));
                range.set_table_name(QString("excel-database-%1").arg(rangeId++));
                range.set_table_display_filter_buttons("true");
            }
        }
    }
}

// Processes the workbook styles. The workbook is the top-level element for content.
void ExcelImport::Private::processWorkbookForStyle(Workbook* workbook, KoXmlWriter* xmlWriter)
{
    if (!workbook) return;
    if (!xmlWriter) return;

    QString contentElement;
    QString masterStyleName("Default");
    QString pageLayoutStyleName("Mpm");

    KoGenStyle pageLayoutStyle(KoGenStyle::PageLayoutStyle);
    pageLayoutStyle.addProperty("style:writing-mode", "lr-tb");

    QBuffer buf;
    buf.open(QIODevice::WriteOnly);
    KoXmlWriter writer(&buf);

    //Hardcoded page-layout
    style_header_style header(&writer);
    style_header_footer_properties hf(header.add_style_header_footer_properties());
    hf.set_fo_min_height("20pt");
    hf.set_fo_margin_left("0pt");
    hf.set_fo_margin_right("0pt");
    hf.set_fo_margin_bottom("10pt");
    header.end();

    style_footer_style footer(&writer);
    style_header_footer_properties hf2(footer.add_style_header_footer_properties());
    hf2.set_fo_min_height("20pt");
    hf2.set_fo_margin_left("0pt");
    hf2.set_fo_margin_right("0pt");
    hf2.set_fo_margin_top("10pt");
    footer.end();

    QString pageLyt = QString::fromUtf8(buf.buffer(), buf.buffer().size());
    buf.close();
    buf.setData("", 0);

    pageLayoutStyle.addProperty("1header-footer-style", pageLyt, KoGenStyle::StyleChildElement);
    pageLayoutStyleName = mainStyles->insert(pageLayoutStyle, pageLayoutStyleName, KoGenStyles::DontAddNumberToName);

    for (unsigned i = 0; i < workbook->sheetCount(); ++i) {
        Sheet* sheet = workbook->sheet(i);
        processSheetForStyle(sheet, xmlWriter);

        buf.open(QIODevice::WriteOnly);
        processSheetForHeaderFooter(workbook->sheet(0), &writer);
        contentElement = QString::fromUtf8(buf.buffer(), buf.buffer().size());
        buf.close();
        QString childElementName = QString::number(i).append("master-style");
        KoGenStyle masterStyle(KoGenStyle::MasterPageStyle);
        masterStyle.addChildElement(childElementName, contentElement);
        masterStyle.addAttribute("style:page-layout-name", pageLayoutStyleName);

        masterStyleName = mainStyles->insert(masterStyle, masterStyleName, KoGenStyles::DontAddNumberToName);
        masterStyle.addAttribute("style:name", masterStyleName);
    }
}

// Processes a sheet.
void ExcelImport::Private::processSheetForBody(Sheet* sheet, KoXmlWriter* xmlWriter, office_spreadsheet& spreadsheet)
{
    if (!sheet) return;
    if (!xmlWriter) return;

    table_table table(spreadsheet.add_table_table());

    table.set_table_name(sheet->name());
    table.set_table_print("false");
    table.set_table_style_name(sheetStyles[sheetFormatIndex]);
    ++sheetFormatIndex;

    if(sheet->password() != 0) {
        //TODO
       //xmlWriter->addAttribute("table:protected", "true");
       //xmlWriter->addAttribute("table:protection-key", uint(sheet->password()));
    }

    if (!sheet->drawObjects().isEmpty()) {
        table_shapes shapes(table.add_table_shapes());
        shapes.addCompleteElement(sheetShapes[sheet]);
    }


    const unsigned columnCount = qMin(maximalColumnCount, sheet->maxColumn());
    unsigned outlineLevel = 0;
    for (unsigned i = 0; i <= columnCount; ++i) {
        processColumnForBody(sheet, i, table, outlineLevel);
    }

    // in odf default-cell-style's only apply to cells/rows/columns that are present in the file while in Excel
    // row/column styles should apply to all cells in that row/column. So, try to fake that behavior by writing
    // a number-columns-repeated to apply the styles/formattings to "all" columns.
    if (columnCount < maximalColumnCount-1) {
        table_table_column column(table.add_table_table_column());
        column.set_table_style_name(defaultColumnStyles[defaultColumnStyleIndex]);
        column.set_table_number_columns_repeated(maximalColumnCount - 1 - columnCount);
    }

    // add rows
    outlineLevel = 0;
    const unsigned rowCount = qMin(maximalRowCount, sheet->maxRow());
    for (unsigned i = 0; i <= rowCount;) {
        i += processRowForBody(sheet, i, xmlWriter, table, outlineLevel);
    }

    // same we did above with columns is also needed for rows.
    if(rowCount < maximalRowCount-1) {
        table_table_row row(table.add_table_table_row());
        row.set_table_number_rows_repeated(maximalRowCount - 1 - rowCount);
        row.add_table_table_cell();
    }
    ++defaultColumnStyleIndex;
}

static QRectF getRect(const MSO::OfficeArtFSPGR &r)
{
    return QRect(r.xLeft, r.yTop, r.xRight - r.xLeft, r.yBottom - r.yTop);
}

// Processes styles for a sheet.
void ExcelImport::Private::processSheetForStyle(Sheet* sheet, KoXmlWriter* xmlWriter)
{
    if (!sheet) return;
    if (!xmlWriter) return;

    KoGenStyle style(KoGenStyle::TableAutoStyle, "table");
    style.addAttribute("style:master-page-name", "Default");

    style.addProperty("table:display", sheet->visible() ? "true" : "false");
    style.addProperty("table:writing-mode", "lr-tb");

    processSheetBackground(sheet, style);

    QString styleName = styles->insert(style, "ta");
    sheetStyles.append(styleName);

    createDefaultColumnStyle( sheet );

    const unsigned columnCount = qMin(maximalColumnCount, sheet->maxColumn());
    for (unsigned i = 0; i <= columnCount; ++i) {
        processColumnForStyle(sheet, i, xmlWriter);
    }

    const unsigned rowCount = qMin(maximalRowCount, sheet->maxRow());
    for (unsigned i = 0; i <= rowCount;) {
        i += processRowForStyle(sheet, i, xmlWriter);
    }

    QList<OfficeArtObject*> objects = sheet->drawObjects();
    int drawObjectGroups = sheet->drawObjectsGroupCount();
    if (!objects.empty() || drawObjectGroups) {
        ODrawClient client = ODrawClient(sheet);
        ODrawToOdf odraw(client);
        QBuffer b;
        KoXmlWriter xml(&b);
        Writer writer(xml, *styles, false);
        foreach (const OfficeArtObject* o, objects) {
            client.setShapeText(o->text());
            odraw.processDrawingObject(o->object(), writer);
        }
        for (int i = 0; i < drawObjectGroups; ++i) {
            xml.startElement("draw:g");

            const MSO::OfficeArtSpgrContainer& group = sheet->drawObjectsGroup(i);
            const MSO::OfficeArtSpContainer* first = group.rgfb.first().anon.get<MSO::OfficeArtSpContainer>();
            if (first && first->clientAnchor && first->shapeGroup) {
                QRectF oldCoords = client.getGlobalRect(*first->clientAnchor);
                QRectF newCoords = getRect(*first->shapeGroup);
                Writer transw = writer.transform(oldCoords, newCoords);
                foreach (const OfficeArtObject* o, sheet->drawObjects(i)) {
                    client.setShapeText(o->text());
                    odraw.processDrawingObject(o->object(), transw);
                }
            } else {
                foreach (const OfficeArtObject* o, sheet->drawObjects(i)) {
                    client.setShapeText(o->text());
                    odraw.processDrawingObject(o->object(), writer);
                }
            }
            xml.endElement(); // draw:g
        }
        sheetShapes[sheet] = b.data();
        //qCDebug(lcExcelImport) << b.data();
    }
}

// Processes headers and footers for a sheet.
void ExcelImport::Private::processSheetForHeaderFooter(Sheet* sheet, KoXmlWriter* xmlWriter)
{
    if (!sheet) return;
    if (!xmlWriter) return;

    style_header header(xmlWriter);
    if (!sheet->leftHeader().isEmpty()) {
        style_region_left left(header.add_style_region_left());
        text_p p(left.add_text_p());
        processHeaderFooterStyle(sheet->leftHeader(), p);
    }
    if (!sheet->centerHeader().isEmpty()) {
        style_region_center center(header.add_style_region_center());
        text_p p(center.add_text_p());
        processHeaderFooterStyle(sheet->centerHeader(), p);
    }
    if (!sheet->rightHeader().isEmpty()) {
        style_region_right right(header.add_style_region_right());
        text_p p(right.add_text_p());
        processHeaderFooterStyle(sheet->rightHeader(), p);
    }
    header.end();

    style_footer footer(xmlWriter);
    if (!sheet->leftFooter().isEmpty()) {
        style_region_left left(footer.add_style_region_left());
        text_p p(left.add_text_p());
        processHeaderFooterStyle(sheet->leftFooter(), p);
    }
    if (!sheet->centerFooter().isEmpty()) {
        style_region_center center(footer.add_style_region_center());
        text_p p(center.add_text_p());
        processHeaderFooterStyle(sheet->centerFooter(), p);
    }
    if (!sheet->rightFooter().isEmpty()) {
        style_region_right right(footer.add_style_region_right());
        text_p p(right.add_text_p());
        processHeaderFooterStyle(sheet->rightFooter(), p);
    }
}

// Processes the styles of a headers and footers for a sheet.
void ExcelImport::Private::processHeaderFooterStyle(const QString& text, text_p& p)
{
    bool skipUnsupported = false;
    int lastPos;
    int pos = text.indexOf('&');
    int len = text.length();
    if ((pos < 0) && (text.length() > 0))   // If ther is no &
        p.addTextNode(text);
    else if (pos > 0) // Some text and '&'
        p.addTextNode(text.mid(0,  pos - 1));

    while (pos >= 0) {
        switch (text[pos + 1].unicode()) {
        case 'D':
            p.add_text_date().addTextNode(QDate::currentDate().toString("DD/MM/YYYY"));
            break;
        case 'T':
            p.add_text_time().addTextNode(QTime::currentTime().toString("HH:MM:SS"));
            break;
        case 'P':
            p.add_text_page_number().addTextNode("1");
            break;
        case 'N':
            p.add_text_page_count().addTextNode("999");
            break;
        case 'F':
            p.add_text_title().addTextNode("???");
            break;
        case 'A':
            p.add_text_sheet_name().addTextNode("???");
            break;
        case '\"':
        default:
            skipUnsupported = true;
            break;
        }
        lastPos = pos;
        pos = text.indexOf('&', lastPos + 1);
        if (!skipUnsupported && (pos > (lastPos + 1)))
            p.addTextNode(text.mid(lastPos + 2, (pos - lastPos - 2)));
        else if (!skipUnsupported && (pos < 0))  //Remaining text
            p.addTextNode(text.mid(lastPos + 2, len - (lastPos + 2)));
        else
            skipUnsupported = false;
    }
}

// Processes a column in a sheet.
void ExcelImport::Private::processColumnForBody(Sheet* sheet, int columnIndex, group_table_columns_and_groups& table, unsigned& outlineLevel)
{
    Column* column = sheet->column(columnIndex, false);

    unsigned newOutlineLevel = column ? column->outlineLevel() : 0;
    if (newOutlineLevel > outlineLevel) {
        table_table_column_group group(table.add_table_table_column_group());
        outlineLevel++;
        if (outlineLevel == newOutlineLevel && column->collapsed())
            group.set_table_display("false");
        processColumnForBody(sheet, columnIndex, group, outlineLevel);
        outlineLevel--;
        return;
    }

    if (!column) {
        table_table_column column(table.add_table_table_column());
        Q_ASSERT(defaultColumnStyleIndex < defaultColumnStyles.count());
        column.set_table_style_name(defaultColumnStyles[defaultColumnStyleIndex] );
        return;
    }
    Q_ASSERT(columnFormatIndex < colStyles.count());
    Q_ASSERT(columnFormatIndex < colCellStyles.count());
    const QString styleName = colStyles[columnFormatIndex];
    const QString defaultStyleName = colCellStyles[columnFormatIndex];
    columnFormatIndex++;

    table_table_column c(table.add_table_table_column());
    c.set_table_default_cell_style_name(defaultStyleName);
    c.set_table_visibility(column->visible() ? "visible" : "collapse");
    //c.set_table_number_columns_repeated( );
    c.set_table_style_name(styleName);
}

// Processes the style of a column in a sheet.
void ExcelImport::Private::processColumnForStyle(Sheet* sheet, int columnIndex, KoXmlWriter* xmlWriter)
{
    Column* column = sheet->column(columnIndex, false);

    if (!xmlWriter) return;
    if (!column) return;

    KoGenStyle style(KoGenStyle::TableColumnAutoStyle, "table-column");
    style.addProperty("fo:break-before", "auto");
    style.addPropertyPt("style:column-width", column->width());

    QString styleName = styles->insert(style, "co");
    colStyles.append(styleName);

    const Format* format = &column->format();
    QString cellStyleName = processCellFormat(format);
    colCellStyles.append(cellStyleName);
}

// Processes a row in a sheet.
int ExcelImport::Private::processRowForBody(Sheet* sheet, int rowIndex, KoXmlWriter* xmlWriter, group_table_rows_and_groups& table, unsigned& outlineLevel)
{
    int repeat = 1;

    Row *row = sheet->row(rowIndex, false);

    unsigned newOutlineLevel = row ? row->outlineLevel() : 0;
    if (newOutlineLevel > outlineLevel) {
        table_table_row_group group(table.add_table_table_row_group());
        outlineLevel++;
        if (outlineLevel == newOutlineLevel && row->collapsed())
            group.set_table_display("false");
        processRowForBody(sheet, rowIndex, xmlWriter, group, outlineLevel);
        outlineLevel--;
        return repeat;
    }

    if (!row) {
        table_table_row row(table.add_table_table_row());
        row.add_table_table_cell();
        return repeat;
    }
    if (!row->sheet()) return repeat;

    const QString styleName = rowStyles[rowFormatIndex];
    rowFormatIndex++;

    repeat = rowsRepeated(row, rowIndex);

    table_table_row r(table.add_table_table_row());
    r.set_table_visibility(row->visible() ? "visible" : "collapse");
    r.set_table_style_name(styleName);

    if(repeat > 1)
        r.set_table_number_rows_repeated(repeat);

    // find the column of the rightmost cell (if any)
    const int lastCol = row->sheet()->maxCellsInRow(rowIndex);
    int i = 0;
    do {
        Cell* cell = row->sheet()->cell(i, row->index(), false);
        if (cell) {
            processCellForBody(cell, xmlWriter, r);
            i += cell->columnRepeat();
        } else { // empty cell
            r.add_table_table_cell();
            ++i;
        }
    } while(i <= lastCol);

    addProgress(repeat);
    return repeat;
}

// Processes the style of a row in a sheet.
int ExcelImport::Private::processRowForStyle(Sheet* sheet, int rowIndex, KoXmlWriter* xmlWriter)
{
    int repeat = 1;
    Row* row = sheet->row(rowIndex, false);

    if (!row) return repeat;
    if (!row->sheet()) return repeat;
    if (!xmlWriter) return repeat;

    repeat = rowsRepeated(row, rowIndex);

    Format format = row->format();
    QString cellStyleName = processRowFormat(&format, "auto", repeat, row->height());
    rowStyles.append(cellStyleName);

    const int lastCol = row->sheet()->maxCellsInRow(rowIndex);
    for (int i = 0; i <= lastCol;) {
        Cell* cell = row->sheet()->cell(i, row->index(), false);
        if (cell) {
            processCellForStyle(cell, xmlWriter);
            i += cell->columnRepeat();
        } else { // row has no style
            ++i;
        }
    }

    addProgress(repeat);
    return repeat;
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
    if (text.startsWith('_') && text.length() >= 3) {
        QChar end;
        if (text[1] == '(') end = ')';
        else if (text[1] == '_') end = '_';
        else if (text[1] == ' ') end = ' ';
        else qCDebug(lcExcelImport) << "Probably unhandled condition=" << text[1] << "in text=" << text;
        if (! end.isNull()) {
            {
                QString regex = QString("^_%1(.*\"\\$\".*)%2;.*").arg(QString("\\%1").arg(text[1])).arg(QString("\\%1").arg(end));
                QRegExp ex(regex);
                ex.setMinimal(true);
                if (ex.indexIn(text) >= 0) return ex.cap(1);
            }
            {
                QString regex = QString("^_%1(.*\\[\\$.*\\].*)%2;.*").arg(QString("\\%1").arg(text[1])).arg(QString("\\%1").arg(end));
                QRegExp ex(regex);
                ex.setMinimal(true);
                if (ex.indexIn(text) >= 0) return ex.cap(1);
            }
        }
    }
#else
    if (text.startsWith('_')) {
        return text.split(';').first();
    }
#endif
    return text;
}

#if 0
// Currency or accounting format.
// "$"* #,##0.0000_
// [$EUR]\ #,##0.00"
// [$₩-412]* #,##0.0000
// * #,##0_)[$€-1]_
static bool currencyFormat(const QString& valueFormat, QString *currencyVal = 0, QString * formatVal = 0)
{
    QString vf = extractConditional(valueFormat);

    // dollar is special cause it starts with a $
    QRegExp dollarRegEx("^\"\\$\"[*\\-\\s]*([#,]*[\\d]+(|.[#0]+)).*");
    if (dollarRegEx.indexIn(vf) >= 0) {
        if (currencyVal) *currencyVal = "$";
        if (formatVal) *formatVal = dollarRegEx.cap(1);
        return true;
    }

    // every other currency or accounting has a [$...] identifier
    QRegExp crRegEx("\\[\\$(.*)\\]");
    crRegEx.setMinimal(true);
    if (crRegEx.indexIn(vf) >= 0) {
        if (currencyVal) {
            *currencyVal = crRegEx.cap(1);
        }
        if (formatVal) {
            QRegExp vlRegEx("([#,]*[\\d]+(|.[#0]+))");
            *formatVal = vlRegEx.indexIn(vf) >= 0 ? vlRegEx.cap(1) : QString();
        }
        return true;
    }

    return false;
}
#endif

// Checks if the as argument passed formatstring defines a date-format or not.
bool ExcelImport::Private::isDateFormat(const QString& valueFormat)
{
    KoGenStyle& style = valueFormatCache[valueFormat];
    if (style.isEmpty()) {
        style = NumberFormatParser::parse( valueFormat );
    }
    return style.type() == KoGenStyle::NumericDateStyle;
}

#if 0
static QByteArray convertCurrency(double currency, const QString& valueFormat)
{
    Q_UNUSED(valueFormat);
    return QByteArray::number(currency, 'g', 15);
}
#endif

static QString convertDate(double serialNo, const QString& valueFormat)
{
    QString vf = valueFormat;
    QString locale = extractLocale(vf);
    Q_UNUSED(locale);   //TODO http://msdn.microsoft.com/en-us/goglobal/bb964664.aspx
    Q_UNUSED(vf);   //TODO

    // reference is midnight 30 Dec 1899
    QDateTime dt(QDate(1899, 12, 30));
    dt = dt.addMSecs((qint64)(serialNo * 86400 * 1000)); // TODO: we probably need double precision here

    //TODO atm we always return a datetime. This works great (time ignored if only date was defined) with Calligra Sheets but probably not with other customers...
    //return dd.toString("yyyy-MM-dd");
    return dt.toString("yyyy-MM-ddThh:mm:ss");
}

static QTime convertToTime(double serialNo)
{
    //QString locale = extractLocale(vf);
    //Q_UNUSED(locale);   //TODO http://msdn.microsoft.com/en-us/goglobal/bb964664.aspx

    // reference is midnight 30 Dec 1899
    QTime tt;
    tt = tt.addMSecs(qRound((serialNo - (int)serialNo) * 86400 * 1000));
    qCDebug(lcExcelImport) << tt;
    return tt;
}

QString cellFormula(Cell* cell)
{
    QString formula = cell->formula();
    if (!formula.isEmpty()) {
        if(formula.startsWith("ROUNDUP(") || formula.startsWith("ROUNDDOWN(") || formula.startsWith("ROUND(") || formula.startsWith("RAND(")) {
            // Special case Excel formulas that differ from OpenFormula
            formula.prepend("msoxl:=");
        } else if (!formula.isEmpty()) {
            formula.prepend("=");
        }
    }
    return formula;
}

QString currencyValue(const QString &value)
{
    if (value.isEmpty()) return QString();
    if (value[0] == '$') return "USD";
    if (value[0] == QChar(UNICODE_EUR)) return "EUR";
    if (value[0] == QChar(UNICODE_GBP)) return "GBP";
    if (value[0] == QChar(UNICODE_JPY)) return "JPY";
    QRegExp symbolRegEx("^([^a-zA-Z0-9\\-_\\s]+)");
    if (symbolRegEx.indexIn(value) >= 0) return symbolRegEx.cap(1);
    return QString();
}

// Processes a cell within a sheet.
void ExcelImport::Private::processCellForBody(Cell* cell, KoXmlWriter* xmlWriter, table_table_row& row)
{
    CellValue cellValue;
    if (cell->isCovered()) {
        table_covered_table_cell c(row.add_table_covered_table_cell());
        processCellAttributesForBody(cell, c, cellValue);
        processCellContentForBody(cell, xmlWriter, c, cellValue);
    } else {
        table_table_cell c(row.add_table_table_cell());
        if (cell->columnSpan() > 1)
            c.set_table_number_columns_spanned(cell->columnSpan());
        if (cell->rowSpan() > 1)
            c.set_table_number_rows_spanned(cell->rowSpan());
        processCellAttributesForBody(cell, c, cellValue);
        processCellContentForBody(cell, xmlWriter, c, cellValue);
    }
}

void ExcelImport::Private::processCellAttributesForBody(Cell* cell, group_table_table_cell_attlist& c, CellValue& cellValue)
{
    Q_ASSERT(cellFormatIndex >= 0 && cellFormatIndex < cellStyles.count());
    c.set_table_style_name(cellStyles[cellFormatIndex]);
    cellFormatIndex++;

    if (cell->columnRepeat() > 1)
        c.set_table_number_columns_repeated(cell->columnRepeat());

    const QString formula = cellFormula(cell);
    if (!formula.isEmpty())
        c.set_table_formula(formula);

    cellValue.value = cell->value();
    const Value& value = cellValue.value;

    if (value.isBoolean()) {
        c.set_office_value_type("boolean");
        c.set_office_boolean_value(value.asBoolean() ? "true" : "false");
    } else if (value.isFloat() || value.isInteger()) {
        const QString valueFormat = cell->format().valueFormat();

        if (isPercentageFormat(valueFormat)) {
            c.set_office_value_type("percentage");
            c.set_office_value(value.asFloat());
        } else if (isDateFormat(valueFormat)) {
            const QString dateValue = convertDate(value.asFloat(), valueFormat);
            c.set_office_value_type("date");
            c.set_office_date_value(dateValue);
        } else if (value.asFloat() < 1.0 && isTimeFormat(valueFormat)) {
            c.set_office_value_type("time");
            c.set_office_time_value(Duration(convertToTime(value.asInteger())));
        } else if (isFractionFormat(valueFormat)) {
            c.set_office_value_type("float");
            c.set_office_value(value.asFloat());
        } else { // fallback is the generic float format
            c.set_office_value_type("float");
            c.set_office_value(value.asFloat());
        }
    } else if (value.isText() || value.isError()) {
        cellValue.str = value.asString();

        cellValue.link = cell->hyperlink();
        if (cellValue.link.isValid) {
            cellValue.linkLocation = cellValue.link.location;
            if(!cellValue.linkLocation.isEmpty()) {
                cellValue.linkName = cellValue.link.displayName.trimmed();
                if(cellValue.linkName.isEmpty())
                    cellValue.linkName = cellValue.str;
                cellValue.str.clear(); // at Excel cells with links don't have additional text content
            }
        }
        if (cellValue.linkLocation.isEmpty() && value.isString()) {
            c.set_office_value_type("string");
            if (!(cell->format().font().subscript() || cell->format().font().superscript()))
                c.set_office_string_value(cellValue.str);
        }
    }
}

void ExcelImport::Private::processCellText(Cell* cell, group_paragraph_content& content, CellValue& cellValue)
{
    const QString& str = cellValue.str;
    if (cellValue.value.isString()) {
        content.addTextNode(str);
    } else {
        // rich text
        std::map<unsigned, FormatFont> formatRuns = cellValue.value.formatRuns();

        // add sentinel to list of format runs
        formatRuns[str.length()] = cell->format().font();

        unsigned index = 0;
        QString style;
        for (std::map<unsigned, FormatFont>::iterator it = formatRuns.begin(); it != formatRuns.end(); ++it) {
            if (it->first > index) {
                if (!style.isEmpty()) {
                    text_span span(content.add_text_span());
                    span.set_text_style_name(style);
                    span.addTextNode(str.mid(index, it->first - index));
                } else {
                    content.addTextNode(str.mid(index, it->first - index));
                }
            }

            index = it->first;

            if (it->second == cell->format().font())
                style.clear();
            else {
                style = fontStyles.value(it->second);
            }
        }
    }
}

void ExcelImport::Private::processCellContentForBody(Cell* cell,
        KoXmlWriter* xmlWriter, group_table_table_cell_content& c,
        CellValue& cellValue)
{
    if (cellValue.value.isText() || cellValue.value.isError()) {
        text_p p(c.add_text_p());

        if(!cellValue.str.isEmpty()) {
            if (cell->format().font().subscript() || cell->format().font().superscript()) {
                text_span span(p.add_text_span());
                if (cell->format().font().subscript()) {
                    span.set_text_style_name(subScriptStyle);
                } else {
                    span.set_text_style_name(superScriptStyle);
                }
                processCellText(cell, span, cellValue);
            } else {
                processCellText(cell, p, cellValue);
            }
        }

        if (!cellValue.linkName.isEmpty()) {
            text_a a(p.add_text_a(urlFromArg(cellValue.linkLocation)));
            const QString targetFrameName = cellValue.link.targetFrameName;
            if (! targetFrameName.isEmpty())
                a.set_office_target_frame_name(targetFrameName);
            a.addTextNode(cellValue.linkName);
        }
    }

    const QString note = cell->note();
    if (! note.isEmpty()) {
        office_annotation annotation(c.add_office_annotation());
        //dc_creator creator(annotation.add_dc_creator());
        //creator.addTextNode(authorName); //TODO
        text_p p(annotation.add_text_p());
        p.addTextNode(note);
    }


    // handle charts
    foreach(ChartObject *chart, cell->charts()) {
        Sheet* const sheet = cell->sheet();
        if(chart->m_chart->m_impl==0) {
            qCDebug(lcExcelImport) << "Invalid chart to be created, no implementation.";
            continue;
        }

        KoOdfChartWriter *c = new KoOdfChartWriter(chart->m_chart);
        c->m_href = QString("Chart%1").arg(this->charts.count()+1);
        c->m_endCellAddress = encodeAddress(sheet->name(), chart->m_colR, chart->m_rwB);
        c->m_notifyOnUpdateOfRanges = "Sheet1.D2:Sheet1.F2";

        const unsigned long colL = chart->m_colL;
        const unsigned long dxL = chart->m_dxL;
        //const unsigned long colR = chart->m_colR;
        //const unsigned long dxR = chart->m_dxR;
        //const unsigned long rwB = chart->m_rwB;
        const unsigned long dyT = chart->m_dyT;
        const unsigned long rwT = chart->m_rwT;
        //const unsigned long dyB = chart->m_dyB;

        c->m_x = offset(columnWidth(sheet, colL), dxL, 1024);
        c->m_y = offset(rowHeight(sheet, rwT), dyT, 256);

        if (!chart->m_chart->m_cellRangeAddress.isNull() )
            c->m_cellRangeAddress = encodeAddress(sheet->name(), chart->m_chart->m_cellRangeAddress.left(), chart->m_chart->m_cellRangeAddress.top()) + ":" +
                                    encodeAddress(sheet->name(), chart->m_chart->m_cellRangeAddress.right(), chart->m_chart->m_cellRangeAddress.bottom());

        this->charts << c;

        c->saveIndex(xmlWriter);
    }

    // handle graphics objects
    if (!cell->drawObjects().isEmpty()) {
        xmlWriter->addCompleteElement(cellShapes[cell].data());
    }
}

void ExcelImport::Private::processCharts(KoXmlWriter* manifestWriter)
{
    foreach(KoOdfChartWriter *c, this->charts) {
        c->saveContent(this->storeout, manifestWriter);
    }
}

// Processes style for a cell within a sheet.
void ExcelImport::Private::processCellForStyle(Cell* cell, KoXmlWriter* xmlWriter)
{
    if (!cell) return;
    if (!xmlWriter) return;

    // TODO optimize with hash table
    const Format* format = &cell->format();
    QString styleName = processCellFormat(format, cellFormula(cell));
    cellStyles.append(styleName);

    if (cell->value().isRichText()) {
        std::map<unsigned, FormatFont> formatRuns = cell->value().formatRuns();
        for (std::map<unsigned, FormatFont>::iterator it = formatRuns.begin(); it != formatRuns.end(); ++it) {
            if (fontStyles.contains(it->second)) continue;
            KoGenStyle style(KoGenStyle::TextAutoStyle, "text");
            processFontFormat(it->second, style, true);
            QString styleName = styles->insert(style, "T");
            fontStyles[it->second] = styleName;
        }
    }

    if (format->font().superscript() && superScriptStyle.isEmpty()) {
        KoGenStyle style(KoGenStyle::TextAutoStyle, "text");
        style.addProperty("style:text-position", "super", KoGenStyle::TextType);
        superScriptStyle = styles->insert(style, "T");
    }
    if (format->font().subscript() && subScriptStyle.isEmpty()) {
        KoGenStyle style(KoGenStyle::TextAutoStyle, "text");
        style.addProperty("style:text-position", "sub", KoGenStyle::TextType);
        subScriptStyle = styles->insert(style, "T");
    }

    QList<OfficeArtObject*> objects = cell->drawObjects();
    if (!objects.empty()) {
        ODrawClient client = ODrawClient(cell->sheet());
        ODrawToOdf odraw( client);
        QBuffer b;
        KoXmlWriter xml(&b);
        Writer writer(xml, *styles, false);
        foreach (OfficeArtObject* o, objects) {
            client.setShapeText(o->text());
            odraw.processDrawingObject(o->object(), writer);
        }
        cellShapes[cell] = b.data();
        //qCDebug(lcExcelImport) << cell->columnLabel() << cell->row() << b.data();
    }
}


// Processes styles for a cell within a sheet.
QString ExcelImport::Private::processCellFormat(const Format* format, const QString& formula)
{
    CellFormatKey key(format, formula);
    QString& styleName = cellFormatCache[key];
    if (styleName.isEmpty()) {
        // handle data format, e.g. number style
        QString refName;
        if (!key.isGeneral) {
            refName = processValueFormat(format->valueFormat());
        } else {
            if (key.decimalCount >= 0) {
                KoGenStyle style(KoGenStyle::NumericNumberStyle);
                QBuffer buffer;
                buffer.open(QIODevice::WriteOnly);
                KoXmlWriter xmlWriter(&buffer);    // TODO pass indentation level
                number_number number(&xmlWriter);
                number.set_number_decimal_places(key.decimalCount);
                number.end();
                QString elementContents = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());
                style.addChildElement("number", elementContents);
                refName = styles->insert(style, "N");
            }
        }

        KoGenStyle style(KoGenStyle::TableCellAutoStyle, "table-cell");
        // now the real table-cell
        if (!refName.isEmpty())
            style.addAttribute("style:data-style-name", refName);

        processFormat(format, style);
        styleName = styles->insert(style, "ce");
    }
    return styleName;
}

// Processes styles for a row within a sheet.
QString ExcelImport::Private::processRowFormat(Format* format, const QString& breakBefore, int rowRepeat, double rowHeight)
{
    QString refName;
    QString valueFormat = format->valueFormat();
    if (valueFormat != QString("General"))
        refName = processValueFormat(valueFormat);

    KoGenStyle style(KoGenStyle::TableRowAutoStyle, "table-row");
    // now the real table-cell
    if (!refName.isEmpty())
        style.addAttribute("style:data-style-name", refName);
    // set break-before
    if(!breakBefore.isEmpty())
        style.addProperty("fo:break-before", breakBefore);
    // set how often the row should be repeated
    if (rowRepeat > 1)
        style.addAttribute("table:number-rows-repeated", rowRepeat);
    // set the height of the row
    if (rowHeight >= 0)
        style.addPropertyPt("style:row-height", rowHeight);

    processFormat(format, style);
    QString styleName = styles->insert(style, "ro");
    return styleName;
}

QString convertColor(const QColor& color)
{
    char buf[8];
    sprintf(buf, "#%02x%02x%02x", color.red(), color.green(), color.blue());
    return QString(buf);
}

void convertBorder(const QString& which, const QString& lineWidthProperty, const Pen& pen, KoGenStyle& style)
{
    if (pen.style == Pen::NoLine || pen.width == 0) {
        //style.addProperty(which, "none");
    } else {
        QString result;
        if (pen.style == Pen::DoubleLine) {
            result += QString::number(pen.width * 3);
        } else {
            result = QString::number(pen.width);
        }
        result += "pt ";

        switch (pen.style) {
        case Pen::SolidLine: result += "solid "; break;
        case Pen::DashLine: result += "dashed "; break;
        case Pen::DotLine: result += "dotted "; break;
        case Pen::DashDotLine: result += "dot-dash "; break;
        case Pen::DashDotDotLine: result += "dot-dot-dash "; break;
        case Pen::DoubleLine: result += "double "; break;
        }

        result += convertColor(pen.color);

        style.addProperty(which, result);
        if (pen.style == Pen::DoubleLine) {
            result = QString::number(pen.width);
            result = result + "pt " + result + "pt " + result + "pt";
            style.addProperty(lineWidthProperty, result);
        }
    }
}

void ExcelImport::Private::processFontFormat(const FormatFont& font, KoGenStyle& style, bool allProps)
{
    if (font.isNull()) return;

    if (font.bold()) {
        style.addProperty("fo:font-weight", "bold", KoGenStyle::TextType);
    } else if (allProps) {
        style.addProperty("fo:font-weight", "normal", KoGenStyle::TextType);
    }

    if (font.italic()) {
        style.addProperty("fo:font-style", "italic", KoGenStyle::TextType);
    } else if (allProps) {
        style.addProperty("fo:font-style", "normal", KoGenStyle::TextType);
    }

    if (font.underline()) {
        style.addProperty("style:text-underline-type", "single", KoGenStyle::TextType);
        style.addProperty("style:text-underline-style", "solid", KoGenStyle::TextType);
        style.addProperty("style:text-underline-width", "auto", KoGenStyle::TextType);
        style.addProperty("style:text-underline-color", "font-color", KoGenStyle::TextType);
    } else if (allProps) {
        style.addProperty("style:text-underline-type", "none", KoGenStyle::TextType);
        style.addProperty("style:text-underline-style", "none", KoGenStyle::TextType);
    }

    if (font.strikeout()) {
        style.addProperty("style:text-line-through-type", "single", KoGenStyle::TextType);
        style.addProperty("style:text-line-through-style", "solid", KoGenStyle::TextType);
    } else {
        style.addProperty("style:text-line-through-type", "none", KoGenStyle::TextType);
        style.addProperty("style:text-line-through-style", "none", KoGenStyle::TextType);
    }

    if (!font.fontFamily().isEmpty())
        style.addProperty("fo:font-family", font.fontFamily(), KoGenStyle::TextType);

    style.addPropertyPt("fo:font-size", font.fontSize(), KoGenStyle::TextType);

    style.addProperty("fo:color", convertColor(font.color()), KoGenStyle::TextType);
}

// Processes a formatting.
void ExcelImport::Private::processFormat(const Format* format, KoGenStyle& style)
{
    if (!format) return;

    FormatFont font = format->font();
    FormatAlignment align = format->alignment();
    FormatBackground back = format->background();
    FormatBorders borders = format->borders();

    processFontFormat(font, style);

    if (!align.isNull()) {
        switch (align.alignY()) {
        case Format::Top:
            style.addProperty("style:vertical-align", "top");
            break;
        case Format::Middle:
            style.addProperty("style:vertical-align", "middle");
            break;
        case Format::Bottom:
            style.addProperty("style:vertical-align", "bottom");
            break;
        case Format::VJustify:
            style.addProperty("style:vertical-align", "top");
            style.addProperty("calligra:vertical-distributed", "distributed");
            break;
        case Format::VDistributed:
            style.addProperty("style:vertical-align", "middle");
            style.addProperty("calligra:vertical-distributed", "distributed");
            break;
        }

        style.addProperty("fo:wrap-option", align.wrap() ? "wrap" : "no-wrap");

        if (align.rotationAngle()) {
            style.addProperty("style:rotation-angle", QString::number(align.rotationAngle()));
        }

        if (align.stackedLetters()) {
            style.addProperty("style:direction", "ttb");
        }

        if (align.shrinkToFit()) {
            style.addProperty("style:shrink-to-fit", "true");
        }
    }

    if (!borders.isNull()) {
        convertBorder("fo:border-left", "fo:border-line-width-left", borders.leftBorder(), style);
        convertBorder("fo:border-right", "fo:border-line-width-right", borders.rightBorder(), style);
        convertBorder("fo:border-top", "fo:border-line-width-top", borders.topBorder(), style);
        convertBorder("fo:border-bottom", "fo:border-line-width-bottom", borders.bottomBorder(), style);
        convertBorder("style:diagonal-tl-br", "style:diagonal-tl-br-widths", borders.topLeftBorder(), style);
        convertBorder("style:diagonal-bl-tr", "style:diagonal-bl-tr-widths", borders.bottomLeftBorder(), style);
    }

    if (!back.isNull() && back.pattern() != FormatBackground::EmptyPattern) {
        KoGenStyle fillStyle = KoGenStyle(KoGenStyle::GraphicAutoStyle, "graphic");

        QColor backColor = back.backgroundColor();
        if (back.pattern() == FormatBackground::SolidPattern)
            backColor = back.foregroundColor();
        const QString bgColor = convertColor(backColor);
        style.addProperty("fo:background-color", bgColor);
        switch(back.pattern()) {
            case FormatBackground::SolidPattern:
                fillStyle.addProperty("draw:fill-color", bgColor);
                fillStyle.addProperty("draw:transparency", "0%");
                fillStyle.addProperty("draw:fill", "solid");
                break;
            case FormatBackground::Dense3Pattern: // 88% gray
                fillStyle.addProperty("draw:fill-color", "#000000");
                fillStyle.addProperty("draw:transparency", "88%");
                fillStyle.addProperty("draw:fill", "solid");
                break;
            case FormatBackground::Dense4Pattern: // 50% gray
                fillStyle.addProperty("draw:fill-color", "#000000");
                fillStyle.addProperty("draw:transparency", "50%");
                fillStyle.addProperty("draw:fill", "solid");
                break;
            case FormatBackground::Dense5Pattern: // 37% gray
                fillStyle.addProperty("draw:fill-color", "#000000");
                fillStyle.addProperty("draw:transparency", "37%");
                fillStyle.addProperty("draw:fill", "solid");
                break;
            case FormatBackground::Dense6Pattern: // 12% gray
                fillStyle.addProperty("draw:fill-color", "#000000");
                fillStyle.addProperty("draw:transparency", "12%");
                fillStyle.addProperty("draw:fill", "solid");
                break;
            case FormatBackground::Dense7Pattern: // 6% gray
                fillStyle.addProperty("draw:fill-color", "#000000");
                fillStyle.addProperty("draw:transparency", "6%");
                fillStyle.addProperty("draw:fill", "solid");
                break;
            case FormatBackground::Dense1Pattern: // diagonal crosshatch
            case FormatBackground::Dense2Pattern: // thick diagonal crosshatch
            case FormatBackground::HorPattern: // Horizonatal lines
            case FormatBackground::VerPattern: // Vertical lines
            case FormatBackground::BDiagPattern: // Left-bottom to right-top diagonal lines
            case FormatBackground::FDiagPattern: // Left-top to right-bottom diagonal lines
            case FormatBackground::CrossPattern: // Horizontal and Vertical lines
            case FormatBackground::DiagCrossPattern: { // Crossing diagonal lines
                fillStyle.addProperty("draw:fill", "hatch");
                KoGenStyle hatchStyle(KoGenStyle::HatchStyle);
                hatchStyle.addAttribute("draw:color", "#000000");
                switch (back.pattern()) {
                case FormatBackground::Dense1Pattern:
                case FormatBackground::HorPattern:
                    hatchStyle.addAttribute("draw:style", "single");
                    hatchStyle.addAttribute("draw:rotation", 0);
                    break;
                case FormatBackground::VerPattern:
                    hatchStyle.addAttribute("draw:style", "single");
                    hatchStyle.addAttribute("draw:rotation", 900);
                    break;
                case FormatBackground::Dense2Pattern:
                case FormatBackground::BDiagPattern:
                    hatchStyle.addAttribute("draw:style", "single");
                    hatchStyle.addAttribute("draw:rotation", 450);
                    break;
                case FormatBackground::FDiagPattern:
                    hatchStyle.addAttribute("draw:style", "single");
                    hatchStyle.addAttribute("draw:rotation", 1350);
                    break;
                case FormatBackground::CrossPattern:
                    hatchStyle.addAttribute("draw:style", "double");
                    hatchStyle.addAttribute("draw:rotation", 0);
                    break;
                case FormatBackground::DiagCrossPattern:
                    hatchStyle.addAttribute("draw:style", "double");
                    hatchStyle.addAttribute("draw:rotation", 450);
                    break;
                default:
                    break;
                }
                fillStyle.addProperty("draw:fill-hatch-name", mainStyles->insert(hatchStyle, "hatch"));
            } break;
            default:
                break;
        }
        style.addProperty("draw:style-name", styles->insert(fillStyle, "gr"));
    }

    if (!align.isNull()) {
        switch (align.alignX()) {
        case Format::Left:
            style.addProperty("fo:text-align", "start", KoGenStyle::ParagraphType); break;
        case Format::Center:
            style.addProperty("fo:text-align", "center", KoGenStyle::ParagraphType); break;
        case Format::Right:
            style.addProperty("fo:text-align", "end", KoGenStyle::ParagraphType); break;
        case Format::Justify:
        case Format::Distributed:
            style.addProperty("fo:text-align", "justify", KoGenStyle::ParagraphType); break;
        }

        if (align.indentLevel() != 0)
            style.addProperty("fo:margin-left", QString::number(align.indentLevel()) + "0pt", KoGenStyle::ParagraphType);
    }
}

// 3.8.31 numFmts
QString ExcelImport::Private::processValueFormat(const QString& valueFormat)
{
    KoGenStyle& style = valueFormatCache[valueFormat];
    if (style.isEmpty()) {
        style = NumberFormatParser::parse( valueFormat, styles );
    }
    if( style.type() == KoGenStyle::ParagraphAutoStyle ) {
        return QString();
    }

    return styles->insert( style, "N" );
}

void ExcelImport::Private::createDefaultColumnStyle( Sheet* sheet ) {
    KoGenStyle style(KoGenStyle::TableColumnAutoStyle, "table-column");

    style.addProperty("fo:break-before", "auto");
    style.addPropertyPt("style:column-width", sheet->defaultColWidth() );

    const QString styleName = styles->insert(style, "co");
    defaultColumnStyles.append( styleName );
}

void ExcelImport::Private::processSheetBackground(Sheet* sheet, KoGenStyle& style)
{
    if( sheet->backgroundImage().isEmpty() )
        return;

    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    KoXmlWriter writer(&buffer);

    //TODO add the manifest entry
    style_background_image bg(&writer);
    bg.set_xlink_href(urlFromArg(sheet->backgroundImage()));
    bg.set_xlink_type("simple");
    bg.set_xlink_show("embed");
    bg.set_xlink_actuate("onLoad");
    bg.end();

    buffer.close();
    style.addChildElement("style:background-image", QString::fromUtf8(buffer.buffer(), buffer.buffer().size()));
    manifestEntries.insert(sheet->backgroundImage(), "image/bmp");
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

void ExcelImport::Private::insertPictureManifest(const QString &fileName)
{
    QString mimeType;
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

#include <excelimporttoods.moc>
