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
#include "globalssubstreamhandler.h"
#include "excel.h"
//#include "ustring.h"
//#include <vector>
//#include "cell.h"
//#include "sheet.h"
//#include <map>

namespace Swinder
{

// class FormulaToken;
// typedef std::vector<FormulaToken> FormulaTokens;

class GlobalsSubStreamHandler;

class Chart
{
public:
    int x, y, width, height;
    //int marginLeft, marginTop, marginRight, MarginBottom;
    uint dataTypeX, dataTypeY, countXValues, countYValues, bubbleSizeDataType, countBubbleSizeValues;
    explicit Chart() : x(-1), y(-1), width(-1), height(-1) {}
};

class ChartSubStreamHandler : public SubStreamHandler
{
public:
    explicit ChartSubStreamHandler(const GlobalsSubStreamHandler* globals) : SubStreamHandler(), m_chart(new Chart()) {}
    virtual ~ChartSubStreamHandler() {}

    virtual void handleRecord(Record* record) {
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
        else if (type == BRAIRecord::id)
            handleBRAI(static_cast<BRAIRecord*>(record));
        else {
            std::cout << "Unhandled chart record with type=" << type << " name=" << record->name() << std::endl;
        }
    }

private:
    Chart *m_chart;
    void handleBOF(BOFRecord*) {
        std::cout << "ChartSubStreamHandler BOFRecord" << std::endl;
    }
    void handleEOF(EOFRecord *) {
        std::cout << "ChartSubStreamHandler EOFRecord" << std::endl;
    }
    void handleFooter(FooterRecord *) {
    }
    void handleHeader(HeaderRecord *) {
    }
    void handleSetup(SetupRecord *) {
    }
    void handleHCenter(HCenterRecord *) {
    }
    void handleVCenter(VCenterRecord *) {
    }
    void handleZoomLevel(ZoomLevelRecord *) {
    }
    void handleChart(ChartRecord *record) {
        if(!record) return;
        std::cout << "ChartSubStreamHandler ChartRecord" << std::endl;

        m_chart->x = record->x();
        m_chart->y = record->y();
        m_chart->width = record->width();
        m_chart->height = record->height();
    }
    void handleBegin(BeginRecord *) { // secifies the begin of a collection of records
    }
    void handleEnd(EndRecord *) { // sepcified the end of a collection of records
    }
    void handleFrame(FrameRecord *record) {
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
    void handleSeries(SeriesRecord *record) { // series, trendline or errorchars
        if(!record) return;
        std::cout << "ChartSubStreamHandler::handleSeries dataTypeX=" << record->dataTypeX() << " dataTypeY=" << record->dataTypeY() << " countXValues=" << record->countXValues() << " countYValues=" << record->countYValues() << " bubbleSizeDataType=" << record->bubbleSizeDataType() << " countBubbleSizeValues=" << record->countBubbleSizeValues() << std::endl;
            
        m_chart->dataTypeX = record->dataTypeX();
        m_chart->dataTypeY = record->dataTypeY();
        m_chart->countXValues = record->countXValues();
        m_chart->countYValues = record->countYValues();
        m_chart->bubbleSizeDataType = record->bubbleSizeDataType();
        m_chart->countBubbleSizeValues = record->countBubbleSizeValues();
    }
    void handleBRAI(BRAIRecord *record) {
        if(!record) return;
        std::cout << "ChartSubStreamHandler::handleBRAI dataId=" << record->dataId() << " type=" << record->type() << " isUnlinkedNumberFormat=" << record->isUnlinkedNumberFormat() << " numberFormat=" << record->numberFormat() << std::endl;
        //TODO
    }
};

} // namespace Swinder

#endif // SWINDER_CHARTSUBSTREAMHANDLER_H
