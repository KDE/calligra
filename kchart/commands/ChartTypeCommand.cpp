/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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
   Boston, MA 02110-1301, USA.
*/

#include "ChartTypeCommand.h"

// KDE
#include <kdebug.h>

// KDChart
#include "KDChartAbstractCoordinatePlane.h"
#include "KDChartBarDiagram.h"
#include "KDChartChart.h"
#include "KDChartLineDiagram.h"
#include "KDChartPieDiagram.h"
#include "KDChartPolarDiagram.h"
#include "KDChartRingDiagram.h"

using namespace KChart;
using namespace KDChart;

static OdfChartType chartType(AbstractDiagram* diagram)
{
    if (qobject_cast<BarDiagram*>(diagram))
        return BarChartType;
    else if (qobject_cast<LineDiagram*>(diagram))
        return LineChartType;
    else if (qobject_cast<PieDiagram*>(diagram))
        return PieChartType;
    else if (qobject_cast<RingDiagram*>(diagram))
        return RingChartType;
    else if (qobject_cast<PolarDiagram*>(diagram))
        return PolarChartType;
    return BoxWhiskerChartType; // unsupported until now
}

ChartTypeCommand::ChartTypeCommand(Chart* chart)
    : m_chart(chart)
    , m_oldType(BarChartType)
    , m_newType(BarChartType)
    , m_oldCoordinatePlane(0)
    , m_oldDiagram(0)
{
}

ChartTypeCommand::~ChartTypeCommand()
{
}

void ChartTypeCommand::redo()
{
    kDebug() << m_newType;
    // save the old type
    m_oldType = chartType(m_chart->coordinatePlane()->diagram());
    if (m_oldType == m_newType)
        return;
    // set a proper coordinate plane
    replaceCoordinatePlane(m_newType);
    // set the new type
    replaceDiagram(m_newType);
}

void ChartTypeCommand::undo()
{
    if (m_oldType == m_newType)
        return;
    // restore the old coordinate plane
    m_chart->replaceCoordinatePlane(m_oldCoordinatePlane);
    m_oldCoordinatePlane = 0;
    // restore the old diagram
    m_chart->coordinatePlane()->replaceDiagram(m_oldDiagram);
    m_oldDiagram = 0;
}

void ChartTypeCommand::setChartType(OdfChartType type)
{
    m_newType = type;
}

void ChartTypeCommand::replaceCoordinatePlane(OdfChartType type)
{
    switch (type)
    {
    case BarChartType:
    case LineChartType:
        if (!qobject_cast<CartesianCoordinatePlane*>(m_chart->coordinatePlane()))
        {
            kDebug() << "replacing coordinate plane by a cartesian coordinate plane";
            m_oldCoordinatePlane = m_chart->coordinatePlane();
            m_chart->takeCoordinatePlane(m_oldCoordinatePlane);
            AbstractCoordinatePlane* coordinatePlane = new CartesianCoordinatePlane();
            m_chart->addCoordinatePlane(coordinatePlane);
        }
        break;
    case PieChartType:
    case RingChartType:
    case PolarChartType:
        if (!qobject_cast<PolarCoordinatePlane*>(m_chart->coordinatePlane()))
        {
            kDebug() << "replacing coordinate plane by a polar coordinate plane";
            m_oldCoordinatePlane = m_chart->coordinatePlane();
            m_chart->takeCoordinatePlane(m_oldCoordinatePlane);
            AbstractCoordinatePlane* coordinatePlane = new PolarCoordinatePlane();
            m_chart->addCoordinatePlane(coordinatePlane);
        }
        break;
    case AreaChartType:
    case BoxWhiskerChartType:
    case HiLoChartType:
        // not supported yet
        break;
    }
}

void ChartTypeCommand::replaceDiagram(OdfChartType type)
{
    Q_ASSERT(m_chart->coordinatePlane());
    // save the old diagram
    m_oldDiagram = m_chart->coordinatePlane()->diagram();
    // do not delete old diagram
    m_chart->coordinatePlane()->takeDiagram(m_oldDiagram);
    switch (type)
    {
    case BarChartType:
        m_chart->coordinatePlane()->replaceDiagram(new BarDiagram());
        break;
    case LineChartType:
        m_chart->coordinatePlane()->replaceDiagram(new LineDiagram());
        break;
    case AreaChartType:
        kDebug() << "Area not supported yet";
//         m_chart->coordinatePlane()->replaceDiagram(new AreaDiagram());
        break;
    case PieChartType:
        m_chart->coordinatePlane()->replaceDiagram(new PieDiagram());
        break;
    case HiLoChartType:
        kDebug() << "HiLo not supported yet";
//         m_chart->coordinatePlane()->replaceDiagram(new HiLoDiagram());
        break;
    case RingChartType:
        m_chart->coordinatePlane()->replaceDiagram(new RingDiagram());
        break;
    case PolarChartType:
        m_chart->coordinatePlane()->replaceDiagram(new PolarDiagram());
        break;
    case BoxWhiskerChartType:
        kDebug() << "BoxWhisker not supported yet";
//         m_chart->coordinatePlane()->replaceDiagram(new BoxWhiskerDiagram());
        break;
    }
}
