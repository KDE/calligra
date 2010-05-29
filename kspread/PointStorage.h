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

#ifndef KSPREAD_POINT_STORAGE
#define KSPREAD_POINT_STORAGE

#include <QRect>
#include <QString>
#include <QVector>

#include "Region.h"

// #define KSPREAD_POINT_STORAGE_HASH

namespace KSpread
{

/**
 * A custom pointwise storage.
 * Based on a compressed sparse matrix data structure.
 * Usable for any kind of data attached to 2D coordinates.
 *
 * Only non-default data with its coordinate is stored. Hence, the storage
 * has a small memory footprint nearly regardless of the data's location.
 * Each empty row before a location occupy an integer, which is not the case
 * for columns. Iterating over the data becomes fast compared to dense
 * matrix/array, where each location has to be traversed irrespective of
 * default or non-default data.
 *
 * The actual data is stored in the list m_data. It is grouped by rows in
 * ascending order. The rows' beginnings and ends are stored in the list
 * m_rows. Its index corresponds to the row index. The values denote the
 * starting index of a row in m_data. The row's end is determined by
 * the starting position of the next row. The entries in each row are ordered
 * by column. The corresponding column indices are stored in m_cols. Hence,
 * m_cols has the same amount of entries as m_data.
 *
 * \author Stefan Nikolaus <stefan.nikolaus@kdemail.net>
 *
 * \note If you fill the storage, do it row-wise. That's more performant.
 * \note For data assigned to rectangular regions use RectStorage.
 * \note It's QVector based. To boost performance a lot, declare the stored
 *       data type as movable.
 */
template<typename T>
class PointStorage
{
    friend class PointStorageBenchmark;
    friend class PointStorageTest;

public:
    /**
     * Constructor.
     * Creates an empty storage. Actually, does nothing.
     */
    PointStorage() {}

    /**
     * Destructor.
     */
    ~PointStorage() {}

    /**
     * Clears the storage.
     */
    void clear() {
        m_cols.clear();
        m_rows.clear();
        m_data.clear();
    }

    /**
     * Returns the number of items in the storage.
     * Usable to iterate over all non-default data.
     * \return number of items
     * \see col()
     * \see row()
     * \see data()
     */
    int count() const {
        return m_data.count();
    }

    /**
     * Inserts \p data at \p col , \p row .
     * \return the overridden data (default data, if no overwrite)
     */
    T insert(int col, int row, const T& data) {
        Q_ASSERT(1 <= col && col <= KS_colMax);
        Q_ASSERT(1 <= row && row <= KS_rowMax);
        // row's missing?
        if (row > m_rows.count()) {
            // insert missing rows
            m_rows.insert(m_rows.count(), row - m_rows.count(), m_data.count());
            // append the actual data
#ifdef KSPREAD_POINT_STORAGE_HASH
            m_data.append(*m_usedData.insert(data));
#else
            m_data.append(data);
#endif
            // append the column index
            m_cols.append(col);
        }
        // the row exists
        else {
            const QVector<int>::const_iterator cstart(m_cols.begin() + m_rows.value(row - 1));
            const QVector<int>::const_iterator cend((row < m_rows.count()) ? (m_cols.begin() + m_rows.value(row)) : m_cols.end());
            const QVector<int>::const_iterator cit = qLowerBound(cstart, cend, col);
            // column's missing?
            if (cit == cend || *cit != col) {
                // determine the index where the data and column has to be inserted
                const int index = m_rows.value(row - 1) + (cit - cstart);
                // insert the actual data
#ifdef KSPREAD_POINT_STORAGE_HASH
                m_data.insert(index, *m_usedData.insert(data));
#else
                m_data.insert(index, data);
#endif
                // insert the column index
                m_cols.insert(index, col);
                // adjust the offsets of the following rows
                for (int r = row; r < m_rows.count(); ++r)
                    ++m_rows[r];
            }
            // column exists
            else {
                const int index = m_rows.value(row - 1) + (cit - cstart);
                const T oldData = m_data[ index ];
#ifdef KSPREAD_POINT_STORAGE_HASH
                m_data[ index ] = *m_usedData.insert(data);
#else
                m_data[ index ] = data;
#endif
                return oldData;
            }
        }
        squeezeRows();
        return T();
    }

