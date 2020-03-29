/* This file is part of the KDE project
   Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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
#include "SheetView.h"

#include <QCache>
#include <QRect>
#include <QPainter>
#include <QPainterPath>
#ifdef CALLIGRA_SHEETS_MT
#include <QMutex>
#include <QMutexLocker>
#include <QReadWriteLock>
#include <QReadLocker>
#include <QWriteLocker>
#endif

#include <KoViewConverter.h>

#include "CellView.h"
#include "calligra_sheets_limits.h"
#include "PointStorage.h"
#include "RectStorage.h"
#include "Region.h"
#include "RowColumnFormat.h"
#include "RowFormatStorage.h"
#include "Sheet.h"

using namespace Calligra::Sheets;

struct CellPaintData
{
    CellPaintData(const CellView &cellView, const Cell &cell, const QPointF &coordinate)
        : cellView(cellView)
          , cell(cell)
          , coordinate(coordinate)
    {}
    CellView cellView;
    Cell cell;
    QPointF coordinate;
};

class Q_DECL_HIDDEN SheetView::Private
{
public:
    Private()
#ifdef CALLIGRA_SHEETS_MT
        : cacheMutex(QMutex::Recursive)
#endif
    {}
    const Sheet* sheet;
    const KoViewConverter* viewConverter;
    QRect visibleRect;
    QCache<QPoint, CellView> cache;
#ifdef CALLIGRA_SHEETS_MT
    QMutex cacheMutex;
#endif
    QRegion cachedArea;
    CellView* defaultCellView;
    // The maximum accessed cell range used for the scrollbar ranges.
    QSize accessedCellRange;
    FusionStorage* obscuredInfo;
    QSize obscuredRange; // size of the bounding box of obscuredInfo
#ifdef CALLIGRA_SHEETS_MT
    QReadWriteLock obscuredLock;
#endif

    PointStorage<bool> highlightedCells;
    QPoint activeHighlight;
#ifdef CALLIGRA_SHEETS_MT
    QReadWriteLock highlightLock;
#endif
    QColor highlightColor;
    QColor highlightMaskColor;
    QColor activeHighlightColor;
public:
    Cell cellToProcess(int col, int row, QPointF& coordinate, QSet<Cell>& processedMergedCells, const QRect& visRect);
#ifdef CALLIGRA_SHEETS_MT
    CellView cellViewToProcess(Cell& cell, QPointF& coordinate, QSet<Cell>& processedObscuredCells,
                               SheetView* sheetView, const QRect& visRect);
#else
    const CellView& cellViewToProcess(Cell& cell, QPointF& coordinate, QSet<Cell>& processedObscuredCells,
                               SheetView* sheetView, const QRect& visRect);
#endif
};

Cell SheetView::Private::cellToProcess(int col, int row, QPointF& coordinate,
                                       QSet<Cell>& processedMergedCells,
                                       const QRect& visRect)
{
    Cell cell(sheet, col, row);
    if (cell.isPartOfMerged()) {
        cell = cell.masterCell();
        // if the rect of visible cells contains this master cell, it was already painted
        if (visRect.contains(cell.cellPosition())) {
            coordinate.setY(coordinate.y() + sheet->rowFormats()->rowHeight(row));
            return Cell(); // next row
        }
        // if the out of bounds master cell was already painted, there's nothing more to do
        if (processedMergedCells.contains(cell)) {
            coordinate.setY(coordinate.y() + sheet->rowFormats()->rowHeight(row));
            return Cell(); // next row
        }
        processedMergedCells.insert(cell);
        // take the coordinate of the master cell
        if (sheet->layoutDirection() == Qt::RightToLeft) {
            for (int c = cell.column()+1; c <= col; ++c)
                coordinate.setX(coordinate.x() + sheet->columnFormat(c)->width());
        } else {
            for (int c = cell.column(); c < col; ++c)
                coordinate.setX(coordinate.x() - sheet->columnFormat(c)->width());
        }
        for (int r = cell.row(); r < row; ++r)
            coordinate.setY(coordinate.y() - sheet->rowFormats()->rowHeight(r));
    }
    return cell;
}

#ifdef CALLIGRA_SHEETS_MT
CellView SheetView::Private::cellViewToProcess(Cell& cell, QPointF& coordinate,
        QSet<Cell>& processedObscuredCells, SheetView* sheetView, const QRect& visRect)
#else
const CellView& SheetView::Private::cellViewToProcess(Cell& cell, QPointF& coordinate,
        QSet<Cell>& processedObscuredCells, SheetView* sheetView, const QRect& visRect)
#endif
{
    const int col = cell.column();
    const int row = cell.row();
    const QPoint cellPos = cell.cellPosition();
#ifdef CALLIGRA_SHEETS_MT
    CellView cellView = sheetView->cellView(col, row);
#else
    const CellView& cellView = sheetView->cellView(col, row);
#endif
    if (sheetView->isObscured(cellPos)) {
        // if the rect of visible cells contains the obscuring cell, it was already painted
        if (visRect.contains(sheetView->obscuringCell(cellPos))) {
            coordinate.setY(coordinate.y() + sheet->rowFormats()->rowHeight(row));
            cell = Cell();
            return cellView; // next row
        }
        cell = Cell(sheet, sheetView->obscuringCell(cellPos));
        if (processedObscuredCells.contains(cell)) {
            coordinate.setY(coordinate.y() + sheet->rowFormats()->rowHeight(row));
            cell = Cell();
            return cellView; // next row
        }
        processedObscuredCells.insert(cell);
        // take the coordinate of the obscuring cell
        if (sheet->layoutDirection() == Qt::RightToLeft) {
            for (int c = cell.column()+1; c <= col; ++c)
                coordinate.setX(coordinate.x() + sheet->columnFormat(c)->width());
        } else {
            for (int c = cell.column(); c < col; ++c)
                coordinate.setX(coordinate.x() - sheet->columnFormat(c)->width());
        }
        for (int r = cell.row(); r < row; ++r)
            coordinate.setY(coordinate.y() - sheet->rowFormats()->rowHeight(r));
        // use the CellView of the obscuring cell
        return sheetView->cellView(cell.column(), cell.row());
    }
    return cellView;
}


SheetView::SheetView(const Sheet* sheet)
        : QObject(const_cast<Sheet*>(sheet))
        , d(new Private)
{
    d->sheet = sheet;
    d->viewConverter = 0;
    d->visibleRect = QRect(1, 1, 0, 0);
    d->cache.setMaxCost(10000);
    d->defaultCellView = createDefaultCellView();
    d->accessedCellRange =  sheet->usedArea().size().expandedTo(QSize(256, 256));
    d->obscuredInfo = new FusionStorage(sheet->map());
    d->obscuredRange = QSize(0, 0);
    d->highlightMaskColor = QColor(0, 0, 0, 128);
    d->activeHighlightColor = QColor(255, 127, 0, 128);
}

SheetView::~SheetView()
{
    delete d->defaultCellView;
    delete d->obscuredInfo;
    delete d;
}

const Sheet* SheetView::sheet() const
{
    return d->sheet;
}

void SheetView::setViewConverter(const KoViewConverter* viewConverter)
{
    Q_ASSERT(viewConverter);
    d->viewConverter = viewConverter;
}

const KoViewConverter* SheetView::viewConverter() const
{
    Q_ASSERT(d->viewConverter);
    return d->viewConverter;
}

#ifdef CALLIGRA_SHEETS_MT
CellView SheetView::cellView(const QPoint& pos)
#else
const CellView& SheetView::cellView(const QPoint& pos)
#endif
{
    return cellView(pos.x(), pos.y());
}

#ifdef CALLIGRA_SHEETS_MT
CellView SheetView::cellView(int col, int row)
#else
const CellView& SheetView::cellView(int col, int row)
#endif
{
    Q_ASSERT(1 <= col && col <= KS_colMax);
    Q_ASSERT(1 <= row && col <= KS_rowMax);
#ifdef CALLIGRA_SHEETS_MT
    QMutexLocker ml(&d->cacheMutex);
#endif
    CellView *v = d->cache.object(QPoint(col, row));
    if (!v) {
        v = createCellView(col, row);
        d->cache.insert(QPoint(col, row), v);
        d->cachedArea += QRect(col, row, 1, 1);
    }
#ifdef CALLIGRA_SHEETS_MT
    // create a copy as long as the mutex is locked
    CellView cellViewCopy = *v;
    return cellViewCopy;
#else
    return *v;
#endif
}

void SheetView::setPaintCellRange(const QRect& rect)
{
#ifdef CALLIGRA_SHEETS_MT
    QMutexLocker ml(&d->cacheMutex);
#endif
    d->visibleRect = rect & QRect(1, 1, KS_colMax, KS_rowMax);
    d->cache.setMaxCost(2 * rect.width() * rect.height());
}

QRect SheetView::paintCellRange() const
{
    return d->visibleRect;
}

void SheetView::invalidateRegion(const Region& region)
{
    QRegion qregion;
    Region::ConstIterator end(region.constEnd());
    for (Region::ConstIterator it(region.constBegin()); it != end; ++it) {
        qregion += (*it)->rect();
    }
    // reduce to the cached area
    qregion &= d->cachedArea;
    QVector<QRect> rects = qregion.rects();
    for (int i = 0; i < rects.count(); ++i)
        invalidateRange(rects[i]);
}

void SheetView::invalidate()
{
#ifdef CALLIGRA_SHEETS_MT
    QMutexLocker ml(&d->cacheMutex);
#endif
    delete d->defaultCellView;
    d->defaultCellView = createDefaultCellView();
    d->cache.clear();
    d->cachedArea = QRegion();
    delete d->obscuredInfo;
    d->obscuredInfo = new FusionStorage(d->sheet->map());
    d->obscuredRange = QSize(0, 0);
}

void SheetView::paintCells(QPainter& painter, const QRectF& paintRect, const QPointF& topLeft, CanvasBase*, const QRect& visibleRect)
{
    const QRect& visRect = visibleRect.isValid() ? visibleRect : d->visibleRect;
    // paintRect:   the canvas area, that should be painted; in document coordinates;
    //              no layout direction consideration; scrolling offset applied;
    //              independent from painter transformations
    // topLeft:     the document coordinate of the top left cell's top left corner;
    //              no layout direction consideration; independent from painter
    //              transformations

    // NOTE Stefan: The painting is split into several steps. In each of these all cells in
    //              d->visibleRect are traversed. This may appear suboptimal at the first look, but
    //              ensures that the borders are not erased by the background of adjacent cells.

// debugSheets << "paintRect:" << paintRect;
// debugSheets << "topLeft:" << topLeft;

    QRegion clipRect(painter.clipRegion());
    // 0. Paint the sheet background
    if (!sheet()->backgroundImage().isNull()) {
        //TODO support all the different properties
        Sheet::BackgroundImageProperties properties = sheet()->backgroundImageProperties();
        if( properties.repeat == Sheet::BackgroundImageProperties::Repeat ) {
            const int firstCol = visRect.left();
            const int firstRow = visRect.top();
            const int firstColPosition = d->sheet->columnPosition(firstCol);
            const int firstRowPosition = d->sheet->rowPosition(firstRow);

            const int imageWidth = sheet()->backgroundImage().rect().width();
            const int imageHeight = sheet()->backgroundImage().rect().height();

            int xBackground = firstColPosition - (firstColPosition % imageWidth);
            int yBackground = firstRowPosition - (firstRowPosition % imageHeight);

            const int lastCol = visRect.right();
            const int lastRow = visRect.bottom();
            const int lastColPosition = d->sheet->columnPosition(lastCol);
            const int lastRowPosition = d->sheet->rowPosition(lastRow);

            while( xBackground < lastColPosition ) {
                int y = yBackground;
                while( y < lastRowPosition ) {
                    painter.drawImage(QRect(xBackground, y, imageWidth, imageHeight), sheet()->backgroundImage());
                    y += imageHeight;
                }
                xBackground += imageWidth;
            }
        }
    }

    // 1. Paint the cell background

    // Handle right-to-left layout.
    // In an RTL sheet the cells have to be painted at their opposite horizontal
    // location on the canvas, meaning that column A will be the rightmost column
    // on screen, column B will be to the left of it and so on. Here we change
    // the horizontal coordinate at which we start painting the cell in case the
    // sheet's direction is RTL.
    const bool rightToLeft = sheet()->layoutDirection() == Qt::RightToLeft;
    const QPointF startCoordinate(rightToLeft ? paintRect.width() - topLeft.x() : topLeft.x(), topLeft.y());
    QPointF coordinate(startCoordinate);
// debugSheets << "start coordinate:" << coordinate;
    QSet<Cell> processedMergedCells;
    QSet<Cell> processedObscuredCells;
    QList<CellPaintData> cached_cells;
    for (int col = visRect.left(); col <= visRect.right(); ++col) {
        if (d->sheet->columnFormat(col)->isHiddenOrFiltered())
            continue;
        if (rightToLeft)
            coordinate.setX(coordinate.x() - d->sheet->columnFormat(col)->width());
// debugSheets <<"coordinate:" << coordinate;
        for (int row = visRect.top(); row <= visRect.bottom(); ++row) {
            int lastHiddenRow;
            if (d->sheet->rowFormats()->isHiddenOrFiltered(row, &lastHiddenRow)) {
                row = lastHiddenRow;
                continue;
            }
            // save the coordinate
            const QPointF savedCoordinate = coordinate;
            // figure out, if any and which cell has to be painted (may be a master cell)
            Cell cell = d->cellToProcess(col, row, coordinate, processedMergedCells, visRect);
            if (!cell)
                continue;
            // figure out, which CellView to use (may be one for an obscuring cell)
            CellPaintData cpd(d->cellViewToProcess(cell, coordinate, processedObscuredCells, this, visRect), cell, coordinate);
            if (!cell)
                continue;
            cpd.cellView.paintCellBackground(painter, clipRect, coordinate);
            cached_cells.append(cpd);
            // restore coordinate
            coordinate = savedCoordinate;
            coordinate.setY(coordinate.y() + d->sheet->rowFormats()->rowHeight(row));
        }
        coordinate.setY(topLeft.y());
        if (!rightToLeft)
            coordinate.setX(coordinate.x() + d->sheet->columnFormat(col)->width());
    }

    // 2. Paint the cell content including markers (formula, comment, ...)
    for (QList<CellPaintData>::ConstIterator it(cached_cells.constBegin()); it != cached_cells.constEnd(); ++it) {
        it->cellView.paintCellContents(paintRect, painter, clipRect, it->coordinate, it->cell, this);
    }

    // 3. Paint the default borders
    coordinate = startCoordinate;
    processedMergedCells.clear();
    for (int col = visRect.left(); col <= visRect.right(); ++col) {
        if (d->sheet->columnFormat(col)->isHiddenOrFiltered())
            continue;
        if (rightToLeft)
            coordinate.setX(coordinate.x() - d->sheet->columnFormat(col)->width());
        for (int row = visRect.top(); row <= visRect.bottom(); ++row) {
            int lastHiddenRow;
            if (d->sheet->rowFormats()->isHiddenOrFiltered(row, &lastHiddenRow)) {
                row = lastHiddenRow;
                continue;
            }
            // For borders even cells, that are merged in, need to be traversed.
            // Think of a merged cell with a set border and one its neighbours has a thicker border.
            // but: also the master cell of a merged cell always needs to be processed
            const QPointF savedCoordinate = coordinate;
            Cell cell = d->cellToProcess(col, row, coordinate, processedMergedCells, visRect);
            if (!!cell && (cell.column() != col || cell.row() != row)) {
                const CellView cellView = this->cellView(cell.cellPosition());
                cellView.paintDefaultBorders(painter, clipRect, paintRect, coordinate,
                                             CellView::LeftBorder | CellView::RightBorder |
                                             CellView::TopBorder | CellView::BottomBorder,
                                             visRect, cell, this);
            }
            coordinate = savedCoordinate;
            const CellView cellView = this->cellView(col, row);
            cellView.paintDefaultBorders(painter, clipRect, paintRect, coordinate,
                                         CellView::LeftBorder | CellView::RightBorder |
                                         CellView::TopBorder | CellView::BottomBorder,
                                         visRect, Cell(d->sheet, col, row), this);
            coordinate.setY(coordinate.y() + d->sheet->rowFormats()->rowHeight(row));
        }
        coordinate.setY(topLeft.y());
        if (!rightToLeft)
            coordinate.setX(coordinate.x() + d->sheet->columnFormat(col)->width());
    }

    // 4. Paint the custom borders, diagonal lines and page borders
    coordinate = startCoordinate;
    processedMergedCells.clear();
    processedObscuredCells.clear();
    for (int col = visRect.left(); col <= visRect.right(); ++col) {
        if (d->sheet->columnFormat(col)->isHiddenOrFiltered())
            continue;
        if (rightToLeft)
            coordinate.setX(coordinate.x() - d->sheet->columnFormat(col)->width());
        for (int row = visRect.top(); row <= visRect.bottom(); ++row) {
            int lastHiddenRow;
            if (d->sheet->rowFormats()->isHiddenOrFiltered(row, &lastHiddenRow)) {
                row = lastHiddenRow;
                continue;
            }
            // For borders even cells, that are merged in, need to be traversed.
            // Think of a merged cell with a set border and one its neighbours has a thicker border.
            // but: also the master cell of a merged cell always needs to be processed
            const QPointF savedCoordinate = coordinate;
            Cell cell = d->cellToProcess(col, row, coordinate, processedMergedCells, visRect);
            if (!!cell && (cell.column() != col || cell.row() != row)) {
                const CellView cellView = this->cellView(cell.cellPosition());
                cellView.paintCellBorders(paintRect, painter, clipRect, coordinate,
                                          visRect,
                                          cell, this);
            }
            coordinate = savedCoordinate;
            Cell theCell(sheet(), col, row);
            const CellView cellView = d->cellViewToProcess(theCell, coordinate, processedObscuredCells, this, visRect);
            if (!!theCell && (theCell.column() != col || theCell.row() != row)) {
                cellView.paintCellBorders(paintRect, painter, clipRect, coordinate,
                                          visRect,
                                          theCell, this);
            }
            const CellView cellView2 = this->cellView(col, row);
            coordinate = savedCoordinate;
            cellView2.paintCellBorders(paintRect, painter, clipRect, coordinate,
                                      visRect,
                                      Cell(sheet(), col, row), this);
            coordinate.setY(coordinate.y() + d->sheet->rowFormats()->rowHeight(row));
        }
        coordinate.setY(topLeft.y());
        if (!rightToLeft)
            coordinate.setX(coordinate.x() + d->sheet->columnFormat(col)->width());
    }

    // 5. Paint cell highlighting
    if (hasHighlightedCells()) {
        QPointF active = activeHighlight();
        QPainterPath p;
        const CellPaintData* activeData = 0;
        for (QList<CellPaintData>::ConstIterator it(cached_cells.constBegin()); it != cached_cells.constEnd(); ++it) {
            if (isHighlighted(it->cell.cellPosition())) {
                p.addRect(it->coordinate.x(), it->coordinate.y(), it->cellView.cellWidth(), it->cellView.cellHeight());
                if (it->cell.cellPosition() == active) {
                    activeData = &*it;
                }
            }
        }
        painter.setPen(Qt::NoPen);
        if (d->highlightColor.isValid()) {
            painter.setBrush(QBrush(d->highlightColor));
            painter.drawPath(p);
        }
        if (d->highlightMaskColor.isValid()) {
            QPainterPath base;
            base.addRect(painter.clipPath().boundingRect().adjusted(-5, -5, 5, 5));
            p = base.subtracted(p);
            painter.setBrush(QBrush(d->highlightMaskColor));
            painter.drawPath(p);
        }

        if (activeData && d->activeHighlightColor.isValid()) {
            painter.setBrush(QBrush(d->activeHighlightColor));
            painter.setPen(QPen(Qt::black, 0));
            painter.drawRect(QRectF(activeData->coordinate.x(), activeData->coordinate.y(), activeData->cellView.cellWidth(), activeData->cellView.cellHeight()));
        }
    }
}

void SheetView::invalidateRange(const QRect& range)
{
#ifdef CALLIGRA_SHEETS_MT
    QMutexLocker ml(&d->cacheMutex);
#endif
    QRegion obscuredRegion;
    const int right  = range.right();
    for (int col = range.left(); col <= right; ++col) {
        const int bottom = range.bottom();
        for (int row = range.top(); row <= bottom; ++row) {
            const QPoint p(col, row);
            if (!d->cache.contains(p))
                continue;
            if (obscuresCells(p) || isObscured(p)) {
                obscuredRegion += obscuredArea(p);
                obscureCells(p, 0, 0);
            }
            d->cache.remove(p);
        }
    }
    d->cachedArea -= range;
    obscuredRegion &= d->cachedArea;
    foreach (const QRect& rect, obscuredRegion.rects()) {
        invalidateRange(rect);
    }
}

void SheetView::obscureCells(const QPoint &position, int numXCells, int numYCells)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&d->obscuredLock);
#endif
    // Start by un-obscuring cells that we might be obscuring right now
    const QPair<QRectF, bool> pair = d->obscuredInfo->containedPair(position);
    if (!pair.first.isNull())
        d->obscuredInfo->insert(Region(pair.first.toRect()), false);
    // Obscure the cells
    if (numXCells != 0 || numYCells != 0)
        d->obscuredInfo->insert(Region(position.x(), position.y(), numXCells + 1, numYCells + 1), true);

    QRect obscuredArea = d->obscuredInfo->usedArea();
    QSize newObscuredRange(obscuredArea.right(), obscuredArea.bottom());
    if (newObscuredRange != d->obscuredRange) {
        d->obscuredRange = newObscuredRange;
        emit obscuredRangeChanged(d->obscuredRange);
    }
}

QPoint SheetView::obscuringCell(const QPoint &obscuredCell) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker(&d->obscuredLock);
#endif
    const QPair<QRectF, bool> pair = d->obscuredInfo->containedPair(obscuredCell);
    if (pair.first.isNull())
        return obscuredCell;
    if (pair.second == false)
        return obscuredCell;
    return pair.first.toRect().topLeft();
}

QSize SheetView::obscuredRange(const QPoint &obscuringCell) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker(&d->obscuredLock);
#endif
    const QPair<QRectF, bool> pair = d->obscuredInfo->containedPair(obscuringCell);
    if (pair.first.isNull())
        return QSize(0, 0);
    if (pair.second == false)
        return QSize(0, 0);
    // Not the master cell?
    if (pair.first.toRect().topLeft() != obscuringCell)
        return QSize(0, 0);
    return pair.first.toRect().size() - QSize(1, 1);
}

QRect SheetView::obscuredArea(const QPoint &cell) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker(&d->obscuredLock);
#endif
    const QPair<QRectF, bool> pair = d->obscuredInfo->containedPair(cell);
    if (pair.first.isNull())
        return QRect(cell, QSize(1, 1));
    if (pair.second == false)
        return QRect(cell, QSize(1, 1));
    // Not the master cell?
    return pair.first.toRect();
}

bool SheetView::isObscured(const QPoint &cell) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker(&d->obscuredLock);
#endif
    const QPair<QRectF, bool> pair = d->obscuredInfo->containedPair(cell);
    if (pair.first.isNull())
        return false;
    if (pair.second == false)
        return false;
    // master cell?
    if (pair.first.toRect().topLeft() == cell)
        return false;
    return true;
}

bool SheetView::obscuresCells(const QPoint &cell) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker(&d->obscuredLock);
#endif
    const QPair<QRectF, bool> pair = d->obscuredInfo->containedPair(cell);
    if (pair.first.isNull())
        return false;
    if (pair.second == false)
        return false;
    // master cell?
    if (pair.first.toRect().topLeft() != cell)
        return false;
    return true;
}

QSize SheetView::totalObscuredRange() const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker(&d->obscuredLock);
#endif
    return d->obscuredRange;
}

#ifdef CALLIGRA_SHEETS_MT
CellView SheetView::defaultCellView() const
#else
const CellView& SheetView::defaultCellView() const
#endif
{
    return *d->defaultCellView;
}

void SheetView::updateAccessedCellRange(const QPoint& location)
{
    const QSize cellRange = d->accessedCellRange.expandedTo(QSize(location.x(), location.y()));
    if (d->accessedCellRange != cellRange || location.isNull()) {
        d->accessedCellRange = cellRange;
        const int col = qMin(KS_colMax, cellRange.width() + 10);
        const int row = qMin(KS_rowMax, cellRange.height() + 10);
        const double width = sheet()->columnPosition(col) + sheet()->columnFormat(col)->width();
        const double height = sheet()->rowPosition(row) + sheet()->rowFormats()->rowHeight(row);
        emit visibleSizeChanged(QSizeF(width, height));
    }
}

CellView* SheetView::createDefaultCellView()
{
    return new CellView(this);
}

CellView* SheetView::createCellView(int col, int row)
{
    return new CellView(this, col, row);
}

bool SheetView::isHighlighted(const QPoint &cell) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker(&d->highlightLock);
#endif
    return d->highlightedCells.lookup(cell.x(), cell.y());
}

void SheetView::setHighlighted(const QPoint &cell, bool isHighlighted)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&d->highlightLock);
#endif
    bool oldHadHighlights = d->highlightedCells.count() > 0;
    bool oldVal;
    if (isHighlighted) {
        oldVal = d->highlightedCells.insert(cell.x(), cell.y(), true);
    } else {
        oldVal = d->highlightedCells.take(cell.x(), cell.y());
    }
    if (oldHadHighlights != (d->highlightedCells.count() > 0)) {
        invalidate();
    } else if (oldVal != isHighlighted) {
        invalidateRegion(Region(cell));
    }
}

bool SheetView::hasHighlightedCells() const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker(&d->highlightLock);
#endif
    return d->highlightedCells.count() > 0;
}

void SheetView::clearHighlightedCells()
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&d->highlightLock);
#endif
    d->activeHighlight = QPoint();
    if (d->highlightedCells.count()) {
        d->highlightedCells.clear();
        invalidate();
    }
}

QPoint SheetView::activeHighlight() const
{
    return d->activeHighlight;
}

void SheetView::setActiveHighlight(const QPoint &cell)
{
    QPoint oldVal = d->activeHighlight;
    d->activeHighlight = cell;
    if (oldVal != cell) {
        Region r;
        if (!oldVal.isNull()) r.add(oldVal);
        if (!cell.isNull()) r.add(cell);
        invalidateRegion(r);
    }
}

void SheetView::setHighlightColor(const QColor &color)
{
    d->highlightColor = color;
    if (hasHighlightedCells()) {
        invalidate();
    }
}

void SheetView::setHighlightMaskColor(const QColor &color)
{
    d->highlightMaskColor = color;
    if (hasHighlightedCells()) {
        invalidate();
    }
}

void SheetView::setActiveHighlightColor(const QColor &color)
{
    d->activeHighlightColor = color;
    if (hasHighlightedCells()) {
        invalidate();
    }
}
