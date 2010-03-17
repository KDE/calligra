/*
 * This file is part of Office 2007 Filters for KOffice
 *
 * Copyright (C) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#define MSOOXML_CURRENT_NS "xdr"
#define MSOOXML_CURRENT_CLASS XlsxXmlDrawingReader
#define BIND_READ_CLASS MSOOXML_CURRENT_CLASS

#include <MsooXmlReader_p.h>
#include <MsooXmlUtils.h>

XlsxXmlDrawingReaderContext::XlsxXmlDrawingReaderContext()
    : MSOOXML::MsooXmlReaderContext()
{
}

XlsxXmlDrawingReaderContext::~XlsxXmlDrawingReaderContext()
{
}

XlsxXmlDrawingReader::XlsxXmlDrawingReader(KoOdfWriters *writers)
    : MSOOXML::MsooXmlCommonReader(writers)
    , m_anchorToken(NoAnchor)
    , m_chartNumber(0)
{
}

XlsxXmlDrawingReader::~XlsxXmlDrawingReader()
{
}

KoFilter::ConversionStatus XlsxXmlDrawingReader::read(MSOOXML::MsooXmlReaderContext* context)
{
    kDebug()<<"XlsxXmlDrawingReader #######################################################";
    
    m_context = dynamic_cast<XlsxXmlDrawingReaderContext*>(context);
    Q_ASSERT(m_context);

    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }

    readNext();
    if(namespaceUri() != "http://schemas.openxmlformats.org/drawingml/2006/spreadsheetDrawing") {
        return KoFilter::WrongFormat;
    }
    if (!expectEl("xdr:wsDr")) {
        return KoFilter::WrongFormat;
    }
    
    while (!atEnd()) {
        QXmlStreamReader::TokenType tokenType = readNext();
        if(tokenType == QXmlStreamReader::Invalid || tokenType == QXmlStreamReader::EndDocument) break;
        if (isStartElement()) {
            //kDebug()<<"............................"<<tokenString()<<prefix()<<name();

            // twoCellAnchor does define the 'from' and 'to' elements which do define the anchor-points
            TRY_READ_IF(from)
            ELSE_TRY_READ_IF(to)
            // the reference to a chart
            ELSE_TRY_READ_IF_NS(c, chart)
        }
    }

//Q_ASSERT(false);
    m_context = 0;
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL from
KoFilter::ConversionStatus XlsxXmlDrawingReader::read_from()
{
    m_anchorToken = FromAnchor;
    READ_PROLOGUE
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
    READ_EPILOGUE
    m_anchorToken = NoAnchor;
}

#undef CURRENT_EL
#define CURRENT_EL to
KoFilter::ConversionStatus XlsxXmlDrawingReader::read_to()
{
    m_anchorToken = ToAnchor;
    READ_PROLOGUE
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
    READ_EPILOGUE
    m_anchorToken = NoAnchor;
}

#undef CURRENT_EL
#define CURRENT_EL col
KoFilter::ConversionStatus XlsxXmlDrawingReader::read_col()
{
    m_positions[m_anchorToken].m_col = readElementText().toInt();
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL row
KoFilter::ConversionStatus XlsxXmlDrawingReader::read_row()
{
    m_positions[m_anchorToken].m_row = readElementText().toInt();
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL colOff
KoFilter::ConversionStatus XlsxXmlDrawingReader::read_colOff()
{
    m_positions[m_anchorToken].m_colOff = readElementText().toInt();
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL rowOff
KoFilter::ConversionStatus XlsxXmlDrawingReader::read_rowOff()
{
    m_positions[m_anchorToken].m_rowOff = readElementText().toInt();
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL chart
KoFilter::ConversionStatus XlsxXmlDrawingReader::read_chart()
{
    kDebug()<<"3AAAAAAAAAAA"<<text();
    
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITH_NS(r, id)
    if(!r_id.isEmpty()) {
        //! @todo use MSOOXML::MsooXmlRelationships

        const QString path = "/xl/charts";
        const QString file = QString("chart%1.xml").arg(++m_chartNumber);
        const QString filepath = path + file;

#if 0
        XlsxXmlChartReader reader(this);
        XlsxXmlChartReaderContext context;
        const KoFilter::ConversionStatus result = m_context->import->loadAndParseDocument(&reader, filepath, &context);
        if (result != KoFilter::OK) {
            raiseError(reader.errorString());
            return result;
        }
#endif
    }


    Q_ASSERT(false);
    return KoFilter::OK;
}
