/* This file is part of the KDE project
   Copyright 2012 Brijesh Patel <brijesh3105@gmail.com>

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

#ifndef KCHART_AXIS_COMMAND
#define KCHART_AXIS_COMMAND

// Qt
#include <kundo2command.h>

// KoChart
#include "kochart_global.h"


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

class Axis;
class ChartShape;

class AxisCommand : public KUndo2Command
{
public:
    /**
     * Constructor.
     */
    AxisCommand(Axis* axis, ChartShape* chart);

    /**
     * Destructor.
     */
    virtual ~AxisCommand();

    /**
     * Executes the actual operation.
     */
    void redo() override;

    /**
     * Executes the actual operation in reverse order.
     */
    void undo() override;

    void setAxisShowTitle(bool show);
    void setShowAxis(bool show);
    void setAxisShowLabels(bool show);
    void setAxisShowMajorGridLines(bool show);
    void setAxisShowMinorGridLines(bool show);
    void setAxisUseLogarithmicScaling(bool b);
    void setAxisStepWidth(qreal width);
    void setAxisSubStepWidth(qreal width);
    void setAxisUseAutomaticStepWidth(bool automatic);
    void setAxisUseAutomaticSubStepWidth(bool automatic);
    void setAxisLabelsFont(const QFont& font);
    void setAxisPosition(const QString &pos);
    void setAxisLabelsPosition(const QString &pos);

private:
    ChartShape *m_chart;
    Axis *m_axis;
    bool m_oldShowAxis;
    bool m_newShowAxis;
    bool m_oldShowLabels;
    bool m_newShowLabels;
    bool m_oldShowMajorGridLines;
    bool m_oldShowMinorGridLines;
    bool m_newShowMajorGridLines;
    bool m_newShowMinorGridLines;
    bool m_oldUseLogarithmicScaling;
    bool m_newUseLogarithmicScaling;
    qreal m_oldStepWidth;
    qreal m_newStepWidth;
    qreal m_oldSubStepWidth;
    qreal m_newSubStepWidth;
    bool m_oldUseAutomaticStepWidth;
    bool m_newUseAutomaticStepWidth;
    bool m_oldUseAutomaticSubStepWidth;
    bool m_newUseAutomaticSubStepWidth;
    QFont m_oldLabelsFont;
    QFont m_newLabelsFont;
    QString m_oldPosition;
    QString m_newPosition;
    QString m_oldLabelsPosition;
    QString m_newLabelsPosition;
};

} // namespace KoChart

#endif // KCHART_AXIS_COMMAND
