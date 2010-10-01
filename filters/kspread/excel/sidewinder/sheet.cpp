/* Swinder - Portable library for spreadsheet
   Copyright (C) 2003 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2010 Carlos Licea <carlos@kdab.com>

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
 * Boston, MA 02110-1301, USA
*/

#include "cell.h"
#include "sheet.h"
#include "workbook.h"
#include "utils.h"
#include "objects.h"

#include <PointStorage.h>

#include <iostream>
#include <map>
#include <QPoint>
#include <QFont>
#include <QFontMetricsF>
#include <QWidget>
#include <QDebug>
#include <QList>

namespace Swinder
{

class Sheet::Private
{
public:
    Workbook* workbook;
    QString name;

    // hash to store cell, FIXME replace with quad-tree
    QHash<unsigned, Cell*> cells;
    unsigned maxRow;
    unsigned maxColumn;
    QHash<unsigned, unsigned> maxCellsInRow;
    QHash<unsigned, Column*> columns;
    QHash<unsigned, Row*> rows;
    KSpread::PointStorage<Hyperlink> hyperlinks;
    KSpread::PointStorage<QList<PictureObject*> > pictures;
    KSpread::PointStorage<QList<ChartObject*> > charts;
    KSpread::PointStorage<QList<OfficeArtObject*> > drawObjects;

    bool visible;
    bool protect;
    bool autoCalc;

    QString leftHeader;
    QString centerHeader;
    QString rightHeader;
    QString leftFooter;
    QString centerFooter;
    QString rightFooter;

    double leftMargin;
    double rightMargin;
    double topMargin;
    double bottomMargin;

    long defaultRowHeight;
    long defaultColWidth;

    double zoomLevel;
    bool showGrid;
    bool showZeroValues;
    QPoint firstVisibleCell;
    bool isPageBreakViewEnabled;

    unsigned long passwd;
    QString backgroundImagePath;

    QList<VerticalPageBreak> verticalPageBreaks;
    QList<HorizontalPageBreak> horizontalPageBreaks;

    QList<MSO::OfficeArtSpgrContainer> sheetDrawObjectsGroups;
    QMultiHash<int, OfficeArtObject*> sheetDrawObjects;

    QList<ConditionalFormat*> conditionalFormats;
};

}

using namespace Swinder;

Sheet::Sheet(Workbook* wb)
{
    d = new Sheet::Private();
    d->workbook = wb;
    clear();
}

Sheet::~Sheet()
{
    clear();
    delete d;
}

Workbook* Sheet::workbook()
{
    return d->workbook;
}

bool Sheet::autoCalc() const
{
    return d->autoCalc;
}

void Sheet::setAutoCalc(bool a)
{
    d->autoCalc = a;
}

void Sheet::clear()
{
    // delete all cell data
    for (int i = 0; i < d->pictures.count(); i++) {
        qDeleteAll(d->pictures.data(i));
    }
    for (int i = 0; i < d->charts.count(); i++) {
        qDeleteAll(d->charts.data(i));
    }
    for (int i = 0; i < d->drawObjects.count(); i++) {
        qDeleteAll(d->drawObjects.data(i));
    }
    qDeleteAll(d->sheetDrawObjects);
    // delete all cells
    qDeleteAll(d->cells);
    d->cells.clear();
    // delete all columns
    qDeleteAll(d->columns);
    d->columns.clear();
    // delete all rows
    qDeleteAll(d->rows);
    d->rows.clear();
    // delete all conditional formats
    qDeleteAll(d->conditionalFormats);
    d->conditionalFormats.clear();

    d->name = "Sheet"; // FIXME better name ?
    d->maxRow = 0;
    d->maxColumn = 0;
    d->maxCellsInRow.clear();
    d->visible      = true;
    d->protect      = false;
    d->leftMargin   = 54;  // 0.75 inch
    d->rightMargin  = 54;  // 0.75 inch
    d->topMargin    = 72;  // 1 inch
    d->bottomMargin = 72;  // 1 inch
    d->autoCalc = true;
    d->defaultRowHeight = 12;
    d->defaultColWidth = Column::columnUnitsToPts(8.43 * 256.0);
    d->zoomLevel = 1.0; // 100%
    d->showGrid = true;
    d->showZeroValues = true;
    d->firstVisibleCell = QPoint(0,0); // A1
    d->isPageBreakViewEnabled = false;
    d->passwd = 0; // password protection disabled
}

