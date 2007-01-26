/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#ifndef KSPREAD_CELL_STORAGE
#define KSPREAD_CELL_STORAGE

#include <QPair>
#include <QRect>
#include <QVector>

namespace KSpread
{
class Cell;
class CommentStorage;
class ConditionsStorage;
class FormulaStorage;
class FusionStorage;
class LinkStorage;
class Region;
class Sheet;
class StyleStorage;
class ValidityStorage;
class ValueStorage;

/**
 * The cell storage.
 * A wrapper around a couple of storages, which hold the cell data.
 * Provides methods to iterate over the non-empty cells.
 *
 * \author Stefan Nikolaus <stefan.nikolaus@kdemail.net>
 * \since 2.0
 *
 * \note If you fill the storage, do it row-wise. That's more performant.
 */
class CellStorage
{
public:
    /**
     * Constructor.
     * Creates an empty storage. Actually, does nothing.
     */
    CellStorage( Sheet* sheet );

    /**
     * Destructor.
     */
    ~CellStorage();

    /**
     * Clears the storage.
     */
    void clear();

    /**
     * Returns the number of items in the storage.
     * Usable to iterate over all non-default data.
     * \return number of items
     * \see col()
     * \see row()
     * \see data()
     */
    int count() const;

    /**
     * Inserts \p data at \p col , \p row .
     * \return the overridden data (default data, if no overwrite)
     */
    Cell insert( int col, int row, Cell data );

    /**
     * Looks up the data at \p col , \p row . If no data was found returns a
     * default object.
     * \return the data at the given coordinate
     */
    Cell lookup( int col, int row ) const;

    /**
     * Removes data at \p col , \p row .
     * \return the removed data (default data, if none)
     */
    Cell take( int col, int row );

    /**
     * Insert \p number columns at \p position .
     * \return the data, that became out of range (shifted over the end)
     */
    QVector< QPair<QPoint,Cell> > insertColumns( int position, int number = 1 );

    /**
     * Removes \p number columns at \p position .
     * \return the removed data
     */
    QVector< QPair<QPoint,Cell> > removeColumns( int position, int number = 1 );

    /**
     * Insert \p number rows at \p position .
     * \return the data, that became out of range (shifted over the end)
     */
    QVector< QPair<QPoint,Cell> > insertRows( int position, int number = 1 );

    /**
     * Removes \p number rows at \p position .
     * \return the removed data
     */
    QVector< QPair<QPoint,Cell> > removeRows( int position, int number = 1 );

    /**
     * Shifts the data right of \p rect to the left by the width of \p rect .
     * The data formerly contained in \p rect becomes overridden.
     * \return the removed data
     */
    QVector< QPair<QPoint,Cell> > removeShiftLeft( const QRect& rect );

    /**
     * Shifts the data in and right of \p rect to the right by the width of \p rect .
     * \return the data, that became out of range (shifted over the end)
     */
    QVector< QPair<QPoint,Cell> > insertShiftRight( const QRect& rect );

    /**
     * Shifts the data below \p rect to the top by the height of \p rect .
     * The data formerly contained in \p rect becomes overridden.
     * \return the removed data
     */
    QVector< QPair<QPoint,Cell> > removeShiftUp( const QRect& rect );

    /**
     * Shifts the data in and below \p rect to the bottom by the height of \p rect .
     * \return the data, that became out of range (shifted over the end)
     */
    QVector< QPair<QPoint,Cell> > insertShiftDown( const QRect& rect );

    /**
     * Retrieve the first used data in \p col .
     * Can be used in conjunction with nextInColumn() to loop through a column.
     * \return the first used data in \p col or the default data, if the column is empty.
     */
    Cell firstInColumn( int col ) const;

    /**
     * Retrieve the first used data in \p row .
     * Can be used in conjunction with nextInRow() to loop through a row.
     * \return the first used data in \p row or the default data, if the row is empty.
     */
    Cell firstInRow( int row ) const;

    /**
     * Retrieve the last used data in \p col .
     * Can be used in conjunction with prevInColumn() to loop through a column.
     * \return the last used data in \p col or the default data, if the column is empty.
     */
    Cell lastInColumn( int col ) const;

    /**
     * Retrieve the last used data in \p row .
     * Can be used in conjunction with prevInRow() to loop through a row.
     * \return the last used data in \p row or the default data, if the row is empty.
     */
    Cell lastInRow( int row ) const;

    /**
     * Retrieve the next used data in \p col after \p row .
     * Can be used in conjunction with firstInColumn() to loop through a column.
     * \return the next used data in \p col or the default data, there is no further data.
     */
    Cell nextInColumn( int col, int row ) const;

    /**
     * Retrieve the next used data in \p row after \p col .
     * Can be used in conjunction with firstInRow() to loop through a row.
     * \return the next used data in \p row or the default data, if there is no further data.
     */
    Cell nextInRow( int col, int row ) const;

    /**
     * Retrieve the previous used data in \p col after \p row .
     * Can be used in conjunction with lastInColumn() to loop through a column.
     * \return the previous used data in \p col or the default data, there is no further data.
     */
    Cell prevInColumn( int col, int row ) const;

    /**
     * Retrieve the previous used data in \p row after \p col .
     * Can be used in conjunction with lastInRow() to loop through a row.
     * \return the previous used data in \p row or the default data, if there is no further data.
     */
    Cell prevInRow( int col, int row ) const;

    /**
     * The maximum occupied column, i.e. the horizontal storage dimension.
     * \return the maximum column
     */
    int columns() const;

    /**
     * The maximum occupied row, i.e. the vertical storage dimension.
     * \return the maximum row
     */
    int rows() const;

    /**
     * Creates a substorage consisting of the values in \p region.
     * \return a subset of the storage stripped down to the values in \p region
     */
    CellStorage subStorage( const Region& region ) const;

    CommentStorage* commentStorage() const;
    ConditionsStorage* conditionsStorage() const;
    FormulaStorage* formulaStorage() const;
    FusionStorage* fusionStorage() const;
    LinkStorage* linkStorage() const;
    StyleStorage* styleStorage() const;
    ValidityStorage* validityStorage() const;
    ValueStorage* valueStorage() const;

    /**
     * Equality operator.
     */
    bool operator==( const CellStorage& o ) const;

private:
    class Private;
    Private * const d;
};

} // namespace KSpread

#endif // KSPREAD_CELL_STORAGE
