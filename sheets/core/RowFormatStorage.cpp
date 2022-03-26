/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "RowFormatStorage.h"

#include "engine/calligra_sheets_limits.h"
#include "3rdparty/mdds/flat_segment_tree.hpp"

#include "Map.h"
#include "Sheet.h"

using namespace Calligra::Sheets;

class Q_DECL_HIDDEN RowFormatStorage::Private
{
public:
    Private();
    double rawRowHeight(int row, int* lastRow = 0, int* firstRow = 0) const;

    Sheet* sheet;
    mdds::flat_segment_tree<int, double> rowHeights;
    mdds::flat_segment_tree<int, bool> hidden;
    mdds::flat_segment_tree<int, bool> filtered;
    mdds::flat_segment_tree<int, bool> hasPageBreak;
};

RowFormatStorage::Private::Private()
    : rowHeights(1, KS_rowMax+1, -1)
    , hidden(1, KS_rowMax+1, false)
    , filtered(1, KS_rowMax+1, false)
    , hasPageBreak(1, KS_rowMax+1, false)
{
}

RowFormatStorage::RowFormatStorage(Sheet* sheet)
    : d(new Private)
{
    d->sheet = sheet;
}

RowFormatStorage::~RowFormatStorage()
{
    delete d;
}

RowFormatStorage& RowFormatStorage::operator=(const RowFormatStorage& r)
{
    *d = *r.d;
    return *this;
}

Sheet* RowFormatStorage::sheet() const
{
    return d->sheet;
}

double RowFormatStorage::rowHeight(int row, int *lastRow, int *firstRow) const
{
    double v = d->rawRowHeight(row, lastRow, firstRow);
    if (v == -1) {
        return d->sheet->fullMap()->defaultRowFormat().height;
    } else {
        return v;
    }
}

double RowFormatStorage::Private::rawRowHeight(int row, int *lastRow, int *firstRow) const
{
    double v;
    auto res = rowHeights.search(row, v, firstRow, lastRow);
    if (!res.second) {    // not found
        if (firstRow) *firstRow = row;
        if (lastRow) *lastRow = row;
        return -1;
    } else {
        if (lastRow) (*lastRow)--;
        return v;
    }
}

void RowFormatStorage::setRowHeight(int firstRow, int lastRow, double height)
{
    // first get old row height to properly update documentSize
    double deltaHeight = -totalVisibleRowHeight(firstRow, lastRow);

    d->rowHeights.insert_back(firstRow, lastRow+1, height);

    deltaHeight += totalVisibleRowHeight(firstRow, lastRow);
    d->sheet->adjustDocumentHeight(deltaHeight);
}

double RowFormatStorage::totalRowHeight(int firstRow, int lastRow) const
{
    if (lastRow < firstRow) return 0;
    double res = 0;
    for (int row = firstRow; row <= lastRow; ++row) {
        int last;
        double h = rowHeight(row, &last);
        res += (qMin(last, lastRow) - row + 1) * h;
        row = last;
    }
    return res;
}

double RowFormatStorage::visibleHeight(int row, int *lastRow, int *firstRow) const
{
    if (isHiddenOrFiltered(row, lastRow, firstRow)) {
        return 0.0;
    } else {
        int hLastRow, hFirstRow;
        double height = rowHeight(row, &hLastRow, &hFirstRow);
        if (lastRow) *lastRow = qMin(*lastRow, hLastRow);
        if (firstRow) *firstRow = qMax(*firstRow, hFirstRow);
        return height;
    }
}

double RowFormatStorage::totalVisibleRowHeight(int firstRow, int lastRow) const
{
    if (lastRow < firstRow) return 0;
    double res = 0;
    for (int row = firstRow; row <= lastRow; ++row) {
        int last;
        double h = visibleHeight(row, &last);
        res += (qMin(last, lastRow) - row + 1) * h;
        row = last;
    }
    return res;
}

int RowFormatStorage::rowForPosition(double ypos, double *topOfRow) const
{
    int row = 1;
    double y = 0;
    while (row < KS_rowMax) {
        int last;
        const double h = visibleHeight(row, &last);
        if (h == 0) {
            row = last+1;
            continue;
        }
        const int cnt = last - row + 1;
        const int maxcnt = qMin(int((ypos - y) / h), cnt);
        y += maxcnt * h;
        row += maxcnt;
        if (maxcnt < cnt) {
            if (topOfRow) *topOfRow = y;
            return row;
        }
    }
    return KS_rowMax;
}

bool RowFormatStorage::isHidden(int row, int *lastRow, int *firstRow) const
{
    bool v;
    auto res = d->hidden.search(row, v, firstRow, lastRow);
    if (!res.second) {   // failed to find
        if (firstRow) *firstRow = row;
        if (lastRow) *lastRow = row;
        return false;
    } else {
        if (lastRow) (*lastRow)--;
        return v;
    }
}

void RowFormatStorage::setHidden(int firstRow, int lastRow, bool hidden)
{
    double deltaHeight = 0;
    if (hidden) deltaHeight -= totalVisibleRowHeight(firstRow, lastRow);
    d->hidden.insert_back(firstRow, lastRow+1, hidden);
    if (!hidden) deltaHeight += totalVisibleRowHeight(firstRow, lastRow);
    d->sheet->adjustDocumentHeight(deltaHeight);
}

