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
#ifndef __KDDRAWTEXT_H__
#define __KDDRAWTEXT_H__

#include <qpainter.h>
#ifdef linux
#include <values.h>
#endif
#include "KDChartGlobal.h"


struct KDDrawTextRegionAndTrueRect
{
    QRegion region;
    QPoint pos;
    int x,y,width,height;
};


class KDDrawText
{
    public:
        static void drawRotatedText( QPainter* painter,
                float  degrees,
                QPoint anchor,
                const QString& text,
                const QFont* font = 0,
                int align = Qt::AlignLeft | Qt::AlignTop,
                // useful for debugging of your layout:
                bool showAnchor    = false,
                const QFontMetrics* fontMet = 0,
                bool noFirstrotate = false,
                bool noBackrotate  = false,
                KDDrawTextRegionAndTrueRect* infos = 0,
                bool optimizeOutputForScreen = false );

        static KDDrawTextRegionAndTrueRect measureRotatedText(
                QPainter* painter,
                float  degrees,
                QPoint anchor,
                const QString& text,
                const QFont* font,
                int align,
                const QFontMetrics* fontMet,
                bool noFirstrotate,
                bool noBackrotate,
                int addPercentOfHeightToRegion );

    private:
        static void drawRotatedTxt( QPainter* painter,
                bool optimizeOutputForScreen,
                float  degrees,
                QPoint anchor,
                const QString& text,
                const QFont* font  = 0,
                int align = Qt::AlignLeft | Qt::AlignTop,
                // usefull for debugging of your layout:
                bool showAnchor    = false,
                // speed-up parameters
                // used internally by drawCircleText()
                // to avoid duplicate calculation
                int txtWidth       = INT_MAX,
                int txtHeight      = INT_MAX,
                const QFontMetrics* fontMet = 0,
                // additional speed-up parameters used by KDChart
                bool calculateOnly = false,
                bool doNotCalculate= false,
                bool noFirstrotate = false,
                bool noBackrotate  = false,
                KDDrawTextRegionAndTrueRect* infos = 0,
                int addPercentOfHeightToRegion = 0 );

};

#endif
