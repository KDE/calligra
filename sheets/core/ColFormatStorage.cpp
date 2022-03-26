/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ColFormatStorage.h"

#include "engine/calligra_sheets_limits.h"

#include "3rdparty/mdds/flat_segment_tree.hpp"

#include "Map.h"
#include "Sheet.h"

using namespace Calligra::Sheets;

class Q_DECL_HIDDEN ColFormatStorage::Private
{
public:
    Private();
    double rawColWidth(int col, int* lastCol = 0, int* firstCol = 0) const;

    Sheet* sheet;
    mdds::flat_segment_tree<int, double> colWidths;
    mdds::flat_segment_tree<int, bool> hidden;
    mdds::flat_segment_tree<int, bool> filtered;
    mdds::flat_segment_tree<int, bool> hasPageBreak;
};

ColFormatStorage::Private::Private()
    : colWidths(1, KS_colMax+1, -1)
    , hidden(1, KS_colMax+1, false)
    , filtered(1, KS_colMax+1, false)
    , hasPageBreak(1, KS_colMax+1, false)
{
}

ColFormatStorage::ColFormatStorage(Sheet* sheet)
    : d(new Private)
{
    d->sheet = sheet;
}

ColFormatStorage::~ColFormatStorage()
{
    delete d;
}

ColFormatStorage& ColFormatStorage::operator=(const ColFormatStorage& r)
{
    *d = *r.d;
    return *this;
}

Sheet* ColFormatStorage::sheet() const
{
    return d->sheet;
}

double ColFormatStorage::colWidth(int col, int *lastCol, int *firstCol) const
{
    double v = d->rawColWidth(col, lastCol, firstCol);
    if (v == -1) {
        return d->sheet->fullMap()->defaultColumnFormat().width;
    } else {
        return v;
    }
}

double ColFormatStorage::Private::rawColWidth(int col, int *lastCol, int *firstCol) const
{
    double v;
    auto res = colWidths.search(col, v, firstCol, lastCol);
    if (!res.second) {
        if (firstCol) *firstCol = col;
        if (lastCol) *lastCol = col;
        return -1;
    } else {
        if (lastCol) (*lastCol)--;
        return v;
    }
}

void ColFormatStorage::setColWidth(int firstCol, int lastCol, double width)
{
    // first get old col width to properly update documentSize
    double deltaWidth = -totalVisibleColWidth(firstCol, lastCol);

    d->colWidths.insert_back(firstCol, lastCol+1, width);

    deltaWidth += totalVisibleColWidth(firstCol, lastCol);
    d->sheet->adjustDocumentWidth(deltaWidth);
}

double ColFormatStorage::totalColWidth(int firstCol, int lastCol) const
{
    if (lastCol < firstCol) return 0;
    double res = 0;
    for (int col = firstCol; col <= lastCol; ++col) {
        int last;
        double h = colWidth(col, &last);
        res += (qMin(last, lastCol) - col + 1) * h;
        col = last;
    }
    return res;
}

double ColFormatStorage::visibleWidth(int col, int *lastCol, int *firstCol) const
{
    if (isHiddenOrFiltered(col, lastCol, firstCol)) {
        return 0.0;
    } else {
        int hLastCol, hFirstCol;
        double width = colWidth(col, &hLastCol, &hFirstCol);
        if (lastCol) *lastCol = qMin(*lastCol, hLastCol);
        if (firstCol) *firstCol = qMax(*firstCol, hFirstCol);
        return width;
    }
}

double ColFormatStorage::totalVisibleColWidth(int firstCol, int lastCol) const
{
    if (lastCol < firstCol) return 0;
    double res = 0;
    for (int col = firstCol; col <= lastCol; ++col) {
        int last;
        double h = visibleWidth(col, &last);
        res += (qMin(last, lastCol) - col + 1) * h;
        col = last;
    }
    return res;
}

int ColFormatStorage::colForPosition(double ypos, double *topOfCol) const
{
    int col = 1;
    double y = 0;
    while (col < KS_colMax) {
        int last;
        const double h = visibleWidth(col, &last);
        if (h == 0) {
            col = last+1;
            continue;
        }
        const int cnt = last - col + 1;
        const int maxcnt = qMin(int((ypos - y) / h), cnt);
        y += maxcnt * h;
        col += maxcnt;
        if (maxcnt < cnt) {
            if (topOfCol) *topOfCol = y;
            return col;
        }
    }
    return KS_colMax;
}

bool ColFormatStorage::isHidden(int col, int *lastCol, int *firstCol) const
{
    bool v;
    auto res = d->hidden.search(col, v, firstCol, lastCol);
    if (!res.second) {    // not found
        if (firstCol) *firstCol = col;
        if (lastCol) *lastCol = col;
        return false;
    } else {
        if (lastCol) (*lastCol)--;
        return v;
    }
}

void ColFormatStorage::setHidden(int firstCol, int lastCol, bool hidden)
{
    double deltaWidth = 0;
    if (hidden) deltaWidth -= totalVisibleColWidth(firstCol, lastCol);
    d->hidden.insert_back(firstCol, lastCol+1, hidden);
    if (!hidden) deltaWidth += totalVisibleColWidth(firstCol, lastCol);
    d->sheet->adjustDocumentWidth(deltaWidth);
}

