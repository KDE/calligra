/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KCHART_CHART_TYPE_COMMAND
#define KCHART_CHART_TYPE_COMMAND

// Qt
#include <kundo2command.h>

// KoChart
#include "kochart_global.h"
// #include "ChartShape.h"

#if 0
namespace KChart
{
class AbstractCoordinatePlane;
class AbstractDiagram;
class Chart;
}
#endif

namespace KoChart
{

class ChartShape;

/**
 * Chart type replacement command.
 */
class ChartTypeCommand : public KUndo2Command
{
public:
    /**
     * Constructor.
     */
    explicit ChartTypeCommand(ChartShape *chart);

    /**
     * Destructor.
     */
    virtual ~ChartTypeCommand();

    /**
     * Executes the actual operation.
     */
    void redo() override;

    /**
     * Executes the actual operation in reverse order.
     */
    void undo() override;

    /**
     * Sets the new chart type.
     */
    void setChartType(ChartType type, ChartSubtype subType);

private:
    ChartShape *m_chart;
    // KChart::Chart                    *m_chart;
    ChartType m_oldType;
    ChartType m_newType;
    ChartSubtype m_oldSubtype;
    ChartSubtype m_newSubtype;
    // KChart::AbstractCoordinatePlane  *m_oldCoordinatePlane;
    // KChart::AbstractDiagram          *m_oldDiagram;
};

} // namespace KoChart

#endif // KCHART_CHART_TYPE_COMMAND
