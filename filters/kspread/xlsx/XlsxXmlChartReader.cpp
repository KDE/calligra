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

#include "XlsxXmlChartReader.h"
#include "XlsxXmlDrawingReader.h"
#include "XlsxXmlWorksheetReader.h"
#include "XlsxImport.h"

#include "Charting.h"
#include "ChartExport.h"

#define MSOOXML_CURRENT_NS "c"
#define MSOOXML_CURRENT_CLASS XlsxXmlChartReader
#define BIND_READ_CLASS MSOOXML_CURRENT_CLASS

#include <MsooXmlReader_p.h>
#include <MsooXmlUtils.h>

// Returns A for 1, B for 2, C for 3, etc.
QString columnName(uint column)
{
    QString s;
    unsigned digits = 1;
    unsigned offset = 0;
    for (unsigned limit = 26; column >= limit + offset; limit *= 26, digits++)
        offset += limit;
    for (unsigned col = column - offset; digits; --digits, col /= 26)
        s.prepend(QChar('A' + (col % 26)));
    return s;
}

XlsxXmlChartReaderContext::XlsxXmlChartReaderContext(XlsxXmlDrawingReaderContext* _drawingReaderContext)
    : MSOOXML::MsooXmlReaderContext()
    , drawingReaderContext(_drawingReaderContext)
{
}

XlsxXmlChartReaderContext::~XlsxXmlChartReaderContext()
{
}

class XlsxXmlChartReader::Private
{
public:
    XlsxXmlChartReaderContext *m_context;
    Charting::Chart *m_chart;
    int chartNumber;
    
    explicit Private() : m_context(0), m_chart(0), chartNumber(0) {}
    ~Private() { delete m_chart; }
};

XlsxXmlChartReader::XlsxXmlChartReader(KoOdfWriters *writers)
    : MSOOXML::MsooXmlCommonReader(writers)
    , d(new Private)
{
}

XlsxXmlChartReader::~XlsxXmlChartReader()
{
    delete d;
}

KoFilter::ConversionStatus XlsxXmlChartReader::read(MSOOXML::MsooXmlReaderContext* context)
{
    d->m_context = dynamic_cast<XlsxXmlChartReaderContext*>(context);
    Q_ASSERT(d->m_context);
    Q_ASSERT(d->m_context->drawingReaderContext);
    Q_ASSERT(d->m_context->drawingReaderContext->worksheetReaderContext);

    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }

    readNext();
    if (!expectEl("c:chartSpace")) {
        return KoFilter::WrongFormat;
    }

    delete d->m_chart;
    d->m_chart = new Charting::Chart;

    while (!atEnd()) {
        QXmlStreamReader::TokenType tokenType = readNext();
        if(tokenType == QXmlStreamReader::Invalid || tokenType == QXmlStreamReader::EndDocument) break;
        if (isStartElement()) {
            TRY_READ_IF(plotArea)
            ELSE_TRY_READ_IF(ser)
            ELSE_TRY_READ_IF(legend)
            ELSE_TRY_READ_IF(pieChart)
            ELSE_TRY_READ_IF(firstSliceAng)
        }
    }

    ChartExport c(d->m_chart);
    c.m_href = QString("Chart%1").arg(++d->chartNumber);

    const QString sheetName = d->m_context->drawingReaderContext->worksheetReaderContext->worksheetName;

    //struct Position {
    //    int m_row, m_col, m_rowOff, m_colOff;
    //enum AnchorToken { NoAnchor, FromAnchor, ToAnchor };
    QMap<XlsxXmlDrawingReaderContext::AnchorType, XlsxXmlDrawingReaderContext::Position> positions = d->m_context->drawingReaderContext->m_positions;

    if(! sheetName.isEmpty()) {
        c.m_endCellAddress += sheetName + '.';
    }

    if(positions.contains(XlsxXmlDrawingReaderContext::FromAnchor)) {
        XlsxXmlDrawingReaderContext::Position pos = positions[XlsxXmlDrawingReaderContext::FromAnchor];
        c.m_endCellAddress += columnName(pos.m_col) + QString::number(pos.m_row);
        c.m_x = "0pt"; //QString::number(columnWidth(cell->sheet(),drawobj->m_colL,drawobj->m_dxL)) + "pt";
        c.m_y = "0pt"; //QString::number(rowHeight(cell->sheet(),drawobj->m_rwT,drawobj->m_dyT)) + "pt";
        c.m_width = "100pt"; //QString::number(columnWidth(cell->sheet(),drawobj->m_colR-drawobj->m_colL,drawobj->m_dxR)) + "pt";
        c.m_height = "100pt"; //QString::number(rowHeight(cell->sheet(),drawobj->m_rwB-drawobj->m_rwT,drawobj->m_dyB)) + "pt";
    }

    c.m_notifyOnUpdateOfRanges = "Sheet1.D2:Sheet1.F2";
    c.m_cellRangeAddress = "Sheet1.D2:Sheet1.F2";//string(cell->sheet()->name()) + "." + columnName(chart->m_chart->m_cellRangeAddress.left()) + QString::number(chart->m_chart->m_cellRangeAddress.top()) + ":" +
    //string(cell->sheet()->name()) + "." + columnName(chart->m_chart->m_cellRangeAddress.right()) + QString::number(chart->m_chart->m_cellRangeAddress.bottom());

    Q_ASSERT(body);
    c.saveIndex(body);//xmlWriter);

    Q_ASSERT(d->m_context->drawingReaderContext->worksheetReaderContext->import);
    KoStore* storeout = d->m_context->drawingReaderContext->worksheetReaderContext->import->outputStore();
    Q_ASSERT(storeout);
    c.saveContent(storeout, manifest);

//Q_ASSERT(false);

    delete d->m_chart;
    d->m_chart = 0;
    d->m_context = 0;
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL plotArea
KoFilter::ConversionStatus XlsxXmlChartReader::read_plotArea()
{
    //TODO
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL ser
KoFilter::ConversionStatus XlsxXmlChartReader::read_ser()
{
    //TODO
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL legend
KoFilter::ConversionStatus XlsxXmlChartReader::read_legend()
{
    //TODO
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL pieChart
KoFilter::ConversionStatus XlsxXmlChartReader::read_pieChart()
{
    if(!d->m_chart->m_impl) {
        d->m_chart->m_impl = new Charting::PieImpl();
    }
    return KoFilter::OK;
}

KoFilter::ConversionStatus XlsxXmlChartReader::read_firstSliceAng()
{
    if(Charting::PieImpl* pie = dynamic_cast<Charting::PieImpl*>(d->m_chart->m_impl)) {
        const QXmlStreamAttributes attrs(attributes());
        TRY_READ_ATTR(val)
        pie->m_anStart = val.toInt(); // default value is zero
    }
    return KoFilter::OK;
}
