/* -*- Mode: C++ -*-
   $Id$
   KDChart - a multi-platform charting engine
*/

/****************************************************************************
** Copyright (C) 2001-2002 Klarälvdalens Datakonsult AB.  All rights reserved.
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
** See http://www.klaralvdalens-datakonsult.se/Public/products/ for
**   information about KDChart Commercial License Agreements.
**
** Contact info@klaralvdalens-datakonsult.se if any conditions of this
** licensing are not clear to you.
**
**********************************************************************/
#ifndef __KDCHARTTABLEINTERFACE_H__
#define __KDCHARTTABLEINTERFACE_H__

#include <KDChartData.h>

#ifdef __WINDOWS__
#include <float.h>
#include <limits.h>
#include <math.h>
#define MAXDOUBLE DBL_MAX
#elif defined __APPLE__
#include <float.h>
#define MAXDOUBLE DBL_MAX
#else
#if defined(__FreeBSD__)
#include <float.h>
#define MAXDOUBLE DBL_MAX
#else
#include <values.h>
#endif
#include <limits.h>
#include <math.h>
#endif

#if defined SUN7 || ( defined HP11_aCC && defined HP1100 )
#define std
#endif



class KDChartTableDataBase
{
public:
    virtual ~KDChartTableDataBase() {}
    double maxValue() const;
    double minValue() const;
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
    double maxInRows( uint row, uint row2 ) const;
    double minInRows( uint row, uint row2 ) const;
    uint lastPositiveCellInColumn( uint col ) const;
    void importFromQTable( QTable* table );
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
};


inline double KDChartTableDataBase::maxValue() const
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



inline double KDChartTableDataBase::minValue() const
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


inline double KDChartTableDataBase::minColSum( uint row, uint row2 ) const
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


inline double KDChartTableDataBase::colSum( uint col ) const
{
    double sum = 0.0;
    for ( uint row = 0; row < usedRows(); row++ ) {
        KDChartData d = cell( row, col );
        if ( d.isDouble() )
            sum += d.doubleValue();
    }

    return sum;
}


inline double KDChartTableDataBase::colAbsSum( uint col ) const
{
    double sum = 0.0;
    for ( uint row = 0; row < usedRows(); row++ ) {
        KDChartData d = cell( row, col );
        if ( d.isDouble() )
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
        KDChartData d = cell( row, col );
        if ( d.isDouble() )
            sum += d.doubleValue();
    }
    return sum;
}


inline double KDChartTableDataBase::rowAbsSum( uint row ) const
{
    double sum = 0.0;
    for ( uint col = 0; col < usedCols(); col++ ) {
        KDChartData d = cell( row, col );
        if ( d.isDouble() )
            sum += fabs( d.doubleValue() );
    }
    return sum;
}


inline double KDChartTableDataBase::maxInColumn( uint col ) const
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


inline double KDChartTableDataBase::minInColumn( uint col ) const
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


inline double KDChartTableDataBase::maxInRow( uint row ) const
{
    double maxValue = KDCHART_MINDOUBLE;
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


inline double KDChartTableDataBase::minInRow( uint row ) const
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


inline double KDChartTableDataBase::maxInRows( uint row, uint row2 ) const
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


inline double KDChartTableDataBase::minInRows( uint row, uint row2 ) const
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


inline uint KDChartTableDataBase::lastPositiveCellInColumn( uint col ) const
{
    uint ret = UINT_MAX;
    for ( uint row = 0; row < usedRows(); row++ ) {
        KDChartData d = cell( row, col );
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
}


#endif
