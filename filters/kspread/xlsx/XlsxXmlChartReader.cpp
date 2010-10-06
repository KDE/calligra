/*
 * This file is part of Office 2007 Filters for KOffice
 *
 * Copyright (C) 2010 Sebastian Sauer <sebsauer@kdab.com>
 * Copyright (c) 2010 Carlos Licea <carlos@kdab.com>
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

#include "Charting.h"
#include "ChartExport.h"
#include "XlsxUtils.h"

#define MSOOXML_CURRENT_NS "c"
#define MSOOXML_CURRENT_CLASS XlsxXmlChartReader
#define BIND_READ_CLASS MSOOXML_CURRENT_CLASS

#include <MsooXmlReader_p.h>
#include <MsooXmlUtils.h>

#include <QFontMetricsF>

class SpPr {
public:
    SpPr() {}
};

class NumCache {
public:
    int m_ptCount;
    QVector< QString > m_cache;
    NumCache() : m_ptCount(0) {}
};

class StrCache {
public:
    int m_ptCount;
    QVector< QString >  m_cache;
    StrCache() : m_ptCount(0) {}
};

class NumRef {
public:
    QString m_f;
    NumCache m_numCache;
};

class NumLit {
public:
    int m_ptCount;
    QVector< QString > m_cache;
    NumLit() : m_ptCount(0) {}
};

class StrLit {
public:
    int m_ptCount;
    QVector< QString > m_cache;
    StrLit() : m_ptCount(0) {}
};

class StrRef {
public:
    QString m_f;
    StrCache m_strCache;
};

class Tx {
public:
    StrRef m_strRef;

    QString writeRefToInternalTable(XlsxXmlChartReader *chartReader);
};

QString Tx::writeRefToInternalTable(XlsxXmlChartReader *chartReader)
{
    chartReader->WriteIntoInternalTable(m_strRef.m_f,m_strRef.m_strCache.m_cache,"string");
    return m_strRef.m_f;
}

class Cat {
public:
    NumRef m_numRef;
    StrRef m_strRef;
    NumLit m_numLit;
    StrLit m_strLit;

    QString writeRefToInternalTable(XlsxXmlChartReader *chartReader);
    QString writeLitToInternalTable(XlsxXmlChartReader *chartReader);
};

QString Cat::writeRefToInternalTable(XlsxXmlChartReader *chartReader)
{
    if (m_numRef.m_numCache.m_ptCount != 0) {
        chartReader->WriteIntoInternalTable(m_numRef.m_f,m_numRef.m_numCache.m_cache,"float");
        return m_numRef.m_f;
    }

    chartReader->WriteIntoInternalTable(m_strRef.m_f,m_strRef.m_strCache.m_cache,"string");
    return m_strRef.m_f;
}

QString Cat::writeLitToInternalTable(XlsxXmlChartReader *chartReader)
{
    if (m_numLit.m_ptCount != 0) {
        return chartReader->AlocateAndWriteIntoInternalTable(m_numLit.m_cache,"float");
    }

    return chartReader->AlocateAndWriteIntoInternalTable(m_strLit.m_cache,"string");
}

class Val {
public:
    NumRef m_numRef;
    NumLit m_numLit;

    QString writeRefToInternalTable(XlsxXmlChartReader *chartReader);
    QString writeLitToInternalTable(XlsxXmlChartReader *chartReader);
};

QString Val::writeRefToInternalTable(XlsxXmlChartReader *chartReader)
{
    chartReader->WriteIntoInternalTable(m_numRef.m_f,m_numRef.m_numCache.m_cache,"float");
    return m_numRef.m_f;
}

QString Val::writeLitToInternalTable(XlsxXmlChartReader *chartReader)
{
    return chartReader->AlocateAndWriteIntoInternalTable(m_numLit.m_cache,"float");
}

class XVal {
public:
    NumRef m_numRef;
    StrRef m_strRef;
    NumLit m_numLit;
    StrLit m_strLit;

    QString writeRefToInternalTable(XlsxXmlChartReader *chartReader);
    QString writeLitToInternalTable(XlsxXmlChartReader *chartReader);
};

QString XVal::writeRefToInternalTable(XlsxXmlChartReader *chartReader)
{
    if (m_numRef.m_numCache.m_ptCount != 0) {
        chartReader->WriteIntoInternalTable(m_numRef.m_f,m_numRef.m_numCache.m_cache,"float");
        return m_numRef.m_f;
    }

    chartReader->WriteIntoInternalTable(m_strRef.m_f,m_strRef.m_strCache.m_cache,"string");
    return m_strRef.m_f;
}

QString XVal::writeLitToInternalTable(XlsxXmlChartReader *chartReader)
{
    if (m_numLit.m_ptCount != 0) {
        return chartReader->AlocateAndWriteIntoInternalTable(m_numLit.m_cache,"float");
    }

    return chartReader->AlocateAndWriteIntoInternalTable(m_strLit.m_cache,"string");
}


class YVal {
public:
    NumRef m_numRef;
    NumLit m_numLit;

    QString writeRefToInternalTable(XlsxXmlChartReader *chartReader);
    QString writeLitToInternalTable(XlsxXmlChartReader *chartReader);
};

QString YVal::writeRefToInternalTable(XlsxXmlChartReader *chartReader)
{
    chartReader->WriteIntoInternalTable(m_numRef.m_f,m_numRef.m_numCache.m_cache,"float");
    return m_numRef.m_f;
}

QString YVal::writeLitToInternalTable(XlsxXmlChartReader *chartReader)
{
    return chartReader->AlocateAndWriteIntoInternalTable(m_numLit.m_cache,"float");
}


class BubbleSize {
public:
    NumRef m_numRef;
    NumLit m_numLit;

    QString writeRefToInternalTable(XlsxXmlChartReader *chartReader);
    QString writeLitToInternalTable(XlsxXmlChartReader *chartReader);
};

QString BubbleSize::writeRefToInternalTable(XlsxXmlChartReader *chartReader)
{
    chartReader->WriteIntoInternalTable(m_numRef.m_f,m_numRef.m_numCache.m_cache,"float");
    return m_numRef.m_f;
}

QString BubbleSize::writeLitToInternalTable(XlsxXmlChartReader *chartReader)
{
    return chartReader->AlocateAndWriteIntoInternalTable(m_numLit.m_cache,"float");
}

class Ser
{
};

class ValSeries : public Ser
{
public:
    int m_idx;
    int m_order;
    Tx  m_tx;
    Cat m_cat;
    Val m_val;
    ValSeries() : m_idx(0), m_order(0) {}
};

class BubbleSeries : public Ser
{
public:
    int m_idx;
    int m_order;
    Tx  m_tx;
    XVal m_xVal;
    YVal m_yVal;
    BubbleSize m_bubbleSize;
    BubbleSeries() : m_idx(0), m_order(0) {}
};

class ScatterSeries : public Ser
{
public:
    int m_idx;
    int m_order;
    Tx  m_tx;
    XVal m_xVal;
    YVal m_yVal;
    SpPr m_spPr;
    ScatterSeries() : m_idx(0), m_order(0) {}
};

class LineSeries :public ValSeries
{
public:
    LineSeries() {}
};

class PieSeries :public ValSeries
{
public:
    int m_explosion;
    PieSeries() : m_explosion(0) {}
};

class BarSeries :public ValSeries
{
public:
    BarSeries() {}
};

class AreaSeries :public ValSeries
{
public:
    AreaSeries() {}
};

class RadarSeries :public ValSeries
{
public:
    RadarSeries() {}
};

class SurfaceSeries :public ValSeries
{
public:
    SurfaceSeries() {}
};

class XlsxXmlChartReader::Private
{
public:
    Private();
    QList<Ser*> m_seriesData;
    QVariant::Type m_currentType;
    int *m_currentIdx;
    int *m_currentOrder;
    int *m_currentExplosion;
    Tx  *m_currentTx;
    Cat *m_currentCat;
    Val *m_currentVal;
    StrRef *m_currentStrRef;
    QString *m_currentF;
    StrCache *m_currentStrCache;
    int *m_currentPtCount;
    QVector< QString >  *m_currentPtCache;
    NumRef *m_currentNumRef;
    NumLit *m_currentNumLit;
    NumCache *m_currentNumCache;
    XVal *m_currentXVal;
    YVal *m_currentYVal;
    BubbleSize *m_currentBubbleSize;
    int m_numReadSeries;
};

XlsxXmlChartReader::Private::Private ( )
    : m_numReadSeries( 0 )
{
    qDeleteAll(m_seriesData);
}

// calculates the column width in pixels
int columnWidth(unsigned long col, unsigned long dx = 0, qreal defaultColumnWidth = 8.43) {
    QFont font("Arial", 10);
    QFontMetricsF fm(font);
    const qreal characterWidth = fm.width("h");
    defaultColumnWidth *= characterWidth;
    return (defaultColumnWidth * col) + (dx / 1024.0 * defaultColumnWidth);
}

// calculates the row height in pixels
int rowHeight(unsigned long row, unsigned long dy = 0, qreal defaultRowHeight = 12.75)
{
    return defaultRowHeight * row + dy;
}

// Returns A for 1, B for 2, C for 3, etc.
QString columnName(uint column)
{
    QString s;
    column = column - 1;
    unsigned digits = 1;
    unsigned offset = 0;
    for (unsigned limit = 26; column >= limit + offset; limit *= 26, digits++)
        offset += limit;
    for (unsigned col = column - offset; digits; --digits, col /= 26)
        s.prepend(QChar('A' + (col % 26)));
    return s;
}



XlsxXmlChartReaderContext::XlsxXmlChartReaderContext(KoStore* _storeout, ChartExport* _chartExport)
    : MSOOXML::MsooXmlReaderContext()
    , m_storeout(_storeout)
    , m_chart(_chartExport->chart())
    , m_chartExport(_chartExport)
{
}

XlsxXmlChartReaderContext::~XlsxXmlChartReaderContext()
{
    delete m_chart;
    delete m_chartExport;
}

XlsxXmlChartReader::XlsxXmlChartReader(KoOdfWriters *writers)
    : MSOOXML::MsooXmlCommonReader(writers)
    , m_context(0)
    , m_currentSeries(0)
    , m_autoTitleDeleted(false)
    , m_readTxContext( None )
    , m_areaContext( ChartArea )
    , d ( new Private( ) )
{
}

XlsxXmlChartReader::~XlsxXmlChartReader()
{
     delete d;
}

//! chart (Chart)
/*! ECMA-376, 21.2.2.29, p.3768.

 Parent elements:
  - chartSpace (§21.2.2.29)

 Child elements:
  - [Done]autoTitleDeleted (Auto Title Is Deleted) §21.2.2.7
  - backWall (Back Wall) §21.2.2.11
  - dispBlanksAs (Display Blanks As) §21.2.2.42
  - extLst (Chart Extensibility) §21.2.2.64
  - floor (Floor) §21.2.2.69
  - [Done]legend (Legend) §21.2.2.93
  - pivotFmts (Pivot Formats) §21.2.2.143
  - [Done]plotArea (Plot Area) §21.2.2.145
  - plotVisOnly (Plot Visible Only) §21.2.2.146
  - showDLblsOverMax (Show Data Labels over Maximum) §21.2.2.180
  - sideWall (Side Wall) §21.2.2.191
  - [Done]title (Title) §21.2.2.210
  - view3D (View In 3D) §21.2.2.228
*/
KoFilter::ConversionStatus XlsxXmlChartReader::read(MSOOXML::MsooXmlReaderContext* context)
{
    m_context = dynamic_cast<XlsxXmlChartReaderContext*>(context);
    Q_ASSERT(m_context);

    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }

    readNext();
    if (!expectEl("c:chartSpace")) {
        return KoFilter::WrongFormat;
    }

    while (!atEnd()) {
        QXmlStreamReader::TokenType tokenType = readNext();
        if(tokenType == QXmlStreamReader::Invalid || tokenType == QXmlStreamReader::EndDocument) break;
        if (isStartElement()) {
            m_areaContext = ChartArea;
            TRY_READ_IF(plotArea)
            ELSE_TRY_READ_IF(title)
            ELSE_TRY_READ_IF(legend)
            ELSE_TRY_READ_IF(spPr)
            if (qualifiedName() == QLatin1String(QUALIFIED_NAME(autoTitleDeleted))) {
                const QXmlStreamAttributes attrs(attributes());
                TRY_READ_ATTR_WITHOUT_NS(val)
                m_autoTitleDeleted = val.toInt();
            }
           if (qualifiedName() == QLatin1String(QUALIFIED_NAME(style))) {
               const QXmlStreamAttributes attrs(attributes());
               TRY_READ_ATTR_WITHOUT_NS(val)
               m_context->m_chart->m_style = val.toInt();
           }
        }
    }

    // static is fine here cause we only need to take care that that number is unique in the
    // exported ODS file and do not take if the number is continuous or whatever.
    static int chartNumber = 0;
    m_context->m_chartExport->m_href = QString("Chart%1").arg(++chartNumber);

    Charting::Chart* c = m_context->m_chart;
    if(!c->m_sheetName.isEmpty()) {
        m_context->m_chartExport->m_endCellAddress += c->m_sheetName + '.';
    }
    if(c->m_fromColumn > 0 && c->m_fromRow > 0) {
        m_context->m_chartExport->m_endCellAddress += columnName(c->m_fromColumn) + QString::number(c->m_fromRow);
        m_context->m_chartExport->m_x = columnWidth(c->m_fromColumn-1, 0 /*f.m_colOff*/);
        m_context->m_chartExport->m_y = rowHeight(c->m_fromRow-1, 0 /*f.m_rowOff*/);
        if(c->m_toColumn > 0 && c->m_toRow > 0) {
            m_context->m_chartExport->m_width = columnWidth( c->m_toColumn - c->m_fromColumn - 1, 0 /*t.m_colOff*/);
            m_context->m_chartExport->m_height = rowHeight( c->m_toRow - c->m_fromRow - 1, 0 /*t.m_rowOff*/);
        }
    }

    if (!c->m_cellRangeAddress.isNull() ) {
        m_context->m_chartExport->m_cellRangeAddress.clear();
        if (!c->m_sheetName.isEmpty())
            m_context->m_chartExport->m_cellRangeAddress += c->m_sheetName + '.';
        m_context->m_chartExport->m_cellRangeAddress += columnName(c->m_cellRangeAddress.left()) + QString::number(c->m_cellRangeAddress.top()) + ":" +
                                                        columnName(c->m_cellRangeAddress.right()) + QString::number(c->m_cellRangeAddress.bottom());
    }

    if (m_currentSeries) {
        m_context->m_chartExport->m_notifyOnUpdateOfRanges = m_currentSeries->m_valuesCellRangeAddress; //m_cellRangeAddress
    }
    
    // the index will by written by the XlsxXmlWorksheetReader
    //m_context->m_chartExport->saveIndex(body);

    // write the embedded object file
    m_context->m_chartExport->saveContent(m_context->m_storeout, manifest);
    
    m_context = 0;
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL valAx
KoFilter::ConversionStatus XlsxXmlChartReader::read_valAx()
{
    READ_PROLOGUE
    Charting::Axis* axis = new Charting::Axis( Charting::Axis::VerticalValueAxis );

    m_context->m_chart->m_axes.push_back( axis );
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            if ( qualifiedName() == QLatin1String( QUALIFIED_NAME(axPos) ) ) {
                  const QXmlStreamAttributes attrs(attributes());
                  TRY_READ_ATTR_WITHOUT_NS(val)
                  if ( val == QLatin1String( "b" ) ){
                      axis->m_type = Charting::Axis::HorizontalValueAxis;
                  }
//                   else if ( val == QLatin1String( "l" ) ){
//                   }
            }
        }
        else if ( qualifiedName() == QLatin1String( QUALIFIED_NAME(majorGridlines) ) ) {
            axis->m_majorGridlines = Charting::Axis::Gridline( Charting::LineFormat( Charting::LineFormat::Solid ) );
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL catAx
KoFilter::ConversionStatus XlsxXmlChartReader::read_catAx()
{
    READ_PROLOGUE
    Charting::Axis* axis = new Charting::Axis( Charting::Axis::VerticalValueAxis );
    m_context->m_chart->m_axes.push_back( axis );
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            if ( qualifiedName() == QLatin1String( QUALIFIED_NAME(axPos) ) ) {
                  const QXmlStreamAttributes attrs(attributes());
                  TRY_READ_ATTR_WITHOUT_NS(val)
                  if ( val == QLatin1String( "b" ) ){
                      axis->m_type = Charting::Axis::HorizontalValueAxis;
                  }
//                   else if ( val == QLatin1String( "l" ) ){
//                   }
            }
            else if ( qualifiedName() == QLatin1String( QUALIFIED_NAME(majorGridlines) ) ) {
                  axis->m_majorGridlines = Charting::Axis::Gridline( Charting::LineFormat( Charting::LineFormat::Solid ) );
            }
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL plotArea
//! plotArea (Plot Area)
/*! ECMA-376, 21.2.2.145, p.3828.

 Parent elements:
  - chart (§21.2.2.27)

 Child elements:
  - [Done]area3DChart (3D Area Charts) §21.2.2.4
  - [Done]areaChart (Area Charts) §21.2.2.5
  - [Done]bar3DChart (3D Bar Charts) §21.2.2.15
  - [Done]barChart (Bar Charts) §21.2.2.16
  - [Done]bubbleChart (Bubble Charts) §21.2.2.20
  - [Done]catAx (Category Axis Data) §21.2.2.25
  - dateAx (Date Axis) §21.2.2.39
  - [Done]doughnutChart (Doughnut Charts) §21.2.2.50
  - dTable (Data Table) §21.2.2.54
  - extLst (Chart Extensibility) §21.2.2.64
  - layout (Layout) §21.2.2.88
  - [Done]line3DChart (3D Line Charts) §21.2.2.96
  - [Done]lineChart (Line Charts) §21.2.2.97
  - ofPieChart (Pie of Pie or Bar of Pie Charts) §21.2.2.126
  - [Done]pie3DChart (3D Pie Charts) §21.2.2.140
  - [Done]pieChart (Pie Charts) §21.2.2.141
  - [Done]radarChart (Radar Charts) §21.2.2.153
  - [Done]scatterChart (Scatter Charts) §21.2.2.161
  - serAx (Series Axis) §21.2.2.175
  - [Done]spPr (Shape Properties) §21.2.2.197
  - [Done]stockChart (Stock Charts) §21.2.2.198
  - [Done]surface3DChart (3D Surface Charts) §21.2.2.203
  - [Done]surfaceChart (Surface Charts) §21.2.2.204
  - [Done]valAx (Value Axis) §21.2.2.226
*/
KoFilter::ConversionStatus XlsxXmlChartReader::read_plotArea()
{
    m_areaContext = PlotArea;
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(spPr)
            ELSE_TRY_READ_IF(valAx)
            ELSE_TRY_READ_IF(catAx)
            ELSE_TRY_READ_IF(pieChart)
            ELSE_TRY_READ_IF(pie3DChart)
            ELSE_TRY_READ_IF(doughnutChart)
            ELSE_TRY_READ_IF(areaChart)
            ELSE_TRY_READ_IF(area3DChart)
            ELSE_TRY_READ_IF(barChart)
            ELSE_TRY_READ_IF(bar3DChart)
            ELSE_TRY_READ_IF(lineChart)
            ELSE_TRY_READ_IF(line3DChart)
            ELSE_TRY_READ_IF(scatterChart)
            ELSE_TRY_READ_IF(radarChart)
            ELSE_TRY_READ_IF(surfaceChart)
            ELSE_TRY_READ_IF(surface3DChart)
            ELSE_TRY_READ_IF(bubbleChart)
            ELSE_TRY_READ_IF(stockChart)
        }
    }
    READ_EPILOGUE
    m_areaContext = ChartArea;
}

