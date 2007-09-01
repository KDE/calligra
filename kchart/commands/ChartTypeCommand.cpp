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
#include <klocalizedstring.h>

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
#if 0  //Temporarily disabled
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
#endif
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
    // save the model
    QAbstractItemModel* model = m_chart->coordinatePlane()->diagram()->model();
    // set a proper coordinate plane
    replaceCoordinatePlane(m_newType);
    // set the new type
    replaceDiagram(m_newType);
    // transfer the model
    m_chart->coordinatePlane()->diagram()->setModel(model);
    m_chart->coordinatePlane()->relayout();
}

void ChartTypeCommand::undo()
{
    if (m_oldType == m_newType)
        return;
    kDebug() << m_oldType;
    // save the model
    QAbstractItemModel* model = m_chart->coordinatePlane()->diagram()->model();
    // restore the old coordinate plane
    m_chart->replaceCoordinatePlane(m_oldCoordinatePlane);
    m_oldCoordinatePlane = 0;
    // restore the old diagram
    m_chart->coordinatePlane()->replaceDiagram(m_oldDiagram);
    m_oldDiagram = 0;
    // transfer the model
    m_chart->coordinatePlane()->diagram()->setModel(model);
    m_chart->coordinatePlane()->relayout();
}

void ChartTypeCommand::setChartType(OdfChartType type)
{
    m_newType = type;

    switch (type) {
    case BarChartType:
        setText(i18n("Bar Chart"));
        break;
    case LineChartType:
        setText(i18n("Line Chart"));
        break;
    case CircleChartType:
        setText(i18n("Circle Chart"));
        break;
    case RingChartType:
        setText(i18n("Ring Chart"));
        break;
    case RadarChartType:
        setText(i18n("Radar Chart"));
        break;
    case AreaChartType:
        setText(i18n("Area Chart"));
        break;
    case StockChartType:
        setText(i18n("Stock Chart"));
        break;
#if 0  // Temporarily disabled
    case HiLoChartType:
        setText(i18n("HiLo Chart"));
        break;
#endif
    }
}

void ChartTypeCommand::replaceCoordinatePlane(OdfChartType type)
{
    switch (type) {
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
    case CircleChartType:
    case RingChartType:
    case RadarChartType:
        if (!qobject_cast<PolarCoordinatePlane*>(m_chart->coordinatePlane())) {
            kDebug() << "replacing coordinate plane by a polar coordinate plane";
            m_oldCoordinatePlane = m_chart->coordinatePlane();
            m_chart->takeCoordinatePlane(m_oldCoordinatePlane);
            AbstractCoordinatePlane* coordinatePlane = new PolarCoordinatePlane();
            m_chart->addCoordinatePlane(coordinatePlane);
        }
        break;
    case AreaChartType:
    case StockChartType:
#if 0
    case HiLoChartType:
#endif
        // not supported yet
        break;
    }
}

void ChartTypeCommand::replaceDiagram(OdfChartType type)
{
    Q_ASSERT(m_chart->coordinatePlane());
    AbstractDiagram* diagram = 0;
    switch (type) {
    case BarChartType:
        diagram = new BarDiagram();
        break;
    case LineChartType:
        diagram = new LineDiagram();
        break;
    case AreaChartType:
        kDebug() << "Area not supported yet";
        break;
    case CircleChartType:
        diagram = new PieDiagram();
        break;
#if 0
    case HiLoChartType:
        kDebug() << "HiLo not supported yet";
        break;
#endif
    case RingChartType:
        diagram = new RingDiagram();
        break;
    case RadarChartType:
        diagram = new PolarDiagram();
        break;
    case StockChartType:
        kDebug() << "Stock not supported yet";
        break;
    }

    if (diagram) {
        // save the old diagram
        m_oldDiagram = m_chart->coordinatePlane()->diagram();
        // remove but do not delete old diagram
        m_chart->coordinatePlane()->takeDiagram(m_oldDiagram);
        // set the new diagram
        m_chart->coordinatePlane()->replaceDiagram(diagram);
    }
}
