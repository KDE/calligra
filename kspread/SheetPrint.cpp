/* This file is part of the KDE project
   Copyright 2007, 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2005 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
   Copyright 2003 Philipp Müller <philipp.mueller@gmx.de>
   Copyright 1998, 1999 Torben Weis <weis@kde.org>

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
   Boston, MA 02110-1301, USA.
*/

// Local
#include "SheetPrint.h"
#include "SheetPrint_p.h"

#include "HeaderFooter.h"
#include "PrintSettings.h"
#include "kspread_limits.h"
#include "Region.h"
#include "Sheet.h"

#include <KDebug>
#include <KMessageBox>

using namespace KSpread;

SheetPrint::SheetPrint(Sheet *sheet)
        : d(new Private(this))
{
    d->m_pSheet = sheet;

    d->m_settings = new PrintSettings();
    d->m_headerFooter = new HeaderFooter(sheet);

    d->m_maxCheckedNewPageX = 0;
    d->m_maxCheckedNewPageY = 0;
    d->m_dPrintRepeatColumnsWidth = 0.0;
    d->m_dPrintRepeatRowsHeight = 0.0;
}

SheetPrint::SheetPrint(const SheetPrint &other)
        : d(new Private(this))
{
    d->m_pSheet = other.d->m_pSheet;

    d->m_settings = new PrintSettings(*other.d->m_settings);
    d->m_headerFooter = new HeaderFooter(*other.d->m_headerFooter);

    d->m_maxCheckedNewPageX = other.d->m_maxCheckedNewPageX;
    d->m_maxCheckedNewPageY = other.d->m_maxCheckedNewPageY;
    d->m_dPrintRepeatColumnsWidth = other.d->m_dPrintRepeatColumnsWidth;
    d->m_dPrintRepeatRowsHeight = other.d->m_dPrintRepeatRowsHeight;
    d->m_lnewPageListX = other.d->m_lnewPageListX;
    d->m_lnewPageListY = other.d->m_lnewPageListY;
}

SheetPrint::~SheetPrint()
{
    delete d->m_headerFooter;
    delete d->m_settings;
    delete d;
}

PrintSettings *SheetPrint::settings() const
{
    return d->m_settings;
}

