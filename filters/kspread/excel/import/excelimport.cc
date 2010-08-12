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

#include <excelimport.h>
#include <excelimport.moc>

#include <QString>
#include <QDate>
#include <QBuffer>
#include <QFontMetricsF>
#include <QPair>
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

#include <Charting.h>
#include <ChartExport.h>
#include <NumberFormatParser.h>

#include "swinder.h"
#include <iostream>
#include "ODrawClient.h"
typedef KGenericFactory<ExcelImport> ExcelImportFactory;
K_EXPORT_COMPONENT_FACTORY(libexcelimport, ExcelImportFactory("kofficefilters"))

#define UNICODE_EUR 0x20AC
#define UNICODE_GBP 0x00A3
#define UNICODE_JPY 0x00A5

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
    return QString("%1.%2%3").arg(sheetName).arg(columnName(column)).arg(row+1);
}

}

using namespace Swinder;

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

    QList<ChartExport*> charts;

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

    void processWorkbookForBody(KoOdfWriteStore* store, Workbook* workbook, KoXmlWriter* xmlWriter);
    void processWorkbookForStyle(Workbook* workbook, KoXmlWriter* xmlWriter);
    void processSheetForBody(KoOdfWriteStore* store, Sheet* sheet, KoXmlWriter* xmlWriter);
    void processSheetForStyle(Sheet* sheet, KoXmlWriter* xmlWriter);
    void processSheetForHeaderFooter(Sheet* sheet, KoXmlWriter* writer);
    void processHeaderFooterStyle(const QString& text, KoXmlWriter* xmlWriter);
    void processColumnForBody(Sheet* sheet, int columnIndex, KoXmlWriter* xmlWriter, unsigned& outlineLevel);
    void processColumnForStyle(Sheet* sheet, int columnIndex, KoXmlWriter* xmlWriter);
    int processRowForBody(KoOdfWriteStore* store, Sheet* sheet, int rowIndex, KoXmlWriter* xmlWriter, unsigned& outlineLevel);
    int processRowForStyle(Sheet* sheet, int rowIndex, KoXmlWriter* xmlWriter);
    void processCellForBody(KoOdfWriteStore* store, Cell* cell, int rowsRepeat, KoXmlWriter* xmlWriter);
    void processCellForStyle(Cell* cell, KoXmlWriter* xmlWriter);
    QString processCellFormat(Format* format, const QString& formula = QString());
    QString processRowFormat(Format* format, const QString& breakBefore = QString(), int rowRepeat = 1, double rowHeight = -1);
    void processFormat(Format* format, KoGenStyle& style);
    QString processValueFormat(const QString& valueFormat);
    void processFontFormat(const FormatFont& font, KoGenStyle& style, bool allProps = false);
    void processCharts(KoXmlWriter* manifestWriter);

    void createDefaultColumnStyle( Sheet* sheet );
    void processSheetBackground(Sheet* sheet, KoGenStyle& style);
    void addManifestEntries(KoXmlWriter* ManifestWriter);
    void insertPictureManifest(PictureObject* picture);

    QList<QString> defaultColumnStyles;
    int defaultColumnStyleIndex;
    QMap<QString,QString> manifestEntries;
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
    d->outputFile = m_chain->outputFile();

    // create output store
    d->storeout = KoStore::createStore(d->outputFile, KoStore::Write,
                                    "application/vnd.oasis.opendocument.spreadsheet", KoStore::Zip);
    if (!d->storeout || d->storeout->bad()) {
        kWarning() << "Couldn't open the requested file.";
        delete d->workbook;
        delete d->storeout;
        return KoFilter::FileNotFound;
    }

    emit sigProgress(0);

    // Tell KoStore not to touch the file names
    d->storeout->disallowNameExpansion();

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
        kWarning() << "Couldn't open the file 'content.xml'.";
        delete d->workbook;
        delete d->storeout;
        return KoFilter::CreationError;
    }

    // store document styles
    if (!d->createStyles(d->storeout, manifestWriter, d->mainStyles)) {
        kWarning() << "Couldn't open the file 'styles.xml'.";
        delete d->workbook;
        delete d->storeout;
        return KoFilter::CreationError;
    }

    // store meta content
    if (!d->createMeta(&oasisStore)) {
        kWarning() << "Couldn't open the file 'meta.xml'.";
        delete d->workbook;
        delete d->storeout;
        return KoFilter::CreationError;
    }

    // store settings
    if (!d->createSettings(&oasisStore)) {
        kWarning() << "Couldn't open the file 'settings.xml'.";
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
    contentWriter->startElement("office:font-face-decls");
    contentWriter->startElement("style:font-face");
    contentWriter->addAttribute("style:name", "Arial");
    contentWriter->addAttribute("svg:font-family", "Arial");
    contentWriter->endElement(); // style:font-face
    contentWriter->startElement("style:font-face");
    contentWriter->addAttribute("style:name", "Times New Roman");
    contentWriter->addAttribute("svg:font-family", "&apos;Times New Roman&apos;");
    contentWriter->endElement(); // style:font-face
    contentWriter->endElement(); // office:font-face-decls


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
    bodyWriter->startElement("office:body");
    processWorkbookForBody(store, workbook, bodyWriter);
    bodyWriter->endElement();  // office:body

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
    stylesWriter->startElement("office:document-styles");
    stylesWriter->addAttribute("xmlns:office", "urn:oasis:names:tc:opendocument:xmlns:office:1.0");
    stylesWriter->addAttribute("xmlns:style", "urn:oasis:names:tc:opendocument:xmlns:style:1.0");
    stylesWriter->addAttribute("xmlns:text", "urn:oasis:names:tc:opendocument:xmlns:text:1.0");
    stylesWriter->addAttribute("xmlns:table", "urn:oasis:names:tc:opendocument:xmlns:table:1.0");
    stylesWriter->addAttribute("xmlns:draw", "urn:oasis:names:tc:opendocument:xmlns:drawing:1.0");
    stylesWriter->addAttribute("xmlns:fo", "urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0");
    stylesWriter->addAttribute("xmlns:svg", "urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0");
    stylesWriter->addAttribute("xmlns:xlink", "http://www.w3.org/1999/xlink");
    stylesWriter->addAttribute("xmlns:chart", "urn:oasis:names:tc:opendocument:xmlns:chart:1.0");
    stylesWriter->addAttribute("xmlns:dc", "http://purl.org/dc/elements/1.1/");
    stylesWriter->addAttribute("xmlns:meta", "urn:oasis:names:tc:opendocument:xmlns:meta:1.0");
    stylesWriter->addAttribute("xmlns:number", "urn:oasis:names:tc:opendocument:xmlns:datastyle:1.0");
    //stylesWriter->addAttribute("xmlns:dr3d", "urn:oasis:names:tc:opendocument:xmlns:dr3d:1.0");
    stylesWriter->addAttribute("xmlns:math", "http://www.w3.org/1998/Math/MathML");
    stylesWriter->addAttribute("xmlns:of", "urn:oasis:names:tc:opendocument:xmlns:of:1.2");
    stylesWriter->addAttribute("office:version", "1.2");

    mainStyles->saveOdfStyles(KoGenStyles::MasterStyles, stylesWriter);
    mainStyles->saveOdfStyles(KoGenStyles::DocumentStyles, stylesWriter); // office:style
    mainStyles->saveOdfStyles(KoGenStyles::DocumentAutomaticStyles, stylesWriter); // office:automatic-styles

    stylesWriter->endElement();  // office:document-styles
    stylesWriter->endDocument();

    delete stylesWriter;
    return store->close();
}