    /**
     * Looks up the data at \p col , \p row . If no data was found returns a
     * default object.
     * \return the data at the given coordinate
     */
    T lookup(int col, int row) const {
        Q_ASSERT(1 <= col && col <= KS_colMax);
        Q_ASSERT(1 <= row && row <= KS_rowMax);
        // is the row not present?
        if (row > m_rows.count())
            return T();
        const QVector<int>::const_iterator cstart(m_cols.begin() + m_rows.value(row - 1));
        const QVector<int>::const_iterator cend((row < m_rows.count()) ? (m_cols.begin() + m_rows.value(row)) : m_cols.end());
        const QVector<int>::const_iterator cit = qBinaryFind(cstart, cend, col);
        // is the col not present?
        if (cit == cend)
            return T();
        return m_data.value(m_rows.value(row - 1) + (cit - cstart));
    }

    /**
     * Removes data at \p col , \p row .
     * \return the removed data (default data, if none)
     */
    T take(int col, int row) {
        Q_ASSERT(1 <= col && col <= KS_colMax);
        Q_ASSERT(1 <= row && row <= KS_rowMax);
        // row's missing?
        if (row > m_rows.count())
            return T();
        const int rowStart = (row - 1 < m_rows.count()) ? m_rows.value(row - 1) : m_data.count();
        const int rowLength = (row < m_rows.count()) ? m_rows.value(row) - rowStart : -1;
        const QVector<int> cols = m_cols.mid(rowStart, rowLength);
        QVector<int>::const_iterator cit = qBinaryFind(cols, col);
        // column's missing?
        if (cit == cols.constEnd())
            return T();
        const int index = rowStart + (cit - cols.constBegin());
        // save the old data
        const T oldData = m_data[ index ];
        // remove the actual data
        m_data.remove(index);
        // remove the column index
        m_cols.remove(index);
        // adjust the offsets of the following rows
        for (int r = row; r < m_rows.count(); ++r)
            --m_rows[r];
        squeezeRows();
        return oldData;
    }

    /**
     * Insert \p number columns at \p position .
     * \return the data, that became out of range (shifted over the end)
     */
    QVector< QPair<QPoint, T> > insertColumns(int position, int number) {
        Q_ASSERT(1 <= position && position <= KS_colMax);
        QVector< QPair<QPoint, T> > oldData;
        for (int row = m_rows.count(); row >= 1; --row) {
            const int rowStart = m_rows.value(row - 1);
            const int rowLength = (row < m_rows.count()) ? m_rows.value(row) - rowStart : -1;
            const QVector<int> cols = m_cols.mid(rowStart, rowLength);
            for (int col = cols.count(); col >= 0; --col) {
                if (cols.value(col) + number > KS_colMax) {
                    oldData.append(qMakePair(QPoint(cols.value(col), row), m_data.value(rowStart + col)));
                    m_cols.remove(rowStart + col);
                    m_data.remove(rowStart + col);
                    // adjust the offsets of the following rows
                    for (int r = row; r < m_rows.count(); ++r)
                        --m_rows[r];
                } else if (cols.value(col) >= position)
                    m_cols[rowStart + col] += number;
            }
        }
        squeezeRows();
        return oldData;
    }

    /**
     * Removes \p number columns at \p position .
     * \return the removed data
     */
    QVector< QPair<QPoint, T> > removeColumns(int position, int number) {
        Q_ASSERT(1 <= position && position <= KS_colMax);
        QVector< QPair<QPoint, T> > oldData;
        for (int row = m_rows.count(); row >= 1; --row) {
            const int rowStart = m_rows.value(row - 1);
            const int rowLength = (row < m_rows.count()) ? m_rows.value(row) - rowStart : -1;
            const QVector<int> cols = m_cols.mid(rowStart, rowLength);
            for (int col = cols.count() - 1; col >= 0; --col) {
                if (cols.value(col) >= position) {
                    if (cols.value(col) < position + number) {
                        oldData.append(qMakePair(QPoint(cols.value(col), row), m_data.value(rowStart + col)));
                        m_cols.remove(rowStart + col);
                        m_data.remove(rowStart + col);
                        for (int r = row; r < m_rows.count(); ++r)
                            --m_rows[r];
                    } else
                        m_cols[rowStart + col] -= number;
                }
            }
        }
        squeezeRows();
        return oldData;
    }

