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

#include "KDChartVectorSeries.h"
#include "KDChartSeriesCollection.h"


KDChartVectorSeries::~KDChartVectorSeries()
{
}



uint KDChartVectorSeries::rows() const
{
    return size();
}

const KDChartData& KDChartVectorSeries::cell( uint row ) const
{
    Q_ASSERT( row < size() );
    // make it compile on windows using qt232
    return this->at(row);
}

void KDChartVectorSeries::setCell( uint row, const KDChartData& element)
{
    Q_ASSERT( row < size() );
    // make it compile on windows using qt232
    this->at(row) = element;
}

void KDChartVectorSeries::expand( uint rows )
{
    resize(rows);
}


// we return !ok if its on the infinite axis
double KDChartVectorSeries::maxValue( int coordinate, bool &ok ) const
{
    double maxValue = 0.0;
    bool bStart = true;

#if COMPAT_QT_VERSION >= 0x030000
    KDChartVectorSeries::const_iterator i;
#else
    KDChartVectorSeries::ConstIterator i;
#endif

    for ( i = begin(); i != end(); i ++ )
    {
        const KDChartData& d = *i;
        if ( d.isDouble( coordinate ) )
        {
            if ( bStart )
            {
                maxValue = d.doubleValue( coordinate );
                bStart = false;
            }
            else
                maxValue = QMAX( maxValue, d.doubleValue( coordinate ) );
        }
    }

    ok = !bStart;
    return maxValue;
}



double KDChartVectorSeries::minValue( int coordinate, bool &ok ) const
{
    double minValue = 0.0;
    bool bStart = true;

#if COMPAT_QT_VERSION >= 0x030000
    KDChartVectorSeries::const_iterator i;
#else
    KDChartVectorSeries::ConstIterator i;
#endif

    for ( i = begin(); i != end(); i ++ )
    {
        const KDChartData& d = *i;
        if ( d.isDouble( coordinate ) ) 
        {
            if ( bStart ) 
            {
                minValue = d.doubleValue( coordinate );
                bStart = false;
            }
            else
                minValue = QMIN( minValue, d.doubleValue( coordinate ) );
        }
    }

    ok = !bStart;
    return minValue;
}
