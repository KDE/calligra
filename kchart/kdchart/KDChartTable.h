// -*- Mode: C++ -*-
/* $Id$

   Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>
   Copyright changes to original version 2001 by Klarälvdalens Datakonsult AB
   Licensed for use in KDChart by Klarälvdalens Datakonsult AB.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this library; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

/**
 * $Id$
 */

#ifndef __KDCHARTTABLE_H__
#define __KDCHARTTABLE_H__

#include <qvaluelist.h>
#include <qshared.h>
#include <qtable.h>

#ifdef __WINDOWS__
#include <float.h>
#include <limits.h>
#include <math.h>
#define MAXDOUBLE DBL_MAX
#define MINDOUBLE DBL_MIN
#define std
#else
#include <values.h>
#include <limits.h>
#include <math.h>
#endif

#if defined SUN7 || ( defined HP11_aCC && defined HP1100 )
#define std
#endif

#include <KDChartData.h>

template
< class RowT, class ColT, class CellT > class KDChartTablePrivate : public QShared
{
public:
    KDChartTablePrivate() : QShared() {
        row_count = 0;
        col_count = 0;
    }

    KDChartTablePrivate( uint _rows, uint _cols ) : QShared() {
        for ( uint i = 0; i < _rows; i++ )
            row_list.append( RowT() );
        for ( uint j = 0; j < _cols; j++ )
            col_list.append( ColT() );
        for ( uint k = 0; k < _rows * _cols; k++ )
            matrix.append( CellT() );
        col_count = _cols;
       row_count = _rows;
    }

    KDChartTablePrivate( const KDChartTablePrivate& _t ) :
        QShared(),
    matrix( _t.matrix ),
    row_list( _t.row_list ),
    col_list( _t.col_list ),
    col_count( _t.col_count ),
    row_count( _t.row_count ) {}
    ~KDChartTablePrivate() {}

    void expand( uint _rows, uint _cols ) {
        if ( _rows > row_count ) {
            for ( uint r = 0; r < _rows - row_count; ++r ) {
                row_list.append( RowT() );
                for ( uint i = 0; i < col_count; ++i )
                    matrix.append( CellT() );
            }
            row_count = _rows;
        }
        if ( _cols > col_count ) {
            uint old = col_count;
            col_count = _cols;
            for ( uint c = 0; c < _cols - old; ++c ) {
                col_list.append( ColT() );
                for ( uint i = 0; i < row_count; ++i )
                    matrix.insert( matrix.at( i * col_count + old + c ), CellT() );
            }
        }
    }

    CellT& cell( uint _row, uint _col ) {
        ASSERT( _row < row_count && _col < col_count );
        return matrix[ static_cast < int > ( _row * col_count + _col ) ];
    }
    const CellT& cell( uint _row, uint _col ) const {
        ASSERT( _row < row_count && _col < col_count );
        return matrix[ static_cast < int > ( _row * col_count + _col ) ];
    }
    void setCell( uint _row, uint _col, const CellT& _element ) {
        ASSERT( _row < row_count && _col < col_count );
        matrix[ static_cast < int > ( _row * col_count + _col ) ] = _element;
    }

    void clearCell( uint _row, uint _col ) {
        ASSERT( _row < row_count && _col < col_count );
        matrix[ static_cast < int > ( _row * col_count + _col ) ].clearValue();
    }

    void clearAllCells() {
        for ( uint r = 0; r < row_count; ++r )
            for ( uint c = 0; c < col_count; ++c )
                matrix[ static_cast < int > ( r * col_count + c ) ].clearValue();
    }

    RowT& row( uint _row ) {
        ASSERT( _row < row_count );
        return row_list[ _row ];
    }

    const RowT& row( uint _row ) const {
        ASSERT( _row < row_count );
        return row_list[ _row ];
    }

    void setRow( uint _row, const RowT& _v ) {
        ASSERT( _row < row_count );
        row_list[ _row ] = _v;
    }

    ColT& col( uint _col ) {
        ASSERT( _col < col_count );
        return col_list[ _col ];
    }


    const ColT& col( uint _col ) const {
        ASSERT( _col < col_count );
        return col_list[ _col ];
    }

    void setCol( uint _col, const ColT& _v ) {
        ASSERT( _col < col_count );
        col_list[ _col ] = _v;
    }

    void insertColumn( uint _c ) {
        ASSERT( _c <= col_count );
        ++col_count;
        typename QValueList < CellT > ::Iterator it;
        for ( uint i = 0; i < row_count; ++i ) {
            it = matrix.at( i * col_count + _c );
            matrix.insert( it, CellT() );
        }

        typename QValueList < ColT > ::Iterator it2 = col_list.at( _c );
        col_list.insert( it2, ColT() );
    }

    void insertRow( uint _r ) {
        ASSERT( _r <= row_count );
        ++row_count;
        typename QValueList < CellT > ::Iterator it = matrix.at( _r * col_count );
        for ( uint i = 0; i < col_count; ++i )
            matrix.insert( it, CellT() );

        typename QValueList < RowT > ::Iterator it2 = row_list.at( _r );
        row_list.insert( it2, RowT() );
    }

    void removeColumn( uint _c ) {
        ASSERT( _c < col_count );
        --col_count;
        typename QValueList < CellT > ::Iterator it;
        for ( uint i = 0; i < row_count; ++i ) {
            it = matrix.at( i * col_count + _c );
            matrix.remove( it );
        }

        typename QValueList < ColT > ::Iterator it2 = col_list.at( _c );
        col_list.remove( it2 );
    }

    void removeRow( uint _r ) {
        ASSERT( _r < row_count );
        --row_count;
        typename QValueList < CellT > ::Iterator it = matrix.at( _r * col_count );
        for ( uint i = 0; i < col_count; ++i )
            it = matrix.remove( it );

        typename QValueList < RowT > ::Iterator it2 = row_list.at( _r );
        row_list.remove( it2 );
    }

    QValueList < CellT > matrix;
    QValueList < RowT > row_list;
    QValueList < ColT > col_list;

    uint col_count;
    uint row_count;
};


