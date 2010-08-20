/*
 * This file is part of Office 2007 Filters for KOffice
 *
 * Copyright (C) 2010 Sebastian Sauer <sebsauer@kdab.com>
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include "XlsxXmlDrawingReader.h"
#include "XlsxXmlWorksheetReader.h"
#include "XlsxXmlChartReader.h"
#include "XlsxImport.h"
#include "Charting.h"
#include "ChartExport.h"

#include <MsooXmlSchemas.h>
#include <MsooXmlUtils.h>
#include <MsooXmlRelationships.h>
#include <MsooXmlUnits.h>
#include <MsooXmlDiagramReader.h>

#include <KoXmlWriter.h>
#include <KoGenStyles.h>
#include <KoOdfGraphicStyles.h>
#include <KoUnit.h>

#define MSOOXML_CURRENT_NS "xdr"
#define MSOOXML_CURRENT_CLASS XlsxXmlDrawingReader
#define BIND_READ_CLASS MSOOXML_CURRENT_CLASS

#include <MsooXmlReader_p.h>
#include <MsooXmlUtils.h>
#include <MsooXmlContentTypes.h>

XlsxXmlDrawingReaderContext::XlsxXmlDrawingReaderContext(XlsxXmlWorksheetReaderContext* _worksheetReaderContext, const QString& _path, const QString& _file)
    : MSOOXML::MsooXmlReaderContext(_worksheetReaderContext->relationships)
    , import(_worksheetReaderContext->import)
    , path(_path)
    , file(_file)
    , themes(&(*_worksheetReaderContext->themes))
    , worksheetReaderContext(_worksheetReaderContext)
{
}

XlsxXmlDrawingReaderContext::~XlsxXmlDrawingReaderContext()
{
    foreach(XlsxXmlChartReaderContext* c, charts) {
        delete c->m_chart;
        delete c->m_chartExport;
    }
}

XlsxXmlDrawingReader::XlsxXmlDrawingReader(KoOdfWriters *writers)
    : MSOOXML::MsooXmlCommonReader(writers)
    , m_anchorType(XlsxXmlDrawingReaderContext::NoAnchor)
    , m_chartNumber(0)
{
}

XlsxXmlDrawingReader::~XlsxXmlDrawingReader()
{
}

KoFilter::ConversionStatus XlsxXmlDrawingReader::read(MSOOXML::MsooXmlReaderContext* context)
{
    m_context = dynamic_cast<XlsxXmlDrawingReaderContext*>(context);
    Q_ASSERT(m_context);

    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }

    readNext();
    if (!expectEl("xdr:wsDr")) {
        return KoFilter::WrongFormat;
    }
    
    while (!atEnd()) {
        QXmlStreamReader::TokenType tokenType = readNext();
        if(tokenType == QXmlStreamReader::Invalid || tokenType == QXmlStreamReader::EndDocument) break;
        if (isStartElement()) {
            // twoCellAnchor does define the 'from' and 'to' elements which do define the anchor-points
            TRY_READ_IF(from)
            ELSE_TRY_READ_IF(to)
            // a graphic-frame
            ELSE_TRY_READ_IF(graphicFrame)
        }
    }

    m_context = 0;
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL from
KoFilter::ConversionStatus XlsxXmlDrawingReader::read_from()
{
    READ_PROLOGUE
    m_anchorType = XlsxXmlDrawingReaderContext::FromAnchor;
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            TRY_READ_IF(col)
            ELSE_TRY_READ_IF(row)
            ELSE_TRY_READ_IF(colOff)
            ELSE_TRY_READ_IF(rowOff)
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    m_anchorType = XlsxXmlDrawingReaderContext::NoAnchor;
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL to
KoFilter::ConversionStatus XlsxXmlDrawingReader::read_to()
{
    READ_PROLOGUE
    m_anchorType = XlsxXmlDrawingReaderContext::ToAnchor;
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            TRY_READ_IF(col)
            ELSE_TRY_READ_IF(row)
            ELSE_TRY_READ_IF(colOff)
            ELSE_TRY_READ_IF(rowOff)
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    m_anchorType = XlsxXmlDrawingReaderContext::NoAnchor;
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL col
KoFilter::ConversionStatus XlsxXmlDrawingReader::read_col()
{
    m_context->m_positions[m_anchorType].m_col = readElementText().toInt(); // default value is zero
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL row
KoFilter::ConversionStatus XlsxXmlDrawingReader::read_row()
{
    m_context->m_positions[m_anchorType].m_row = readElementText().toInt(); // default value is zero
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL colOff
KoFilter::ConversionStatus XlsxXmlDrawingReader::read_colOff()
{
    m_context->m_positions[m_anchorType].m_colOff = readElementText().toInt(); // default value is zero
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL rowOff
KoFilter::ConversionStatus XlsxXmlDrawingReader::read_rowOff()
{
    m_context->m_positions[m_anchorType].m_rowOff = readElementText().toInt(); // default value is zero
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL chart
KoFilter::ConversionStatus XlsxXmlDrawingReader::read_chart2()
{
    Q_ASSERT(m_context);
    Q_ASSERT(m_context->worksheetReaderContext);
    Q_ASSERT(m_context->worksheetReaderContext->import);

    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITH_NS(r, id)
    if(!r_id.isEmpty()) {
        //! @todo use MSOOXML::MsooXmlRelationships
        const QString path = "/xl/charts";
        const QString file = QString("chart%1.xml").arg(++m_chartNumber);
        const QString filepath = path + "/" + file;

        Charting::Chart* chart = new Charting::Chart;
        chart->m_sheetName = m_context->worksheetReaderContext->worksheetName;
        if(m_context->m_positions.contains(XlsxXmlDrawingReaderContext::FromAnchor)) {
            XlsxXmlDrawingReaderContext::Position f = m_context->m_positions[XlsxXmlDrawingReaderContext::FromAnchor];
            chart->m_fromRow = f.m_row;
            chart->m_fromColumn = f.m_col;
            if(m_context->m_positions.contains(XlsxXmlDrawingReaderContext::ToAnchor)) {
                f = m_context->m_positions[XlsxXmlDrawingReaderContext::ToAnchor];
                chart->m_toRow = f.m_row;
                chart->m_toColumn = f.m_col;
            }
        }
        ChartExport* chartexport = new ChartExport(chart);

        KoStore* storeout = m_context->worksheetReaderContext->import->outputStore();
        XlsxXmlChartReaderContext* context = new XlsxXmlChartReaderContext(storeout, chartexport);
        
        XlsxXmlChartReader reader(this);
        const KoFilter::ConversionStatus result = m_context->worksheetReaderContext->import->loadAndParseDocument(&reader, filepath, context);
        if (result != KoFilter::OK) {
            raiseError(reader.errorString());
            delete context;
            return result;
        }

        m_context->charts << context;
    }

    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL graphicFrame
//! graphicFrame
/*!
  This element specifies the existence of a graphics frame. This frame contains a graphic that was generated
  by an external source and needs a container in which to be displayed on the slide surface.

  Parent Elements:
    - grpSp (§4.4.1.19); spTree (§4.4.1.42)
  Child Elements:
    - extLst (Extension List with Modification Flag) (§4.2.4)
    - graphic (Graphic Object) (§5.1.2.1.16)
    - nvGraphicFramePr (Non-Visual Properties for a Graphic Frame) (§4.4.1.27)
    - xfrm (2D Transform for Graphic Frame)
*/
KoFilter::ConversionStatus XlsxXmlDrawingReader::read_graphicFrame()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            if (qualifiedName() == "a:graphic") {
                read_graphic2();
            }
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#define blipFill_NS "a"
#undef MSOOXML_CURRENT_NS
#define MSOOXML_CURRENT_NS "a"

