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
#ifndef __KDCHART_H__
#define __KDCHART_H__

#include <KDChartGlobal.h>
#include <KDChartTable.h>
#include <KDChartUnknownTypeException.h>
#include <KDChartParams.h>
#include <KDChartDataRegion.h>

class KDChartPainter;
class QPainter;

class KDChart
{
    friend class KDChartCleanup;
  friend void cleanupPainter();

public:
    static void paint( QPainter*, KDChartParams*, KDChartTableDataBase*,
                       KDChartDataRegionList* regions = 0,
                       const QRect* rect = 0 )
    #ifdef USE_EXCEPTIONS
    throw( KDChartUnknownTypeException )
    #endif
    ;

private:
    KDChart(); // prevent instantiations

    static KDChartParams* oldParams;
    static KDChartPainter* cpainter;
    static KDChartPainter* cpainter2;
    static KDChartParams::ChartType cpainterType;
    static KDChartParams::ChartType cpainterType2;
};

#endif