#undef CURRENT_EL
#define CURRENT_EL title
/*! Read the horizontal value. */
KoFilter::ConversionStatus XlsxXmlChartReader::read_title()
{
    m_readTxContext = Title;
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            if(!m_autoTitleDeleted)
                if (QUALIFIED_NAME_IS(tx)) {
                    TRY_READ(chartText_Tx)
                }
        }
    }
    if ( m_context->m_chart->m_title.isEmpty() )
        m_context->m_chart->m_title = "Chart Title";
    m_readTxContext = None;
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL val
//! val (Values)
/*! ECMA-376, 21.2.2.224, p.3867.

 Parent elements:
  - ser §21.2.2.168
  - ser §21.2.2.170
  - ser §21.2.2.174
  - ser §21.2.2.171
  - ser §21.2.2.172
  - ser §21.2.2.169
  - ser §21.2.2.167
  - ser §21.2.2.173

 Child elements:
  - numLit (Number Literal) §21.2.2.122
  - [Done]numRef (Number Reference) §21.2.2.123
*/
KoFilter::ConversionStatus XlsxXmlChartReader::read_val()
{
    READ_PROLOGUE
    d->m_currentNumRef = &d->m_currentVal->m_numRef;
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(numRef)
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL xVal
//! xVal (X Values)
/*! ECMA-376, 21.2.2.234, p.3872.

 Parent elements:
  - ser §21.2.2.174
  - ser §21.2.2.167

 Child elements:
  - multiLvlStrRef (Multi Level String Reference) §21.2.2.115
  - numLit (Number Literal) §21.2.2.122
  - [Done]numRef (Number Reference) §21.2.2.123
  - strLit (String Literal) §21.2.2.200
  - [Done]strRef (String Reference) §21.2.2.201
*/
KoFilter::ConversionStatus XlsxXmlChartReader::read_xVal()
{
    READ_PROLOGUE
    d->m_currentNumRef = &d->m_currentXVal->m_numRef;
    d->m_currentStrRef = &d->m_currentXVal->m_strRef;
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(numRef)
            ELSE_TRY_READ_IF(strRef)
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL yVal
//! yVal (Y Values)
/*! ECMA-376, 21.2.2.237, p.3873.

 Parent elements:
  - ser §21.2.2.174
  - ser §21.2.2.167

 Child elements:
  - numLit (Number Literal) §21.2.2.122
  - numRef (Number Reference) §21.2.2.123
*/
KoFilter::ConversionStatus XlsxXmlChartReader::read_yVal()
{
    READ_PROLOGUE
    d->m_currentNumRef = &d->m_currentYVal->m_numRef;
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(numRef)
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL cat
//! cat (Category Axis Data)
/*! ECMA-376, 21.2.2.24, p.3766.

 Parent elements:
  - ser §21.2.2.168
  - ser §21.2.2.170
  - ser §21.2.2.174
  - ser §21.2.2.171
  - ser §21.2.2.172
  - ser §21.2.2.169
  - ser §21.2.2.167
  - ser §21.2.2.173

 Child elements:
  - multiLvlStrRef (Multi Level String Reference) §21.2.2.115
  - numLit (Number Literal) §21.2.2.122
  - [Done]numRef (Number Reference) §21.2.2.123
  - strLit (String Literal) §21.2.2.200
  - strRef (String Reference) §21.2.2.201
*/
KoFilter::ConversionStatus XlsxXmlChartReader::read_cat()
{
    READ_PROLOGUE
    d->m_currentStrRef = &d->m_currentCat->m_strRef;
    d->m_currentNumRef = &d->m_currentCat->m_numRef;
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(strRef)
            ELSE_TRY_READ_IF(numRef)
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL tx
//! tx (Chart Text)
/*! ECMA-376, 21.2.2.215, p.3863.

 Parent elements:
  - dispUnitsLbl (§21.2.2.46)
  - dLbl (§21.2.2.47)
  - title (§21.2.2.210)
  - trendlineLbl (§21.2.2.212)

 Child elements:
  - rich (Rich Text) §21.2.2.156
  - strRef (String Reference) §21.2.2.201
*/

/*! This element specifies text to use on a chart, including rich text formatting. */
KoFilter::ConversionStatus XlsxXmlChartReader::read_chartText_Tx()
{
    READ_PROLOGUE2(chartText_Tx)
    enum { Start, InStrRef, InRichText } state;
    state = Start;
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        switch(state) {
            case Start:
                if (qualifiedName() == QLatin1String(QUALIFIED_NAME(strRef)))
                    state = isStartElement() ? InStrRef : Start;
                else if (qualifiedName() == QLatin1String(QUALIFIED_NAME(rich)))
                    state = isStartElement() ? InRichText : Start;
                break;
            case InStrRef: // plaintext within a series
//                if (isStartElement() && !m_currentSeriesData->m_datasetValue.contains(Charting::Value::SeriesLegendOrTrendlineName)) {
//                    if (qualifiedName() == QLatin1String(QUALIFIED_NAME(f))) {
//                        Charting::Value* v = new Charting::Value(Charting::Value::SeriesLegendOrTrendlineName, Charting::Value::CellRange, readElementText());
//                        m_currentSeriesData->m_datasetValue[v->m_dataId] = v;
//                    } else if (qualifiedName() == QLatin1String(QUALIFIED_NAME(v))) {
//                        Charting::Value* v = new Charting::Value(Charting::Value::SeriesLegendOrTrendlineName, Charting::Value::TextOrValue, readElementText());
//                        m_currentSeriesData->m_datasetValue[v->m_dataId] = v;
//                    }
//                }
                break;
            case InRichText: // richtext means the title text
                // we extract the text from the richtext cause we cannot handle the richtext formattings anyway
                QString result;
                enum { Rich, Paragraph, TextRun } s;
                s = Rich;
                while (!atEnd()) {
                    readNext();
                    switch(s) {
                        case Rich:
                            if (isStartElement() && qualifiedName() == QLatin1String("a:p")) s = Paragraph;
                            break;
                        case Paragraph:
                            if (qualifiedName() == QLatin1String("a:r")) // text run
                            s = isStartElement() ? TextRun : Rich;
                        break;
                        case TextRun:
                            if (qualifiedName() == QLatin1String("a:t")) {
                                if(isStartElement()) {
                                    if(!result.isEmpty()) result += ' '; //concat multiple strings into one result
                                    const QString text = readElementText();
                                    result += text;
                                    m_context->m_chart->m_title = text;
                                } else
                                    s = Paragraph;
                            }
                            break;
                    }
                    BREAK_IF_END_OF(rich);
                }
                if(!result.isEmpty())
                    m_context->m_chart->m_texts << new Charting::Text(result);
                state = Start;
                break;
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL tx
//! tx (Series Text)
/*! ECMA-376, 21.2.2.215, p.3863.

 Parent elements:
  - ser §21.2.2.168
  - ser §21.2.2.170
  - ser §21.2.2.174
  - ser §21.2.2.171
  - ser §21.2.2.172
  - ser §21.2.2.169
  - ser §21.2.2.167
  - ser §21.2.2.173

 Child elements:
  - [Done]strRef (String Reference) §21.2.2.201
  - v (Text Value) §21.2.2.223
*/

/*! This element specifies text to use on a chart, including rich text formatting. */
KoFilter::ConversionStatus XlsxXmlChartReader::read_seriesText_Tx()
{
    READ_PROLOGUE2(seriesText_Tx)

    d->m_currentStrRef = &d->m_currentTx->m_strRef;
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(strRef)
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL numCache
//! numCache (Number Cache)
/*! ECMA-376, 21.2.2.120, p.3813.

 Parent elements:
 - numRef (§21.2.2.123)

 Child elements:
  - extLst (Chart Extensibility) §21.2.2.64
  - formatCode (Format Code) §21.2.2.71
  - [Done]pt (Numeric Point) §21.2.2.150
  - [Done]ptCount (Point Count) §21.2.2.152

*/
KoFilter::ConversionStatus XlsxXmlChartReader::read_numCache()
{
    READ_PROLOGUE

    d->m_currentPtCount = &d->m_currentNumCache->m_ptCount;
    d->m_currentPtCache = &d->m_currentNumCache->m_cache;

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(ptCount)
            ELSE_TRY_READ_IF(pt)
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL legend
KoFilter::ConversionStatus XlsxXmlChartReader::read_legend()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        //TODO
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL dLbl
//! dLbl (Data Label)
/*! ECMA-376, 21.2.2.47, p.3780.

 Parent elements:
 - dLbls (§21.2.2.49)
 - pivotFmt (§21.2.2.142)

 Child elements:
  - delete (Delete) §21.2.2.40
  - dLblPos (Data Label Position) §21.2.2.48
  - extLst (Chart Extensibility) §21.2.2.64
  - idx (Index) §21.2.2.84
  - layout (Layout) §21.2.2.88
  - numFmt (Number Format) §21.2.2.121
  - separator (Separator) §21.2.2.166
  - showBubbleSize (Show Bubble Size) §21.2.2.178
  - showCatName (Show Category Name) §21.2.2.179
  - showLegendKey (Show Legend Key) §21.2.2.184
  - showPercent (Show Percent) §21.2.2.187
  - showSerName (Show Series Name) §21.2.2.188
  - [Done]showVal (Show Value) §21.2.2.189
  - spPr (Shape Properties) §21.2.2.197
  - tx (Chart Text) §21.2.2.214
  - txPr (Text Properties) §21.2.2.216

*/
KoFilter::ConversionStatus XlsxXmlChartReader::read_dLbl()
{
    READ_PROLOGUE
      while (!atEnd()) {
          readNext();
          BREAK_IF_END_OF(CURRENT_EL);
          if (isStartElement()) {
              if ( qualifiedName() == "c:showVal" ) {
                  m_currentSeries->m_showDataValues = true;
              }
          }
      }
      READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL dLbls
//! dLbls (Data Labels)
/*! ECMA-376, 21.2.2.49, p.3781.

 Parent elements:
 - area3DChart (§21.2.2.4)
 - areaChart (§21.2.2.5)
 - bar3DChart (§21.2.2.15)
 - barChart (§21.2.2.16)
 - bubbleChart (§21.2.2.20)
 - doughnutChart (§21.2.2.50)
 - line3DChart (§21.2.2.96)
 - lineChart (§21.2.2.97)
 - ofPieChart (§21.2.2.126)
 - pie3DChart (§21.2.2.140)
 - pieChart (§21.2.2.141)
 - radarChart (§21.2.2.153)
 - scatterChart (§21.2.2.161)
 - ser (§21.2.2.168)
 - ser (§21.2.2.170)
 - ser (§21.2.2.174)
 - ser (§21.2.2.171)
 - ser (§21.2.2.172)
 - ser (§21.2.2.169)
 - ser (§21.2.2.167)
 - stockChart (§21.2.2.198)

 Child elements:
  - delete (Delete) §21.2.2.40
  - [Done]dLbl (Data Label) §21.2.2.47
  - dLblPos (Data Label Position) §21.2.2.48
  - extLst (Chart Extensibility) §21.2.2.64
  - leaderLines (Leader Lines) §21.2.2.92
  - numFmt (Number Format) §21.2.2.121
  - separator (Separator) §21.2.2.166
  - showBubbleSize (Show Bubble Size) §21.2.2.178
  - showCatName (Show Category Name) §21.2.2.179
  - showLeaderLines (Show Leader Lines) §21.2.2.183
  - showLegendKey (Show Legend Key) §21.2.2.184
  - showPercent (Show Percent) §21.2.2.187
  - showSerName (Show Series Name) §21.2.2.188
  - [Done]showVal (Show Value) §21.2.2.189
  - spPr (Shape Properties) §21.2.2.197
  - txPr (Text Properties) §21.2.2.216

*/
KoFilter::ConversionStatus XlsxXmlChartReader::read_dLbls()
{
    READ_PROLOGUE
      while (!atEnd()) {
          readNext();
          BREAK_IF_END_OF(CURRENT_EL);
          if (isStartElement()) {
              TRY_READ_IF(dLbl)
              if ( qualifiedName() == "c:showVal" ) {
                  m_currentSeries->m_showDataValues = true;
              }
          }
      }
      READ_EPILOGUE
}



#undef CURRENT_EL
#define CURRENT_EL spPr
// Visual shape properties that can be applied to a shape.
KoFilter::ConversionStatus XlsxXmlChartReader::read_spPr()
{
    enum State { Start, NoFill, InFill };
    State state = Start;
    READ_PROLOGUE
    int level = 0;
    bool readingGradient = false;
    bool readingGradientStop = false;
    Charting::Gradient* gradient = NULL;
    Charting::Gradient::GradientStop currentStop;
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if(isStartElement()) ++level;
        else if(isEndElement()) --level;

        if (qualifiedName() == "a:solidFill" || qualifiedName() == "a:pattFill" || qualifiedName() == "a:gradFill") {
            if (level == 1)
                state = isStartElement() ? InFill : Start;
        } else if (qualifiedName() == "a:noFill") {
            if (level == 1)
                state = isStartElement() ? NoFill : Start;
        } else if ((state == NoFill || state == InFill) && qualifiedName() == "a:srgbClr") {
            const QXmlStreamAttributes attrs(attributes());
            TRY_READ_ATTR_WITHOUT_NS(val)
            if(!val.isEmpty() && !m_context->m_chart->m_areaFormat) {
                if(!val.startsWith('#')) val.prepend('#');
                    if ( readingGradientStop )
                        currentStop.knownColorValue = QColor( val );
                    else
                        if ( m_areaContext == ChartArea )
                            m_context->m_chart->m_areaFormat = new Charting::AreaFormat(QColor(val), QColor(), state == InFill);
                        else
                            m_context->m_chart->m_plotAreaFillColor = QColor( val );
            }
            state = Start; // job done
        } else if ( qualifiedName() == "a:srgbClr" ) {
            if ( isStartElement() ) {
                const QXmlStreamAttributes attrs(attributes());
                TRY_READ_ATTR_WITHOUT_NS(val)
                if(!val.isEmpty() && !m_context->m_chart->m_areaFormat) {
                    if(!val.startsWith('#')) val.prepend('#');
                        if ( readingGradientStop )
                            currentStop.knownColorValue = QColor( val );
                        else
                            if ( m_areaContext == ChartArea )
                                m_context->m_chart->m_areaFormat = new Charting::AreaFormat(QColor(val), QColor(), state == InFill);
                            else
                                m_context->m_chart->m_plotAreaFillColor = QColor( val );
                }
            }
        } else if ( qualifiedName() == "a:alpha" ) {
            const QXmlStreamAttributes attrs(attributes());
            TRY_READ_ATTR_WITHOUT_NS(val)
                  if ( !val.isEmpty() )
                  {
                      if ( readingGradientStop )
                      {
                          currentStop.knownColorValue.setAlphaF( val.toDouble() / 100000.0 );
                      }else
                      {
                          if ( m_areaContext == ChartArea ) {
                              if (m_context->m_chart->m_areaFormat)
                                  m_context->m_chart->m_areaFormat->m_foreground.setAlphaF( val.toDouble() / 100000.0 );
                          } else {
                              m_context->m_chart->m_plotAreaFillColor.setAlphaF( val.toDouble() / 100000.0 );
                          }                      
                      }
                  }
        } else if ( qualifiedName() == "a:gsLst" ) {
            if ( isStartElement() ) {
                readingGradient = true;
                gradient =  new Charting::Gradient;
            } else if ( isEndElement() ) {
                readingGradient = false;                
                switch ( m_areaContext ) {
                    case( PlotArea ):
                      m_context->m_chart->m_plotAreaFillGradient = gradient;
                      break;
                    case( ChartArea ):
                      m_context->m_chart->m_fillGradient = gradient;
                      break;
                }
                gradient = NULL;
            }
        } else if ( qualifiedName() == "a:gs" && readingGradient ) {
            if ( isStartElement() ) {
                readingGradientStop = true;
                const QXmlStreamAttributes attrs(attributes());
                TRY_READ_ATTR_WITHOUT_NS(pos)
                if ( !pos.isEmpty() )
                    currentStop.position = pos.toDouble() / 1000.0;
                
            } else if ( isEndElement() ) {
                // append the current gradient stop                
                gradient->gradientStops.append( currentStop );
                readingGradientStop = false;
                currentStop.reset();
            }
        } else if ( qualifiedName() == "a:schemeClr" && readingGradientStop ) {
            if ( isStartElement() ) {
                const QXmlStreamAttributes attrs(attributes());
                TRY_READ_ATTR_WITHOUT_NS(val)
                if ( !val.isEmpty() )
                    currentStop.referenceColor = val;
            } else if ( isEndElement() ) {
            }
        } else if ( qualifiedName() == "a:tint" && readingGradientStop ) {
            const QXmlStreamAttributes attrs(attributes());
                TRY_READ_ATTR_WITHOUT_NS(val)
                if ( !val.isEmpty() )
                    currentStop.tintVal = val.toDouble() / 1000.0;
        } else if ( qualifiedName() == "a:satMod" && readingGradientStop ) {
            const QXmlStreamAttributes attrs(attributes());
            TRY_READ_ATTR_WITHOUT_NS(val)
            if ( !val.isEmpty() )
                currentStop.satVal = val.toDouble() / 1000.0;
        }
         else if ( qualifiedName() == "a:lin" && readingGradient ) {
            const QXmlStreamAttributes attrs(attributes());
            TRY_READ_ATTR_WITHOUT_NS(ang)
            if ( !ang.isEmpty() )
                gradient->angle = ang.toDouble() / 60000.0;
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL pieChart
//! pieChart (Pie Charts)
/*! ECMA-376, 21.2.2.141, p.3826.

 Parent elements:
 - plotArea §21.2.2.145

 Child elements:
 - dLbls (Data Labels) §21.2.2.49
 - extLst (Chart Extensibility) §21.2.2.64
 - [Done]firstSliceAng (First Slice Angle) §21.2.2.68
 - [Done] ser (Pie Chart Series) §21.2.2.172
 - varyColors (Vary Colors by Point) §21.2.2.227
*/
KoFilter::ConversionStatus XlsxXmlChartReader::read_pieChart()
{
    if(!m_context->m_chart->m_impl) {
        m_context->m_chart->m_impl = new Charting::PieImpl();
    }

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            if (QUALIFIED_NAME_IS(ser)) {
                TRY_READ(pieChart_Ser)
            }
            ELSE_TRY_READ_IF(firstSliceAng)
        }
    }

    qDeleteAll(d->m_seriesData);

    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL pie3DChart
//! pie3DChart (3D Pie Charts)
/*! ECMA-376, 21.2.2.140, p.3826.

 Parent elements:
 - plotArea §21.2.2.145

 Child elements:
 - dLbls (Data Labels) §21.2.2.49
 - extLst (Chart Extensibility) §21.2.2.64
 - [Done] ser (Pie Chart Series) §21.2.2.172
 - varyColors (Vary Colors by Point) §21.2.2.227
*/
KoFilter::ConversionStatus XlsxXmlChartReader::read_pie3DChart()
{
    if(!m_context->m_chart->m_impl) {
        m_context->m_chart->m_impl = new Charting::PieImpl();
        m_context->m_chart->m_is3d = true;
    }

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            if (QUALIFIED_NAME_IS(ser)) {
                TRY_READ(pieChart_Ser)
            }
        }
    }

//    // if there is only one c:ser, then c:tx can be chart title
//    if ((m_context->m_chart->m_title == "Chart Title") && (d->m_seriesData.size() == 1)) {
//        PieSeries * tempPieSeriesData = (PieSeries *)d->m_seriesData[0];
//        if (tempPieSeriesData->m_tx.m_strRef.m_strCache.m_cache.size() == 1) {
//            m_context->m_chart->m_title =  tempPieSeriesData->m_tx.m_strRef.m_strCache.m_cache[0];
//        }
//    }

    qDeleteAll(d->m_seriesData);

    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL doughnutChart
//! doughnutChart (Doughnut Charts)
/*! ECMA-376, 21.2.2.50, p.3782.

 Parent elements:
 - plotArea §21.2.2.145

 Child elements:
 - dLbls (Data Labels) §21.2.2.49
 - extLst (Chart Extensibility) §21.2.2.64
 - firstSliceAng (First Slice Angle) §21.2.2.68
 - [Done]holeSize (Hole Size) §21.2.2.82
 - [Done]ser (Pie Chart Series) §21.2.2.172
 - varyColors (Vary Colors by Point) §21.2.2.227
*/
KoFilter::ConversionStatus XlsxXmlChartReader::read_doughnutChart()
{
    if(!m_context->m_chart->m_impl) {
        m_context->m_chart->m_impl = new Charting::RingImpl();
    }

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            if (QUALIFIED_NAME_IS(ser)) {
                TRY_READ(pieChart_Ser)
            }
            ELSE_TRY_READ_IF(holeSize)
        }
    }

    qDeleteAll(d->m_seriesData);

    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL areaChart
//! areaChart (Area Charts)
/*! ECMA-376, 21.2.2.5, p.3757.

 Parent elements:
 - plotArea §21.2.2.145

 Child elements:
 - axId (Axis ID) §21.2.2.9
 - dLbls (Data Labels) §21.2.2.49
 - dropLines (Drop Lines) §21.2.2.53
 - extLst (Chart Extensibility) §21.2.2.64
 - grouping (Grouping) §21.2.2.76
 - [Done]ser (Area Chart Series) §21.2.2.168
 - varyColors (Vary Colors by Point) §21.2.2.227
*/
KoFilter::ConversionStatus XlsxXmlChartReader::read_areaChart()
{
    if(!m_context->m_chart->m_impl) {
        m_context->m_chart->m_impl = new Charting::AreaImpl();
    }

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            if (QUALIFIED_NAME_IS(ser)) {
                TRY_READ(areaChart_Ser)
            }
        }
    }

    qDeleteAll(d->m_seriesData);

    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL area3DChart
//! area3DChart (3D Area Charts)
/*! ECMA-376, 21.2.2.4, p.3757.

 Parent elements:
 - plotArea §21.2.2.145

 Child elements:
 - axId (Axis ID) §21.2.2.9
 - dLbls (Data Labels) §21.2.2.49
 - dropLines (Drop Lines) §21.2.2.53
 - extLst (Chart Extensibility) §21.2.2.64
 - gapDepth (Gap Depth) §21.2.2.74
 - grouping (Grouping) §21.2.2.76
 - [Done]ser (Area Chart Series) §21.2.2.168
 - varyColors (Vary Colors by Point) §21.2.2.227
*/
KoFilter::ConversionStatus XlsxXmlChartReader::read_area3DChart()
{
    if(!m_context->m_chart->m_impl) {
        m_context->m_chart->m_impl = new Charting::AreaImpl();
        m_context->m_chart->m_is3d = true;
    }

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            if (QUALIFIED_NAME_IS(ser)) {
                TRY_READ(areaChart_Ser)
            }
        }
    }

    qDeleteAll(d->m_seriesData);

    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL barChart
//! barChart (Bar Charts)
/*! ECMA-376, 21.2.2.16, p.3863.

 Parent elements:
 - plotArea §21.2.2.145

 Child elements:
 - axId (Axis ID) §21.2.2.9
 - [Done]barDir (Bar Direction) §21.2.2.17
 - dLbls (Data Labels) §21.2.2.49
 - extLst (Chart Extensibility) §21.2.2.64
 - gapWidth (Gap Width) §21.2.2.75
 - [Done]grouping (Bar Grouping) §21.2.2.77
 - overlap (Overlap) §21.2.2.131
 - [Done]ser (Bar Chart Series) §21.2.2.170
 - serLines (Series Lines) §21.2.2.176
 - varyColors (Vary Colors by Point) §21.2.2.227
*/
KoFilter::ConversionStatus XlsxXmlChartReader::read_barChart()
{
    if(!m_context->m_chart->m_impl) {
        m_context->m_chart->m_impl = new Charting::BarImpl();
    }

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            if (QUALIFIED_NAME_IS(ser)) {
                TRY_READ(barChart_Ser)
            }
            ELSE_TRY_READ_IF(barDir)
            ELSE_TRY_READ_IF(grouping)
        }
    }

    qDeleteAll(d->m_seriesData);

    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL bar3DChart
//! bar3DChart (3D Bar Charts)
/*! ECMA-376, 21.2.2.15, p.3862.

 Parent elements:
 - plotArea §21.2.2.145

 Child elements:
 - axId (Axis ID) §21.2.2.9
 - [done]barDir (Bar Direction) §21.2.2.17
 - dLbls (Data Labels) §21.2.2.49
 - extLst (Chart Extensibility) §21.2.2.64
 - gapDepth (Gap Depth) §21.2.2.74
 - gapWidth (Gap Width) §21.2.2.75
 - [Done]grouping (Bar Grouping) §21.2.2.77
 - [Done]ser (Bar Chart Series) §21.2.2.170
 - shape (Shape) §21.2.2.177
 - varyColors (Vary Colors by Point) §21.2.2.227
*/
KoFilter::ConversionStatus XlsxXmlChartReader::read_bar3DChart()
{
    if(!m_context->m_chart->m_impl) {
        m_context->m_chart->m_impl = new Charting::BarImpl();
        m_context->m_chart->m_is3d = true;
    }

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            if (QUALIFIED_NAME_IS(ser)) {
                TRY_READ(barChart_Ser)
            }
            ELSE_TRY_READ_IF(barDir)
            ELSE_TRY_READ_IF(grouping)
        }
    }

    qDeleteAll(d->m_seriesData);

    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL lineChart
//! lineChart (Line Charts)
/*! ECMA-376, 21.2.2.97, p.3804.

 Parent elements:
 - plotArea §21.2.2.145

 Child elements:
 - axId (Axis ID) §21.2.2.9
 - dLbls (Data Labels) §21.2.2.49
 - dropLines (Drop Lines) §21.2.2.53
 - extLst (Chart Extensibility) §21.2.2.64
 - [Done]grouping (Grouping) §21.2.2.76
 - hiLowLines (High Low Lines) §21.2.2.80
 - marker (Show Marker) §21.2.2.105
 - [Done]ser (Line Chart Series) §21.2.2.171
 - smooth (Smoothing) §21.2.2.194
 - upDownBars (Up/Down Bars) §21.2.2.218
 - varyColors (Vary Colors by Point) §21.2.2.227
*/
KoFilter::ConversionStatus XlsxXmlChartReader::read_lineChart()
{
    if(!m_context->m_chart->m_impl) {
        m_context->m_chart->m_impl = new Charting::LineImpl();
    }

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            if (QUALIFIED_NAME_IS(ser)) {
                TRY_READ(lineChart_Ser)
            }
            ELSE_TRY_READ_IF(grouping)
//             if ( qualifiedName() == "c:marker" )
            {
                const QXmlStreamAttributes attrs(attributes());
                TRY_READ_ATTR_WITHOUT_NS(val);
                if ( val == "1" || val == "true" || val == "on " )
                {                    
                    m_context->m_chart->m_showMarker = true;
                }
            }
        }
    }

    qDeleteAll(d->m_seriesData);

    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL line3DChart
//! line3DChart (3D Line Charts)
/*! ECMA-376, 21.2.2.96, p.3803.

 Parent elements:
 - plotArea §21.2.2.145

 Child elements:
 - axId (Axis ID) §21.2.2.9
 - dLbls (Data Labels) §21.2.2.49
 - dropLines (Drop Lines) §21.2.2.53
 - extLst (Chart Extensibility) §21.2.2.64
 - gapDepth (Gap Depth) §21.2.2.74
 - [Done]grouping (Grouping) §21.2.2.76
 - [Done]ser (Line Chart Series) §21.2.2.171
 - varyColors (Vary Colors by Point) §21.2.2.227
*/
KoFilter::ConversionStatus XlsxXmlChartReader::read_line3DChart()
{
    if(!m_context->m_chart->m_impl) {
        m_context->m_chart->m_impl = new Charting::LineImpl();
        m_context->m_chart->m_is3d = true;
    }

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            if (QUALIFIED_NAME_IS(ser)) {
                TRY_READ(lineChart_Ser)
            }
            ELSE_TRY_READ_IF(grouping)
        }
    }

    qDeleteAll(d->m_seriesData);

    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL scatterChart
//! scatterChart (Scatter Charts)
/*! ECMA-376, 21.2.2.161, p.3836.

 Parent elements:
 - plotArea §21.2.2.145

 Child elements:
 - axId (Axis ID) §21.2.2.9
 - dLbls (Data Labels) §21.2.2.49
 - extLst (Chart Extensibility) §21.2.2.64
 - scatterStyle (Scatter Style) §21.2.2.162
 - [Done]ser (Scatter Chart Series) §21.2.2.167
 - varyColors (Vary Colors by Point) §21.2.2.227
*/
KoFilter::ConversionStatus XlsxXmlChartReader::read_scatterChart()
{
    if(!m_context->m_chart->m_impl) {
        m_context->m_chart->m_impl = new Charting::ScatterImpl();
    }

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            if (QUALIFIED_NAME_IS(ser)) {
                TRY_READ(scatterChart_Ser)
            }
        }
    }

    qDeleteAll(d->m_seriesData);

    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL radarChart
//! radarChart (Radar Charts)
/*! ECMA-376, 21.2.2.153, p.3832.

 Parent elements:
 - plotArea §21.2.2.145

 Child elements:
 - axId (Axis ID) §21.2.2.9
 - dLbls (Data Labels) §21.2.2.49
 - extLst (Chart Extensibility) §21.2.2.64
 - radarStyle (Radar Style) §21.2.2.154
 - [Done]ser (Radar Chart Series) §21.2.2.169
 - varyColors (Vary Colors by Point) §21.2.2.227
*/
KoFilter::ConversionStatus XlsxXmlChartReader::read_radarChart()
{
    if(!m_context->m_chart->m_impl) {
        m_context->m_chart->m_impl = new Charting::RadarImpl();
    }

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            if (QUALIFIED_NAME_IS(ser)) {
                TRY_READ(radarChart_Ser)
            }
        }
    }

    qDeleteAll(d->m_seriesData);

    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL surfaceChart
