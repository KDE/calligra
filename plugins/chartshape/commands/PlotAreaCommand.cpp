/* This file is part of the KDE project
 * Copyright 2018 Dag Andersen <danders@get2net.dk>
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB. If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "PlotAreaCommand.h"

// KF5
#include <klocalizedstring.h>

// KChart
#include "KChartCartesianAxis.h"

// KoChart
#include "ChartShape.h"
#include "PlotArea.h"
#include "Axis.h"
#include "ChartDebug.h"

using namespace KoChart;


PlotAreaCommand::PlotAreaCommand(PlotArea *plotArea)
    : m_plotArea(plotArea)
    , m_chart(plotArea->parent())
{
    m_newOrientation = m_oldOrientation = plotArea->isVertical() ? Qt::Vertical : Qt::Horizontal;
}

PlotAreaCommand::~PlotAreaCommand()
{
}

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
