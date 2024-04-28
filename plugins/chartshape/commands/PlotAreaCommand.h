/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2018 Dag Andersen <danders@get2net.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KCHART_PLOTAREACOMMAND_H
#define KCHART_PLOTAREACOMMAND_H

#include <kundo2command.h>

namespace KoChart
{

class ChartShape;
class PlotArea;

class PlotAreaCommand : public KUndo2Command
{
public:
    explicit PlotAreaCommand(PlotArea *plotArea);
    virtual ~PlotAreaCommand();

    void redo() override;
    void undo() override;

    void setOrientation(Qt::Orientation orientation);

private:
    PlotArea *m_plotArea;
    ChartShape *m_chart;

    Qt::Orientation m_oldOrientation;
    Qt::Orientation m_newOrientation;
};

} // namespace KoChart

#endif // KCHART_PLOTAREACOMMAND_H
