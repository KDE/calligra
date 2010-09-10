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

class XlsxXmlChartReader::Private
{
public:
    Private( XlsxXmlChartReader * parent = NULL );
    Charting::Series::DomainIndexes m_currentDomainIndex;
    XlsxXmlChartReader *parent;
    QVariant::Type m_currentType;
    inline void readv();
};

XlsxXmlChartReader::Private::Private ( XlsxXmlChartReader* parent ) :
                                                            parent( parent ),
                                                            m_currentType( QVariant::String )
{

}

/*!
  appends values to the given table
 */
void appendDataValue( Charting::InternalDataTable& table, Charting::Series::DomainIndexes currentDim, const QVariant& value )
{
    QVector< QVariant > * corSeries;
    switch( currentDim )
    {
      case( Charting::Series::XAxis ):
          corSeries = &table.xValues;
          break;
      case( Charting::Series::YAxis ):
          corSeries = &table.yValues;
          break;
      case( Charting::Series::ZAxis ):
          corSeries = &table.zValues;
          break;
      default:
          corSeries = NULL;
        break;
    }
    if ( corSeries )
        corSeries->append( value );
}

void XlsxXmlChartReader::Private::readv()
{
    // since reading data in a read_v method via macro stuff invalidates the stream
    // this is done with this helper method
    const QString read =  parent->readElementText();
    QVariant value = read;
    if ( m_currentType == QVariant::Double )
        value = read.toDouble();
    appendDataValue( parent->m_currentSeries->internalData, m_currentDomainIndex, value );
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
    , d ( new Private( this ) )
{
}

XlsxXmlChartReader::~XlsxXmlChartReader()
{
    delete d;
}

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

    m_context->m_chartExport->m_notifyOnUpdateOfRanges = m_currentSeries->m_valuesCellRangeAddress; //m_cellRangeAddress
    
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
#define CURRENT_EL numLit
KoFilter::ConversionStatus XlsxXmlChartReader::read_numLit()
{
    READ_PROLOGUE
    d->m_currentType = QVariant::Double;
    while ( !atEnd() ) {
        readNext();
        if ( isStartElement() ) {        
            TRY_READ_IF( pt )
        }
        BREAK_IF_END_OF( CURRENT_EL );
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL pt
KoFilter::ConversionStatus XlsxXmlChartReader::read_pt()
{
    READ_PROLOGUE
    while ( !atEnd() ) {
        readNext();
        if ( isStartElement() ) {        
          if ( qualifiedName() == QLatin1String( QUALIFIED_NAME( v ) ) )
              d->readv();
        }
        BREAK_IF_END_OF( CURRENT_EL );
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
KoFilter::ConversionStatus XlsxXmlChartReader::read_plotArea()
{
    m_areaContext = PlotArea;
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(ser)
            ELSE_TRY_READ_IF(spPr)
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
            ELSE_TRY_READ_IF(scatter3DChart)
            ELSE_TRY_READ_IF(radarChart)
            ELSE_TRY_READ_IF(radar3DChart)
            ELSE_TRY_READ_IF(surfaceChart)
            ELSE_TRY_READ_IF(surface3DChart)
            ELSE_TRY_READ_IF(bubbleChart)
            ELSE_TRY_READ_IF(bubble3DChart)
            ELSE_TRY_READ_IF(stockChart)
            ELSE_TRY_READ_IF(stock3DChart)
            ELSE_TRY_READ_IF(barDir)
            ELSE_TRY_READ_IF(grouping)
            ELSE_TRY_READ_IF(firstSliceAng)
            ELSE_TRY_READ_IF(holeSize)
            //ELSE_TRY_READ_IF(bubbleSize)
            ELSE_TRY_READ_IF(bubbleScale)
        }
    }
    READ_EPILOGUE
    m_areaContext = ChartArea;
}

#undef CURRENT_EL
#define CURRENT_EL ser
/*! Read the series. */
KoFilter::ConversionStatus XlsxXmlChartReader::read_ser()
{
    READ_PROLOGUE
    
    m_currentSeries = new Charting::Series;
    m_context->m_chart->m_series << m_currentSeries;
    //m_currentSeries->m_dataTypeX = record->dataTypeX();
    //m_currentSeries->m_countXValues = record->countXValues();
    //m_currentSeries->m_countYValues = record->countYValues();
    //m_currentSeries->m_countBubbleSizeValues = record->countBubbleSizeValues();

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(val)
            ELSE_TRY_READ_IF(xVal)
            ELSE_TRY_READ_IF(yVal)
            ELSE_TRY_READ_IF(bubbleSize)
            ELSE_TRY_READ_IF(cat)
            ELSE_TRY_READ_IF(tx)
            ELSE_TRY_READ_IF(dLbl)
            ELSE_TRY_READ_IF(dLbls)
            if (qualifiedName() == QLatin1String(QUALIFIED_NAME(explosion))) {
                const QXmlStreamAttributes attrs(attributes());
                TRY_READ_ATTR_WITHOUT_NS(val)
                const int explosion = val.toInt();
                if(Charting::PieImpl* pie = dynamic_cast<Charting::PieImpl*>(m_context->m_chart->m_impl)) {
                    Q_UNUSED(pie);
                    m_currentSeries->m_datasetFormat << new Charting::PieFormat(explosion);
                }
            }
        }
    }

    READ_EPILOGUE
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
                TRY_READ_IF(tx)
        }
    }
    if ( m_context->m_chart->m_title.isEmpty() )
        m_context->m_chart->m_title = "Chart Title";
    m_readTxContext = None;
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL val
/*! Read the horizontal value. */
KoFilter::ConversionStatus XlsxXmlChartReader::read_val()
{
    READ_PROLOGUE
    d->m_currentDomainIndex = Charting::Series::YAxis;
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF( numCache )
            ELSE_TRY_READ_IF( strCache )
            if (qualifiedName() == QLatin1String(QUALIFIED_NAME(f))) {
                m_currentSeries->m_valuesCellRangeAddress = readElementText();
                QPair<QString,QRect> result = splitCellRange( m_currentSeries->m_valuesCellRangeAddress );
                m_context->m_chart->addRange( result.second );
            }
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL xVal
/*! Read the horizontal value. */
KoFilter::ConversionStatus XlsxXmlChartReader::read_xVal()
{
    READ_PROLOGUE
    d->m_currentDomainIndex = Charting::Series::XAxis;
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(numCache)
            ELSE_TRY_READ_IF( strCache )
            if ( qualifiedName() == QLatin1String( QUALIFIED_NAME( f ) ) ) {
                //m_currentSeries->m_valuesCellRangeAddress = readElementText();
//                 m_currentSeries->m_domainValuesCellRangeAddress.push_back( readElementText() );
                m_currentSeries->m_domainValuesCellRangeAddress[ Charting::Series::XAxis ] = readElementText();
                //QPair<QString,QRect> result = splitCellRange( m_currentSeries->m_valuesCellRangeAddress );
                //m_context->m_chart->addRange( result.second );
            }
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL yVal
/*! Read the vertical value. */
KoFilter::ConversionStatus XlsxXmlChartReader::read_yVal()
{
    READ_PROLOGUE
    d->m_currentDomainIndex = Charting::Series::YAxis;
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(numCache)
            ELSE_TRY_READ_IF( strCache )
            if ( qualifiedName() == QLatin1String( QUALIFIED_NAME( f ) ) ) {
                if ( m_currentSeries->m_valuesCellRangeAddress.isEmpty() )
                  m_currentSeries->m_valuesCellRangeAddress = readElementText();
                else
                  m_currentSeries->m_domainValuesCellRangeAddress[ Charting::Series::YAxis ] = readElementText();
                //QPair<QString,QRect> result = splitCellRange( m_currentSeries->m_valuesCellRangeAddress );
                //m_context->m_chart->addRange( result.second );
            }
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL cat
/*! Read the vertical category value. */
KoFilter::ConversionStatus XlsxXmlChartReader::read_cat()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            if (qualifiedName() == QLatin1String(QUALIFIED_NAME(f))) {
                m_context->m_chart->m_verticalCellRangeAddress = readElementText();
                QPair<QString,QRect> result = splitCellRange( m_context->m_chart->m_verticalCellRangeAddress );
                m_context->m_chart->addRange( result.second );
            }
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL tx

/*! This element specifies text to use on a chart, including rich text formatting. */
KoFilter::ConversionStatus XlsxXmlChartReader::read_tx()
{
    READ_PROLOGUE
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
                if (isStartElement() && !m_currentSeries->m_datasetValue.contains(Charting::Value::SeriesLegendOrTrendlineName)) {
                    if (qualifiedName() == QLatin1String(QUALIFIED_NAME(f))) {
                        Charting::Value* v = new Charting::Value(Charting::Value::SeriesLegendOrTrendlineName, Charting::Value::CellRange, readElementText());
                        m_currentSeries->m_datasetValue[v->m_dataId] = v;
                    } else if (qualifiedName() == QLatin1String(QUALIFIED_NAME(v))) {
                        Charting::Value* v = new Charting::Value(Charting::Value::SeriesLegendOrTrendlineName, Charting::Value::TextOrValue, readElementText());
                        m_currentSeries->m_datasetValue[v->m_dataId] = v;
                    }
                }
                break;
            case InRichText: // richtext means the title text
                // we extract the text from the richtext cause we cannot handle the richtext formattings anyway
                QString result;
                enum { Rich, Paragraph, TextRun } s;
                s = Rich;
                while (!atEnd()) {
                    readNext();
                    BREAK_IF_END_OF(rich);
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
#define CURRENT_EL numCache
KoFilter::ConversionStatus XlsxXmlChartReader::read_numCache()
{
    READ_PROLOGUE
    d->m_currentType = QVariant::Double;
    while ( !atEnd() ) {
        readNext();
        BREAK_IF_END_OF( CURRENT_EL );
        if ( isStartElement() ) 
        {
            TRY_READ_IF( pt )
            if ( qualifiedName() == QLatin1String( QUALIFIED_NAME ( ptCount ) ) )
            {
                const QXmlStreamAttributes attrs(attributes());
                TRY_READ_ATTR_WITHOUT_NS(val)
                m_currentSeries->m_countYValues = val.toInt();
            }
            //else if (qualifiedName() == QLatin1String(QUALIFIED_NAME(pt)))
            //else if (qualifiedName() == QLatin1String(QUALIFIED_NAME(formatCode)))
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL strCache
KoFilter::ConversionStatus XlsxXmlChartReader::read_strCache()
{
    READ_PROLOGUE
    d->m_currentType = QVariant::String;
    while (!atEnd()) {
        readNext();
        if ( isStartElement() ) {
            TRY_READ_IF( pt )
            if (qualifiedName() == QLatin1String( QUALIFIED_NAME( ptCount ) ) )
            {
                const QXmlStreamAttributes attrs(attributes());
                TRY_READ_ATTR_WITHOUT_NS(val)
                m_currentSeries->m_countYValues = val.toInt();
            }
            //else if (qualifiedName() == QLatin1String(QUALIFIED_NAME(pt)))
            //else if (qualifiedName() == QLatin1String(QUALIFIED_NAME(formatCode)))
        }
        BREAK_IF_END_OF( CURRENT_EL );
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
KoFilter::ConversionStatus XlsxXmlChartReader::read_dLbl()
{
    READ_PROLOGUE
      while (!atEnd()) {
          readNext();
          BREAK_IF_END_OF(CURRENT_EL);
          if ( qualifiedName() == "c:showVal" ) {
              m_currentSeries->m_showDataValues = true;
          }
      }
      READ_EPILOGUE
}
#undef CURRENT_EL
#define CURRENT_EL dLbls
KoFilter::ConversionStatus XlsxXmlChartReader::read_dLbls()
{
    READ_PROLOGUE
      while (!atEnd()) {
          readNext();
          BREAK_IF_END_OF(CURRENT_EL);
          if ( qualifiedName() == "c:showVal" ) {
              m_currentSeries->m_showDataValues = true;
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
                      if ( readingGradientStop )
                      {
                          currentStop.knownColorValue.setAlphaF( val.toDouble() / 100000.0 );
                      }
                      else
                      {
                          if ( m_areaContext == ChartArea )
                              m_context->m_chart->m_areaFormat->m_foreground.setAlphaF( val.toDouble() / 100000.0 );
                          else
                              m_context->m_chart->m_plotAreaFillColor.setAlphaF( val.toDouble() / 100000.0 );
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
KoFilter::ConversionStatus XlsxXmlChartReader::read_pieChart()
{
    if(!m_context->m_chart->m_impl) {
        m_context->m_chart->m_impl = new Charting::PieImpl();
    }
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL pie3DChart
KoFilter::ConversionStatus XlsxXmlChartReader::read_pie3DChart()
{
    if(!m_context->m_chart->m_impl) {
        m_context->m_chart->m_impl = new Charting::PieImpl();
        m_context->m_chart->m_is3d = true;
    }
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL doughnutChart
KoFilter::ConversionStatus XlsxXmlChartReader::read_doughnutChart()
{
    if(!m_context->m_chart->m_impl) {
        m_context->m_chart->m_impl = new Charting::RingImpl();
    }
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL areaChart
KoFilter::ConversionStatus XlsxXmlChartReader::read_areaChart()
{
    if(!m_context->m_chart->m_impl) {
        m_context->m_chart->m_impl = new Charting::AreaImpl();
    }
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL area3DChart
KoFilter::ConversionStatus XlsxXmlChartReader::read_area3DChart()
{
    if(!m_context->m_chart->m_impl) {
        m_context->m_chart->m_impl = new Charting::AreaImpl();
        m_context->m_chart->m_is3d = true;
    }
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL barChart
KoFilter::ConversionStatus XlsxXmlChartReader::read_barChart()
{
    if(!m_context->m_chart->m_impl) {
        m_context->m_chart->m_impl = new Charting::BarImpl();
    }
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL bar3DChart
KoFilter::ConversionStatus XlsxXmlChartReader::read_bar3DChart()
{
    if(!m_context->m_chart->m_impl) {
        m_context->m_chart->m_impl = new Charting::BarImpl();
        m_context->m_chart->m_is3d = true;
    }
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL lineChart
KoFilter::ConversionStatus XlsxXmlChartReader::read_lineChart()
{
    if(!m_context->m_chart->m_impl) {
        m_context->m_chart->m_impl = new Charting::LineImpl();
    }
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL line3DChart
KoFilter::ConversionStatus XlsxXmlChartReader::read_line3DChart()
{
    if(!m_context->m_chart->m_impl) {
        m_context->m_chart->m_impl = new Charting::LineImpl();
        m_context->m_chart->m_is3d = true;
    }
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL scatterChart
KoFilter::ConversionStatus XlsxXmlChartReader::read_scatterChart()
{
    if(!m_context->m_chart->m_impl) {
        m_context->m_chart->m_impl = new Charting::ScatterImpl();
    }
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL scatter3DChart
KoFilter::ConversionStatus XlsxXmlChartReader::read_scatter3DChart()
{
    if(!m_context->m_chart->m_impl) {
        m_context->m_chart->m_impl = new Charting::ScatterImpl();
        m_context->m_chart->m_is3d = true;
    }
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL radarChart
KoFilter::ConversionStatus XlsxXmlChartReader::read_radarChart()
{
    if(!m_context->m_chart->m_impl) {
        m_context->m_chart->m_impl = new Charting::RadarImpl();
    }
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL radar3DChart
KoFilter::ConversionStatus XlsxXmlChartReader::read_radar3DChart()
{
    if(!m_context->m_chart->m_impl) {
        m_context->m_chart->m_impl = new Charting::RadarImpl();
        m_context->m_chart->m_is3d = true;
    }
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL surfaceChart
KoFilter::ConversionStatus XlsxXmlChartReader::read_surfaceChart()
{
    if(!m_context->m_chart->m_impl) {
        m_context->m_chart->m_impl = new Charting::SurfaceImpl();
    }
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL surface3DChart
KoFilter::ConversionStatus XlsxXmlChartReader::read_surface3DChart()
{
    if(!m_context->m_chart->m_impl) {
        m_context->m_chart->m_impl = new Charting::SurfaceImpl();
        m_context->m_chart->m_is3d = true;
    }
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL bubbleChart
KoFilter::ConversionStatus XlsxXmlChartReader::read_bubbleChart()
{
    if(!m_context->m_chart->m_impl) {
        m_context->m_chart->m_impl = new Charting::BubbleImpl();
    }
    return KoFilter::OK;
}
#undef CURRENT_EL
#define CURRENT_EL bubble3DChart
KoFilter::ConversionStatus XlsxXmlChartReader::read_bubble3DChart()
{
    if(!m_context->m_chart->m_impl) {
        m_context->m_chart->m_impl = new Charting::BubbleImpl();
        m_context->m_chart->m_is3d = true;
    }
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL stockChart
KoFilter::ConversionStatus XlsxXmlChartReader::read_stockChart()
{
    if(!m_context->m_chart->m_impl) {
        m_context->m_chart->m_impl = new Charting::StockImpl();
    }
    return KoFilter::OK;
}
#undef CURRENT_EL
#define CURRENT_EL stock3DChart
KoFilter::ConversionStatus XlsxXmlChartReader::read_stock3DChart()
{
    if(!m_context->m_chart->m_impl) {
        m_context->m_chart->m_impl = new Charting::StockImpl();
        m_context->m_chart->m_is3d = true;
    }
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL barDir
KoFilter::ConversionStatus XlsxXmlChartReader::read_barDir()
{
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(val)
    m_context->m_chart->m_transpose = (val == "bar"); // "bar" or "col"
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL grouping
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
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL firstSliceAng
KoFilter::ConversionStatus XlsxXmlChartReader::read_firstSliceAng()
{
    if(Charting::PieImpl* pie = dynamic_cast<Charting::PieImpl*>(m_context->m_chart->m_impl)) {
        const QXmlStreamAttributes attrs(attributes());
        TRY_READ_ATTR(val)
        pie->m_anStart = val.toInt(); // default value is zero
    }
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL holeSize
KoFilter::ConversionStatus XlsxXmlChartReader::read_holeSize()
{
    if(Charting::RingImpl* ring = dynamic_cast<Charting::RingImpl*>(m_context->m_chart->m_impl)) {
        const QXmlStreamAttributes attrs(attributes());
        TRY_READ_ATTR(val)
        ring->m_pcDonut = val.toInt(); // default value is zero
    }
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL bubbleSize
KoFilter::ConversionStatus XlsxXmlChartReader::read_bubbleSize()
{
    READ_PROLOGUE
    d->m_currentDomainIndex = Charting::Series::ZAxis;
    if (!m_currentSeries->m_valuesCellRangeAddress.isEmpty() )
                  m_currentSeries->m_domainValuesCellRangeAddress.append( m_currentSeries->m_valuesCellRangeAddress );
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF( numLit )
            ELSE_TRY_READ_IF( numCache )
            if ( qualifiedName() == QLatin1String( QUALIFIED_NAME( f ) ) )
            {         
                if ( !m_currentSeries->m_valuesCellRangeAddress.isEmpty() )
                    m_currentSeries->m_domainValuesCellRangeAddress[ Charting::Series::YAxis ] =  m_currentSeries->m_valuesCellRangeAddress;
                m_currentSeries->m_valuesCellRangeAddress = readElementText();
                /*QPair<QString,QRect> result = splitCellRange( m_currentSeries->m_valuesCellRangeAddress );
                m_context->m_chart->addRange( result.second );*/
            }
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL bubbleScale
KoFilter::ConversionStatus XlsxXmlChartReader::read_bubbleScale()
{
    if(Charting::BubbleImpl* bubble = dynamic_cast<Charting::BubbleImpl*>(m_context->m_chart->m_impl)) {
        const QXmlStreamAttributes attrs(attributes());
        TRY_READ_ATTR(val)
        bool ok;
        const int i = val.toInt(&ok);
        if(ok)
            bubble->m_sizeRatio = i;
    }
    return KoFilter::OK;
}
