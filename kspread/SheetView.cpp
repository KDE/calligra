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
#include "Format.h"
#include "Region.h"
#include "Sheet.h"
#include "View.h"

#include "SheetView.h"
#include "Storage.h"

using namespace KSpread;

class SheetView::Private
{
public:
    const Sheet* sheet;
    QRect visibleRect;
    QCache<QPoint, CellView> cache;
};

SheetView::SheetView( const Sheet* sheet )
    : d( new Private )
{
    d->sheet = sheet;
    d->visibleRect = QRect(1,1,0,0);
    d->cache.setMaxCost( 10000 );
}

SheetView::~SheetView()
{
    delete d;
}

const Sheet* SheetView::sheet() const
{
    return d->sheet;
}

CellView SheetView::cellView( int col, int row )
{
    if ( !d->cache.contains( QPoint(col,row) ) )
        d->cache.insert( QPoint(col,row), new CellView( this, col, row ) );
    return *d->cache.object( QPoint(col,row) );
}

void SheetView::setPaintCellRange( const QRect& rect )
{
    d->visibleRect = rect;
    d->cache.setMaxCost( 2 * rect.width() * rect.height() );
}

void SheetView::invalidateRegion( const Region& region )
{
    Region::ConstIterator end(region.constEnd());
    for (Region::ConstIterator it(region.constBegin()); it != end; ++it)
    {
        const QRect range = (*it)->rect() & d->visibleRect;
        int right  = range.right();
        for ( int col = range.left(); col <= right; ++col )
        {
            int bottom = range.bottom();
            for ( int row = range.top(); row <= bottom; ++row )
            {
                d->cache.remove( QPoint(col,row) );
            }
        }
    }
}

void SheetView::paintCells( View* view, QPainter& painter, const QRectF& paintRect, const QPointF& topLeft )
{
    QLinkedList<QPoint> mergedCellsPainted;
// kDebug() << "paintRect: " << paintRect << endl;
// kDebug() << "topLeft: " << topLeft << endl;
    // 1. Paint the cell content, background, ... (except borders)

    // Handle right-to-left layout.
    // In an RTL sheet the cells have to be painted at their opposite horizontal
    // location on the canvas, meaning that column A will be the rightmost column
    // on screen, column B will be to the left of it and so on. Here we change
    // the horizontal coordinate at which we start painting the cell in case the
    // sheet's direction is RTL. We do this only if paintingObscured is 0,
    // otherwise the cell's painting location will flip back and forth in
    // consecutive calls to paintCell when painting obscured cells.
    const bool rightToLeft = sheet()->layoutDirection() == Sheet::RightToLeft;
    QPointF dblCorner( rightToLeft ? paintRect.width() - topLeft.x() : topLeft.x(), topLeft.y() );
    int right = d->visibleRect.right();
    for ( int col = d->visibleRect.left(); col <= right; ++col )
    {
        if ( rightToLeft )
            dblCorner.setX( dblCorner.x() - d->sheet->columnFormat( col )->dblWidth() );
// kDebug() << "dblCorner: " << dblCorner << endl;
        int bottom = d->visibleRect.bottom();
        for ( int row = d->visibleRect.top(); row <= bottom; ++row )
        {
            CellView cellView = this->cellView( col, row );
            cellView.paintCell( paintRect, painter, view, dblCorner,
                                QPoint( col, row ), mergedCellsPainted,
                                sheet()->cellAt( col, row ) );
            dblCorner.setY( dblCorner.y() + d->sheet->rowFormat( row )->dblHeight() );
        }
        dblCorner.setY( topLeft.y() );
        if ( !rightToLeft )
            dblCorner.setX( dblCorner.x() + d->sheet->columnFormat( col )->dblWidth() );
    }

    // 2. Paint the default borders
    dblCorner = QPointF( rightToLeft ? paintRect.width() - topLeft.x() : topLeft.x(), topLeft.y() );
    right = d->visibleRect.right();
    for ( int col = d->visibleRect.left(); col <= right; ++col )
    {
        if ( rightToLeft )
            dblCorner.setX( dblCorner.x() - d->sheet->columnFormat( col )->dblWidth() );
        int bottom = d->visibleRect.bottom();
        for ( int row = d->visibleRect.top(); row <= bottom; ++row )
        {
            Cell* const cell = sheet()->cellAt( col, row );
            const QRectF cellRect = QRectF( dblCorner.x(), dblCorner.y(), cell->dblWidth(col), cell->dblHeight(row) );
            CellView cellView = this->cellView( col, row );
            cellView.paintDefaultBorders( painter, paintRect, cellRect, QPoint( col, row ),
                                          CellView::LeftBorder | CellView::RightBorder |
                                          CellView::TopBorder | CellView::BottomBorder,
                                          d->visibleRect, cell, this );
            dblCorner.setY( dblCorner.y() + d->sheet->rowFormat( row )->dblHeight() );
        }
        dblCorner.setY( topLeft.y() );
        if ( !rightToLeft )
            dblCorner.setX( dblCorner.x() + d->sheet->columnFormat( col )->dblWidth() );
    }

    // 3. Paint the custom borders, diagonal lines and page borders
    dblCorner = QPointF( rightToLeft ? paintRect.width() - topLeft.x() : topLeft.x(), topLeft.y() );
    right = d->visibleRect.right();
    for ( int col = d->visibleRect.left(); col <= right; ++col )
    {
        if ( rightToLeft )
            dblCorner.setX( dblCorner.x() - d->sheet->columnFormat( col )->dblWidth() );
        int bottom = d->visibleRect.bottom();
        for ( int row = d->visibleRect.top(); row <= bottom; ++row )
        {
            CellView cellView = this->cellView( col, row );
            cellView.paintCellBorders( paintRect, painter, dblCorner,
                                       QPoint( col, row ), d->visibleRect,
                                       mergedCellsPainted, sheet()->cellAt( col, row ), this );
            dblCorner.setY( dblCorner.y() + d->sheet->rowFormat( row )->dblHeight() );
        }
        dblCorner.setY( topLeft.y() );
        if ( !rightToLeft )
            dblCorner.setX( dblCorner.x() + d->sheet->columnFormat( col )->dblWidth() );
    }
}

