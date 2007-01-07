/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#ifndef KSPREAD_POINT_STORAGE
#define KSPREAD_POINT_STORAGE

#include <QDebug>
#include <QRect>
#include <QString>
#include <QVector>

// #include "Global.h"
#warning Include Global.h for KS_colMax, KS_rowMax.
#define KS_rowMax 0x7FFF
#define KS_colMax 0x7FFF

namespace KSpread
{

/**
 * A custom pointwise storage.
 * Based on a compressed sparse matrix data structure.
 * Usable for any kind of data attached to 2D coordinates.
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
 * \since 2.0
 *
 * \note For data assigned to rectangular regions use Storage.
 */
template<typename T>
class PointStorage
{
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
    void clear()
    {
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
    int count() const
    {
        return m_data.count();
    }

    /**
     * Inserts \p data at \p col , \p row .
     * \return the overridden data (default data, if no overwrite)
     */
    T insert( int col, int row, const T& data )
    {
        Q_ASSERT( 1 <= col && col <= KS_colMax );
        Q_ASSERT( 1 <= row && row <= KS_rowMax );
        // row's missing?
        if ( row > m_rows.count() )
        {
            // insert missing rows
            m_rows.insert( m_rows.count(), row - m_rows.count(), m_data.count() );
            // append the actual data
            m_data.append( data );
            // append the column index
            m_cols.append( col );
        }
        // the row exists
        else
        {
            const QVector<int>::const_iterator cstart( m_cols.begin() + m_rows.value( row - 1 ) );
            const QVector<int>::const_iterator cend( ( row < m_rows.count() ) ? ( m_cols.begin() + m_rows.value( row ) ) : m_cols.end() );
            const QVector<int>::const_iterator cit = qBinaryFind( cstart, cend, col );
            // column's missing?
            if ( cit == cend )
            {
                // determine the index where the data and column has to be inserted
                const int index = m_rows.value( row - 1 ) + ( cend - cstart );
                // insert the actual data
                m_data.insert( index, data );
                // insert the column index
                m_cols.insert( index, col );
                // adjust the offsets of this row and the following rows
                for ( int r = row; r < m_rows.count(); ++r )
                    ++m_rows[r];
            }
            // column exists
            else
            {
                const int index = m_rows.value( row - 1 ) + ( cit - cstart );
                const T oldData = m_data[ index ];
                m_data[ index ] = data;
                return oldData;
            }
        }
        return T();
    }

    /**
     * Looks up the data at \p col , \p row . If no data was found returns a
     * default object.
     * \return the data at the given coordinate
     */
    T lookup( int col, int row ) const
    {
        Q_ASSERT( 1 <= col && col <= KS_colMax );
        Q_ASSERT( 1 <= row && row <= KS_rowMax );
        // is the row not present?
        if ( row - 1 > m_rows.count() )
            return T();
        const QVector<int>::const_iterator cstart( m_cols.begin() + m_rows.value( row - 1 ) );
        const QVector<int>::const_iterator cend( ( row < m_rows.count() ) ? ( m_cols.begin() + m_rows.value( row ) ) : m_cols.end() );
        const QVector<int>::const_iterator cit = qBinaryFind( cstart, cend, col );
        // is the col not present?
        if ( cit == cend )
            return T();
        return m_data.value( m_rows.value( row - 1 ) + ( cit - cstart ) );
    }

    /**
     * Removes data at \p col , \p row .
     * \return the removed data (default data, if none)
     */
    T take( int col, int row )
    {
        Q_ASSERT( 1 <= col && col <= KS_colMax );
        Q_ASSERT( 1 <= row && row <= KS_rowMax );
        // row's missing?
        if ( row - 1 > m_rows.count() )
            return T();
        const int rowStart = m_rows.value( row - 1 );
        const int rowLength = ( row < m_rows.count() ) ? m_rows.value( row ) - rowStart : -1;
        const QVector<int> cols = m_cols.mid( rowStart, rowLength );
        QVector<int>::const_iterator cit = qBinaryFind( cols, col );
        // column's missing?
        if ( cit == cols.constEnd() )
            return T();
        const int index = rowStart + ( cit - cols.constBegin() );
        // save the old data
        const T oldData = m_data[ index ];
        // remove the actual data
        m_data.remove( index );
        // remove the column index
        m_cols.remove( index );
        // adjust the offsets of this row and the following rows
        for ( int r = row; r < m_rows.count(); ++r )
            --m_rows[r];
        return oldData;
    }

    /**
     * Insert \p number columns at \p position .
     * \return the data, that became out of range (shifted over the end)
     */
    QVector< QPair<QPoint,T> > insertColumns( int position, int number = 1 )
    {
        Q_ASSERT( 1 <= position && position <= KS_colMax );
        QVector< QPair<QPoint,T> > oldData;
        for ( int row = 1; row <= m_rows.count(); ++row )
        {
            const int rowStart = m_rows.value( row - 1 );
            const int rowLength = ( row < m_rows.count() ) ? m_rows.value( row ) - rowStart : -1;
            const QVector<int> cols = m_cols.mid( rowStart, rowLength );
            for ( int col = 0; col < cols.count(); ++col )
            {
                if ( cols.value( col ) > KS_colMax - position )
                    oldData.append( qMakePair( QPoint( cols.value( col ), row ), m_data.value( rowStart + col ) ) );
                else if ( cols.value( col ) >= position )
                    m_cols[rowStart + col] += number;
            }
        }
        return oldData;
    }

    /**
     * Removes \p number columns at \p position .
     * \return the removed data
     */
    QVector< QPair<QPoint,T> > removeColumns( int position, int number = 1 )
    {
        Q_ASSERT( 1 <= position && position <= KS_colMax );
        QVector< QPair<QPoint,T> > oldData;
        for ( int row = m_rows.count(); row > 0; --row )
        {
            const int rowStart = m_rows.value( row - 1 );
            const int rowLength = ( row < m_rows.count() ) ? m_rows.value( row ) - rowStart : -1;
            const QVector<int> cols = m_cols.mid( rowStart, rowLength );
            for ( int col = cols.count() - 1; col >= 0; --col )
            {
                if ( cols.value( col ) >= position )
                {
                    if ( cols.value( col ) < position + number )
                    {
                        oldData.append( qMakePair( QPoint( cols.value( col ), row ), m_data.value( rowStart + col ) ) );
                        m_cols.remove( rowStart + col );
                        m_data.remove( rowStart + col );
                        for ( int r = row; r < m_rows.count(); ++r )
                            --m_rows[r];
                    }
                    else
                        m_cols[rowStart + col] -= number;
                }
            }
        }
        return oldData;
    }

    /**
     * Insert \p number rows at \p position .
     * \return the data, that became out of range (shifted over the end)
     */
    QVector< QPair<QPoint,T> > insertRows( int position, int number = 1 )
    {
        Q_ASSERT( 1 <= position && position <= KS_rowMax );
        // row's missing?
        if ( position - 1 > m_rows.count() )
            return QVector< QPair<QPoint,T> >();
        QVector< QPair<QPoint,T> > oldData;
        int dataCount = 0;
        int rowCount = 0;
        // save the old data
        for ( int row = KS_rowMax - number + 1; row - 1 <= m_rows.count() && row <= KS_rowMax; ++row )
        {
            const int rowStart = m_rows.value( row - 1 );
            const int rowLength = ( row < m_rows.count() ) ? m_rows.value( row ) - rowStart : -1;
            const QVector<int> cols = m_cols.mid( rowStart, rowLength );
            const QVector<int> data = m_data.mid( rowStart, rowLength );
            for ( int col = 0; col < cols.count(); ++col )
                oldData.append( qMakePair( QPoint( cols.value( col ), row ), data.value( col ) ) );
            dataCount += data.count();
            ++rowCount;
        }
        // remove the out of range data
        while ( dataCount-- > 0 )
        {
            m_data.remove( m_data.count() - 1 );
            m_cols.remove( m_cols.count() - 1 );
        }
        while ( rowCount-- > 0 )
            m_rows.remove( m_rows.count() - 1 );
        // insert the new rows
        const int index = m_rows.value( position - 1 );
        for ( int r = 0; r < number; ++r )
            m_rows.insert( position, index );
        return oldData;
    }

    /**
     * Removes \p number rows at \p position .
     * \return the removed data
     */
    QVector< QPair<QPoint,T> > removeRows( int position, int number = 1 )
    {
        Q_ASSERT( 1 <= position && position <= KS_rowMax );
        // row's missing?
        if ( position - 1 > m_rows.count() )
            return QVector< QPair<QPoint,T> >();
        QVector< QPair<QPoint,T> > oldData;
        int dataCount = 0;
        int rowCount = 0;
        // save the old data
        for ( int row = position; row <= m_rows.count() && row <= position + number - 1; ++row )
        {
            const int rowStart = m_rows.value( row - 1 );
            const int rowLength = ( row < m_rows.count() ) ? m_rows.value( row ) - rowStart : -1;
            const QVector<int> cols = m_cols.mid( rowStart, rowLength );
            const QVector<int> data = m_data.mid( rowStart, rowLength );
            for ( int col = 0; col < cols.count(); ++col )
                oldData.append( qMakePair( QPoint( cols.value( col ), row ), data.value( col ) ) );
            dataCount += data.count();
            ++rowCount;
        }
        // adjust the offsets of the following rows
        for ( int r = position + number - 1; r < m_rows.count(); ++r )
            m_rows[r] -= dataCount;
        // remove the out of range data
        while ( dataCount-- > 0 )
        {
            m_data.remove( m_rows.value( position - 1 ) );
            m_cols.remove( m_rows.value( position - 1 ) );
        }
        while ( rowCount-- > 0 )
            m_rows.remove( position - 1 );
        return oldData;
    }

    /**
     * Shifts the data right of \p rect to the left by the width of \p rect .
     * The data formerly contained in \p rect becomes overridden.
     * \return the removed data
     */
    QVector< QPair<QPoint,T> > removeShiftLeft( const QRect& rect )
    {
        Q_ASSERT( 1 <= rect.left() && rect.left() <= KS_colMax );
        QVector< QPair<QPoint,T> > oldData;
        for ( int row = rect.bottom(); row >= rect.top(); --row )
        {
            const int rowStart = m_rows.value( row - 1 );
            const int rowLength = ( row < m_rows.count() ) ? m_rows.value( row ) - rowStart : -1;
            const QVector<int> cols = m_cols.mid( rowStart, rowLength );
            for ( int col = cols.count() - 1; col >= 0; --col )
            {
                if ( cols.value( col ) >= rect.left() )
                {
                    if ( cols.value( col ) <= rect.right() )
                    {
                        oldData.append( qMakePair( QPoint( cols.value( col ), row ), m_data.value( rowStart + col ) ) );
                        m_cols.remove( rowStart + col );
                        m_data.remove( rowStart + col );
                        for ( int r = row; r < m_rows.count(); ++r )
                            --m_rows[r];
                    }
                    else
                        m_cols[rowStart + col] -= rect.width();
                }
            }
        }
        return oldData;
    }

    /**
     * Shifts the data in and right of \p rect to the right by the width of \p rect .
     * \return the data, that became out of range (shifted over the end)
     */
    QVector< QPair<QPoint,T> > insertShiftRight( const QRect& rect )
    {
        Q_ASSERT( 1 <= rect.left() && rect.left() <= KS_colMax );
        QVector< QPair<QPoint,T> > oldData;
        for ( int row = rect.left(); row <= rect.right(); ++row )
        {
            const int rowStart = m_rows.value( row - 1 );
            const int rowLength = ( row < m_rows.count() ) ? m_rows.value( row ) - rowStart : -1;
            const QVector<int> cols = m_cols.mid( rowStart, rowLength );
            for ( int col = 0; col < cols.count(); ++col )
            {
                if ( cols.value( col ) > KS_colMax - rect.left() )
                    oldData.append( qMakePair( QPoint( cols.value( col ), row ), m_data.value( rowStart + col ) ) );
                else if ( cols.value( col ) >= rect.left() )
                    m_cols[rowStart + col] += rect.width();
            }
        }
        return oldData;
    }

    /**
     * Shifts the data below \p rect to the top by the height of \p rect .
     * The data formerly contained in \p rect becomes overridden.
     * \return the removed data
     */
    QVector< QPair<QPoint,T> > removeShiftUp( const QRect& rect )
    {
        Q_ASSERT( 1 <= rect.top() && rect.top() <= KS_rowMax );
        // row's missing?
        if ( rect.top() - 1 > m_rows.count() )
            return QVector< QPair<QPoint,T> >();
        QVector< QPair<QPoint,T> > oldData;
        for ( int row = rect.top(); row <= m_rows.count() && row <= rect.bottom(); ++row )
        {
            const int rowStart = m_rows.value( row - 1 );
            const int rowLength = ( row < m_rows.count() ) ? m_rows.value( row ) - rowStart : -1;
            const QVector<int> cols = m_cols.mid( rowStart, rowLength );
            const QVector<int> data = m_data.mid( rowStart, rowLength );
            for ( int col = 0; col < cols.count(); ++col )
            {
                if ( cols.value( col ) >= rect.left() )
                {
                    if ( cols.value( col ) <= rect.right() )
                    {
                        oldData.append( qMakePair( QPoint( cols.value( col ), row ), data.value( col ) ) );
                        m_cols.remove( rowStart + col );
                        m_data.remove( rowStart + col );
                        // adjust the offsets of the following rows
                        for ( int r = row; r < m_rows.count(); ++r )
                            --m_rows[r];
                    }
                }
            }
        }
        return oldData;
    }

    /**
     * Shifts the data in and below \p rect to the bottom by the height of \p rect .
     * \return the data, that became out of range (shifted over the end)
     */
    QVector< QPair<QPoint,T> > insertShiftDown( const QRect& rect )
    {
        Q_ASSERT( 1 <= rect.top() && rect.top() <= KS_rowMax );
        // row's missing?
        if ( rect.top() - 1 > m_rows.count() )
            return QVector< QPair<QPoint,T> >();
        QVector< QPair<QPoint,T> > oldData;
        for ( int row = rect.bottom(); row >= rect.top(); --row )
        {
            const int rowStart = m_rows.value( row - 1 );
            const int rowLength = ( row < m_rows.count() ) ? m_rows.value( row ) - rowStart : -1;
            const QVector<int> cols = m_cols.mid( rowStart, rowLength );
            const QVector<int> data = m_data.mid( rowStart, rowLength );
            for ( int col = 0; col < cols.count(); ++col )
            {
                if ( cols.value( col ) >= rect.left() )
                {
                    if ( cols.value( col ) <= rect.right() )
                    {
                        oldData.append( qMakePair( QPoint( cols.value( col ), row ), data.value( col ) ) );
                        m_cols.remove( rowStart + col );
                        m_data.remove( rowStart + col );
                        // adjust the offsets of the following rows
                        for ( int r = row; r < m_rows.count(); ++r )
                            --m_rows[r];
                    }
                }
            }
        }
        return oldData;
    }

    /**
     * Retrieve the first used data in \p col .
     * Can be used in conjunction with nextColumnData() to loop through a column.
     * \return the first used data in \p col or the default data, if the column is empty.
     */
    T firstColumnData( int col ) const
    {
        Q_ASSERT( 1 <= col && col <= KS_colMax );
        return m_data.value( m_cols.indexOf( col ) );
    }

    /**
     * Retrieve the first used data in \p row .
     * Can be used in conjunction with nextRowData() to loop through a row.
     * \return the first used data in \p row or the default data, if the row is empty.
     */
    T firstRowData( int row ) const
    {
        Q_ASSERT( 1 <= row && row <= KS_rowMax );
        return m_data.value( m_rows.value( row - 1 ) );
    }

    /**
     * Retrieve the next used data in \p col after \p row .
     * Can be used in conjunction with firstColumnData() to loop through a column.
     * \return the next used data in \p col or the default data, there is no further data.
     */
    T nextColumnData( int col, int row ) const
    {
        Q_ASSERT( 1 <= col && col <= KS_colMax );
        Q_ASSERT( 1 <= row && row <= KS_rowMax );
        return m_data.value( m_cols.indexOf( col, m_rows.value( row ) ) );
    }

    /**
     * Retrieve the next used data in \p row after \p col .
     * Can be used in conjunction with firstRowData() to loop through a row.
     * \return the next used data in \p row or the default data, if there is no further data.
     */
    T nextRowData( int col, int row ) const
    {
        Q_ASSERT( 1 <= col && col <= KS_colMax );
        Q_ASSERT( 1 <= row && row <= KS_rowMax );
        // is the row not present?
        if ( row - 1 > m_rows.count() )
            return T();
        const QVector<int>::const_iterator cstart( m_cols.begin() + m_rows.value( row - 1 ) );
        const QVector<int>::const_iterator cend( ( row < m_rows.count() ) ? ( m_cols.begin() + m_rows.value( row ) ) : m_cols.end() );
        const QVector<int>::const_iterator cit = qUpperBound( cstart, cend, col );
        return ( cit == cend ) ? T() : m_data.value( m_rows.value( row - 1 ) + ( cit - cstart ) );
    }

    /**
     * For debugging/testing purposes.
     * \note only works with primitive/printable data
     */
    QString dump() const
    {
        QString str;
        // determine the dimension of the matrix (the missing column number)
        int maxCols = 0;
        for ( int row = 0; row < m_rows.count(); ++row )
        {
            const int rowStart = m_rows.value( row );
            const int rowLength = ( row + 1 < m_rows.count() ) ? m_rows.value( row + 1 ) - rowStart : -1;
            const QVector<int> cols = m_cols.mid( rowStart, rowLength );
            maxCols = qMax( maxCols, cols.value( cols.count() - 1 ) );
        }
        for ( int row = 0; row < m_rows.count(); ++row )
        {
            str += '(';
            const int rowStart = m_rows.value( row );
            const int rowLength = ( row + 1 < m_rows.count() ) ? m_rows.value( row + 1 ) - rowStart : -1;
            const QVector<int> cols = m_cols.mid( rowStart, rowLength );
            const QVector<int> data = m_data.mid( rowStart, rowLength );
            int lastCol = 0;
            for ( int col = 0; col < cols.count(); ++col )
            {
                int counter = cols.value( col ) - lastCol;
                while ( counter-- > 1 )
                    str += "  ,";
                str += QString( "%1," ).arg( data.value( col ), 2 );
                lastCol = cols.value( col );
            }
            // fill the column up to the max
            int counter = maxCols - lastCol;
            while ( counter-- > 0 )
                str += "  ,";
            // replace the last comma
            str[str.length()-1] = ')';
            str += '\n';
        }
        return str.isEmpty() ? QString( "()" ) : str.mid( 0, str.length() - 1 );
    }

    /**
     * Returns the column of the non-default data at \p index .
     * \return the data's column at \p index .
     * \see count()
     * \see row()
     * \see data()
     */
    int col( int index ) const
    {
        return m_cols.value( index );
    }

    /**
     * Returns the row of the non-default data at \p index .
     * \return the data's row at \p index .
     * \see count()
     * \see col()
     * \see data()
     */
    int row( int index ) const
    {
        return qUpperBound( m_rows, index ) - m_rows.begin();
    }

    /**
     * Returns the non-default data at \p index .
     * \return the data at \p index .
     * \see count()
     * \see col()
     * \see row()
     */
    T data( int index ) const
    {
        return m_data.value( index );
    }

private:
    QVector<int> m_cols;    // stores the column indices (beginning with one)
    QVector<int> m_rows;    // stores the row offsets in m_data
    QVector<T>   m_data;    // stores the actual non-default data
};

} // namespace KSpread

#endif // KSPREAD_POINT_STORAGE
