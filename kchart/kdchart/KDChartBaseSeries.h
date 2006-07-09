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
#ifndef __KDCHARTBASESERIES_H__
#define __KDCHARTBASESERIES_H__

// A single data series abstracted.
// Is included in a DataSeriesBag.
// Will be a base class for other series objects, such as DataVectorSeries,
// and DataQuerySeries.
//
// Requirements:
// - Able to handle its own parameters, colours, legend texts, etc.
// - Able to hide/show itself.
// - Almost completely abstract, so we can inherit it from other classes.
//   Implement things like hide and show here tho.

#include "KDChartDataIntern.h"

class KDCHART_EXPORT KDChartBaseSeries
{
    public:
        virtual uint rows() const = 0;
        virtual const KDChartData& cell( uint row ) const = 0;
        virtual void setCell( uint row, const KDChartData& element) = 0;
        virtual void expand( uint rows ) = 0;

        // methods modelled on the TableBase methods.
        virtual double maxValue( int coordinate, bool &ok ) const = 0;
        virtual double minValue( int coordinate, bool &ok ) const = 0;
};


#endif
