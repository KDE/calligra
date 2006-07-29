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
#ifndef __KDCHARTDATAREGION__
#define __KDCHARTDATAREGION__

#include <qregion.h>
#include <qpointarray.h>

#include "KDChartGlobal.h"

/*!
  \internal
  */
struct KDCHART_EXPORT KDChartDataRegion
{
    KDChartDataRegion()
    {
        init();
    }

    KDChartDataRegion( QRegion region, uint r, uint c, uint ch )
    {
        init();
        pRegion = new QRegion( region );
        row     = r;
        col     = c;
        chart   = ch;
    }

    KDChartDataRegion( uint r, uint c, uint ch, QPointArray array )
    {
        init();
        pArray = new QPointArray( array );
        row     = r;
        col     = c;
        chart   = ch;
    }

    KDChartDataRegion( uint r, uint c, uint ch, QRect rect )
    {
        init();
        pRect = new QRect( rect );
        row   = r;
        col   = c;
        chart = ch;
    }

    ~KDChartDataRegion()
    {
        //qDebug ("~KDChartDataRegion");
        if( pRegion )
            delete pRegion;
        if( pArray )
            delete pArray;
        if( pRect )
            delete pRect;
        if( pTextRegion )
            delete pTextRegion;
    }

    QRegion region() const
    {
        if( pRegion )
            return *pRegion;
        if( pArray )
            return QRegion( *pArray );
        if( pRect )
            return QRegion( *pRect );
        return QRegion();
    }

    QRect rect() const
    {
        if( pRegion )
            return pRegion->boundingRect();
        if( pArray )
            return pArray->boundingRect();
        if( pRect )
            return *pRect;
        return QRect();
    }

    void init()
    {
        pRegion     = 0;
        pArray      = 0;
        pRect       = 0;
        pTextRegion = 0;
        row    = 0;
        col    = 0;
        chart  = 0;
        negative = false; // default value (useful if value is a string)
        points.resize( 9 );
        startAngle = 1440;
        angleLen   =    1;
    }

    QRegion*     pRegion;
    QPointArray* pArray;
    QRect*       pRect;
    QRegion*     pTextRegion;  // for the data values text

    // For rectangular data representation  (bar, line, area, point, ...)
    // we use the above declared 'pRect'.
    // For curved data representations (pie slice, ring segment, ...)
    // we store the following additional anchor information:

    // store 9 elements: one for each value of KDChartEnums::PositionFlag
    QPointArray points;

    int startAngle; // Note: 5760 makes a full circle, 2880 is left 'corner'.
    int angleLen;

    uint row;
    uint col;
    // members needed for calculation of data values texts
    uint chart;
    QString text;        // the data values text
    bool    negative;    // stores whether the data value is less than zero
};


/**
  \class KDChartDataRegionList KDChartDataRegion.h

  \brief The collection class used by KD Chart to store data region information.

  This class is derived from QPtrList, so all of the Qt documentation for this class
  is valid for KDChartDataRegionList too.

  \note Normally there is no need to use this class yourself, since it
  is instantiated by the KDChartWidget. If however you are not using the
  KDChartWidget class but calling the painting methods of KDChart directly,
  make sure to either free the pointer stored in KDChartDataRegionList manually,
  or to call setAutoDelete( true ) to let your KDChartDataRegionList own these pointers:
  in this case please also make sure to call the clear() method whenever you want
  your KDChartDataRegionList to free these pointers.
  Note that all of this ONLY applies in case of NOT using the KDChartWidget.

  \sa KDChart, KDChartWidget
  */
typedef QPtrList < KDChartDataRegion > KDChartDataRegionList;

#endif
