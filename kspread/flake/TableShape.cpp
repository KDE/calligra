/* This file is part of the KDE project
   Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; only
   version 2 of the License.

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
};

TableShape::TableShape( int columns, int rows )
    : d( new Private )
{
    d->columns  = 0;
    d->rows     = 0;
    d->doc      = new Doc();
    d->doc->map()->addNewSheet();
    d->sheetView = new SheetView( sheet() );

    setColumns( columns );
    setRows( rows );

    sheet()->nonDefaultCell(1,1)->setCellText( "42" );
}

TableShape::~TableShape()
{
    delete d->sheetView;
    delete d->doc;
    delete d;
}

void TableShape::setColumns( int columns )
{
    Q_ASSERT( columns > 0 );
    d->columns = columns;
}

void TableShape::setRows( int rows )
{
    Q_ASSERT( rows > 0 );
    d->rows = rows;
}

void TableShape::paint( QPainter& painter, const KoViewConverter& converter )
{
    const QRectF paintRect = QRectF( QPointF( 0.0, 0.0 ), size() );

    applyConversion( painter, converter );
    painter.setClipRect( paintRect, Qt::IntersectClip );

    // painting cell contents
    QPointF dblCorner( 0.0, 0.0 );
    d->sheetView->setPaintCellRange( QRect( 1, 1, d->columns, d->rows ) );
    d->sheetView->paintCells( 0 /*view*/, painter, paintRect, QPointF( 0.0, 0.0 ) );
}

Sheet* TableShape::sheet() const
{
    return d->doc->map()->sheet( 0 );
}
