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
#ifndef __KDCHARTVECTORSERIES_H__
#define __KDCHARTVECTORSERIES_H__

// A single data series abstracted.
// Is included in a DataSeriesBag.
// Will be a base class for other series objects, such as DataVectorSeries,
// and my DataQuerySeries.
//
// Requirements:
// - Implement a QValueVector interface

#include "KDChartBaseSeries.h"
#if COMPAT_QT_VERSION >= 0x030000
#include <qvaluevector.h>
#else
#include <qarray.h>
#endif
class KDChartSeriesCollection;



class KDCHART_EXPORT KDChartVectorSeries : public KDChartBaseSeries, 
#if COMPAT_QT_VERSION >= 0x030000
    public QValueVector<KDChartData>
#else
    public QArray<KDChartData>
#endif
{
    public:
        virtual ~KDChartVectorSeries();

        virtual uint rows() const;
        virtual const KDChartData& cell( uint row ) const;
        virtual void setCell( uint row, const KDChartData& element);
        virtual void expand( uint rows );

        // methods modelled on the TableBase methods, but these
        // inherit from BaseSeries.
        virtual double maxValue( int coordinate, bool &ok ) const;
        virtual double minValue( int coordinate, bool &ok ) const;
};

#endif
