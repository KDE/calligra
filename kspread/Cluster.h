/* This file is part of the KDE project
   Copyright (C) 2000 Torben Weis <weis@kde.org>

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

#ifndef CLUSTER_H
#define CLUSTER_H

#include "Value.h"

#define KSPREAD_CLUSTER_LEVEL1 256
#define KSPREAD_CLUSTER_LEVEL2 256
#define KSPREAD_CLUSTER_MAX (256*256)

class QPoint;

namespace KSpread
{
class Cell;
class ColumnFormat;
class RowFormat;

/**
\ingroup Storage
This class defines a pointer map to all cells, which makes access to them more performant
and additionally limits memory consumption.

In detail: The class defines 2 cluster, where the second cluster (LEVEL2) is a matrix for
single cells, while the first cluster (LEVEL1) is a matrix to handle the matrices of LEVEL2.
On initialization, one LEVEL1 matrix is generated only.
Each time, a cell stores something, this class checks if for the given column and row a
matrix of LEVEL2 is already initialized and in case not generates it on the fly.
This helps to reduce the memory usage to only consum one pointer matrix for LEVEL1 and all
matrices of LEVEL2 that are necessary.

LEVEL1 is defined as 128x128 matrix.
LEVEL2 is defined as 256x256 matrices.
Each direction then can have LEVEL1 * LEVEL2 = 128*256 = 2^15 different cells, which
is in total (2^15)^2 cells.

It can be changed easily to different sizes, but it should be more senseful to have a small LEVEL1,
as in most cases only one/two entries in LEVEL1 will be used.

There are 2 additional special classes to store pointers for column and row formats.

Future enhancements:
To reduce memory consumption, it should be possible to enhance the functionality by
another LEVEL0, which then keeps the LEVEL1 size smaller.

Maybe the LEVEL1 should only be generated when there is a need for more than 1 LEVEL2.

LEVEL1 maybe reallocated.

Another interesting possibility would be to differentiate between x size and y size. Currently both
are equal in both matrizes, but normally it will be the regular case, that you have more need for
a lot of rows than columns. Maybe something like LEVEL1=128/256 and LEVEL2=256/128 (x/y), still keeping
2^15 values/cells in each direction (benefit: you won't loose memory in empty columns).
*/
class Cluster
{
    friend class ClusterTest;

public:
    Cluster();
    ~Cluster();

    Cell* lookup(int x, int y) const;

    /**
     * Removes all cells from the sheet and frees memory that
     * was used for the clusters.
     */
    void clear();

    /**
     * Inserts a cell at the requested position. If there is already
     * a cell, then @ref #remove is called on it.
     */
    void insert(Cell* cell, int x, int y);
    /**
     * Removes the cell at the given position, if there is any.
     */
    void remove(int x, int y);

    void setAutoDelete(bool);
    bool autoDelete() const;

    Cell* firstCell() const;

    bool insertShiftRight(const QPoint& marker);
    /**
     * Moves all cells in the column marker.x() beginning with
     * the one at marker.y() one position downwards.
     *
     * @return false if a cell would drop out of the sheet because of that.
     *         In this case the shift is not performed.
     */
    bool insertShiftDown(const QPoint& marker);

    /**
     * Moves all cells in the column marker.x() beginning with
     * the one at marker.y() + 1 one position upwards.
     */
    void removeShiftUp(const QPoint& marker);
    void removeShiftLeft(const QPoint& marker);

    /**
     * Moves all columns beginning with @p col one position
     * to the right. If that does not work because a cell would
     * drop out of the sheet, then false is returned.
     *
     * @see #removeColumn
     */
    bool insertColumn(int col);
    bool insertRow(int row);

    /**
     * Removes all elements from the column and
     * move all columns right of @p col one position
     * to the left.
     *
     * @see #clearColumn
     */
    void removeColumn(int col);
    void removeRow(int row);

    /**
     * Removes all elements from the column.
     *
     */
    void clearColumn(int col);
    void clearRow(int row);

    /** Retrieve a range of values stored in a Value.
    The range is two-leveled with similar structure and reasoning as the
    storage of cells themselves.
    */
    Value valueRange(int col1, int row1, int col2, int row2) const;

    /**
     * Retrieve the first used cell in a given column.  Can be used in conjunction
     * with getNextCellDown to loop through a column.
     *
     * @param col The column to get the first cell from
     *
     * @return Returns a pointer to the cell, or 0 if there are no used cells
     *         in this column
     */
    Cell* getFirstCellColumn(int col) const;

