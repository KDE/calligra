/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2012 Brijesh Patel <brijesh3105@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KCHART_DATASET_COMMAND
#define KCHART_DATASET_COMMAND

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

class DataSet;
class ChartShape;
class Axis;

class DatasetCommand : public KUndo2Command
{
public:
    /**
     * Constructor.
     */
    DatasetCommand(DataSet *dataSet, ChartShape *chart, int section = -1, KUndo2Command *parent = nullptr);

    /**
     * Destructor.
     */
    virtual ~DatasetCommand();

    /**
     * Executes the actual operation.
     */
    void redo() override;

    /**
     * Executes the actual operation in reverse order.
     */
    void undo() override;

    void setDataSetChartType(ChartType type, ChartSubtype subtype);

    void setDataSetShowCategory(bool show);
    void setDataSetShowNumber(bool show);
    void setDataSetShowPercent(bool show);
    void setDataSetShowSymbol(bool show);

    void setDataSetPen(const QColor &color);
    void setDataSetBrush(const QColor &color);
    void setDataSetMarker(OdfSymbolType type, OdfMarkerStyle style);
    void setDataSetAxis(Axis *axis);

private:
    DataSet *m_dataSet;
    ChartShape *m_chart;
    int m_section;

    ChartType m_oldType;
    ChartType m_newType;
    ChartSubtype m_oldSubtype;
    ChartSubtype m_newSubtype;

    bool m_oldShowCategory;
    bool m_newShowCategory;
    bool m_oldShowNumber;
    bool m_newShowNumber;
    bool m_oldShowPercent;
    bool m_newShowPercent;
    bool m_oldShowSymbol;
    bool m_newShowSymbol;

    QColor m_oldPenColor;
    QColor m_newPenColor;
    QColor m_oldBrushColor;
    QColor m_newBrushColor;
    OdfSymbolType m_oldOdfSymbolType;
    OdfSymbolType m_newOdfSymbolType;
    OdfMarkerStyle m_oldMarkerStyle;
    OdfMarkerStyle m_newMarkerStyle;

    Axis *m_oldAxis;
    Axis *m_newAxis;
};

} // namespace KoChart

#endif // KCHART_DATASET_COMMAND
