/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2012 Brijesh Patel <brijesh3105@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "DatasetCommand.h"

// KF5
#include <KLocalizedString>

// KoChart
#include "Axis.h"
#include "ChartDebug.h"
#include "DataSet.h"

using namespace KoChart;
using namespace KChart;

DatasetCommand::DatasetCommand(DataSet *dataSet, ChartShape *chart, int section, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_dataSet(dataSet)
    , m_chart(chart)
    , m_section(section)
{
    m_newType = dataSet->chartType();
    m_newSubtype = dataSet->chartSubType();
    m_newShowCategory = dataSet->valueLabelType(section).category;
    m_newShowNumber = dataSet->valueLabelType(section).number;
    m_newShowPercent = dataSet->valueLabelType(section).percentage;
    m_newShowSymbol = dataSet->valueLabelType(section).symbol;
    m_newBrushColor = dataSet->brush(section).color();
    m_newPenColor = dataSet->pen(section).color();
    m_newOdfSymbolType = dataSet->odfSymbolType();
    m_newMarkerStyle = dataSet->markerStyle();
    m_newAxis = dataSet->attachedAxis();
}

DatasetCommand::~DatasetCommand() = default;

void DatasetCommand::redo()
{
    // save the old type
    /*m_oldType = m_dataSet->chartType();
    m_oldSubtype = m_dataSet->chartSubType();*/
    m_oldShowCategory = m_dataSet->valueLabelType(m_section).category;
    m_oldShowNumber = m_dataSet->valueLabelType(m_section).number;
    m_oldShowPercent = m_dataSet->valueLabelType(m_section).percentage;
    m_oldShowSymbol = m_dataSet->valueLabelType(m_section).symbol;
    m_oldBrushColor = m_dataSet->brush(m_section).color();
    m_oldPenColor = m_dataSet->pen(m_section).color();
    m_oldOdfSymbolType = m_dataSet->odfSymbolType();
    m_oldMarkerStyle = m_dataSet->markerStyle();
    m_oldAxis = m_dataSet->attachedAxis();
    m_oldType = m_dataSet->chartType();
    m_oldSubtype = m_dataSet->chartSubType();

    if (m_oldShowCategory != m_newShowCategory || m_oldShowNumber != m_newShowNumber || m_oldShowPercent != m_newShowPercent
        || m_oldShowSymbol != m_newShowSymbol) {
        DataSet::ValueLabelType valueLabelType = m_dataSet->valueLabelType(m_section);
        valueLabelType.category = m_newShowCategory;
        valueLabelType.number = m_newShowNumber;
        valueLabelType.percentage = m_newShowPercent;
        valueLabelType.symbol = m_newShowSymbol;
        debugChartUiDataSet << "section:" << m_section << m_dataSet->valueLabelType(m_section) << "->" << valueLabelType;
        m_dataSet->setValueLabelType(valueLabelType, m_section);
    }
    if (m_oldBrushColor != m_newBrushColor) {
        m_dataSet->setBrush(m_section, QBrush(m_newBrushColor));
    }
    if (m_oldPenColor != m_newPenColor) {
        m_dataSet->setPen(m_section, QPen(m_newPenColor, 0));
    }
    if (m_oldOdfSymbolType != m_newOdfSymbolType) {
        m_dataSet->setOdfSymbolType(m_newOdfSymbolType);
    }
    if (m_oldMarkerStyle != m_newMarkerStyle) {
        m_dataSet->setMarkerStyle(m_newMarkerStyle);
    }
    if (m_newAxis != m_oldAxis) {
        m_oldAxis->detachDataSet(m_dataSet);
        m_newAxis->attachDataSet(m_dataSet);
    }
    if (m_newType != m_oldType) {
        m_dataSet->setChartType(m_newType);
    }
    if (m_newSubtype != m_oldSubtype) {
        m_dataSet->setChartSubType(m_newSubtype);
    }
    m_chart->updateAll();
}

void DatasetCommand::undo()
{
    if (m_oldShowCategory != m_newShowCategory || m_oldShowNumber != m_newShowNumber || m_oldShowPercent != m_newShowPercent
        || m_oldShowSymbol != m_newShowSymbol) {
        DataSet::ValueLabelType valueLabelType = m_dataSet->valueLabelType(m_section);
        valueLabelType.category = m_oldShowCategory;
        valueLabelType.number = m_oldShowNumber;
        valueLabelType.percentage = m_oldShowPercent;
        valueLabelType.symbol = m_oldShowSymbol;
        debugChartUiDataSet << "section:" << m_section << m_dataSet->valueLabelType(m_section) << "->" << valueLabelType;
        m_dataSet->setValueLabelType(valueLabelType, m_section);
    }
    if (m_oldBrushColor != m_newBrushColor) {
        m_dataSet->setBrush(m_section, QBrush(m_oldBrushColor));
    }
    if (m_oldPenColor != m_newPenColor) {
        m_dataSet->setPen(m_section, QPen(m_oldPenColor, 0));
    }
    if (m_oldOdfSymbolType != m_newOdfSymbolType) {
        m_dataSet->setOdfSymbolType(m_oldOdfSymbolType);
    }
    if (m_oldMarkerStyle != m_newMarkerStyle) {
        m_dataSet->setMarkerStyle(m_oldMarkerStyle);
    }
    if (m_newAxis != m_oldAxis) {
        m_newAxis->detachDataSet(m_dataSet);
        m_oldAxis->attachDataSet(m_dataSet);
    }
    if (m_newType != m_oldType) {
        m_dataSet->setChartType(m_oldType);
    }
    if (m_newSubtype != m_oldSubtype) {
        m_dataSet->setChartSubType(m_oldSubtype);
    }
    m_chart->updateAll();
}

void DatasetCommand::setDataSetChartType(ChartType type, ChartSubtype subtype)
{
    m_newType = type;
    m_newSubtype = subtype;

    setText(kundo2_i18n("Set Dataset Chart Type"));
}

void DatasetCommand::setDataSetShowCategory(bool show)
{
    m_newShowCategory = show;

    if (show) {
        setText(kundo2_i18n("Show Dataset Category"));
    } else {
        setText(kundo2_i18n("Hide Dataset Category"));
    }
}

void DatasetCommand::setDataSetShowNumber(bool show)
{
    m_newShowNumber = show;

    if (show) {
        setText(kundo2_i18n("Show Dataset Number"));
    } else {
        setText(kundo2_i18n("Hide Dataset Number"));
    }
}

void DatasetCommand::setDataSetShowPercent(bool show)
{
    m_newShowPercent = show;

    if (show) {
        setText(kundo2_i18n("Show Dataset Percent"));
    } else {
        setText(kundo2_i18n("Hide Dataset Percent"));
    }
}

void DatasetCommand::setDataSetShowSymbol(bool show)
{
    m_newShowSymbol = show;

    if (show) {
        setText(kundo2_i18n("Show Dataset Symbol"));
    } else {
        setText(kundo2_i18n("Hide Dataset Symbol"));
    }
}

void DatasetCommand::setDataSetBrush(const QColor &color)
{
    m_newBrushColor = color;

    setText(kundo2_i18n("Set Dataset Brush Color"));
}

void DatasetCommand::setDataSetPen(const QColor &color)
{
    m_newPenColor = color;

    setText(kundo2_i18n("Set Dataset Pen Color"));
}

void DatasetCommand::setDataSetMarker(OdfSymbolType type, OdfMarkerStyle style)
{
    m_newOdfSymbolType = type;
    m_newMarkerStyle = style;

    if (type == NoSymbol) {
        setText(kundo2_i18n("Hide Dataset Marker"));
    } else {
        setText(kundo2_i18n("Show Dataset Marker Symbol"));
    }
}

void DatasetCommand::setDataSetAxis(Axis *axis)
{
    m_newAxis = axis;

    setText(kundo2_i18n("Set Dataset Axis"));
}
