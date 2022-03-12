/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007, 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2005 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
   SPDX-FileCopyrightText: 2003 Philipp Müller <philipp.mueller@gmx.de>
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "SheetPrint.h"
#include "SheetPrint_p.h"

#include "PrintSettings.h"
#include "calligra_sheets_limits.h"
#include "Region.h"
#include "Sheet.h"
#include "SheetsDebug.h"

using namespace Calligra::Sheets;

SheetPrint::SheetPrint(Sheet *sheet)
        : QObject(nullptr),
        d(new Private(this))
{
    d->m_pSheet = sheet;

    d->m_maxCheckedNewPageX = 0;
    d->m_maxCheckedNewPageY = 0;
    d->m_dPrintRepeatColumnsWidth = 0.0;
    d->m_dPrintRepeatRowsHeight = 0.0;

    connect(sheet, &Sheet::columnsAdded, this, &SheetPrint::insertColumn);
    connect(sheet, &Sheet::rowsAdded, this, &SheetPrint::insertRow);
    connect(sheet, &Sheet::columnsRemoved, this, &SheetPrint::removeColumn);
    connect(sheet, &Sheet::rowsRemoved, this, &SheetPrint::removeRow);
}

SheetPrint::SheetPrint(const SheetPrint &other)
        : QObject(nullptr),
        d(new Private(this))
{
    d->m_pSheet = other.d->m_pSheet;

    d->m_maxCheckedNewPageX = other.d->m_maxCheckedNewPageX;
    d->m_maxCheckedNewPageY = other.d->m_maxCheckedNewPageY;
    d->m_dPrintRepeatColumnsWidth = other.d->m_dPrintRepeatColumnsWidth;
    d->m_dPrintRepeatRowsHeight = other.d->m_dPrintRepeatRowsHeight;
    d->m_lnewPageListX = other.d->m_lnewPageListX;
    d->m_lnewPageListY = other.d->m_lnewPageListY;

    connect(d->m_pSheet, &Sheet::columnsAdded, this, &SheetPrint::insertColumn);
    connect(d->m_pSheet, &Sheet::rowsAdded, this, &SheetPrint::insertRow);
    connect(d->m_pSheet, &Sheet::columnsRemoved, this, &SheetPrint::removeColumn);
    connect(d->m_pSheet, &Sheet::rowsRemoved, this, &SheetPrint::removeRow);
}

SheetPrint::~SheetPrint()
{
    delete d;
}

// TODO - this should all be done differently?
void SheetPrint::setSettings(const PrintSettings &settings, bool force)
{
    // Relayout forced?
    if (force) {
        d->updateRepeatedColumnsWidth();
        d->updateRepeatedRowsHeight();
        const QSize pageLimits = settings.pageLimits();
        const QSize usedArea = d->m_pSheet->usedArea(true).size();
        if (pageLimits.width() > 0) {
            d->calculateZoomForPageLimitX();
        } else {
            updateHorizontalPageParameters(0);
            d->calculateHorizontalPageParameters(usedArea.width());
        }
        if (pageLimits.height() > 0) {
            d->calculateZoomForPageLimitY();
        } else {
            updateVerticalPageParameters(0);
            d->calculateVerticalPageParameters(usedArea.height());
        }
        return;
    }

    PrintSettings *sett = d->m_pSheet->printSettings();
    const KoPageLayout oldPageLayout = sett->pageLayout();
    const KoPageLayout pageLayout = settings.pageLayout();
    const QRect oldPrintRange = sett->printRegion().lastRange();
    const QRect printRange = settings.printRegion().lastRange();
    const QSize oldPageLimits = sett->pageLimits();
    const QSize pageLimits = settings.pageLimits();
    const QPair<int, int> oldRepeatedColumns = sett->repeatedColumns();
    const QPair<int, int> repeatedColumns = settings.repeatedColumns();
    const QPair<int, int> oldRepeatedRows = sett->repeatedRows();
    const QPair<int, int> repeatedRows = settings.repeatedRows();

    const bool pageWidthChanged = oldPageLayout.width != pageLayout.width;
    const bool pageHeightChanged = oldPageLayout.height != pageLayout.height;
    const bool horizontalLimitChanged = oldPageLimits.width() != pageLimits.width();
    const bool verticalLimitChanged = oldPageLimits.height() != pageLimits.height();
    const bool repeatedColumnsChanged = oldRepeatedColumns != repeatedColumns;
    const bool repeatedRowsChanged = oldRepeatedRows != repeatedRows;
    const bool zoomChanged = sett->zoom() != settings.zoom();

    // The starting column/row for the page parameter updates.
    int column = KS_colMax + 1;
    int row = KS_rowMax + 1;

    // The print range.
    if (oldPrintRange.left() != printRange.left()) {
        column = qMin(oldPrintRange.left(), printRange.left());
    }
    if (oldPrintRange.top() != printRange.top()) {
        row = qMin(oldPrintRange.top(), printRange.top());
    }

    // The zoom.
    if (zoomChanged) {
        column = 0;
        row = 0;
    }

    // The page limits.
    if (horizontalLimitChanged && pageLimits.width() <= 0) {
        column = 0;
    }
    if (verticalLimitChanged && pageLimits.height() <= 0) {
        row = 0;
    }

    // The page dimensions.
    if (pageWidthChanged) {
        column = 0;
    }
    if (pageHeightChanged) {
        row = 0;
    }

    // The column/row repetitions.
    if (repeatedColumnsChanged) {
        d->updateRepeatedColumnsWidth();
        column = qMin(column, oldRepeatedColumns.first);
        column = qMin(column, repeatedColumns.first);
    }
    if (repeatedRowsChanged) {
        d->updateRepeatedRowsHeight();
        row = qMin(row, oldRepeatedRows.first);
        row = qMin(row, repeatedRows.first);
    }

    // Update the page parameters.
    // If page limits are set to non-zero, call the special methods.
    if (horizontalLimitChanged && pageLimits.width() > 0) {
        d->calculateZoomForPageLimitX();
    } else if (column <= KS_colMax) {
        updateHorizontalPageParameters(column);
    }
    if (verticalLimitChanged && pageLimits.height() > 0) {
        d->calculateZoomForPageLimitY();
    } else if (row <= KS_rowMax) {
        updateVerticalPageParameters(row);
    }
}

