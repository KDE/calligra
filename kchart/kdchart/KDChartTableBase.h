/* -*- Mode: C++ -*-
   $Id$
   KDChart - a multi-platform charting engine
*/

/****************************************************************************
 ** Copyright (C) 2001-2003 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KDChart library.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** Licensees holding valid commercial KDChart licenses may use this file in
 ** accordance with the KDChart Commercial License Agreement provided with
 ** the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.klaralvdalens-datakonsult.se/?page=products for
 **   information about KDChart Commercial License Agreements.
 **
 ** Contact info@klaralvdalens-datakonsult.se if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/
#ifndef __KDCHARTTABLEINTERFACE_H__
#define __KDCHARTTABLEINTERFACE_H__

#include <KDChartData.h>

#include <math.h>
#include <limits.h>

/**
   \file KDChartTableBase.h

   \brief Provides a table class holding all data values
   that are to be used in a chart.
*/


/**
   \brief Encapsulates all data values that are to be used in a chart.

   \note To create your data table you would <em>not</em> use a
   \c KDChartTableDataBase but instantiate the class \c KDChartTableData.
   The \c KDChartTableData class is an auxiliary class: depending on your
   Qt version it will be mapped onto a \c KDChartVectorTableData or onto
   a \c KDChartListTableData both of which are derived from
   \c KDChartTableDataBase and implement all of its functions.
   Thus you would create a table of 3 datasets with 25 cells each like this:
   \verbatim

   KDChartTableData myData( 3, 25 );

   \endverbatim

   Data values are of type KDChartData.

   You may adjust or modify your table like this:

   \li Entering the data can be done either manually using \c setCell()
   or by passing a QTable to the \c importFromQTable() function.

   \li Performance of KD Chart can be increased by specifying the number
   of rows and or the number of columns actually used: \c setUsedRows()
   and/or \c setUsedCols() prevents KD Chart from iterating over thousands
   of empty rows/cols that might follow your data cells in case your
   table is much bigger than needed.

   \li In case you want to increase your table's size without using the
   data stored in it please call the \c expand() function with the new
   total number of rows and cells.

   \li Accessing one data cell is possible by the \c cell() method,
   see the KDChartData documentation for details on the available functions,
   e.g. you might assign a special property set ID to all cells with a
   future absicssa axis value:
   \verbatim

   const QDateTime currentTime( QDateTime::currentDateTime() );
   for( int iCell = 0;  iCell < usedValues;  ++iCell ){
   KDChartData& cell = myData.cell( 0, iCell );
   // assign special property set ID if X value is in the future
   if( cell.isDateTime( 2 ) && cell.dateTimeValue( 2 ) > currentTime )
   cell.setPropertySet( idProp_FutureValues );
   }

   \endverbatim

   \note All of the other functions provided by KDChartTableDataBase are
   either used internally by KD Chart or they are const methods
   returning some usefull figures like the sum of all values in a row...
*/
class KDChartTableDataBase
{
public:
    KDChartTableDataBase() : _sorted(false) {}
    virtual ~KDChartTableDataBase() {}

    // PCH: I need these to be virtual so I can override them!
    // Other methods should also be virtual, but this is the
    // MINIMUM I need right now.
    virtual double maxValue( int coordinate=1 ) const;
    virtual double minValue( int coordinate=1 ) const;

    QDateTime maxDtValue( int coordinate=1 ) const;
    QDateTime minDtValue( int coordinate=1 ) const;
    double maxColSum() const;
    double minColSum() const;
    double maxColSum( uint row, uint row2 ) const;
    double minColSum( uint row, uint row2 ) const;
    double colSum( uint col ) const;
    double colAbsSum( uint col ) const;
    double maxRowSum() const;
    double minRowSum() const;
    double rowSum( uint row ) const;
    double rowAbsSum( uint row ) const;
    double maxInColumn( uint col ) const;
    double minInColumn( uint col ) const;
    double maxInRow( uint row ) const;
    double minInRow( uint row ) const;
    double maxInRows( uint row, uint row2, int coordinate=1 ) const;
    double minInRows( uint row, uint row2, int coordinate=1 ) const;
    QDateTime maxDtInRows( uint row, uint row2, int coordinate=1 ) const;
    QDateTime minDtInRows( uint row, uint row2, int coordinate=1 ) const;
    uint lastPositiveCellInColumn( uint col ) const;
    bool cellsHaveSeveralCoordinates(KDChartData::ValueType* type2Ref) const;
    bool cellsHaveSeveralCoordinates(uint row1=0, uint row2=UINT_MAX,
                                     KDChartData::ValueType* type2Ref=NULL) const;
    KDChartData::ValueType cellsValueType( uint row1=0, uint row2=UINT_MAX,
                                           int coordinate=1 ) const;
    KDChartData::ValueType cellsValueType( int coordinate=1 ) const;
    void importFromQTable( QTable* table );
    void setSorted(bool sorted);
    bool sorted() const;
    virtual uint rows() const = 0;
    virtual uint cols() const = 0;
    virtual void setUsedRows( uint _rows ) = 0;
    virtual uint usedRows() const = 0;
    virtual void setUsedCols( uint _cols ) = 0;
    virtual uint usedCols() const = 0;
    virtual void setCell( uint _row, uint _col,
                          const KDChartData& _element ) = 0;
    virtual const KDChartData& cell( uint _row, uint _col ) const = 0;
    virtual void expand( uint _rows, uint _cols ) = 0;
private:
    bool _sorted;
};


