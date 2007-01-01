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
#include "KDChartAreaPainter.h"

/**
  \class KDChartAreaPainter KDChartAreaPainter.h

  \brief Implements a chart painter that draws area charts.
  */

/**
  Constructor. Sets up internal data structures as necessary.

  \param params the KDChartParams structure that defines the chart
  */
    KDChartAreaPainter::KDChartAreaPainter( KDChartParams* params ) :
KDChartLinesPainter( params )
{
    // This constructor intentionally left blank so far; we cannot setup the
    // geometry yet since we do not know the size of the painter.
}


/**
  Destructor.
  */
KDChartAreaPainter::~KDChartAreaPainter()
{
    // intentionally left blank
}


/**
  Paints the actual data area.

  \param painter the QPainter onto which the chart should be painted
  \param data the data that will be displayed as a chart
  \param paint2nd specifies whether the main chart or the additional chart is to be drawn now
  \param regions a pointer to a list of regions that will be filled
  with regions representing the data segments, if not null
  */
void KDChartAreaPainter::paintData( QPainter* painter,
        KDChartTableDataBase* data,
        bool paint2nd,
        KDChartDataRegionList* regions )
{
    paintDataInternal( painter, data,
            false,  // non-centered points
            false,  // no line markers
            true,  // an area
            paint2nd,
            regions );
}

