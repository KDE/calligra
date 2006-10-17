/* This file is part of the KDE project
   Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
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
#include <Format.h>
#include <Map.h>
#include <Sheet.h>

#include "TableShape.h"

using namespace KSpread;

class TableShape::Private
{
public:
    int     columns;
    int     rows;
    Doc*    doc;
    QList< /*columns*/ QList< /*rows*/ CellView* > > cellWindowMatrix;
};

TableShape::TableShape( int columns, int rows )
    : d( new Private )
{
    d->columns  = 0;
    d->rows     = 0;
    d->doc      = new Doc();
    d->doc->map()->addNewSheet();

    setColumns( columns );
    setRows( rows );

    sheet()->nonDefaultCell(1,1)->setCellText( "42" );
}

TableShape::~TableShape()
{
    foreach ( QList<CellView*> row, d->cellWindowMatrix )
        qDeleteAll( row );
    delete d->doc;
    delete d;
}

void TableShape::setColumns( int columns )
{
    Q_ASSERT( columns > 0 );

    // create the new matrix, copy existing views and create missing ones
    QList< QList<CellView*> > newMatrix;
    for ( int col = 1; col <= columns; ++col )
    {
        QList<CellView*> matrixRow;
        for ( int row = 1; row <= d->rows; ++row )
            matrixRow.append( new CellView( sheet(), col, row ) );
        newMatrix.append( matrixRow );
    }
    // delete the unused CellViews
    for ( int col = 1; col <= d->columns; ++col )
        for ( int row = 1; row <= d->rows; ++row )
            if ( col > columns )
                delete d->cellWindowMatrix[col-1][row-1];

    d->cellWindowMatrix = newMatrix;
    d->columns = columns;
}

void TableShape::setRows( int rows )
{
    Q_ASSERT( rows > 0 );

    // create the new matrix, copy existing views and create missing ones
    QList< QList<CellView*> > newMatrix;
    for ( int col = 1; col <= d->columns; ++col )
    {
        QList<CellView*> matrixRow;
        for ( int row = 1; row <= rows; ++row )
        {
            if ( row <= d->rows )
                matrixRow.append( d->cellWindowMatrix[col-1][row-1] );
            else
                matrixRow.append( new CellView( sheet(), col, row ) );
        }
        newMatrix.append( matrixRow );
    }
    // delete the unused CellViews
    for ( int col = 1; col <= d->columns; ++col )
        for ( int row = 1; row <= d->rows; ++row )
            if ( row > rows )
                delete d->cellWindowMatrix[col-1][row-1];

    d->cellWindowMatrix = newMatrix;
    d->rows = rows;
}

void TableShape::paint( QPainter& painter, const KoViewConverter& converter )
{
    Q_UNUSED( converter );

    const QRectF unzoomedRect = QRectF( QPointF( 0.0, 0.0 ), size() );

    painter.setClipRect( unzoomedRect, Qt::IntersectClip );

    // painting cell contents
    KoPoint dblCorner( 0.0, 0.0 );
    QLinkedList<QPoint> mergedCellsPainted;
    for ( int col = 1; col <= d->columns; ++col )
    {
        for ( int row = 1; row <= d->rows; ++row )
        {
            // relayout in CellView
            CellView* cellView = d->cellWindowMatrix[col-1][row-1];
            cellView->paintCell( unzoomedRect, painter, 0 /*view*/, dblCorner,
                                 QPoint( col, row ), mergedCellsPainted );

            dblCorner.setY( dblCorner.y() + sheet()->rowFormat( row )->dblHeight() );
        }
        dblCorner.setX( dblCorner.x() + sheet()->columnFormat( col )->dblWidth() );
        dblCorner.setY( 0.0 );
    }
    // painting cell borders
    dblCorner = KoPoint( 0.0, 0.0 );
    mergedCellsPainted.clear();
    for ( int col = 1; col <= d->columns; ++col )
    {
        for ( int row = 1; row <= d->rows; ++row )
        {
            // relayout in CellView
            CellView* cellView = d->cellWindowMatrix[col-1][row-1];
            cellView->paintCellBorders( unzoomedRect, painter, 0 /*view*/, dblCorner,
                                        QPoint( col, row ), QRect( 1, 1, d->columns, d->rows ),
                                        mergedCellsPainted );

            dblCorner.setY( dblCorner.y() + sheet()->rowFormat( row )->dblHeight() );
        }
        dblCorner.setX( dblCorner.x() + sheet()->columnFormat( col )->dblWidth() );
        dblCorner.setY( 0.0 );
    }
}

Sheet* TableShape::sheet() const
{
    return d->doc->map()->sheet( 0 );
}