QString Sheet::name() const
{
    return d->name;
}

void Sheet::setName(const QString& name)
{
    d->name = name;
}

Cell* Sheet::cell(unsigned columnIndex, unsigned rowIndex, bool autoCreate)
{
    const unsigned hashed = (rowIndex + 1) * maximalColumnCount + columnIndex + 1;
    Cell* c = d->cells[ hashed ];

    // create cell if necessary
    if (!c && autoCreate) {
        c = new Cell(this, columnIndex, rowIndex);
        d->cells[ hashed ] = c;

        // force creating the column and row
        this->column(columnIndex, true);
        this->row(rowIndex, true);

        if (rowIndex > d->maxRow) d->maxRow = rowIndex;
        if (columnIndex > d->maxColumn) d->maxColumn = columnIndex;

        if(!d->maxCellsInRow.contains(rowIndex) || columnIndex > d->maxCellsInRow[rowIndex])
            d->maxCellsInRow[rowIndex] = columnIndex;
    }

    return c;
}

Column* Sheet::column(unsigned index, bool autoCreate)
{
    Column* c = d->columns[ index ];

    // create column if necessary
    if (!c && autoCreate) {
        c = new Column(this, index);
        d->columns[ index ] = c;
        if (index > d->maxColumn) d->maxColumn = index;
    }

    return c;
}

Row* Sheet::row(unsigned index, bool autoCreate)
{
    Row* r = d->rows[ index ];

    // create row if necessary
    if (!r && autoCreate) {
        r = new Row(this, index);
        d->rows[ index ] = r;
        if (index > d->maxRow) d->maxRow = index;
    }

    return r;
}

unsigned Sheet::maxRow() const
{
    return d->maxRow;
}

unsigned Sheet::maxColumn() const
{
    return d->maxColumn;
}

void Sheet::setMaxRow(unsigned row)
{
    if (row > d->maxRow)
        d->maxRow = row;
}

void Sheet::setMaxColumn(unsigned column)
{
    if (column > d->maxColumn)
        d->maxColumn = column;
}

unsigned Sheet::maxCellsInRow(int rowIndex) const
{
    if(d->maxCellsInRow.contains(rowIndex))
        return d->maxCellsInRow[rowIndex];
    return 0;
}

bool Sheet::visible() const
{
    return d->visible;
}

void Sheet::setVisible(bool v)
{
    d->visible = v;
}

bool Sheet::protect() const
{
    return d->protect;
}

void Sheet::setProtect(bool p)
{
    d->protect = p;
}

QString Sheet::leftHeader() const
{
    return d->leftHeader;
}

void Sheet::setLeftHeader(const QString& h)
{
    d->leftHeader = h;
}

QString Sheet::centerHeader() const
{
    return d->centerHeader;
}

void Sheet::setCenterHeader(const QString& h)
{
    d->centerHeader = h;
}

QString Sheet::rightHeader() const
{
    return d->rightHeader;
}

void Sheet::setRightHeader(const QString& h)
{
    d->rightHeader = h;
}

QString Sheet::leftFooter() const
{
    return d->leftFooter;
}

void Sheet::setLeftFooter(const QString& h)
{
    d->leftFooter = h;
}

QString Sheet::centerFooter() const
{
    return d->centerFooter;
}

void Sheet::setCenterFooter(const QString& h)
{
    d->centerFooter = h;
}

QString Sheet::rightFooter() const
{
    return d->rightFooter;
}

void Sheet::setRightFooter(const QString& h)
{
    d->rightFooter = h;
}

double Sheet::leftMargin() const
{
    return d->leftMargin;
}

void Sheet::setLeftMargin(double m)
{
    d->leftMargin = m;
}

double Sheet::rightMargin() const
{
    return d->rightMargin;
}

void Sheet::setRightMargin(double m)
{
    d->rightMargin = m;
}

double Sheet::topMargin() const
{
    return d->topMargin;
}

