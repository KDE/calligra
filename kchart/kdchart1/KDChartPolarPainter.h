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
#ifndef __KDCHARTPOLARPAINTER_H__
#define __KDCHARTPOLARPAINTER_H__

#include "KDChartPainter.h"
#include <KDChartTable.h>
#include <KDChartParams.h>

class KDChartPolarPainter : public KDChartPainter
{
    friend class KDChartPainter;

    protected:
    KDChartPolarPainter( KDChartParams* params );
    virtual ~KDChartPolarPainter();

    virtual void paintData( QPainter* painter, 
            KDChartTableDataBase* data,
            bool paint2nd,
            KDChartDataRegionList* regions = 0 );
/*    void drawMarker( QPainter* painter,
            KDChartParams::PolarMarkerStyle style,
            const QColor& color, const QPoint& p,
            uint dataset, uint value, uint chart,
            double minSizeP1000,
            QRegion & region );*/
    virtual QString fallbackLegendText( uint dataset ) const;
    virtual uint numLegendFallbackTexts( KDChartTableDataBase* data ) const;

    private:
    QPoint polarToXY( int radius, int angle );
    void paintAxisLabels( QPainter* painter,
            QStringList& labelTexts,
            double radiusDelta,
            const QRect& position,
            const QPoint& center,
            double radiusPPU,
            double& currentRadiusPPU,
            const KDChartAxisParams & paraCircular,
            double minSizeP1000 );
    void paintCircularAxisLabel( QPainter* painter,
            bool rotate,
            int txtAngle,
            QPoint center,
            double currentRadiusPPU,
            const QString& txt,
            int align,
            int step );
};

#endif