bool SheetPrint::isColumnOnNewPage(int _column)
{
    if (_column > d->m_maxCheckedNewPageX)
        d->calculateHorizontalPageParameters(_column);

    //Are these the edges of the print range?
    const QRect printRange = d->m_pSheet->printSettings()->printRegion().lastRange();
    if (_column == printRange.left() || _column == printRange.right()) {
        return true;
    }

    //beyond the print range it's always false
    if (_column < printRange.left() || _column > printRange.right()) {
        return false;
    }

    //Now check if we find the column already in the list
    if (d->m_lnewPageListX.indexOf(PrintNewPageEntry(_column)) != -1) {
        if (_column > d->m_maxCheckedNewPageX)
            d->m_maxCheckedNewPageX = _column;
        return true;
    }
    return false;
}



bool SheetPrint::isRowOnNewPage(int _row)
{
    if (_row > d->m_maxCheckedNewPageY)
        d->calculateVerticalPageParameters(_row);

    //Are these the edges of the print range?
    const QRect printRange = d->m_pSheet->printSettings()->printRegion().lastRange();
    if (_row == printRange.top() || _row == printRange.bottom()) {
        return true;
    }

    //beyond the print range it's always false
    if (_row < printRange.top() || _row > printRange.bottom()) {
        return false;
    }

    //Now check if we find the row already in the list
    if (d->m_lnewPageListY.indexOf(PrintNewPageEntry(_row)) != -1) {
        if (_row > d->m_maxCheckedNewPageY)
            d->m_maxCheckedNewPageY = _row;
        return true;
    }

    return false;
}

void SheetPrint::updateHorizontalPageParameters(int _col)
{
    //If the new range is after the first entry, we need to delete the whole list
    const QRect printRange = d->m_pSheet->printSettings()->printRegion().lastRange();
    if (d->m_lnewPageListX.isEmpty() || d->m_lnewPageListX.first().startItem() != printRange.left() || _col == 0) {
        d->m_lnewPageListX.clear();
        d->m_maxCheckedNewPageX = 0;
        d->updateRepeatedColumnsWidth();
        return;
    }

    if (_col <= d->m_lnewPageListX.last().endItem()) {
        // Find the page entry for this column
        int index = d->m_lnewPageListX.count() - 1;
        while (_col < d->m_lnewPageListX[index].startItem()) {
            --index;
        }

        //Remove later pages
        while (index != d->m_lnewPageListX.count())
            d->m_lnewPageListX.removeAt(index);

        d->m_maxCheckedNewPageX = d->m_lnewPageListX.isEmpty() ? 0 : d->m_lnewPageListX.last().endItem();
    }

    // The column is not beyond the repeated columns?
    if (_col <= d->m_pSheet->printSettings()->repeatedColumns().second) {
        d->updateRepeatedColumnsWidth();
    }
}

