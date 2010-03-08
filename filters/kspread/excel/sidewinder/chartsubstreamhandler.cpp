/* Swinder - Portable library for spreadsheet
   Copyright (C) 2009-2010 Sebastian Sauer <sebsauer@kdab.com>

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
   Boston, MA 02110-1301, USA
 */

#include "chartsubstreamhandler.h"
#include "globalssubstreamhandler.h"
#include "worksheetsubstreamhandler.h"

#include <math.h>

#include <QRegExp>

namespace Swinder {

// translate the range-character to a number
int rangeCharToInt(char c)
{
    return (c >= 'A' && c <= 'Z') ? (c - 'A' + 1) : -1;
}

// translates the range-string into a number
int rangeStringToInt(const QString &string)
{
    int result = 0;
    const int size = string.size();
    for ( int i = 0; i < size; i++ )
        result += rangeCharToInt( string[i].toAscii() ) * pow( 10.0, ( size - i - 1 ) );
    return result;
}

// splits a given cellrange like Sheet1.D2:Sheet1.F2, Sheet1.D2:F2, D2:F2 or D2 into its parts
QPair<QString,QRect> splitCellRange(QString range)
{
    range.remove( "$" ); // remove "fixed" character
    if(range.startsWith('[') && range.endsWith(']')) range = range.mid(1, range.length() - 2); // remove []
    QPair<QString,QRect> result;
    const bool isPoint = !range.contains( ':' );
    QRegExp regEx = isPoint ? QRegExp( "(.*)\\.([A-Z]+)([0-9]+)" ) : QRegExp ( "(.*)\\.([A-Z]+)([0-9]+)\\:(|.*\\.)([A-Z]+)([0-9]+)" );
    if ( regEx.indexIn( range ) >= 0 ) {
        const QString sheetName = regEx.cap( 1 );
        QPoint topLeft( rangeStringToInt( regEx.cap(2) ), regEx.cap(3).toInt() );
        if ( isPoint ) {
            result = QPair<QString,QRect>(sheetName, QRect(topLeft,QSize(1,1)));
        } else {
            QPoint bottomRight( rangeStringToInt( regEx.cap(5) ), regEx.cap(6).toInt() );
            result = QPair<QString,QRect>(sheetName, QRect(topLeft,bottomRight));
        }
    }
    return result;
}

class BRAIRecord : public Record
{
public:
    ChartObject::Value* m_value;

    static const unsigned int id;
    unsigned int rtti() const { return this->id; }
    virtual const char* name() const { return "BRAI"; }
    static Record *createRecord(Workbook *book, void *arg) { return new BRAIRecord(book, arg); }

    BRAIRecord(Swinder::Workbook *book, void *arg) : Record(book), m_handler(static_cast<ChartSubStreamHandler*>(arg))
    {
        m_worksheetHandler = dynamic_cast<WorksheetSubStreamHandler*>(m_handler->parentHandler());
        m_value = 0;
    }
    virtual ~BRAIRecord() { delete m_value; }

    virtual void dump(std::ostream&) const { /*TODO*/ }

    virtual void setData(unsigned size, const unsigned char* data, const unsigned int* /*continuePositions*/)
    {
        if (size < 8) {
            setIsValid(false);
            return;
        }

        ChartObject::Value::DataId dataId = (ChartObject::Value::DataId) readU8(data);
        ChartObject::Value::DataType type = (ChartObject::Value::DataType) readU8(data + 1);
        bool isUnlinkedFormat = readU16(data + 2) & 0x01;
        unsigned numberFormat = readU16(data + 4);

        UString formula;
        if(m_worksheetHandler) {
            FormulaTokens tokens = m_worksheetHandler->decodeFormula(size, 6, data, version());
            formula = m_worksheetHandler->decodeFormula(0, 0, true, tokens);
        } else {
            FormulaTokens tokens = m_handler->globals()->decodeFormula(size, 6, data, version());
            formula = m_handler->globals()->decodeFormula(0, 0, true, tokens);
        }

        if(m_value) delete m_value;
        m_value = new ChartObject::Value(dataId, type, isUnlinkedFormat, numberFormat, formula);
    }

private:
    ChartSubStreamHandler* m_handler;
    WorksheetSubStreamHandler* m_worksheetHandler;
};

const unsigned BRAIRecord::id = 0x1051;

} // namespace Swinder

