/* Swinder - Portable library for spreadsheet
   Copyright (C) 2003 Ariya Hidayat <ariya@kde.org>

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
#include "ustring.h"

#include <iostream>
#include <map>
#include <QPoint>

namespace Swinder
{

class Sheet::Private
{
public:
    Workbook* workbook;
    UString name;

    // hash to store cell, FIXME replace with quad-tree
    std::map<unsigned, Cell*> cells;
    unsigned maxRow;
    unsigned maxColumn;
    std::map<unsigned, Column*> columns;
    std::map<unsigned, Row*> rows;

    bool visible;
    bool protect;
    bool autoCalc;

    UString leftHeader;
    UString centerHeader;
    UString rightHeader;
    UString leftFooter;
    UString centerFooter;
    UString rightFooter;

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
};

}

using namespace Swinder;

Sheet::Sheet(Workbook* wb)
{
    d = new Sheet::Private();
    d->workbook = wb;
    d->name = "Sheet"; // FIXME better name ?
    d->maxRow = 0;
    d->maxColumn = 0;
    d->visible      = true;
    d->protect      = false;
    d->leftMargin   = 54;  // 0.75 inch
    d->rightMargin  = 54;  // 0.75 inch
    d->topMargin    = 72;  // 1 inch
    d->bottomMargin = 72;  // 1 inch
    d->autoCalc = true;
    d->defaultRowHeight = -1;
    d->defaultColWidth = -1;
    d->zoomLevel = 1.0; // 100%
    d->showGrid = true;
    d->showZeroValues = true;
    d->firstVisibleCell = QPoint(0,0); // A1
    d->isPageBreakViewEnabled = false;
    d->passwd = 0; // password protection disabled
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
    // delete all cells
    std::map<unsigned, Cell*>::iterator cell_it;
    for (cell_it = d->cells.begin(); cell_it != d->cells.end(); ++cell_it)
        delete cell_it->second;

    // delete all columns
    std::map<unsigned, Column*>::iterator col_it;
    for (col_it = d->columns.begin(); col_it != d->columns.end(); ++col_it)
        delete col_it->second;

    // delete all rows
    std::map<unsigned, Row*>::iterator row_it;
    for (row_it = d->rows.begin(); row_it != d->rows.end(); ++row_it)
        delete row_it->second;
}

UString Sheet::name() const
{
    return d->name;
}

void Sheet::setName(const UString& name)
{
    d->name = name;
}

Cell* Sheet::cell(unsigned columnIndex, unsigned rowIndex, bool autoCreate)
{
    unsigned hashed = (rowIndex + 1) * 1024 + columnIndex + 1;
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

UString Sheet::leftHeader() const
{
    return d->leftHeader;
}

void Sheet::setLeftHeader(const UString& h)
{
    d->leftHeader = h;
}

UString Sheet::centerHeader() const
{
    return d->centerHeader;
}

void Sheet::setCenterHeader(const UString& h)
{
    d->centerHeader = h;
}

UString Sheet::rightHeader() const
{
    return d->rightHeader;
}

void Sheet::setRightHeader(const UString& h)
{
    d->rightHeader = h;
}

UString Sheet::leftFooter() const
{
    return d->leftFooter;
}

void Sheet::setLeftFooter(const UString& h)
{
    d->leftFooter = h;
}

UString Sheet::centerFooter() const
{
    return d->centerFooter;
}

void Sheet::setCenterFooter(const UString& h)
{
    d->centerFooter = h;
}

UString Sheet::rightFooter() const
{
    return d->rightFooter;
}

void Sheet::setRightFooter(const UString& h)
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

class Column::Private
{
public:
    Sheet* sheet;
    unsigned index;
    double width;
    Format format;
    bool visible;
};

Column::Column(Sheet* sheet, unsigned index)
{
    d = new Column::Private;
    d->sheet   = sheet;
    d->index   = index;
    d->width   = 2560 / 120;
    d->visible = true;
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
    return d->width;
}

void Column::setWidth(double w)
{
    d->width = w;
}

const Format& Column::format() const
{
    return d->format;
}

void Column::setFormat(const Format& f)
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

class Row::Private
{
public:
    Sheet* sheet;
    unsigned index;
    double height;
    Format format;
    bool visible;
};

Row::Row(Sheet* sheet, unsigned index)
{
    d = new Row::Private;
    d->sheet   = sheet;
    d->index   = index;
    d->height  = 10;
    d->visible = true;
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
    return d->format;
}

void Row::setFormat(const Format& f)
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
