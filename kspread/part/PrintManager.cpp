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

#include "ui/SheetView.h"

#include <KoShape.h>
#include <KoZoomHandler.h>

#include <QPainter>
#include <QPrinter>

using namespace KSpread;

class PrintManager::Private
{
public:
    Private(PrintManager* const parent) : q(parent) {}

    SheetView* sheetView;
    double zoom;

public:
    void printPage(int page, QPainter&) const;
    bool pageNeedsPrinting(const QRect& cellRange) const;
    void setZoomFactor();
    /// Takes the repeated columns into account.
    double printWidth() const;
    /// Takes the repeated rows into account.
    double printHeight() const;

private:
    PrintManager* const q;
};

void PrintManager::Private::printPage(int page, QPainter& painter) const
{
    const Sheet* sheet = q->sheet();
    kDebug() << "printing page" << page;
    painter.save();

    const QRect cellRange = q->cellRange(page);

    QPointF topLeft(0.0, 0.0);

    // Calculate the dimension of the cell range. Needed for RTL painting and table centering.
    QRectF paintRect(topLeft, topLeft);
    for (int col = cellRange.left(); col <= cellRange.right(); ++col)
        paintRect.adjust(0.0, 0.0, sheet->columnFormat(col)->visibleWidth(), 0.0);
    for (int row = cellRange.top(); row <= cellRange.bottom(); ++row)
        paintRect.adjust(0.0, 0.0, 0.0, sheet->rowFormat(row)->visibleHeight());

    // Paint the cells.
    sheetView->setPaintCellRange(cellRange);
    sheetView->paintCells(painter, paintRect, topLeft);
    painter.restore();
}

bool PrintManager::Private::pageNeedsPrinting(const QRect& cellRange) const
{
    const Sheet* sheet = q->sheet();
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
    const Sheet* sheet = q->sheet();
    const PrintSettings settings = q->printSettings();
    const QSize pageLimits = settings.pageLimits();

    // if there are no page limits, take the usual zoom factor
    if (!pageLimits.isValid()) {
        zoom = settings.zoom();
        return;
    }

    // calculate the zoom factor from the page limits
    double zoomX = 1.0;
    double zoomY = 1.0;
    // iterate over the print ranges
    Region::ConstIterator end = settings.printRegion().constEnd();
    for (Region::ConstIterator it = settings.printRegion().constBegin(); it != end; ++it) {
        if (!(*it)->isValid())
            continue;

        // Take the document range limited to the used area.
        const QRectF printRange = sheet->cellCoordinatesToDocument((*it)->rect() & sheet->usedArea());

        if (pageLimits.width() > 0)
            zoomX = qMin(zoomX, printWidth() / printRange.width());
        if (pageLimits.height() > 0)
            zoomY = qMin(zoomY, printHeight() / printRange.height());
    }
    zoom = qMin(zoomX, zoomY);
}

double PrintManager::Private::printWidth() const
{
    const Sheet* sheet = q->sheet();
    const PrintSettings settings = q->printSettings();
    double width = settings.printWidth();
    const QPair<int, int> repeatedColumns = settings.repeatedColumns();
    if (repeatedColumns.first > 0) {
        const int startColumn = qMin(repeatedColumns.first, repeatedColumns.second);
        const int endColumn = qMax(repeatedColumns.first, repeatedColumns.second);
        for (int col = startColumn; col <= endColumn; ++col)
            width -= sheet->columnFormat(col)->visibleWidth();
    }
    return width;
}

double PrintManager::Private::printHeight() const
{
    const Sheet* sheet = q->sheet();
    const PrintSettings settings = q->printSettings();
    double height = settings.printHeight();
    const QPair<int, int> repeatedRows = settings.repeatedRows();
    if (repeatedRows.first > 0) {
        const int startRow = qMin(repeatedRows.first, repeatedRows.second);
        const int endRow = qMax(repeatedRows.first, repeatedRows.second);
        for (int row = startRow; row <= endRow; ++row)
            height -= sheet->rowFormat(row)->visibleHeight();
    }
    return height;
}


PrintManager::PrintManager(Sheet* sheet)
        : PageManager(sheet)
        , d(new Private(this))
{
    d->sheetView = new SheetView(sheet);
    d->setZoomFactor();
}

PrintManager::~PrintManager()
{
    delete d->sheetView;
    delete d;
}

void PrintManager::printPage(int page, QPainter& painter)
{
    const KoPageLayout pageLayout = printSettings().pageLayout();
    kDebug() << "page's cell range" << cellRange(page);

    // setup the QPainter
    painter.save();
    painter.setClipRect(0.0, 0.0, pageLayout.width / d->zoom, pageLayout.height / d->zoom);

    // setup the SheetView
    KoZoomHandler zoomHandler;
    zoomHandler.setZoom(d->zoom);
    d->sheetView->setViewConverter(&zoomHandler);

    // save and set painting flags
    const bool grid = sheet()->getShowGrid();
    const bool commentIndicator = sheet()->getShowCommentIndicator();
    const bool formulaIndicator = sheet()->getShowFormulaIndicator();
    sheet()->setShowGrid(printSettings().printGrid());
    sheet()->setShowCommentIndicator(printSettings().printCommentIndicator());
    sheet()->setShowFormulaIndicator(printSettings().printFormulaIndicator());

    // print the page
    d->printPage(page, painter);

    // restore painting flags
    sheet()->setShowGrid(grid);
    sheet()->setShowCommentIndicator(commentIndicator);
    sheet()->setShowFormulaIndicator(formulaIndicator);
    painter.restore();
}

double PrintManager::zoom() const
{
    return d->zoom;
}

QSizeF PrintManager::size(int page) const
{
    Q_UNUSED(page);
    const QSizeF size(d->printWidth(), d->printHeight());
    return (size / d->zoom + QSizeF(0.5, 0.5)).toSize();
}

bool PrintManager::pageNeedsPrinting(const QRect& cellRange) const
{
    return d->pageNeedsPrinting(cellRange);
}
