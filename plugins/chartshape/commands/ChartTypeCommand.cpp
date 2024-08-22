/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ChartTypeCommand.h"

// KF5
#include <KLocalizedString>

// KChart
#include "KChartAbstractCoordinatePlane"
#include "KChartBarDiagram"
#include "KChartChart"
#include "KChartLineDiagram"
#include "KChartPieDiagram"
#include "KChartPolarDiagram"
#include "KChartRingDiagram"

// KoChart
#include "ChartDebug.h"
#include "ChartShape.h"
#include "Legend.h"

using namespace KoChart;
using namespace KChart;

ChartTypeCommand::ChartTypeCommand(ChartShape *chart)
    : m_chart(chart)
    , m_oldType(BarChartType)
    , m_newType(BarChartType)
    , m_oldSubtype(NormalChartSubtype)
    , m_newSubtype(NormalChartSubtype)
{
}

ChartTypeCommand::~ChartTypeCommand() = default;

void ChartTypeCommand::redo()
{
    // save the old type
    m_oldType = m_chart->chartType();
    m_oldSubtype = m_chart->chartSubType();
    if (m_oldType == m_newType && m_oldSubtype == m_newSubtype)
        return;

    // Actually do the work
    m_chart->setChartType(m_newType);
    m_chart->setChartSubType(m_newSubtype, m_newType == StockChartType);
    m_chart->update();
    m_chart->legend()->update();
}

void ChartTypeCommand::undo()
{
    if (m_oldType == m_newType && m_oldSubtype == m_newSubtype)
        return;

    m_chart->setChartType(m_oldType);
    m_chart->setChartSubType(m_oldSubtype, m_oldType == StockChartType);
    m_chart->update();
    m_chart->legend()->update();
}

void ChartTypeCommand::setChartType(ChartType type, ChartSubtype subtype)
{
    m_newType = type;
    m_newSubtype = subtype;

    switch (type) {
    case BarChartType:
        switch (subtype) {
        case NormalChartSubtype:
            setText(kundo2_i18n("Normal Bar Chart"));
            break;
        case StackedChartSubtype:
            setText(kundo2_i18n("Stacked Bar Chart"));
            break;
        case PercentChartSubtype:
            setText(kundo2_i18n("Percent Bar Chart"));
            break;
        default:
            Q_ASSERT("Invalid bar chart subtype!");
        }
        break;
    case LineChartType:
        switch (subtype) {
        case NormalChartSubtype:
            setText(kundo2_i18n("Normal Line Chart"));
            break;
        case StackedChartSubtype:
            setText(kundo2_i18n("Stacked Line Chart"));
            break;
        case PercentChartSubtype:
            setText(kundo2_i18n("Percent Line Chart"));
            break;
        default:
            Q_ASSERT("Invalid line chart subtype!");
        }
        break;
    case AreaChartType:
        switch (subtype) {
        case NormalChartSubtype:
            setText(kundo2_i18n("Normal Area Chart"));
            break;
        case StackedChartSubtype:
            setText(kundo2_i18n("Stacked Area Chart"));
            break;
        case PercentChartSubtype:
            setText(kundo2_i18n("Percent Area Chart"));
            break;
        default:
            Q_ASSERT("Invalid area chart subtype!");
        }
        break;
    case CircleChartType:
        setText(kundo2_i18n("Circle Chart"));
        break;
    case RingChartType:
        setText(kundo2_i18n("Ring Chart"));
        break;
    case ScatterChartType:
        setText(kundo2_i18n("Scatter Chart"));
        break;
    case RadarChartType:
        setText(kundo2_i18n("Radar Chart"));
        break;
    case FilledRadarChartType:
        setText(kundo2_i18n("Filled Radar Chart"));
        break;
    case StockChartType:
        setText(kundo2_i18n("Stock Chart"));
        break;
    case BubbleChartType:
        setText(kundo2_i18n("Bubble Chart"));
        break;
    case SurfaceChartType:
        setText(kundo2_i18n("Surface Chart"));
        break;
    case GanttChartType:
        setText(kundo2_i18n("Gantt Chart"));
        break;
    case LastChartType:
    default:
        break;
    }
}