template
< class RowT, class ColT, class CellT > class KDChartTable
{
private:
    typedef KDChartTablePrivate < RowT, ColT, CellT > Priv;
    uint _usedRows, _usedCols;

public:
    /**
    * Typedefs
    */
    typedef typename QValueList < CellT > ::Iterator Iterator;
    typedef typename QValueList < CellT > ::ConstIterator ConstIterator;

    typedef typename QValueList < RowT > ::Iterator RowIterator;
    typedef typename QValueList < RowT > ::ConstIterator ConstRowIterator;

    typedef typename QValueList < ColT > ::Iterator ColIterator;
    typedef typename QValueList < ColT > ::ConstIterator ConstColIterator;

    /**
    * API
    */
    KDChartTable() {
        sh = new Priv;
        _usedCols = 0;
        _usedRows = 0;
    }
    KDChartTable( uint _rows, uint _cols ) {
        sh = new Priv( _rows, _cols );
        _usedRows = _rows;
        _usedCols = _cols;
    }

    KDChartTable( const KDChartTable& _t ) {
        _usedRows = _t._usedRows;
        _usedCols = _t._usedCols;
        sh = _t.sh;
        sh->ref();
    }

    virtual ~KDChartTable() {
        if ( sh->deref() )
            delete sh;
    }

    KDChartTable& operator=( const KDChartTable& t ) {
        if ( &t == this )
            return * this;
        _usedRows = t._usedRows;
        _usedCols = t._usedCols;
        t.sh->ref();
        if ( sh->deref() )
            delete sh;
        sh = t.sh;
        return *this;
    }

    Iterator begin() {
        return sh->matrix.begin();
    }

    ConstIterator begin() const {
        return sh->matrix.begin();
    }

    Iterator end() {
        return sh->matrix.end();
    }

    ConstIterator end() const {
        return sh->matrix.end();
    }

    ColIterator colBegin() {
        return sh->col_list.begin();
    }

    ConstColIterator colBegin() const {
        return sh->col_list.begin();
    }

    ColIterator colEnd() {
        return sh->col_list.end();
    }

    ConstColIterator colEnd() const
{
    return sh->col_list.end();
}

    RowIterator rowBegin() {
        return sh->row_list.begin();
    }

    ConstRowIterator rowBegin() const {
        return sh->row_list.begin();
    }

    RowIterator rowEnd() {
        return sh->row_list.end();
    }

    ConstRowIterator rowEnd() const {
        return sh->row_list.end();
    }

    bool isEmpty() const {
        return ( sh->col_count == 0 && sh->row_count == 0 );
    }

    uint cols() const {
        return sh->col_count;
    }

    uint rows() const {
        return sh->row_count;
    }

    CellT& cell( uint _row, uint _col ) {
        detach();
        return sh->cell( _row, _col );
    }

    const CellT& cell( uint _row, uint _col ) const {
        return sh->cell( _row, _col );
    }

    void setCell( uint _row, uint _col, const CellT& _element ) {
        detach();
        sh->setCell( _row, _col, _element );
    }

    void clearCell( uint _row, uint _col ) {
        detach();
        sh->clearCell( _row, _col );
    }

    void clearAllCells() {
        detach();
        sh->clearAllCells();
    }

    RowT& row( uint _row ) {
        detach();
        return sh->row( _row );
    }

    const RowT& row( uint _row ) const {
        return sh->row( _row );
    }

    void setRow( uint _row, const RowT& _v ) {
        detach();
        sh->setRow( _row, _v );
    }

    ColT& col( uint _col ) {
        detach();
        return sh->col( _col );
    }

    const ColT& col( uint _col ) const {
        return sh->col( _col );
    }

    void setCol( uint _col, const ColT& _v ) {
        detach();
        sh->setCol( _col, _v );
    }

    void insertColumn( uint _c ) {
        detach();
        sh->insertColumn( _c );
    }

    void insertRow( uint _r ) {
        detach();
        sh->insertRow( _r );
    }

    void removeColumn( uint _c ) {
        detach();
        sh->removeColumn( _c );
    }

    void removeRow( uint _r ) {
        detach();
        sh->removeRow( _r );
    }

    void expand( uint _rows, uint _cols ) {
        detach();
        sh->expand( _rows, _cols );
    }

    void setUsedRows( uint _rows ) {
        ASSERT( _rows <= rows() );
        _usedRows = _rows;
    }

    uint usedRows() const {
        return _usedRows;
    }

    void setUsedCols( uint _cols ) {
        ASSERT( _cols <= cols() );
        _usedCols = _cols;
    }

    uint usedCols() const {
        return _usedCols;
    }

private:
    /**
    * Helpers
    */
    void detach()
{
    if ( sh->count > 1 ) {
        sh->deref();
        sh = new Priv( *sh );
    }
}

/**
 * Variables
 */
Priv* sh;
};

