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

#include <QCache>
#include <QList>
#include <QRect>

#include "Canvas.h"
#include "CellView.h"
#include "Region.h"
#include "RectStorage.h"
#include "RowColumnFormat.h"
#include "Sheet.h"

#include "SheetView.h"

using namespace KSpread;

class SheetView::Private
{
public:
    const Sheet* sheet;
    QPaintDevice* paintDevice;
    QRect visibleRect;
    QCache<QPoint, CellView> cache;
    QRegion cachedArea;
    CellView* defaultCellView;
};

SheetView::SheetView( const Sheet* sheet, QPaintDevice* paintDevice )
    : d( new Private )
{
    d->sheet = sheet;
    d->paintDevice = paintDevice;
    d->visibleRect = QRect(1,1,0,0);
    d->cache.setMaxCost( 10000 );
    d->defaultCellView = new CellView( this );
}

SheetView::~SheetView()
{
    delete d->defaultCellView;
    delete d;
}

const Sheet* SheetView::sheet() const
{
    return d->sheet;
}

QPaintDevice* SheetView::paintDevice() const
{
    return d->paintDevice;
}

const CellView& SheetView::cellView( int col, int row )
{
    Q_ASSERT( 1 <= col && col <= KS_colMax );
    Q_ASSERT( 1 <= row && col <= KS_rowMax );
    if ( !d->cache.contains( QPoint(col,row) ) )
    {
        d->cache.insert( QPoint(col,row), new CellView( this, col, row ) );
        d->cachedArea += QRect( col, row, 1, 1 );
    }
    return *d->cache.object( QPoint(col,row) );
}

void SheetView::setPaintCellRange( const QRect& rect )
{
    d->visibleRect = rect;
    d->cache.setMaxCost( 2 * rect.width() * rect.height() );
}

void SheetView::invalidateRegion( const Region& region )
{
    QRegion qregion;
    Region::ConstIterator end(region.constEnd());
    for (Region::ConstIterator it(region.constBegin()); it != end; ++it)
    {
        qregion += (*it)->rect();
    }
    // reduce to the cached area
    qregion &= d->cachedArea;
    QVector<QRect> rects = qregion.rects();
    for ( int i = 0; i < rects.count(); ++i )
        invalidateRange( rects[i] );
}

void SheetView::invalidate()
{
    delete d->defaultCellView;
    d->defaultCellView = new CellView( this );
    d->cache.clear();
    d->cachedArea = QRegion();
}