//! surface3DChart (3D Surface Charts)
/*! ECMA-376, 21.2.2.203, p.3858.

 Parent elements:
 - plotArea §21.2.2.145

 Child elements:
 - axId (Axis ID) §21.2.2.9
 - bandFmts (Band Formats) §21.2.2.14
 - extLst (Chart Extensibility) §21.2.2.64
 - [Done]ser (Surface Chart Series) §21.2.2.173
 - wireframe (Wireframe) §21.2.2.230
*/
KoFilter::ConversionStatus XlsxXmlChartReader::read_surfaceChart()
{
    if(!m_context->m_chart->m_impl) {
        m_context->m_chart->m_impl = new Charting::SurfaceImpl();
    }

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            if (QUALIFIED_NAME_IS(ser)) {
                TRY_READ(surfaceChart_Ser)
            }
        }
    }

    qDeleteAll(d->m_seriesData);

    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL surface3DChart
//! surfaceChart (Surface Charts)
/*! ECMA-376, 21.2.2.204, p.3858.

 Parent elements:
 - plotArea §21.2.2.145

 Child elements:
 - axId (Axis ID) §21.2.2.9
 - bandFmts (Band Formats) §21.2.2.14
 - extLst (Chart Extensibility) §21.2.2.64
 - ser (Surface Chart Series) §21.2.2.173
 - wireframe (Wireframe) §21.2.2.230
*/
KoFilter::ConversionStatus XlsxXmlChartReader::read_surface3DChart()
{
    if(!m_context->m_chart->m_impl) {
        m_context->m_chart->m_impl = new Charting::SurfaceImpl();
        m_context->m_chart->m_is3d = true;
    }

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            if (QUALIFIED_NAME_IS(ser)) {
                TRY_READ(surfaceChart_Ser)
            }
        }
    }

    qDeleteAll(d->m_seriesData);

    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL bubbleChart
