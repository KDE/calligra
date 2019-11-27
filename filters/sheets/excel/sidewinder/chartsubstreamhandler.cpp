
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
#include "xmltk.h"

#include <QRegExp>

#include <XlsUtils.h>  // splitCellRange() et al.

#include <sstream>

namespace Swinder {

class BRAIRecord : public Record
{
public:
    KoChart::Value* m_value;

    static const unsigned int id;
    unsigned int rtti() const override { return this->id; }
    const char* name() const override { return "BRAI"; }
    static Record *createRecord(Workbook *book, void *arg) { return new BRAIRecord(book, arg); }

    BRAIRecord(Swinder::Workbook *book, void *arg) : Record(book), m_handler(static_cast<ChartSubStreamHandler*>(arg))
    {
        m_worksheetHandler = dynamic_cast<WorksheetSubStreamHandler*>(m_handler->parentHandler());
        m_value = 0;
    }
    ~BRAIRecord() override { delete m_value; }

    void dump(std::ostream&) const override { /*TODO*/ }

    void setData(unsigned size, const unsigned char* data, const unsigned int* /*continuePositions*/) override
    {
        if (size < 8) {
            setIsValid(false);
            return;
        }

        KoChart::Value::DataId dataId = (KoChart::Value::DataId) readU8(data);
        KoChart::Value::DataType type = (KoChart::Value::DataType) readU8(data + 1);
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

        delete m_value;
        m_value = new KoChart::Value(dataId, type, formula, isUnlinkedFormat, numberFormat);
    }

private:
    ChartSubStreamHandler* m_handler;
    WorksheetSubStreamHandler* m_worksheetHandler;
};

class CrtMlFrtRecord : public Record
{
public:
    static const unsigned int id;
    unsigned int rtti() const override { return this->id; }
    const char* name() const override { return "CrtMlFrt"; }
    static Record *createRecord(Workbook *book, void *arg) { return new CrtMlFrtRecord(book, arg); }
    CrtMlFrtRecord(Swinder::Workbook *book, void *arg) : Record(book), m_handler(static_cast<ChartSubStreamHandler*>(arg)), m_xmlTkParent(0) {
        m_worksheetHandler = dynamic_cast<WorksheetSubStreamHandler*>(m_handler->parentHandler());
    }
    ~CrtMlFrtRecord() override { qDeleteAll(m_tokens); }
    unsigned xmlTkParent() const { return m_xmlTkParent; }
    const QList<XmlTk*>& tokens() const { return m_tokens; }
    void dump(std::ostream& out) const override {
        QStringList tokens;
        foreach(XmlTk* t, m_tokens)
            tokens.append( QString("%1(%2)=%3").arg(QString::fromUtf8(xmlTkTagName(XmlTkTags(t->m_xmlTkTag)))).arg(t->type()).arg(t->value()) );
        out << qPrintable(QString("[%1]").arg(tokens.join(", ")));
    }
    void setData(unsigned size, const unsigned char* data, const unsigned int* /*continuePositions*/) override {
        if (size < 20) {
            setIsValid(false);
            return;
        }
        //unsigned rt = readU16(data);
        //unsigned grbitFrt = readU16(data + 2);
        //unsigned cb = readU32(data + 12);
        //unsigned recordVersion = readU8(data + 16);
        m_xmlTkParent = readU16(data + 18);

        qDeleteAll(m_tokens);
        m_tokens = parseXmlTkChain(data + 20, size - 20);
    }

private:
    ChartSubStreamHandler* m_handler;
    WorksheetSubStreamHandler* m_worksheetHandler;
    unsigned m_xmlTkParent;
    QList<XmlTk*> m_tokens;
};

const unsigned BRAIRecord::id = 0x1051;
const unsigned CrtMlFrtRecord::id = 0x89E;

} // namespace Swinder

using namespace Swinder;

