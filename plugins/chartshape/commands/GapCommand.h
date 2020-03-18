/* This file is part of the KDE project
   Copyright 2018 Dag Andersen <danders@get2net.dk>

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

#ifndef KCHART_GAPCOMMAND_H
#define KCHART_GAPCOMMAND_H

// Qt
#include <kundo2command.h>

// KoChart
#include "kochart_global.h"


namespace KoChart
{

class Axis;
class ChartShape;

class GapCommand : public KUndo2Command
{
public:
    /**
     * Constructor.
     */
    GapCommand(Axis *axis, ChartShape *chart);

    /**
     * Destructor.
     */
    virtual ~GapCommand();

    /**
     * Executes the actual operation.
     */
    void redo() override;

    /**
     * Executes the actual operation in reverse order.
     */
    void undo() override;

    void setGapBetweenBars(int percent);
    void setGapBetweenSets(int percent);

private:
    Axis *m_axis;
    ChartShape *m_chart;

    int m_oldGapBetweenBars;
    int m_newGapBetweenBars;
    int m_oldGapBetweenSets;
    int m_newGapBetweenSets;
};

} // namespace KoChart

#endif // KCHART_GAPCOMMAND_H