void Sheet::setTopMargin(double m)
{
    d->topMargin = m;
}

double Sheet::bottomMargin() const
{
    return d->bottomMargin;
}

void Sheet::setBottomMargin(double m)
{
    d->bottomMargin = m;
}

long Sheet::defaultRowHeight() const
{
    return d->defaultRowHeight;
}

void Sheet::setDefaultRowHeight(long i)
{
    d->defaultRowHeight = i;
}

long Sheet::defaultColWidth() const
{
    return d->defaultColWidth;
}

void Sheet::setDefaultColWidth(long i)
{
    d->defaultColWidth = i;
}

double Sheet::zoomLevel() const
{
    return d->zoomLevel;
}

void Sheet::setZoomLevel(double fraction)
{
    d->zoomLevel = fraction;
}

bool Sheet::showGrid() const
{
    return d->showGrid;
}

void Sheet::setShowGrid(bool show)
{
    d->showGrid = show;
}

bool Sheet::showZeroValues() const
{
    return d->showZeroValues;
}

void Sheet::setShowZeroValues(bool show)
{
    d->showZeroValues = show;
}

QPoint Sheet::firstVisibleCell() const
{
    return d->firstVisibleCell;
}

void Sheet::setFirstVisibleCell(const QPoint &point)
{
    d->firstVisibleCell = point;
}

bool Sheet::isPageBreakViewEnabled() const
{
    return d->isPageBreakViewEnabled;
}

void Sheet::setPageBreakViewEnabled(bool enabled)
{
    d->isPageBreakViewEnabled = enabled;
}

unsigned long Sheet::password() const
{
    return d->passwd;
}

void Sheet::setPassword(unsigned long hash)
{
    d->passwd = hash;
}

void Sheet::setBackgroundImage( const QString& imagePath )
{
    d->backgroundImagePath = imagePath;
}

QString Sheet::backgroundImage()
{
    return d->backgroundImagePath;
}

void Sheet::addVerticalPageBreak(const Swinder::VerticalPageBreak& pageBreak)
{
    d->verticalPageBreaks.append(pageBreak);
}

QList<VerticalPageBreak> Sheet::verticalPageBreaks()
{
    return d->verticalPageBreaks;
}

void Sheet::addHorizontalPageBreak(const Swinder::HorizontalPageBreak& pageBreak)
{
    d->horizontalPageBreaks.append(pageBreak);
}

QList<HorizontalPageBreak> Sheet::horizontalPageBreaks()
{
    return d->horizontalPageBreaks;
}

Hyperlink Sheet::hyperlink(unsigned column, unsigned row) const
{
    return d->hyperlinks.lookup(column+1, row+1);
}

void Sheet::setHyperlink(unsigned column, unsigned row, const Hyperlink& link)
{
    if (link.isValid)
        d->hyperlinks.insert(column+1, row+1, link);
    else
        d->hyperlinks.take(column+1, row+1);
}

QList<PictureObject*> Sheet::pictures(unsigned column, unsigned row) const
{
    return d->pictures.lookup(column+1, row+1);
}

void Sheet::setPictures(unsigned column, unsigned row, const QList<PictureObject*>& pictures)
{
    if (pictures.isEmpty())
        d->pictures.take(column+1, row+1);
    else
        d->pictures.insert(column+1, row+1, pictures);
}

void Sheet::addPicture(unsigned column, unsigned row, PictureObject* picture)
{
    QList<PictureObject*> pics = pictures(column, row);
    pics.append(picture);
    setPictures(column, row, pics);
}

QList<ChartObject*> Sheet::charts(unsigned column, unsigned row) const
{
    return d->charts.lookup(column+1, row+1);
}

void Sheet::setCharts(unsigned column, unsigned row, const QList<ChartObject*>& charts)
{
    if (charts.isEmpty())
        d->charts.take(column+1, row+1);
    else
        d->charts.insert(column+1, row+1, charts);
}

void Sheet::addChart(unsigned column, unsigned row, ChartObject* chart)
{
    QList<ChartObject*> chrts = charts(column, row);
    chrts.append(chart);
    setCharts(column, row, chrts);
}
    