typedef KDChartTable < int, int, KDChartData > _KDChartTableData;

class KDChartTableData : public _KDChartTableData
{
public:
    KDChartTableData()
{}
    ;
    KDChartTableData( uint _rows, uint _cols ) :
        _KDChartTableData( _rows, _cols )
{}
KDChartTableData( const KDChartTableData& _t ) :
    _KDChartTableData( _t )
{}

double maxValue() const;
double minValue() const;
double maxColSum() const;
double minColSum() const;
double maxColSum( uint row, uint row2 ) const;
double minColSum( uint row, uint row2 ) const;
double colSum( uint col ) const;
double colAbsSum( uint col ) const;
double rowSum( uint row ) const;
double rowAbsSum( uint row ) const;
double maxInColumn( uint col ) const;
double minInColumn( uint col ) const;
double maxInRow( uint row ) const;
double minInRow( uint row ) const;
double maxInRows( uint row, uint row2 ) const;
double minInRows( uint row, uint row2 ) const;
uint lastPositiveCellInColumn( uint col ) const;
void importFromQTable( QTable* table );
};


inline double KDChartTableData::maxValue() const
{
    double maxValue = 0.0;
    bool bStart = true;
    for ( uint row = 0; row < usedRows(); row++ ) {
        for ( uint col = 0; col < usedCols(); col++ ) {
            KDChartData d = cell( row, col );
            if ( d.isDouble() ) {
                if ( bStart ) {
                    maxValue = d.doubleValue();
                    bStart = false;
                } else
                    maxValue = QMAX( maxValue, d.doubleValue() );
            }
        }
    }
    return maxValue;
}



inline double KDChartTableData::minValue() const
{
    double minValue = 0.0;
    bool bStart = true;
    for ( uint row = 0; row < usedRows(); row++ ) {
        for ( uint col = 0; col < usedCols(); col++ ) {
            KDChartData d = cell( row, col );
            if ( d.isDouble() ) {
                if ( bStart ) {
                    minValue = d.doubleValue();
                    bStart = false;
                } else
                    minValue = QMIN( minValue, d.doubleValue() );
            }
        }
    }
    return minValue;
}


