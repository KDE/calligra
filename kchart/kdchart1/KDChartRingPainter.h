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
#ifndef __KDCHARTRINGPAINTER_H__
#define __KDCHARTRINGPAINTER_H__

#include <KDChartPainter.h>
#include <KDChartTable.h>

class KDChartParams;

class KDChartRingPainter : public KDChartPainter
{
    friend class KDChartPainter;
    protected:
    KDChartRingPainter( KDChartParams* params );
    virtual ~KDChartRingPainter();

    virtual void paintData( QPainter* painter, 
            KDChartTableDataBase* data,
            bool paint2nd,
            KDChartDataRegionList* regions = 0 );
    void drawOneSegment( QPainter* painter,
            uint outerRadius, uint innerRadius,
            double startAngle, double angles,
            uint dataset, uint value, uint chart,
            bool explode,
            KDChartDataRegionList* regions = 0 );

    virtual QString fallbackLegendText( uint dataset ) const;
    virtual uint numLegendFallbackTexts( KDChartTableDataBase* data ) const;

    QRect _position;
    int _size;
    int _numValues; // PENDING(kalle) Move to base class
}
;

#endif
