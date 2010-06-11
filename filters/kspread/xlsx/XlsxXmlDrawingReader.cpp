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

#define MSOOXML_CURRENT_NS "xdr"
#define MSOOXML_CURRENT_CLASS XlsxXmlDrawingReader
#define BIND_READ_CLASS MSOOXML_CURRENT_CLASS

#include <MsooXmlReader_p.h>
#include <MsooXmlUtils.h>

XlsxXmlDrawingReaderContext::XlsxXmlDrawingReaderContext(XlsxXmlWorksheetReaderContext* _worksheetReaderContext)
    : MSOOXML::MsooXmlReaderContext()
    , worksheetReaderContext(_worksheetReaderContext)
{
}

XlsxXmlDrawingReaderContext::~XlsxXmlDrawingReaderContext()
{
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
            // the reference to a chart
            ELSE_TRY_READ_IF_NS(c, chart)
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
KoFilter::ConversionStatus XlsxXmlDrawingReader::read_chart()
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

        KoStore* storeout = m_context->worksheetReaderContext->import->outputStore();
        XlsxXmlChartReaderContext* context = new XlsxXmlChartReaderContext(m_context, storeout);
        
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