    /**
     * Insert \p number rows at \p position .
     * \return the data, that became out of range (shifted over the end)
     */
    QVector< QPair<QPoint, T> > insertRows(int position, int number) {
        Q_ASSERT(1 <= position && position <= KS_rowMax);
        // row's missing?
        if (position > m_rows.count())
            return QVector< QPair<QPoint, T> >();
        QVector< QPair<QPoint, T> > oldData;
        int dataCount = 0;
        int rowCount = 0;
        // save the old data
        for (int row = KS_rowMax - number + 1; row <= m_rows.count() && row <= KS_rowMax; ++row) {
            const QVector<int>::const_iterator cstart(m_cols.begin() + m_rows.value(row - 1));
            const QVector<int>::const_iterator cend((row < m_rows.count()) ? (m_cols.begin() + m_rows.value(row)) : m_cols.end());
            for (QVector<int>::const_iterator cit = cstart; cit != cend; ++cit)
                oldData.append(qMakePair(QPoint(*cit, row), m_data.value(cit - m_cols.constBegin())));
            dataCount += (cend - cstart);
            ++rowCount;
        }
        // remove the out of range data
        while (dataCount-- > 0) {
            m_data.remove(m_data.count() - 1);
            m_cols.remove(m_cols.count() - 1);
        }
        while (rowCount-- > 0)
            m_rows.remove(m_rows.count() - 1);
        // insert the new rows
        const int index = m_rows.value(position - 1);
        for (int r = 0; r < number; ++r)
            m_rows.insert(position, index);
        squeezeRows();
        return oldData;
    }

    /**
     * Removes \p number rows at \p position .
     * \return the removed data
     */
    QVector< QPair<QPoint, T> > removeRows(int position, int number) {
        Q_ASSERT(1 <= position && position <= KS_rowMax);
        // row's missing?
        if (position > m_rows.count())
            return QVector< QPair<QPoint, T> >();
        QVector< QPair<QPoint, T> > oldData;
        int dataCount = 0;
        int rowCount = 0;
        // save the old data
        for (int row = position; row <= m_rows.count() && row <= position + number - 1; ++row) {
            const int rowStart = m_rows.value(row - 1);
            const int rowLength = (row < m_rows.count()) ? m_rows.value(row) - rowStart : -1;
            const QVector<int> cols = m_cols.mid(rowStart, rowLength);
            const QVector<T> data = m_data.mid(rowStart, rowLength);
            for (int col = 0; col < cols.count(); ++col)
                oldData.append(qMakePair(QPoint(cols.value(col), row), data.value(col)));
            dataCount += data.count();
            ++rowCount;
        }
        // adjust the offsets of the following rows
        for (int r = position + number - 1; r < m_rows.count(); ++r)
            m_rows[r] -= dataCount;
        // remove the out of range data
        while (dataCount-- > 0) {
            m_data.remove(m_rows.value(position - 1));
            m_cols.remove(m_rows.value(position - 1));
        }
        while (rowCount-- > 0)
            m_rows.remove(position - 1);
        squeezeRows();
        return oldData;
    }

    /**
     * Shifts the data right of \p rect to the left by the width of \p rect .
     * The data formerly contained in \p rect becomes overridden.
     * \return the removed data
     */
    QVector< QPair<QPoint, T> > removeShiftLeft(const QRect& rect) {
        Q_ASSERT(1 <= rect.left() && rect.left() <= KS_colMax);
        QVector< QPair<QPoint, T> > oldData;
        for (int row = qMin(rect.bottom(), m_rows.count()); row >= rect.top(); --row) {
            const int rowStart = m_rows.value(row - 1);
            const int rowLength = (row < m_rows.count()) ? m_rows.value(row) - rowStart : -1;
            const QVector<int> cols = m_cols.mid(rowStart, rowLength);
            for (int col = cols.count() - 1; col >= 0; --col) {
                if (cols.value(col) >= rect.left()) {
                    if (cols.value(col) <= rect.right()) {
                        oldData.append(qMakePair(QPoint(cols.value(col), row), m_data.value(rowStart + col)));
                        m_cols.remove(rowStart + col);
                        m_data.remove(rowStart + col);
                        for (int r = row; r < m_rows.count(); ++r)
                            --m_rows[r];
                    } else
                        m_cols[rowStart + col] -= rect.width();
                }
            }
        }
        squeezeRows();
        return oldData;
    }