using namespace Swinder;

ChartSubStreamHandler::ChartSubStreamHandler(GlobalsSubStreamHandler* globals, SubStreamHandler* parentHandler)
    : SubStreamHandler(), m_globals(globals), m_parentHandler(parentHandler), m_chart(0), m_sheet(0), m_currentSeries(0), m_currentObj(0), m_level(0)
{
    RecordRegistry::registerRecordClass(BRAIRecord::id, BRAIRecord::createRecord, this);

    if(WorksheetSubStreamHandler* worksheetHandler = dynamic_cast<WorksheetSubStreamHandler*>(parentHandler)) {
        m_sheet = worksheetHandler->sheet();
        Q_ASSERT(m_sheet);

        std::vector<unsigned long>& charts = worksheetHandler->charts();
        Q_ASSERT(!charts.empty());
        const unsigned long id = charts.back();

        std::map<unsigned long, Object*>::iterator it = worksheetHandler->sharedObjects().find(id);
        Q_ASSERT(it != worksheetHandler->sharedObjects().end());
        //worksheetHandler->sharedObjects().erase(id); // remove from the sharedObjects and take over ownership
        m_chart = dynamic_cast<ChartObject*>(it->second);
        Q_ASSERT(m_chart);

        //unsigned long m_colL, m_dxL, m_rwT, m_dyT, m_colR, m_dxR, m_rwB, m_dyB;
        Cell* cell = m_sheet->cell(m_chart->drawingObject()->m_colL, m_chart->drawingObject()->m_rwT, true);
        cell->addChart(m_chart);
    } else {
        std::cerr << "ChartSubStreamHandler: Chart is not embedded into a worksheet. This is not handled yet." << std::endl;
        //TODO
    }
}

ChartSubStreamHandler::~ChartSubStreamHandler()
{
    RecordRegistry::unregisterRecordClass(BRAIRecord::id);
}