//! bubbleChart (Bubble Charts)
/*! ECMA-376, 21.2.2.20, p.3765.

 Parent elements:
 - plotArea §21.2.2.145

 Child elements:
 - axId (Axis ID) §21.2.2.9
 - [Done]bubble3D (3D Bubble) §21.2.2.19
 - [Done]bubbleScale (Bubble Scale) §21.2.2.21
 - dLbls (Data Labels) §21.2.2.49
 - extLst (Chart Extensibility) §21.2.2.64
 - [Done]ser (Bubble Chart Series) §21.2.2.174
 - showNegBubbles (Show Negative Bubbles) §21.2.2.185
 - sizeRepresents (Size Represents) §21.2.2.193
 - varyColors (Vary Colors by Point) §21.2.2.227
*/
KoFilter::ConversionStatus XlsxXmlChartReader::read_bubbleChart()
{
    if(!m_context->m_chart->m_impl) {
        m_context->m_chart->m_impl = new Charting::BubbleImpl();
        m_context->m_chart->m_is3d = true;
    }

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            if (QUALIFIED_NAME_IS(ser)) {
                TRY_READ(bubbleChart_Ser)
            }
            ELSE_TRY_READ_IF(bubbleScale)
            ELSE_TRY_READ_IF(bubble3D)
        }
    }

    // check if there are some c:strLit or c:numLit data and if yes then write them into internalTable