    /**
     * Retrieve the last used cell in a given column.  Can be used in conjunction
     * with getNextCellUp to loop through a column.
     *
     * @param col The column to get the cell from
     *
     * @return Returns a pointer to the cell, or 0 if there are no used cells
     *         in this column
     */
    Cell* getLastCellColumn(int col) const;

    /**
     * Retrieve the first used cell in a given row.  Can be used in conjunction
     * with getNextCellRight to loop through a row.
     *
     * @param row The row to get the first cell from
     *
     * @return Returns a pointer to the cell, or 0 if there are no used cells
     *         in this row
     */
    Cell* getFirstCellRow(int row) const;

    /**
     * Retrieve the last used cell in a given row.  Can be used in conjunction
     * with getNextCellLeft to loop through a row.
     *
     * @param row The row to get the last cell from
     *
     * @return Returns a pointer to the cell, or 0 if there are no used cells
     *         in this row
     */
    Cell* getLastCellRow(int row) const;

    /**
     * Retrieves the next used cell above the given col/row pair.  The given
     * col/row pair does not need to reference a used cell.
     *
     * @param col column to start looking through
     * @param row the row above which to start looking.
     *
     * @return Returns the next used cell above this one, or 0 if there are none
     */
    Cell* getNextCellUp(int col, int row) const;

    /**
     * Retrieves the next used cell below the given col/row pair.  The given
     * col/row pair does not need to reference a used cell.
     *
     * @param col column to start looking through
     * @param row the row below which to start looking.
     *
     * @return Returns the next used cell below this one, or 0 if there are none
     */
    Cell* getNextCellDown(int col, int row) const;

    /**
     * Retrieves the next used cell to the right of the given col/row pair.
     * The given col/row pair does not need to reference a used cell.
     *
     * @param col the column after which should be searched
     * @param row the row to search through
     *
     * @return Returns the next used cell to the right of this one, or 0 if
     * there are none
     */
    Cell* getNextCellRight(int col, int row) const;

    /**
     * Retrieves the next used cell to the left of the given col/row pair.
     * The given col/row pair does not need to reference a used cell.
     *
     * @param col the column before which should be searched
     * @param row the row to search through
     *
     * @return Returns the next used cell to the left of this one, or 0 if
     * there are none
     */
    Cell* getNextCellLeft(int col, int row) const;

private:
    /**
     * @param work is set to true if the method found some clusters
     *        which belong to the shifted row.
     */
    bool insertShiftRight(const QPoint& marker, bool& work);
    bool insertShiftDown(const QPoint& marker, bool& work);

    void removeShiftUp(const QPoint& marker, bool& work);
    void removeShiftLeft(const QPoint& marker, bool& work);

    /** helper method used by valueRange */
    Value makeArray(int col1, int row1, int col2, int row2) const;

    Cell*** m_cluster;
    Cell* m_first;
    bool m_autoDelete;
    int m_biggestX, m_biggestY;
};

/**
 * \ingroup Storage
 * A pointer map to all column formats.
 * \see Cluster
 */
class ColumnCluster
{
public:
    ColumnCluster();
    ~ColumnCluster();

    const ColumnFormat* lookup(int col) const;
    ColumnFormat* lookup(int col);

    void clear();

    void insertElement(ColumnFormat*, int col);
    void removeElement(int col);

    bool insertColumn(int col);
    bool removeColumn(int col);

    void setAutoDelete(bool);
    bool autoDelete() const;

    ColumnFormat* first() const {
        return m_first;
    }
    ColumnFormat* next(int col) const;

    void operator=(const ColumnCluster& other);

private:
    ColumnCluster(const ColumnCluster& other);

private:
    ColumnFormat*** m_cluster;
    ColumnFormat* m_first;
    bool m_autoDelete;
};

/**
 * \ingroup Storage
 * A pointer map to all row formats.
 * \see Cluster
 */
class RowCluster
{
public:
    RowCluster();
    ~RowCluster();

    const RowFormat* lookup(int col) const;
    RowFormat* lookup(int col);

    void clear();

    void insertElement(RowFormat*, int row);
    void removeElement(int row);

    bool insertRow(int row);
    bool removeRow(int row);

    void setAutoDelete(bool);
    bool autoDelete() const;

    RowFormat* first()const {
        return m_first;
    }

    void operator=(const RowCluster& other);

private:
    RowCluster(const RowCluster& other);

private:
    RowFormat*** m_cluster;
    RowFormat* m_first;
    bool m_autoDelete;
};

} // namespace KSpread;

#endif
