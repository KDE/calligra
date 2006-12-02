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

using namespace KSpread;

inline uint qHash( const QPoint& point )
{
    return ( static_cast<uint>( point.x() ) << 16 ) + static_cast<uint>( point.y() );
}

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

    // 1. Paint the cell content, background, ... (except borders)
    KoPoint dblCorner( topLeft.x(), topLeft.y() );
    int right = d->visibleRect.right();
    for ( int col = d->visibleRect.left(); col <= right; ++col )
    {
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
        dblCorner.setX( dblCorner.x() + d->sheet->columnFormat( col )->dblWidth() );
    }
#if 0
    // 2. Paint the default borders
    dblCorner = KoPoint( topLeft.x(), topLeft.y() );
    right = d->visibleRect.right();
    for ( int col = d->visibleRect.left(); col <= right; ++col )
    {
        int bottom = d->visibleRect.bottom();
        for ( int row = d->visibleRect.top(); row <= bottom; ++row )
        {
            CellView cellView = this->cellView( col, row );
            cellView.paintDefaultBorders( painter, paintRect, dblCorner,
                                         QPoint( col, row ), QRect( 1, 1, KS_colMax, KS_rowMax ),
                                         mergedCellsPainted, sheet()->cellAt( col, row ), this );
            dblCorner.setY( dblCorner.y() + d->sheet->rowFormat( row )->dblHeight() );
        }
        dblCorner.setY( topLeft.y() );
        dblCorner.setX( dblCorner.x() + d->sheet->columnFormat( col )->dblWidth() );
    }
#endif
    // 3. Paint the custom borders, diagonal lines and page borders
    dblCorner = KoPoint( topLeft.x(), topLeft.y() );
    right = d->visibleRect.right();
    for ( int col = d->visibleRect.left(); col <= right; ++col )
    {
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
        dblCorner.setX( dblCorner.x() + d->sheet->columnFormat( col )->dblWidth() );
    }
}

