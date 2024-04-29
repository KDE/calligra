/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2017 Dag Andersen <danders@get2net.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
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
    AddRemoveAxisCommand(Axis *axis, ChartShape *chart, bool add, KoShapeManager *shapeManager, KUndo2Command *parent = nullptr);

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