void SheetPrint::setSettings(const PrintSettings &settings, bool force)
{
    // Relayout forced?
    if (force) {
        *d->m_settings = settings;
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

    const KoPageLayout oldPageLayout = d->m_settings->pageLayout();
    const KoPageLayout pageLayout = settings.pageLayout();
    const QRect oldPrintRange = d->m_settings->printRegion().lastRange();
    const QRect printRange = settings.printRegion().lastRange();
    const QSize oldPageLimits = d->m_settings->pageLimits();
    const QSize pageLimits = settings.pageLimits();
    const QPair<int, int> oldRepeatedColumns = d->m_settings->repeatedColumns();
    const QPair<int, int> repeatedColumns = settings.repeatedColumns();
    const QPair<int, int> oldRepeatedRows = d->m_settings->repeatedRows();
    const QPair<int, int> repeatedRows = settings.repeatedRows();

    const bool pageWidthChanged = oldPageLayout.width != pageLayout.width;
    const bool pageHeightChanged = oldPageLayout.height != pageLayout.height;
    const bool horizontalLimitChanged = oldPageLimits.width() != pageLimits.width();
    const bool verticalLimitChanged = oldPageLimits.height() != pageLimits.height();
    const bool repeatedColumnsChanged = oldRepeatedColumns != repeatedColumns;
    const bool repeatedRowsChanged = oldRepeatedRows != repeatedRows;
    const bool zoomChanged = d->m_settings->zoom() != settings.zoom();

    *d->m_settings = settings;

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

HeaderFooter *SheetPrint::headerFooter() const
{
    return d->m_headerFooter;
}

bool SheetPrint::isColumnOnNewPage(int _column)
{
    if (_column > d->m_maxCheckedNewPageX)
        d->calculateHorizontalPageParameters(_column);

    //Are these the edges of the print range?
    const QRect printRange = d->m_settings->printRegion().lastRange();
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
    const QRect printRange = d->m_settings->printRegion().lastRange();
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
    const QRect printRange = d->m_settings->printRegion().lastRange();
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
    if (_col <= d->m_settings->repeatedColumns().second) {
        d->updateRepeatedColumnsWidth();
    }
}

void SheetPrint::updateVerticalPageParameters(int _row)
{
    //If the new range is after the first entry, we need to delete the whole list
    const QRect printRange = d->m_settings->printRegion().lastRange();
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
    if (_row <= d->m_settings->repeatedRows().second) {
        d->updateRepeatedRowsHeight();
    }
}

void SheetPrint::insertColumn(int col, int nbCol)
{
    //update print range, when it has been defined
    const QRect printRange = d->m_settings->printRegion().lastRange();
    if (printRange != QRect(QPoint(1, 1), QPoint(KS_colMax, KS_rowMax))) {
        int left = printRange.left();
        int right = printRange.right();

        for (int i = 0; i < nbCol; i++) {
            if (left >= col) left++;
            if (right >= col) right++;
        }
        //Validity checks
        if (left > KS_colMax) left = KS_colMax;
        if (right > KS_colMax) right = KS_colMax;
        const Region region(QRect(QPoint(left, printRange.top()),
                                  QPoint(right, printRange.bottom())), d->m_pSheet);
        // Trigger an update by setting it indirectly.
        PrintSettings settings = *d->m_settings;
        settings.setPrintRegion(region);
        setSettings(settings);
    }
}

void SheetPrint::insertRow(int row, int nbRow)
{
    //update print range, when it has been defined
    const QRect printRange = d->m_settings->printRegion().lastRange();
    if (printRange != QRect(QPoint(1, 1), QPoint(KS_colMax, KS_rowMax))) {
        int top = printRange.top();
        int bottom = printRange.bottom();

        for (int i = 0; i < nbRow; i++) {
            if (top >= row) top++;
            if (bottom >= row) bottom++;
        }
        //Validity checks
        if (top > KS_rowMax) top = KS_rowMax;
        if (bottom > KS_rowMax) bottom = KS_rowMax;
        const Region region(QRect(QPoint(printRange.left(), top),
                                  QPoint(printRange.right(), bottom)), d->m_pSheet);
        // Trigger an update by setting it indirectly.
        PrintSettings settings = *d->m_settings;
        settings.setPrintRegion(region);
        setSettings(settings);
    }
}

void SheetPrint::removeColumn(int col, int nbCol)
{
    PrintSettings settings = *d->m_settings;
    //update print range, when it has been defined
    const QRect printRange = d->m_settings->printRegion().lastRange();
    if (printRange != QRect(QPoint(1, 1), QPoint(KS_colMax, KS_rowMax))) {
        int left = printRange.left();
        int right = printRange.right();

        for (int i = 0; i < nbCol; i++) {
            if (left > col) left--;
            if (right >= col) right--;
        }
        //Validity checks
        if (left < 1) left = 1;
        if (right < 1) right = 1;
        const Region region(QRect(QPoint(left, printRange.top()),
                                  QPoint(right, printRange.bottom())), d->m_pSheet);
        settings.setPrintRegion(region);
    }

    //update repeat columns, when it has been defined
    const QPair<int, int> repeatedColumns = d->m_settings->repeatedColumns();
    if (repeatedColumns.first != 0) {
        int left = repeatedColumns.first;
        int right = repeatedColumns.second;

        for (int i = 0; i < nbCol; i++) {
            if (left > col) left--;
            if (right >= col) right--;
        }
        //Validity checks
        if (left < 1) left = 1;
        if (right < 1) right = 1;
        settings.setRepeatedColumns(qMakePair(left, right));
    }
    // Trigger an update by setting them indirectly.
    setSettings(settings);
}

void SheetPrint::removeRow(int row, int nbRow)
{
    PrintSettings settings = *d->m_settings;
    //update print range, when it has been defined
    const QRect printRange = d->m_settings->printRegion().lastRange();
    if (printRange != QRect(QPoint(1, 1), QPoint(KS_colMax, KS_rowMax))) {
        int top = printRange.top();
        int bottom = printRange.bottom();

        for (int i = 0; i < nbRow; i++) {
            if (top > row) top--;
            if (bottom >= row) bottom--;
        }
        //Validity checks
        if (top < 1) top = 1;
        if (bottom < 1) bottom = 1;
        const Region region(QRect(QPoint(printRange.left(), top),
                                  QPoint(printRange.right(), bottom)), d->m_pSheet);
        settings.setPrintRegion(region);
    }

    //update repeat rows, when it has been defined
    const QPair<int, int> repeatedRows = d->m_settings->repeatedRows();
    if (repeatedRows.first != 0) {
        int top = repeatedRows.first;
        int bottom = repeatedRows.second;

        for (int i = 0; i < nbRow; i++) {
            if (top > row) top--;
            if (bottom >= row) bottom--;
        }
        //Validity checks
        if (top < 1) top = 1;
        if (bottom < 1) bottom = 1;
        settings.setRepeatedRows(qMakePair(top, bottom));
    }
    // Trigger an update by setting them indirectly.
    setSettings(settings);
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
    kDebug() << "page:" << page << "of" << pageCount();

    int horizontalIndex = 0;
    int verticalIndex = 0;
    if (d->m_settings->pageOrder() == PrintSettings::LeftToRight) {
        horizontalIndex = (page - 1) % d->m_lnewPageListX.count();
        verticalIndex = (page - 1) / d->m_lnewPageListX.count();
    } else {
        horizontalIndex = (page - 1) / d->m_lnewPageListY.count();
        verticalIndex = (page - 1) % d->m_lnewPageListY.count();
    }
    kDebug() << "horizontal:" << horizontalIndex + 1 << "of" << d->m_lnewPageListX.count();
    kDebug() << "vertical:" << verticalIndex + 1 << "of" << d->m_lnewPageListY.count();

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
    if (d->m_settings->pageOrder() == PrintSettings::LeftToRight) {
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

    *d->m_settings = *other.d->m_settings;
    *d->m_headerFooter = *other.d->m_headerFooter;

    d->m_maxCheckedNewPageX = other.d->m_maxCheckedNewPageX;
    d->m_maxCheckedNewPageY = other.d->m_maxCheckedNewPageY;
    d->m_dPrintRepeatColumnsWidth = other.d->m_dPrintRepeatColumnsWidth;
    d->m_dPrintRepeatRowsHeight = other.d->m_dPrintRepeatRowsHeight;
    d->m_lnewPageListX = other.d->m_lnewPageListX;
    d->m_lnewPageListY = other.d->m_lnewPageListY;
}