QList<OfficeArtObject*> Sheet::drawObjects(unsigned column, unsigned row) const
{
    return d->drawObjects.lookup(column+1, row+1);
}

void Sheet::setDrawObjects(unsigned column, unsigned row, const QList<OfficeArtObject*>& drawObjects)
{
    if (drawObjects.isEmpty())
        d->drawObjects.take(column+1, row+1);
    else
        d->drawObjects.insert(column+1, row+1, drawObjects);
}

void Sheet::addDrawObject(unsigned column, unsigned row, OfficeArtObject* drawObject)
{
    QList<OfficeArtObject*> objects = drawObjects(column, row);
    objects.append(drawObject);
    setDrawObjects(column, row, objects);
}

int Sheet::drawObjectsGroupCount() const
{
    return d->sheetDrawObjectsGroups.size();
}

MSO::OfficeArtSpgrContainer Sheet::drawObjectsGroup(int groupId) const
{
    Q_ASSERT(groupId >= 0 && groupId < drawObjectsGroupCount());
    return d->sheetDrawObjectsGroups[groupId];
}

QList<OfficeArtObject*> Sheet::drawObjects(int groupId) const
{
    Q_ASSERT(groupId < drawObjectsGroupCount());
    if (groupId < 0) {
        return d->sheetDrawObjects.values(-1);
    } else {
        return d->sheetDrawObjects.values(groupId);
    }
}

static int shapeGroupId(const MSO::OfficeArtSpgrContainer& group)
{
    return group.rgfb.first().anon.get<MSO::OfficeArtSpContainer>()->shapeProp.spid;
}

void Sheet::addDrawObject(OfficeArtObject* drawObject, const MSO::OfficeArtSpgrContainer* group )
{
    int groupId = -1;
    if (group) {
        for (int i = 0; i < d->sheetDrawObjectsGroups.size(); ++i) {
            if (shapeGroupId(*group) == shapeGroupId(d->sheetDrawObjectsGroups[i])) {
                groupId = i;
            }
        }
        if (groupId == -1) {
            d->sheetDrawObjectsGroups.append(*group);
            groupId = d->sheetDrawObjectsGroups.size() - 1;
        }
    }
    d->sheetDrawObjects.insert(groupId, drawObject);
}

void Sheet::addConditionalFormat(ConditionalFormat *format)
{
    d->conditionalFormats.append(format);
}

QList<ConditionalFormat*> Sheet::conditionalFormats() const
{
    return d->conditionalFormats;
}

#ifdef SWINDER_XLS2RAW
void Sheet::dumpStats()
{
    int ndValue = 0, ndFormula = 0, ndFormat = 0, ndColumnSpan = 0, ndRowSpan = 0, ndCovered = 0, ndColumnRepeat = 0, ndHyperlink = 0, ndNote = 0, ndPictures = 0, ndCharts = 0;
    foreach (Cell* c, d->cells) {
        if (c->value() != Value()) ndValue++;
        if (!c->formula().isEmpty()) ndFormula++;
        if (c->format() != Format()) ndFormat++;
        if (c->columnSpan() != 1) ndColumnSpan++;
        if (c->rowSpan() != 1) ndRowSpan++;
        if (c->isCovered()) ndCovered++;
        if (c->columnRepeat() != 1) ndColumnRepeat++;
        if (c->hasHyperlink()) ndHyperlink++;
        if (!c->note().isEmpty()) ndNote++;
        if (c->pictures().size()) ndPictures++;
        if (c->charts().size()) ndCharts++;
    }
    printf("    rows: %d\n  cols: %d\n  cells: %d\n", d->rows.size(), d->columns.size(), d->cells.size());
    printf("       values: %d\n       formulas: %d\n       formats: %d\n       colspans: %d\n       rowspans: %d\n       covered: %d\n       colrepeat: %d\n       hyperlink: %d\n       note: %d\n       pics: %d\n       charts: %d\n", ndValue, ndFormula, ndFormat, ndColumnSpan, ndRowSpan, ndCovered, ndColumnRepeat, ndHyperlink, ndNote, ndPictures, ndCharts);
}
#endif

