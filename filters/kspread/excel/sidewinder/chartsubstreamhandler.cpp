/* Swinder - Portable library for spreadsheet
   Copyright (C) 2009-2010 Sebastian Sauer <sebsauer@kdab.com>
   Copyright (C) 2010 Carlos Licea <carlos@kdab.com>

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

#include <QRegExp>
#include <QDebug>

#include <XlsxUtils.h>

namespace Swinder {

class BRAIRecord : public Record
{
public:
    Charting::Value* m_value;

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

        Charting::Value::DataId dataId = (Charting::Value::DataId) readU8(data);
        Charting::Value::DataType type = (Charting::Value::DataType) readU8(data + 1);
        bool isUnlinkedFormat = readU16(data + 2) & 0x01;
        unsigned numberFormat = readU16(data + 4);

        QString formula;
        if (m_worksheetHandler) {
            FormulaTokens tokens = m_worksheetHandler->decodeFormula(size, 6, data, version());
            formula = m_worksheetHandler->decodeFormula(0, 0, true, tokens);
        } else {
            FormulaTokens tokens = m_handler->globals()->decodeFormula(size, 6, data, version());
            formula = m_handler->globals()->decodeFormula(0, 0, true, tokens);
        }

        if (m_value) delete m_value;
        m_value = new Charting::Value(dataId, type, formula, isUnlinkedFormat, numberFormat);
    }

private:
    ChartSubStreamHandler* m_handler;
    WorksheetSubStreamHandler* m_worksheetHandler;
};

const unsigned BRAIRecord::id = 0x1051;

} // namespace Swinder

using namespace Swinder;

ChartSubStreamHandler::ChartSubStreamHandler(GlobalsSubStreamHandler* globals,
                                             SubStreamHandler* parentHandler)
    : SubStreamHandler()
    , m_globals(globals)
    , m_parentHandler(parentHandler)
    , m_sheet(0)
    , m_chartObject(0)
    , m_chart(0)
    , m_currentSeries(0)
    , m_currentObj(0)
    , m_defaultTextId(-1)
    , m_axisId(-1)
{
    RecordRegistry::registerRecordClass(BRAIRecord::id, BRAIRecord::createRecord, this);

    WorksheetSubStreamHandler* worksheetHandler = dynamic_cast<WorksheetSubStreamHandler*>(parentHandler);
    if (worksheetHandler) {
        m_sheet = worksheetHandler->sheet();
        Q_ASSERT(m_sheet);

        std::vector<unsigned long>& charts = worksheetHandler->charts();
        Q_ASSERT(!charts.empty());
        const unsigned long id = charts.back();

        std::map<unsigned long, Object*>::iterator it = worksheetHandler->sharedObjects().find(id);
        Q_ASSERT(it != worksheetHandler->sharedObjects().end());
        //worksheetHandler->sharedObjects().erase(id); // remove from the sharedObjects and take over ownership
        m_chartObject = dynamic_cast<ChartObject*>(it->second);
        Q_ASSERT(m_chartObject);
        m_chart = m_chartObject->m_chart;
        Q_ASSERT(m_chart);
        m_currentObj = m_chart;

        Cell* cell = m_sheet->cell(m_chartObject->m_colL, m_chartObject->m_rwT, true);
        cell->addChart(m_chartObject);
    } else {
        Q_ASSERT(globals);
        if (globals->chartSheets().isEmpty()) {
            std::cerr << "ChartSubStreamHandler: Got a chart substream without having enough chart sheets..." << std::endl;
        } else {
            m_sheet = globals->chartSheets().takeFirst();
#if 0
            m_chartObject = new ChartObject(m_chartObject->id());
            m_chart = m_chartObject->m_chart;
            Q_ASSERT(m_chart);
            m_currentObj = m_chart;
#if 0
            DrawingObject* drawing = new DrawingObject;
            drawing->m_properties[DrawingObject::pid] = m_chartObject->id();
            drawing->m_properties[DrawingObject::itxid] = m_chartObject->id();
            drawing->m_colL = drawing->m_dxL = drawing->m_rwT = drawing->m_dyT = drawing->m_dxR = drawing->m_dyB = 0;
            drawing->m_colR = 10; drawing->m_rwB = 30; //FIXME use sheet "fullscreen" rather then hardcode
            m_chartObject->setDrawingObject(drawing);
#else
            m_chartObject->m_colL = m_chartObject->m_dxL = m_chartObject->m_rwT = m_chartObject->m_dyT = m_chartObject->m_dxR = m_chartObject->m_dyB = 0;
            m_chartObject->m_colR = 10; m_chartObject->m_rwB = 30; //FIXME use sheet "fullscreen" rather then hardcode
#endif
            Cell* cell = m_sheet->cell(0, 0, true); // anchor to the first cell
            cell->addChart(m_chartObject);
#else
            std::cerr << "ChartSubStreamHandler: FIXME" << std::endl;
#endif
        }
    }
}

ChartSubStreamHandler::~ChartSubStreamHandler()
{
    RecordRegistry::unregisterRecordClass(BRAIRecord::id);
}

std::string whitespaces(int number)
{
    std::string s;
    for (int i = 0; i < number; ++i)
        s += " ";
    return s;
}

#define DEBUG \
    std::cout << whitespaces(m_stack.count()) << "ChartSubStreamHandler::" << __FUNCTION__ << " "

void ChartSubStreamHandler::handleRecord(Record* record)
{
    if (!record) return;
    if (!m_chart) return;
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
    else if (type == BarRecord::id)
        handleBar(static_cast<BarRecord*>(record));
    else if (type == AreaRecord::id)
        handleArea(static_cast<AreaRecord*>(record));
    else if (type == LineRecord::id)
        handleLine(static_cast<LineRecord*>(record));
    else if (type == ScatterRecord::id)
        handleScatter(static_cast<ScatterRecord*>(record));
    else if (type == RadarRecord::id)
        handleRadar(static_cast<RadarRecord*>(record));
    else if (type == RadarAreaRecord::id)
        handleRadarArea(static_cast<RadarAreaRecord*>(record));
    else if (type == SurfRecord::id)
        handleSurf(static_cast<SurfRecord*>(record));
    else if (type == AxisRecord::id)
        handleAxis(static_cast<AxisRecord*>(record));
    else if (type == AxisLineRecord::id)
        handleAxisLine(static_cast<AxisLineRecord*>(record));
    else if (type == ValueRangeRecord::id)
        handleValueRange(static_cast<ValueRangeRecord*>(record));
    else if (type == TickRecord::id)
        handleTick(static_cast<TickRecord*>(record));
    else if (type == AxcExtRecord::id)
        handleAxcExt(static_cast<AxcExtRecord*>(record));
    else if (type == CrtLineRecord::id)
        handleCrtLine(static_cast<CrtLineRecord*>(record));
    else if (type == CatSerRangeRecord::id)
        handleCatSerRange(static_cast<CatSerRangeRecord*>(record));
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
    else if (type == PlotAreaRecord::id)
        handlePlotArea(static_cast<PlotAreaRecord*>(record));
    else if (type == CrtLinkRecord::id)
        {} // written but unused record
    else if (type == UnitsRecord::id)
        {} // written but must be ignored
    else if (type == StartBlockRecord::id || type == EndBlockRecord::id)
        {} // not evaluated atm
    else {
        DEBUG << "Unhandled chart record with type=" << type << " name=" << record->name() << std::endl;
        //record->dump(std::cout);
    }
}

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
    if (!record) return;
    m_chart->m_leftMargin = record->leftMargin();
}

void ChartSubStreamHandler::handleRightMargin(RightMarginRecord* record)
{
    if (!record) return;
    m_chart->m_rightMargin = record->rightMargin();
}

void ChartSubStreamHandler::handleTopMargin(TopMarginRecord* record)
{
    if (!record) return;
    m_chart->m_topMargin = record->topMargin();
}

void ChartSubStreamHandler::handleBottomMargin(BottomMarginRecord* record)
{
    if (!record) return;
    m_chart->m_bottomMargin = record->bottomMargin();
}

void ChartSubStreamHandler::handleDimension(DimensionRecord *record)
{
    if (!record) return;
    DEBUG << "firstRow=" << record->firstRow() << " lastRowPlus1=" << record->lastRowPlus1() << " firstColumn=" << record->firstColumn() << " lastColumnPlus1=" << record->lastColumnPlus1() << " lastRow=" << record->lastRow() << " lastColumn=" << record->lastColumn() << std::endl;
}

void ChartSubStreamHandler::handleChart(ChartRecord *record)
{
    if (!record) return;
    DEBUG << "x=" << record->x() << " y=" << record->y() << " width=" << record->width() << " height=" << record->height() <<  std::endl;
    m_chart->m_x1 = record->x();
    m_chart->m_y1 = record->y();
    m_chart->m_x2 = record->width() - m_chart->m_x1;
    m_chart->m_y2 = record->height() - m_chart->m_y1;
}

// secifies the begin of a collection of records
void ChartSubStreamHandler::handleBegin(BeginRecord *)
{
    m_stack.push(m_currentObj);
}

// sepcified the end of a collection of records
void ChartSubStreamHandler::handleEnd(EndRecord *)
{
    m_currentObj = m_stack.pop();
    if (Charting::Series* series = dynamic_cast<Charting::Series*>(m_currentObj))
        m_currentSeries = series;
}

void ChartSubStreamHandler::handleFrame(FrameRecord *record)
{
    if (!record) return;
    if (record->isAutoPosition()) {
        m_chart->m_x1 = -1;
        m_chart->m_y1 = -1;
    }
    if (record->isAutoSize()) {
        m_chart->m_x2 = -1;
        m_chart->m_y2 = -1;
    }
}

// properties of the data for series, trendlines or errorbars
void ChartSubStreamHandler::handleSeries(SeriesRecord *record)
{
    if (!record) return;
    DEBUG << "dataTypeX=" << record->dataTypeX() << " dataTypeY=" << record->dataTypeY() << " countXValues=" << record->countXValues() << " countYValues=" << record->countYValues() << " bubbleSizeDataType=" << record->bubbleSizeDataType() << " countBubbleSizeValues=" << record->countBubbleSizeValues() << std::endl;
    
    m_currentSeries = new Charting::Series;
    m_currentSeries->m_dataTypeX = record->dataTypeX();
    m_currentSeries->m_countXValues = record->countXValues();
    m_currentSeries->m_countYValues = record->countYValues();
    m_currentSeries->m_countBubbleSizeValues = record->countBubbleSizeValues();
    m_chart->m_series << m_currentSeries;
    m_currentObj = m_currentSeries;
}

// specifies a reference to data in a sheet that is used by a part of a series, legend entry, trendline or error bars.
void ChartSubStreamHandler::handleBRAI(BRAIRecord *record)
{
    if (!record) return;
    DEBUG << "dataId=" << record->m_value->m_dataId << " type=" << record->m_value->m_type << " isUnlinkedNumberFormat=" << record->m_value->m_isUnlinkedFormat << " numberFormat=" << record->m_value->m_numberFormat << " formula=" << record->m_value->m_formula.toUtf8().constData() << std::endl;

    if (m_currentSeries) {
        // FIXME: Is that correct or do we need to take the series
        //        somehow into account to provide one cellRangeAddress
        //        per series similar to valuesCellRangeAddress?
        //
        // FIXME: Handle VerticalValues and BubbleSizeValues
        if (!record->m_value->m_formula.isEmpty()) {
            if (record->m_value->m_type == Charting::Value::TextOrValue
                || record->m_value->m_type == Charting::Value::CellRange)
            {
                if (record->m_value->m_dataId == Charting::Value::HorizontalValues)
                    m_currentSeries->m_valuesCellRangeAddress = record->m_value->m_formula;
                else if (record->m_value->m_dataId == Charting::Value::VerticalValues)
                    m_chart->m_verticalCellRangeAddress = record->m_value->m_formula;
                
                // FIXME: We are ignoring the sheetname here but we
                //        probably should handle the case where a
                //        series is made from different sheets...
                QPair<QString, QRect> result = splitCellRange( record->m_value->m_formula );
                m_chart->addRange(result.second);
            }
        }

        // FIXME: Is it ok to only accept the first or should we merge them somehow?
        if (!m_currentSeries->m_datasetValue.contains(record->m_value->m_dataId)) {
            m_currentSeries->m_datasetValue[record->m_value->m_dataId] = record->m_value;
            record->m_value = 0; // take over ownership
        }
    }
}

void ChartSubStreamHandler::handleDataFormat(DataFormatRecord *record)
{
    if (!record) return;
    DEBUG << "xi=" << record->xi()
          << " yi=" << record->yi()
          << " iss=" << record->iss() << std::endl;
    //TODO
}

void ChartSubStreamHandler::handleChart3DBarShape(Chart3DBarShapeRecord * record)
{
    if (!record) return;
    DEBUG << "riser=" << record->riser() << " taper=" << record->taper() << std::endl;
    //TODO
}

// specifies that chart is rendered in 3d scene
void ChartSubStreamHandler::handleChart3d(Chart3dRecord *record)
{
    if (!record) return;
    DEBUG << "anRot=" << record->anRot() << " anElev=" << record->anElev() << " pcDist=" << record->pcDist() << " pcHeight=" << record->pcHeight() << " pcDepth=" << record->pcDepth() << std::endl;
    m_chart->m_is3d = true;
    //TODO
}

void ChartSubStreamHandler::handleLineFormat(LineFormatRecord *record)
{
    if (!record) return;
    DEBUG << "lns=" << record->lns() << " we=" << record->we() << " fAxisOn=" << record->isFAxisOn() << std::endl;
    if (Charting::Axis* axis = dynamic_cast<Charting::Axis*>(m_currentObj)) {
        Charting::LineFormat format(Charting::LineFormat::Style(record->lns()), Charting::LineFormat::Tickness(record->we()));
        switch(m_axisId) {
            case 0x0000: // The axis line itself
                axis->m_format = format;
                break;
            case 0x0001: // The major gridlines along the axis
                axis->m_majorGridlines = Charting::Axis::Gridline(format);
                break;
            case 0x0002: // The minor gridlines along the axis
                axis->m_minorGridlines = Charting::Axis::Gridline(format);
                break;
            case 0x0003: // The walls or floor of a 3-D chart
                //TODO
                break;
        }
        m_axisId = -1;
    }
}

void ChartSubStreamHandler::handleAreaFormat(AreaFormatRecord *record)
{
    if (!record || !m_currentObj || m_currentObj->m_areaFormat) return;
    QColor foreground(record->redForeground(), record->greenForeground(), record->blueForeground());
    QColor background(record->redBackground(), record->greenBackground(), record->blueBackground());
    DEBUG << "foreground=" << foreground.name().toUtf8().data()
          << " background=" << background.name().toUtf8().data()
          << " fillStyle=" << record->fls() << std::endl;
    m_currentObj->m_areaFormat = new Charting::AreaFormat(foreground, background,
                                                          record->fls() != 0x0000);
}

void ChartSubStreamHandler::handlePieFormat(PieFormatRecord *record)
{
    if (!record) return;
    if (!m_currentSeries) return;
    DEBUG << "pcExplode=" << record->pcExplode() << std::endl;
    m_currentSeries->m_datasetFormat << new Charting::PieFormat(record->pcExplode());
}

void ChartSubStreamHandler::handleMarkerFormat(MarkerFormatRecord *record)
{
    if (!record) return;
    DEBUG << std::endl;
    //TODO
}

void ChartSubStreamHandler::handleChartFormat(ChartFormatRecord *record)
{
    if (!record) return;
    DEBUG << "fVaried=" << record->isFVaried() << std::endl;
    //TODO
}

void ChartSubStreamHandler::handleGelFrame(GelFrameRecord *record)
{
    if (!record) return;
    DEBUG << std::endl;
    //TODO
}

// specifies the chartgroup for the current series
void ChartSubStreamHandler::handleSerToCrt(SerToCrtRecord *record)
{
    if (!record) return;
    DEBUG << "id=" << record->identifier() << std::endl;
}

// properties
void ChartSubStreamHandler::handleShtProps(ShtPropsRecord *record)
{
    if (!record) return;
    DEBUG << std::endl;
    //TODO
}

// Specifies the text elements that are formatted using the information specified by the Text record
// immediately following this record. The identifier is one of;
//   * 0x0000 Format all Text records in the chart group where fShowPercent is equal to 0 or
//     fShowValue is equal to 0.
//   * 0x0001 Format all Text records in the chart group where fShowPercent is equal to 1 or
//     fShowValue is equal to 1.
//   * 0x0002 Format all Text records in the chart where the value of fScalable of the associated
//     FontInfo structure is equal to 0.
//   * 0x0003 Format all Text records in the chart where the value of fScalable of the associated
//     FontInfo structure is equal to 1.
void ChartSubStreamHandler::handleDefaultText(DefaultTextRecord *record)
{
    if (!record) return;
    DEBUG << "id=" << record->identifier() << std::endl;
    m_defaultTextId = record->identifier();
}

// specifies the properties of an attached label
void ChartSubStreamHandler::handleText(TextRecord *record)
{
    if (!record || record->isFDeleted()) return;
    DEBUG << "at=" << record->at()
          << " vat=" << record->vat()
          << " x=" << record->x()
          << " y=" << record->y()
          << " dx=" << record->dx()
          << " dy=" << record->dy()
          << " fShowKey=" << record->isFShowKey()
          << " fShowValue=" << record->isFShowValue() << std::endl;

    m_currentObj = new Charting::Text;
    if (m_defaultTextId >= 0) {  
        //m_defaultObjects[m_currentObj] = m_defaultTextId;
        m_defaultTextId = -1;
    }
}

void ChartSubStreamHandler::handleSeriesText(SeriesTextRecord* record)
{
    if (!record || !m_currentSeries) return;
    DEBUG << "text=" << record->text() << std::endl;
    if (Charting::Text *t = dynamic_cast<Charting::Text*>(m_currentObj)) {
        t->m_text = record->text();
    } else if (Charting::Legend *l = dynamic_cast<Charting::Legend*>(m_currentObj)) {
        //TODO
        Q_UNUSED(l);
    } else if (Charting::Series* series = dynamic_cast<Charting::Series*>(m_currentObj)) {
        series->m_texts << new Charting::Text(record->text());
    } else {
        //m_currentSeries->m_texts << new Charting::Text(string(record->text()));
    }
}

void ChartSubStreamHandler::handlePos(PosRecord *record)
{
    if (!record) return;
    DEBUG << "mdTopLt=" << record->mdTopLt() << " mdBotRt=" << record->mdBotRt() << " x1=" << record->x1() << " y1=" << record->y1() << " x2=" << record->x2() << " y2=" << record->y2() << std::endl;

    if (m_currentObj) {
        m_currentObj->m_mdBotRt = record->mdBotRt();
        m_currentObj->m_mdTopLt = record->mdTopLt();
        m_currentObj->m_x1 = record->x1();
        m_currentObj->m_y1 = record->y1();
        m_currentObj->m_x2 = record->x2();
        m_currentObj->m_y2 = record->y2();
    }
}

void ChartSubStreamHandler::handleFontX(FontXRecord *record)
{
    if (!record) return;
    DEBUG << std::endl;
    //TODO
}

void ChartSubStreamHandler::handlePlotGrowth(PlotGrowthRecord *record)
{
    if (!record) return;
    DEBUG << std::endl;
    //TODO
}

void ChartSubStreamHandler::handleLegend(LegendRecord *record)
{
    if (!record) return;
    DEBUG << std::endl;
    m_currentObj = new Charting::Legend();
    //TODO
}

void ChartSubStreamHandler::handleAxesUsed(AxesUsedRecord *record)
{
    if (!record) return;
    DEBUG << std::endl;
    //TODO
}

void ChartSubStreamHandler::handleAxisParent(AxisParentRecord *record)
{
    if (!record) return;
    DEBUG << std::endl;
    //TODO
}

// specifies that the chartgroup is a pie chart
void ChartSubStreamHandler::handlePie(PieRecord *record)
{
    if (!record || m_chart->m_impl) return;
    DEBUG << "anStart=" << record->anStart() << " pcDonut=" << record->pcDonut() << std::endl;
    if (record->pcDonut() > 0)
        m_chart->m_impl = new Charting::RingImpl(record->anStart(), record->pcDonut());
    else
        m_chart->m_impl = new Charting::PieImpl(record->anStart());
}

// specifies that the chartgroup is a bar chart
void ChartSubStreamHandler::handleBar(BarRecord *record)
{
    if (!record || m_chart->m_impl) return;
    DEBUG << "pcOverlap=" << record->pcOverlap() << " pcGap=" << record->pcGap() << " fTranspose=" << record->isFTranspose() << " fStacked=" << record->isFStacked() << " f100=" << record->isF100() << std::endl;
    m_chart->m_impl = new Charting::BarImpl();
    m_chart->m_transpose = record->isFTranspose();
    m_chart->m_stacked = record->isFStacked();
    m_chart->m_f100 = record->isF100();
}

// specifies that the chartgroup is a area chart
void ChartSubStreamHandler::handleArea(AreaRecord* record)
{
    if (!record || m_chart->m_impl) return;
    DEBUG << std::endl;
    m_chart->m_impl = new Charting::AreaImpl();
    m_chart->m_stacked = record->isFStacked();
    m_chart->m_f100 = record->isF100();
}

// specifies that the chartgroup is a line chart
void ChartSubStreamHandler::handleLine(LineRecord* record)
{
    if (!record || m_chart->m_impl) return;
    DEBUG << std::endl;
    m_chart->m_impl = new Charting::LineImpl();
    m_chart->m_stacked = record->isFStacked();
    m_chart->m_f100 = record->isF100();
}

// specifies that the chartgroup is a scatter chart
void ChartSubStreamHandler::handleScatter(ScatterRecord* record)
{
    if (!record || m_chart->m_impl) return;
    DEBUG << std::endl;
    if (record->isFBubbles())
        m_chart->m_impl = new Charting::BubbleImpl(Charting::BubbleImpl::SizeType(record->wBubbleSize()), record->pcBubbleSizeRatio(), record->isFShowNegBubbles());
    else
        m_chart->m_impl = new Charting::ScatterImpl();
}

// specifies that the chartgroup is a radar chart
void ChartSubStreamHandler::handleRadar(RadarRecord *record)
{
    if (!record || m_chart->m_impl) return;
    DEBUG << std::endl;
    m_chart->m_impl = new Charting::RadarImpl();
}

// specifies that the chartgroup is a filled radar chart
void ChartSubStreamHandler::handleRadarArea(RadarAreaRecord *record)
{
    if (!record || m_chart->m_impl) return;
    DEBUG << std::endl;
    m_chart->m_impl = new Charting::RadarImpl();
}

// specifies that the chartgroup is a surface chart
void ChartSubStreamHandler::handleSurf(SurfRecord *record)
{
    if (!record || m_chart->m_impl) return;
    DEBUG << std::endl;
    m_chart->m_impl = new Charting::SurfaceImpl(record->isFFillSurface());
}

void ChartSubStreamHandler::handleAxis(AxisRecord* record)
{
    if (!record) return;
    DEBUG << "wType=" << record->wType() << std::endl;
    Charting::Axis* axis = new Charting::Axis(Charting::Axis::Type(record->wType()));
    m_chart->m_axes << axis;
    m_currentObj = axis;
}

// This record specifies which part of the axis is specified by the LineFormat record that follows.
void ChartSubStreamHandler::handleAxisLine(AxisLineRecord* record)
{
    if (!record) return;
    DEBUG << "identifier=" << record->identifier() << std::endl;
    m_axisId = record->identifier();
}

// type of data contained in the Number records following
void ChartSubStreamHandler::handleSIIndex(SIIndexRecord *record)
{
    if (!record) return;
    DEBUG << "numIndex=" << record->numIndex() << std::endl;
    /*TODO
    0x0001 Series values or vertical values (for scatter or bubble chart groups)
    0x0002 Category labels or horizontal values (for scatter or bubble chart groups)
    0x0003 Bubble sizes
    */
}

