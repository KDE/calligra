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

#ifndef ODFCHARTREADERBACKEND_H
#define ODFCHARTREADERBACKEND_H

// Calligra
#include <KoXmlStreamReader.h>
#include <KoFilter.h>

// this library
#include "koodfreader_export.h"
#include "OdfChartReader.h"
#include "OdfReaderInternals.h"


class QByteArray;
class QSizeF;
class QStringList;
class KoStore;
class OdfReaderContext;


/** @brief A default backend for the OdfChartReader class.
 *
 * This class defines an interface and the default behaviour for the
 * backend to the ODF chart reader (@see OdfChartReader). When the
 * reader is called upon to traverse a certain XML tree, there will
 * be two parameters to the root traverse function: a pointer to a
 * backend object and a pointer to a context object.
 *
 * The reader will traverse (read) the XML tree and for every element
 * it comes across it will call a specific function in the backend and
 * every call will pass the pointer to the context object.
 *
 * Each supported XML tag has a corresponding callback function. This
 * callback function will be called twice: once when the tag is first
 * encountered anc once when the tag is closed.  This means that an
 * element with no child elements will be called twice in succession.
 */
class KOODFREADER_EXPORT OdfChartReaderBackend
{
 public:
    explicit OdfChartReaderBackend();
    virtual ~OdfChartReaderBackend();

    DECLARE_BACKEND_FUNCTION(OfficeChart);
    DECLARE_BACKEND_FUNCTION(ChartChart);

    DECLARE_BACKEND_FUNCTION(ChartFooter);
    DECLARE_BACKEND_FUNCTION(ChartSubtitle);
    DECLARE_BACKEND_FUNCTION(ChartTitle);
    DECLARE_BACKEND_FUNCTION(ChartLegend);
    DECLARE_BACKEND_FUNCTION(ChartPlotArea);
    DECLARE_BACKEND_FUNCTION(ChartWall);
    DECLARE_BACKEND_FUNCTION(ChartFloor);
    // qwe
    DECLARE_BACKEND_FUNCTION(ChartEquation);
    DECLARE_BACKEND_FUNCTION(ChartStockGainMarker);
    DECLARE_BACKEND_FUNCTION(ChartStockLossMarker);
    DECLARE_BACKEND_FUNCTION(ChartStockRangeLine);

 private:
    class Private;
    Private * const d;
};


#endif // ODFCHARTREADERBACKEND_H
