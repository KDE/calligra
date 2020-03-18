/* This file is part of the KDE project
 * Copyright 2017 Dag Andersen <danders@get2net.dk>
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KCHART_CHART_ADDREMOVEAXIS_COMMAND
#define KCHART_CHART_ADDREMOVEAXIS_COMMAND

// Qt
#include <kundo2command.h>

// KoChart
#include "kochart_global.h"


class KoShape;
class KoShapeManager;


namespace KoChart
{

class ChartShape;
class Axis;

/**
 * Chart type replacement command.
 */
class AddRemoveAxisCommand : public KUndo2Command
{
public:
    /**
     * Constructor.
     */
    AddRemoveAxisCommand(Axis *axis, ChartShape *chart, bool add, KoShapeManager *shapeManager, KUndo2Command *parent = 0);

    /**
     * Destructor.
     */
    virtual ~AddRemoveAxisCommand();

    /**
     * Executes the actual operation.
     */
    void redo() override;

    /**
     * Executes the actual operation in reverse order.
     */
    void undo() override;

private:
    void initAdd();
    void initRemove();

private:
    Axis *m_axis;
    ChartShape *m_chart;
    bool m_add;
    bool mine;
    KoShapeManager *m_shapeManager;
};

} // namespace KoChart

#endif // KCHART_CHART_ADDREMOVEAXIS_COMMAND