inline double KDChartTableData::maxColSum() const
{
    double maxValue = 0.0;
    bool bStart = true;
    for ( uint col = 0; col < usedCols(); col++ ) {
        double colValue = colSum( col );
        if ( bStart ) {
            maxValue = colValue;
            bStart = false;
        } else
            maxValue = QMAX( maxValue, colValue );
    }
    return maxValue;
}


inline double KDChartTableData::minColSum() const
{
    double minValue = 0.0;
    bool bStart = true;
    for ( uint col = 0; col < usedCols(); col++ ) {
        double colValue = colSum( col );
        if ( bStart ) {
            minValue = colValue;
            bStart = false;
        } else
            minValue = QMIN( minValue, colValue );
    }

    return minValue;
}


inline double KDChartTableData::maxColSum( uint row, uint row2 ) const
{
    double maxValue = 0;
    bool bStart = true;
    if ( 0 < usedRows() ) {
        uint a = row;
        uint z = row2;
        if ( usedRows() >= a )
            a = usedRows() - 1;
        if ( usedRows() >= z )
            z = usedRows() - 1;
        for ( uint col = 0; col < usedCols(); col++ ) {
            double valueValue = 0.0;
            for ( uint row = a; row <= z; row++ ) {
                KDChartData d = cell( row, col );
                if ( d.isDouble() )
                    valueValue += d.doubleValue();
            }
            if ( bStart ) {
                maxValue = valueValue;
                bStart = false;
            } else
                maxValue = QMAX( maxValue, valueValue );
        }
    }
    return maxValue;
}


inline double KDChartTableData::minColSum( uint row, uint row2 ) const
{
    double minValue = 0;
    bool bStart = true;
    if ( 0 < usedRows() ) {
        uint a = row;
        uint z = row2;
        if ( usedRows() >= a )
            a = usedRows() - 1;
        if ( usedRows() >= z )
            z = usedRows() - 1;
        for ( uint col = 0; col < usedCols(); col++ ) {
            double valueValue = 0.0;
            for ( uint row = a; row <= z; row++ ) {
                KDChartData d = cell( row, col );
                if ( d.isDouble() )
                    valueValue += d.doubleValue();
            }
            if ( bStart ) {
                minValue = valueValue;
                bStart = false;
            } else
                minValue = QMIN( minValue, valueValue );
        }
    }
    return minValue;
}


inline double KDChartTableData::colSum( uint col ) const
{
    double sum = 0.0;
    for ( uint row = 0; row < usedRows(); row++ ) {
        KDChartData d = cell( row, col );
        if ( d.isDouble() )
            sum += d.doubleValue();
    }

    return sum;
}


inline double KDChartTableData::colAbsSum( uint col ) const
{
    double sum = 0.0;
    for ( uint row = 0; row < usedRows(); row++ ) {
        KDChartData d = cell( row, col );
        if ( d.isDouble() )
            sum += fabs( d.doubleValue() );
    }

    return sum;
}


inline double KDChartTableData::rowSum( uint row ) const
{
    double sum = 0.0;
    for ( uint col = 0; col < usedCols(); col++ ) {
        KDChartData d = cell( row, col );
        if ( d.isDouble() )
            sum += d.doubleValue();
    }
    return sum;
}


inline double KDChartTableData::rowAbsSum( uint row ) const
{
    double sum = 0.0;
    for ( uint col = 0; col < usedCols(); col++ ) {
        KDChartData d = cell( row, col );
        if ( d.isDouble() )
            sum += fabs( d.doubleValue() );
    }
    return sum;
}


inline double KDChartTableData::maxInColumn( uint col ) const
{
    double maxValue = 0.0;
    bool bStart = true;
    for ( uint row = 0; row < usedRows(); row++ ) {
        KDChartData d = cell( row, col );
        if ( d.isDouble() ) {
            double cellValue = d.doubleValue();
            if ( bStart ) {
                maxValue = cellValue;
                bStart = false;
            } else
                maxValue = QMAX( maxValue, cellValue );
        }
    }

    return maxValue;
}


inline double KDChartTableData::minInColumn( uint col ) const
{
    double minValue = 0.0;
    bool bStart = true;
    for ( uint row = 0; row < usedRows(); row++ ) {
        KDChartData d = cell( row, col );
        if ( d.isDouble() ) {
            double cellValue = d.doubleValue();
            if ( bStart ) {
                minValue = cellValue;
                bStart = false;
            } else
                minValue = QMIN( minValue, cellValue );
        }
    }

    return minValue;
}


