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
#ifndef __KDCHARTBARPAINTER_H__
#define __KDCHARTBARPAINTER_H__

#include "KDChartAxesPainter.h"
#include <KDChartTable.h>

#include <qnamespace.h>

class KDChartParams;

class KDChartBarPainter : public KDChartAxesPainter, public Qt
{
    friend class KDChartPainter;
    protected:
    KDChartBarPainter( KDChartParams* params );
    virtual ~KDChartBarPainter();

    /*
       virtual void paintData( QPainter* painter,
       KDChartTableDataBase* data,
       bool paint2nd,
       KDChartDataRegionList* regions );
       */
    virtual bool isNormalMode() const;
    virtual int clipShiftUp( bool normalMode, double areaWidthP1000 ) const;
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
    virtual void calculateXFront1_2( bool bNormalMode, bool bIsVeryFirstBar, bool bIsFirstDataset, bool _bThreeDBars,
                                     double xpos, double valueBlockGap, double datasetGap, double frontBarWidth,
                                     int& frontX1, int& frontX2, int& previousFrontX2 );
    private:
    void initMyPainter( QPainter* painter );
    void shiftMyPainter( double dx, double dy );
    void shiftMyPainterBack();
    QPainter* _myPainter;
    double _painterDX;
    double _painterDY;
    bool _bThreeDBars;
  double _areaP1000;
};

#endif