//    for (int i=0; i<d->m_seriesData.size(); i++ ){
//        QString range = ((BubbleSeries *)d->m_seriesData[i])->m_bubbleSize.writeLitToInternalTable(this);
//        if (!range.isEmpty()) {
//            m_context->m_chart->m_series[i]->m_domainValuesCellRangeAddress.push_back(range);
//        }
//    }

    qDeleteAll(d->m_seriesData);

    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL stockChart
//! stockChart (Stock Charts)
/*! ECMA-376, 21.2.2.199, p.3856.

 Parent elements:
 - plotArea §21.2.2.145

 Child elements:
 - axId (Axis ID) §21.2.2.9
 - dLbls (Data Labels) §21.2.2.49
 - dropLines (Drop Lines) §21.2.2.53
 - extLst (Chart Extensibility) §21.2.2.64
 - hiLowLines (High Low Lines) §21.2.2.80
 - [done]ser (Line Chart Series) §21.2.2.171
 - upDownBars (Up/Down Bars) §21.2.2.218
*/
KoFilter::ConversionStatus XlsxXmlChartReader::read_stockChart()
{
    if(!m_context->m_chart->m_impl) {
        m_context->m_chart->m_impl = new Charting::StockImpl();
    }

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            if (QUALIFIED_NAME_IS(ser)) {
                TRY_READ(lineChart_Ser)
            }
        }
    }

    qDeleteAll(d->m_seriesData);

    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL ser
//! ser (Pie Chart Series)
/*! ECMA-376, 21.2.2.172, p.3842.

 Parent elements:
 - doughnutChart §21.2.2.50
 - ofPieChart §21.2.2.126
 - pie3DChart §21.2.2.140
 - pieChart §21.2.2.141

 Child elements:
 - cat (Category Axis Data) §21.2.2.24
 - [Done]dLbls (Data Labels) §21.2.2.49
 - dPt (Data Point) §21.2.2.52
 - explosion (Explosion) §21.2.2.61
 - extLst (Chart Extensibility) §21.2.2.64
 - [Done]idx (Index) §21.2.2.84
 - [Done] order (Order) §21.2.2.128
 - spPr (Shape Properties) §21.2.2.197
 - tx (Series Text) §21.2.2.215
 - val (Values) §21.2.2.224

*/

