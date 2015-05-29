/* This file is part of the KDE project

   Copyright (C) 2012-2014 Inge Wallin            <inge@lysator.liu.se>

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

#ifndef ODFCHARTREADER_H
#define ODFCHARTREADER_H

// Qt
#include <QHash>
#include <QString>

// Calligra
#include <KoXmlStreamReader.h>

// this library
#include "koodfreader_export.h"
#include "OdfReaderInternals.h"


class QSizeF;

class OdfReader;
class OdfChartReaderBackend;
class OdfReaderContext;


/** @brief Read the XML tree of the content of an ODT file.
 *
 * The OdfChartReader is used to traverse (read) the chart contents of
 * an ODF file using an XML stream reader.  For every XML element that
 * the reading process comes across it will call a specific function
 * in a backend class: @see OdfChartReaderBackend.  The OdfChartReader
 * is used as a common way to read chart content and is called from all
 * readers for different ODF formats.  @see OdtReader, @see OdsReader,
 * @see OdpReader.
 */
class KOODFREADER_EXPORT OdfChartReader
{
 public:
    OdfChartReader();
    ~OdfChartReader();

    void setParent(OdfReader *parent);
    void setBackend(OdfChartReaderBackend *backend);
    void setContext(OdfReaderContext *context);

    // ----------------------------------------------------------------
    // element reader functions

    DECLARE_READER_FUNCTION(OfficeChart);
    DECLARE_READER_FUNCTION(ChartChart);           // ODF 1.2  11.1

    DECLARE_READER_FUNCTION(ChartFooter);	   // ODF 1.2  11.2.3
    DECLARE_READER_FUNCTION(ChartSubtitle);	   // ODF 1.2  11.2.2
    DECLARE_READER_FUNCTION(ChartTitle);	   // ODF 1.2  11.2.1
    DECLARE_READER_FUNCTION(ChartLegend);	   // ODF 1.2  11.3
    DECLARE_READER_FUNCTION(ChartPlotArea);	   // ODF 1.2  11.4
    DECLARE_READER_FUNCTION(ChartWall);		   // ODF 1.2  11.6
    DECLARE_READER_FUNCTION(ChartFloor);	   // ODF 1.2  11.7
    DECLARE_READER_FUNCTION(ChartAxis);            // ODF 1.2  11.8
    DECLARE_READER_FUNCTION(ChartCategories);      // ODF 1.2  11.9
    DECLARE_READER_FUNCTION(ChartGrid);		   // ODF 1.2  11.10
    DECLARE_READER_FUNCTION(ChartSeries);	   // ODF 1.2  11.11
    DECLARE_READER_FUNCTION(ChartDomain);	   // ODF 1.2  11.12
    DECLARE_READER_FUNCTION(ChartDataPoint);	   // ODF 1.2  11.13
    DECLARE_READER_FUNCTION(ChartDataLabel);	   // ODF 1.2  11.14
    DECLARE_READER_FUNCTION(ChartMeanValue);	   // ODF 1.2  11.15
    DECLARE_READER_FUNCTION(ChartErrorIndicator);  // ODF 1.2  11.16
    DECLARE_READER_FUNCTION(ChartRegressionCurve); // ODF 1.2  11.17
    DECLARE_READER_FUNCTION(ChartEquation);        // ODF 1.2  11.18
    DECLARE_READER_FUNCTION(ChartStockGainMarker); // ODF 1.2  11.19
    DECLARE_READER_FUNCTION(ChartStockLossMarker); // ODF 1.2  11.20
    DECLARE_READER_FUNCTION(ChartStockRangeLine);  // ODF 1.2  11.21

 protected:


    // ----------------------------------------------------------------
    // Other functions

    // FIXME: Move this to a common file (OdfReaderUtils?)
    void readUnknownElement(KoXmlStreamReader &reader);


 private:
    OdfReader             *m_parent;  // The OdfReader controlling this one.

    OdfChartReaderBackend *m_backend;
    OdfReaderContext      *m_context;
};

#endif // ODFCHARTREADER_H
