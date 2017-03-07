/* This file is part of the KDE project

   Copyright (C) 2012-2013 Inge Wallin            <inge@lysator.liu.se>

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
#include "OdfTextReader.h"

// Qt
#include <QStringList>
#include <QBuffer>

// KF5
#include <klocalizedstring.h>

// Calligra
#include <KoStore.h>
#include <KoXmlStreamReader.h>
#include <KoXmlNS.h>
#include <KoXmlWriter.h>  // For copyXmlElement
#include <KoOdfReadStore.h>

// Reader library
#include "OdfReader.h"
#include "OdfChartReaderBackend.h"
#include "OdfReaderContext.h"
#include "OdfReaderDebug.h"


#if 1
static int debugIndent = 0;
#define DEBUGSTART() \
    ++debugIndent; \
    DEBUG_READING("entering")
#define DEBUGEND() \
    DEBUG_READING("exiting"); \
    --debugIndent
#define DEBUG_READING(param) \
    debugOdfReader << QString("%1").arg(" ", debugIndent * 2) << param << ": " \
    << (reader.isStartElement() ? "start": (reader.isEndElement() ? "end" : "other")) \
    << reader.qualifiedName().toString()
#else
#define DEBUGSTART() \
    // NOTHING
#define DEBUGEND() \
    // NOTHING
#define DEBUG_READING(param) \
    // NOTHING
#endif


OdfChartReader::OdfChartReader()
    : m_parent(0)
    , m_backend(0)
    , m_context(0)
{
}

OdfChartReader::~OdfChartReader()
{
}


// ----------------------------------------------------------------


void OdfChartReader::setParent(OdfReader *parent)
{
    m_parent = parent;
}

void OdfChartReader::setBackend(OdfChartReaderBackend *backend)
{
    m_backend = backend;
}

void OdfChartReader::setContext(OdfReaderContext *context)
{
    m_context = context;
}


// ----------------------------------------------------------------


void OdfChartReader::readElementOfficeChart(KoXmlStreamReader &reader)
{
    DEBUGSTART();
    m_backend->elementOfficeChart(reader, m_context);

    // <office:chart> has the following children in ODF 1.2:
    //   [done] <chart:chart> 11.1
    //          <table:calculation-settings> 9.4.1
    //          <table:consolidation> 9.7
    //          <table:content-validations> 9.4.4
    //          <table:database-ranges> 9.4.14
    //          <table:data-pilot-tables> 9.6.2
    //          <table:dde-links> 9.8
    //          <table:label-ranges> 9.4.10
    //          <table:named-expressions> 9.4.11
    //          <text:alphabetical-index-auto-mark-file> 8.8.3
    //          <text:dde-connection-decls> 14.6.2
    //          <text:sequence-decls> 7.4.11
    //          <text:user-field-decls> 7.4.7
    //          <text:variable-decls> 7.4.2
    while (reader.readNextStartElement()) {
        QString tagName = reader.qualifiedName().toString();
        
        if (tagName == "chart:chart") {
	    readElementChartChart(reader);
        }
        else if (tagName == "table:calculation-settings") {
            // FIXME: NYI
            reader.skipCurrentElement();
        }
        //...  MORE else if () HERE
        else {
            reader.skipCurrentElement();
        }
    }

    m_backend->elementOfficeChart(reader, m_context);
    DEBUGEND();
}

void OdfChartReader::readElementChartChart(KoXmlStreamReader &reader)
{
    DEBUGSTART();
    m_backend->elementChartChart(reader, m_context);

    // <chart:chart> has the following children in ODF 1.2:
    //   [done] <chart:footer> 11.2.3
    //   [done] <chart:legend> 11.3
    //   [done] <chart:plot-area> 11.4
    //   [done] <chart:subtitle> 11.2.2
    //   [done] <chart:title> 11.2.1
    //   [done] <table:table> 9.1.2
    while (reader.readNextStartElement()) {
        QString tagName = reader.qualifiedName().toString();
        
        if (tagName == "chart:footer") {
	    readElementChartFooter(reader);
        }
        else if (tagName == "chart:subtitle") {
	    readElementChartSubtitle(reader);
        }
        else if (tagName == "chart:title") {
	    readElementChartTitle(reader);
        }
        else if (tagName == "chart:legend") {
	    readElementChartLegend(reader);
        }
        else if (tagName == "chart:plot-area") {
	    readElementChartPlotArea(reader);
        }
        else if (tagName == "table:table") {
	    OdfTextReader *textReader = m_parent->textReader();
	    if (textReader) {
		textReader->readElementTableTable(reader);
	    }
	    else {
		reader.skipCurrentElement();
	    }
        }
        else {
            reader.skipCurrentElement();
        }
    }

    m_backend->elementChartChart(reader, m_context);
    DEBUGEND();
}

#define IMPLEMENT_READER_FUNCTION_TEXTP_ONLY(readername, name)      \
IMPLEMENT_READER_FUNCTION_START(readername, name)                   \
    while (reader.readNextStartElement()) {                         \
        QString tagName = reader.qualifiedName().toString();        \
                                                                    \
        if (tagName == "text:p") {                                  \
	    OdfTextReader *textReader = m_parent->textReader();     \
	    if (textReader) {                                       \
		textReader->readElementTextP(reader);               \
	    }                                                       \
	    else {                                                  \
		reader.skipCurrentElement();                        \
	    }                                                       \
        }                                                           \
        else {                                                      \
            reader.skipCurrentElement();                            \
        }                                                           \
    }                                                               \
IMPLEMENT_READER_FUNCTION_END(name)


IMPLEMENT_READER_FUNCTION_TEXTP_ONLY(OdfChartReader, ChartFooter)
IMPLEMENT_READER_FUNCTION_TEXTP_ONLY(OdfChartReader, ChartSubtitle)
IMPLEMENT_READER_FUNCTION_TEXTP_ONLY(OdfChartReader, ChartTitle)
IMPLEMENT_READER_FUNCTION_TEXTP_ONLY(OdfChartReader, ChartLegend)


void OdfChartReader::readElementChartPlotArea(KoXmlStreamReader &reader)
{
    DEBUGSTART();
    m_backend->elementChartPlotArea(reader, m_context);

    // <chart:plot-area> has the following children in ODF 1.2:
    //   [done] <chart:wall> 11.6
    //   [done] <chart:floor> 11.7
    //   [done] <chart:axis> 11.8
    //   [done] <chart:series> 11.11
    //   [done] <chart:stock-gain-marker> 11.19
    //   [done] <chart:stock-loss-marker> 11.20
    //   [done] <chart:stock-range-line> 11.21
    //          <dr3d:light> 10.5.3
    while (reader.readNextStartElement()) {
        QString tagName = reader.qualifiedName().toString();
        
	if (tagName == "chart:wall") {
	    readElementChartWall(reader);
        }
        else if (tagName == "chart:floor") {
	    readElementChartFloor(reader);
        }
        else if (tagName == "chart:axis") {
	    readElementChartAxis(reader);
        }
        else if (tagName == "chart:series") {
	    readElementChartSeries(reader);
        }
        else if (tagName == "chart:stock-gain-marker") {
	    readElementChartStockGainMarker(reader);
        }
        else if (tagName == "chart:stock-loss-marker") {
	    readElementChartStockLossMarker(reader);
        }
        else if (tagName == "chart:stock-range-line") {
	    readElementChartStockRangeLine(reader);
        }
        else if (tagName == "dr3d:light") {
            // FIXME: NYI
            reader.skipCurrentElement();
        }
        else {
            reader.skipCurrentElement();
        }
    }

    m_backend->elementChartPlotArea(reader, m_context);
    DEBUGEND();
}

IMPLEMENT_READER_FUNCTION_NO_CHILDREN(OdfChartReader, ChartWall)
IMPLEMENT_READER_FUNCTION_NO_CHILDREN(OdfChartReader, ChartFloor)

void OdfChartReader::readElementChartAxis(KoXmlStreamReader &reader)
{
    DEBUGSTART();
    m_backend->elementChartAxis(reader, m_context);

    // <chart:axis> has the following children in ODF 1.2:
    //   [done] <chart:categories> 11.9
    //   [done] <chart:grid> 11.10
    //   [done] <chart:title> 11.2.1

    while (reader.readNextStartElement()) {
        QString tagName = reader.qualifiedName().toString();
        
        if (tagName == "chart:categories") {
	    readElementChartCategories(reader);
        }
        else if (tagName == "chart:grid") {
	    readElementChartGrid(reader);
        }
        else if (tagName == "chart:title") {
	    readElementChartTitle(reader);
        }
        else {
            reader.skipCurrentElement();
        }
    }

    m_backend->elementChartAxis(reader, m_context);
    DEBUGEND();
}

IMPLEMENT_READER_FUNCTION_NO_CHILDREN(OdfChartReader, ChartCategories)
IMPLEMENT_READER_FUNCTION_NO_CHILDREN(OdfChartReader, ChartGrid)

// ODF 1.2  11.11: <chart:series>
void OdfChartReader::readElementChartSeries(KoXmlStreamReader &reader)
{
    DEBUGSTART();
    m_backend->elementChartSeries(reader, m_context);

    // <chart:series> has the following children in ODF 1.2:
    //   [done] <chart:data-label> 11.14
    //   [done] <chart:data-point> 11.13
    //   [done] <chart:domain> 11.12
    //   [done] <chart:error-indicator> 11.16
    //   [done] <chart:mean-value> 11.15
    //   [done] <chart:regression-curve> 11.17

    while (reader.readNextStartElement()) {
        QString tagName = reader.qualifiedName().toString();
        
        if (tagName == "chart:data-label") {
	    readElementChartDataLabel(reader);
        }
        else if (tagName == "chart:data-point") {
	    readElementChartDataPoint(reader);
        }
        else if (tagName == "chart:domain") {
	    readElementChartDomain(reader);
        }
        else if (tagName == "chart:error-indicator") {
	    readElementChartErrorIndicator(reader);
        }
        else if (tagName == "chart:mean-value") {
	    readElementChartMeanValue(reader);
        }
        else if (tagName == "chart:regression-curve") {
	    readElementChartRegressionCurve(reader);
        }
        else {
            reader.skipCurrentElement();
        }
    }

    m_backend->elementChartSeries(reader, m_context);
    DEBUGEND();
}

IMPLEMENT_READER_FUNCTION_NO_CHILDREN(OdfChartReader, ChartDomain)          // ODF 1.2  11.12
IMPLEMENT_READER_FUNCTION_ONE_CHILD(OdfChartReader, ChartDataPoint,
				    "chart:data-label", ChartDataLabel)     // ODF 1.2  11.13
IMPLEMENT_READER_FUNCTION_TEXTP_ONLY(OdfChartReader, ChartDataLabel)        // ODF 1.2  11.14
IMPLEMENT_READER_FUNCTION_NO_CHILDREN(OdfChartReader, ChartMeanValue)       // ODF 1.2  11.15
IMPLEMENT_READER_FUNCTION_NO_CHILDREN(OdfChartReader, ChartErrorIndicator)  // ODF 1.2  11.16
IMPLEMENT_READER_FUNCTION_ONE_CHILD(OdfChartReader, ChartRegressionCurve,
				    "chart:equation", ChartEquation)        // ODF 1.2  11.17
IMPLEMENT_READER_FUNCTION_TEXTP_ONLY(OdfChartReader, ChartEquation)         // ODF 1.2  11.18
IMPLEMENT_READER_FUNCTION_NO_CHILDREN(OdfChartReader, ChartStockGainMarker) // ODF 1.2  11.19
IMPLEMENT_READER_FUNCTION_NO_CHILDREN(OdfChartReader, ChartStockLossMarker) // ODF 1.2  11.20
IMPLEMENT_READER_FUNCTION_NO_CHILDREN(OdfChartReader, ChartStockRangeLine)  // ODF 1.2  11.21



// ----------------------------------------------------------------
//                             Other functions


void OdfChartReader::readUnknownElement(KoXmlStreamReader &reader)
{
    DEBUGSTART();

#if 0  // FIXME: Fix this
    if (m_context->isInsideParagraph()) {
        // readParagraphContents expect to have the reader point to the
        // contents of the paragraph so we have to read past the chart:p
        // start tag here.
        reader.readNext();
        readParagraphContents(reader);
    }
    else {
        while (reader.readNextStartElement()) {
            readTextLevelElement(reader);
        }
    }
#else
    reader.skipCurrentElement();
#endif

    DEBUGEND();
}
