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

#ifndef SWINDER_CHARTSUBSTREAMHANDLER_H
#define SWINDER_CHARTSUBSTREAMHANDLER_H

#include "substreamhandler.h"
#include "objects.h"
#include "swinder.h"
//#include "ustring.h"
//#include <vector>
//#include "cell.h"
//#include "sheet.h"
//#include <map>

#include <QStack>

namespace KoChart
{
    class Chart;
    class Series;
    class Obj;
}

namespace Swinder
{

class GlobalsSubStreamHandler;

class BRAIRecord;
class CrtMlFrtRecord;

// The chart substream can be either embedded into a worksheet or define an own sheet.
class ChartSubStreamHandler : public SubStreamHandler
{
public:
    ChartSubStreamHandler(GlobalsSubStreamHandler* globals, SubStreamHandler* parentHandler);
    ~ChartSubStreamHandler() override;
    //Chart *chart() const { return m_chart; }
    GlobalsSubStreamHandler *globals() const { return m_globals; }
    SubStreamHandler *parentHandler() const { return m_parentHandler; }
    void handleRecord(Record* record) override;

private:
    GlobalsSubStreamHandler* m_globals;
    SubStreamHandler* m_parentHandler;
    Sheet* m_sheet;
    ChartObject* m_chartObject;
    KoChart::Chart* m_chart;

    KoChart::Series* m_currentSeries;
    KoChart::Obj* m_currentObj;
    QStack<KoChart::Obj*> m_stack;
    QStack<KoChart::Series*> m_seriesStack;
    std::vector<XFRecord> m_xfTable;

    class InternalDataCache;
    friend class InternalDataCache;
    InternalDataCache *m_internalDataCache;

    //QMap<KoChart::Obj*, int> m_defaultObjects;
    int m_defaultTextId;
    int m_axisId;
    
    bool m_disableAutoMarker;
    
    void handleBOF(BOFRecord*);
    void handleEOF(EOFRecord *);
    void handleFooter(FooterRecord *);
    void handleHeader(HeaderRecord *);
    void handleSetup(SetupRecord *);
    void handleHCenter(HCenterRecord *);
    void handleVCenter(VCenterRecord *);
    void handleZoomLevel(ZoomLevelRecord *);
    void handleLeftMargin(LeftMarginRecord *);
    void handleRightMargin(RightMarginRecord *);
    void handleTopMargin(TopMarginRecord *);
    void handleBottomMargin(BottomMarginRecord *);
    void handleDimension(DimensionRecord *);
    void handleChart(ChartRecord *);
    void handleBegin(BeginRecord *);
    void handleEnd(EndRecord *);
    void handleFrame(FrameRecord *);
    void handleSeries(SeriesRecord *);
    void handleSeriesList(SeriesListRecord *);
    void handleNumber(NumberRecord *);
    void handleBRAI(BRAIRecord *);
    void handleDataFormat(DataFormatRecord *);
    void handleChart3DBarShape(Chart3DBarShapeRecord *);
    void handleChart3d(Chart3dRecord *);
    void handleLineFormat(LineFormatRecord *);
    void handleAreaFormat(AreaFormatRecord *);
    void handlePieFormat(PieFormatRecord *);
    void handleMarkerFormat(MarkerFormatRecord *);
    void handleChartFormat(ChartFormatRecord *);
    void handleGelFrame(GelFrameRecord *);
    void handleSerToCrt(SerToCrtRecord *);
    void handleShtProps(ShtPropsRecord *);
    void handleDefaultText(DefaultTextRecord *);
    void handleText(TextRecord *);
    void handleSeriesText(SeriesTextRecord *);
    void handlePos(PosRecord *);
    void handleFontX(FontXRecord *);
    void handlePlotGrowth(PlotGrowthRecord *);
    void handleLegend(LegendRecord *);
    void handleAxesUsed(AxesUsedRecord *);
    void handleAxisParent(AxisParentRecord *);
    void handlePie(PieRecord *);
    void handleBar(BarRecord *);
    void handleArea(AreaRecord *);
    void handleLine(LineRecord *);
    void handleScatter(ScatterRecord *);
    void handleRadar(RadarRecord *);
    void handleRadarArea(RadarAreaRecord *);
    void handleSurf(SurfRecord *);
    void handleAxis(AxisRecord* record);
    void handleAxisLine(AxisLineRecord* record);
    void handleCatLab(CatLabRecord* record);
    void handleSIIndex(SIIndexRecord *);
    void handleMsoDrawing(MsoDrawingRecord *);
    void handleShapePropsStream(ShapePropsStreamRecord *);
    void handleTextPropsStream(TextPropsStreamRecord *);
    void handleObjectLink(ObjectLinkRecord *);
    void handlePlotArea(PlotAreaRecord *);
    void handleValueRange(ValueRangeRecord *);
    void handleTick(TickRecord *);
    void handleAxcExt(AxcExtRecord *);
    void handleCrtLine(CrtLineRecord *);
    void handleCatSerRange(CatSerRangeRecord *);
    void handleAttachedLabel(AttachedLabelRecord *);
    void handleDataLabelExtContents(DataLabelExtContentsRecord *);
    void handleXF(XFRecord *);
    void handleLabel(LabelRecord *);
    void handleIFmt(IFmtRecord *);
    void handleCrtMlFrt(CrtMlFrtRecord *);
};

} // namespace Swinder

#endif // SWINDER_CHARTSUBSTREAMHANDLER_H
