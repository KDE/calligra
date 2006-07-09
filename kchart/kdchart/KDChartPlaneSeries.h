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
#ifndef __KDCHARTPLANESERIES_H__
#define __KDCHARTPLANESERIES_H__


#include "KDChartBaseSeries.h"


class KDCHART_EXPORT KDChartPlaneSeries : public KDChartBaseSeries
{
    public:
        KDChartPlaneSeries( bool isX = false, double location = 0 );
        virtual ~KDChartPlaneSeries();

        virtual uint rows() const;
        virtual const KDChartData& cell( uint row ) const;
        virtual void setCell( uint row, const KDChartData& element);
        virtual void expand( uint rows );


        // methods modelled on the TableBase methods, but these
        // inherit from BaseSeries.
        virtual double maxValue( int coordinate, bool &ok ) const;
        virtual double minValue( int coordinate, bool &ok ) const;


        // NOW for our special API.
        virtual bool isXAxis() const;
        virtual double location() const;

        virtual void setXAxis( bool isX );  // if false, its a y axis plane
        virtual void setLocation( double location );

    protected:
        bool _isX;
        double _location;
        KDChartData _start, _stop;
        virtual void update();
};


#endif
