/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2012 Brijesh Patel <brijesh3105@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KCHART_LEGEND_COMMAND
#define KCHART_LEGEND_COMMAND

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

class Legend;
class ChartShape;

class LegendCommand : public KUndo2Command
{
public:
    /**
     * Constructor.
     */
    explicit LegendCommand(Legend *legend);

    /**
     * Destructor.
     */
    virtual ~LegendCommand();

    /**
     * Executes the actual operation.
     */
    void redo() override;

    /**
     * Executes the actual operation in reverse order.
     */
    void undo() override;

    void setLegendTitle(const QString &title);
    void setLegendFont(const QFont &font);
    void setLegendFontSize(int size);
    void setLegendExpansion(LegendExpansion expansion);
    /*void setLegendAlignment(Qt::Alignment);
    void setLegendFixedPosition(Position);
    void setLegendBackgroundColor(QColor &color);
    void setLegendFrameColor(QColor &color);*/

private:
    Legend *m_legend;
    QString m_oldTitle;
    QString m_newTitle;
    QFont m_oldFont;
    QFont m_newFont;
    int m_oldFontSize;
    int m_newFontSize;
    LegendExpansion m_oldExpansion;
    LegendExpansion m_newExpansion;
    ChartShape *m_chart;
};

} // namespace KoChart

#endif // KCHART_LEGEND_COMMAND