bool RowFormatStorage::isFiltered(int row, int* lastRow, int *firstRow) const
{
    bool v;
    auto res = d->filtered.search(row, v, firstRow, lastRow);
    if (!res.second) {   // failed to find
        if (firstRow) *firstRow = row;
        if (lastRow) *lastRow = row;
        return false;
    } else {
        if (lastRow) (*lastRow)--;
        return v;
    }
}

void RowFormatStorage::setFiltered(int firstRow, int lastRow, bool filtered)
{
    double deltaHeight = 0;
    if (filtered) deltaHeight -= totalVisibleRowHeight(firstRow, lastRow);
    d->filtered.insert_back(firstRow, lastRow+1, filtered);
    if (!filtered) deltaHeight += totalVisibleRowHeight(firstRow, lastRow);
    d->sheet->adjustDocumentHeight(deltaHeight);
}

bool RowFormatStorage::isHiddenOrFiltered(int row, int* lastRow, int* firstRow) const
{
    int hLastRow, hFirstRow, fLastRow, fFirstRow;
    bool v = isHidden(row, &hLastRow, &hFirstRow);
    v = isFiltered(row, &fLastRow, &fFirstRow) || v;
    if (lastRow) *lastRow = qMin(hLastRow, fLastRow);
    if (firstRow) *firstRow = qMax(hFirstRow, fFirstRow);
    return v;
}

bool RowFormatStorage::hasPageBreak(int row, int* lastRow, int* firstRow) const
{
    bool v;
    auto res = d->hasPageBreak.search(row, v, firstRow, lastRow);
    if (!res.second) {   // failed to find
        if (lastRow) *lastRow = row;
        if (firstRow) *firstRow = row;
        return false;
    } else {
        if (lastRow) (*lastRow)--;
        return v;
    }
}

void RowFormatStorage::setPageBreak(int firstRow, int lastRow, bool pageBreak)
{
    d->hasPageBreak.insert_back(firstRow, lastRow+1, pageBreak);
}

void RowFormatStorage::setRowFormat (int firstRow, int lastRow, const RowFormat &f)
{
    setRowHeight (firstRow, lastRow, f.height);
    setHidden (firstRow, lastRow, f.hidden);
    setFiltered (firstRow, lastRow, f.filtered);
    setPageBreak (firstRow, lastRow, f.hasPageBreak);
}

RowFormat RowFormatStorage::getRowFormat(int row) const
{
    RowFormat res;
    res.height = rowHeight(row);
    res.hidden = isHidden(row);
    res.filtered = isFiltered(row);
    res.hasPageBreak = hasPageBreak(row);
    return res;
}


int RowFormatStorage::lastNonDefaultRow() const
{
    int row = KS_rowMax;
    int firstRow;
    while (row > 0 && isDefaultRow(row, 0, &firstRow)) {
        row = firstRow-1;
    }
    if (row < 1) return 1;
    return row;
}

bool RowFormatStorage::rowsAreEqual(int row1, int row2) const
{
    return rowHeight(row1) == rowHeight(row2)
            && isHidden(row1) == isHidden(row2)
            && isFiltered(row1) == isFiltered(row2)
            && hasPageBreak(row1) == hasPageBreak(row2);
}

bool RowFormatStorage::isDefaultRow(int row, int* lastRow, int* firstRow) const
{
    bool isDef = true;
    int l, f;
    isDef = d->rawRowHeight(row, lastRow, firstRow) == -1 && isDef;
    isDef = !isHiddenOrFiltered(row, &l, &f) && isDef;
    if (lastRow) *lastRow = qMin(*lastRow, l);
    if (firstRow) *firstRow = qMax(*firstRow, f);
    isDef = !hasPageBreak(row, &l, &f) && isDef;
    if (lastRow) *lastRow = qMin(*lastRow, l);
    if (firstRow) *firstRow = qMax(*firstRow, f);
    return isDef;
}

void RowFormatStorage::setDefault(int firstRow, int lastRow)
{
    setRowHeight(firstRow, lastRow, -1);
    setHidden(firstRow, lastRow, false);
    setFiltered(firstRow, lastRow, false);
    setPageBreak(firstRow, lastRow, false);
}

void RowFormatStorage::insertRows(int row, int number)
{
    double deltaHeight = -totalRowHeight(KS_rowMax - number + 1, KS_rowMax);
    d->rowHeights.shift_right(row, number, false);
    deltaHeight += totalRowHeight(row, row + number - 1);
    d->sheet->adjustDocumentHeight(deltaHeight);

    d->hidden.shift_right(row, number, false);
    d->filtered.shift_right(row, number, false);
    d->hasPageBreak.shift_right(row, number, false);
}

void RowFormatStorage::removeRows(int row, int number)
{
    double deltaHeight = -totalRowHeight(row, row + number - 1);
    d->rowHeights.shift_left(row, row+number-1);
    deltaHeight += totalRowHeight(KS_rowMax - number + 1, KS_rowMax);
    d->sheet->adjustDocumentHeight(deltaHeight);

    d->hidden.shift_left(row, row+number-1);
    d->filtered.shift_left(row, row+number-1);
    d->hasPageBreak.shift_left(row, row+number-1);
}