    /**
     * Shifts the data in and right of \p rect to the right by the width of \p rect .
     * \return the data, that became out of range (shifted over the end)
     */
    QVector< QPair<QPoint, T> > insertShiftRight(const QRect& rect) {
        Q_ASSERT(1 <= rect.left() && rect.left() <= KS_colMax);
        QVector< QPair<QPoint, T> > oldData;
        for (int row = rect.top(); row <= rect.bottom() && row <= m_rows.count(); ++row) {
            const int rowStart = m_rows.value(row - 1);
            const int rowLength = (row < m_rows.count()) ? m_rows.value(row) - rowStart : -1;
            const QVector<int> cols = m_cols.mid(rowStart, rowLength);
            for (int col = cols.count(); col >= 0; --col) {
                if (cols.value(col) + rect.width() > KS_colMax) {
                    oldData.append(qMakePair(QPoint(cols.value(col), row), m_data.value(rowStart + col)));
                    m_cols.remove(rowStart + col);
                    m_data.remove(rowStart + col);
                    // adjust the offsets of the following rows
                    for (int r = row; r < m_rows.count(); ++r)
                        --m_rows[r];
                } else if (cols.value(col) >= rect.left())
                    m_cols[rowStart + col] += rect.width();
            }
        }
        squeezeRows();
        return oldData;
    }

    /**
     * Shifts the data below \p rect to the top by the height of \p rect .
     * The data formerly contained in \p rect becomes overridden.
     * \return the removed data
     */
    QVector< QPair<QPoint, T> > removeShiftUp(const QRect& rect) {
        Q_ASSERT(1 <= rect.top() && rect.top() <= KS_rowMax);
        // row's missing?
        if (rect.top() > m_rows.count())
            return QVector< QPair<QPoint, T> >();
        QVector< QPair<QPoint, T> > oldData;
        for (int row = rect.top(); row <= m_rows.count() && row <= KS_rowMax - rect.height(); ++row) {
            const int rowStart = m_rows.value(row - 1);
            const int rowLength = (row < m_rows.count()) ? m_rows.value(row) - rowStart : -1;
            const QVector<int> cols = m_cols.mid(rowStart, rowLength);
            const QVector<T> data = m_data.mid(rowStart, rowLength);
            // first, iterate over the destination row
            for (int col = cols.count() - 1; col >= 0; --col) {
                const int column = cols.value(col); // real column value (1...KS_colMax)
                if (column >= rect.left() && column <= rect.right()) {
                    // save the old data
                    if (row <= rect.bottom())
                        oldData.append(qMakePair(QPoint(column, row), data.value(col)));
                    // search
                    const int srcRow = row + rect.height();
                    const QVector<int>::const_iterator cstart2((srcRow - 1 < m_rows.count()) ? m_cols.begin() + m_rows.value(srcRow - 1) : m_cols.end());
                    const QVector<int>::const_iterator cend2((srcRow < m_rows.count()) ? (m_cols.begin() + m_rows.value(srcRow)) : m_cols.end());
                    const QVector<int>::const_iterator cit2 = qBinaryFind(cstart2, cend2, column);
                    // column's missing?
                    if (cit2 == cend2) {
                        m_cols.remove(rowStart + col);
                        m_data.remove(rowStart + col);
                        // adjust the offsets of the following rows
                        for (int r = row; r < m_rows.count(); ++r)
                            --m_rows[r];
                    }
                    // column exists
                    else {
                        // copy
                        m_data[rowStart + col] = m_data.value(cit2 - m_cols.constBegin());
                        // remove
                        m_cols.remove(cit2 - m_cols.constBegin());
                        m_data.remove(cit2 - m_cols.constBegin());
                        // adjust the offsets of the following rows
                        for (int r = row + rect.height(); r < m_rows.count(); ++r)
                            --m_rows[r];
                    }
                }
            }
            // last, iterate over the source row
            const int srcRow = row + rect.height();
            const int rowStart2 = (srcRow - 1 < m_rows.count()) ? m_rows.value(srcRow - 1) : m_data.count();
            const int rowLength2 = (srcRow < m_rows.count()) ? m_rows.value(srcRow) - rowStart2 : -1;
            const QVector<int> cols2 = m_cols.mid(rowStart2, rowLength2);
            const QVector<T> data2 = m_data.mid(rowStart2, rowLength2);
            int offset = 0;
            for (int col = cols2.count() - 1; col >= 0; --col) {
                const int column = cols2.value(col); // real column value (1...KS_colMax)
                if (column >= rect.left() && column <= rect.right()) {
                    // find the insertion position
                    const QVector<int>::const_iterator cstart((row - 1 < m_rows.count()) ? m_cols.begin() + m_rows.value(row - 1) : m_cols.end());
                    const QVector<int>::const_iterator cend(((row < m_rows.count())) ? (m_cols.begin() + m_rows.value(row)) : m_cols.end());
                    const QVector<int>::const_iterator cit = qUpperBound(cstart, cend, cols2.value(col));
                    // Destination column:
                    const QVector<int>::const_iterator dstcit = qBinaryFind(cols.begin(), cols.end(), column);
                    if (dstcit != cols.end()) { // destination column exists
                        // replace the existing destination value
                        const int dstCol = (dstcit - cols.constBegin());
                        m_data[rowStart + dstCol] = m_data.value(rowStart2 + col);
                        // remove it from its old position
                        m_data.remove(rowStart2 + col + 1);
                        m_cols.remove(rowStart2 + col + 1);
                        // The amount of values in the range from the
                        // destination row to the source row have not changed.
                        // adjust the offsets of the following rows
                        for (int r = srcRow; r < m_rows.count(); ++r) {
                            ++m_rows[r];
                        }
                    } else { // destination column does not exist yet
                        // copy it to its new position
                        const int dstCol = cit - m_cols.constBegin();
                        m_data.insert(dstCol, data2.value(col));
                        m_cols.insert(dstCol, cols2.value(col));
                        // remove it from its old position
                        m_data.remove(rowStart2 + col + 1 + offset);
                        m_cols.remove(rowStart2 + col + 1 + offset);
                        ++offset;
                        // adjust the offsets of the following rows
                        for (int r = row; r < srcRow; ++r) {
                            ++m_rows[r];
                        }
                    }
                }
            }
        }
        squeezeRows();
        return oldData;
    }