void ChartSubStreamHandler::handleRecord(Record* record)
{
    if (!record) return;
    const unsigned type = record->rtti();

    if (type == BOFRecord::id)
        handleBOF(static_cast<BOFRecord*>(record));
    else if (type == EOFRecord::id)
        handleEOF(static_cast<EOFRecord*>(record));
    else if (type == FooterRecord::id)
        handleFooter(static_cast<FooterRecord*>(record));
    else if (type == HeaderRecord::id)
        handleHeader(static_cast<HeaderRecord*>(record));
    else if (type == SetupRecord::id)
        handleSetup(static_cast<SetupRecord*>(record));
    else if (type == HCenterRecord::id)
        handleHCenter(static_cast<HCenterRecord*>(record));
    else if (type == VCenterRecord::id)
        handleVCenter(static_cast<VCenterRecord*>(record));
    else if (type == ZoomLevelRecord::id)
        handleZoomLevel(static_cast<ZoomLevelRecord*>(record));
    else if (type == DimensionRecord::id)
        handleDimension(static_cast<DimensionRecord*>(record));
    else if (type == ChartRecord::id)
        handleChart(static_cast<ChartRecord*>(record));
    else if (type == BeginRecord::id)
        handleBegin(static_cast<BeginRecord*>(record));
    else if (type == EndRecord::id)
        handleEnd(static_cast<EndRecord*>(record));
    else if (type == FrameRecord::id)
        handleFrame(static_cast<FrameRecord*>(record));
    else if (type == SeriesRecord::id)
        handleSeries(static_cast<SeriesRecord*>(record));
    else if (type == DataFormatRecord::id)
        handleDataFormat(static_cast<DataFormatRecord*>(record));
    else if (type == Chart3DBarShapeRecord::id)
        handleChart3DBarShape(static_cast<Chart3DBarShapeRecord*>(record));
    else if (type == Chart3dRecord::id)
        handleChart3d(static_cast<Chart3dRecord*>(record));
    else if (type == LineFormatRecord::id)
        handleLineFormat(static_cast<LineFormatRecord*>(record));
    else if (type == AreaFormatRecord::id)
        handleAreaFormat(static_cast<AreaFormatRecord*>(record));
    else if (type == PieFormatRecord::id)
        handlePieFormat(static_cast<PieFormatRecord*>(record));
    else if (type == MarkerFormatRecord::id)
        handleMarkerFormat(static_cast<MarkerFormatRecord*>(record));
    else if (type == ChartFormatRecord::id)
        handleChartFormat(static_cast<ChartFormatRecord*>(record));
    else if (type == GelFrameRecord::id)
        handleGelFrame(static_cast<GelFrameRecord*>(record));
    else if (type == SerToCrtRecord::id)
        handleSerToCrt(static_cast<SerToCrtRecord*>(record));
    else if (type == ShtPropsRecord::id)
        handleShtProps(static_cast<ShtPropsRecord*>(record));
    else if (type == DefaultTextRecord::id)
        handleDefaultText(static_cast<DefaultTextRecord*>(record));
    else if (type == TextRecord::id)
        handleText(static_cast<TextRecord*>(record));
    else if (type == SeriesTextRecord::id)
        handleSeriesText(static_cast<SeriesTextRecord*>(record));
    else if (type == PosRecord::id)
        handlePos(static_cast<PosRecord*>(record));
    else if (type == FontXRecord::id)
        handleFontX(static_cast<FontXRecord*>(record));
    else if (type == PlotGrowthRecord::id)
        handlePlotGrowth(static_cast<PlotGrowthRecord*>(record));
    else if (type == LegendRecord::id)
        handleLegend(static_cast<LegendRecord*>(record));
    else if (type == AxesUsedRecord::id)
        handleAxesUsed(static_cast<AxesUsedRecord*>(record));
    else if (type == AxisParentRecord::id)
        handleAxisParent(static_cast<AxisParentRecord*>(record));
    else if (type == BRAIRecord::id)
        handleBRAI(static_cast<BRAIRecord*>(record));
    else if (type == PieRecord::id)
        handlePie(static_cast<PieRecord*>(record));
    else if (type == SIIndexRecord::id)
        handleSIIndex(static_cast<SIIndexRecord*>(record));
    else if (type == MsoDrawingRecord::id)
        handleMsoDrawing(static_cast<MsoDrawingRecord*>(record));
    else if (type == LeftMarginRecord::id)
        handleLeftMargin(static_cast<LeftMarginRecord*>(record));
    else if (type == RightMarginRecord::id)
        handleRightMargin(static_cast<RightMarginRecord*>(record));
    else if (type == TopMarginRecord::id)
        handleTopMargin(static_cast<TopMarginRecord*>(record));
    else if (type == BottomMarginRecord::id)
        handleBottomMargin(static_cast<BottomMarginRecord*>(record));
    else if (type == ShapePropsStreamRecord::id)
        handleShapePropsStream(static_cast<ShapePropsStreamRecord*>(record));
    else if (type == TextPropsStreamRecord::id)
        handleTextPropsStream(static_cast<TextPropsStreamRecord*>(record));
    else if (type == ObjectLinkRecord::id)
        handleObjectLink(static_cast<ObjectLinkRecord*>(record));
    else if (type == CrtLinkRecord::id)
        {} // written but unused record
    else if (type == UnitsRecord::id)
        {} // written but must be ignored
    else if (type == StartBlockRecord::id || type == EndBlockRecord::id)
        {} // not evaluated atm
    else {
        std::cout << "Unhandled chart record with type=" << type << " name=" << record->name() << std::endl;
        //record->dump(std::cout);
    }
}

std::string whitespaces(int number)
{
    std::string s;
    for(int i = 0; i < number; ++i) s += " ";
    return s;
}

#define DEBUG \
    std::cout << whitespaces(m_level) << "ChartSubStreamHandler::" << __FUNCTION__ << " "

void ChartSubStreamHandler::handleBOF(BOFRecord*)
{
    //DEBUG << std::endl;
}

void ChartSubStreamHandler::handleEOF(EOFRecord *)
{
    //DEBUG << std::endl;
}

void ChartSubStreamHandler::handleFooter(FooterRecord *)
{
}

void ChartSubStreamHandler::handleHeader(HeaderRecord *)
{
}

void ChartSubStreamHandler::handleSetup(SetupRecord *)
{
}

void ChartSubStreamHandler::handleHCenter(HCenterRecord *)
{
}

void ChartSubStreamHandler::handleVCenter(VCenterRecord *)
{
}