class Column::Private
{
public:
    Sheet* sheet;
    unsigned index;
    double width;
    const Format* format;
    bool visible;
    unsigned outlineLevel;
    bool collapsed;
};

Column::Column(Sheet* sheet, unsigned index)
{
    d = new Column::Private;
    d->sheet   = sheet;
    d->index   = index;
    d->width   = 0.0;
    d->visible = true;
    d->outlineLevel = 0;
    d->collapsed = false;
    d->format  = 0;
}

Column::~Column()
{
    delete d;
}

Sheet* Column::sheet() const
{
    return d->sheet;
}

unsigned Column::index() const
{
    return d->index;
}

double Column::width() const
{
    if( d->width == 0.0)
        return d->sheet->defaultColWidth();

    return d->width;
}

void Column::setWidth(double w)
{
    d->width = w;
}

const Format& Column::format() const
{
    static const Format null;
    if (!d->format) return null;
    return *(d->format);
}

void Column::setFormat(const Format* f)
{
    d->format = f;
}

bool Column::visible() const
{
    return d->visible;
}

void Column::setVisible(bool b)
{
    d->visible = b;
}

unsigned Column::outlineLevel() const
{
    return d->outlineLevel;
}

void Column::setOutlineLevel(unsigned level)
{
    d->outlineLevel = level;
}

bool Column::collapsed() const
{
    return d->collapsed;
}

void Column::setCollapsed(bool collapsed)
{
    d->collapsed = collapsed;
}

bool Column::operator==(const Column &other) const
{
    return width() == other.width() &&
           visible() == other.visible() &&
           format() == other.format() &&
           outlineLevel() == other.outlineLevel();
}

bool Column::operator!=(const Column &other) const
{
    return ! (*this == other);
}

double Column::columnUnitsToPts(const double columnUnits)
{
    QFont font("Arial", 10);
    QFontMetricsF fontMetrics(font);
    double characterWidth = qMax(fontMetrics.width("0"),qMax(fontMetrics.width("1"),
    qMax(fontMetrics.width("2"),qMax(fontMetrics.width("3"),
    qMax(fontMetrics.width("4"),qMax(fontMetrics.width("5"),
    qMax(fontMetrics.width("6"),qMax(fontMetrics.width("7"),
    qMax(fontMetrics.width("8"),fontMetrics.width("9"))))))))));

    double width = characterWidth * columnUnits / 256.0; //px
    width = qRound(width / 8.0 + 0.5) * 8.0;

    QWidget widget;
    width /= (double)widget.physicalDpiX(); //in
    width *= 72.0; //pt
    return width;
}

class Row::Private
{
public:
    Sheet* sheet;
    unsigned index;
    double height;
    const Format* format;
    bool visible;
    unsigned outlineLevel;
    bool collapsed;
};

Row::Row(Sheet* sheet, unsigned index)
{
    d = new Row::Private;
    d->sheet   = sheet;
    d->index   = index;
    d->height  = 12;
    d->visible = true;
    d->outlineLevel = 0;
    d->collapsed = false;
    d->format  = 0;
}

Row::~Row()
{
    delete d;
}

Sheet* Row::sheet() const
{
    return d->sheet;
}

unsigned Row::index() const
{
    return d->index;
}

double Row::height() const
{
    return d->height;
}

void Row::setHeight(double w)
{
    d->height = w;
}

const Format& Row::format() const
{
    static const Format null;
    if (!d->format) return null;
    return *(d->format);
}

void Row::setFormat(const Format* f)
{
    d->format = f;
}

bool Row::visible() const
{
    return d->visible;
}

void Row::setVisible(bool b)
{
    d->visible = b;
}

unsigned Row::outlineLevel() const
{
    return d->outlineLevel;
}

void Row::setOutlineLevel(unsigned level)
{
    d->outlineLevel = level;
}

bool Row::collapsed() const
{
    return d->collapsed;
}

void Row::setCollapsed(bool collapsed)
{
    d->collapsed = collapsed;
}

bool Row::operator==(const Row &other) const
{
    return height() == other.height() &&
           visible() == other.visible() &&
           format() == other.format() &&
           outlineLevel() == other.outlineLevel();
}

bool Row::operator!=(const Row &other) const
{
    return ! (*this == other);
}