    /**
     * Shifts the data in and below \p rect to the bottom by the height of \p rect .
     * \return the data, that became out of range (shifted over the end)
     */
    QVector< QPair<QPoint, T> > insertShiftDown(const QRect& rect) {
        Q_ASSERT(1 <= rect.top() && rect.top() <= KS_rowMax);
        // row's missing?
        if (rect.top() > m_rows.count())
            return QVector< QPair<QPoint, T> >();
        QVector< QPair<QPoint, T> > oldData;
        for (int row = m_rows.count(); row >= rect.top(); --row) {
            const int rowStart = m_rows.value(row - 1);
            const int rowLength = (row < m_rows.count()) ? m_rows.value(row) - rowStart : -1;
            const QVector<int> cols = m_cols.mid(rowStart, rowLength);
            const QVector<T> data = m_data.mid(rowStart, rowLength);
            for (int col = cols.count() - 1; col >= 0; --col) {
                if (cols.value(col) >= rect.left() && cols.value(col) <= rect.right()) {
                    if (row + rect.height() > KS_rowMax) {
                        // save old data
                        oldData.append(qMakePair(QPoint(cols.value(col), row), data.value(col)));
                    } else {
                        // insert missing rows
                        if (row + rect.height() > m_rows.count())
                            m_rows.insert(m_rows.count(), row + rect.height() - m_rows.count(), m_data.count());

                        // copy the data down
                        const int row2 = row + rect.height();
                        const QVector<int>::const_iterator cstart2(m_cols.begin() + m_rows.value(row2 - 1));
                        const QVector<int>::const_iterator cend2((row2 < m_rows.count()) ? (m_cols.begin() + m_rows.value(row2)) : m_cols.end());
                        const QVector<int>::const_iterator cit2 = qLowerBound(cstart2, cend2, cols.value(col));
                        // column's missing?
                        if (cit2 == cend2 || *cit2 != cols.value(col)) {
                            // determine the index where the data and column has to be inserted
                            const int index = m_rows.value(row2 - 1) + (cit2 - cstart2);
                            // insert the actual data
                            m_data.insert(index, data.value(col));
                            // insert the column index
                            m_cols.insert(index, cols.value(col));
                            // adjust the offsets of the following rows
                            for (int r = row2; r < m_rows.count(); ++r)
                                ++m_rows[r];
                        }
                        // column exists
                        else {
                            const int index = m_rows.value(row2 - 1) + (cit2 - cstart2);
                            m_data[ index ] = data.value(col);
                        }
                    }

                    // remove the data
                    m_cols.remove(rowStart + col);
                    m_data.remove(rowStart + col);
                    // adjust the offsets of the following rows
                    for (int r = row; r < m_rows.count(); ++r)
                        --m_rows[r];
                }
            }
        }
        squeezeRows();
        return oldData;
    }

