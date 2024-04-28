/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2018 Dag Andersen <danders@get2net.dk>

   SPDX-License-Identifier: LGPL-2.0-or-later
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