#undef CURRENT_EL
#define CURRENT_EL graphic
//! graphic handler (Graphic Object)
KoFilter::ConversionStatus XlsxXmlDrawingReader::read_graphic2()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            if (qualifiedName() == "a:graphicData") {
                read_graphicData2();
            }
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL graphicData
//! graphicData handler (Graphic Object Data)
KoFilter::ConversionStatus XlsxXmlDrawingReader::read_graphicData2()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            //TRY_READ_IF_NS(pic, pic)
            if (qualifiedName() == "c:chart") {
                read_chart2();
            }
            else if (qualifiedName() == QLatin1String("dgm:relIds")) {
                read_diagram(); // DrawingML diagram
            }
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL diagram
//! 5.9 DrawingML - Diagrams
/*!
A DrawingML diagram allows the definition of diagrams using DrawingML objects and constructs. This
namespace defines the contents of a DrawingML diagram.
*/
KoFilter::ConversionStatus XlsxXmlDrawingReader::read_diagram()
{
    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR_WITH_NS(r, cs) // colors
    TRY_READ_ATTR_WITH_NS(r, dm) // data
    TRY_READ_ATTR_WITH_NS(r, lo) // layout
    TRY_READ_ATTR_WITH_NS(r, qs) // quickStyle

    const QString path = "xl/diagrams/";
    //const QString colorsfile     = r_cs.isEmpty() ? QString() : path + m_context->relationships->linkTarget(r_cs, m_context->path, m_context->file);
    const QString datafile       = r_dm.isEmpty() ? QString() : path + m_context->relationships->linkTarget(r_dm, m_context->path, m_context->file);
    const QString layoutfile     = r_lo.isEmpty() ? QString() : path + m_context->relationships->linkTarget(r_lo, m_context->path, m_context->file);
    //const QString quickstylefile = r_qs.isEmpty() ? QString() : path + m_context->relationships->linkTarget(r_qs, m_context->path, m_context->file);

    //kDebug()<<"colorsfile="<<colorsfile<<"datafile="<<datafile<<"layoutfile="<<layoutfile<<"quickstylefile="<<quickstylefile;

    KoStore* storeout = m_context->import->outputStore();
    MSOOXML::MsooXmlDiagramReaderContext context(storeout);

    // first read the data-model
    MSOOXML::MsooXmlDiagramReader dataReader(this);
    const KoFilter::ConversionStatus dataReaderResult = m_context->import->loadAndParseDocument(&dataReader, datafile, &context);
    if (dataReaderResult != KoFilter::OK) {
       raiseError(dataReader.errorString());
       return dataReaderResult;
    }
    
    // then read the layout definition
    MSOOXML::MsooXmlDiagramReader layoutReader(this);
    const KoFilter::ConversionStatus layoutReaderResult = m_context->import->loadAndParseDocument(&layoutReader, layoutfile, &context);
    if (layoutReaderResult != KoFilter::OK) {
       raiseError(layoutReader.errorString());
       return layoutReaderResult;
    }

    return KoFilter::OK;
}

// // in PPTX we do not have pPr, so p@text:style-name should be added earlier
// //#define SETUP_PARA_STYLE_IN_READ_P
// #include <MsooXmlCommonReaderImpl.h> // this adds a:p, a:pPr, a:t, a:r, etc.
// #define DRAWINGML_NS "a"
// #define DRAWINGML_PIC_NS "p" // DrawingML/Picture
// #include <MsooXmlCommonReaderDrawingMLImpl.h> // this adds p:pic, etc.
// //#include <MsooXmlDrawingReaderTableImpl.h> //this adds a:tbl
