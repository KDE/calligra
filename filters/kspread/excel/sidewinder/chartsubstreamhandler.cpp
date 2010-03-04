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

namespace Swinder {

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
    : SubStreamHandler(), m_globals(globals), m_parentHandler(parentHandler), m_chart(0), m_sheet(0), m_currentSeries(0)
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

void ChartSubStreamHandler::handleBOF(BOFRecord*)
{
    //std::cout << "ChartSubStreamHandler BOFRecord" << std::endl;
}

void ChartSubStreamHandler::handleEOF(EOFRecord *)
{
    //std::cout << "ChartSubStreamHandler EOFRecord" << std::endl;
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

void ChartSubStreamHandler::handleDimension(DimensionRecord *record)
{
    if(!record) return;
    std::cout << "ChartSubStreamHandler::handleDimension firstRow=" << record->firstRow() << " lastRowPlus1=" << record->lastRowPlus1() << " firstColumn=" << record->firstColumn() << " lastColumnPlus1=" << record->lastColumnPlus1() << " lastRow=" << record->lastRow() << " lastColumn=" << record->lastColumn() << std::endl;
}

void ChartSubStreamHandler::handleChart(ChartRecord *record)
{
    if(!record) return;
    std::cout << "ChartSubStreamHandler::handleChart x=" << record->x() << " y=" << record->y() << " width=" << record->width() << " height=" << record->height() <<  std::endl;
    m_chart->x = record->x();
    m_chart->y = record->y();
    m_chart->width = record->width();
    m_chart->height = record->height();
}

// secifies the begin of a collection of records
void ChartSubStreamHandler::handleBegin(BeginRecord *)
{
    //std::cout << "ChartSubStreamHandler::handleBegin" << std::endl;
}

// sepcified the end of a collection of records
void ChartSubStreamHandler::handleEnd(EndRecord *)
{
    //std::cout << "ChartSubStreamHandler::handleEnd" << std::endl;
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
    std::cout << "ChartSubStreamHandler::handleSeries dataTypeX=" << record->dataTypeX() << " dataTypeY=" << record->dataTypeY() << " countXValues=" << record->countXValues() << " countYValues=" << record->countYValues() << " bubbleSizeDataType=" << record->bubbleSizeDataType() << " countBubbleSizeValues=" << record->countBubbleSizeValues() << std::endl;
    m_currentSeries = new ChartObject::Series;
    m_currentSeries->dataTypeX = record->dataTypeX();
    m_currentSeries->countXValues = record->countXValues();
    m_currentSeries->countYValues = record->countYValues();
    m_currentSeries->countBubbleSizeValues = record->countBubbleSizeValues();
    m_chart->series << m_currentSeries;
}

void ChartSubStreamHandler::handleBRAI(BRAIRecord *record)
{
    if(!record) return;
    if(!m_currentSeries) return;
    std::cout << "ChartSubStreamHandler::handleBRAI dataId=" << record->m_value->dataId << " type=" << record->m_value->type << " isUnlinkedNumberFormat=" << record->m_value->isUnlinkedFormat << " numberFormat=" << record->m_value->numberFormat << " formula=" << record->m_value->formula << std::endl;

    //FIXME is that correct or do we need to take the series somehow into account to provide one cellRangeAddress per series similar to valuesCellRangeAddress?
    //FIXME handle VerticalValues and BubbleSizeValues
    if(record->m_value->dataId == ChartObject::Value::HorizontalValues) {
        if(record->m_value->type == ChartObject::Value::TextOrValue || record->m_value->type == ChartObject::Value::CellRange) {
            if(!record->m_value->formula.isEmpty()) {
                m_currentSeries->valuesCellRangeAddress = record->m_value->formula;
                m_chart->cellRangeAddress = record->m_value->formula;
            }
        }
    }

    m_currentSeries->datasetValue[record->m_value->dataId] = record->m_value;
    record->m_value = 0; //take over ownership
}

void ChartSubStreamHandler::handleDataFormat(DataFormatRecord *record)
{
    if(!record) return;
    std::cout << "ChartSubStreamHandler::handleDataFormat xi=" << record->xi() << " yi=" << record->yi() << " iss=" << record->iss() << std::endl;
    //TODO
}

void ChartSubStreamHandler::handleChart3DBarShape(Chart3DBarShapeRecord * record)
{
    if(!record) return;
    std::cout << "ChartSubStreamHandler::handleChart3DBarShape" << std::endl;
    //TODO
}

// specifies that chart is rendered in 3d scene
void ChartSubStreamHandler::handleChart3d(Chart3dRecord *record)
{
    if(!record) return;
    std::cout << "ChartSubStreamHandler::handleChart3d" << std::endl;
    //TODO
}

void ChartSubStreamHandler::handleLineFormat(LineFormatRecord *)
{
    //TODO
}

void ChartSubStreamHandler::handleAreaFormat(AreaFormatRecord *)
{
    //TODO
}

void ChartSubStreamHandler::handlePieFormat(PieFormatRecord *record)
{
    if(!record) return;
    if(!m_currentSeries) return;
    std::cout << "ChartSubStreamHandler::handlePieFormat pcExplode="<<record->pcExplode()<<std::endl;
    m_currentSeries->datasetFormat << new ChartObject::PieFormat(record->pcExplode());
}

void ChartSubStreamHandler::handleMarkerFormat(MarkerFormatRecord *)
{
    //TODO
}

void ChartSubStreamHandler::handleChartFormat(ChartFormatRecord *)
{
    //TODO
}

void ChartSubStreamHandler::handleGelFrame(GelFrameRecord *)
{
    //TODO
}

// specifies the chartgroup for the current series
void ChartSubStreamHandler::handleSerToCrt(SerToCrtRecord *record)
{
    if(!record) return;
    std::cout << "ChartSubStreamHandler::handleSerToCrt id=" << record->identifier() << std::endl;
}

// properties
void ChartSubStreamHandler::handleShtProps(ShtPropsRecord *)
{
    //TODO
}

// text
void ChartSubStreamHandler::handleDefaultText(DefaultTextRecord *record)
{
    if(!record) return;
    std::cout << "ChartSubStreamHandler::handleDefaultText id=" << record->identifier() << std::endl;
    //TODO
}

// text formatting
void ChartSubStreamHandler::handleText(TextRecord *record)
{
    if(!record) return;
    std::cout << "ChartSubStreamHandler::handleText" << std::endl;
    //TODO
}

void ChartSubStreamHandler::handleSeriesText(SeriesTextRecord* record)
{
    if(!record) return;
    std::cout << "ChartSubStreamHandler::handleSeriesText text=" << record->text() << std::endl;
    //TODO
}

void ChartSubStreamHandler::handlePos(PosRecord *record)
{
    if(!record) return;
    std::cout << "ChartSubStreamHandler::handlePos mdTopLt=" << record->mdTopLt() << " mdBotRt=" << record->mdBotRt() << " x1=" << record->x1() << " y1=" << record->y1() << " x2=" << record->x2() << " y2=" << record->y2() << std::endl;
    //TODO
}

void ChartSubStreamHandler::handleFontX(FontXRecord *)
{
    //TODO
}

void ChartSubStreamHandler::handlePlotGrowth(PlotGrowthRecord *)
{
    //TODO
}

void ChartSubStreamHandler::handleLegend(LegendRecord *)
{
    //TODO
}

void ChartSubStreamHandler::handleAxesUsed(AxesUsedRecord *)
{
    //TODO
}

void ChartSubStreamHandler::handleAxisParent(AxisParentRecord *)
{
    //TODO
}

// specifies that the chartgroup is a pie chart
void ChartSubStreamHandler::handlePie(PieRecord *record)
{
    if(!record) return;
    std::cout << "ChartSubStreamHandler::handlePie anStart=" << record->anStart() << " pcDonut=" << record->pcDonut() << std::endl;
    m_chart->impl = new ChartObject::PieImpl(record->anStart(), record->pcDonut());
}

// type of data contained in the Number records following
void ChartSubStreamHandler::handleSIIndex(SIIndexRecord *record)
{
    if(!record) return;
    std::cout << "ChartSubStreamHandler::handleSIIndex numIndex=" << record->numIndex() << std::endl;
    /*TODO
    0x0001 Series values or vertical values (for scatter or bubble chart groups)
    0x0002 Category labels or horizontal values (for scatter or bubble chart groups)
    0x0003 Bubble sizes
    */
}

void ChartSubStreamHandler::handleMsoDrawing(MsoDrawingRecord* record)
{
    if(!record) return;
    std::cout << "ChartSubStreamHandler::handleMsoDrawing" << std::endl;
    //TODO
}

void ChartSubStreamHandler::handleShapePropsStream(ShapePropsStreamRecord* record)
{
    if(!record) return;
    std::cout << "ChartSubStreamHandler::handleShapePropsStream rgb=" << record->rgb().length() << " " << record->rgb() << std::endl;
    //TODO
}

void ChartSubStreamHandler::handleTextPropsStream(TextPropsStreamRecord* record)
{
    if(!record) return;
    std::cout << "ChartSubStreamHandler::handleTextPropsStream rgb=" << record->rgb().length() << " " << record->rgb() << std::endl;
    //TODO
}

void ChartSubStreamHandler::handleObjectLink(ObjectLinkRecord *record)
{
    if(!record) return;
    std::cout << "ChartSubStreamHandler::handleObjectLink wLinkObj=" << record->wLinkObj() << " wLinkVar1=" << record->wLinkVar1() << " wLinkVar2=" << record->wLinkVar2() << std::endl;
    //TODO
}
