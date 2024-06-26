/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2013-2014 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ODFCHARTREADERBACKEND_H
#define ODFCHARTREADERBACKEND_H

// Calligra
#include <KoFilter.h>
#include <KoXmlStreamReader.h>

// this library
#include "OdfChartReader.h"
#include "OdfReaderInternals.h"
#include "koodfreader_export.h"

class QByteArray;
class QSizeF;
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
    DECLARE_BACKEND_FUNCTION(ChartChart); // ODF 1.2  11.1

    DECLARE_BACKEND_FUNCTION(ChartFooter); // ODF 1.2  11.2.3
    DECLARE_BACKEND_FUNCTION(ChartSubtitle); // ODF 1.2  11.2.2
    DECLARE_BACKEND_FUNCTION(ChartTitle); // ODF 1.2  11.2.1
    DECLARE_BACKEND_FUNCTION(ChartLegend); // ODF 1.2  11.3
    DECLARE_BACKEND_FUNCTION(ChartPlotArea); // ODF 1.2  11.4
    DECLARE_BACKEND_FUNCTION(ChartWall); // ODF 1.2  11.6
    DECLARE_BACKEND_FUNCTION(ChartFloor); // ODF 1.2  11.7
    DECLARE_BACKEND_FUNCTION(ChartAxis); // ODF 1.2  11.8
    DECLARE_BACKEND_FUNCTION(ChartCategories); // ODF 1.2  11.9
    DECLARE_BACKEND_FUNCTION(ChartGrid); // ODF 1.2  11.10
    DECLARE_BACKEND_FUNCTION(ChartSeries); // ODF 1.2  11.11
    DECLARE_BACKEND_FUNCTION(ChartDomain); // ODF 1.2  11.12
    DECLARE_BACKEND_FUNCTION(ChartDataPoint); // ODF 1.2  11.13
    DECLARE_BACKEND_FUNCTION(ChartDataLabel); // ODF 1.2  11.14
    DECLARE_BACKEND_FUNCTION(ChartMeanValue); // ODF 1.2  11.15
    DECLARE_BACKEND_FUNCTION(ChartErrorIndicator); // ODF 1.2  11.16
    DECLARE_BACKEND_FUNCTION(ChartRegressionCurve); // ODF 1.2  11.17
    DECLARE_BACKEND_FUNCTION(ChartEquation); // ODF 1.2  11.18
    DECLARE_BACKEND_FUNCTION(ChartStockGainMarker); // ODF 1.2  11.19
    DECLARE_BACKEND_FUNCTION(ChartStockLossMarker); // ODF 1.2  11.20
    DECLARE_BACKEND_FUNCTION(ChartStockRangeLine); // ODF 1.2  11.21

private:
    class Private;
    Private *const d;
};

#endif // ODFCHARTREADERBACKEND_H
