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
#ifndef __KDCHARTSERIESCOLLECTION_H__
#define __KDCHARTSERIESCOLLECTION_H__

// to avoid problems in TableBase - it doesn't include this - bug!
#include <qtable.h>

#include "KDChartTableBase.h"
#include "KDChartBaseSeries.h"
#if COMPAT_QT_VERSION >= 0x030000
#include <qvaluevector.h>
#else
#include <qarray.h>
#endif
class KDChartParams;
#include "KDChartAxisParams.h"

// takes ownership of any series passed to it, and will delete
// them when it is deleted.
class KDCHART_EXPORT KDChartSeriesCollection : public KDChartTableDataBase,
#if COMPAT_QT_VERSION >= 0x030000
    public QValueVector<KDChartBaseSeries *>
#else
    public QArray<KDChartBaseSeries *>
#endif
{
    // Standard KDChartTableDataBase interface
    public:
        KDChartSeriesCollection( KDChartParams *params );
        virtual ~KDChartSeriesCollection();

        virtual uint rows() const;
        virtual uint cols() const;
        virtual void setUsedRows( uint _rows );
        virtual uint usedRows() const;
        virtual void setUsedCols( uint _cols );
        virtual uint usedCols() const;
        virtual void setCell( uint _row, uint _col,
                const KDChartData& _element );
        virtual const KDChartData& cell( uint _row, uint _col ) const;
        virtual void expand( uint _rows, uint _cols );


        // Methods we need KHZ to modify in HIS code so we can use our code
        // needs to become virtual!
        virtual double minValue( int coordinate=1 ) const;
        virtual double maxValue( int coordinate=1 ) const;


        // extra functions for our use
        virtual void setLegendText( KDChartBaseSeries *series, QString text );
        virtual QString legendText( KDChartBaseSeries *series );

        virtual void setYaxis( KDChartBaseSeries *series,
                KDChartAxisParams::AxisPos axis );// PENDING(blackie) possible enum problem
        virtual KDChartAxisParams::AxisPos yAxis( KDChartBaseSeries *series );// PENDING(blackie) possible enum problem

        virtual unsigned int indexOf( KDChartBaseSeries *series );

    protected:
        KDChartData _blank;
        KDChartParams *_params;
};


#endif
