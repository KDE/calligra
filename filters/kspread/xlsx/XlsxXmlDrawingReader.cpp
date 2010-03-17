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

XlsxXmlDrawingReaderContext::XlsxXmlDrawingReaderContext() : MSOOXML::MsooXmlReaderContext() {}
XlsxXmlDrawingReaderContext::~XlsxXmlDrawingReaderContext() {}

XlsxXmlDrawingReader::XlsxXmlDrawingReader(KoOdfWriters *writers) : MSOOXML::MsooXmlCommonReader(writers) {}
XlsxXmlDrawingReader::~XlsxXmlDrawingReader() {}

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
            kDebug()<<"............................"<<tokenString()<<prefix()<<name();

            // twoCellAnchor does define from and to
            TRY_READ_IF(from)
            ELSE_TRY_READ_IF(to)
            // the reference to a chart
            //ELSE_TRY_READ_IF(chart)
        }
    }


    //readNext();
    //TRY_READ(worksheet)

//Q_ASSERT(false);
    m_context = 0;
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL from
KoFilter::ConversionStatus XlsxXmlDrawingReader::read_from()
{
    kDebug()<<"1-AAAAAAAAAAA"<<text();
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL to
KoFilter::ConversionStatus XlsxXmlDrawingReader::read_to()
{
    kDebug()<<"2-AAAAAAAAAAA"<<text();
    return KoFilter::OK;
}

/*
#undef CURRENT_EL
#define CURRENT_EL chart
KoFilter::ConversionStatus XlsxXmlDrawingReader::read_chart()
{
    kDebug()<<"3AAAAAAAAAAA"<<text();
    return KoFilter::OK;
}
*/