void ChartSubStreamHandler::handleMsoDrawing(MsoDrawingRecord* record)
{
    if (!record) return;
    DEBUG << std::endl;
    //TODO
}

void ChartSubStreamHandler::handleShapePropsStream(ShapePropsStreamRecord* record)
{
    if (!record) return;
    DEBUG << "rgb=" << record->rgb().length() << " " << record->rgb() << std::endl;
    //TODO
}

void ChartSubStreamHandler::handleTextPropsStream(TextPropsStreamRecord* record)
{
    if (!record) return;
    DEBUG << "rgb=" << record->rgb().length() << " " << record->rgb() << std::endl;
    //TODO
}

void ChartSubStreamHandler::handleObjectLink(ObjectLinkRecord *record)
{
    if (!record) return;
    DEBUG << "wLinkObj=" << record->wLinkObj() << " wLinkVar1=" << record->wLinkVar1() << " wLinkVar2=" << record->wLinkVar2() << std::endl;

    Charting::Text *t = dynamic_cast<Charting::Text*>(m_currentObj);
    if (!t) return;         // if the current object is not text, just do nothing
//    Q_ASSERT(t);          // if the current object is not text, terminate

    switch(record->wLinkObj()) {
        case ObjectLinkRecord::EntireChart: {
            m_chart->m_texts << t;
        } break;
        case ObjectLinkRecord::ValueOrVerticalAxis:
            //TODO
            break;
        case ObjectLinkRecord::CategoryOrHorizontalAxis:
            //TODO
            break;
        case ObjectLinkRecord::SeriesOrDatapoints: {
            if ((int)record->wLinkVar1() >= m_chart->m_series.count()) return;
            //Charting::Series* series = m_chart->m_series[ record->wLinkVar1() ];
            if (record->wLinkVar2() == 0xFFFF) {
                //TODO series->texts << t;
            } else {
                //TODO series->category[record->wLinkVar2()];
            }
        } break;
        case ObjectLinkRecord::SeriesAxis: break; //TODO
        case ObjectLinkRecord::DisplayUnitsLabelsOfAxis: break; //TODO
    }
}

