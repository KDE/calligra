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
   Library General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this library; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __KDCHARTPIEPAINTER_H__
#define __KDCHARTPIEPAINTER_H__

#include <KDChartPainter.h>
#include <KDChartTable.h>

class KDChartParams;

class KDChartPiePainter : public KDChartPainter
{
    friend class KDChartPainter;
protected:
    KDChartPiePainter( KDChartParams* params );
    virtual ~KDChartPiePainter();

    virtual void paintData( QPainter* painter, KDChartTableData* data,
                            bool paint2nd,
                            KDChartDataRegionList* regions = 0 );
    virtual void drawOnePie( QPainter* painter, KDChartTableData* data,
                             uint dataset, uint pie, uint threeDPieHeight,
                             KDChartDataRegionList* regions = 0 );
    virtual void draw3DEffect( QPainter* painter, const QRect& drawPosition,
                               uint dataset, uint pie, uint threeDPieHeight,
                               bool explode,
                               KDChartDataRegionList* regions = 0 );
    void drawStraightEffectSegment( QPainter* painter, const QRect& rect,
                                    uint dataset, uint pie, int
                                    threeDHeight, int angle,
                                    KDChartDataRegionList* regions = 0 );
    void drawArcEffectSegment( QPainter* painter, const QRect& rect,
                               uint dataset, uint pie,
                               int threeDHeight, int startAngle,
                               int endAngle,
                               KDChartDataRegionList* regions = 0 );

    virtual QString fallbackLegendText( uint dataset ) const;
    virtual uint numLegendFallbackTexts( KDChartTableData* data ) const;

    uint findPieAt( int angle );
    uint findLeftPie( uint pie );
    uint findRightPie( uint pie );
    QPoint pointOnCircle( const QRect& rect, int angle );

    QArray < int > _startAngles;
    QArray < int > _angleLens;

    QRect _position;
    uint _size;
    uint _numValues; // PENDING(kalle) Move to base class
}
;

#endif
