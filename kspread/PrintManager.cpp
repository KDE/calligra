/* This file is part of the KDE project
   Copyright 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#include "PrintManager.h"

#include "PrintSettings.h"
#include "Region.h"
#include "RowColumnFormat.h"
#include "Sheet.h"
#include "SheetView.h"

#include <KoZoomHandler.h>

#include <QPainter>
#include <QPrinter>

using namespace KSpread;

class PrintManager::Private
{
public:
    Sheet* sheet;
    SheetView* sheetView;
    QMap<int, QRect> pages; // page number to cell range
    Region printRegion;
    PrintSettings settings;
    KoZoomHandler* zoomHandler;

public:
    void calculatePages();
    void printPage(int page, QPainter&) const;
    bool pageNeedsPrinting(const QRect& cellRange) const;
};

void PrintManager::Private::calculatePages()
{
    int pageNumber = 1;
    const double printWidth = settings.printWidth();
    const double printHeight = settings.printHeight();
    kDebug() << "printWidth" << printWidth << "printHeight" << printHeight;

    if (settings.pageOrder() == PrintSettings::LeftToRight)
    {
//         kDebug() << "processing printRanges" << printRegion;
        // iterate over the print ranges
        Region::ConstIterator end = printRegion.constEnd();
        for (Region::ConstIterator it = printRegion.constBegin(); it != end; ++it)
        {
            if (!(*it)->isValid())
                continue;
            if ((*it)->sheet() != sheet)
                continue;

            // limit the print range to the used area
            const QRect printRange = (*it)->rect() & sheet->usedArea();
            kDebug() << "processing printRange" << printRange;

            int rows = 0;
            double height = 0.0;
            for (int row = printRange.top(); row <= printRange.bottom(); ++row)
            {
                rows++;
                height += sheet->rowFormat(row)->height();

                // 1. find the number of rows per page
                if (row == printRange.bottom()) // always iterate over the last 'page row'
                    ;
                else if (height + sheet->rowFormat(row + 1)->height() <= printHeight)
                    continue;

//                 kDebug() << "1. done: row" << row << "rows" << rows << "height" << height;

                int columns = 0;
                double width = 0.0;
                // 2. iterate over the columns and create the pages
                for (int col = printRange.left(); col < printRange.right(); ++col)
                {
                    columns++;
                    width += sheet->columnFormat(col)->width();

                    // Does the next column fit too?
                    if (width + sheet->columnFormat(col + 1)->width() <= printWidth)
                        continue;

//                     kDebug() << "col" << col << "columns" << columns << "width" << width;
                    const QRect cellRange(col - columns + 1, row - rows + 1, columns, rows);
                    if (pageNeedsPrinting(cellRange))
                        pages.insert(pageNumber++, cellRange);
                    columns = 0;
                    width = 0.0;
                }
                // Always insert a page for the last column
                columns++;
                const QRect cellRange(printRange.right() - columns + 1, row - rows + 1, columns, rows);
                if (pageNeedsPrinting(cellRange))
                    pages.insert(pageNumber++, cellRange);

                // 3. prepare for the next row of pages
                rows = 0;
                height = 0.0;
            }
        }
    }
    else // if (settings.pageOrder() == PrintSettings::TopToBottom)
    {
//         kDebug() << "processing printRanges" << printRegion;
        // iterate over the print ranges
        Region::ConstIterator end = printRegion.constEnd();
        for (Region::ConstIterator it = printRegion.constBegin(); it != end; ++it)
        {
            if (!(*it)->isValid())
                continue;
            if ((*it)->sheet() != sheet)
                continue;

            // limit the print range to the used area
            const QRect printRange = (*it)->rect() & sheet->usedArea();
            kDebug() << "processing printRange" << printRange;

            int columns = 0;
            double width = 0.0;
            for (int col = printRange.left(); col <= printRange.right(); ++col)
            {
                columns++;
                width += sheet->columnFormat(col)->width();

                // 1. find the number of columns per page
                if (col == printRange.right()) // always iterate over the last 'page column'
                    ;
                else if (width + sheet->columnFormat(col + 1)->width() <= printWidth)
                    continue;

//                 kDebug() << "1. done: col" << col << "columns" << columns << "width" << width;

                int rows = 0;
                double height = 0.0;
                // 2. iterate over the rows and create the pages
                for (int row = printRange.top(); row < printRange.bottom(); ++row)
                {
                    rows++;
                    height += sheet->rowFormat(row)->height();

                    // Does the next row fit too?
                    if (height + sheet->rowFormat(row + 1)->height() <= printHeight)
                        continue;

//                     kDebug() << "row" << row << "rows" << rows << "height" << height;
                    const QRect cellRange(col - columns + 1, row - rows + 1, columns, rows);
                    if (pageNeedsPrinting(cellRange))
                        pages.insert(pageNumber++, cellRange);
                    rows = 0;
                    height = 0.0;
                }
                // Always insert a page for the last row
                rows++;
                const QRect cellRange(col - columns + 1, printRange.bottom() - rows + 1, columns, rows);
                if (pageNeedsPrinting(cellRange))
                    pages.insert(pageNumber++, cellRange);

                // 3. prepare for the next column of pages
                columns = 0;
                width = 0.0;
            }
        }
    }
}

void PrintManager::Private::printPage(int page, QPainter& painter) const
{
    kDebug() << "printing page" << page;

    const QRect cellRange = pages[page];

    QPointF topLeft(0.0, 0.0);

    // Calculate the dimension of the cell range. Needed for RTL painting.
    QRectF paintRect(topLeft, topLeft);
    for (int col = cellRange.left(); col <= cellRange.right(); ++col)
        paintRect.adjust(0.0, 0.0, sheet->columnFormat(col)->width(), 0.0);
    for (int row = cellRange.top(); row <= cellRange.bottom(); ++row)
        paintRect.adjust(0.0, 0.0, 0.0, sheet->rowFormat(row)->height());

    // Paint the cells.
    sheetView->setPaintCellRange(cellRange);
    sheetView->paintCells(painter.device(), painter, paintRect, topLeft);
}

bool PrintManager::Private::pageNeedsPrinting(const QRect& cellRange) const
{
    // TODO Stefan: Is there a better, faster approach?
    for (int row = cellRange.top(); row <= cellRange.bottom() ; ++row)
        for (int col = cellRange.left(); col <= cellRange.right(); ++col)
            if (Cell(sheet, col, row).needsPrinting())
                return true;
    return false;
}


PrintManager::PrintManager(Sheet* sheet)
    : d(new Private)
{
    d->sheet = sheet;
    d->sheetView = new SheetView(sheet);
    d->settings = *sheet->printSettings();
    d->printRegion = Region(1, 1, KS_colMax, KS_rowMax, sheet);
    d->zoomHandler = new KoZoomHandler();
}

PrintManager::~PrintManager()
{
    delete d->sheetView;
    delete d->zoomHandler;
    delete d;
}

void PrintManager::setPrintSettings(const PrintSettings& settings, bool force)
{
    if (!force && settings == d->settings)
        return;
    kDebug() << (d->pages.isEmpty() ? "Creating" : "Recreating") << "pages...";
    d->settings = settings;
    d->calculatePages();
}

bool PrintManager::print(QPainter& painter, QPrinter* printer)
{
    const KoPageLayout pageLayout = d->settings.pageLayout();
    kDebug(36004) << "PageLayout:"
                  << "w" << pageLayout.width
                  << "h" << pageLayout.height
                  << "l" << pageLayout.left
                  << "r" << pageLayout.right
                  << "t" << pageLayout.top
                  << "b" << pageLayout.bottom;
    d->calculatePages();
    kDebug(36004) << d->pages;

    // setup the QPainter
    painter.translate(pageLayout.left, pageLayout.top);
    painter.scale(d->zoomHandler->zoomedResolutionX(), d->zoomHandler->zoomedResolutionY());
    painter.setClipRect(0.0, 0.0, pageLayout.width, pageLayout.height);

    // setup the SheetView
    d->sheetView->setPaintDevice(painter.device());
    d->sheetView->setViewConverter(d->zoomHandler);

    // print the pages
    for (int page = 1; page <= d->pages.count(); ++page)
    {
        d->printPage(page, painter);
        if (page != d->pages.count())
            printer->newPage();
    }

    return !d->pages.isEmpty();
}

void PrintManager::printPage(int page, QPainter& painter)
{
    const KoPageLayout pageLayout = d->settings.pageLayout();
    kDebug(36004) << "PageLayout:"
                  << "w" << pageLayout.width
                  << "h" << pageLayout.height
                  << "l" << pageLayout.left
                  << "r" << pageLayout.right
                  << "t" << pageLayout.top
                  << "b" << pageLayout.bottom;

    // setup the QPainter
    painter.save();
    painter.translate(pageLayout.left, pageLayout.top);
    painter.scale(d->zoomHandler->zoomedResolutionX(), d->zoomHandler->zoomedResolutionY());
    painter.setClipRect(0.0, 0.0, pageLayout.width, pageLayout.height);

    // setup the SheetView
    d->sheetView->setPaintDevice(painter.device());
    d->sheetView->setViewConverter(d->zoomHandler);

    // print the page
    d->printPage(page, painter);

    painter.restore();
}

int PrintManager::pageCount() const
{
    return d->pages.count();
}

QRect PrintManager::cellRange(int page) const
{
    if (page < 1 || page > d->pages.count())
        return QRect();
    return d->pages[page];
}
