/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2017 Dag Andersen <danders@get2net.dk>
 * SPDX-FileCopyrightText: 2012 Brijesh Patel <brijesh3105@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KCHART_CHART_TEXTSHAPE_COMMAND
#define KCHART_CHART_TEXTSHAPE_COMMAND

// Qt
#include <kundo2command.h>

// KoChart
#include "kochart_global.h"

class KoShape;

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
class ChartTextShapeCommand : public KUndo2Command
{
public:
    /**
     * Constructor.
     */
    ChartTextShapeCommand(KoShape *textShape, ChartShape *chart, bool isVisible, KUndo2Command *parent = nullptr);

    /**
     * Destructor.
     */
    virtual ~ChartTextShapeCommand();

    /**
     * Executes the actual operation.
     */
    void redo() override;

    /**
     * Executes the actual operation in reverse order.
     */
    void undo() override;

    void setRotation(int angle);

private:
    void init();

private:
    KoShape *m_textShape;
    ChartShape *m_chart;
    bool m_oldIsVisible;
    bool m_newIsVisible;
    int m_oldRotation;
    int m_newRotation;
};

} // namespace KoChart

#endif // KCHART_CHART_TEXTSHAPE_COMMAND
