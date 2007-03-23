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

#include <QPainter>

#include <kdebug.h>

#include <CellView.h>
#include <Doc.h>
#include <Map.h>
#include <RowColumnFormat.h>
#include <Sheet.h>
#include <SheetView.h>

#include "TableShape.h"

using namespace KSpread;

class TableShape::Private
{
public:
    int         columns;
    int         rows;
    Doc*        doc;
    SheetView*  sheetView;

public:
    Sheet* sheet() const;
    void adjustColumnDimensions( double factor );
    void adjustRowDimensions( double factor );
};


Sheet* TableShape::Private::sheet() const
{
    return doc->map()->sheet( 0 );
}

void TableShape::Private::adjustColumnDimensions( double factor )
{
    doc->setDefaultColumnWidth( doc->defaultColumnFormat()->width() * factor );
    for ( ColumnFormat* columnFormat = sheet()->firstCol(); columnFormat; columnFormat->next() )
    {
        if ( columnFormat->column() > columns )
            break;
        columnFormat->setWidth( columnFormat->width() * factor );
    }
}

void TableShape::Private::adjustRowDimensions( double factor )
{
    doc->setDefaultRowHeight( doc->defaultRowFormat()->height() * factor );
    for ( RowFormat* rowFormat = sheet()->firstRow(); rowFormat; rowFormat->next() )
    {
        if ( rowFormat->row() > rows )
            break;
        rowFormat->setHeight( rowFormat->height() * factor );
    }
}



TableShape::TableShape( int columns, int rows )
    : d( new Private )
{
    d->columns  = 1;
    d->rows     = 1;
    d->doc      = new Doc();
    d->doc->map()->addNewSheet();
    d->sheetView = new SheetView( sheet() );

    // initialize the default column width / row height
    d->doc->setDefaultColumnWidth( size().width() );
    d->doc->setDefaultRowHeight( size().height() );

    setColumns( columns );
    setRows( rows );

    Cell( sheet(), 1, 1 ).setCellText( "42" );
}

TableShape::~TableShape()
{
    delete d->sheetView;
    delete d->doc;
    delete d;
}

int TableShape::columns() const
{
    return d->columns;
}

int TableShape::rows() const
{
    return d->rows;
}

void TableShape::setColumns( int columns )
{
    Q_ASSERT( columns > 0 );
    const double factor = (double) d->columns / columns;
    d->columns = columns;
    d->adjustColumnDimensions( factor );
    d->sheetView->invalidate();
}

void TableShape::setRows( int rows )
{
    Q_ASSERT( rows > 0 );
    const double factor = (double) d->rows / rows;
    d->rows = rows;
    d->adjustRowDimensions( factor );
    d->sheetView->invalidate();
}

void TableShape::paint( QPainter& painter, const KoViewConverter& converter )
{
    const QRectF paintRect = QRectF( QPointF( 0.0, 0.0 ), size() );

    applyConversion( painter, converter );
    painter.setClipRect( paintRect, Qt::IntersectClip );

    // painting cell contents
    d->sheetView->setPaintCellRange( QRect( 1, 1, d->columns, d->rows ) );
    d->sheetView->paintCells( 0 /*paintDevice*/, painter, paintRect, QPointF( 0.0, 0.0 ) );
}

void TableShape::resize( const QSizeF& newSize )
{
    if ( size() == newSize )
        return;

    // adjust the column widths / row heights
    d->adjustColumnDimensions( newSize.width() / size().width() );
    d->adjustRowDimensions( newSize.height() / size().height() );
    d->sheetView->invalidate();

    KoShape::resize( newSize );
}

Sheet* TableShape::sheet() const
{
    return d->sheet();
}