// This empty record specifies that the Frame record that immediately
// follows this record specifies properties of the plot area.
void ChartSubStreamHandler::handlePlotArea(PlotAreaRecord *record)
{
    if (!record) return;
    DEBUG << std::endl;
}

void ChartSubStreamHandler::handleValueRange(ValueRangeRecord *record)
{
    if (!record) return;
    DEBUG << "fAutoMin=" << record->isFAutoMin() << " fAutoMax=" << record->isFAutoMax() << " fAutoMajor=" << record->isFAutoMajor() << " fAutoMinor=" << record->isFAutoMinor() << " fAutoCross=" << record->isFAutoCross() << " fLog=" << record->isFLog() << " fReversed=" << record->isFReversed() << " fMaxCross=" << record->isFMaxCross() << std::endl;
    //TODO
}

void ChartSubStreamHandler::handleTick(TickRecord *record)
{
    if (!record) return;
    DEBUG << "tktMajor=" << record->tktMajor()
          << " tktMinor=" << record->tktMinor()
          << " tlt=" << record->tlt() << std::endl;
    //TODO
}

void ChartSubStreamHandler::handleAxcExt(AxcExtRecord *record)
{
    if (!record) return;
    DEBUG << "fAutoMin=" << record->isFAutoMin()
          << " fAutoMax=" << record->isFAutoMax()
          << " fAutoMajor=" << record->isFAutoMajor()
          << " fAutoMinor=" << record->isFAutoMinor()
          << " fDateAxis=" << record->isFDateAxis()
          << " fAutoBase=" << record->isFAutoBase()
          << " fAutoCross=" << record->isFAutoCross()
          << " fAutoDate=" << record->isFAutoDate() << std::endl;
    //TODO
}