void SheetPrint::updateVerticalPageParameters(int _row)
{
    //If the new range is after the first entry, we need to delete the whole list
    const QRect printRange = d->m_pSheet->printSettings()->printRegion().lastRange();
    if (d->m_lnewPageListY.isEmpty() || d->m_lnewPageListY.first().startItem() != printRange.top() || _row == 0) {
        d->m_lnewPageListY.clear();
        d->m_maxCheckedNewPageY = 0;
        d->updateRepeatedRowsHeight();
        return;
    }

    if (_row <= d->m_lnewPageListY.last().endItem()) {
        // Find the page entry for this row
        int index = d->m_lnewPageListY.count() - 1;
        while (_row < d->m_lnewPageListY[index].startItem()) {
            --index;
        }

        //Remove later pages
        while (index != d->m_lnewPageListY.count())
            d->m_lnewPageListY.removeAt(index);

        d->m_maxCheckedNewPageY = d->m_lnewPageListY.isEmpty() ? 0 : d->m_lnewPageListY.last().endItem();
    }

    // The row is not beyond the repeated rows?
    if (_row <= d->m_pSheet->printSettings()->repeatedRows().second) {
        d->updateRepeatedRowsHeight();
    }
}

// helper func for (insert/remove)(Column/Row)
static void updateRowCol (int &v1, int &v2, int start, int len)
{
    if (v1 >= start) {
        v1 += len;
        if (v1 < start) v1 = start;
        if (v1 > KS_colMax) v1 = KS_colMax;
    }
    if (v2 >= start) {
        v2 += len;
        if (v2 < start) v2 = start - 1;
        if (v2 > KS_colMax) v2 = KS_colMax;
    }
}

void SheetPrint::insertColumn(int col, int nbCol)
{
    //update print range, when it has been defined
    const QRect printRange = d->m_pSheet->printSettings()->printRegion().lastRange();
    if (printRange != QRect(QPoint(1, 1), QPoint(KS_colMax, KS_rowMax))) {
        int left = printRange.left();
        int right = printRange.right();

        updateRowCol(left, right, col, nbCol);
        const Region region(QRect(QPoint(left, printRange.top()),
                                  QPoint(right, printRange.bottom())), d->m_pSheet);
        // Trigger an update by setting it indirectly.
        PrintSettings *settings = d->m_pSheet->printSettings();
        settings->setPrintRegion(region);
        d->m_pSheet->setPrintSettings(*settings);
    }
}

void SheetPrint::insertRow(int row, int nbRow)
{
    //update print range, when it has been defined
    const QRect printRange = d->m_pSheet->printSettings()->printRegion().lastRange();
    if (printRange != QRect(QPoint(1, 1), QPoint(KS_colMax, KS_rowMax))) {
        int top = printRange.top();
        int bottom = printRange.bottom();

        updateRowCol(top, bottom, row, nbRow);
        const Region region(QRect(QPoint(printRange.left(), top),
                                  QPoint(printRange.right(), bottom)), d->m_pSheet);
        // Trigger an update by setting it indirectly.
        PrintSettings *settings = d->m_pSheet->printSettings();
        settings->setPrintRegion(region);
        d->m_pSheet->setPrintSettings(*settings);
    }
}

void SheetPrint::removeColumn(int col, int nbCol)
{
    PrintSettings *settings = d->m_pSheet->printSettings();
    //update print range, when it has been defined
    const QRect printRange = settings->printRegion().lastRange();
    if (printRange != QRect(QPoint(1, 1), QPoint(KS_colMax, KS_rowMax))) {
        int left = printRange.left();
        int right = printRange.right();

        updateRowCol(left, right, col, nbCol);
        const Region region(QRect(QPoint(left, printRange.top()),
                                  QPoint(right, printRange.bottom())), d->m_pSheet);
        settings->setPrintRegion(region);
    }

    //update repeat columns, when it has been defined
    const QPair<int, int> repeatedColumns = settings->repeatedColumns();
    if (repeatedColumns.first) {
        int left = repeatedColumns.first;
        int right = repeatedColumns.second;

        updateRowCol(left, right, col, nbCol);
        settings->setRepeatedColumns(qMakePair(left, right));
    }
    // Trigger an update by setting them indirectly.
    d->m_pSheet->setPrintSettings(*settings);
}