/// This represents the internal chart data cache aka the "local-table" that
/// is embedded into the charts content.xml and not fetched from the application
/// embedding the chart (e.g. from a Calligra Sheets sheet).
class ChartSubStreamHandler::InternalDataCache
{
public:
    InternalDataCache(ChartSubStreamHandler *chartSubStreamHandler, unsigned index) : m_chartSubStreamHandler(chartSubStreamHandler), m_siIndex(index) {}
    ~InternalDataCache() {
        QString cellRegion = m_cellRegion.isNull() ? QString() : Swinder::encodeAddress("local", m_cellRegion);
        bool isBubble = dynamic_cast<KoChart::BubbleImpl*>(m_chartSubStreamHandler->m_chart->m_impl);
        bool isScatter = dynamic_cast<KoChart::ScatterImpl*>(m_chartSubStreamHandler->m_chart->m_impl);
        foreach(KoChart::Series *series, m_chartSubStreamHandler->m_chart->m_series) {
            switch (m_siIndex) {
                case 0x0001: { // Series values or vertical values (for scatter or bubble chart groups)
                    if (isBubble || isScatter) {
                        bool change = !series->m_datasetValue.contains(KoChart::Value::VerticalValues) || (series->m_datasetValue[KoChart::Value::VerticalValues]->m_type == KoChart::Value::TextOrValue && series->m_datasetValue[KoChart::Value::VerticalValues]->m_formula.isEmpty());
                        if (change) {
                            if (isBubble) {
                                QString y = series->m_domainValuesCellRangeAddress.isEmpty() ? QString() : series->m_domainValuesCellRangeAddress[0];
                                series->m_domainValuesCellRangeAddress = QStringList() << y << cellRegion;
                            } else if (isScatter) {
                                series->m_domainValuesCellRangeAddress = QStringList() << cellRegion;
                            }
                            //m_chartSubStreamHandler->m_chart->m_verticalCellRangeAddress = cellRegion;
                        }
                    } else {
                        if (series->m_valuesCellRangeAddress.isEmpty())
                            series->m_valuesCellRangeAddress = cellRegion;
                    }
                } break;
                case 0x0002: { // Category labels or horizontal values (for scatter or bubble chart groups)
                    if (isBubble || isScatter) {
                        bool change = !series->m_datasetValue.contains(KoChart::Value::HorizontalValues) || (series->m_datasetValue[KoChart::Value::HorizontalValues]->m_type == KoChart::Value::TextOrValue && series->m_datasetValue[KoChart::Value::HorizontalValues]->m_formula.isEmpty());
                        if (change) {
                            if (isBubble) {
                                QString x = series->m_domainValuesCellRangeAddress.count() < 2 ? QString() : series->m_domainValuesCellRangeAddress[1];
                                series->m_domainValuesCellRangeAddress = QStringList() << cellRegion << x;
                            }
                            //series->m_valuesCellRangeAddress = cellRegion;
                        }
                    } else {
                        if (m_chartSubStreamHandler->m_chart->m_verticalCellRangeAddress.isEmpty())
                            m_chartSubStreamHandler->m_chart->m_verticalCellRangeAddress = cellRegion;
                    }
                } break;
                case 0x0003: { // Bubble sizes
                    if (isBubble) {
                        if (series->m_valuesCellRangeAddress.isEmpty())
                            series->m_valuesCellRangeAddress = cellRegion;
                    }
                } break;
                default:
                    break;
            }
        }
    }
    void add(unsigned column, unsigned row) {
        QRect r(column, row, 1, 1);
        if (m_cellRegion.isNull()) {
            m_cellRegion = r;
        } else {
            m_cellRegion |= r;
        }
    }
private:
    ChartSubStreamHandler *m_chartSubStreamHandler;
    unsigned m_siIndex;
    QRect m_cellRegion;
};

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
    , m_internalDataCache(0)
    , m_defaultTextId(-1)
    , m_axisId(-1)
    , m_disableAutoMarker( false )
{
    RecordRegistry::registerRecordClass(BRAIRecord::id, BRAIRecord::createRecord, this);
    RecordRegistry::registerRecordClass(CrtMlFrtRecord::id, CrtMlFrtRecord::createRecord, this);

    WorksheetSubStreamHandler* worksheetHandler = dynamic_cast<WorksheetSubStreamHandler*>(parentHandler);
    if (worksheetHandler) {
        m_sheet = worksheetHandler->sheet();
        Q_ASSERT(m_sheet);

        std::vector<unsigned long>& charts = worksheetHandler->charts();
        if (charts.empty()) {
            qCWarning(lcSidewinder) << "Got a chart substream without having charts in the worksheet";
            return;
        }
        const unsigned long id = charts.back();

        std::map<unsigned long, Object*>::iterator it = worksheetHandler->sharedObjects().find(id);
        if (it == worksheetHandler->sharedObjects().end()) {
            qCWarning(lcSidewinder) << "Got a chart substream without having a chart in the worksheet";
            return;
        }        
        m_chartObject = dynamic_cast<ChartObject*>(it->second);
        worksheetHandler->sharedObjects().erase(id); // remove from the sharedObjects and take over ownership
        Q_ASSERT(m_chartObject);
        m_chart = m_chartObject->m_chart;
        Q_ASSERT(m_chart);
        m_currentObj = m_chart;

        Cell* cell = m_sheet->cell(m_chartObject->m_colL, m_chartObject->m_rwT, true);
        cell->addChart(m_chartObject);
    } else {
        Q_ASSERT(globals);
        if (globals->chartSheets().isEmpty()) {
            qCWarning(lcSidewinder) << "ChartSubStreamHandler: Got a chart substream without having enough chart sheets...";
        } else {
#if 0
            m_sheet = globals->chartSheets().takeFirst();
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
            qCWarning(lcSidewinder) << "ChartSubStreamHandler: FIXME";
#endif
        }
    }
}

ChartSubStreamHandler::~ChartSubStreamHandler()
{
    // Set the chart's title once everything is done.
    if (m_chart && m_chart->m_title.isEmpty()) {
        if (!m_chart->m_texts.isEmpty()) {
            // If defined direct within the chart using a ObjectLinkRecord then we use that as title.
            m_chart->m_title = m_chart->m_texts.first()->m_text;
        }
        if (m_chart->m_title.isEmpty() && m_chart->m_series.count() == 1) {
            // Else we are using the same logic that is used in the 2007 filter and fetch the title
            // from the series collection of TextRecord's.
            KoChart::Series* series = m_chart->m_series.first();
            if (!series->m_texts.isEmpty() )
                m_chart->m_title = series->m_texts.first()->m_text;
        }
    }

    delete m_internalDataCache;
    RecordRegistry::unregisterRecordClass(BRAIRecord::id);
    RecordRegistry::unregisterRecordClass(CrtMlFrtRecord::id);
}

#define DEBUG \
    qCDebug(lcSidewinder) << QString(m_stack.count(), QChar(' ')) << "ChartSubStreamHandler::" << __FUNCTION__

void ChartSubStreamHandler::handleRecord(Record* record)
{
    if (!record) return;
    if (!m_chart) return;
    const unsigned type = record->rtti();

    if (m_internalDataCache && type != NumberRecord::id) {
        delete m_internalDataCache;
        m_internalDataCache = 0;
    }

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
    else if (type == SeriesListRecord::id)
        handleSeriesList(static_cast<SeriesListRecord*>(record));
    else if (type == NumberRecord::id)
        handleNumber(static_cast<NumberRecord*>(record));
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
    else if (type == CatLabRecord::id)
        handleCatLab(static_cast<CatLabRecord*>(record));
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
    else if (type == AttachedLabelRecord::id)
        handleAttachedLabel(static_cast<AttachedLabelRecord*>(record));
    else if (type == DataLabelExtContentsRecord::id)
        handleDataLabelExtContents(static_cast<DataLabelExtContentsRecord*>(record));
    else if (type == XFRecord::id)
        handleXF(static_cast<XFRecord*>(record));
    else if (type == LabelRecord::id)
        handleLabel(static_cast<LabelRecord*>(record));
    else if (type == IFmtRecord::id)
        handleIFmt(static_cast<IFmtRecord*>(record));
    else if (type == CrtMlFrtRecord::id)
        handleCrtMlFrt(static_cast<CrtMlFrtRecord*>(record));
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
        DEBUG << "Unhandled chart record with type=" << type << "name=" << record->name();
        //record->dump(std::cout);
    }
}

