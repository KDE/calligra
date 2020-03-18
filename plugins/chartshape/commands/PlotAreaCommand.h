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