KoFilter::ConversionStatus XlsxXmlChartReader::read_pieChart_Ser()
{
    READ_PROLOGUE2(pieChart_Ser)

    m_currentSeries  = new Charting::Series();
    m_context->m_chart->m_series << m_currentSeries;

    PieSeries * tempPieSeriesData = new PieSeries();
    d->m_seriesData << tempPieSeriesData;

    d->m_currentIdx = &tempPieSeriesData->m_idx;
    d->m_currentOrder = &tempPieSeriesData->m_order;
    d->m_currentTx = &tempPieSeriesData->m_tx;
    d->m_currentCat = &tempPieSeriesData->m_cat;
    d->m_currentVal = &tempPieSeriesData->m_val;
    d->m_currentExplosion = &tempPieSeriesData->m_explosion;

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(order)
            ELSE_TRY_READ_IF(idx)
            if (QUALIFIED_NAME_IS(tx)) {
                TRY_READ(seriesText_Tx)
            }
            ELSE_TRY_READ_IF(cat)
            ELSE_TRY_READ_IF(val)
            ELSE_TRY_READ_IF(explosion)
            ELSE_TRY_READ_IF(dLbls)
        }
    }

    // set data ranges and write data to internal table
    m_currentSeries->m_countYValues = tempPieSeriesData->m_val.m_numRef.m_numCache.m_ptCount;

    m_currentSeries->m_labelCell = tempPieSeriesData->m_tx.writeRefToInternalTable(this);

    m_currentSeries->m_valuesCellRangeAddress = tempPieSeriesData->m_val.writeRefToInternalTable(this);

    m_context->m_chart->m_verticalCellRangeAddress = tempPieSeriesData->m_cat.writeRefToInternalTable(this);

    // set explosion
    if (tempPieSeriesData->m_explosion != 0) {
        if(Charting::PieImpl* pie = dynamic_cast<Charting::PieImpl*>(m_context->m_chart->m_impl)) {
            Q_UNUSED(pie);
            m_currentSeries->m_datasetFormat << new Charting::PieFormat(tempPieSeriesData->m_explosion);
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL ser
//! ser (Bubble Chart Series)
/*! ECMA-376, 21.2.2.174, p.3843.

 Parent elements:
  - ser (Bubble Chart Series)

 Child elements:
 - [Done]bubble3D (3D Bubble) §21.2.2.19
 - [Done]bubbleSize (Bubble Size) §21.2.2.22
 - [Done]dLbls (Data Labels) §21.2.2.49
 - dPt (Data Point) §21.2.2.52
 - errBars (Error Bars) §21.2.2.55
 - extLst (Chart Extensibility) §21.2.2.64
 - [Done]idx (Index) §21.2.2.84
 - invertIfNegative (Invert if Negative) §21.2.2.86
 - [Done]order (Order) §21.2.2.128
 - spPr (Shape Properties) §21.2.2.197
 - trendline (Trendlines) §21.2.2.211
 - [Done]tx (Series Text) §21.2.2.215
 - [Done]xVal (X Values) §21.2.2.234
 - [Done]yVal (Y Values) §21.2.2.237

*/

KoFilter::ConversionStatus XlsxXmlChartReader::read_bubbleChart_Ser()
{
    READ_PROLOGUE2(bubbleChart_Ser)

    m_currentSeries  = new Charting::Series();
    m_context->m_chart->m_series << m_currentSeries;

    BubbleSeries * tempBubbleSeriesData = new BubbleSeries();
    d->m_seriesData << tempBubbleSeriesData;

    d->m_currentIdx = &tempBubbleSeriesData->m_idx;
    d->m_currentOrder = &tempBubbleSeriesData->m_order;
    d->m_currentTx = &tempBubbleSeriesData->m_tx;
    d->m_currentXVal = &tempBubbleSeriesData->m_xVal;
    d->m_currentYVal = &tempBubbleSeriesData->m_yVal;
    d->m_currentBubbleSize = &tempBubbleSeriesData->m_bubbleSize;

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(order)
            ELSE_TRY_READ_IF(idx)
            if (QUALIFIED_NAME_IS(tx)) {
                TRY_READ(seriesText_Tx)
            }
            ELSE_TRY_READ_IF(xVal)
            ELSE_TRY_READ_IF(yVal)
            ELSE_TRY_READ_IF(bubbleSize)
            ELSE_TRY_READ_IF(dLbls)
            ELSE_TRY_READ_IF(bubble3D)
        }
    }

    // set data ranges and write data to internal table
    m_currentSeries->m_labelCell = tempBubbleSeriesData->m_tx.writeRefToInternalTable(this);

    m_currentSeries->m_countYValues = tempBubbleSeriesData->m_yVal.m_numRef.m_numCache.m_ptCount;

    m_currentSeries->m_valuesCellRangeAddress = tempBubbleSeriesData->m_yVal.writeRefToInternalTable(this);

//    m_currentSeries->m_domainValuesCellRangeAddress.push_back(tempBubbleSeriesData->m_xVal.writeRefToInternalTable(this));
//
//    QString bubbleSizeRange = tempBubbleSeriesData->m_bubbleSize.writeRefToInternalTable(this);
//    if (!bubbleSizeRange.isEmpty()) {
//        m_currentSeries->m_domainValuesCellRangeAddress.push_back(tempBubbleSeriesData->m_bubbleSize.writeRefToInternalTable(this));
//    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL ser
//! ser (Scatter Chart Series)
/*! ECMA-376, 21.2.2.167, p.3838.

 Parent elements:
  - scatterChart (§21.2.2.161)

 Child elements:
 - [Done]dLbls (Data Labels) §21.2.2.49
 - dPt (Data Point) §21.2.2.52
 - errBars (Error Bars) §21.2.2.55
 - extLst (Chart Extensibility) §21.2.2.64
 - [done]idx (Index) §21.2.2.84
 - marker (Marker) §21.2.2.106
 - [Done]order (Order) §21.2.2.128
 - smooth (Smoothing) §21.2.2.194
 - spPr (Shape Properties) §21.2.2.197
 - trendline (Trendlines) §21.2.2.211
 - [Done]tx (Series Text) §21.2.2.215
 - [Done]xVal (X Values) §21.2.2.234
 - [Done]yVal (Y Values) §21.2.2.237
*/

KoFilter::ConversionStatus XlsxXmlChartReader::read_scatterChart_Ser()
{
    READ_PROLOGUE2(scatterChart_Ser)

    m_currentSeries  = new Charting::Series();
    m_context->m_chart->m_series << m_currentSeries;

    ScatterSeries * tempScatterSeriesData = new ScatterSeries();
    d->m_seriesData << tempScatterSeriesData;

    d->m_currentIdx = &tempScatterSeriesData->m_idx;
    d->m_currentOrder = &tempScatterSeriesData->m_order;
    d->m_currentTx = &tempScatterSeriesData->m_tx;
    d->m_currentXVal = &tempScatterSeriesData->m_xVal;
    d->m_currentYVal = &tempScatterSeriesData->m_yVal;

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(order)
            ELSE_TRY_READ_IF(idx)
            if (QUALIFIED_NAME_IS(tx)) {
                TRY_READ(seriesText_Tx)
            }
            ELSE_TRY_READ_IF(xVal)
            ELSE_TRY_READ_IF(yVal)
            ELSE_TRY_READ_IF(dLbls)
//            ELSE_TRY_READ_IF(spPr)
        }
    }

    // set data ranges and write data to internal table
    m_currentSeries->m_labelCell = tempScatterSeriesData->m_tx.writeRefToInternalTable(this);

    m_currentSeries->m_countYValues = tempScatterSeriesData->m_yVal.m_numRef.m_numCache.m_ptCount;

    m_currentSeries->m_valuesCellRangeAddress = tempScatterSeriesData->m_yVal.writeRefToInternalTable(this);

    //m_currentSeries->m_domainValuesCellRangeAddress.push_back(tempScatterSeriesData->m_xVal.writeRefToInternalTable(this));

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL ser
//! ser (Bar Chart Series)
/*! ECMA-376, 21.2.2.167, p.3840.

 Parent elements:
  - bar3DChart (§21.2.2.15)
  - barChart (§21.2.2.16)

 Child elements:
  - [Done]cat (Category Axis Data) §21.2.2.24
  - [Done]dLbls (Data Labels) §21.2.2.49
  - dPt (Data Point) §21.2.2.52
  - errBars (Error Bars) §21.2.2.55
  - extLst (Chart Extensibility) §21.2.2.64
  - idx (Index) §21.2.2.84
  - invertIfNegative (Invert if Negative) §21.2.2.86
  - order (Order) §21.2.2.128
  - pictureOptions (Picture Options) §21.2.2.138
  - shape (Shape) §21.2.2.177
  - spPr (Shape Properties) §21.2.2.197
  - trendline (Trendlines) §21.2.2.211
  - [Done]tx (Series Text) §21.2.2.215
  - [Done]val (Values) §21.2.2.224
*/

KoFilter::ConversionStatus XlsxXmlChartReader::read_barChart_Ser()
{
    READ_PROLOGUE2(barChart_Ser)

    m_currentSeries  = new Charting::Series();
    m_context->m_chart->m_series << m_currentSeries;

    BarSeries * tempBarSeriesData = new BarSeries();
    d->m_seriesData << tempBarSeriesData;

    d->m_currentIdx = &tempBarSeriesData->m_idx;
    d->m_currentOrder = &tempBarSeriesData->m_order;
    d->m_currentTx = &tempBarSeriesData->m_tx;
    d->m_currentCat = &tempBarSeriesData->m_cat;
    d->m_currentVal = &tempBarSeriesData->m_val;

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(order)
            ELSE_TRY_READ_IF(idx)
            if (QUALIFIED_NAME_IS(tx)) {
                TRY_READ(seriesText_Tx)
            }
            ELSE_TRY_READ_IF(cat)
            ELSE_TRY_READ_IF(val)
            ELSE_TRY_READ_IF(dLbls)
        }
    }

    // set data ranges and write data to internal table
    m_currentSeries->m_countYValues = tempBarSeriesData->m_val.m_numRef.m_numCache.m_ptCount;

    m_currentSeries->m_labelCell = tempBarSeriesData->m_tx.writeRefToInternalTable(this);

    m_currentSeries->m_valuesCellRangeAddress = tempBarSeriesData->m_val.writeRefToInternalTable(this);

    m_context->m_chart->m_verticalCellRangeAddress = tempBarSeriesData->m_cat.writeRefToInternalTable(this);

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL ser
//! ser (Area Chart Series)
/*! ECMA-376, 21.2.2.168, p.3839.

 Parent elements:
  - area3DChart (§21.2.2.4)
  - areaChart (§21.2.2.5)

 Child elements:
  - cat (Category Axis Data) §21.2.2.24
  - [Done]dLbls (Data Labels) §21.2.2.49
  - dPt (Data Point) §21.2.2.52
  - errBars (Error Bars) §21.2.2.55
  - extLst (Chart Extensibility) §21.2.2.64
  - idx (Index) §21.2.2.84
  - order (Order) §21.2.2.128
  - pictureOptions (Picture Options) §21.2.2.138
  - spPr (Shape Properties) §21.2.2.197
  - trendline (Trendlines) §21.2.2.211
  - tx (Series Text) §21.2.2.215
  - val (Values) §21.2.2.224
*/

KoFilter::ConversionStatus XlsxXmlChartReader::read_areaChart_Ser()
{
    READ_PROLOGUE2(areaChart_Ser)

    m_currentSeries  = new Charting::Series();
    m_context->m_chart->m_series << m_currentSeries;

    AreaSeries * tempAreaSeriesData = new AreaSeries();
    d->m_seriesData << tempAreaSeriesData;

    d->m_currentIdx = &tempAreaSeriesData->m_idx;
    d->m_currentOrder = &tempAreaSeriesData->m_order;
    d->m_currentTx = &tempAreaSeriesData->m_tx;
    d->m_currentCat = &tempAreaSeriesData->m_cat;
    d->m_currentVal = &tempAreaSeriesData->m_val;

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(order)
            ELSE_TRY_READ_IF(idx)
            if (QUALIFIED_NAME_IS(tx)) {
                TRY_READ(seriesText_Tx)
            }
            ELSE_TRY_READ_IF(cat)
            ELSE_TRY_READ_IF(val)
            ELSE_TRY_READ_IF(dLbls)
        }
    }

    // set data ranges and write data to internal table
    m_currentSeries->m_countYValues = tempAreaSeriesData->m_val.m_numRef.m_numCache.m_ptCount;

    m_currentSeries->m_labelCell = tempAreaSeriesData->m_tx.writeRefToInternalTable(this);

    m_currentSeries->m_valuesCellRangeAddress = tempAreaSeriesData->m_val.writeRefToInternalTable(this);

    m_context->m_chart->m_verticalCellRangeAddress = tempAreaSeriesData->m_cat.writeRefToInternalTable(this);

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL ser
//! ser (Radar Chart Series)
/*! ECMA-376, 21.2.2.169, p.3840.

 Parent elements:
  - radarChart (§21.2.2.153)

 Child elements:
  - [Done]cat (Category Axis Data) §21.2.2.24
  - [Done]dLbls (Data Labels) §21.2.2.49
  - dPt (Data Point) §21.2.2.52
  - extLst (Chart Extensibility) §21.2.2.64
  - [Done]idx (Index) §21.2.2.84
  - marker (Marker) §21.2.2.106
  - [Done]order (Order) §21.2.2.128
  - spPr (Shape Properties) §21.2.2.197
  - [Done]tx (Series Text) §21.2.2.215
  - [Done]val (Values) §21.2.2.224
*/

