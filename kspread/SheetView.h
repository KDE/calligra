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

#ifndef KSPREAD_SHEET_VIEW
#define KSPREAD_SHEET_VIEW

#include "CellView.h"

class QPainter;
class QPointF;
class QRect;
class QRectF;

namespace KSpread
{
class Canvas;
class Region;
class Sheet;
class View;

/**
 * The SheetView controls the painting of the sheets' cells.
 * It caches a set of CellViews.
 */
class KSPREAD_EXPORT SheetView
{
public:
    /**
     * Constructor.
     */
    explicit SheetView( const Sheet* sheet );

    /**
     * Destructor.
     */
    ~SheetView();

    /**
     * Looks up a CellView for the position \p col , \p row in the cache.
     * If no CellView exists yet, one is created and inserted into the cache.
     *
     * \return the CellView for the position
     */
    CellView cellView( int col, int row );

    /**
     * Set the cell range, that should be painted to \p rect .
     * It also adjusts the cache size linear to the size of \p rect .
     */
    void setPaintCellRange( const QRect& rect );

    /**
     * Invalidates all cached CellViews in \p region .
     */
    void invalidateRegion( const Region& region );

    /**
     * Paints the cells.
     */
    void paintCells( View* view, QPainter& painter, const QRectF& paintRect, const QPointF& topLeft );

private:
    class Private;
    Private * const d;
};

} // namespace KSpread

#endif // KSPREAD_SHEET_VIEW
