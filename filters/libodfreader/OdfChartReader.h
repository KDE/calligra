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
    DECLARE_READER_FUNCTION(ChartChart);

    DECLARE_READER_FUNCTION(ChartFooter);
    DECLARE_READER_FUNCTION(ChartSubtitle);
    DECLARE_READER_FUNCTION(ChartTitle);
    DECLARE_READER_FUNCTION(ChartLegend);
    DECLARE_READER_FUNCTION(ChartPlotArea);
    DECLARE_READER_FUNCTION(ChartWall);
    DECLARE_READER_FUNCTION(ChartFloor);

    DECLARE_READER_FUNCTION(ChartEquation);
    DECLARE_READER_FUNCTION(ChartStockGainMarker);
    DECLARE_READER_FUNCTION(ChartStockLossMarker);
    DECLARE_READER_FUNCTION(ChartStockRangeLine);

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