KoFilter::ConversionStatus XlsxXmlChartReader::read_radarChart_Ser()
{
    READ_PROLOGUE2(radarChart_Ser)

    m_currentSeries  = new Charting::Series();
    m_context->m_chart->m_series << m_currentSeries;

    RadarSeries * tempRadarSeriesData = new RadarSeries();
    d->m_seriesData << tempRadarSeriesData;

    d->m_currentIdx = &tempRadarSeriesData->m_idx;
    d->m_currentOrder = &tempRadarSeriesData->m_order;
    d->m_currentTx = &tempRadarSeriesData->m_tx;
    d->m_currentCat = &tempRadarSeriesData->m_cat;
    d->m_currentVal = &tempRadarSeriesData->m_val;

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(order)
            ELSE_TRY_READ_IF(idx)
            if (QUALIFIED_NAME_IS(tx)) {
                TRY_READ(seriesText_Tx)
            }
            ELSE_TRY_READ_IF(cat)
            ELSE_TRY_READ_IF(val)
            ELSE_TRY_READ_IF(dLbls)
        }
    }

    // set data ranges and write data to internal table
    m_currentSeries->m_countYValues = tempRadarSeriesData->m_val.m_numRef.m_numCache.m_ptCount;

    m_currentSeries->m_labelCell = tempRadarSeriesData->m_tx.writeRefToInternalTable(this);

    m_currentSeries->m_valuesCellRangeAddress = tempRadarSeriesData->m_val.writeRefToInternalTable(this);

    m_context->m_chart->m_verticalCellRangeAddress = tempRadarSeriesData->m_cat.writeRefToInternalTable(this);

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL ser
//! ser (Line Chart Series)
/*! ECMA-376, 21.2.2.168, p.3839.

 Parent elements:
  - line3DChart (§21.2.2.96)
  - lineChart (§21.2.2.97)
  - stockChart (§21.2.2.198)

 Child elements:
  - [Done]cat (Category Axis Data) §21.2.2.24
  - [Done]dLbls (Data Labels) §21.2.2.49
  - dPt (Data Point) §21.2.2.52
  - errBars (Error Bars) §21.2.2.55
  - extLst (Chart Extensibility) §21.2.2.64
  - [Done]idx (Index) §21.2.2.84
  - marker (Marker) §21.2.2.106
  - [Done]order (Order) §21.2.2.128
  - smooth (Smoothing) §21.2.2.194
  - spPr (Shape Properties) §21.2.2.197
  - trendline (Trendlines) §21.2.2.211
  - [Done]tx (Series Text) §21.2.2.215
  - [Done]val (Values) §21.2.2.224
*/

