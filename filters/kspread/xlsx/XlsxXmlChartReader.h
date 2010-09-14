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

#ifndef XLSXXMLCHARTREADER_H
#define XLSXXMLCHARTREADER_H

#include <MsooXmlCommonReader.h>

namespace Charting {
    class Series;
    class Chart;
}

class ChartExport;

class XlsxXmlChartReaderContext : public MSOOXML::MsooXmlReaderContext
{
public:
    KoStore* m_storeout;
    Charting::Chart* m_chart;
    ChartExport* m_chartExport;
    explicit XlsxXmlChartReaderContext(KoStore* _storeout, ChartExport* _chartExport);
    virtual ~XlsxXmlChartReaderContext();
};

class XlsxXmlChartReader : public MSOOXML::MsooXmlCommonReader
{
public:
    XlsxXmlChartReader(KoOdfWriters *writers);
    virtual ~XlsxXmlChartReader();
    virtual KoFilter::ConversionStatus read(MSOOXML::MsooXmlReaderContext* context = 0);
    void WriteIntoInternalTable(QString &range, QVector< QString > &buffer, QString format);
    QString AlocateAndWriteIntoInternalTable(QVector< QString > &buffer, QString format);
protected:
    KoFilter::ConversionStatus read_plotArea();
    KoFilter::ConversionStatus read_valAx();
    KoFilter::ConversionStatus read_catAx();
    KoFilter::ConversionStatus read_title();
    KoFilter::ConversionStatus read_legend();
    KoFilter::ConversionStatus read_spPr();
    KoFilter::ConversionStatus read_dLbl();
    KoFilter::ConversionStatus read_dLbls();

    KoFilter::ConversionStatus read_pieChart();
    KoFilter::ConversionStatus read_pie3DChart();
    KoFilter::ConversionStatus read_doughnutChart();
    KoFilter::ConversionStatus read_areaChart();
    KoFilter::ConversionStatus read_area3DChart();
    KoFilter::ConversionStatus read_barChart();
    KoFilter::ConversionStatus read_bar3DChart();
    KoFilter::ConversionStatus read_lineChart();
    KoFilter::ConversionStatus read_line3DChart();
    KoFilter::ConversionStatus read_scatterChart();
    KoFilter::ConversionStatus read_radarChart();
    KoFilter::ConversionStatus read_surfaceChart();
    KoFilter::ConversionStatus read_surface3DChart();
    KoFilter::ConversionStatus read_bubbleChart();
    KoFilter::ConversionStatus read_stockChart();

    KoFilter::ConversionStatus read_pieChart_Ser();
    KoFilter::ConversionStatus read_bubbleChart_Ser();
    KoFilter::ConversionStatus read_scatterChart_Ser();
    KoFilter::ConversionStatus read_barChart_Ser();
    KoFilter::ConversionStatus read_areaChart_Ser();
    KoFilter::ConversionStatus read_radarChart_Ser();
    KoFilter::ConversionStatus read_lineChart_Ser();
    KoFilter::ConversionStatus read_surfaceChart_Ser();

    KoFilter::ConversionStatus read_barDir();
    KoFilter::ConversionStatus read_grouping();
    KoFilter::ConversionStatus read_val();
    KoFilter::ConversionStatus read_xVal();
    KoFilter::ConversionStatus read_yVal();
    KoFilter::ConversionStatus read_cat();
    KoFilter::ConversionStatus read_seriesText_Tx();
    KoFilter::ConversionStatus read_chartText_Tx();
    KoFilter::ConversionStatus read_numCache();
    KoFilter::ConversionStatus read_firstSliceAng();
    KoFilter::ConversionStatus read_holeSize();
    KoFilter::ConversionStatus read_bubbleSize();
    KoFilter::ConversionStatus read_bubbleScale();
    KoFilter::ConversionStatus read_bubble3D();

    KoFilter::ConversionStatus read_pt();
    KoFilter::ConversionStatus read_order();
    KoFilter::ConversionStatus read_idx();
    KoFilter::ConversionStatus read_explosion();
    KoFilter::ConversionStatus read_strRef();
    KoFilter::ConversionStatus read_numRef();
    KoFilter::ConversionStatus read_f();
    KoFilter::ConversionStatus read_ptCount();
    KoFilter::ConversionStatus read_numLit();
    KoFilter::ConversionStatus read_strCache();

private:

    enum ReadTxContext{ Title, None };
    enum ReadAreaContext{ PlotArea, ChartArea };
    XlsxXmlChartReaderContext *m_context;
    Charting::Series *m_currentSeries;
    QString m_cellRangeAddress;
    bool m_autoTitleDeleted;    
    ReadTxContext m_readTxContext;
    ReadAreaContext m_areaContext;

    class Private;
    Private * const d;
};

#endif
