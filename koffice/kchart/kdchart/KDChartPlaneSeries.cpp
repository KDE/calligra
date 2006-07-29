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


#include "KDChartPlaneSeries.h"

KDChartPlaneSeries::KDChartPlaneSeries( bool isX, double location )
{
    setXAxis(isX);
    setLocation(location);
}

KDChartPlaneSeries::~KDChartPlaneSeries()
{
}


uint KDChartPlaneSeries::rows() const
{
    return 2;
}


const KDChartData& KDChartPlaneSeries::cell( uint row ) const
{
    switch (row)
    {
        case 0: return _start;
        case 1: return _stop;
        default: Q_ASSERT(0);
                 return _start;  // won't get here
    }
}

void KDChartPlaneSeries::setCell( uint row, const KDChartData& element)
{
    Q_ASSERT(0);    // not possible
    // avoid compiler warnings
    row = (uint)element.doubleValue();
}

void KDChartPlaneSeries::expand( uint rows )
{
    Q_ASSERT(0);    // not possible
    // avoid compiler warnings
    rows = 0;
}



// NOW for our special API.
bool KDChartPlaneSeries::isXAxis() const
{
    return _isX;
}

double KDChartPlaneSeries::location() const
{
    return _location;
}

void KDChartPlaneSeries::setXAxis( bool isX )
{
    _isX = isX;
    update();
}

void KDChartPlaneSeries::setLocation( double location )
{
    _location = location;
    update();
}

// this is the magic part of the class.
// draw a line from DBL_MIN to DBL_MAX.
void KDChartPlaneSeries::update()
{
    if ( _isX )
    {
        _start = KDChartData( DBL_MIN, _location );
        _stop = KDChartData( DBL_MAX, _location );
    }
    else
    {
        _start = KDChartData( _location, DBL_MIN );
        _stop = KDChartData( _location, DBL_MAX );
    }
}



// we return !ok if its on the infinite axis
double KDChartPlaneSeries::maxValue( int coordinate, bool &ok ) const
{
    // coordinate==0 is the x value...
    // is not ok
    if ( _isX == (coordinate==0) )
    {
        ok = false;
        return 0;
    }

    ok = true;
    return _location;
}



double KDChartPlaneSeries::minValue( int coordinate, bool &ok ) const
{
    return maxValue(coordinate,ok);
}
