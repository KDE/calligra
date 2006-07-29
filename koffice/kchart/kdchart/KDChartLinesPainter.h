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
#ifndef __KDCHARTLINESPAINTER_H__
#define __KDCHARTLINESPAINTER_H__

#include "KDChartAxesPainter.h"
#include <KDChartTable.h>
#include <KDChartParams.h>

class QSize;

class KDChartLinesPainter : public KDChartAxesPainter
{
    friend class KDChartPainter;

    protected:
    KDChartLinesPainter( KDChartParams* params );
    virtual ~KDChartLinesPainter();

    virtual void paintData( QPainter* painter, 
            KDChartTableDataBase* data,
            bool paint2nd,
            KDChartDataRegionList* regions = 0 );
    virtual bool isNormalMode() const;
    virtual int clipShiftUp( bool, double ) const;
    virtual void specificPaintData( QPainter* painter,
            const QRect& ourClipRect,
            KDChartTableDataBase* data,
            KDChartDataRegionList* regions,
            const KDChartAxisParams* axisPara,
            bool bNormalMode,
            uint chart,
            double logWidth,
            double areaWidthP1000,
            double logHeight,
            double axisYOffset,
            double minColumnValue,
            double maxColumnValue,
            double columnValueDistance,
            uint chartDatasetStart,
            uint chartDatasetEnd,
            uint datasetStart,
            uint datasetEnd );

    void paintDataInternal( QPainter* painter,
            KDChartTableDataBase* data,
            bool centerThePoints,
            bool drawMarkers,
            bool isArea,
            bool paint2nd,
            KDChartDataRegionList* regions = 0 );
    QPoint project( int x, int y, int z );
    private:
    KDChartParams::ChartType mChartType;
    bool mCenterThePoints;
    bool mDrawMarkers;
    bool mIsArea;
};

#endif