void ChartSubStreamHandler::handleBOF(BOFRecord*)
{
}

void ChartSubStreamHandler::handleEOF(EOFRecord *)
{
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
    DEBUG << "firstRow=" << record->firstRow() << "lastRowPlus1=" << record->lastRowPlus1()
          << "firstColumn=" << record->firstColumn() << "lastColumnPlus1=" << record->lastColumnPlus1()
          << "lastRow=" << record->lastRow() << "lastColumn=" << record->lastColumn();
}

void ChartSubStreamHandler::handleChart(ChartRecord *record)
{
    if (!record) return;
    DEBUG << "x=" << record->x() << "y=" << record->y() << "width=" << record->width() << "height=" << record->height();
    m_chart->m_x1 = record->x();
    m_chart->m_y1 = record->y();
    m_chart->m_x2 = record->width() - m_chart->m_x1;
    m_chart->m_y2 = record->height() - m_chart->m_y1;
}

// specifies the begin of a collection of records
void ChartSubStreamHandler::handleBegin(BeginRecord *)
{
    m_stack.push(m_currentObj);
}

// specified the end of a collection of records
void ChartSubStreamHandler::handleEnd(EndRecord *)
{
    m_currentObj = m_stack.pop();
    if (!m_seriesStack.isEmpty())
        m_currentSeries = m_seriesStack.pop();
    else if (KoChart::Series* series = dynamic_cast<KoChart::Series*>(m_currentObj))
        m_currentSeries = series;
}

void ChartSubStreamHandler::handleFrame(FrameRecord *record)
{
    if (!record) return;
    DEBUG << "autoPosition=" << record->isAutoPosition() << "autoSize=" << record->isAutoSize();
    if ( dynamic_cast< KoChart::Chart* > ( m_currentObj ) ) {
        if (record->isAutoPosition()) {
            m_chart->m_x1 = -1;
            m_chart->m_y1 = -1;
        }
        if (record->isAutoSize()) {
            m_chart->m_x2 = -1;
            m_chart->m_y2 = -1;
        }
    }
    else if ( dynamic_cast< KoChart::PlotArea* > ( m_currentObj ) ) {
    }
}

// properties of the data for series, trendlines or errorbars
void ChartSubStreamHandler::handleSeries(SeriesRecord *record)
{
    if (!record) return;
    DEBUG << "dataTypeX=" << record->dataTypeX() << "dataTypeY=" << record->dataTypeY() << "countXValues=" << record->countXValues()
          << "countYValues=" << record->countYValues() << "bubbleSizeDataType=" << record->bubbleSizeDataType()
          << "countBubbleSizeValues=" << record->countBubbleSizeValues();
    
    m_currentSeries = new KoChart::Series;
    m_currentSeries->m_dataTypeX = record->dataTypeX();
    m_currentSeries->m_countXValues = record->countXValues();
    m_currentSeries->m_countYValues = record->countYValues();
    m_currentSeries->m_countBubbleSizeValues = record->countBubbleSizeValues();

    m_chart->m_series << m_currentSeries;
    m_currentObj = m_currentSeries;
}

void ChartSubStreamHandler::handleSeriesList(SeriesListRecord *record)
{
    DEBUG << "cser=" << record->cser();
    for(unsigned i = 0; i < record->cser(); ++i)
        DEBUG << "number=" << i << "rgiser=" << record->rgiser(i);
    //TODO
}

void ChartSubStreamHandler::handleNumber(NumberRecord *record)
{
    DEBUG << "row=" << record->row() << "column=" << record->column() << "xfIndex=" << record->xfIndex() << "number=" << record->number();

    // The formatting of the value doesn't really matter or does it? Well, maybe for data-value-label's that should be displayed as formatted?
    //m_xfTable[record->xfIndex()]

    KoChart::Cell *cell = m_chart->m_internalTable.cell(record->column() + 1, record->row() + 1, true);
    cell->m_value = QString::number(record->number(), 'f');
    cell->m_valueType = "float";

    if (m_internalDataCache)
        m_internalDataCache->add(record->column(), record->row());
}