inline bool KDChartTableDataBase::cellsHaveSeveralCoordinates(
    KDChartData::ValueType* type2Ref ) const
{
    return cellsHaveSeveralCoordinates( 0, UINT_MAX, type2Ref );
}


inline bool KDChartTableDataBase::cellsHaveSeveralCoordinates(
    uint row1,
    uint row2,
    KDChartData::ValueType* type2Ref ) const
{
    // return true if all wanted datasets have at least two coordinates
    // stored in all of their cells - BUT only if these coordinates are
    // of equal type for each of the cells
    // note: We skip cells that are empty, this means having
    //       set neither coordinate #1 nor coordinate #2.
    bool severalCoordinates = row1 < usedRows();
    if( severalCoordinates ) {
        severalCoordinates = false;
        KDChartData::ValueType testType = KDChartData::NoValue;
        uint r2 = (UINT_MAX == row2)
                  ? (usedRows()-1)
                  : QMIN( row2, usedRows()-1 );
        for ( uint row = row1; row <= r2; ++row ){
            for ( uint col = 0; col < usedCols(); ++col ){
                const KDChartData& d = cell( row, col );
                if( d.hasValue( 2 ) ){
                    if( (KDChartData::NoValue != testType) &&
                        (d.valueType( 2 )     != testType) ){
                        severalCoordinates = false;
                        break;
                    }else{
                        testType = d.valueType( 2 );
                        if( NULL != type2Ref )
                            *type2Ref = testType;
                        severalCoordinates = true;
                    }
                }else if( d.hasValue( 1 ) ){
                    severalCoordinates = false;
                    break;
                }
            }
        }
    }
    return severalCoordinates;
}


inline KDChartData::ValueType KDChartTableDataBase::cellsValueType(
    uint row1,
    uint row2,
    int coordinate ) const
{
    uint r2 = (UINT_MAX == row2)
              ? (usedRows()-1)
              : QMIN( row2, usedRows()-1 );
    for ( uint row = row1; row <= r2; ++row )
        for ( uint col = 0; col < usedCols(); ++col ) {
            const KDChartData& d = cell( row, col );
            if( d.hasValue( coordinate ) )
                return d.valueType( coordinate );
        }
    return KDChartData::NoValue;
}


inline KDChartData::ValueType KDChartTableDataBase::cellsValueType(
    int coordinate ) const
{
    return cellsValueType( 0, UINT_MAX, coordinate );
}


inline double KDChartTableDataBase::maxValue( int coordinate ) const
{
    double maxValue = 0.0;
    bool bStart = true;
    for ( uint row = 0; row < usedRows(); row++ ) {
        for ( uint col = 0; col < usedCols(); col++ ) {
            const KDChartData& d = cell( row, col );
            if ( d.isNormalDouble( coordinate ) ) {
                if ( bStart ) {
                    maxValue = d.doubleValue( coordinate );
                    bStart = false;
                } else
                    maxValue = QMAX( maxValue, d.doubleValue( coordinate ) );
            }
        }
    }
    return maxValue;
}



inline double KDChartTableDataBase::minValue( int coordinate ) const
{
    double minValue = 0.0;
    bool bStart = true;
    for ( uint row = 0; row < usedRows(); row++ ) {
        for ( uint col = 0; col < usedCols(); col++ ) {
            const KDChartData& d = cell( row, col );
            if ( d.isNormalDouble( coordinate ) ) {
                if ( bStart ) {
                    minValue = d.doubleValue( coordinate );
                    bStart = false;
                } else
                    minValue = QMIN( minValue, d.doubleValue( coordinate ) );
            }
        }
    }
    return minValue;
}


inline QDateTime KDChartTableDataBase::maxDtValue( int coordinate ) const
{
    QDateTime maxValue = QDateTime( QDate(1970,1,1) );
    bool bStart = true;
    for ( uint row = 0; row < usedRows(); row++ ) {
        for ( uint col = 0; col < usedCols(); col++ ) {
            const KDChartData& d = cell( row, col );
            if ( d.isDateTime( coordinate ) ) {
                if ( bStart ) {
                    maxValue = d.dateTimeValue( coordinate );
                    bStart = false;
                } else
                    maxValue = QMAX(maxValue, d.dateTimeValue( coordinate ));
            }
        }
    }
    return maxValue;
}



