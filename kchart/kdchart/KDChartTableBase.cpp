/* -*- Mode: C++ -*-
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
#include <math.h>
#include <limits.h>
#include <qtable.h>

#include <KDChartTableBase.h>


/**
   \class KDChartTableDataBase KDChartTableBase.h
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

   Data values may be specified via \c setCell().
   Cell specific properties may be specified via \c setCellProp().

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

   \li Accessing one data cell is possible via \c cellCoord() 
   and via \c cellProp(),
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


void KDChartTableDataBase::setUsedRows( uint _rows ) {
    _usedRows = _rows;
    _useUsedRows = true;
}
uint KDChartTableDataBase::usedRows() const {
    return _useUsedRows ? _usedRows : rows();
}
void KDChartTableDataBase::setUsedCols( uint _cols ) {
    _usedCols = _cols;
    _useUsedCols = true;
}
uint KDChartTableDataBase::usedCols() const {
    return _useUsedCols ? _usedCols : cols();
}


bool KDChartTableDataBase::cellsHaveSeveralCoordinates(
    QVariant::Type* type2Ref ) const
{
    return cellsHaveSeveralCoordinates( 0, UINT_MAX, type2Ref );
}


bool KDChartTableDataBase::cellsHaveSeveralCoordinates(
    uint row1,
    uint row2,
    QVariant::Type* type2Ref ) const
{
    // return true if all wanted datasets have at least two coordinates
    // stored in all of their cells - BUT only if these coordinates are
    // of equal type for each of the cells
    // note: We skip cells that are empty, this means having
    //       set neither coordinate #1 nor coordinate #2.
    bool severalCoordinates = row1 < usedRows();
    if( severalCoordinates ) {
        severalCoordinates = false;
        QVariant::Type testType = QVariant::Invalid;
        const uint r2 = (UINT_MAX == row2)
                  ? usedRows()
                  : QMIN( row2+1, usedRows() );
        QVariant value1;
        QVariant value2;
        for ( uint row = row1; row < r2; ++row ){
            for ( uint col = 0; col < usedCols(); ++col ){
                if( cellCoords( row, col, value1, value2 ) ){
                    if( QVariant::Invalid != value2.type() ){
                        if( (QVariant::Invalid != testType) &&
                            (value2.type()     != testType) ){
                            severalCoordinates = false;
                            break;
                        }else{
                            testType = value2.type();
                            if( NULL != type2Ref )
                                *type2Ref = testType;
                            severalCoordinates = true;
                        }
                    }else if( QVariant::Invalid != value1.type() ){
                        severalCoordinates = false;
                        break;
                    }
                }
            }
        }
    }
    return severalCoordinates;
}


QVariant::Type KDChartTableDataBase::cellsValueType(
    uint row1,
    uint row2,
    int coordinate ) const
{
    QVariant::Type res = QVariant::Invalid;
    const uint r2 = (UINT_MAX == row2)
              ? usedRows()
              : QMIN( row2+1, usedRows() );
    
    QVariant value;
    for ( uint row = row1; row < r2; ++row )
        for ( uint col = 0; col < usedCols(); ++col )
            if( cellCoord( row, col, value, coordinate ) )
                if( QVariant::Invalid != value.type() )
                    res = value.type();
    return res;
}


QVariant::Type KDChartTableDataBase::cellsValueType(
    int coordinate ) const
{
    return cellsValueType( 0, UINT_MAX, coordinate );
}


double KDChartTableDataBase::maxValue( int coordinate ) const
{
    double maxValue = 0.0;
    bool bStart = true;
    QVariant value;
    double dVal;
    for ( uint row = 0; row < usedRows(); row++ ) {
        for ( uint col = 0; col < usedCols(); col++ ) {
            if( cellCoord( row, col, value, coordinate ) &&
                QVariant::Double == value.type() ) {
                dVal = value.toDouble();
                if( isNormalDouble( dVal ) ){
                    if ( bStart ) {
                        maxValue = dVal;
                        bStart = false;
                    } else
                        maxValue = QMAX( maxValue, dVal );
                }
            }
        }
    }
    return maxValue;
}



double KDChartTableDataBase::minValue( int coordinate, bool bOnlyGTZero ) const
{
    double minValue = 0.0;
    bool bStart = true;
    QVariant value;
    double dVal;
    for ( uint row = 0; row < usedRows(); row++ ) {
        for ( uint col = 0; col < usedCols(); col++ ) {
            if( cellCoord( row, col, value, coordinate ) &&
                QVariant::Double == value.type() ) {
                dVal = value.toDouble();
                if( !bOnlyGTZero || 0.0 < dVal ){
                    if ( bStart ) {
                        minValue = dVal;
                        bStart = false;
                    }else{
                        minValue = QMIN( minValue, dVal );
                    }
                }
            }
        }
    }
    return minValue;
}


QDateTime KDChartTableDataBase::maxDtValue( int coordinate ) const
{
    QDateTime maxValue = QDateTime( QDate(1970,1,1) );
    bool bStart = true;
    QVariant value;
    QDateTime dtVal;
    for ( uint row = 0; row < usedRows(); row++ ) {
        for ( uint col = 0; col < usedCols(); col++ ) {
            if( cellCoord( row, col, value, coordinate ) &&
                QVariant::DateTime == value.type() ) {
                dtVal = value.toDateTime();
                if ( bStart ) {
                    maxValue = dtVal;
                    bStart = false;
                } else
                    maxValue = QMAX(maxValue, dtVal);
            }
        }
    }
    return maxValue;
}



QDateTime KDChartTableDataBase::minDtValue( int coordinate ) const
{
    QDateTime minValue = QDateTime( QDate(1970,1,1) );
    bool bStart = true;
    QVariant value;
    QDateTime dtVal;
    for ( uint row = 0; row < usedRows(); row++ ) {
        for ( uint col = 0; col < usedCols(); col++ ) {
            if( cellCoord( row, col, value, coordinate ) &&
                QVariant::DateTime == value.type() ) {
                dtVal = value.toDateTime();
                if ( bStart ) {
                    minValue = dtVal;
                    bStart = false;
                } else
                    minValue = QMIN(minValue, dtVal);
            }
        }
    }
    return minValue;
}


double KDChartTableDataBase::maxColSum( int coordinate ) const
{
    double maxValue = 0.0;
    bool bStart = true;
    for ( uint col = 0; col < usedCols(); col++ ) {
        double colValue = colSum( col, coordinate );
        if ( bStart ) {
            maxValue = colValue;
            bStart = false;
        } else
            maxValue = QMAX( maxValue, colValue );
    }
    return maxValue;
}


double KDChartTableDataBase::minColSum( int coordinate ) const
{
    double minValue = 0.0;
    bool bStart = true;
    for ( uint col = 0; col < usedCols(); col++ ) {
        double colValue = colSum( col, coordinate );
        if ( bStart ) {
            minValue = colValue;
            bStart = false;
        } else
            minValue = QMIN( minValue, colValue );
    }

    return minValue;
}


double KDChartTableDataBase::maxColSum( uint row, uint row2, int coordinate ) const
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
            QVariant value;
            double dVal;
            for ( uint row = a; row <= z; row++ ) {
                if( cellCoord( row, col, value, coordinate ) &&
                    QVariant::Double == value.type() ) {
                    dVal = value.toDouble();
                    if( isNormalDouble( dVal ) )
                        valueValue += dVal;
                }
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


double KDChartTableDataBase::minColSum( uint row, uint row2, int coordinate ) const
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
            QVariant value;
            double dVal;
            for ( uint row = a; row <= z; row++ ) {
                if( cellCoord( row, col, value, coordinate ) &&
                    QVariant::Double == value.type() ) {
                    dVal = value.toDouble();
                    if( isNormalDouble( dVal ) )
                        valueValue += dVal;
                }
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


double KDChartTableDataBase::colSum( uint col, int coordinate ) const
{
    double sum = 0.0;
    QVariant value;
    double dVal;
    for ( uint row = 0; row < usedRows(); row++ ) {
        if( cellCoord( row, col, value, coordinate ) &&
            QVariant::Double == value.type() ) {
            dVal = value.toDouble();
            if( isNormalDouble( dVal ) )
                sum += dVal;
        }
    }

    return sum;
}


double KDChartTableDataBase::colAbsSum( uint col, int coordinate ) const
{
    double sum = 0.0;
    QVariant value;
    double dVal;
    for ( uint row = 0; row < usedRows(); row++ ) {
        if( cellCoord( row, col, value, coordinate ) &&
            QVariant::Double == value.type() ) {
            dVal = value.toDouble();
            if( isNormalDouble( dVal ) )
                sum += fabs( dVal );
        }
    }

    return sum;
}


double KDChartTableDataBase::maxRowSum( int coordinate ) const
{
    double maxValue = 0.0;
    bool bStart = true;
    for ( uint row = 0; row < usedRows(); row++ ) {
        double rowValue = rowSum( row, coordinate );
        if ( bStart ) {
            maxValue = rowValue;
            bStart = false;
        } else
            maxValue = QMAX( maxValue, rowValue );
    }
    return maxValue;
}


double KDChartTableDataBase::minRowSum( int coordinate ) const
{
    double minValue = 0.0;
    bool bStart = true;
    for ( uint row = 0; row < usedRows(); row++ ) {
        double rowValue = rowSum( row, coordinate );
        if ( bStart ) {
            minValue = rowValue;
            bStart = false;
        } else
            minValue = QMIN( minValue, rowValue );
    }

    return minValue;
}


double KDChartTableDataBase::rowSum( uint row, int coordinate ) const
{
    double sum = 0.0;
    QVariant value;
    double dVal;
    for ( uint col = 0; col < usedCols(); col++ ) {
        if( cellCoord( row, col, value, coordinate ) &&
            QVariant::Double == value.type() ) {
            dVal = value.toDouble();
            if( isNormalDouble( dVal ) )
                sum += dVal;
        }
    }
    return sum;
}


double KDChartTableDataBase::rowAbsSum( uint row, int coordinate ) const
{
    double sum = 0.0;
    QVariant value;
    double dVal;
    for ( uint col = 0; col < usedCols(); col++ ) {
        if( cellCoord( row, col, value, coordinate ) &&
            QVariant::Double == value.type() ) {
            dVal = value.toDouble();
            if( isNormalDouble( dVal ) )
                sum += fabs( dVal );
        }
    }
    return sum;
}


double KDChartTableDataBase::maxInColumn( uint col, int coordinate ) const
{
    double maxValue = 0.0;
    bool bStart = true;
    QVariant value;
    double dVal;
    for ( uint row = 0; row < usedRows(); row++ ) {
        if( cellCoord( row, col, value, coordinate ) &&
            QVariant::Double == value.type() ) {
            dVal = value.toDouble();
            if( isNormalDouble( dVal ) ){
                if ( bStart ) {
                    maxValue = dVal;
                    bStart = false;
                } else
                    maxValue = QMAX( maxValue, dVal );
            }
        }
    }

    return maxValue;
}


double KDChartTableDataBase::minInColumn( uint col, int coordinate ) const
{
    double minValue = 0.0;
    bool bStart = true;
    QVariant value;
    double dVal;
    for ( uint row = 0; row < usedRows(); row++ ) {
        if( cellCoord( row, col, value, coordinate ) &&
            QVariant::Double == value.type() ) {
            dVal = value.toDouble();
            if( isNormalDouble( dVal ) ){
                if ( bStart ) {
                    minValue = dVal;
                    bStart = false;
                } else
                    minValue = QMIN( minValue, dVal );
            }
        }
    }

    return minValue;
}


double KDChartTableDataBase::maxInRow( uint row, int coordinate ) const
{
    double maxValue = DBL_MIN;
    bool bStart = true;
    QVariant value;
    double dVal;
    if ( UINT_MAX > row ) {
        for ( uint col = 0; col < usedCols(); col++ ) {
            if( cellCoord( row, col, value, coordinate ) &&
                QVariant::Double == value.type() ) {
                dVal = value.toDouble();
                if( isNormalDouble( dVal ) ){
                    if ( bStart ) {
                        maxValue = dVal;
                        bStart = false;
                    } else
                        maxValue = QMAX( maxValue, dVal );
                }
            }
        }
    }
    return maxValue;
}


double KDChartTableDataBase::minInRow( uint row, int coordinate ) const
{
    double minValue = DBL_MAX;
    bool bStart = true;
    QVariant value;
    double dVal;
    if ( UINT_MAX > row ) {
        for ( uint col = 0; col < usedCols(); col++ ) {
            if( cellCoord( row, col, value, coordinate ) &&
                QVariant::Double == value.type() ) {
                dVal = value.toDouble();
                if( isNormalDouble( dVal ) ){
                    if ( bStart ) {
                        minValue = dVal;
                        bStart = false;
                    } else
                        minValue = QMIN( minValue, dVal );
                }
            }
        }
    }
    return minValue;
}


double KDChartTableDataBase::maxInRows( uint row, uint row2, int coordinate ) const
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
            QVariant value;
            double dVal;
            for ( uint col = 0; col < usedCols(); ++col ) {
                if( cellCoord( row, col, value, coordinate ) &&
                    QVariant::Double == value.type() ) {
                    dVal = value.toDouble();
                    if( isNormalDouble( dVal ) ){
                        if ( bStart ) {
                            maxValue = dVal;
                            bStart = false;
                        } else
                            maxValue = QMAX( maxValue, dVal );
                    }
                }
            }
        }
    }
    return maxValue;
}


double KDChartTableDataBase::minInRows( uint row, uint row2, int coordinate, bool bOnlyGTZero ) const
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
            QVariant value;
            double dVal;
            for ( uint col = 0; col < usedCols(); ++col ) {
                if( cellCoord( row, col, value, coordinate ) &&
                    QVariant::Double == value.type() ) {
                    dVal = value.toDouble();
                    if( isNormalDouble( dVal ) ){
                        if( !bOnlyGTZero || 0.0 < dVal ){
                            if ( bStart ) {
                                minValue = dVal;
                                bStart = false;
                            }else{
                                minValue = QMIN( minValue, dVal );
                            }
                        }
                    }
                }
            }
        }
    }
    return minValue;
}


QDateTime KDChartTableDataBase::maxDtInRows( uint row, uint row2,
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
            QVariant value;
            QDateTime dtVal;
            for ( uint col = 0; col < usedCols(); ++col ) {
                if( cellCoord( row, col, value, coordinate ) &&
                    QVariant::DateTime == value.type() ) {
                    dtVal = value.toDateTime();
                    if ( bStart ) {
                        maxValue = dtVal;
                        bStart = false;
                    } else
                        maxValue = QMAX( maxValue, dtVal );
                }
            }
        }
    }
    return maxValue;
}


QDateTime KDChartTableDataBase::minDtInRows( uint row, uint row2,
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
            QVariant value;
            QDateTime dtVal;
            for ( uint col = 0; col < usedCols(); ++col ) {
                if( cellCoord( row, col, value, coordinate ) &&
                    QVariant::DateTime == value.type() ) {
                    dtVal = value.toDateTime();
                    if ( bStart ) {
                        minValue = dtVal;
                        bStart = false;
                    } else
                        minValue = QMIN( minValue, dtVal );
                }
            }
        }
    }
    return minValue;
}


uint KDChartTableDataBase::lastPositiveCellInColumn( uint col, int coordinate ) const
{
    uint ret = UINT_MAX;
    QVariant value;
    double dVal;
    for ( uint row = 0; row < usedRows(); row++ ) {
        if( cellCoord( row, col, value, coordinate ) &&
            QVariant::Double == value.type() ) {
            dVal = value.toDouble();
            if( isNormalDouble( dVal ) && 0 < dVal )
                ret = row;
        }
    }
    return ret;
}


void KDChartTableDataBase::importFromQTable( QTable* table )
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
                    setCell( row, col, value );
                } else {
                    // no double, but at least a string
                    setCell( row, col, cellContents );
                }
            } // don't do anything if no contents
        }
    setSorted( false );
}


void KDChartTableDataBase::setSorted(bool sorted)
{
    _sorted = sorted;
}
bool KDChartTableDataBase::sorted() const
{
    return _sorted;
}

#include "KDChartTableBase.moc"