bool ColFormatStorage::isFiltered(int col, int* lastCol, int *firstCol) const
{
    bool v;
    auto res = d->filtered.search(col, v, firstCol, lastCol);
    if (!res.second) {    // not found
        if (firstCol) *firstCol = col;
        if (lastCol) *lastCol = col;
        return false;
    } else {
        if (lastCol) (*lastCol)--;
        return v;
    }
}

void ColFormatStorage::setFiltered(int firstCol, int lastCol, bool filtered)
{
    double deltaWidth = 0;
    if (filtered) deltaWidth -= totalVisibleColWidth(firstCol, lastCol);
    d->filtered.insert_back(firstCol, lastCol+1, filtered);
    if (!filtered) deltaWidth += totalVisibleColWidth(firstCol, lastCol);
    d->sheet->adjustDocumentWidth(deltaWidth);
}

bool ColFormatStorage::isHiddenOrFiltered(int col, int* lastCol, int* firstCol) const
{
    int hLastCol, hFirstCol, fLastCol, fFirstCol;
    bool v = isHidden(col, &hLastCol, &hFirstCol);
    v = isFiltered(col, &fLastCol, &fFirstCol) || v;
    if (lastCol) *lastCol = qMin(hLastCol, fLastCol);
    if (firstCol) *firstCol = qMax(hFirstCol, fFirstCol);
    return v;
}

bool ColFormatStorage::hasPageBreak(int col, int* lastCol, int* firstCol) const
{
    bool v;
    auto res = d->hasPageBreak.search(col, v, firstCol, lastCol);
    if (!res.second) {    // not found
        if (lastCol) *lastCol = col;
        if (firstCol) *firstCol = col;
        return false;
    } else {
        if (lastCol) (*lastCol)--;
        return v;
    }
}

void ColFormatStorage::setPageBreak(int firstCol, int lastCol, bool pageBreak)
{
    d->hasPageBreak.insert_back(firstCol, lastCol+1, pageBreak);
}

void ColFormatStorage::setColFormat (int firstCol, int lastCol, const ColFormat &f)
{
    setColWidth(firstCol, lastCol, f.width);
    setHidden (firstCol, lastCol, f.hidden);
    setFiltered (firstCol, lastCol, f.filtered);
    setPageBreak (firstCol, lastCol, f.hasPageBreak);
}

ColFormat ColFormatStorage::getColFormat(int col) const
{
    ColFormat res;
    res.width = colWidth(col);
    res.hidden = isHidden(col);
    res.filtered = isFiltered(col);
    res.hasPageBreak = hasPageBreak(col);
    return res;
}


int ColFormatStorage::lastNonDefaultCol() const
{
    int col = KS_colMax;
    int firstCol;
    while (col > 0 && isDefaultCol(col, 0, &firstCol)) {
        col = firstCol-1;
    }
    if (col < 1) return 1;
    return col;
}

bool ColFormatStorage::colsAreEqual(int col1, int col2) const
{
    return colWidth(col1) == colWidth(col2)
            && isHidden(col1) == isHidden(col2)
            && isFiltered(col1) == isFiltered(col2)
            && hasPageBreak(col1) == hasPageBreak(col2);
}

bool ColFormatStorage::isDefaultCol(int col, int* lastCol, int* firstCol) const
{
    bool isDef = true;
    int l, f;
    isDef = d->rawColWidth(col, lastCol, firstCol) == -1 && isDef;
    isDef = !isHiddenOrFiltered(col, &l, &f) && isDef;
    if (lastCol) *lastCol = qMin(*lastCol, l);
    if (firstCol) *firstCol = qMax(*firstCol, f);
    isDef = !hasPageBreak(col, &l, &f) && isDef;
    if (lastCol) *lastCol = qMin(*lastCol, l);
    if (firstCol) *firstCol = qMax(*firstCol, f);
    return isDef;
}

void ColFormatStorage::setDefault(int firstCol, int lastCol)
{
    setColWidth(firstCol, lastCol, -1);
    setHidden(firstCol, lastCol, false);
    setFiltered(firstCol, lastCol, false);
    setPageBreak(firstCol, lastCol, false);
}

void ColFormatStorage::insertCols(int col, int number)
{
    double deltaWidth = -totalColWidth(KS_colMax - number + 1, KS_colMax);
    d->colWidths.shift_right(col, number, false);
    deltaWidth += totalColWidth(col, col + number - 1);
    d->sheet->adjustDocumentWidth(deltaWidth);

    d->hidden.shift_right(col, number, false);
    d->filtered.shift_right(col, number, false);
    d->hasPageBreak.shift_right(col, number, false);
}

void ColFormatStorage::removeCols(int col, int number)
{
    double deltaWidth = -totalColWidth(col, col + number - 1);
    d->colWidths.shift_left(col, col+number-1);
    deltaWidth += totalColWidth(KS_colMax - number + 1, KS_colMax);
    d->sheet->adjustDocumentWidth(deltaWidth);

    d->hidden.shift_left(col, col+number-1);
    d->filtered.shift_left(col, col+number-1);
    d->hasPageBreak.shift_left(col, col+number-1);
}

