/* -*- Mode: C++ -*-

  $Id$

  KDChart - a multi-platform charting engine

  Copyright (C) 2001 by Klarälvdalens Datakonsult AB
*/

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
                                    KDChartTableData* data,
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