inline double KDChartTableData::maxInRow( uint row ) const
{
    double maxValue = MINDOUBLE;
    bool bStart = true;
    if ( UINT_MAX > row ) {
        for ( uint col = 0; col < usedCols(); col++ ) {
            KDChartData d = cell( row, col );
            if ( d.isDouble() ) {
                double cellValue = d.doubleValue();
                if ( bStart ) {
                    maxValue = cellValue;
                    bStart = false;
                } else
                    maxValue = QMAX( maxValue, cellValue );
            }
        }
    }
    return maxValue;
}


inline double KDChartTableData::minInRow( uint row ) const
{
    double minValue = MAXDOUBLE;
    bool bStart = true;
    if ( UINT_MAX > row ) {
        for ( uint col = 0; col < usedCols(); col++ ) {
            KDChartData d = cell( row, col );
            if ( d.isDouble() ) {
                double cellValue = d.doubleValue();
                if ( bStart ) {
                    minValue = cellValue;
                    bStart = false;
                } else
                    minValue = QMIN( minValue, cellValue );
            }
        }
    }
    return minValue;
}


inline double KDChartTableData::maxInRows( uint row, uint row2 ) const
{
    double maxValue = 0.0;
    bool bStart = true;
    if ( 0 < usedRows() ) {
        uint a = row;
        uint z = row2;
        // qDebug("KDChartTableData::maxInRows()   (1)     a: %u     z: %u", a, z);
        if ( usedRows() <= a )
            a = usedRows() - 1;
        if ( usedRows() <= z )
            z = usedRows() - 1;
        // qDebug("KDChartTableData::maxInRows()   (2)     a: %u     z: %u", a, z);
        for ( uint row = a; row <= z; ++row ) {
            for ( uint col = 0; col < usedCols(); ++col ) {
                KDChartData d = cell( row, col );
                if ( d.isDouble() ) {
                    double cellValue = d.doubleValue();
                    if ( bStart ) {
                        maxValue = cellValue;
                        bStart = false;
                    } else
                        maxValue = QMAX( maxValue, cellValue );
                }
            }
        }
    }
    return maxValue;
}


inline double KDChartTableData::minInRows( uint row, uint row2 ) const
{
    double minValue = 0.0;
    bool bStart = true;
    if ( 0 < usedRows() ) {
        uint a = row;
        uint z = row2;
        // qDebug("KDChartTableData::minInRows()   (1)     a: %u     z: %u", a, z);
        if ( usedRows() <= a )
            a = usedRows() - 1;
        if ( usedRows() <= z )
            z = usedRows() - 1;
        //qDebug("KDChartTableData::minInRows()   (2)     a: %u     z: %u", a, z);
        for ( uint row = a; row <= z; ++row ) {
            for ( uint col = 0; col < usedCols(); ++col ) {
                KDChartData d = cell( row, col );
                if ( d.isDouble() ) {
                    double cellValue = d.doubleValue();
                    if ( bStart ) {
                        minValue = cellValue;
                        bStart = false;
                    } else
                        minValue = QMIN( minValue, cellValue );
                }
            }
        }
    }
    return minValue;
}


inline uint KDChartTableData::lastPositiveCellInColumn( uint col ) const
{
    uint ret = UINT_MAX;
    for ( uint row = 0; row < usedRows(); row++ ) {
        KDChartData d = cell( row, col );
        if ( d.isDouble() && d.doubleValue() > 0 )
            ret = row;
    }
    return ret;
}


inline void KDChartTableData::importFromQTable( QTable* table )
{
    if( table->numRows() > (int)rows() ||
        table->numCols() > (int)cols() )
        expand( table->numRows(), table->numCols() );
    setUsedRows( table->numRows() );
    setUsedCols( table->numCols() );
    for( int row = 0; row < table->numRows(); row++ )
        for( int col = 0; col < table->numCols(); col++ ) {
            QString cellContents = table->text( row, col );
            if( !cellContents.isEmpty() ) {
                // First try to parse a double
                bool ok = false;
                double value = cellContents.toDouble( &ok );
                if( ok ) {
                    // there was a double
                    setCell( row, col, KDChartData( value ) );
                } else {
                    // no double, but at least a string
                    setCell( row, col, KDChartData( cellContents ) );
                }
            } // don't do anything if no contents
        }
}

#endif
