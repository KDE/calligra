/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2018 Dag Andersen <danders@get2net.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "PlotAreaCommand.h"

// KF5
#include <KLocalizedString>

// KChart
#include "KChartCartesianAxis.h"

// KoChart
#include "Axis.h"
#include "ChartDebug.h"
#include "ChartShape.h"
#include "PlotArea.h"

using namespace KoChart;

PlotAreaCommand::PlotAreaCommand(PlotArea *plotArea)
    : m_plotArea(plotArea)
    , m_chart(plotArea->parent())
{
    m_newOrientation = m_oldOrientation = plotArea->isVertical() ? Qt::Vertical : Qt::Horizontal;
}

PlotAreaCommand::~PlotAreaCommand() = default;

void PlotAreaCommand::redo()
{
    KUndo2Command::redo();
    if (m_oldOrientation != m_newOrientation) {
        m_plotArea->setVertical(m_newOrientation == Qt::Vertical);
    }
    m_chart->update();
    m_chart->relayout();
}

void PlotAreaCommand::undo()
{
    if (m_oldOrientation != m_newOrientation) {
        m_plotArea->setVertical(m_oldOrientation == Qt::Vertical);
    }
    KUndo2Command::undo();
    m_chart->update();
    m_chart->relayout();
}

void PlotAreaCommand::setOrientation(Qt::Orientation orientation)
{
    m_newOrientation = orientation;
    if (orientation == Qt::Vertical) {
        setText(kundo2_i18n("Set Horizontal Bars"));
    } else {
        setText(kundo2_i18n("Set Vertical Bars"));
    }
}
