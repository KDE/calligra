/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "PageManager.h"

#include "core/ColFormatStorage.h"
#include "core/PrintSettings.h"
#include "core/RowFormatStorage.h"
#include "core/Sheet.h"
#include "engine/Region.h"

#include <QList>

using namespace Calligra::Sheets;

class Q_DECL_HIDDEN PageManager::Private
{
public:
    Sheet *sheet;
    QList<QRect> pages; // page number to cell range
    PrintSettings settings;
};

PageManager::PageManager(Sheet *sheet)
    : d(new Private)
{
    d->sheet = sheet;
    d->settings = *sheet->printSettings();
}

PageManager::~PageManager()
{
    delete d;
}

void PageManager::layoutPages()
{
    const Sheet *sheet = d->sheet;
    const PrintSettings settings = d->settings;
    d->pages.clear();
    clearPages();
    int pageNumber = 1;
    preparePage(pageNumber);

    if (settings.pageOrder() == PrintSettings::LeftToRight) {
        //         debugSheets << "processing printRanges" << settings.printRegion();
        // iterate over the print ranges
        Region::ConstIterator end = settings.printRegion().constEnd();
        for (Region::ConstIterator it = settings.printRegion().constBegin(); it != end; ++it) {
            if (!(*it)->isValid())
                continue;

            // limit the print range to the used area
            const QRect printRange = (*it)->rect() & sheet->usedArea(true);
            //             debugSheets << "processing printRange" << printRange;

            int rows = 0;
            double height = 0.0;
            for (int row = printRange.top(); row <= printRange.bottom(); ++row) {
                rows++;
                height += sheet->rowFormats()->visibleHeight(row);

                // 1. find the number of rows per page
                if (row == printRange.bottom()) // always iterate over the last 'page row'
                    ;
                else if (height + sheet->rowFormats()->visibleHeight(row + 1) <= size(pageNumber).height())
                    continue;

                //                 debugSheets << "1. done: row" << row << "rows" << rows << "height" << height;

                int columns = 0;
                double width = 0.0;
                // 2. iterate over the columns and create the pages
                for (int col = printRange.left(); col < printRange.right(); ++col) {
                    columns++;
                    width += sheet->columnFormats()->visibleWidth(col);

                    // Does the next column fit too?
                    if (width + sheet->columnFormats()->visibleWidth(col + 1) <= size(pageNumber).width())
                        continue;

                    //                     debugSheets << "col" << col << "columns" << columns << "width" << width;
                    const QRect cellRange(col - columns + 1, row - rows + 1, columns, rows);
                    if (pageNeedsPrinting(cellRange)) {
                        d->pages.append(cellRange);
                        insertPage(pageNumber++);
                        preparePage(pageNumber); // prepare the next page
                    }
                    columns = 0;
                    width = 0.0;
                }
                // Always insert a page for the last column
                columns++;
                const QRect cellRange(printRange.right() - columns + 1, row - rows + 1, columns, rows);
                if (pageNeedsPrinting(cellRange)) {
                    d->pages.append(cellRange);
                    insertPage(pageNumber);
                    pageNumber++;
                }

                // 3. prepare for the next row of pages
                if (row != printRange.bottom()) {
                    preparePage(pageNumber);
                }
                rows = 0;
                height = 0.0;
            }
        }
    } else { // if (settings.pageOrder() == PrintSettings::TopToBottom)
        //         debugSheets << "processing printRanges" << settings.printRegion();
        // iterate over the print ranges
        Region::ConstIterator end = settings.printRegion().constEnd();
        for (Region::ConstIterator it = settings.printRegion().constBegin(); it != end; ++it) {
            if (!(*it)->isValid())
                continue;

            // limit the print range to the used area
            const QRect printRange = (*it)->rect() & sheet->usedArea();
            debugSheets << "processing printRange" << printRange;

            int columns = 0;
            double width = 0.0;
            for (int col = printRange.left(); col <= printRange.right(); ++col) {
                columns++;
                width += sheet->columnFormats()->visibleWidth(col);

                // 1. find the number of columns per page
                if (col == printRange.right()) // always iterate over the last 'page column'
                    ;
                else if (width + sheet->columnFormats()->visibleWidth(col + 1) <= size(pageNumber).width())
                    continue;

                //                 debugSheets << "1. done: col" << col << "columns" << columns << "width" << width;

                int rows = 0;
                double height = 0.0;
                // 2. iterate over the rows and create the pages
                for (int row = printRange.top(); row < printRange.bottom(); ++row) {
                    rows++;
                    height += sheet->rowFormats()->visibleHeight(row);

                    // Does the next row fit too?
                    if (height + sheet->rowFormats()->visibleHeight(row + 1) <= size(pageNumber).height())
                        continue;

                    //                     debugSheets << "row" << row << "rows" << rows << "height" << height;
                    const QRect cellRange(col - columns + 1, row - rows + 1, columns, rows);
                    if (pageNeedsPrinting(cellRange)) {
                        d->pages.append(cellRange);
                        insertPage(pageNumber++);
                        preparePage(pageNumber); // prepare the next page
                    }
                    rows = 0;
                    height = 0.0;
                }
                // Always insert a page for the last row
                rows++;
                const QRect cellRange(col - columns + 1, printRange.bottom() - rows + 1, columns, rows);
                if (pageNeedsPrinting(cellRange)) {
                    d->pages.append(cellRange);
                    insertPage(pageNumber);
                    pageNumber++;
                }

                // 3. prepare for the next column of pages
                if (col != printRange.right()) {
                    preparePage(pageNumber);
                }
                columns = 0;
                width = 0.0;
            }
        }
    }
    debugSheets << d->pages.count() << "page(s) created";
}

void PageManager::setPrintSettings(const PrintSettings &settings, bool force)
{
    if (!force && settings == d->settings)
        return;
    debugSheets << (d->pages.isEmpty() ? "Creating" : "Recreating") << "pages...";
    d->settings = settings;
    layoutPages();
}

int PageManager::pageCount() const
{
    return d->pages.count();
}

QRect PageManager::cellRange(int page) const
{
    if (page < 1 || page > d->pages.count())
        return QRect();
    return d->pages[page - 1];
}

QSizeF PageManager::size(int page) const
{
    if (page < 1 || page > d->pages.count())
        return QSizeF();
    return QSizeF(d->settings.printWidth() + 0.5, d->settings.printHeight() + 0.5); // FIXME
}

Sheet *PageManager::sheet() const
{
    return d->sheet;
}

const PrintSettings &PageManager::printSettings() const
{
    return d->settings;
}

void PageManager::clearPages()
{
}

bool PageManager::pageNeedsPrinting(const QRect &cellRange) const
{
    Q_UNUSED(cellRange);
    return true;
}

void PageManager::insertPage(int page)
{
    Q_UNUSED(page);
}

void PageManager::preparePage(int page)
{
    Q_UNUSED(page);
}