void SheetPrint::removeRow(int row, int nbRow)
{
    PrintSettings *settings = d->m_pSheet->printSettings();
    //update print range, when it has been defined
    const QRect printRange = settings->printRegion().lastRange();
    if (printRange != QRect(QPoint(1, 1), QPoint(KS_colMax, KS_rowMax))) {
        int top = printRange.top();
        int bottom = printRange.bottom();

        updateRowCol(top, bottom, row, nbRow);
        const Region region(QRect(QPoint(printRange.left(), top),
                                  QPoint(printRange.right(), bottom)), d->m_pSheet);
        settings->setPrintRegion(region);
    }

    //update repeat rows, when it has been defined
    const QPair<int, int> repeatedRows = settings->repeatedRows();
    if (repeatedRows.first) {
        int top = repeatedRows.first;
        int bottom = repeatedRows.second;

        updateRowCol(top, bottom, row, nbRow);
        settings->setRepeatedRows(qMakePair(top, bottom));
    }
    // Trigger an update by setting them indirectly.
    d->m_pSheet->setPrintSettings(*settings);
}

int SheetPrint::pageCount() const
{
    return d->m_lnewPageListX.count() * d->m_lnewPageListY.count();
}

QRect SheetPrint::cellRange(int page) const
{
    if (d->m_lnewPageListX.isEmpty() || d->m_lnewPageListY.isEmpty()) {
        return QRect();
    }
    if (page - 1 > pageCount()) {
        return QRect();
    }
    debugSheets << "page:" << page << "of" << pageCount();

    int horizontalIndex = 0;
    int verticalIndex = 0;
    if (d->m_pSheet->printSettings()->pageOrder() == PrintSettings::LeftToRight) {
        horizontalIndex = (page - 1) % d->m_lnewPageListX.count();
        verticalIndex = (page - 1) / d->m_lnewPageListX.count();
    } else {
        horizontalIndex = (page - 1) / d->m_lnewPageListY.count();
        verticalIndex = (page - 1) % d->m_lnewPageListY.count();
    }
    debugSheets << "horizontal:" << horizontalIndex + 1 << "of" << d->m_lnewPageListX.count();
    debugSheets << "vertical:" << verticalIndex + 1 << "of" << d->m_lnewPageListY.count();

    const PrintNewPageEntry horizontalParameters = d->m_lnewPageListX[horizontalIndex];
    const PrintNewPageEntry verticalParameters = d->m_lnewPageListY[verticalIndex];

    QRect cellRange;
    cellRange.setLeft(horizontalParameters.startItem());
    cellRange.setRight(horizontalParameters.endItem());
    cellRange.setTop(verticalParameters.startItem());
    cellRange.setBottom(verticalParameters.endItem());
    return cellRange;
}

QRectF SheetPrint::documentArea(int page) const
{
    if (d->m_lnewPageListX.isEmpty() || d->m_lnewPageListY.isEmpty()) {
        return QRectF();
    }
    if (page - 1 > pageCount()) {
        return QRectF();
    }

    int horizontalIndex = 0;
    int verticalIndex = 0;
    if (d->m_pSheet->printSettings()->pageOrder() == PrintSettings::LeftToRight) {
        horizontalIndex = (page - 1) % d->m_lnewPageListX.count();
        verticalIndex = (page - 1) / d->m_lnewPageListX.count();
    } else {
        horizontalIndex = (page - 1) / d->m_lnewPageListY.count();
        verticalIndex = (page - 1) % d->m_lnewPageListY.count();
    }

    const PrintNewPageEntry horizontalParameters = d->m_lnewPageListX[horizontalIndex];
    const PrintNewPageEntry verticalParameters = d->m_lnewPageListY[verticalIndex];

    QRectF documentArea;
    documentArea.setLeft(horizontalParameters.offset());
    documentArea.setWidth(horizontalParameters.size());
    documentArea.setTop(verticalParameters.offset());
    documentArea.setHeight(verticalParameters.size());
    return documentArea;
}

void SheetPrint::operator=(const SheetPrint & other)
{
    d->m_pSheet = other.d->m_pSheet;

    d->m_maxCheckedNewPageX = other.d->m_maxCheckedNewPageX;
    d->m_maxCheckedNewPageY = other.d->m_maxCheckedNewPageY;
    d->m_dPrintRepeatColumnsWidth = other.d->m_dPrintRepeatColumnsWidth;
    d->m_dPrintRepeatRowsHeight = other.d->m_dPrintRepeatRowsHeight;
    d->m_lnewPageListX = other.d->m_lnewPageListX;
    d->m_lnewPageListY = other.d->m_lnewPageListY;
}