void ChartSubStreamHandler::handleZoomLevel(ZoomLevelRecord *)
{
}

void ChartSubStreamHandler::handleLeftMargin(LeftMarginRecord* record)
{
    if(!record) return;
    m_chart->leftMargin = record->leftMargin();
}

void ChartSubStreamHandler::handleRightMargin(RightMarginRecord* record)
{
    if(!record) return;
    m_chart->rightMargin = record->rightMargin();
}

void ChartSubStreamHandler::handleTopMargin(TopMarginRecord* record)
{
    if(!record) return;
    m_chart->topMargin = record->topMargin();
}

void ChartSubStreamHandler::handleBottomMargin(BottomMarginRecord* record)
{
    if(!record) return;
    m_chart->bottomMargin = record->bottomMargin();
}

void ChartSubStreamHandler::handleDimension(DimensionRecord *record)
{
    if(!record) return;
    DEBUG << "firstRow=" << record->firstRow() << " lastRowPlus1=" << record->lastRowPlus1() << " firstColumn=" << record->firstColumn() << " lastColumnPlus1=" << record->lastColumnPlus1() << " lastRow=" << record->lastRow() << " lastColumn=" << record->lastColumn() << std::endl;
}

void ChartSubStreamHandler::handleChart(ChartRecord *record)
{
    if(!record) return;
    DEBUG << "x=" << record->x() << " y=" << record->y() << " width=" << record->width() << " height=" << record->height() <<  std::endl;
    m_chart->x = record->x();
    m_chart->y = record->y();
    m_chart->width = record->width();
    m_chart->height = record->height();
}


// secifies the begin of a collection of records
void ChartSubStreamHandler::handleBegin(BeginRecord *)
{
    ++m_level;
}

// sepcified the end of a collection of records
void ChartSubStreamHandler::handleEnd(EndRecord *)
{
    --m_level;
}

void ChartSubStreamHandler::handleFrame(FrameRecord *record)
{
    if(!record) return;
    if(record->isAutoPosition()) {
        m_chart->x = -1;
        m_chart->y = -1;
    }
    if(record->isAutoSize()) {
        m_chart->width = -1;
        m_chart->height = -1;
    }
}

// properties of the data for series, trendlines or errorbars
void ChartSubStreamHandler::handleSeries(SeriesRecord *record)
{
    if(!record) return;
    DEBUG << "dataTypeX=" << record->dataTypeX() << " dataTypeY=" << record->dataTypeY() << " countXValues=" << record->countXValues() << " countYValues=" << record->countYValues() << " bubbleSizeDataType=" << record->bubbleSizeDataType() << " countBubbleSizeValues=" << record->countBubbleSizeValues() << std::endl;
    m_currentSeries = new ChartObject::Series;
    m_currentSeries->dataTypeX = record->dataTypeX();
    m_currentSeries->countXValues = record->countXValues();
    m_currentSeries->countYValues = record->countYValues();
    m_currentSeries->countBubbleSizeValues = record->countBubbleSizeValues();
    m_chart->series << m_currentSeries;
}

// specifies a reference to data in a sheet that is used by a part of a series, legend entry, trendline or error bars.
void ChartSubStreamHandler::handleBRAI(BRAIRecord *record)
{
    if(!record) return;
    if(!m_currentSeries) return;
    DEBUG << "dataId=" << record->m_value->dataId << " type=" << record->m_value->type << " isUnlinkedNumberFormat=" << record->m_value->isUnlinkedFormat << " numberFormat=" << record->m_value->numberFormat << " formula=" << record->m_value->formula << std::endl;

    //FIXME is that correct or do we need to take the series somehow into account to provide one cellRangeAddress per series similar to valuesCellRangeAddress?
    //FIXME handle VerticalValues and BubbleSizeValues
    if(!record->m_value->formula.isEmpty()) {
        if(record->m_value->type == ChartObject::Value::TextOrValue || record->m_value->type == ChartObject::Value::CellRange) {
            if(record->m_value->dataId == ChartObject::Value::HorizontalValues)
                m_currentSeries->valuesCellRangeAddress = record->m_value->formula;
            else if(record->m_value->dataId == ChartObject::Value::VerticalValues)
                m_chart->verticalCellRangeAddress = record->m_value->formula;

            //FIXME we are ignoring the sheetname here but we probably should handle the case where a series is made from different sheets...
            QPair<QString,QRect> result = splitCellRange( QString::fromRawData(reinterpret_cast<const QChar*>(record->m_value->formula.data()), record->m_value->formula.length()) );
            QRect r = result.second;
            if(r.isValid() && m_chart->cellRangeAddress.isValid()) {
                if(r.left() < m_chart->cellRangeAddress.left()) m_chart->cellRangeAddress.setLeft(r.left());
                if(r.top() < m_chart->cellRangeAddress.top()) m_chart->cellRangeAddress.setTop(r.top());
                if(r.right() > m_chart->cellRangeAddress.right()) m_chart->cellRangeAddress.setRight(r.right());
                if(r.bottom() > m_chart->cellRangeAddress.bottom()) m_chart->cellRangeAddress.setBottom(r.bottom());
            } else {
                m_chart->cellRangeAddress = r;
            }
        }
    }

    m_currentSeries->datasetValue[record->m_value->dataId] = record->m_value;
    record->m_value = 0; //take over ownership
}