    /**
     * Retrieve the first used data in \p col .
     * Can be used in conjunction with nextInColumn() to loop through a column.
     * \return the first used data in \p col or the default data, if the column is empty.
     */
    T firstInColumn(int col, int* newRow = 0) const {
        Q_ASSERT(1 <= col && col <= KS_colMax);
        const int index = m_cols.indexOf(col);
        if (newRow) {
            if (index == -1)   // not found
                *newRow = 0;
            else
                *newRow = qUpperBound(m_rows, index) - m_rows.begin();
        }
        return m_data.value(index);
    }

    /**
     * Retrieve the first used data in \p row .
     * Can be used in conjunction with nextInRow() to loop through a row.
     * \return the first used data in \p row or the default data, if the row is empty.
     */
    T firstInRow(int row, int* newCol = 0) const {
        Q_ASSERT(1 <= row && row <= KS_rowMax);
        // row's empty?
        if ((row < m_rows.count()) && m_rows.value(row - 1) == m_rows.value(row)) {
            if (newCol)
                *newCol = 0;
            return T();
        }
        if (newCol)
            *newCol = m_cols.value(m_rows.value(row - 1));
        return m_data.value(m_rows.value(row - 1));
    }

    /**
     * Retrieve the last used data in \p col .
     * Can be used in conjunction with prevInColumn() to loop through a column.
     * \return the last used data in \p col or the default data, if the column is empty.
     */
    T lastInColumn(int col, int* newRow = 0) const {
        Q_ASSERT(1 <= col && col <= KS_colMax);
        const int index = m_cols.lastIndexOf(col);
        if (newRow) {
            if (index == -1)   // not found
                *newRow = 0;
            else
                *newRow = qUpperBound(m_rows, index) - m_rows.begin();
        }
        return m_data.value(index);
    }

    /**
     * Retrieve the last used data in \p row .
     * Can be used in conjunction with prevInRow() to loop through a row.
     * \return the last used data in \p row or the default data, if the row is empty.
     */
    T lastInRow(int row, int* newCol = 0) const {
        Q_ASSERT(1 <= row && row <= KS_rowMax);
        // row's empty?
        if (m_rows.value(row - 1) == m_rows.value(row) || m_rows.value(row - 1) == m_data.count()) {
            if (newCol)
                *newCol = 0;
            return T();
        }
        // last row ends on data vector end
        if (row == m_rows.count()) {
            if (newCol)
                *newCol = m_cols.value(m_data.count() - 1);
            return m_data.value(m_data.count() - 1);
        }
        if (newCol)
            *newCol = m_cols.value(m_rows.value(row) - 1);
        return m_data.value(m_rows.value(row) - 1);
    }

    /**
     * Retrieve the next used data in \p col after \p row .
     * Can be used in conjunction with firstInColumn() to loop through a column.
     * \return the next used data in \p col or the default data, there is no further data.
     */
    T nextInColumn(int col, int row, int* newRow = 0) const {
        Q_ASSERT(1 <= col && col <= KS_colMax);
        Q_ASSERT(1 <= row && row <= KS_rowMax);
        // no next row?
        if (row + 1 > m_rows.count()) {
            if (newRow)
                *newRow = 0;
            return T();
        }
        // search beginning in rows after the specified row
        const int index = m_cols.indexOf(col, m_rows.value(row));
        if (newRow) {
            if (index == -1)   // not found
                *newRow = 0;
            else
                *newRow = qUpperBound(m_rows, index) - m_rows.begin();
        }
        return m_data.value(index);
    }

