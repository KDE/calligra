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

// KDE
#include <kdebug.h>
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


#if 1
static int debugIndent = 0;
#define DEBUGSTART() \
    ++debugIndent; \
    DEBUG_READING("entering")
#define DEBUGEND() \
    DEBUG_READING("exiting"); \
    --debugIndent
#define DEBUG_READING(param) \
    kDebug(30503) << QString("%1").arg(" ", debugIndent * 2) << param << ": " \
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


#if 0
// This is a template function for the reader library.
// Copy this one and change the name and fill in the code.
void OdfChartReader::readElementNamespaceTagname(KoXmlStreamReader &reader)
{
    DEBUGSTART();
    m_backend->elementNamespaceTagname(reader, m_context);

    // <namespace:tagname> has the following children in ODF 1.2:
    //   FILL IN THE CHILDREN LIKE THIS EXAMPLE (taken from office:document-content):
    //          <office:automatic-styles> 3.15.3
    //          <office:body> 3.3
    //          <office:font-face-decls> 3.14
    //          <office:scripts> 3.12.
    while (reader.readNextStartElement()) {
        QString tagName = reader.qualifiedName().toString();
        
        if (tagName == "office:automatic-styles") {
            // FIXME: NYI
            reader.skipCurrentElement();
        }
        else if (tagName == "office:body") {
            readElementOfficeBody(reader);
        }
        ...  MORE else if () HERE
        else {
            reader.skipCurrentElement();
        }
    }

    m_backend->elementNamespaceTagname(reader, m_context);
    DEBUGEND();
}
#endif


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
    //          <chart:axis> 11.8
    //          <chart:series> 11.11
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
            // FIXME: NYI
            reader.skipCurrentElement();
        }
        else if (tagName == "chart:series") {
            // FIXME: NYI
            reader.skipCurrentElement();
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

//----------------------------------------------------------------

#if 0
void OdfChartReader::readElementChart...(KoXmlStreamReader &reader)
{
    DEBUGSTART();
    m_backend->elementChart...(reader, m_context);

    // <chart:...> has the following children in ODF 1.2:
    //          <text:p> 5.1.3
    while (reader.readNextStartElement()) {
        QString tagName = reader.qualifiedName().toString();
        
        if (tagName == "text:p") {
	    OdfTextReader *textReader = m_parent->textReader();
	    if (textReader) {
		textReader->readElementTextP(reader);
	    }
	    else {
		reader.skipCurrentElement();
	    }
        }
        else {
            reader.skipCurrentElement();
        }
    }

    m_backend->elementChart...(reader, m_context);
    DEBUGEND();
}
#endif
// qwe

IMPLEMENT_READER_FUNCTION_TEXTP_ONLY(OdfChartReader, ChartEquation)
IMPLEMENT_READER_FUNCTION_NO_CHILDREN(OdfChartReader, ChartStockGainMarker)
IMPLEMENT_READER_FUNCTION_NO_CHILDREN(OdfChartReader, ChartStockLossMarker)
IMPLEMENT_READER_FUNCTION_NO_CHILDREN(OdfChartReader, ChartStockRangeLine)



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
