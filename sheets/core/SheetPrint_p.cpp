/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007, 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2005 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
   SPDX-FileCopyrightText: 2003 Philipp Müller <philipp.mueller@gmx.de>
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "SheetPrint_p.h"

#include "engine/Region.h"

#include "ColFormatStorage.h"
#include "PrintSettings.h"
#include "RowFormatStorage.h"
#include "Sheet.h"

using namespace Calligra::Sheets;

void SheetPrint::Private::calculateHorizontalPageParameters(int _column)
{
    PrintSettings *settings = m_pSheet->printSettings();
    // Zoom the print width ONCE here, instead for each column width.
    const double printWidth = settings->printWidth() / settings->zoom();

    float offset = 0.0;

    // Are these the edges of the print range?
    const QRect printRange = settings->printRegion().lastRange();
#if 0
    if (_column == printRange.left() || _column == printRange.right()) {
        if (_column > m_maxCheckedNewPageX)
            m_maxCheckedNewPageX = _column;
        return;
    }

    //We don't check beyond the print range
    if (_column < printRange.left() || _column > printRange.right()) {
        if (_column > m_maxCheckedNewPageX)
            m_maxCheckedNewPageX = _column;
        if (_column > printRange.right()) {
            if (m_lnewPageListX.last().endItem() == 0)
                m_lnewPageListX.last().setEndItem(printRange.right());
        }
        return;
    }
#endif

    // Check if the pre-calculated width matches the repeated columns setting.
    const bool repetitions = settings->repeatedColumns().first != 0;
    if (repetitions != (m_dPrintRepeatColumnsWidth == 0.0)) {
        // Existing column repetitions, but their pre-calculated width is zero?
        // Must be the first run. Or the other way around? Seem to be orphaned.
        // Either way, seems they do not match. Calculate them!
        updateRepeatedColumnsWidth();
    }

    // The end of the last item (zero, if list is empty).
    const int end = m_lnewPageListX.empty() ? 0 : m_lnewPageListX.last().endItem();

    // If _column is greater than the last entry, we need to calculate the result
    if (_column > end && _column > m_maxCheckedNewPageX) { // this columns hasn't been calculated before
        int startCol = end + 1;
        int col = startCol;
        double x = m_pSheet->columnFormats()->colWidth(col);

        // Add a new page.
        m_lnewPageListX.append(PrintNewPageEntry(startCol));

        // Add repeated column width, when necessary
        const QPair<int, int> repeatedColumns = settings->repeatedColumns();
        if (col > repeatedColumns.first) {
            x += m_dPrintRepeatColumnsWidth;
            offset = m_dPrintRepeatColumnsWidth;
        }
        debugSheets << "startCol:" << startCol << "col:" << col << "x:" << x << "offset:" << offset << repeatedColumns;

        while ((col <= _column) && (col < printRange.right())) {
            debugSheets << "loop:"
                        << "startCol:" << startCol << "col:" << col << "x:" << x << "offset:" << offset;
            // end of page?
            if (x > printWidth || m_pSheet->columnFormats()->hasPageBreak(col)) {
                // Now store into the previous entry the enditem and the width
                m_lnewPageListX.last().setEndItem(col - 1);
                m_lnewPageListX.last().setSize(x - m_pSheet->columnFormats()->colWidth(col));
                m_lnewPageListX.last().setOffset(offset);

                // start a new page
                m_lnewPageListX.append(PrintNewPageEntry(col));
                startCol = col;
                x = m_pSheet->columnFormats()->colWidth(col);
                if (col >= repeatedColumns.first) {
                    debugSheets << "col >= repeatedColumns.first:" << col << repeatedColumns.first;
                    x += m_dPrintRepeatColumnsWidth;
                    offset = m_dPrintRepeatColumnsWidth;
                }
            }
            col++;
            x += m_pSheet->columnFormats()->colWidth(col);
        }

        // Iterate to the end of the page.
        while (m_lnewPageListX.last().endItem() == 0) {
            debugSheets << "loop to end"
                        << "col:" << col << "x:" << x << "offset:" << offset << "m_maxCheckedNewPageX:" << m_maxCheckedNewPageX;
            if (x > printWidth || m_pSheet->columnFormats()->hasPageBreak(col)) {
                // Now store into the previous entry the enditem and the width
                m_lnewPageListX.last().setEndItem(col - 1);
                m_lnewPageListX.last().setSize(x - m_pSheet->columnFormats()->colWidth(col));
                m_lnewPageListX.last().setOffset(offset);

                if (col - 1 > m_maxCheckedNewPageX) {
                    m_maxCheckedNewPageX = col - 1;
                }
                return;
            }
            ++col;
            x += m_pSheet->columnFormats()->colWidth(col);
        }
    }

    debugSheets << "m_maxCheckedNewPageX:" << m_maxCheckedNewPageX;
    if (_column > m_maxCheckedNewPageX) {
        m_maxCheckedNewPageX = _column;
        m_lnewPageListX.last().setEndItem(_column);
    }
}

