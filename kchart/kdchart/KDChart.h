/* -*- Mode: C++ -*-

  $Id$

  KDChart - a multi-platform charting engine

  Copyright (C) 2001 by Klarälvdalens Datakonsult AB

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this library; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __KDCHART_H__
#define __KDCHART_H__

#include <KDChartTable.h>
#include <KDChartUnknownTypeException.h>
#include <KDChartParams.h>
#include <KDChartDataRegion.h>

class KDChartPainter;
class QPainter;

class KDChart
{
    friend class KDChartCleanup;
public:
    static void paint( QPainter*, KDChartParams*, KDChartTableData*,
                       KDChartDataRegionList* regions = 0 )
    #ifdef USE_EXCEPTIONS
    throw( KDChartUnknownTypeException )
    #endif
    ;

private:
    KDChart(); // prevent instantiations

    static KDChartPainter* cpainter;
    static KDChartPainter* cpainter2;
    static KDChartParams::ChartType cpainterType;
    static KDChartParams::ChartType cpainterType2;
};

#endif