    /**
     * Retrieve the next used data in \p row after \p col .
     * Can be used in conjunction with firstInRow() to loop through a row.
     * \return the next used data in \p row or the default data, if there is no further data.
     */
    T nextInRow(int col, int row, int* newCol = 0) const {
        Q_ASSERT(1 <= col && col <= KS_colMax);
        Q_ASSERT(1 <= row && row <= KS_rowMax);
        // is the row not present?
        if (row > m_rows.count()) {
            if (newCol)
                *newCol = 0;
            return T();
        }
        const QVector<int>::const_iterator cstart(m_cols.begin() + m_rows.value(row - 1));
        const QVector<int>::const_iterator cend((row < m_rows.count()) ? (m_cols.begin() + m_rows.value(row)) : m_cols.end());
        const QVector<int>::const_iterator cit = qUpperBound(cstart, cend, col);
        if (cit == cend || *cit <= col) {
            if (newCol)
                *newCol = 0;
            return T();
        }
        if (newCol)
            *newCol = m_cols.value(m_rows.value(row - 1) + (cit - cstart));
        return m_data.value(m_rows.value(row - 1) + (cit - cstart));
    }

    /**
     * Retrieve the previous used data in \p col after \p row .
     * Can be used in conjunction with lastInColumn() to loop through a column.
     * \return the previous used data in \p col or the default data, there is no further data.
     */
    T prevInColumn(int col, int row, int* newRow = 0) const {
        Q_ASSERT(1 <= col && col <= KS_colMax);
        Q_ASSERT(1 <= row && row <= KS_rowMax);
        // first row?
        if (row <= m_rows.count() && m_rows.value(row - 1) == 0) {
            if (newRow)
                *newRow = 0;
            return T();
        }
        const int index = m_cols.lastIndexOf(col, m_rows.value(row - 1) - 1);
        if (newRow) {
            if (index == -1)   // not found
                *newRow = 0;
            else
                *newRow = qUpperBound(m_rows, index) - m_rows.begin();
        }
        return m_data.value(index);
    }

    /**
     * Retrieve the previous used data in \p row after \p col .
     * Can be used in conjunction with lastInRow() to loop through a row.
     * \return the previous used data in \p row or the default data, if there is no further data.
     */
    T prevInRow(int col, int row, int* newCol = 0) const {
        Q_ASSERT(1 <= col && col <= KS_colMax);
        Q_ASSERT(1 <= row && row <= KS_rowMax);
        const QVector<int>::const_iterator cstart((row - 1 < m_rows.count()) ? m_cols.begin() + m_rows.value(row - 1) : m_cols.end());
        const QVector<int>::const_iterator cend((row < m_rows.count()) ? (m_cols.begin() + m_rows.value(row)) : m_cols.end());
        const QVector<int>::const_iterator cit = qLowerBound(cstart, cend, col);
        if (cit == cstart) {
            if (newCol)
                *newCol = 0;
            return T();
        }
        if (newCol)
            *newCol = m_cols.value(cit - 1 - m_cols.begin());
        return m_data.value(cit - 1 - m_cols.begin());
    }

    /**
     * For debugging/testing purposes.
     * \note only works with primitive/printable data
     */
    QString dump() const {
        QString str;
        // determine the dimension of the matrix (the missing column number)
        int maxCols = 0;
        for (int row = 0; row < m_rows.count(); ++row) {
            const int rowStart = m_rows.value(row);
            const int rowLength = (row + 1 < m_rows.count()) ? m_rows.value(row + 1) - rowStart : -1;
            const QVector<int> cols = m_cols.mid(rowStart, rowLength);
            maxCols = qMax(maxCols, cols.value(cols.count() - 1));
        }
        for (int row = 0; row < m_rows.count(); ++row) {
            str += '(';
            const int rowStart = m_rows.value(row);
            const int rowLength = (row + 1 < m_rows.count()) ? m_rows.value(row + 1) - rowStart : -1;
            const QVector<int> cols = m_cols.mid(rowStart, rowLength);
            const QVector<T> data = m_data.mid(rowStart, rowLength);
            int lastCol = 0;
            for (int col = 0; col < cols.count(); ++col) {
                int counter = cols.value(col) - lastCol;
                while (counter-- > 1)
                    str += "  ,";
                str += QString("%1,").arg(data.value(col), 2);
//                 str += QString( "%1," ).arg( (data.value( col ) == T()) ? "" : "_", 2 );
                lastCol = cols.value(col);
            }
            // fill the column up to the max
            int counter = maxCols - lastCol;
            while (counter-- > 0)
                str += "  ,";
            // replace the last comma
            str[str.length()-1] = ')';
            str += '\n';
        }
        return str.isEmpty() ? QString("()") : str.mid(0, str.length() - 1);
    }

