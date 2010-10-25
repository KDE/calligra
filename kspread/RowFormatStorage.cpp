#include "RowFormatStorage.h"

#include "kspread_limits.h"
#include "mdds/flat_segment_tree.hpp"

#include "Map.h"
#include "RowColumnFormat.h"
#include "Sheet.h"

using namespace KSpread;

class RowFormatStorage::Private
{
public:
    Private();

    Sheet* sheet;
    mdds::flat_segment_tree<int, qreal> rowHeights;
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

qreal RowFormatStorage::rowHeight(int row, int *lastRow, int *firstRow) const
{
    qreal v;
    if (!d->rowHeights.search(row, v, firstRow, lastRow)) {
        if (firstRow) *firstRow = row;
        if (lastRow) *lastRow = row;
        return d->sheet->map()->defaultRowFormat()->height();
    } else {
        if (lastRow) (*lastRow)--;
        if (v == -1) {
            return d->sheet->map()->defaultRowFormat()->height();
        } else {
            return v;
        }
    }
}

void RowFormatStorage::setRowHeight(int firstRow, int lastRow, qreal height)
{
    d->rowHeights.insert_back(firstRow, lastRow+1, height);
}

qreal RowFormatStorage::totalRowHeight(int firstRow, int lastRow) const
{
    if (lastRow < firstRow) return 0;
    // TODO: better implementation
    qreal res = 0;
    for (int row = firstRow; row <= lastRow; ++row) {
        res += rowHeight(row);
    }
    return res;
}

qreal RowFormatStorage::visibleHeight(int row, int *lastRow, int *firstRow) const
{
    if (isHiddenOrFiltered(row, lastRow, firstRow)) {
        return 0.0;
    } else {
        int hLastRow, hFirstRow;
        qreal height = rowHeight(row, &hLastRow, &hFirstRow);
        if (lastRow) *lastRow = qMin(*lastRow, hLastRow);
        if (firstRow) *firstRow = qMax(*firstRow, hFirstRow);
        return height;
    }
}

qreal RowFormatStorage::totalVisibleRowHeight(int firstRow, int lastRow) const
{
    if (lastRow < firstRow) return 0;
    // TODO: better implementation
    qreal res = 0;
    for (int row = firstRow; row <= lastRow; ++row) {
        res += visibleHeight(row);
    }
    return res;
}

bool RowFormatStorage::isHidden(int row, int *lastRow, int *firstRow) const
{
    bool v;
    if (!d->hidden.search(row, v, firstRow, lastRow)) {
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
    d->hidden.insert_back(firstRow, lastRow+1, hidden);
}

bool RowFormatStorage::isFiltered(int row, int* lastRow, int *firstRow) const
{
    bool v;
    if (!d->filtered.search(row, v, firstRow, lastRow)) {
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
    d->filtered.insert_back(firstRow, lastRow+1, filtered);
}

bool RowFormatStorage::isHiddenOrFiltered(int row, int* lastRow, int* firstRow) const
{
    int hLastRow, hFirstRow, fLastRow, fFirstRow;
    bool v = isHidden(row, &hLastRow, &hFirstRow);
    v = v || isFiltered(row, &fLastRow, &fFirstRow);
    if (lastRow) *lastRow = qMin(hLastRow, fLastRow);
    if (firstRow) *firstRow = qMax(hFirstRow, fFirstRow);
    return v;
}

bool RowFormatStorage::hasPageBreak(int row, int* lastRow, int* firstRow) const
{
    bool v;
    if (!d->hasPageBreak.search(row, v, firstRow, lastRow)) {
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
    d->hasPageBreak.insert_back(firstRow, lastRow, pageBreak);
}

int RowFormatStorage::lastNonDefaultRow() const
{
    // TODO
    return KS_rowMax;
}

bool RowFormatStorage::rowsAreEqual(int row1, int row2) const
{
    // TODO
    return false;
}

bool RowFormatStorage::isDefaultRow(int row, int* lastRow, int* firstRow) const
{
    // TODO
    return false;
}

void RowFormatStorage::setDefault(int firstRow, int lastRow)
{
    // TODO
}

void RowFormatStorage::insertRows(int row, int number)
{
    qreal deltaHeight = -totalRowHeight(KS_rowMax - number + 1, KS_rowMax);
    d->rowHeights.shift_right(row, number, false);
    deltaHeight += totalRowHeight(row, row + number - 1);
    d->sheet->adjustDocumentHeight(deltaHeight);

    d->hidden.shift_right(row, number, false);
    d->filtered.shift_right(row, number, false);
    d->hasPageBreak.shift_right(row, number, false);
}

void RowFormatStorage::removeRows(int row, int number)
{
    qreal deltaHeight = -totalRowHeight(row, row + number - 1);
    d->rowHeights.shift_left(row, row+number-1);
    deltaHeight += totalRowHeight(KS_rowMax - number + 1, KS_rowMax);
    d->sheet->adjustDocumentHeight(deltaHeight);

    d->hidden.shift_left(row, row+number-1);
    d->filtered.shift_left(row, row+number-1);
    d->hasPageBreak.shift_left(row, row+number-1);
}

