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

#include <KoShape.h>
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
    PrintSettings settings;
    double zoom;

public:
    void calculatePages();
    void printPage(int page, QPainter&) const;
    bool pageNeedsPrinting(const QRect& cellRange) const;
    void setZoomFactor();
};

void PrintManager::Private::calculatePages()
{
    pages.clear();
    int pageNumber = 1;
    const double printWidth = qRound(settings.printWidth() / zoom + 0.5);
    const double printHeight = qRound(settings.printHeight() / zoom + 0.5);
//     kDebug() << "printWidth" << printWidth << "printHeight" << printHeight;

    if (settings.pageOrder() == PrintSettings::LeftToRight)
    {
//         kDebug() << "processing printRanges" << settings.printRegion();
        // iterate over the print ranges
        Region::ConstIterator end = settings.printRegion().constEnd();
        for (Region::ConstIterator it = settings.printRegion().constBegin(); it != end; ++it)
        {
            if (!(*it)->isValid())
                continue;

            // limit the print range to the used area
            const QRect printRange = (*it)->rect() & sheet->usedArea();
//             kDebug() << "processing printRange" << printRange;

            int rows = 0;
            double height = 0.0;
            for (int row = printRange.top(); row <= printRange.bottom(); ++row)
            {
                rows++;
                height += sheet->rowFormat(row)->visibleHeight();

                // 1. find the number of rows per page
                if (row == printRange.bottom()) // always iterate over the last 'page row'
                    ;
                else if (height + sheet->rowFormat(row + 1)->visibleHeight() <= printHeight)
                    continue;

//                 kDebug() << "1. done: row" << row << "rows" << rows << "height" << height;

                int columns = 0;
                double width = 0.0;
                // 2. iterate over the columns and create the pages
                for (int col = printRange.left(); col < printRange.right(); ++col)
                {
                    columns++;
                    width += sheet->columnFormat(col)->visibleWidth();

                    // Does the next column fit too?
                    if (width + sheet->columnFormat(col + 1)->visibleWidth() <= printWidth)
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
//         kDebug() << "processing printRanges" << settings.printRegion();
        // iterate over the print ranges
        Region::ConstIterator end = settings.printRegion().constEnd();
        for (Region::ConstIterator it = settings.printRegion().constBegin(); it != end; ++it)
        {
            if (!(*it)->isValid())
                continue;

            // limit the print range to the used area
            const QRect printRange = (*it)->rect() & sheet->usedArea();
            kDebug() << "processing printRange" << printRange;

            int columns = 0;
            double width = 0.0;
            for (int col = printRange.left(); col <= printRange.right(); ++col)
            {
                columns++;
                width += sheet->columnFormat(col)->visibleWidth();

                // 1. find the number of columns per page
                if (col == printRange.right()) // always iterate over the last 'page column'
                    ;
                else if (width + sheet->columnFormat(col + 1)->visibleWidth() <= printWidth)
                    continue;

//                 kDebug() << "1. done: col" << col << "columns" << columns << "width" << width;

                int rows = 0;
                double height = 0.0;
                // 2. iterate over the rows and create the pages
                for (int row = printRange.top(); row < printRange.bottom(); ++row)
                {
                    rows++;
                    height += sheet->rowFormat(row)->visibleHeight();

                    // Does the next row fit too?
                    if (height + sheet->rowFormat(row + 1)->visibleHeight() <= printHeight)
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
    kDebug() << pages.count() << "page(s) created";
}

void PrintManager::Private::printPage(int page, QPainter& painter) const
{
    kDebug() << "printing page" << page;

    const QRect cellRange = pages[page];

    QPointF topLeft(0.0, 0.0);

    // Calculate the dimension of the cell range. Needed for RTL painting.
    QRectF paintRect(topLeft, topLeft);
    for (int col = cellRange.left(); col <= cellRange.right(); ++col)
        paintRect.adjust(0.0, 0.0, sheet->columnFormat(col)->visibleWidth(), 0.0);
    for (int row = cellRange.top(); row <= cellRange.bottom(); ++row)
        paintRect.adjust(0.0, 0.0, 0.0, sheet->rowFormat(row)->visibleHeight());

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

    QRectF shapesBoundingRect;
    const QList<KoShape*> shapes = sheet->shapes();
    for (int i = 0; i < shapes.count(); ++i)
        shapesBoundingRect |= shapes[i]->boundingRect();
    const QRect shapesCellRange = sheet->documentToCellCoordinates(shapesBoundingRect);
    return !(cellRange & shapesCellRange).isEmpty();
}

void PrintManager::Private::setZoomFactor()
{
    const QSize pageLimits = settings.pageLimits();

    // if there are no page limits, take the usual zoom factor
    if (!pageLimits.isValid())
    {
        zoom = settings.zoom();
        return;
    }

    // calculate the zoom factor from the page limits
    double zoomX = 1.0;
    double zoomY = 1.0;
    // iterate over the print ranges
    Region::ConstIterator end = settings.printRegion().constEnd();
    for (Region::ConstIterator it = settings.printRegion().constBegin(); it != end; ++it)
    {
        if (!(*it)->isValid())
            continue;

        // Take the document range limited to the used area.
        const QRectF printRange = sheet->cellCoordinatesToDocument((*it)->rect() & sheet->usedArea());

        if (pageLimits.width() > 0)
            zoomX = qMin(zoomX, settings.printWidth() / printRange.width());
        if (pageLimits.height() > 0)
            zoomY = qMin(zoomY, settings.printHeight() / printRange.height());
    }
    zoom = qMin(zoomX, zoomY);
}


PrintManager::PrintManager(Sheet* sheet)
    : d(new Private)
{
    d->sheet = sheet;
    d->sheetView = new SheetView(sheet);
    d->settings = *sheet->printSettings();
    d->setZoomFactor();
}

PrintManager::~PrintManager()
{
    delete d->sheetView;
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

void PrintManager::printPage(int page, QPainter& painter)
{
    const KoPageLayout pageLayout = d->settings.pageLayout();
    kDebug() << "page's cell range" << d->pages[page];

    // setup the QPainter
    painter.save();
    painter.setClipRect(0.0, 0.0, pageLayout.width / d->zoom, pageLayout.height / d->zoom);

    // setup the SheetView
    d->sheetView->setPaintDevice(painter.device());
    KoZoomHandler zoomHandler;
    zoomHandler.setZoom(d->zoom);
    d->sheetView->setViewConverter(&zoomHandler);

    // save and set painting flags
    const bool grid = d->sheet->getShowGrid();
    const bool commentIndicator = d->sheet->getShowCommentIndicator();
    const bool formulaIndicator = d->sheet->getShowFormulaIndicator();
    d->sheet->setShowGrid(d->settings.printGrid());
    d->sheet->setShowCommentIndicator(d->settings.printCommentIndicator());
    d->sheet->setShowFormulaIndicator(d->settings.printFormulaIndicator());

    // print the page
    d->printPage(page, painter);

    // restore painting flags
    d->sheet->setShowGrid(grid);
    d->sheet->setShowCommentIndicator(commentIndicator);
    d->sheet->setShowFormulaIndicator(formulaIndicator);
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

double PrintManager::zoom() const
{
    return d->zoom;
}