    /**
     * Returns the column of the non-default data at \p index .
     * \return the data's column at \p index .
     * \see count()
     * \see row()
     * \see data()
     */
    int col(int index) const {
        return m_cols.value(index);
    }

    /**
     * Returns the row of the non-default data at \p index .
     * \return the data's row at \p index .
     * \see count()
     * \see col()
     * \see data()
     */
    int row(int index) const {
        return qUpperBound(m_rows, index) - m_rows.begin();
    }

    /**
     * Returns the non-default data at \p index .
     * \return the data at \p index .
     * \see count()
     * \see col()
     * \see row()
     */
    T data(int index) const {
        return m_data.value(index);
    }

    /**
     * The maximum occupied column, i.e. the horizontal storage dimension.
     * \return the maximum column
     */
    int columns() const {
        int columns = 0;
        for (int c = 0; c < m_cols.count(); ++c)
            columns = qMax(m_cols.value(c), columns);
        return columns;
    }

    /**
     * The maximum occupied row, i.e. the vertical storage dimension.
     * \return the maximum row
     */
    int rows() const {
        return m_rows.count();
    }

    /**
     * Creates a substorage consisting of the values in \p region.
     * If \p keepOffset is \c true, the values' positions are not altered.
     * Otherwise, the upper left of \p region's bounding rect is used as new origin,
     * and all positions are adjusted.
     * \return a subset of the storage stripped down to the values in \p region
     */
    PointStorage<T> subStorage(const Region& region, bool keepOffset = true) const {
        // Determine the offset.
        const QPoint offset = keepOffset ? QPoint(0, 0) : region.boundingRect().topLeft() - QPoint(1, 1);
        // this generates an array of values
        PointStorage<T> subStorage;
        Region::ConstIterator end(region.constEnd());
        for (Region::ConstIterator it(region.constBegin()); it != end; ++it) {
            const QRect rect = (*it)->rect();
            for (int row = rect.top(); row <= rect.bottom() && row <= m_rows.count(); ++row) {
                const QVector<int>::const_iterator cstart(m_cols.begin() + m_rows.value(row - 1));
                const QVector<int>::const_iterator cend((row < m_rows.count()) ? (m_cols.begin() + m_rows.value(row)) : m_cols.end());
                for (QVector<int>::const_iterator cit = cstart; cit != cend; ++cit) {
                    if (*cit >= rect.left() && *cit <= rect.right()) {
                        if (keepOffset)
                            subStorage.insert(*cit, row, m_data.value(cit - m_cols.begin()));
                        else
                            subStorage.insert(*cit - offset.x(), row - offset.y(), m_data.value(cit - m_cols.begin()));
                    }
                }
            }
        }
        return subStorage;
    }

    /**
     * Equality operator.
     */
    bool operator==(const PointStorage<T>& o) const {
        return (m_rows == o.m_rows && m_cols == o.m_cols && m_data == o.m_data);
    }

private:
    void squeezeRows() {
        int row = m_rows.count() - 1;
        while (m_rows.value(row) == m_data.count() && row >= 0)
            m_rows.remove(row--);
    }

private:
    QVector<int> m_cols;    // stores the column indices (beginning with one)
    QVector<int> m_rows;    // stores the row offsets in m_data
    QVector<T>   m_data;    // stores the actual non-default data

#ifdef KSPREAD_POINT_STORAGE_HASH
    QSet<T> m_usedData;
#endif
};

} // namespace KSpread

#endif // KSPREAD_POINT_STORAGE