// Writes meta-informations into the meta.xml
bool ExcelImport::Private::createMeta(KoOdfWriteStore* store)
{
    if (!store->store()->open("meta.xml"))
        return false;

    KoStoreDevice dev(store->store());
    KoXmlWriter* metaWriter = new KoXmlWriter(&dev);
    metaWriter->startDocument("office:document-meta");
    metaWriter->startElement("office:document-meta");
    metaWriter->addAttribute("xmlns:office", "urn:oasis:names:tc:opendocument:xmlns:office:1.0");
    metaWriter->addAttribute("xmlns:xlink", "http://www.w3.org/1999/xlink");
    metaWriter->addAttribute("xmlns:dc", "http://purl.org/dc/elements/1.1/");
    metaWriter->addAttribute("xmlns:meta", "urn:oasis:names:tc:opendocument:xmlns:meta:1.0");
    metaWriter->startElement("office:meta");

    if (workbook->hasProperty(Workbook::PIDSI_TITLE)) {
        metaWriter->startElement("dc:title");
        metaWriter->addTextNode(workbook->property(Workbook::PIDSI_TITLE).toString());
        metaWriter->endElement();
    }
    if (workbook->hasProperty(Workbook::PIDSI_SUBJECT)) {
        metaWriter->startElement("dc:subject", false);
        metaWriter->addTextNode(workbook->property(Workbook::PIDSI_SUBJECT).toString());
        metaWriter->endElement();
    }
    if (workbook->hasProperty(Workbook::PIDSI_AUTHOR)) {
        metaWriter->startElement("dc:creator", false);
        metaWriter->addTextNode(workbook->property(Workbook::PIDSI_AUTHOR).toString());
        metaWriter->endElement();
    }
    if (workbook->hasProperty(Workbook::PIDSI_KEYWORDS)) {
        metaWriter->startElement("meta:keyword", false);
        metaWriter->addTextNode(workbook->property(Workbook::PIDSI_KEYWORDS).toString());
        metaWriter->endElement();
    }
    if (workbook->hasProperty(Workbook::PIDSI_COMMENTS)) {
        metaWriter->startElement("meta:comments", false);
        metaWriter->addTextNode(workbook->property(Workbook::PIDSI_COMMENTS).toString());
        metaWriter->endElement();
    }
    if (workbook->hasProperty(Workbook::PIDSI_REVNUMBER)) {
        metaWriter->startElement("meta:editing-cycles", false);
        metaWriter->addTextNode(workbook->property(Workbook::PIDSI_REVNUMBER).toString());
        metaWriter->endElement();
    }
    if (workbook->hasProperty(Workbook::PIDSI_LASTPRINTED_DTM)) {
        metaWriter->startElement("dc:print-date", false);
        metaWriter->addTextNode(workbook->property(Workbook::PIDSI_LASTPRINTED_DTM).toString());
        metaWriter->endElement();
    }
    if (workbook->hasProperty(Workbook::PIDSI_CREATE_DTM)) {
        metaWriter->startElement("meta:creation-date", false);
        metaWriter->addTextNode(workbook->property(Workbook::PIDSI_CREATE_DTM).toString());
        metaWriter->endElement();
    }
    if (workbook->hasProperty(Workbook::PIDSI_LASTSAVED_DTM)) {
        metaWriter->startElement("dc:date", false);
        metaWriter->addTextNode(workbook->property(Workbook::PIDSI_LASTSAVED_DTM).toString());
        metaWriter->endElement();
    }

    //if( workbook->hasProperty( Workbook::PIDSI_TEMPLATE )  ) metaWriter->addAttribute( "dc:", workbook->property( Workbook::PIDSI_TEMPLATE ).toString() );
    //if( workbook->hasProperty( Workbook::PIDSI_LASTAUTHOR )  ) metaWriter->addAttribute( "dc:", workbook->property( Workbook::PIDSI_LASTAUTHOR ).toString() );
    //if( workbook->hasProperty( Workbook::PIDSI_EDITTIME )  ) metaWriter->addAttribute( "dc:date", workbook->property( Workbook::PIDSI_EDITTIME ).toString() );

    metaWriter->endElement(); // office:meta
    metaWriter->endElement(); // office:document-meta
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
    settingsWriter->startElement("office:settings");
    settingsWriter->startElement("config:config-item-set");
    settingsWriter->addAttribute("config:name", "view-settings");

    // units...

    // settings
    settingsWriter->startElement("config:config-item-map-indexed");
    settingsWriter->addAttribute("config:name", "Views");
    settingsWriter->startElement("config:config-item-map-entry");
    settingsWriter->addConfigItem("ViewId", QString::fromLatin1("View1"));
    if(Sheet *sheet = workbook->sheet(workbook->activeTab()))
        settingsWriter->addConfigItem("ActiveTable", sheet->name());

    settingsWriter->startElement("config:config-item-map-named");
    settingsWriter->addAttribute("config:name", "Tables");
    for(uint i = 0; i < workbook->sheetCount(); ++i) {
        Sheet* sheet = workbook->sheet(i);
        settingsWriter->startElement("config:config-item-map-entry");
        settingsWriter->addAttribute("config:name", sheet->name());
        QPoint point = sheet->firstVisibleCell();
        settingsWriter->addConfigItem("CursorPositionX", point.x());
        settingsWriter->addConfigItem("CursorPositionY", point.y());
        //TODO how should we replace these settings?
//         settingsWriter->addConfigItem("xOffset", columnWidth(sheet,point.x()));
//         settingsWriter->addConfigItem("yOffset", rowHeight(sheet,point.y()));
        settingsWriter->addConfigItem("ShowZeroValues", sheet->showZeroValues());
        settingsWriter->addConfigItem("ShowGrid", sheet->showGrid());
        settingsWriter->addConfigItem("FirstLetterUpper", false);
        settingsWriter->addConfigItem("ShowFormulaIndicator", false);
        settingsWriter->addConfigItem("ShowCommentIndicator", true);
        settingsWriter->addConfigItem("ShowPageBorders", sheet->isPageBreakViewEnabled()); // best match kspread provides
        settingsWriter->addConfigItem("lcmode", false);
        settingsWriter->addConfigItem("autoCalc", sheet->autoCalc());
        settingsWriter->addConfigItem("ShowColumnNumber", false);
        settingsWriter->endElement();
    }
    settingsWriter->endElement(); // config:config-item-map-named

    settingsWriter->endElement(); // config:config-item-map-entry
    settingsWriter->endElement(); // config:config-item-map-indexed
    settingsWriter->endElement(); // config:config-item-set

    settingsWriter->endElement(); // office:settings
    settingsWriter->endElement(); // Root:element
    settingsWriter->endDocument();
    delete settingsWriter;
    return store->store()->close();
}