void SheetPrint::Private::calculateVerticalPageParameters(int _row)
{
    PrintSettings *settings = m_pSheet->printSettings();
    // Zoom the print height ONCE here, instead for each row height.
    const double printHeight = settings->printHeight() / settings->zoom();

    float offset = 0.0;

    // Are these the edges of the print range?
    const QRect printRange = settings->printRegion().lastRange();
#if 0
    if (_row == printRange.top() || _row == printRange.bottom()) {
        if (_row > m_maxCheckedNewPageY)
            m_maxCheckedNewPageY = _row;
        return;
    }

    //beyond the print range it's always false
    if (_row < printRange.top() || _row > printRange.bottom()) {
        if (_row > m_maxCheckedNewPageY)
            m_maxCheckedNewPageY = _row;
        if (_row > printRange.bottom()) {
            if (m_lnewPageListY.last().endItem() == 0)
                m_lnewPageListY.last().setEndItem(printRange.bottom());
        }
        return;
    }
#endif

    // Check if the pre-calculated height matches the repeated rows setting.
    const bool repetitions = settings->repeatedRows().first != 0;
    if (repetitions != (m_dPrintRepeatRowsHeight == 0.0)) {
        // Existing row repetitions, but their pre-calculated height is zero?
        // Must be the first run. Or the other way around? Seem to be orphaned.
        // Either way, seems they do not match. Calculate them!
        updateRepeatedRowsHeight();
    }

    // The end of the last item (zero, if list is empty).
    const int end = m_lnewPageListY.empty() ? 0 : m_lnewPageListY.last().endItem();

    // If _column is greater than the last entry, we need to calculate the result
    if (_row > end && _row > m_maxCheckedNewPageY) { // this columns hasn't been calculated before
        int startRow = end + 1;
        int row = startRow;
        double y = m_pSheet->rowFormats()->rowHeight(row);

        // Add a new page.
        m_lnewPageListY.append(PrintNewPageEntry(startRow));

        // Add repeated row height, when necessary
        const QPair<int, int> repeatedRows = settings->repeatedRows();
        if (row > repeatedRows.first) {
            y += m_dPrintRepeatRowsHeight;
            offset = m_dPrintRepeatRowsHeight;
        }

        while ((row <= _row) && (row < printRange.bottom())) {
            // end of page?
            if (y > printHeight || m_pSheet->rowFormats()->hasPageBreak(row)) {
                // Now store into the previous entry the enditem and the width
                m_lnewPageListY.last().setEndItem(row - 1);
                m_lnewPageListY.last().setSize(y - m_pSheet->rowFormats()->rowHeight(row));
                m_lnewPageListY.last().setOffset(offset);

                // start a new page
                m_lnewPageListY.append(PrintNewPageEntry(row));
                startRow = row;
                y = m_pSheet->rowFormats()->rowHeight(row);
                if (row >= repeatedRows.first) {
                    y += m_dPrintRepeatRowsHeight;
                    offset = m_dPrintRepeatRowsHeight;
                }
            }
            row++;
            y += m_pSheet->rowFormats()->rowHeight(row);
        }

        // Iterate to the end of the page.
        while (m_lnewPageListY.last().endItem() == 0) {
            if (y > printHeight || m_pSheet->rowFormats()->hasPageBreak(row)) {
                // Now store into the previous entry the enditem and the width
                m_lnewPageListY.last().setEndItem(row - 1);
                m_lnewPageListY.last().setSize(y - m_pSheet->rowFormats()->rowHeight(row));
                m_lnewPageListY.last().setOffset(offset);

                if (row - 1 > m_maxCheckedNewPageY) {
                    m_maxCheckedNewPageY = row - 1;
                }
                return;
            }
            ++row;
            y += m_pSheet->rowFormats()->rowHeight(row);
        }
    }

    if (_row > m_maxCheckedNewPageY) {
        m_maxCheckedNewPageY = _row;
        m_lnewPageListY.last().setEndItem(_row);
    }
}