inline QDateTime KDChartTableDataBase::minDtValue( int coordinate ) const
{
    QDateTime minValue = QDateTime( QDate(1970,1,1) );
    bool bStart = true;
    for ( uint row = 0; row < usedRows(); row++ ) {
        for ( uint col = 0; col < usedCols(); col++ ) {
            const KDChartData& d = cell( row, col );
            if ( d.isDateTime( coordinate ) ) {
                if ( bStart ) {
                    minValue = d.dateTimeValue( coordinate );
                    bStart = false;
                } else
                    minValue = QMIN(minValue, d.dateTimeValue( coordinate ));
            }
        }
    }
    return minValue;
}


inline double KDChartTableDataBase::maxColSum() const
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


inline double KDChartTableDataBase::minColSum() const
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


inline double KDChartTableDataBase::maxColSum( uint row, uint row2 ) const
{
    double maxValue = 0;
    bool bStart = true;
    if ( 0 < usedRows() ) {
        uint a = row;
        uint z = row2;
        if ( usedRows() <= a )
            a = usedRows() - 1;
        if ( usedRows() <= z )
            z = usedRows() - 1;
        for ( uint col = 0; col < usedCols(); col++ ) {
            double valueValue = 0.0;
            for ( uint row = a; row <= z; row++ ) {
                const KDChartData& d = cell( row, col );
                if ( d.isNormalDouble() )
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


inline double KDChartTableDataBase::minColSum( uint row, uint row2 ) const
{
    double minValue = 0;
    bool bStart = true;
    if ( 0 < usedRows() ) {
        uint a = row;
        uint z = row2;
        if ( usedRows() <= a )
            a = usedRows() - 1;
        if ( usedRows() <= z )
            z = usedRows() - 1;
        for ( uint col = 0; col < usedCols(); col++ ) {
            double valueValue = 0.0;
            for ( uint row = a; row <= z; row++ ) {
                const KDChartData& d = cell( row, col );
                if ( d.isNormalDouble() )
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


inline double KDChartTableDataBase::colSum( uint col ) const
{
    double sum = 0.0;
    for ( uint row = 0; row < usedRows(); row++ ) {
        const KDChartData& d = cell( row, col );
        if ( d.isNormalDouble() )
            sum += d.doubleValue();
    }

    return sum;
}


inline double KDChartTableDataBase::colAbsSum( uint col ) const
{
    double sum = 0.0;
    for ( uint row = 0; row < usedRows(); row++ ) {
        const KDChartData& d = cell( row, col );
        if ( d.isNormalDouble() )
            sum += fabs( d.doubleValue() );
    }

    return sum;
}


inline double KDChartTableDataBase::maxRowSum() const
{
    double maxValue = 0.0;
    bool bStart = true;
    for ( uint row = 0; row < usedRows(); row++ ) {
        double rowValue = rowSum( row );
        if ( bStart ) {
            maxValue = rowValue;
            bStart = false;
        } else
            maxValue = QMAX( maxValue, rowValue );
    }
    return maxValue;
}


inline double KDChartTableDataBase::minRowSum() const
{
    double minValue = 0.0;
    bool bStart = true;
    for ( uint row = 0; row < usedRows(); row++ ) {
        double rowValue = rowSum( row );
        if ( bStart ) {
            minValue = rowValue;
            bStart = false;
        } else
            minValue = QMIN( minValue, rowValue );
    }

    return minValue;
}


inline double KDChartTableDataBase::rowSum( uint row ) const
{
    double sum = 0.0;
    for ( uint col = 0; col < usedCols(); col++ ) {
        const KDChartData& d = cell( row, col );
        if ( d.isNormalDouble() )
            sum += d.doubleValue();
    }
    return sum;
}


inline double KDChartTableDataBase::rowAbsSum( uint row ) const
{
    double sum = 0.0;
    for ( uint col = 0; col < usedCols(); col++ ) {
        const KDChartData& d = cell( row, col );
        if ( d.isNormalDouble() )
            sum += fabs( d.doubleValue() );
    }
    return sum;
}


inline double KDChartTableDataBase::maxInColumn( uint col ) const
{
    double maxValue = 0.0;
    bool bStart = true;
    for ( uint row = 0; row < usedRows(); row++ ) {
        const KDChartData& d = cell( row, col );
        if ( d.isNormalDouble() ) {
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


inline double KDChartTableDataBase::minInColumn( uint col ) const
{
    double minValue = 0.0;
    bool bStart = true;
    for ( uint row = 0; row < usedRows(); row++ ) {
        const KDChartData& d = cell( row, col );
        if ( d.isNormalDouble() ) {
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


inline double KDChartTableDataBase::maxInRow( uint row ) const
{
    double maxValue = DBL_MIN;
    bool bStart = true;
    if ( UINT_MAX > row ) {
        for ( uint col = 0; col < usedCols(); col++ ) {
            const KDChartData& d = cell( row, col );
            if ( d.isNormalDouble() ) {
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


inline double KDChartTableDataBase::minInRow( uint row ) const
{
    double minValue = DBL_MAX;
    bool bStart = true;
    if ( UINT_MAX > row ) {
        for ( uint col = 0; col < usedCols(); col++ ) {
            const KDChartData& d = cell( row, col );
            if ( d.isNormalDouble() ) {
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


inline double KDChartTableDataBase::maxInRows( uint row, uint row2, int coordinate ) const
{
    double maxValue = 0.0;
    bool bStart = true;
    if ( 0 < usedRows() ) {
        uint a = row;
        uint z = row2;
        // qDebug("KDChartTableDataBase::maxInRows()   (1)     a: %u     z: %u", a, z);
        if ( usedRows() <= a )
            a = usedRows() - 1;
        if ( usedRows() <= z )
            z = usedRows() - 1;
        // qDebug("KDChartTableDataBase::maxInRows()   (2)     a: %u     z: %u", a, z);
        for ( uint row = a; row <= z; ++row ) {
            for ( uint col = 0; col < usedCols(); ++col ) {
                const KDChartData& d = cell( row, col );
                if ( d.isNormalDouble( coordinate ) ) {
                    double cellValue = d.doubleValue( coordinate );
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


inline double KDChartTableDataBase::minInRows( uint row, uint row2, int coordinate ) const
{
    double minValue = 0.0;
    bool bStart = true;
    if ( 0 < usedRows() ) {
        uint a = row;
        uint z = row2;
        // qDebug("KDChartTableDataBase::minInRows()   (1)     a: %u     z: %u", a, z);
        if ( usedRows() <= a )
            a = usedRows() - 1;
        if ( usedRows() <= z )
            z = usedRows() - 1;
        //qDebug("KDChartTableDataBase::minInRows()   (2)     a: %u     z: %u", a, z);
        for ( uint row = a; row <= z; ++row ) {
            for ( uint col = 0; col < usedCols(); ++col ) {
                const KDChartData& d = cell( row, col );
                if ( d.isNormalDouble( coordinate ) ) {
                    double cellValue = d.doubleValue( coordinate );
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


inline QDateTime KDChartTableDataBase::maxDtInRows( uint row, uint row2,
                                                    int coordinate ) const
{
    QDateTime maxValue = QDateTime( QDate(1970,1,1) );
    bool bStart = true;
    if ( 0 < usedRows() ) {
        uint a = row;
        uint z = row2;
        if ( usedRows() <= a )
            a = usedRows() - 1;
        if ( usedRows() <= z )
            z = usedRows() - 1;
        for ( uint row = a; row <= z; ++row ) {
            for ( uint col = 0; col < usedCols(); ++col ) {
                const KDChartData& d = cell( row, col );
                if ( d.isDateTime( coordinate ) ) {
                    QDateTime cellValue = d.dateTimeValue( coordinate );
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


inline QDateTime KDChartTableDataBase::minDtInRows( uint row, uint row2,
                                                    int coordinate ) const
{
    QDateTime minValue = QDateTime( QDate(1970,1,1) );
    bool bStart = true;
    if ( 0 < usedRows() ) {
        uint a = row;
        uint z = row2;
        if ( usedRows() <= a )
            a = usedRows() - 1;
        if ( usedRows() <= z )
            z = usedRows() - 1;
        for ( uint row = a; row <= z; ++row ) {
            for ( uint col = 0; col < usedCols(); ++col ) {
                const KDChartData& d = cell( row, col );
                if ( d.isDateTime( coordinate ) ) {
                    QDateTime cellValue = d.dateTimeValue( coordinate );
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


inline uint KDChartTableDataBase::lastPositiveCellInColumn( uint col ) const
{
    uint ret = UINT_MAX;
    for ( uint row = 0; row < usedRows(); row++ ) {
        const KDChartData& d = cell( row, col );
        if ( d.isDouble() && d.doubleValue() > 0 )
            ret = row;
    }
    return ret;
}


inline void KDChartTableDataBase::importFromQTable( QTable* table )
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
    setSorted( false );
}


inline void KDChartTableDataBase::setSorted(bool sorted)
{
    _sorted = sorted;
}
inline bool KDChartTableDataBase::sorted() const
{
    return _sorted;
}


#endif