// Specifies the presence of drop lines, high-low lines, series lines or leader lines on the chart group. This record is followed by a LineFormat record which specifies the format of the lines.
void ChartSubStreamHandler::handleCrtLine(CrtLineRecord *record)
{
    if (!record) return;
    DEBUG << "identifier=" << record->identifier() << std::endl;
    switch(record->identifier()) {
        case 0x0000: // Drop lines below the data points of line, area, and stock chart groups.
            //TODO
            break;
        case 0x0001: // High-Low lines around the data points of line and stock chart groups.
            if (Charting::LineImpl* line = dynamic_cast<Charting::LineImpl*>(m_chart->m_impl)) {
                // It seems that a stockchart is always a linechart with a CrtLine record that defines High-Low lines.
                delete line;
                m_chart->m_impl = new Charting::StockImpl();
            }
            break;
        case 0x0002: // Series lines connecting data points of stacked column and bar chart groups, and the primary pie to the secondary bar/pie of bar of pie and pie of pie chart groups.
            //TODO
            break;
        case 0x0003: // Leader lines with non-default formatting connecting data labels to the data point of pie and pie of pie chart groups.
            //TODO
            break;
    }
}

void ChartSubStreamHandler::handleCatSerRange(CatSerRangeRecord *record)
{
    if (!record) return;
    DEBUG << "fBetween=" << record->isFBetween() << " fMaxCross=" << record->isFMaxCross() << " fReverse=" << record->isFReverse() << std::endl;
    //TODO
}