KoFilter::ConversionStatus XlsxXmlChartReader::read_lineChart_Ser()
{
    READ_PROLOGUE2(lineChart_Ser)

    m_currentSeries  = new Charting::Series();
    m_context->m_chart->m_series << m_currentSeries;

    LineSeries * tempLineSeriesData = new LineSeries();
    d->m_seriesData << tempLineSeriesData;

    d->m_currentIdx = &tempLineSeriesData->m_idx;
    d->m_currentOrder = &tempLineSeriesData->m_order;
    d->m_currentTx = &tempLineSeriesData->m_tx;
    d->m_currentCat = &tempLineSeriesData->m_cat;
    d->m_currentVal = &tempLineSeriesData->m_val;

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(order)
            ELSE_TRY_READ_IF(idx)
            if (QUALIFIED_NAME_IS(tx)) {
                TRY_READ(seriesText_Tx)
            }
            ELSE_TRY_READ_IF(cat)
            ELSE_TRY_READ_IF(val)
            ELSE_TRY_READ_IF(dLbls)
            ELSE_TRY_READ_IF(serMarker)
//             if ( qualifiedName() == "c:marker" )
//             {
//                 const QXmlStreamAttributes attrs(attributes());
//                 TRY_READ_ATTR_WITHOUT_NS(val);
//                 if ( val == "1" || val == "true" || val == "on " )
//                 {                    
//                     if ( m_currentSeries->markerType == Charting::Series::None )
//                         switch ( d->m_numReadSeries )
//                         {
//                             case 0:
//                                 m_currentSeries->markerType = Charting::Series::Square;
//                                 break;
//                             case 1:
//                                 m_currentSeries->markerType = Charting::Series::Diamond;
//                                 break;
//                             default:
//                                 break;
//                         }
//                     ++d->m_numReadSeries;
//                 }
//             }
                
        }
    }

    // set data ranges and write data to internal table
    m_currentSeries->m_countYValues = tempLineSeriesData->m_val.m_numRef.m_numCache.m_ptCount;

    m_currentSeries->m_labelCell = tempLineSeriesData->m_tx.writeRefToInternalTable(this);

    m_currentSeries->m_valuesCellRangeAddress = tempLineSeriesData->m_val.writeRefToInternalTable(this);

    m_context->m_chart->m_verticalCellRangeAddress = tempLineSeriesData->m_cat.writeRefToInternalTable(this);

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL marker
KoFilter::ConversionStatus XlsxXmlChartReader::read_serMarker()
{
    using namespace Charting;
    READ_PROLOGUE2( serMarker )
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            if ( qualifiedName() == "c:symbol" )
            {
                const QXmlStreamAttributes attrs(attributes());
                TRY_READ_ATTR_WITHOUT_NS(val);
                if ( val.toLower() == "star" )
                    m_currentSeries->markerType = Series::Star;
                else if ( val.toLower() == "dash" )
                    m_currentSeries->markerType = Series::Dash;
                else if ( val.toLower() == "dot" )
                    m_currentSeries->markerType = Series::Dot;
                else if ( val.toLower() == "plus" )
                    m_currentSeries->markerType = Series::Plus;
                else if ( val.toLower() == "circle" )
                    m_currentSeries->markerType = Series::Circle;
                else if ( val.toLower() == "x" )
                    m_currentSeries->markerType = Series::SymbolX;
                else if ( val.toLower() == "triangle" )
                    m_currentSeries->markerType = Series::Triangle;
                else if ( val.toLower() == "squre" )
                    m_currentSeries->markerType = Series::Square;
                else if ( val.toLower() == "diamond" )
                    m_currentSeries->markerType = Series::Diamond;
            }
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL ser
//! ser (Surface Chart Series)
/*! ECMA-376, 21.2.2.169, p.3840.

 Parent elements:
  - surface3DChart (§21.2.2.203)
  - surfaceChart (§21.2.2.204)

 Child elements:
  - [Done]cat (Category Axis Data) §21.2.2.24
  - extLst (Chart Extensibility) §21.2.2.64
  - [Done]idx (Index) §21.2.2.84
  - marker (Marker) §21.2.2.106
  - [Done]order (Order) §21.2.2.128
  - spPr (Shape Properties) §21.2.2.197
  - [Done]tx (Series Text) §21.2.2.215
  - [Done]val (Values) §21.2.2.224
*/

KoFilter::ConversionStatus XlsxXmlChartReader::read_surfaceChart_Ser()
{
    READ_PROLOGUE2(surfaceChart_Ser)

    m_currentSeries  = new Charting::Series();
    m_context->m_chart->m_series << m_currentSeries;

    SurfaceSeries * tempSurfaceSeriesData = new SurfaceSeries();
    d->m_seriesData << tempSurfaceSeriesData;

    d->m_currentIdx = &tempSurfaceSeriesData->m_idx;
    d->m_currentOrder = &tempSurfaceSeriesData->m_order;
    d->m_currentTx = &tempSurfaceSeriesData->m_tx;
    d->m_currentCat = &tempSurfaceSeriesData->m_cat;
    d->m_currentVal = &tempSurfaceSeriesData->m_val;

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(order)
            ELSE_TRY_READ_IF(idx)
            if (QUALIFIED_NAME_IS(tx)) {
                TRY_READ(seriesText_Tx)
            }
            ELSE_TRY_READ_IF(cat)
            ELSE_TRY_READ_IF(val)
        }
    }

    // set data ranges and write data to internal table
    m_currentSeries->m_countYValues = tempSurfaceSeriesData->m_val.m_numRef.m_numCache.m_ptCount;

    m_currentSeries->m_labelCell = tempSurfaceSeriesData->m_tx.writeRefToInternalTable(this);

    m_currentSeries->m_valuesCellRangeAddress = tempSurfaceSeriesData->m_val.writeRefToInternalTable(this);

    m_context->m_chart->m_verticalCellRangeAddress = tempSurfaceSeriesData->m_cat.writeRefToInternalTable(this);

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL barDir
//! barDir (Bar Direction)
/*! ECMA-376, 21.2.2.17, p.3763.

 Parent elements:
  - bar3DChart (§21.2.2.15)
  - barChart (§21.2.2.16)

 Attributes:
  - [Done] val (Bar Direction Value)
*/
KoFilter::ConversionStatus XlsxXmlChartReader::read_barDir()
{
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(val)
    m_context->m_chart->m_transpose = (val == "bar"); // "bar" or "col"
    while (!atEnd()) {
        BREAK_IF_END_OF(CURRENT_EL);
        readNext();
    }
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL grouping
//! grouping (Bar Grouping)
/*! ECMA-376, 21.2.2.77, p.3794.

 Parent elements:
  - bar3DChart (§21.2.2.15)
  - barChart (§21.2.2.16)

 Attributes:
  - [Done] val (Bar Grouping Value)
*/
KoFilter::ConversionStatus XlsxXmlChartReader::read_grouping()
{
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(val)
    if(val == "stacked") {
        m_context->m_chart->m_stacked = true;
    } else if(val == "percentStacked") {
        m_context->m_chart->m_stacked = true;
        m_context->m_chart->m_f100 = true;
    } else if(val == "clustered") {
        //TODO
    } // else if(val == "standard") is not needed cause that's the default anyway
    while (!atEnd()) {
        BREAK_IF_END_OF(CURRENT_EL);
        readNext();
    }
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL firstSliceAng
//! firstSliceAng (First Slice Angle)
/*! ECMA-376, 21.2.2.68, p.3790.

 Parent elements:
  - doughnutChart (§21.2.2.50)
  - pieChart (§21.2.2.141)

 Child elements:
  - val (First Slice Angle Value)
*/
KoFilter::ConversionStatus XlsxXmlChartReader::read_firstSliceAng()
{
    if(Charting::PieImpl* pie = dynamic_cast<Charting::PieImpl*>(m_context->m_chart->m_impl)) {
        const QXmlStreamAttributes attrs(attributes());
        QString val(attrs.value("val").toString());
        pie->m_anStart = val.toInt(); // default value is zero
    }
    while (!atEnd()) {
        BREAK_IF_END_OF(CURRENT_EL);
        readNext();
    }
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL holeSize
//! holeSize (Hole Size)
/*! ECMA-376, 21.2.2.82, p.3797.

 Parent elements:
  - doughnutChart (§21.2.2.50)

 Child elements:
  - val (Hole Size Value)
*/
KoFilter::ConversionStatus XlsxXmlChartReader::read_holeSize()
{
    if(Charting::RingImpl* ring = dynamic_cast<Charting::RingImpl*>(m_context->m_chart->m_impl)) {
        const QXmlStreamAttributes attrs(attributes());
        QString val(attrs.value("val").toString());
        ring->m_pcDonut = val.toInt(); // default value is zero
    }
    while (!atEnd()) {
        BREAK_IF_END_OF(CURRENT_EL);
        readNext();
    }
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL bubbleSize
//! bubbleSize (Bubble Size)
/*! ECMA-376, 21.2.2.22, p.3876.

 Parent elements:
  - ser §21.2.2.174

 Child elements:
  - numLit (Number Literal) §21.2.2.122
  - [done]numRef (Number Reference) §21.2.2.123
*/
KoFilter::ConversionStatus XlsxXmlChartReader::read_bubbleSize()
{
    READ_PROLOGUE
    d->m_currentNumRef = &d->m_currentBubbleSize->m_numRef;
    d->m_currentNumLit = &d->m_currentBubbleSize->m_numLit;
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(numRef)
            ELSE_TRY_READ_IF(numLit)

        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL bubbleScale
//! bubbleScale (Bubble Scale)
/*! ECMA-376, 21.2.2.21, p.3765.

 Parent elements:
  - bubbleChart (§21.2.2.20)

 Attributes:
  - [Done] val (Bubble Scale Value)
*/
KoFilter::ConversionStatus XlsxXmlChartReader::read_bubbleScale()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    QString val(attrs.value("val").toString());

    if(Charting::BubbleImpl* bubble = dynamic_cast<Charting::BubbleImpl*>(m_context->m_chart->m_impl)) {
        bool ok;
        const int i = val.toInt(&ok);
        if(ok)
            bubble->m_sizeRatio = i;
    }
    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL bubble3D
//! bubble3D (3D Bubble)
/*! ECMA-376, 21.2.2.21, p.3765.

 Parent elements:
  - bubbleChart (§21.2.2.20)
  - dPt (§21.2.2.52)
  - ser (§21.2.2.174)

 Attributes:
  - [Done] val (Boolean Value)
*/
KoFilter::ConversionStatus XlsxXmlChartReader::read_bubble3D()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    QString val(attrs.value("val").toString());

    m_context->m_chart->m_is3d  = val.toInt();
    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL numLit
//! numLit (Number Literal)
/*! ECMA-376, 21.2.2.122, p.3815.

 Parent elements:
  - bubbleSize (§21.2.2.22)
  - cat (§21.2.2.24)
  - minus (§21.2.2.113)
  - plus (§21.2.2.147)
  - val (§21.2.2.224)
  - xVal(§21.2.2.234)
  - yVal (§21.2.2.237)

 Child elements:
  - extLst (Chart Extensibility) §21.2.2.64
  - formatCode (Format Code) §21.2.2.71
  - [Done]pt (Numeric Point) §21.2.2.150
  - [Done]ptCount (Point Count) §21.2.2.152
*/
KoFilter::ConversionStatus XlsxXmlChartReader::read_numLit()
{
    READ_PROLOGUE
    d->m_currentPtCount = &d->m_currentNumLit->m_ptCount;
    d->m_currentPtCache = &d->m_currentNumLit->m_cache;
    while ( !atEnd() ) {
        readNext();
        BREAK_IF_END_OF( CURRENT_EL );
        if ( isStartElement() ) {
            TRY_READ_IF(ptCount)
            ELSE_TRY_READ_IF(pt)
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL pt
//! pt (String Point)
/*! ECMA-376, 21.2.2.151, p.3831.

 Parent elements:
  - lvl (§21.2.2.99)
  - strCache (§21.2.2.199)
  - strLit (§21.2.2.200)

 Child elements:
  - [Done]v (Text Value)

 Attributes:
  - idx (Index)
*/
KoFilter::ConversionStatus XlsxXmlChartReader::read_pt()
{
    READ_PROLOGUE
    while ( !atEnd() ) {
        readNext();
        BREAK_IF_END_OF( CURRENT_EL );
        if ( isStartElement() ) {
          if ( qualifiedName() == QLatin1String( QUALIFIED_NAME( v ) ) ) {
              d->m_currentPtCache->append(readElementText());
          }
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL order
//! order (Order)
/*! ECMA-376, 21.2.2.128, p.3817.

 Parent elements:
  - ser §21.2.2.168
  - ser §21.2.2.170
  - ser §21.2.2.174
  - ser §21.2.2.171
  - ser §21.2.2.172
  - ser §21.2.2.169
  - ser §21.2.2.167
  - ser §21.2.2.173

 Attributes:
  - [Done] val (Integer Value)
*/
KoFilter::ConversionStatus XlsxXmlChartReader::read_order()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    QString val(attrs.value("val").toString());
    *d->m_currentOrder = val.toInt();

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL idx
//! idx (Index)
/*! ECMA-376, 21.2.2.84, p.3798.

 Parent elements:
  - bandFmt (§21.2.2.13)
  - dLbl (§21.2.2.47)
  - dPt (§21.2.2.52)
  - legendEntry (§21.2.2.94)
  - pivotFmt (§21.2.2.142)
  - ser §21.2.2.168
  - ser §21.2.2.170
  - ser §21.2.2.174
  - ser §21.2.2.171
  - ser §21.2.2.172
  - ser §21.2.2.169
  - ser §21.2.2.167
  - ser §21.2.2.173

 Attributes:
  - [Done] val (Integer Value)
*/
KoFilter::ConversionStatus XlsxXmlChartReader::read_idx()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    QString val(attrs.value("val").toString());
    *d->m_currentIdx = val.toInt();

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL explosion
//! explosion (Explosion)
/*! ECMA-376, 21.2.2.61, p.3787.

 Parent elements:
  - dPt (§21.2.2.52)
  - ser (§21.2.2.172)

 Attributes:
  - [Done] val (Integer Value)
*/
KoFilter::ConversionStatus XlsxXmlChartReader::read_explosion()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    QString val(attrs.value("val").toString());
    *d->m_currentExplosion = val.toInt();

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL strRef
//! strRef (String Reference)
/*! ECMA-376, 21.2.2.201, p.3857.

 Parent elements:
  - cat (§21.2.2.24)
  - tx (§21.2.2.215)
  - tx (§21.2.2.214)
  - xVal (§21.2.2.234)

 Attributes:
  - extLst (Chart Extensibility) §21.2.2.64
  - [Done]f (Formula) §21.2.2.65
  - [Done]strCache (String Cache) §21.2.2.199
*/
KoFilter::ConversionStatus XlsxXmlChartReader::read_strRef()
{
    READ_PROLOGUE

    d->m_currentF = &d->m_currentStrRef->m_f;
    d->m_currentStrCache = &d->m_currentStrRef->m_strCache;

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(f)
            ELSE_TRY_READ_IF(strCache)
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL numRef
//! numRef (String Reference)
/*! ECMA-376, 21.2.2.123, p.3815.

 Parent elements:
  - bubbleSize (§21.2.2.22)
  - cat (§21.2.2.24)
  - minus (§21.2.2.113)
  - plus (§21.2.2.147)
  - val (§21.2.2.224)
  - xVal (§21.2.2.234)
  - yVal (§21.2.2.237)

 Child elements:
  - extLst (Chart Extensibility) §21.2.2.64
  - [Done]f (Formula) §21.2.2.65
  - [Done]numCache (Number Cache) §21.2.2.120
*/
KoFilter::ConversionStatus XlsxXmlChartReader::read_numRef()
{
    READ_PROLOGUE

    d->m_currentF = &d->m_currentNumRef->m_f;
    d->m_currentNumCache = &d->m_currentNumRef->m_numCache;

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(f)
            ELSE_TRY_READ_IF(numCache)
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL f
//! f (Formula)
/*! ECMA-376, 21.2.2.65, p.3789.

 Parent elements:
  - multiLvlStrRef (§21.2.2.115)
  - numRef (§21.2.2.123)
  - strRef (§21.2.2.201)
*/
KoFilter::ConversionStatus XlsxXmlChartReader::read_f()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    *d->m_currentF = readElementText();
    while (!atEnd()) {
        BREAK_IF_END_OF(CURRENT_EL);
        readNext();
    }

    if (d->m_currentF->size() != 0) {
        QPair<QString,QRect> result = splitCellRange( *d->m_currentF );
        m_context->m_chart->addRange( result.second );
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL ptCount
//! ptCount (Point Count)
/*! ECMA-376, 21.2.2.152, p.3832.

 Parent elements:
  - multiLvlStrCache (§21.2.2.114)
  - numCache (§21.2.2.120)
  - numLit (§21.2.2.122)
  - strCache (§21.2.2.199)
  - strLit (§21.2.2.200)

 Attributes:
  - [Done] val (Integer Value)
*/
KoFilter::ConversionStatus XlsxXmlChartReader::read_ptCount()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    QString val(attrs.value("val").toString());
    *d->m_currentPtCount = val.toInt();

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL strCache
//! strCache (String Cache)
/*! ECMA-376, 21.2.2.199, p.3856.

 Parent elements:
 - strRef (§21.2.2.201)

 Child elements:
  - extLst (Chart Extensibility) §21.2.2.64
  - [Done]pt (String Point) §21.2.2.151
  - [Done]ptCount (Point Count) §21.2.2.152

*/
KoFilter::ConversionStatus XlsxXmlChartReader::read_strCache()
{
    READ_PROLOGUE

    d->m_currentPtCount = &d->m_currentStrCache->m_ptCount;
    d->m_currentPtCache = &d->m_currentStrCache->m_cache;

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(ptCount)
            ELSE_TRY_READ_IF(pt)
        }
    }
    READ_EPILOGUE
}

int charToInt( const QString& string )
{
    if (string.isEmpty()) {
        return -1;
    }

    int ret = 0;
    int multiplier = 1;
    for(int i=string.size()-1; i>-1; i--,multiplier = multiplier*26) {
        char val = string[i].toAscii();
        if ( val >= 65 && val <= 90 ) {
            ret = ret +  (val - 64)*multiplier;
        } else {
            ret = -1;
            break;
        }
    }
    return ret;
}

QString XlsxXmlChartReader::AlocateAndWriteIntoInternalTable(QVector< QString > &buffer, QString format)
{
    if (buffer.size() == 0)
        return QString();

    //create range where to place the data
    QString range("local-table.");
    Charting::InternalTable *internalTable = &m_context->m_chart->m_internalTable;

    range += "!$" + columnName(internalTable->maxColumn()+1) +"$" + "1" + ":$" + columnName(internalTable->maxColumn()+1) +
             "$" + QString::number(buffer.size());

    WriteIntoInternalTable(range, buffer, format);
    return range;
}

void XlsxXmlChartReader::WriteIntoInternalTable(QString &range, QVector< QString > &buffer, QString format)
{
    if(range.isEmpty()) {
        return;
    }
    const QString sheet = range.section( '!', 0, 0 );
    const QString cellRange = range.section( '!', 1, -1 );
    const QStringList& res = cellRange.split( QRegExp( "[$:]" ), QString::SkipEmptyParts );

    if (res.isEmpty()) {
        return;
    }

    int startColumn = charToInt( res[ 0 ] );
    int startRow = res[ 1 ].toInt();
    int endColumn = 0;
    int endRow = 0;
    if (res.size() >= 4) {
        endColumn = charToInt( res[ 2 ] );
        endRow = res[ 3 ].toInt();
    } else {
        endColumn = startColumn ;
        endRow = startRow;
    }

//    kDebug()<<"range " << range;
//    kDebug()<<"sheet " << sheet;
//    kDebug()<<"cellRange " << cellRange;
//    kDebug()<<"startColumn " << startColumn;
//    kDebug()<<"startRow " << startRow;
//    kDebug()<<"endColumn " << endColumn;
//    kDebug()<<"endRow " << endRow;
//
//    kDebug()<<"buffer.size() " << buffer.size();

    Charting::InternalTable *internalTable = &m_context->m_chart->m_internalTable;
    if (startColumn < endColumn) {
        if ((endColumn - startColumn +1) == buffer.size()) {

            int bufferIndex = 0;
            for(int i = startColumn; i <=endColumn; i++,bufferIndex++) {
                Charting::Cell *cell = internalTable->cell(i,startRow,true);
                cell->m_valueType = format;
                cell->m_value = buffer[bufferIndex];
//                kDebug()<<"m_value " << format;
//                kDebug()<<"buffer[bufferIndex] " << buffer[bufferIndex];
//                kDebug()<<"cell row" << startRow;
//                kDebug()<<"cell column " << i;
            }
        }
    } else if (startRow < endRow){
        if ((endRow - startRow +1) == buffer.size()) {

            int bufferIndex = 0;
            for(int i = startRow; i <=endRow; i++,bufferIndex++) {
                Charting::Cell *cell = internalTable->cell(startColumn,i,true);
                cell->m_valueType = format;
                cell->m_value = buffer[bufferIndex];
//                kDebug()<<"m_value " << format;
//                kDebug()<<"buffer[bufferIndex] " << buffer[bufferIndex];
//                kDebug()<<"cell row" << i;
//                kDebug()<<"cell column " << startColumn;
            }
        }
    } else {
        if (buffer.size() != 0) {
            Charting::Cell *cell = internalTable->cell(startColumn,startRow,true);
            cell->m_valueType = format;
            cell->m_value = buffer[0];
//            kDebug()<<"m_value " << format;
//            kDebug()<<"buffer[bufferIndex] " << buffer[0];
//            kDebug()<<"cell row" << startRow;
//            kDebug()<<"cell column " << startColumn;
        }
    }
}
