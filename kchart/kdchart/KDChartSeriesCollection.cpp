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


#include "KDChartSeriesCollection.h"
#include "KDChartParams.h"


KDChartSeriesCollection::KDChartSeriesCollection( KDChartParams *params )
{
    _params = params;
}


KDChartSeriesCollection::~KDChartSeriesCollection()
{
    for ( int i = 0; i < static_cast < int > ( (*this).size() ); i ++ )
        delete (*this)[i];
}


// both rows return the same amount
uint KDChartSeriesCollection::rows() const
{
    return usedRows();
}
uint KDChartSeriesCollection::usedRows() const
{
    return (*this).size();
}


uint KDChartSeriesCollection::cols() const
{
    return usedCols();
}


uint KDChartSeriesCollection::usedCols() const
{
    uint result = 0;

    // find the maximum number of rows in all the visible series

    for ( int i = 0; i < static_cast < int > ( (*this).size() ); i ++ )
        if ( result < (*this)[i]->rows())
            result = (*this)[i]->rows();

    return result;
}

// Don't really know how to handle these yet, I have no need yet.
// It appears to be only used to load QTables.
// ASSERT if used.
void KDChartSeriesCollection::setUsedRows( uint )
{
    Q_ASSERT(0);
}
void KDChartSeriesCollection::setUsedCols( uint )
{
    Q_ASSERT(0);
}


void KDChartSeriesCollection::setCell( uint row, uint col,
        const KDChartData& element )
{
    Q_ASSERT( row < (*this).size() );
    this->at(row)->setCell(col, element);
}

const KDChartData& KDChartSeriesCollection::cell( uint row, uint col ) const
{
    Q_ASSERT( row < (*this).size() );
    // Put this back in if/when KHZ adds the performance improvements
    // re usedCols( uint row )   Q_ASSERT( col < (*this)[row]->rows() );

    if ( col < this->at(row)->rows() )
        return this->at(row)->cell(col);
    else
        return _blank;
}

void KDChartSeriesCollection::expand( uint cols, uint rows )
{
    // first expand ourselves - cols-wise
    (*this).resize(rows);

    // now expand our babies
    for ( int i = 0; i < static_cast < int > ( (*this).size() ); i ++ )
        (*this)[i]->expand(cols);
}



// coordinate is the first or second value in a data point
double KDChartSeriesCollection::maxValue( int coordinate ) const
{
    // IF there are no series to read from, then just return zero.
    // KHZ: perhaps this should assert?

    bool ok;    // the ok is required in case we check a PlaneSeries, which
    // cannot possibly have a min or max on one of the axis.

    double result = 0;      // if no valid min/max, then this is the default
    bool first_max = true;

    // find the first max
#if COMPAT_QT_VERSION >= 0x030000
    QValueVector<KDChartBaseSeries *>::const_iterator i;
#else
    QArray<KDChartBaseSeries *>::ConstIterator i;
#endif
    for ( i = (*this).begin(); i != (*this).end(); i ++ )
    {
        double temp = (*i)->maxValue(coordinate, ok);
        if ( ok && (first_max || temp > result) )
        {
            first_max = false;
            result = temp;
        }
    }

    return result;
}



double KDChartSeriesCollection::minValue( int coordinate ) const
{
    // IF there are no series to read from, then just return zero.
    // KHZ: perhaps this should assert?

    bool ok = false;    // the ok is required in case we check a PlaneSeries, which
    // cannot possibly have a min or max on one of the axis.

    double result = 0;      // if no valid min/max, then this is the default

    // find the first min
#if COMPAT_QT_VERSION >= 0x030000
    QValueVector<KDChartBaseSeries *>::const_iterator i;
#else
    QArray<KDChartBaseSeries *>::ConstIterator i;
#endif
    for ( i = (*this).begin(); !ok && i != (*this).end(); i ++ )
        result = (*i)->minValue(coordinate, ok);

    if ( ok )
        for ( ; i != (*this).end(); i ++ )
        {
            double temp = (*i)->minValue(coordinate, ok);
            if (ok)
                result = QMIN( result, temp );
        }

    return result;
}


unsigned int KDChartSeriesCollection::indexOf( KDChartBaseSeries *series )
{
    unsigned int index = 0;
#if COMPAT_QT_VERSION >= 0x030000
    QValueVector<KDChartBaseSeries *>::const_iterator i;
#else
    QArray<KDChartBaseSeries *>::ConstIterator i;
#endif
    for ( i = (*this).begin(); i != (*this).end(); i ++, index ++ )
        if ( *i == series )
            break;

    // must find it
    Q_ASSERT( index < (*this).size() );

    return index;
}


void KDChartSeriesCollection::setLegendText( KDChartBaseSeries *series, QString text )
{
    _params->setLegendText( indexOf( series ), text );
}



void KDChartSeriesCollection::setYaxis( KDChartBaseSeries *series, 
        KDChartAxisParams::AxisPos axis )
{
    unsigned int index = indexOf( series );
    _params->setAxisDatasets( axis, index, index, 0 );
}




QString KDChartSeriesCollection::legendText( KDChartBaseSeries *series )
{
    return _params->legendText( indexOf(series) );
}


KDChartAxisParams::AxisPos KDChartSeriesCollection::yAxis( KDChartBaseSeries *series )
{
    unsigned int index = indexOf( series );
    unsigned int tempchart = 0; // needed cause for some reason KHZ wants a reference.

    // now we have to look through
    for ( int i = 0; i < KDCHART_MAX_AXES; i ++ )
        if ( _params->axisDatasets( i, index, index, tempchart ) )
            return (KDChartAxisParams::AxisPos) i;

    Q_ASSERT(0);    // should find it
    return (KDChartAxisParams::AxisPos) 0;
}
