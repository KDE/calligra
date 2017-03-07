/* This file is part of the KDE project

   Copyright (C) 2013-2014 Inge Wallin       <inge@lysator.liu.se>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/


// Own
#include "OdfChartReaderBackend.h"

// Calligra
#include <KoXmlReader.h>

// Odftraverse library
#include "OdfParser.h"


// ================================================================
//             class OdfChartReaderBackend::Private


class OdfChartReaderBackend::Private
{
 public:
    Private();
    ~Private();

    bool dummy;                 // We don't have any actual content in
                                // this class yet but it's still
                                // needed for forward binary compatibility.
};

OdfChartReaderBackend::Private::Private()
{
}

OdfChartReaderBackend::Private::~Private()
{
}


// ================================================================
//                 class OdfChartReaderBackend


OdfChartReaderBackend::OdfChartReaderBackend()
    : d(new OdfChartReaderBackend::Private)
{
}

OdfChartReaderBackend::~OdfChartReaderBackend()
{
    delete d;
}


// ----------------------------------------------------------------


IMPLEMENT_BACKEND_FUNCTION(OdfChartReader, OfficeChart)

IMPLEMENT_BACKEND_FUNCTION(OdfChartReader, ChartChart)
IMPLEMENT_BACKEND_FUNCTION(OdfChartReader, ChartFooter)
IMPLEMENT_BACKEND_FUNCTION(OdfChartReader, ChartSubtitle)
IMPLEMENT_BACKEND_FUNCTION(OdfChartReader, ChartTitle)
IMPLEMENT_BACKEND_FUNCTION(OdfChartReader, ChartLegend)
IMPLEMENT_BACKEND_FUNCTION(OdfChartReader, ChartPlotArea)
IMPLEMENT_BACKEND_FUNCTION(OdfChartReader, ChartWall)
IMPLEMENT_BACKEND_FUNCTION(OdfChartReader, ChartFloor)
IMPLEMENT_BACKEND_FUNCTION(OdfChartReader, ChartAxis)
IMPLEMENT_BACKEND_FUNCTION(OdfChartReader, ChartCategories)
IMPLEMENT_BACKEND_FUNCTION(OdfChartReader, ChartGrid)
IMPLEMENT_BACKEND_FUNCTION(OdfChartReader, ChartSeries)
IMPLEMENT_BACKEND_FUNCTION(OdfChartReader, ChartDomain)
IMPLEMENT_BACKEND_FUNCTION(OdfChartReader, ChartDataPoint)
IMPLEMENT_BACKEND_FUNCTION(OdfChartReader, ChartDataLabel)
IMPLEMENT_BACKEND_FUNCTION(OdfChartReader, ChartMeanValue)
IMPLEMENT_BACKEND_FUNCTION(OdfChartReader, ChartErrorIndicator)
IMPLEMENT_BACKEND_FUNCTION(OdfChartReader, ChartRegressionCurve)
IMPLEMENT_BACKEND_FUNCTION(OdfChartReader, ChartEquation)
IMPLEMENT_BACKEND_FUNCTION(OdfChartReader, ChartStockGainMarker)
IMPLEMENT_BACKEND_FUNCTION(OdfChartReader, ChartStockLossMarker)
IMPLEMENT_BACKEND_FUNCTION(OdfChartReader, ChartStockRangeLine)