// specifies a reference to data in a sheet that is used by a part of a series, legend entry, trendline or error bars.
void ChartSubStreamHandler::handleBRAI(BRAIRecord *record)
{
    if (!record) return;
    DEBUG << "dataId=" << record->m_value->m_dataId << "type=" << record->m_value->m_type
          << "isUnlinkedNumberFormat=" << record->m_value->m_isUnlinkedFormat
          << "numberFormat=" << record->m_value->m_numberFormat << "formula=" << record->m_value->m_formula.toUtf8();

    if (m_currentSeries) {
        // FIXME: Is that correct or do we need to take the series
        //        somehow into account to provide one cellRangeAddress
        //        per series similar to valuesCellRangeAddress?
        //
        // FIXME: Handle VerticalValues and BubbleSizeValues
        if (!record->m_value->m_formula.isEmpty()) {
            if (record->m_value->m_type == KoChart::Value::TextOrValue
                || record->m_value->m_type == KoChart::Value::CellRange)
            {
                if (record->m_value->m_dataId == KoChart::Value::HorizontalValues) {
                    m_currentSeries->m_valuesCellRangeAddress = record->m_value->m_formula;
                } else if (record->m_value->m_dataId == KoChart::Value::VerticalValues) {
                    m_chart->m_verticalCellRangeAddress = record->m_value->m_formula;
                }
                
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

// This record specifies the data point or series that the formatting information that follows applies to.
void ChartSubStreamHandler::handleDataFormat(DataFormatRecord *record)
{
    if (!record) return;
    DEBUG << "xi=" << record->xi() << "yi=" << record->yi() << "iss=" << record->iss();
    if (record->yi() >= uint(m_chart->m_series.count())) {
        DEBUG << "Invalid series index=" << record->yi();
        m_currentObj = 0; // be sure to ignore all defined sub-elements
        return;
    }
    m_seriesStack.push(m_currentSeries);
    m_currentSeries = m_chart->m_series[record->yi()];
    if ( record->xi() == 0xFFFF ) { // applies to series
        m_currentObj = m_currentSeries;
    } else { // applies to data-point
        KoChart::DataPoint *dataPoint = 0;
        if (record->xi() > uint(m_currentSeries->m_dataPoints.count())) {
            DEBUG << "Invalid data-point index=" << record->yi();
        } else if (record->xi() == uint(m_currentSeries->m_dataPoints.count())) {
            dataPoint = new KoChart::DataPoint();
            m_currentSeries->m_dataPoints << dataPoint;
        } else {
            dataPoint = m_currentSeries->m_dataPoints[record->xi()];
        }
        m_currentObj = dataPoint;
    }
}

void ChartSubStreamHandler::handleChart3DBarShape(Chart3DBarShapeRecord * record)
{
    if (!record) return;
    DEBUG << "riser=" << record->riser() << "taper=" << record->taper();
    //TODO
}

// specifies that chart is rendered in 3d scene
void ChartSubStreamHandler::handleChart3d(Chart3dRecord *record)
{
    if (!record) return;
    DEBUG << "anRot=" << record->anRot() << "anElev=" << record->anElev() << "pcDist=" << record->pcDist()
          << "pcHeight=" << record->pcHeight() << "pcDepth=" << record->pcDepth();
    m_chart->m_is3d = true;
    //TODO
}

void ChartSubStreamHandler::handleLineFormat(LineFormatRecord *record)
{    
    if (!record) return;
    DEBUG << "lns=" << record->lns() << "we=" << record->we() << "fAxisOn=" << record->isFAxisOn();
    if (KoChart::Axis* axis = dynamic_cast<KoChart::Axis*>(m_currentObj)) {
        KoChart::LineFormat format(KoChart::LineFormat::Style(record->lns()), KoChart::LineFormat::Thickness(record->we()));
        switch(m_axisId) {
            case 0x0000: // The axis line itself
                axis->m_format = format;
                break;
            case 0x0001: // The major gridlines along the axis
                axis->m_majorGridlines = KoChart::Axis::Gridline(format);
                break;
            case 0x0002: // The minor gridlines along the axis
                axis->m_minorGridlines = KoChart::Axis::Gridline(format);
                break;
            case 0x0003: // The walls or floor of a 3-D chart
                //TODO
                break;
        }
        m_axisId = -1;
    } else if ( dynamic_cast< KoChart::Legend* > ( m_currentObj ) ) {
        if ( record->lns() == 0x0005 )
            m_chart->m_showLines = false;
        else if ( record->lns() == 0x0000 )
            m_chart->m_showLines = true;
//     } else if ( dynamic_cast< KoChart::Text* > ( m_currentObj ) ) {
//         return;
    } else if ( KoChart::Series* series = dynamic_cast< KoChart::Series* > ( m_currentObj/*m_currentSeries*/ ) ) {
        //Q_ASSERT( false );
        if ( !series->spPr )
            series->spPr = new KoChart::ShapeProperties;
        m_chart->m_showLines = false;
        const int index = m_chart->m_series.indexOf( series );
        const QColor color = record->isFAuto() ? globals()->workbook()->colorTable().at( 24 + index ) : QColor( record->red(), record->green(), record->blue() );
        series->spPr->lineFill.setColor( color );
        switch ( record->lns() )
        {
            case( 0x0000 ):
                series->spPr->lineFill.setType( KoChart::Fill::Solid );
                break;
            case( 0x0005 ):
            {
                series->spPr->lineFill.setType( KoChart::Fill::None );
//                 KoChart::ScatterImpl* impl = dynamic_cast< KoChart::ScatterImpl* >( m_chart->m_impl );
//                 if ( impl )
//                 {
//                     if ( impl->style == KoChart::ScatterImpl::Marker || impl->style == KoChart::ScatterImpl::LineMarker )
//                         impl->style = KoChart::ScatterImpl::Marker;
//                     else
//                         impl->style = KoChart::ScatterImpl::None;
//                 }
            }
                break;
            default:
                series->spPr->lineFill.setType( KoChart::Fill::None );
        }
        //series->spPr->lineFill.type = KoChart::Fill::Solid;
    }
    else if ( dynamic_cast< KoChart::ChartImpl* > ( m_currentObj ) ) {
        Q_ASSERT( false );
    }
    else if ( dynamic_cast< KoChart::Chart* > ( m_currentObj ) ) {
        DEBUG << "color=" << QColor( record->red(), record->green(), record->blue() ).name() << "automatic=" << record->isFAuto();
        //m_chart->m_showLines = record->isFAuto();
        Q_ASSERT( !dynamic_cast< KoChart::Series* > ( m_currentSeries ) );
    }
    else if ( KoChart::DataPoint *dataPoint = dynamic_cast< KoChart::DataPoint* > ( m_currentObj ) ) {
        Q_UNUSED( dataPoint );
    }
}

// This record specifies the patterns and colors used in a filled region of a chart. If this record is not
// present in the sequence of records that conforms to the SS rule of the Chart Sheet Substream
// ABNF, the patterns and colors used are specified by the default values of the fields of this record.
void ChartSubStreamHandler::handleAreaFormat(AreaFormatRecord *record)
{
    if (!record || !m_currentObj || m_currentObj->m_areaFormat) return;

    bool fill = record->fls() != 0x0000;
    QColor foreground, background;
    if ( record->isFAuto() ) {
        int index = 0;
        if ( KoChart::Series* series = dynamic_cast< KoChart::Series* > ( m_currentObj ) ) {
            index = m_chart->m_series.indexOf( series ) % 8;
            Q_ASSERT(index >= 0);
            foreground = globals()->workbook()->colorTable().at( 16 + index );
        } else if ( KoChart::DataPoint *dataPoint = dynamic_cast< KoChart::DataPoint* > ( m_currentObj ) ) {
            index = m_currentSeries->m_dataPoints.indexOf( dataPoint ) % 8;
            Q_ASSERT(index >= 0);
            foreground = globals()->workbook()->colorTable().at( 16 + index );
        } else {
            // The specs say that the default background-color is white but it is not clear
            // what automatic means for the case of the PlotArea. So, let's just not use any
            // color in that case what means the chart's color will be used (PlotArea is
            // transparent). That is probably not correct and we would need to just use
            // white as color but since so far I did not found any test-doc that indicates
            // that we are just going with transparent for now.
            fill = false;
            //foreground = background = QColor("#FFFFFF");
        }
        //background = QColor("#FFFFFF");
    } else {
        foreground = QColor(record->redForeground(), record->greenForeground(), record->blueForeground());
        background = QColor(record->redBackground(), record->greenBackground(), record->blueBackground());
    }

    DEBUG << "foreground=" << foreground.name() << "background=" << background.name()
          << "fillStyle=" << record->fls() << "fAuto=" << record->isFAuto();

    m_currentObj->m_areaFormat = new KoChart::AreaFormat(foreground, background, fill);

    if ( KoChart::Series* series = dynamic_cast< KoChart::Series* > ( m_currentObj ) ) {
        if ( !series->spPr )
            series->spPr = new KoChart::ShapeProperties;
        series->spPr->areaFill.setColor( foreground );
    }
    //else if ( KoChart::PlotArea* plotArea = dynamic_cast< KoChart::PlotArea* > ( m_currentObj ) ) {
    //}
    //else if ( KoChart::DataPoint *dataPoint = dynamic_cast< KoChart::DataPoint* > ( m_currentObj ) ) {
    //}
}

void ChartSubStreamHandler::handlePieFormat(PieFormatRecord *record)
{
    if (!record) return;
    if (!m_currentSeries) return;
    DEBUG << "pcExplode=" << record->pcExplode();
    m_currentSeries->m_datasetFormat << new KoChart::PieFormat(record->pcExplode());
}

void ChartSubStreamHandler::handleMarkerFormat(MarkerFormatRecord *record)
{
    if (!record) return;
    DEBUG << "fAuto=" << record->fAuto() << "imk=" << record->imk();
    const bool legend = dynamic_cast< KoChart::Legend* >( m_currentObj );
    if ( m_disableAutoMarker && legend )
        return;
    m_chart->m_markerType = KoChart::NoMarker;

    if ( KoChart::DataPoint *dataPoint = dynamic_cast<KoChart::DataPoint*>(m_currentObj) ) {
        Q_UNUSED(dataPoint);
    }
    else if ( KoChart::Series *series = dynamic_cast<KoChart::Series*>(m_currentObj) ) {
        if ( !series->spPr )
            series->spPr = new KoChart::ShapeProperties;
        const int index = m_chart->m_series.indexOf( series ) % 8;
        if ( record->fAuto() ) {
            if ( !m_disableAutoMarker )
                m_chart->m_markerType = KoChart::AutoMarker;
            if ( !series->spPr->areaFill.valid )
                series->spPr->areaFill.setColor( globals()->workbook()->colorTable().at( 24 + index ) );
            switch ( index ) {
                case( 0x0000 ):
                    series->m_markerType = KoChart::SquareMarker;
                    break;
                case( 0x0001 ):
                    series->m_markerType = KoChart::DiamondMarker;
                    break;
                case( 0x0002 ):
                    series->m_markerType = KoChart::SymbolXMarker;
                    break;
                case( 0x0003 ):
                    series->m_markerType = KoChart::SquareMarker;
                    break;
                case( 0x0004 ):
                    series->m_markerType = KoChart::DashMarker;
                    break;
                case( 0x0005 ):
                    series->m_markerType = KoChart::DashMarker;
                    break;
                case( 0x0006 ):
                    series->m_markerType = KoChart::CircleMarker;
                    break;
                case( 0x0007 ):
                    series->m_markerType = KoChart::PlusMarker;
                    break;
                default:
                    series->m_markerType = KoChart::SquareMarker;
                    break;
            }
        } else {
            if ( series ) {
                switch ( record->imk() ) {
                    case( 0x0000 ):
                        series->m_markerType = KoChart::NoMarker;
                        m_disableAutoMarker = true;
                        break;
                    case( 0x0001 ):
                        series->m_markerType = KoChart::SquareMarker;
                        break;
                    case( 0x0002 ):
                        series->m_markerType = KoChart::DiamondMarker;
                        break;
                    case( 0x0003 ):
                        series->m_markerType = KoChart::SymbolXMarker;
                        break;
                    case( 0x0004 ):
                        series->m_markerType = KoChart::SquareMarker;
                        break;
                    case( 0x0005 ):
                        series->m_markerType = KoChart::DashMarker;
                        break;
                    case( 0x0006 ):
                        series->m_markerType = KoChart::DashMarker;
                        break;
                    case( 0x0007 ):
                        series->m_markerType = KoChart::CircleMarker;
                        break;
                    case( 0x0008 ):
                        series->m_markerType = KoChart::PlusMarker;
                        break;
                    default:
                        series->m_markerType = KoChart::SquareMarker;
                        break;
                }
                if ( !series->spPr->areaFill.valid )
                    series->spPr->areaFill.setColor( QColor( record->redBackground(), record->greenBackground(), record->blueBackground() ) );
            }
        }
    }
}

void ChartSubStreamHandler::handleChartFormat(ChartFormatRecord *record)
{
    if (!record) return;
    DEBUG << "fVaried=" << record->isFVaried();
    //TODO
}

void ChartSubStreamHandler::handleGelFrame(GelFrameRecord *record)
{
    if (!record) return;
    DEBUG << "";
    //TODO
}

// specifies the chartgroup for the current series
void ChartSubStreamHandler::handleSerToCrt(SerToCrtRecord *record)
{
    if (!record) return;
    DEBUG << "id=" << record->identifier();
}

// properties
void ChartSubStreamHandler::handleShtProps(ShtPropsRecord *record)
{
    if (!record) return;
    DEBUG << "fManSerAlloc=" << record->isFManSerAlloc() << "fPlotVisOnly=" << record->isFPlotVisOnly()
          << "fNotSizeWIth=" << record->isFNotSizeWIth() << "fManPlotArea=" << record->isFManPlotArea()
          << "fAlwaysAutoPlotArea=" << record->isFAlwaysAutoPlotArea() << "mdBlank=" << record->mdBlank();
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
    DEBUG << "id=" << record->identifier();
    m_defaultTextId = record->identifier();
}

// specifies the properties of an attached label
void ChartSubStreamHandler::handleText(TextRecord *record)
{
    if (!record || record->isFDeleted()) return;
    DEBUG << "at=" << record->at()
          << "vat=" << record->vat()
          << "x=" << record->x()
          << "y=" << record->y()
          << "dx=" << record->dx()
          << "dy=" << record->dy()
          << "fShowKey=" << record->isFShowKey()
          << "fShowValue=" << record->isFShowValue();

    m_currentObj = new KoChart::Text;
    if (m_defaultTextId >= 0) {  
        //m_defaultObjects[m_currentObj] = m_defaultTextId;
        m_defaultTextId = -1;
    }
}

void ChartSubStreamHandler::handleSeriesText(SeriesTextRecord* record)
{
    if (!record || !m_currentSeries) return;
    DEBUG << "text=" << record->text();
    if (KoChart::Text *t = dynamic_cast<KoChart::Text*>(m_currentObj)) {
        t->m_text = record->text();
    } else if (KoChart::Legend *l = dynamic_cast<KoChart::Legend*>(m_currentObj)) {
        //TODO
        Q_UNUSED(l);
    } else if (KoChart::Series* series = dynamic_cast<KoChart::Series*>(m_currentObj)) {
        series->m_texts << new KoChart::Text(record->text());
    } else {
        //m_currentSeries->m_texts << new KoChart::Text(string(record->text()));
    }
}

void ChartSubStreamHandler::handlePos(PosRecord *record)
{
    if (!record) return;
    DEBUG << "mdTopLt=" << record->mdTopLt() << "mdBotRt=" << record->mdBotRt()
          << "x1=" << record->x1() << "y1=" << record->y1() << "x2=" << record->x2() << "y2=" << record->y2();

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
    DEBUG << "";
    //TODO
}

void ChartSubStreamHandler::handlePlotGrowth(PlotGrowthRecord *record)
{
    if (!record) return;
    DEBUG << "";
    //TODO
}

void ChartSubStreamHandler::handleLegend(LegendRecord *record)
{
    if (!record) return;
    DEBUG << "fAutoPosition=" << record->isFAutoPosition() << "fAutoPosX=" << record->isFAutoPosX()
          << "fAutoPosY=" << record->isFAutoPosY() << "fVert=" << record->isFVert()
          << "fWasDataTable=" << record->isFWasDataTable();
    m_currentObj = m_chart->m_legend = new KoChart::Legend();
}

// specifies the number of axis groups on the chart.
// cAxes specifies the number of axis groups on the chart.
//   0x0001 A single primary axis group is present
//   0x0002 Both a primary axis group and a secondary axis group are present
void ChartSubStreamHandler::handleAxesUsed(AxesUsedRecord *record)
{
    if (!record) return;
    DEBUG << "cAxes=" << record->cAxes();
    //TODO
}

// specifies properties of an axis group.
// iax specifies whether the axis group is primary or secondary.
//   0x0000 Axis group is primary.
//   0x0001 Axis group is secondary.
void ChartSubStreamHandler::handleAxisParent(AxisParentRecord *record)
{
    if (!record) return;
    DEBUG << "iax=" << record->iax();
    //TODO
}

// specifies that the chartgroup is a pie chart
void ChartSubStreamHandler::handlePie(PieRecord *record)
{
    if (!record || m_chart->m_impl) return;
    DEBUG << "anStart=" << record->anStart() << "pcDonut=" << record->pcDonut();
    if (record->pcDonut() > 0)
        m_chart->m_impl = new KoChart::RingImpl(record->anStart(), record->pcDonut());
    else
        m_chart->m_impl = new KoChart::PieImpl(record->anStart());
}

// specifies that the chartgroup is a bar chart
void ChartSubStreamHandler::handleBar(BarRecord *record)
{
    if (!record || m_chart->m_impl) return;
    DEBUG << "pcOverlap=" << record->pcOverlap() << "pcGap=" << record->pcGap() << "fTranspose=" << record->isFTranspose()
          << "fStacked=" << record->isFStacked() << "f100=" << record->isF100();
    m_chart->m_impl = new KoChart::BarImpl();
    m_chart->m_transpose = record->isFTranspose();
    m_chart->m_stacked = record->isFStacked();
    m_chart->m_f100 = record->isF100();
}

// specifies that the chartgroup is a area chart
void ChartSubStreamHandler::handleArea(AreaRecord* record)
{
    if (!record || m_chart->m_impl) return;
    DEBUG << "";
    m_chart->m_impl = new KoChart::AreaImpl();
    m_chart->m_stacked = record->isFStacked();
    m_chart->m_f100 = record->isF100();
}

// specifies that the chartgroup is a line chart
void ChartSubStreamHandler::handleLine(LineRecord* record)
{
    if (!record || m_chart->m_impl) return;
    DEBUG << "";
    m_chart->m_impl = new KoChart::LineImpl();
    m_chart->m_stacked = record->isFStacked();
    m_chart->m_f100 = record->isF100();
    if ( !m_disableAutoMarker )
        m_chart->m_markerType = KoChart::AutoMarker;
//     Q_FOREACH( const KoChart::Series* const series, m_chart->m_series )
//     {
//         if ( series->m_markerType == KoChart::Series::None )
//             m_chart->m_markerType = KoChart::NoMarker;
//     }
}

// specifies that the chartgroup is a scatter chart
void ChartSubStreamHandler::handleScatter(ScatterRecord* record)
{
    if (!record || m_chart->m_impl) return;
    DEBUG << "";
    if (record->isFBubbles())
        m_chart->m_impl = new KoChart::BubbleImpl(KoChart::BubbleImpl::SizeType(record->wBubbleSize()), record->pcBubbleSizeRatio(), record->isFShowNegBubbles());
    else
        m_chart->m_impl = new KoChart::ScatterImpl();

    // For scatter charts, one <chart:domain> element shall exist. Its table:cell-range-address
    // attribute references the x coordinate values for the scatter chart.
    // For bubble charts, two <chart:domain> elements shall exist. The values for the y-coordinates are
    // given by the first <chart:domain> element. The values for the x-coordinates are given by the
    // second <chart:domain> element.
    QString x, y;
    if (m_currentSeries->m_datasetValue.contains(KoChart::Value::VerticalValues))
        x = m_currentSeries->m_datasetValue[KoChart::Value::VerticalValues]->m_formula;
    if (m_currentSeries->m_datasetValue.contains(KoChart::Value::HorizontalValues))
        y = m_currentSeries->m_datasetValue[KoChart::Value::HorizontalValues]->m_formula;
    foreach(KoChart::Series *series, m_chart->m_series) {
        Q_ASSERT(series->m_domainValuesCellRangeAddress.isEmpty()); // what should we do if that happens?
        if (!series->m_domainValuesCellRangeAddress.isEmpty())
            continue;
        if (record->isFBubbles()) {
            series->m_domainValuesCellRangeAddress << y << x;
            if (series->m_datasetValue.contains(KoChart::Value::BubbleSizeValues))
                series->m_valuesCellRangeAddress = series->m_datasetValue[KoChart::Value::BubbleSizeValues]->m_formula;
            //m_chart->m_verticalCellRangeAddress = series->m_valuesCellRangeAddress;
        } else {
            series->m_domainValuesCellRangeAddress << x;
        }
    }

    if ( !m_disableAutoMarker ) {
        m_chart->m_markerType = KoChart::AutoMarker;
    }
    // KoChart::ScatterImpl* impl = dynamic_cast< KoChart::ScatterImpl* >( m_chart->m_impl );
    // if ( impl )
    //     impl->style = KoChart::ScatterImpl::Marker;
}

// specifies that the chartgroup is a radar chart
void ChartSubStreamHandler::handleRadar(RadarRecord *record)
{
    if (!record || m_chart->m_impl) return;
    DEBUG << "";
    m_chart->m_impl = new KoChart::RadarImpl(false);
    m_chart->m_markerType = KoChart::AutoMarker;
}

// specifies that the chartgroup is a filled radar chart
void ChartSubStreamHandler::handleRadarArea(RadarAreaRecord *record)
{
    if (!record || m_chart->m_impl) return;
    DEBUG << "";
    m_chart->m_impl = new KoChart::RadarImpl(true);
}

// specifies that the chartgroup is a surface chart
void ChartSubStreamHandler::handleSurf(SurfRecord *record)
{
    if (!record || m_chart->m_impl) return;
    DEBUG << "";
    m_chart->m_impl = new KoChart::SurfaceImpl(record->isFFillSurface());
}

void ChartSubStreamHandler::handleAxis(AxisRecord* record)
{
    if (!record) return;
    DEBUG << "wType=" << record->wType();
    KoChart::Axis* axis = new KoChart::Axis(KoChart::Axis::Type(record->wType()));
    m_chart->m_axes << axis;
    m_currentObj = axis;
}

// This record specifies which part of the axis is specified by the LineFormat record that follows.
void ChartSubStreamHandler::handleAxisLine(AxisLineRecord* record)
{
    if (!record) return;
    DEBUG << "identifier=" << record->identifier();
    m_axisId = record->identifier();
}

// This record specifies the attributes of the axis label.
// * wOffset: Specifies the distance between the axis and axis label. It contains the
//   offset as a percentage of the default distance. The default distance is equal to 1/3 the
//   height of the font calculated in pixels. MUST be a value greater than or equal to 0 (0%)
//   and less than or equal to 1000 (1000%).
// * at: An unsigned integer that specifies the alignment of the axis label. MUST be
//   a value from the following table:
//   * 0x0001: Top-aligned if the trot field of the Text record of the axis is not equal to 0.
//     Left-aligned if the iReadingOrder field of the Text record of the axis specifies left
//     to-right reading order; otherwise, right-aligned.
//   * 0x0002: Center-alignment
//   * 0x0003: Bottom-aligned if the trot field of the Text record of the axis is not equal to 0.
//     Right-aligned if the iReadingOrder field of the Text record of the axis specifies
//     left-to-right reading order; otherwise, left-aligned.
// * cAutoCatLabelReal: Specifies whether the number of categories (3).
//   between axis labels is set to the default value. MUST be a value from the following table:
//   * 0: The value is set to catLabel field as specified by CatSerRange record.
//   * 1: The value is set to the default value. The number of category (3) labels is
//     automatically calculated by the application based on the data in the chart.
void ChartSubStreamHandler::handleCatLab(CatLabRecord* record)
{
    if (!record) return;
    void handleCatLab(CatLabRecord* record);
    DEBUG << "wOffset=" << record->wOffset() << "at=" << record->at() << "cAutoCatLabelReal=" << record->cAutoCatLabelReal();
    //TODO
}

// Type of data contained in the Number records following.
void ChartSubStreamHandler::handleSIIndex(SIIndexRecord *record)
{
    if (!record) return;
    DEBUG << "numIndex=" << record->numIndex();
    Q_ASSERT(!m_internalDataCache);
    m_internalDataCache = new InternalDataCache(this, record->numIndex());
}

void ChartSubStreamHandler::handleMsoDrawing(MsoDrawingRecord* record)
{
    if (!record) return;
    DEBUG << "";
    //TODO
}

void ChartSubStreamHandler::handleShapePropsStream(ShapePropsStreamRecord* record)
{
    if (!record) return;
    DEBUG << "wObjContext=" << record->wObjContext() << "rgbLength=" << record->rgb().length() << "rgbString=" << record->rgb();
    //TODO
}

void ChartSubStreamHandler::handleTextPropsStream(TextPropsStreamRecord* record)
{
    if (!record) return;
    DEBUG << "rgbLength=" << record->rgb().length() << "rgbString=" << record->rgb();
    //TODO
}

void ChartSubStreamHandler::handleObjectLink(ObjectLinkRecord *record)
{
    if (!record) return;
    DEBUG << "wLinkObj=" << record->wLinkObj() << "wLinkVar1=" << record->wLinkVar1() << "wLinkVar2=" << record->wLinkVar2();

    KoChart::Text *t = dynamic_cast<KoChart::Text*>(m_currentObj);
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
            //KoChart::Series* series = m_chart->m_series[ record->wLinkVar1() ];
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
    DEBUG << "";
    m_currentObj = m_chart->m_plotArea = new KoChart::PlotArea();
}

void ChartSubStreamHandler::handleValueRange(ValueRangeRecord *record)
{
    if (!record) return;
    DEBUG << "fAutoMin=" << record->isFAutoMin() << "fAutoMax=" << record->isFAutoMax() << "fAutoMajor=" << record->isFAutoMajor()
          << "fAutoMinor=" << record->isFAutoMinor() << "fAutoCross=" << record->isFAutoCross() << "fLog=" << record->isFLog()
          << "fReversed=" << record->isFReversed() << "fMaxCross=" << record->isFMaxCross();
    if ( KoChart::Axis *axis = dynamic_cast< KoChart::Axis* > ( m_currentObj ) ) {
        axis->m_reversed = record->isFReversed();
        axis->m_logarithmic = record->isFLog();
        axis->m_autoMinimum = record->isFAutoMin();
        axis->m_autoMaximum = record->isFAutoMax();
        axis->m_minimum = record->isFAutoMin() ? 0 : record->numMin();
        axis->m_maximum = record->isFAutoMax() ? 0 : record->numMax();
    }
}

void ChartSubStreamHandler::handleTick(TickRecord *record)
{
    if (!record) return;
    DEBUG << "tktMajor=" << record->tktMajor()
          << "tktMinor=" << record->tktMinor()
          << "tlt=" << record->tlt();
    //TODO
}

void ChartSubStreamHandler::handleAxcExt(AxcExtRecord *record)
{
    if (!record) return;
    DEBUG << "fAutoMin=" << record->isFAutoMin()
          << "fAutoMax=" << record->isFAutoMax()
          << "fAutoMajor=" << record->isFAutoMajor()
          << "fAutoMinor=" << record->isFAutoMinor()
          << "fDateAxis=" << record->isFDateAxis()
          << "fAutoBase=" << record->isFAutoBase()
          << "fAutoCross=" << record->isFAutoCross()
          << "fAutoDate=" << record->isFAutoDate();
    //TODO
}

// Specifies the presence of drop lines, high-low lines, series lines or leader lines on the chart group. This record is followed by a LineFormat record which specifies the format of the lines.
void ChartSubStreamHandler::handleCrtLine(CrtLineRecord *record)
{
    if (!record) return;
    DEBUG << "identifier=" << record->identifier();
    switch(record->identifier()) {
        case 0x0000: // Drop lines below the data points of line, area, and stock chart groups.
            //TODO
            break;
        case 0x0001: // High-Low lines around the data points of line and stock chart groups.
            if (KoChart::LineImpl* line = dynamic_cast<KoChart::LineImpl*>(m_chart->m_impl)) {
                // It seems that a stockchart is always a linechart with a CrtLine record that defines High-Low lines.
                delete line;
                m_chart->m_impl = new KoChart::StockImpl();
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
    DEBUG << "fBetween=" << record->isFBetween() << "fMaxCross=" << record->isFMaxCross() << "fReverse=" << record->isFReverse();
    //TODO
}

void ChartSubStreamHandler::handleAttachedLabel(AttachedLabelRecord *record)
{
    if (!record) return;
    DEBUG << "fShowValue=" << record->isFShowValue() << "fShowPercent=" << record->isFShowPercent()
          << "fShowLabelAndPerc=" << record->isFShowLabelAndPerc() << "fShowLabel=" << record->isFShowLabel()
          << "fShowBubbleSizes=" << record->isFShowBubbleSizes() << "fShowSeriesName=" << record->isFShowSeriesName();
    if (m_currentSeries) {
        m_currentSeries->m_showDataLabelValues = record->isFShowValue();
        m_currentSeries->m_showDataLabelPercent = record->isFShowPercent() || record->isFShowLabelAndPerc();
        m_currentSeries->m_showDataLabelCategory = record->isFShowLabel() || record->isFShowLabelAndPerc();
        m_currentSeries->m_showDataLabelSeries = record->isFShowSeriesName();
    }
}

void ChartSubStreamHandler::handleDataLabelExtContents(DataLabelExtContentsRecord *record)
{
    if (!record) return;
    DEBUG << "rt=" << record->rt() << "grbitFrt=" << record->grbitFrt() << "fSerName=" << record->isFSerName()
          << "fCatName=" << record->isFCatName() << "fValue=" << record->isFValue() << "fPercent=" << record->isFPercent()
          << "fBubSize=" << record->isFBubSize();
    //TODO
}

void ChartSubStreamHandler::handleXF(XFRecord *record)
{
    if (!record) return;
    DEBUG << "formatIndex=" << record->formatIndex();
    m_xfTable.push_back(*record);
}

// This record specifies a label on the category (3) axis for each series.
void ChartSubStreamHandler::handleLabel(LabelRecord *record)
{
    if (!record) return;
    DEBUG << "row=" << record->row() << "column=" << record->column() << "xfIndex=" << record->xfIndex()
          << "label=" << record->label().toUtf8();
    //TODO
}

// This record specifies the number format to use for the text on an axis.
void ChartSubStreamHandler::handleIFmt(IFmtRecord *record)
{
    if (!record) return;
    //Q_ASSERT(record->ifmt() >= 0x00A4 && record->ifmt() <= 0x0188);
    const Format *f = globals()->convertedFormat(record->ifmt());
    if (!f) return;
    DEBUG << "ifmt=" << record->ifmt() << "valueFormat=" << qPrintable(f->valueFormat());
//     if (!f->valueFormat().isEmpty() && f->valueFormat() != "General") {
//         Q_ASSERT(false);
//     }
    //TODO
}

void ChartSubStreamHandler::handleCrtMlFrt(CrtMlFrtRecord *record)
{
    if (!record) return;
    std::stringstream out;
    record->dump(out);
    DEBUG << "xmlTkParent=" << QString::number(record->xmlTkParent(), 16) << "tokens=" << QString::fromStdString(out.str());

    //TODO
}