void SheetView::paintCells( QPaintDevice* paintDevice, QPainter& painter, const QRectF& paintRect,
                            const QPointF& topLeft )
{
    // NOTE Stefan: The painting is splitted into several steps. In each of these all cells in
    //              d->visibleRect are traversed. This may appear suboptimal at the first look, but
    //              ensures that the borders are not erased by the background of adjacent cells.

    QLinkedList<QPoint> mergedCellsPainted;
// kDebug() << "paintRect: " << paintRect << endl;
// kDebug() << "topLeft: " << topLeft << endl;
    // 1. Paint the cell background

    // Handle right-to-left layout.
    // In an RTL sheet the cells have to be painted at their opposite horizontal
    // location on the canvas, meaning that column A will be the rightmost column
    // on screen, column B will be to the left of it and so on. Here we change
    // the horizontal coordinate at which we start painting the cell in case the
    // sheet's direction is RTL. We do this only if paintingObscured is 0,
    // otherwise the cell's painting location will flip back and forth in
    // consecutive calls to paintCell when painting obscured cells.
    const bool rightToLeft = sheet()->layoutDirection() == Qt::RightToLeft;
    QPointF offset( rightToLeft ? paintRect.width() - topLeft.x() : topLeft.x(), topLeft.y() );
    int right = d->visibleRect.right();
    for ( int col = d->visibleRect.left(); col <= right; ++col )
    {
        if ( rightToLeft )
            offset.setX( offset.x() - d->sheet->columnFormat( col )->width() );
// kDebug() << "offset: " << offset << endl;
        int bottom = d->visibleRect.bottom();
        for ( int row = d->visibleRect.top(); row <= bottom; ++row )
        {
            CellView cellView = this->cellView( col, row );
            cellView.paintCellBackground( painter, offset );
            offset.setY( offset.y() + d->sheet->rowFormat( row )->height() );
        }
        offset.setY( topLeft.y() );
        if ( !rightToLeft )
            offset.setX( offset.x() + d->sheet->columnFormat( col )->width() );
    }

    // 2. Paint the cell content including markers (formula, comment, ...)
    offset = QPointF( rightToLeft ? paintRect.width() - topLeft.x() : topLeft.x(), topLeft.y() );
    right = d->visibleRect.right();
    for ( int col = d->visibleRect.left(); col <= right; ++col )
    {
        if ( rightToLeft )
            offset.setX( offset.x() - d->sheet->columnFormat( col )->width() );
        int bottom = d->visibleRect.bottom();
        for ( int row = d->visibleRect.top(); row <= bottom; ++row )
        {
            CellView cellView = this->cellView( col, row );
            cellView.paintCellContents( paintRect, painter, paintDevice, offset,
                                        QPoint( col, row ), mergedCellsPainted,
                                        Cell( sheet(), col, row ) );
            offset.setY( offset.y() + d->sheet->rowFormat( row )->height() );
        }
        offset.setY( topLeft.y() );
        if ( !rightToLeft )
            offset.setX( offset.x() + d->sheet->columnFormat( col )->width() );
    }

    // 3. Paint the default borders
    offset = QPointF( rightToLeft ? paintRect.width() - topLeft.x() : topLeft.x(), topLeft.y() );
    right = d->visibleRect.right();
    for ( int col = d->visibleRect.left(); col <= right; ++col )
    {
        if ( rightToLeft )
            offset.setX( offset.x() - d->sheet->columnFormat( col )->width() );
        int bottom = d->visibleRect.bottom();
        for ( int row = d->visibleRect.top(); row <= bottom; ++row )
        {
            Cell cell = Cell( sheet(), col, row );
            const QRectF cellRect = QRectF( offset.x(), offset.y(), cell.width(), cell.height() );
            CellView cellView = this->cellView( col, row );
            cellView.paintDefaultBorders( painter, paintRect, cellRect, QPoint( col, row ),
                                          CellView::LeftBorder | CellView::RightBorder |
                                          CellView::TopBorder | CellView::BottomBorder,
                                          d->visibleRect, cell, this );
            offset.setY( offset.y() + d->sheet->rowFormat( row )->height() );
        }
        offset.setY( topLeft.y() );
        if ( !rightToLeft )
            offset.setX( offset.x() + d->sheet->columnFormat( col )->width() );
    }

    // 4. Paint the custom borders, diagonal lines and page borders
    offset = QPointF( rightToLeft ? paintRect.width() - topLeft.x() : topLeft.x(), topLeft.y() );
    right = d->visibleRect.right();
    for ( int col = d->visibleRect.left(); col <= right; ++col )
    {
        if ( rightToLeft )
            offset.setX( offset.x() - d->sheet->columnFormat( col )->width() );
        int bottom = d->visibleRect.bottom();
        for ( int row = d->visibleRect.top(); row <= bottom; ++row )
        {
            CellView cellView = this->cellView( col, row );
            cellView.paintCellBorders( paintRect, painter, offset,
                                       QPoint( col, row ), d->visibleRect,
                                       mergedCellsPainted, Cell( sheet(), col, row ), this );
            offset.setY( offset.y() + d->sheet->rowFormat( row )->height() );
        }
        offset.setY( topLeft.y() );
        if ( !rightToLeft )
            offset.setX( offset.x() + d->sheet->columnFormat( col )->width() );
    }
}

void SheetView::invalidateRange( const QRect& range )
{
    const int right  = range.right();
    for ( int col = range.left(); col <= right; ++col )
    {
        const int bottom = range.bottom();
        for ( int row = range.top(); row <= bottom; ++row )
        {
            if ( !d->cache.contains( QPoint(col,row) ) )
                continue;
            const CellView cellView = this->cellView( col, row );
            if ( cellView.obscuresCells() )
                invalidateRange( QRect( range.topLeft(), cellView.obscuredRange() ) );
            else if ( cellView.isObscured() )
                if ( !range.contains( cellView.obscuringCell() ) )
                    invalidateRange( QRect( cellView.obscuringCell(), QSize( 1, 1 ) ) );
            d->cache.remove( QPoint(col,row) );
            d->cachedArea -= QRect( col, row, 1, 1 );
        }
    }
}

void SheetView::obscureCells( const QRect& range, const QPoint& position )
{
    const int right = range.right();
    const int bottom = range.bottom();
    for ( int col = range.left(); col <= right; ++col )
    {
        for ( int row = range.top(); row <= bottom; ++row )
        {
            // create the CellView, but do not use the returned CellView. It is shared!
            cellView( col, row );
            // alter the CellView directly instead
            d->cache.object( QPoint(col,row) )->obscure( position.x(), position.y() );
        }
    }
}

const CellView& SheetView::defaultCellView() const
{
    return *d->defaultCellView;
}