void ChartSubStreamHandler::handleDataFormat(DataFormatRecord *record)
{
    if(!record) return;
    DEBUG << "xi=" << record->xi() << " yi=" << record->yi() << " iss=" << record->iss() << std::endl;
    //TODO
}

void ChartSubStreamHandler::handleChart3DBarShape(Chart3DBarShapeRecord * record)
{
    if(!record) return;
    DEBUG << std::endl;



}

// specifies that chart is rendered in 3d scene
void ChartSubStreamHandler::handleChart3d(Chart3dRecord *record)
{
    if(!record) return;
    DEBUG << "anRot=" << record->anRot() << " anElev=" << record->anElev() << " pcDist=" << record->pcDist() << " pcHeight=" << record->pcHeight() << " pcDepth=" << record->pcDepth() << std::endl;
    m_chart->is3d = true;
    //TODO
}

void ChartSubStreamHandler::handleLineFormat(LineFormatRecord *record)
{
    if(!record) return;
    DEBUG << std::endl;
    //TODO
}

void ChartSubStreamHandler::handleAreaFormat(AreaFormatRecord *record)
{
    if(!record) return;
    DEBUG << std::endl;
    //TODO
}

void ChartSubStreamHandler::handlePieFormat(PieFormatRecord *record)
{
    if(!record) return;
    if(!m_currentSeries) return;
    DEBUG << "pcExplode="<<record->pcExplode()<<std::endl;
    m_currentSeries->datasetFormat << new ChartObject::PieFormat(record->pcExplode());
}

void ChartSubStreamHandler::handleMarkerFormat(MarkerFormatRecord *record)
{
    if(!record) return;
    DEBUG << std::endl;
    //TODO
}

void ChartSubStreamHandler::handleChartFormat(ChartFormatRecord *record)
{
    if(!record) return;
    DEBUG << std::endl;
    //TODO
}

void ChartSubStreamHandler::handleGelFrame(GelFrameRecord *record)
{
    if(!record) return;
    DEBUG << std::endl;
    //TODO
}

// specifies the chartgroup for the current series
void ChartSubStreamHandler::handleSerToCrt(SerToCrtRecord *record)
{
    if(!record) return;
    DEBUG << "id=" << record->identifier() << std::endl;
}

// properties
void ChartSubStreamHandler::handleShtProps(ShtPropsRecord *record)
{
    if(!record) return;
    DEBUG << std::endl;
    //TODO
}

// specifies the text elements that are formatted using the information specified by the Text record
// immediately following this record.
void ChartSubStreamHandler::handleDefaultText(DefaultTextRecord *record)
{
    if(!record) return;
    DEBUG << "id=" << record->identifier() << std::endl;
    //TODO
}

// specifies the properties of an attached label
void ChartSubStreamHandler::handleText(TextRecord *record)
{
    if(!record) return;
    DEBUG << std::endl;
    m_currentObj = new ChartObject::Text;
}

void ChartSubStreamHandler::handleSeriesText(SeriesTextRecord* record)
{
    if(!record) return;
    DEBUG << "text=" << record->text() << std::endl;
    if(ChartObject::Text *t = dynamic_cast<ChartObject::Text*>(m_currentObj))
        t->text = record->text();
}