void SheetPrint::Private::calculateZoomForPageLimitX()
{
    debugSheets << "Calculating zoom for X limit";
    PrintSettings *settings = m_pSheet->printSettings();
    const int horizontalPageLimit = settings->pageLimits().width();
    if (horizontalPageLimit == 0)
        return;

    const double origZoom = settings->zoom();

    if (settings->zoom() < 1.0) {
        q->updateHorizontalPageParameters(0); // clear all parameters
        settings->setZoom(1.0);
    }

    QRect printRange = m_pSheet->usedArea(true);
    calculateHorizontalPageParameters(printRange.right());
    int currentPages = m_lnewPageListX.count();

    if (currentPages <= horizontalPageLimit)
        return;

    // calculating a factor for scaling the zoom down makes it lots faster
    double factor = (double)horizontalPageLimit / (double)currentPages + 1 - (double)currentPages / ((double)currentPages + 1); // add possible error;
    debugSheets << "Calculated factor for scaling settings->zoom():" << factor;
    settings->setZoom(settings->zoom() * factor);

    debugSheets << "New exact zoom:" << settings->zoom();

    if (settings->zoom() < 0.01)
        settings->setZoom(0.01);
    if (settings->zoom() > 1.0)
        settings->setZoom(1.0);

    settings->setZoom((((int)(settings->zoom() * 100 + 0.5)) / 100.0));

    debugSheets << "New rounded zoom:" << settings->zoom();

    q->updateHorizontalPageParameters(0); // clear all parameters
    calculateHorizontalPageParameters(printRange.right());
    currentPages = m_lnewPageListX.count();

    debugSheets << "Number of pages with this zoom:" << currentPages;

    while ((currentPages > horizontalPageLimit) && (settings->zoom() > 0.01)) {
        settings->setZoom(settings->zoom() - 0.01);
        q->updateHorizontalPageParameters(0); // clear all parameters
        calculateHorizontalPageParameters(printRange.right());
        currentPages = m_lnewPageListX.count();
        debugSheets << "Looping -0.01; current zoom:" << settings->zoom();
    }

    if (settings->zoom() < origZoom) {
        // Trigger an update of the vertical page parameters.
        q->updateVerticalPageParameters(0); // clear all parameters
        calculateVerticalPageParameters(printRange.bottom());
    } else
        settings->setZoom(origZoom);
}

void SheetPrint::Private::calculateZoomForPageLimitY()
{
    debugSheets << "Calculating zoom for Y limit";
    PrintSettings *settings = m_pSheet->printSettings();
    const int verticalPageLimit = settings->pageLimits().height();
    if (verticalPageLimit == 0)
        return;

    const double origZoom = settings->zoom();

    if (settings->zoom() < 1.0) {
        q->updateVerticalPageParameters(0); // clear all parameters
        settings->setZoom(1.0);
    }

    QRect printRange = m_pSheet->usedArea(true);
    calculateVerticalPageParameters(printRange.bottom());
    int currentPages = m_lnewPageListY.count();

    if (currentPages <= verticalPageLimit)
        return;

    double factor = (double)verticalPageLimit / (double)currentPages + 1 - (double)currentPages / ((double)currentPages + 1); // add possible error
    debugSheets << "Calculated factor for scaling settings->zoom():" << factor;
    settings->setZoom(settings->zoom() * factor);

    debugSheets << "New exact zoom:" << settings->zoom();

    if (settings->zoom() < 0.01)
        settings->setZoom(0.01);
    if (settings->zoom() > 1.0)
        settings->setZoom(1.0);

    settings->setZoom((((int)(settings->zoom() * 100 + 0.5)) / 100.0));

    debugSheets << "New rounded zoom:" << settings->zoom();

    q->updateVerticalPageParameters(0); // clear all parameters
    calculateVerticalPageParameters(printRange.bottom());
    currentPages = m_lnewPageListY.count();

    debugSheets << "Number of pages with this zoom:" << currentPages;

    while ((currentPages > verticalPageLimit) && (settings->zoom() > 0.01)) {
        settings->setZoom(settings->zoom() - 0.01);
        q->updateVerticalPageParameters(0); // clear all parameters
        calculateVerticalPageParameters(printRange.bottom());
        currentPages = m_lnewPageListY.count();
        debugSheets << "Looping -0.01; current zoom:" << settings->zoom();
    }

    if (settings->zoom() < origZoom) {
        // Trigger an update of the horizontal page parameters.
        q->updateHorizontalPageParameters(0); // clear all parameters
        calculateHorizontalPageParameters(printRange.right());
    } else
        settings->setZoom(origZoom);
}

void SheetPrint::Private::updateRepeatedColumnsWidth()
{
    PrintSettings *settings = m_pSheet->printSettings();
    m_dPrintRepeatColumnsWidth = 0.0;
    const QPair<int, int> repeatedColumns = settings->repeatedColumns();
    if (repeatedColumns.first != 0)
        m_dPrintRepeatColumnsWidth = m_pSheet->columnFormats()->totalColWidth(repeatedColumns.first, repeatedColumns.second);
}

void SheetPrint::Private::updateRepeatedRowsHeight()
{
    PrintSettings *settings = m_pSheet->printSettings();
    m_dPrintRepeatRowsHeight = 0.0;
    const QPair<int, int> repeatedRows = settings->repeatedRows();
    if (repeatedRows.first != 0) {
        m_dPrintRepeatRowsHeight += m_pSheet->rowFormats()->totalRowHeight(repeatedRows.first, repeatedRows.second);
    }
}

bool PrintNewPageEntry::operator==(PrintNewPageEntry const &entry) const
{
    return m_iStartItem == entry.m_iStartItem;
}