// Processes the workbook content. The workbook is the top-level element for content.
void ExcelImport::Private::processWorkbookForBody(KoOdfWriteStore* store, Workbook* workbook, KoXmlWriter* xmlWriter)
{
    if (!workbook) return;
    if (!xmlWriter) return;

    xmlWriter->startElement("office:spreadsheet");

    // count the number of rows in total to provide a good progress value
    rowsCountTotal = rowsCountDone = 0;
    for (unsigned i = 0; i < workbook->sheetCount(); i++) {
        Sheet* sheet = workbook->sheet(i);
        rowsCountTotal += qMin(maximalRowCount, sheet->maxRow()) * 2; // double cause we will count them 2 times, once for styles and once for content
    }

    // now start the whole work
    for (unsigned i = 0; i < workbook->sheetCount(); i++) {
        Sheet* sheet = workbook->sheet(i);
        processSheetForBody(store, sheet, xmlWriter);
    }

    std::map<std::pair<unsigned, QString>, QString> &namedAreas = workbook->namedAreas();
    if(namedAreas.size() > 0) {
        xmlWriter->startElement("table:named-expressions");
        for(std::map<std::pair<unsigned, QString>, QString>::iterator it = namedAreas.begin(); it != namedAreas.end(); it++) {
            xmlWriter->startElement("table:named-range");
            xmlWriter->addAttribute("table:name", it->first.second ); // e.g. "My Named Range"
            QString range = it->second;
            if(range.startsWith('[') && range.endsWith(']'))
                range = range.mid(1, range.length() - 2);
            xmlWriter->addAttribute("table:cell-range-address", range); // e.g. "$Sheet1.$B$2:.$B$3"
            xmlWriter->endElement();//[Sheet1.$B$2:$B$3]
        }
        xmlWriter->endElement();
    }

    bool openedDBRanges = false;
    int rangeId = 1;
    for (unsigned i = 0; i < workbook->sheetCount(); i++) {
        QList<QRect> filters = workbook->filterRanges(i);
        QString sheetName = workbook->sheet(i)->name();
        if (filters.size()) {
            if (!openedDBRanges) xmlWriter->startElement("table:database-ranges");
            openedDBRanges = true;

            foreach (const QRect& filter, filters) {
                QString sRange(encodeAddress(sheetName, filter.left(), filter.top()));
                sRange.append(":");
                sRange.append(encodeAddress(sheetName, filter.right(), workbook->sheet(i)->maxRow()));
                xmlWriter->startElement("table:database-range");
                xmlWriter->addAttribute("table:name", QString("excel-database-%1").arg(rangeId++));
                xmlWriter->addAttribute("table:display-filter-buttons", "true");
                xmlWriter->addAttribute("table:target-range-address", sRange);
                xmlWriter->endElement(); // table:database-range
            }
        }
    }
    if (openedDBRanges) xmlWriter->endElement(); // table:database-ranges

    xmlWriter->endElement();  // office:spreadsheet
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
    writer.startElement("style:header-style");
    writer.startElement("style:header-footer-properties");
    writer.addAttribute("fo:min-height", "20pt");
    writer.addAttribute("fo:margin-left", "0pt");
    writer.addAttribute("fo:margin-right", "0pt");
    writer.addAttribute("fo:margin-bottom", "10pt");
    writer.endElement();
    writer.endElement();

    writer.startElement("style:footer-style");
    writer.startElement("style:header-footer-properties");
    writer.addAttribute("fo:min-height", "20pt");
    writer.addAttribute("fo:margin-left", "0pt");
    writer.addAttribute("fo:margin-right", "0pt");
    writer.addAttribute("fo:margin-top", "10pt");
    writer.endElement();
    writer.endElement();
    QString pageLyt = QString::fromUtf8(buf.buffer(), buf.buffer().size());
    buf.close();
    buf.setData("", 0);

    pageLayoutStyle.addProperty("1header-footer-style", pageLyt, KoGenStyle::StyleChildElement);
    pageLayoutStyleName = mainStyles->insert(pageLayoutStyle, pageLayoutStyleName, KoGenStyles::DontAddNumberToName);

    for (unsigned i = 0; i < workbook->sheetCount(); i++) {
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
void ExcelImport::Private::processSheetForBody(KoOdfWriteStore* store, Sheet* sheet, KoXmlWriter* xmlWriter)
{
    if (!sheet) return;
    if (!xmlWriter) return;

    xmlWriter->startElement("table:table");

    xmlWriter->addAttribute("table:name", sheet->name());
    xmlWriter->addAttribute("table:print", "false");
    xmlWriter->addAttribute("table:style-name", sheetStyles[sheetFormatIndex]);
    ++sheetFormatIndex;

    if(sheet->password() != 0) {
        //TODO
       //xmlWriter->addAttribute("table:protected", "true");
       //xmlWriter->addAttribute("table:protection-key", uint(sheet->password()));
    }

    const unsigned columnCount = qMin(maximalColumnCount, sheet->maxColumn());
    unsigned outlineLevel = 0;
    for (unsigned i = 0; i <= columnCount; ++i) {
        processColumnForBody(sheet, i, xmlWriter, outlineLevel);
    }
    while (outlineLevel > 0) {
        xmlWriter->endElement(); // table:table-column-group
        outlineLevel--;
    }

    // in odf default-cell-style's only apply to cells/rows/columns that are present in the file while in Excel
    // row/column styles should apply to all cells in that row/column. So, try to fake that behavior by writting
    // a number-columns-repeated to apply the styles/formattings to "all" columns.
    if (columnCount < maximalColumnCount-1) {
        xmlWriter->startElement("table:table-column");
        xmlWriter->addAttribute("table:style-name", defaultColumnStyles[defaultColumnStyleIndex]);
        xmlWriter->addAttribute("table:number-columns-repeated", maximalColumnCount - 1 - columnCount);
        xmlWriter->endElement();
    }

    // add rows
    const unsigned rowCount = qMin(maximalRowCount, sheet->maxRow());
    for (unsigned i = 0; i <= rowCount;) {
        i += processRowForBody(store, sheet, i, xmlWriter, outlineLevel);
    }
    while (outlineLevel > 0) {
        xmlWriter->endElement(); // table:table-row-group
        outlineLevel--;
    }

    // same we did above with columns is also needed for rows.
    if(rowCount < maximalRowCount-1) {
        xmlWriter->startElement("table:table-row");
        xmlWriter->addAttribute("table:number-rows-repeated", maximalRowCount - 1 - rowCount);
        xmlWriter->endElement();
    }

    xmlWriter->endElement();  // table:table
    ++defaultColumnStyleIndex;
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
}

// Processes headers and footers for a sheet.
void ExcelImport::Private::processSheetForHeaderFooter(Sheet* sheet, KoXmlWriter* xmlWriter)
{
    if (!sheet) return;
    if (!xmlWriter) return;

    xmlWriter->startElement("style:header");
    if (!sheet->leftHeader().isEmpty()) {
        xmlWriter->startElement("style:region-left");
        xmlWriter->startElement("text:p");
        processHeaderFooterStyle(sheet->leftHeader(), xmlWriter);
        xmlWriter->endElement();
        xmlWriter->endElement();
    }
    if (!sheet->centerHeader().isEmpty()) {
        xmlWriter->startElement("style:region-center");
        xmlWriter->startElement("text:p");
        processHeaderFooterStyle(sheet->centerHeader(), xmlWriter);
        xmlWriter->endElement();
        xmlWriter->endElement();
    }
    if (!sheet->rightHeader().isEmpty()) {
        xmlWriter->startElement("style:region-right");
        xmlWriter->startElement("text:p");
        processHeaderFooterStyle(sheet->rightHeader(), xmlWriter);
        xmlWriter->endElement();
        xmlWriter->endElement();
    }
    xmlWriter->endElement();

    xmlWriter->startElement("style:footer");
    if (!sheet->leftFooter().isEmpty()) {
        xmlWriter->startElement("style:region-left");
        xmlWriter->startElement("text:p");
        processHeaderFooterStyle(sheet->leftFooter(), xmlWriter);
        xmlWriter->endElement();
        xmlWriter->endElement();
    }
    if (!sheet->centerFooter().isEmpty()) {
        xmlWriter->startElement("style:region-center");
        xmlWriter->startElement("text:p");
        processHeaderFooterStyle(sheet->centerFooter(), xmlWriter);
        xmlWriter->endElement();
        xmlWriter->endElement();
    }
    if (!sheet->rightFooter().isEmpty()) {
        xmlWriter->startElement("style:region-right");
        xmlWriter->startElement("text:p");
        processHeaderFooterStyle(sheet->rightFooter(), xmlWriter);
        xmlWriter->endElement();
        xmlWriter->endElement();
    }
    xmlWriter->endElement();
}

// Processes the styles of a headers and footers for a sheet.
void ExcelImport::Private::processHeaderFooterStyle(const QString& text, KoXmlWriter* xmlWriter)
{
    QString content;
    bool skipUnsupported = false;
    int lastPos;
    int pos = text.indexOf('&');
    int len = text.length();
    if ((pos < 0) && (text.length() > 0))   // If ther is no &
        xmlWriter->addTextNode(text);
    else if (pos > 0) // Some text and '&'
        xmlWriter->addTextNode(text.mid(0,  pos - 1));

    while (pos >= 0) {
        switch (text[pos + 1].unicode()) {
        case 'D':
            xmlWriter->startElement("text:date");
            xmlWriter->addTextNode(QDate::currentDate().toString("DD/MM/YYYY"));
            xmlWriter->endElement();
            break;
        case 'T':
            xmlWriter->startElement("text:time");
            xmlWriter->addTextNode(QTime::currentTime().toString("HH:MM:SS"));
            xmlWriter->endElement();
            break;
        case 'P':
            xmlWriter->startElement("text:page-number");
            xmlWriter->addTextNode("1");
            xmlWriter->endElement();
            break;
        case 'N':
            xmlWriter->startElement("text:page-count");
            xmlWriter->addTextNode("999");
            xmlWriter->endElement();
            break;
        case 'F':
            xmlWriter->startElement("text:title");
            xmlWriter->addTextNode("???");
            xmlWriter->endElement();
            break;
        case 'A':
            xmlWriter->startElement("text:sheet-name");
            xmlWriter->addTextNode("???");
            xmlWriter->endElement();
            break;
        case '\"':
        default:
            skipUnsupported = true;
            break;
        }
        lastPos = pos;
        pos = text.indexOf('&', lastPos + 1);
        if (!skipUnsupported && (pos > (lastPos + 1)))
            xmlWriter->addTextNode(text.mid(lastPos + 2, (pos - lastPos - 2)));
        else if (!skipUnsupported && (pos < 0))  //Remaining text
            xmlWriter->addTextNode(text.mid(lastPos + 2, len - (lastPos + 2)));
        else
            skipUnsupported = false;
    }
}

// Processes a column in a sheet.
void ExcelImport::Private::processColumnForBody(Sheet* sheet, int columnIndex, KoXmlWriter* xmlWriter, unsigned& outlineLevel)
{
    Column* column = sheet->column(columnIndex, false);

    if (!xmlWriter) return;

    unsigned newOutlineLevel = column ? column->outlineLevel() : 0;
    while (newOutlineLevel > outlineLevel) {
        xmlWriter->startElement("table:table-column-group");
        outlineLevel++;
        if (outlineLevel == newOutlineLevel && column->collapsed())
            xmlWriter->addAttribute("table:display", "false");
    }
    while (newOutlineLevel < outlineLevel) {
        xmlWriter->endElement(); // table:table-column-group
        outlineLevel--;
    }

    if (!column) {
        xmlWriter->startElement("table:table-column");
        Q_ASSERT(defaultColumnStyleIndex < defaultColumnStyles.count());
        xmlWriter->addAttribute("table:style-name", defaultColumnStyles[defaultColumnStyleIndex] );
        xmlWriter->endElement();
        return;
    }
    Q_ASSERT(columnFormatIndex < colStyles.count());
    Q_ASSERT(columnFormatIndex < colCellStyles.count());
    const QString styleName = colStyles[columnFormatIndex];
    const QString defaultStyleName = colCellStyles[columnFormatIndex];
    columnFormatIndex++;

    xmlWriter->startElement("table:table-column");
    xmlWriter->addAttribute("table:default-cell-style-name", defaultStyleName);
    xmlWriter->addAttribute("table:visibility", column->visible() ? "visible" : "collapse");
    //xmlWriter->addAttribute("table:number-columns-repeated", );
    xmlWriter->addAttribute("table:style-name", styleName);
    xmlWriter->endElement();  // table:table-column
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

    Format format = column->format();
    QString cellStyleName = processCellFormat(&format);
    colCellStyles.append(cellStyleName);
}

// Processes a row in a sheet.
int ExcelImport::Private::processRowForBody(KoOdfWriteStore* store, Sheet* sheet, int rowIndex, KoXmlWriter* xmlWriter, unsigned& outlineLevel)
{
    int repeat = 1;

    if (!xmlWriter) return repeat;
    Row *row = sheet->row(rowIndex, false);

    unsigned newOutlineLevel = row ? row->outlineLevel() : 0;
    while (newOutlineLevel > outlineLevel) {
        xmlWriter->startElement("table:table-row-group");
        outlineLevel++;
        if (outlineLevel == newOutlineLevel && row->collapsed())
            xmlWriter->addAttribute("table:display", "false");
    }
    while (newOutlineLevel < outlineLevel) {
        xmlWriter->endElement(); // table:table-row-group
        outlineLevel--;
    }


    if (!row) {
        xmlWriter->startElement("table:table-row");
        xmlWriter->endElement();
        return repeat;
    }
    if (!row->sheet()) return repeat;

    const QString styleName = rowStyles[rowFormatIndex];
    rowFormatIndex++;

    repeat = rowsRepeated(row, rowIndex);

    xmlWriter->startElement("table:table-row");
    xmlWriter->addAttribute("table:visibility", row->visible() ? "visible" : "collapse");
    xmlWriter->addAttribute("table:style-name", styleName);

    if(repeat > 1)
        xmlWriter->addAttribute("table:number-rows-repeated", repeat);

    // find the column of the rightmost cell (if any)
    const int lastCol = row->sheet()->maxCellsInRow(rowIndex);
    int i = 0;
    while(i <= lastCol) {
        Cell* cell = row->sheet()->cell(i, row->index(), false);
        if (cell) {
            processCellForBody(store, cell, repeat, xmlWriter);
            i += cell->columnRepeat();
        } else { // empty cell
            xmlWriter->startElement("table:table-cell");
            xmlWriter->endElement();
            ++i;
        }
    }

    xmlWriter->endElement();  // table:table-row
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

static bool isPercentageFormat(const QString& valueFormat)
{
    if (valueFormat.isEmpty()) return false;
    if (valueFormat.length() < 1) return false;
    return valueFormat[valueFormat.length()-1] == QChar('%');
}

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
        else kDebug() << "Probably unhandled condition=" << text[1] << "in text=" << text;
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

// extract and return locale and remove locale from time string.
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

// Checks if the as argument passed formatstring defines a date-format or not.
static bool isDateFormat(const Value &value, const QString& valueFormat)
{
    if (value.type() != Value::Float)
        return false;

    const KoGenStyle style = NumberFormatParser::parse( valueFormat );
    return style.type() == KoGenStyle::NumericDateStyle;
}

static bool isTimeFormat(const Value &value, const QString& valueFormat)
{
    if (value.type() != Value::Float)
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
    return (ex.indexIn(vf) >= 0) && value.asFloat() < 1.0;
}

static bool isFractionFormat(const QString& valueFormat)
{
    QRegExp ex("^#[?]+/[0-9?]+$");
    QString vf = removeEscaped(valueFormat);
    return ex.indexIn(vf) >= 0;
}

static QString convertCurrency(double currency, const QString& valueFormat)
{
    Q_UNUSED(valueFormat);
    return QString::number(currency, 'g', 15);
}

static QString convertDate(double serialNo, const QString& valueFormat)
{
    QString vf = valueFormat;
    QString locale = extractLocale(vf);
    Q_UNUSED(locale);   //TODO http://msdn.microsoft.com/en-us/goglobal/bb964664.aspx
    Q_UNUSED(vf);   //TODO

    // reference is midnight 30 Dec 1899
    QDateTime dt(QDate(1899, 12, 30));
    dt = dt.addMSecs((qint64)(serialNo * 86400 * 1000)); // TODO: we probably need double precision here

    //TODO atm we always return a datetime. This works great (time ignored if only date was defined) with KSpread but probably not with other customers...
    //return dd.toString("yyyy-MM-dd");
    return dt.toString("yyyy-MM-ddThh:mm:ss");
}

static QString convertTime(double serialNo, const QString& valueFormat)
{
    QString vf = valueFormat;
    QString locale = extractLocale(vf);
    Q_UNUSED(locale);   //TODO http://msdn.microsoft.com/en-us/goglobal/bb964664.aspx
    Q_UNUSED(vf);   //TODO

    // reference is midnight 30 Dec 1899
    QTime tt;
    tt = tt.addMSecs(qRound((serialNo - (int)serialNo) * 86400 * 1000));
    qDebug() << tt;
    return tt.toString("'PT'hh'H'mm'M'ss'S'");
}

static QString convertFraction(double serialNo, const QString& valueFormat)
{
    Q_UNUSED(valueFormat);
    return QString::number(serialNo, 'g', 15);
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
void ExcelImport::Private::processCellForBody(KoOdfWriteStore* store, Cell* cell, int rowsRepeat, KoXmlWriter* xmlWriter)
{
    if (!cell) return;
    if (!xmlWriter) return;

    if (cell->isCovered())
        xmlWriter->startElement("table:covered-table-cell");
    else
        xmlWriter->startElement("table:table-cell");

    Q_ASSERT(cellFormatIndex >= 0 && cellFormatIndex < cellStyles.count());
    xmlWriter->addAttribute("table:style-name", cellStyles[cellFormatIndex]);
    cellFormatIndex++;

    if (cell->columnSpan() > 1)
        xmlWriter->addAttribute("table:number-columns-spanned", cell->columnSpan());
    if (cell->rowSpan() > 1)
        xmlWriter->addAttribute("table:number-rows-spanned", cell->rowSpan());
    if (cell->columnRepeat() > 1)
        xmlWriter->addAttribute("table:number-columns-repeated", cell->columnRepeat());

    const QString formula = cellFormula(cell);
    if (!formula.isEmpty())
        xmlWriter->addAttribute("table:formula", formula);

    Value value = cell->value();

    if (value.isBoolean()) {
        xmlWriter->addAttribute("office:value-type", "boolean");
        xmlWriter->addAttribute("office:boolean-value", value.asBoolean() ? "true" : "false");
    } else if (value.isFloat() || value.isInteger()) {
        const QString valueFormat = cell->format().valueFormat();

        if (isPercentageFormat(valueFormat)) {
            xmlWriter->addAttribute("office:value-type", "percentage");
            xmlWriter->addAttribute("office:value", QString::number(value.asFloat(), 'g', 15));
        } else if (isDateFormat(value, valueFormat)) {
            const QString dateValue = convertDate(value.asFloat(), valueFormat);
            xmlWriter->addAttribute("office:value-type", "date");
            xmlWriter->addAttribute("office:date-value", dateValue);
        } else if (isTimeFormat(value, valueFormat)) {
            const QString timeValue = convertTime(value.asFloat(), valueFormat);
            xmlWriter->addAttribute("office:value-type", "time");
            xmlWriter->addAttribute("office:time-value", timeValue);
        } else if (isFractionFormat(valueFormat)) {
            const QString fractionValue = convertFraction(value.asFloat(), valueFormat);
            xmlWriter->addAttribute("office:value-type", "float");
            xmlWriter->addAttribute("office:value", fractionValue);
        } else { // fallback is the generic float format
            xmlWriter->addAttribute("office:value-type", "float");
            xmlWriter->addAttribute("office:value", QString::number(value.asFloat(), 'g', 15));
        }
    } else if (value.isText() || value.isError()) {
        QString str = value.asString();
        QString linkName, linkLocation;

        Hyperlink link = cell->hyperlink();
        if (link.isValid) {
            linkLocation = link.location;
            if(!linkLocation.isEmpty()) {
                linkName = link.displayName.trimmed();
                if(linkName.isEmpty())
                    linkName = str;
                str.clear(); // at Excel cells with links don't have additional text content
            }
        }
        if (linkLocation.isEmpty() && value.isString()) {
            xmlWriter->addAttribute("office:value-type", "string");
             if (!(cell->format().font().subscript() || cell->format().font().superscript()))
                 xmlWriter->addAttribute("office:string-value", str);
        }

        xmlWriter->startElement("text:p", false);

        if(!str.isEmpty()) {
            if (cell->format().font().subscript() || cell->format().font().superscript()) {
                xmlWriter->startElement("text:span");
                if (cell->format().font().subscript())
                    xmlWriter->addAttribute("text:style-name", subScriptStyle);
                else
                    xmlWriter->addAttribute("text:style-name", superScriptStyle);
            }

            if (value.isString()) {
                xmlWriter->addTextNode(str);
            } else {
                // rich text
                std::map<unsigned, FormatFont> formatRuns = value.formatRuns();

                // add sentinel to list of format runs
                formatRuns[str.length()] = cell->format().font();

                unsigned index = 0;
                QString style;
                for (std::map<unsigned, FormatFont>::iterator it = formatRuns.begin(); it != formatRuns.end(); ++it) {
                    if (!style.isEmpty() && it->first > index) {
                        xmlWriter->startElement("text:span");
                        xmlWriter->addAttribute("text:style-name", style);
                    }
                    if (it->first > index)
                        xmlWriter->addTextNode(str.mid(index, it->first - index));
                    if (!style.isEmpty() && it->first > index) {
                        xmlWriter->endElement(); // text:span
                    }

                    index = it->first;

                    if (it->second == cell->format().font())
                        style = "";
                    else {
                        style = fontStyles.value(it->second);
                    }
                }
            }

            if (cell->format().font().subscript() || cell->format().font().superscript())
                xmlWriter->endElement(); // text:span
        }

        if (!linkName.isEmpty()) {
            xmlWriter->startElement("text:a");
            xmlWriter->addAttribute("xlink:href", linkLocation);
            const QString targetFrameName = link.targetFrameName;
            if (! targetFrameName.isEmpty())
                xmlWriter->addAttribute("office:target-frame-name", targetFrameName);
            xmlWriter->addTextNode(linkName);
            xmlWriter->endElement(); // text:a
        }

        xmlWriter->endElement(); //  text:p
    }

    const QString note = cell->note();
    if (! note.isEmpty()) {
        xmlWriter->startElement("office:annotation");
        //xmlWriter->startElement("dc:creator");
        //xmlWriter->addTextNode(authorName); //TODO
        //xmlWriter->endElement(); // dc:creator
        xmlWriter->startElement("text:p");
        xmlWriter->addTextNode(note);
        xmlWriter->endElement(); // text:p
        xmlWriter->endElement(); // office:annotation
    }

    // handle pictures
    foreach(PictureObject *picture, cell->pictures()) {
        Sheet* const sheet = cell->sheet();
        const unsigned long colL = picture->m_colL;
        const unsigned long dxL = picture->m_dxL;
        const unsigned long colR = picture->m_colR;
        const unsigned long dxR = picture->m_dxR;
        const unsigned long rwB = picture->m_rwB;
        const unsigned long dyT = picture->m_dyT;
        const unsigned long rwT = picture->m_rwT;
        const unsigned long dyB = picture->m_dyB;

        xmlWriter->startElement("draw:frame");
        //xmlWriter->addAttribute("draw:name", "Graphics 1");
        xmlWriter->addAttribute("table:end-cell-address", encodeAddress(sheet->name(), picture->m_colR, picture->m_rwB));
        xmlWriter->addAttributePt("table:end-x", offset(columnWidth(sheet, colR), dxR, 1024));
        xmlWriter->addAttributePt("table:end-y", offset(rowHeight(sheet, rwB), dyB, 256));
        xmlWriter->addAttribute("draw:z-index", "0");
        xmlWriter->addAttributePt("svg:x", offset(columnWidth(sheet, colL), dxL, 1024) );
        xmlWriter->addAttributePt("svg:y", offset(rowHeight(sheet, rwT), dyT, 256));

        xmlWriter->startElement("draw:image");
        xmlWriter->addAttribute("xlink:href", "Pictures/" + picture->fileName());
        xmlWriter->addAttribute("xlink:type", "simple");
        xmlWriter->addAttribute("xlink:show", "embed");
        xmlWriter->addAttribute("xlink:actuate", "onLoad");
        xmlWriter->endElement(); // draw:image
        xmlWriter->endElement(); // draw:frame

        insertPictureManifest(picture);
    }

    // handle charts
    foreach(ChartObject *chart, cell->charts()) {
        Sheet* const sheet = cell->sheet();
        if(chart->m_chart->m_impl==0) {
            kDebug() << "Invalid chart to be created, no implementation.";
            continue;
        }

        ChartExport *c = new ChartExport(chart->m_chart);
        c->m_href = QString("Chart%1").arg(this->charts.count()+1);
        c->m_endCellAddress = encodeAddress(sheet->name(), chart->m_colR, chart->m_rwB);
        c->m_notifyOnUpdateOfRanges = "Sheet1.D2:Sheet1.F2";

        const unsigned long colL = chart->m_colL;
        const unsigned long dxL = chart->m_dxL;
        const unsigned long colR = chart->m_colR;
        const unsigned long dxR = chart->m_dxR;
        const unsigned long rwB = chart->m_rwB;
        const unsigned long dyT = chart->m_dyT;
        const unsigned long rwT = chart->m_rwT;
        const unsigned long dyB = chart->m_dyB;

        c->m_x = offset(columnWidth(sheet, colL), dxL, 1024);
        c->m_y = offset(rowHeight(sheet, rwT), dyT, 256);

        if (!chart->m_chart->m_cellRangeAddress.isNull() )
            c->m_cellRangeAddress = encodeAddress(sheet->name(), chart->m_chart->m_cellRangeAddress.left(), chart->m_chart->m_cellRangeAddress.top()) + ":" +
                                    encodeAddress(sheet->name(), chart->m_chart->m_cellRangeAddress.right(), chart->m_chart->m_cellRangeAddress.bottom());

        this->charts << c;

        c->saveIndex(xmlWriter);
    }

    // handle graphics objects
    QList<MSO::OfficeArtSpgrContainerFileBlock> objects = cell->drawObjects();
    if (!objects.empty()) {
        ODrawClient client = ODrawClient(cell->sheet());
        ODrawToOdf odraw( client);
        Writer writer(*xmlWriter, *styles, false);
        foreach (const MSO::OfficeArtSpgrContainerFileBlock& fb, objects) {
            odraw.processDrawing(fb, writer);
        }
    }


    xmlWriter->endElement(); // table:[covered-]table-cell
}

void ExcelImport::Private::processCharts(KoXmlWriter* manifestWriter)
{
    foreach(ChartExport *c, this->charts) {
        c->saveContent(this->storeout, manifestWriter);
    }
}

// Processes style for a cell within a sheet.
void ExcelImport::Private::processCellForStyle(Cell* cell, KoXmlWriter* xmlWriter)
{
    if (!cell) return;
    if (!xmlWriter) return;

    // TODO optimize with hash table
    Format format = cell->format();
    QString styleName = processCellFormat(&format, cellFormula(cell));
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

    if (format.font().superscript() && superScriptStyle.isEmpty()) {
        KoGenStyle style(KoGenStyle::TextAutoStyle, "text");
        style.addProperty("style:text-position", "super", KoGenStyle::TextType);
        superScriptStyle = styles->insert(style, "T");
    }
    if (format.font().subscript() && subScriptStyle.isEmpty()) {
        KoGenStyle style(KoGenStyle::TextAutoStyle, "text");
        style.addProperty("style:text-position", "sub", KoGenStyle::TextType);
        subScriptStyle = styles->insert(style, "T");
    }

    QList<MSO::OfficeArtSpgrContainerFileBlock> objects = cell->drawObjects();
    if (!objects.empty()) {
        ODrawClient client = ODrawClient(cell->sheet());
        ODrawToOdf odraw( client);
        QBuffer b;
        KoXmlWriter xml(&b);
        Writer writer(xml, *styles, false);
        foreach (const MSO::OfficeArtSpgrContainerFileBlock& fb, objects) {
            odraw.processDrawing(fb, writer);
        }
        //qDebug() << cell->columnLabel() << cell->row() << b.data();
    }
}

// Processes styles for a cell within a sheet.
QString ExcelImport::Private::processCellFormat(Format* format, const QString& formula)
{
    // handle data format, e.g. number style
    QString refName;
    QString valueFormat = format->valueFormat();
    if (valueFormat != QString("General")) {
        refName = processValueFormat(valueFormat);
    } else {
        if(formula.startsWith("msoxl:=")) { // special cases
            QRegExp roundRegExp( "^msoxl:=ROUND[A-Z]*\\(.*;[\\s]*([0-9]+)[\\s]*\\)$" );
            if (roundRegExp.indexIn(formula) >= 0) {
                bool ok = false;
                int decimals = roundRegExp.cap(1).trimmed().toInt(&ok);
                if (ok) {
                    KoGenStyle style(KoGenStyle::NumericNumberStyle);
                    QBuffer buffer;
                    buffer.open(QIODevice::WriteOnly);
                    KoXmlWriter xmlWriter(&buffer);    // TODO pass indentation level
                    xmlWriter.startElement("number:number");
                    xmlWriter.addAttribute("number:decimal-places", decimals);
                    xmlWriter.endElement(); // number:number
                    QString elementContents = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());
                    style.addChildElement("number", elementContents);
                    refName = styles->insert(style, "N");
                }
            } else if(formula.startsWith("msoxl:=RAND(")) {
                KoGenStyle style(KoGenStyle::NumericNumberStyle);
                QBuffer buffer;
                buffer.open(QIODevice::WriteOnly);
                KoXmlWriter xmlWriter(&buffer);    // TODO pass indentation level
                xmlWriter.startElement("number:number");
                xmlWriter.addAttribute("number:decimal-places", 9);
                xmlWriter.endElement(); // number:number
                QString elementContents = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());
                style.addChildElement("number", elementContents);
                refName = styles->insert(style, "N");
            }
        }
    }

    KoGenStyle style(KoGenStyle::TableCellAutoStyle, "table-cell");
    // now the real table-cell
    if (!refName.isEmpty())
        style.addAttribute("style:data-style-name", refName);

    processFormat(format, style);
    QString styleName = styles->insert(style, "ce");
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

QString convertColor(const Color& color)
{
    char buf[8];
    sprintf(buf, "#%02x%02x%02x", color.red, color.green, color.blue);
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
        style.addProperty("fo:font-family", QString::fromRawData(reinterpret_cast<const QChar*>(font.fontFamily().data()), font.fontFamily().length()), KoGenStyle::TextType);

    style.addPropertyPt("fo:font-size", font.fontSize(), KoGenStyle::TextType);

    style.addProperty("fo:color", convertColor(font.color()), KoGenStyle::TextType);
}

// Processes a formatting.
void ExcelImport::Private::processFormat(Format* format, KoGenStyle& style)
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
            style.addProperty("koffice:vertical-distributed", "distributed");
            break;
        case Format::VDistributed:
            style.addProperty("style:vertical-align", "middle");
            style.addProperty("koffice:vertical-distributed", "distributed");
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

        Color backColor = back.backgroundColor();
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
    NumberFormatParser::setStyles( styles );
    const KoGenStyle style = NumberFormatParser::parse( valueFormat );
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
    writer.startElement("style:background-image");
    writer.addAttribute("xlink:href", sheet->backgroundImage());
    writer.addAttribute("xlink:type", "simple");
    writer.addAttribute("xlink:show", "embed");
    writer.addAttribute("xlink:actuate", "onLoad");
    writer.endElement();

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