void ChartSubStreamHandler::handlePos(PosRecord *record)
{
    if(!record) return;
    DEBUG << "mdTopLt=" << record->mdTopLt() << " mdBotRt=" << record->mdBotRt() << " x1=" << record->x1() << " y1=" << record->y1() << " x2=" << record->x2() << " y2=" << record->y2() << std::endl;
    if(m_currentObj) {
        m_currentObj->mdBotRt = record->mdBotRt();
        m_currentObj->mdTopLt = record->mdTopLt();
        m_currentObj->x1 = record->x1();
        m_currentObj->y1 = record->y1();
        m_currentObj->x2 = record->x2();
        m_currentObj->y2 = record->y2();
    }
}

void ChartSubStreamHandler::handleFontX(FontXRecord *record)
{
    if(!record) return;
    DEBUG << std::endl;
    //TODO
}

void ChartSubStreamHandler::handlePlotGrowth(PlotGrowthRecord *record)
{
    if(!record) return;
    DEBUG << std::endl;
    //TODO
}

void ChartSubStreamHandler::handleLegend(LegendRecord *record)
{
    if(!record) return;
    DEBUG << std::endl;
    //TODO
}

void ChartSubStreamHandler::handleAxesUsed(AxesUsedRecord *record)
{
    if(!record) return;
    DEBUG << std::endl;
    //TODO
}

void ChartSubStreamHandler::handleAxisParent(AxisParentRecord *record)
{
    if(!record) return;
    DEBUG << std::endl;
    //TODO
}

// specifies that the chartgroup is a pie chart
void ChartSubStreamHandler::handlePie(PieRecord *record)
{
    if(!record) return;
    DEBUG << "anStart=" << record->anStart() << " pcDonut=" << record->pcDonut() << std::endl;
    m_chart->impl = new ChartObject::PieImpl(record->anStart(), record->pcDonut());
}

// type of data contained in the Number records following
void ChartSubStreamHandler::handleSIIndex(SIIndexRecord *record)
{
    if(!record) return;
    DEBUG << "numIndex=" << record->numIndex() << std::endl;
    /*TODO
    0x0001 Series values or vertical values (for scatter or bubble chart groups)
    0x0002 Category labels or horizontal values (for scatter or bubble chart groups)
    0x0003 Bubble sizes
    */
}

void ChartSubStreamHandler::handleMsoDrawing(MsoDrawingRecord* record)
{
    if(!record) return;
    DEBUG << std::endl;
    //TODO
}

void ChartSubStreamHandler::handleShapePropsStream(ShapePropsStreamRecord* record)
{
    if(!record) return;
    DEBUG << "rgb=" << record->rgb().length() << " " << record->rgb() << std::endl;
    //TODO
}

void ChartSubStreamHandler::handleTextPropsStream(TextPropsStreamRecord* record)
{
    if(!record) return;
    DEBUG << "rgb=" << record->rgb().length() << " " << record->rgb() << std::endl;
    //TODO
}

void ChartSubStreamHandler::handleObjectLink(ObjectLinkRecord *record)
{
    if(!record) return;
    DEBUG << "wLinkObj=" << record->wLinkObj() << " wLinkVar1=" << record->wLinkVar1() << " wLinkVar2=" << record->wLinkVar2() << std::endl;

    ChartObject::Text *t = dynamic_cast<ChartObject::Text*>(m_currentObj);
    if(!t) return;

    switch(record->wLinkObj()) {
        case ObjectLinkRecord::EntireChart: {
            m_chart->texts << t;
        } break;
        case ObjectLinkRecord::ValueOrVerticalAxis: break; //TODO
        case ObjectLinkRecord::CategoryOrHorizontalAxis: break; //TODO
        case ObjectLinkRecord::SeriesOrDatapoints: {
            if(record->wLinkVar1() < 0 || record->wLinkVar1() >= m_chart->series.count()) return;
            ChartObject::Series* series = m_chart->series[ record->wLinkVar1() ];
            if(record->wLinkVar2() == 0xFFFF) {
                //TODO series->texts << t;
            } else {
                //TODO series->category[record->wLinkVar2()];
            }
        } break;
        case ObjectLinkRecord::SeriesAxis: break; //TODO
        case ObjectLinkRecord::DisplayUnitsLabelsOfAxis: break; //TODO
    }
}
