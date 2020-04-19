/* This file is part of the KDE project
 *
 * Copyright (C) 2007      Inge Wallin  <inge@lysator.liu.se>
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


#ifndef KCHART_CHARTTOOL_H
#define KCHART_CHARTTOOL_H

// Calligra
#include <KoToolBase.h>

// KoChart
#include "ChartShape.h"

class QAction;


namespace KoChart
{


/**
 * This is the tool for the chart shape, which is a flake-based plugin.
 */

class ChartTool : public KoToolBase
{
    Q_OBJECT
public:
    explicit ChartTool(KoCanvasBase *canvas);
    ~ChartTool();

    /// reimplemented from superclass
    void paint(QPainter &painter, const KoViewConverter &converter) override;

    /// reimplemented from superclass
    void mousePressEvent(KoPointerEvent *event) override;
    /// reimplemented from superclass
    void mouseMoveEvent(KoPointerEvent *event) override;
    /// reimplemented from superclass
    void mouseReleaseEvent(KoPointerEvent *event) override;
    /// reimplemented from superclass
    void activate(ToolActivation toolActivation, const QSet<KoShape*> &shapes) override;
    /// reimplemented from superclass
    void deactivate() override;
    /// reimplemented from superclass
    QList<QPointer<QWidget> > createOptionWidgets() override;

private Q_SLOTS:
    void setChartType(ChartType type, ChartSubtype subtype);
    void setChartSubType(ChartSubtype subtype);
    void setThreeDMode(bool threeD);
    void setDataDirection(Qt::Orientation);
    void setChartOrientation(Qt::Orientation);
    void setShowTitle(bool show);
    void setTitlePositioning(int index);
    void setTitleResize(int index);
    void setShowSubTitle(bool show);
    void setSubTitlePositioning(int index);
    void setSubTitleResize(int index);
    void setShowFooter(bool show);
    void setFooterPositioning(int index);
    void setFooterResize(int index);

    // Datasets
    void setDataSetXDataRegion(DataSet *dataSet, const CellRegion &region);
    void setDataSetYDataRegion(DataSet *dataSet, const CellRegion &region);
    void setDataSetCustomDataRegion(DataSet *dataSet, const CellRegion &region);
    void setDataSetLabelDataRegion(DataSet *dataSet, const CellRegion &region);
    void setDataSetCategoryDataRegion(DataSet *dataSet, const CellRegion &region);

    void setDataSetChartType(DataSet *dataSet, ChartType type, ChartSubtype subType);

    void setDataSetShowCategory(DataSet *dataSet, bool b, int section = -1);
    void setDataSetShowNumber(DataSet *dataSet, bool b, int section = -1);
    void setDataSetShowPercent(DataSet *dataSet, bool b, int section = -1);
    void setDataSetShowSymbol(DataSet *dataSet, bool b, int section = -1);

    void setDataSetPen(DataSet *dataSet, const QColor& color, int section = -1);
    void setDataSetBrush(DataSet *dataSet, const QColor& color, int section = -1);
    void setDataSetMarker(DataSet *dataSet, OdfSymbolType type, OdfMarkerStyle style);
    void setDataSetAxis(DataSet *dataSet, Axis *axis);

    // Plot Area
    void setGapBetweenBars(Axis *axis, int percent);
    void setGapBetweenSets(Axis *axis, int percent);
    void setPieExplodeFactor(DataSet *dataSet, int section, int percent);

    // Axes
    void addAxis(AxisDimension, const QString& title = QString());
    void removeAxis(Axis *axis);
    void setShowAxis(Axis *axis, bool show);
    void setAxisPosition(Axis *axis, const QString &pos);
    void setAxisLabelsPosition(Axis *axis, const QString &pos);
    void setAxisShowTitle(Axis *axis, bool show);
    void setAxisShowLabels(Axis *axis, bool b);
    void setAxisShowMajorGridLines(Axis *axis, bool b = true);
    void setAxisShowMinorGridLines(Axis *axis, bool b = true);
    void setAxisUseLogarithmicScaling(Axis *axis, bool b = true);
    void setAxisStepWidth(Axis *axis, qreal width);
    void setAxisSubStepWidth(Axis *axis, qreal width);
    void setAxisUseAutomaticStepWidth(Axis *axis, bool automatic);
    void setAxisUseAutomaticSubStepWidth(Axis *axis, bool automatic);
    void setAxisLabelsFont(Axis *axis, const QFont& font);

    // Legend
    void setShowLegend(bool show);
    void setLegendTitle(const QString& title);
    void setLegendFont(const QFont& font);
    void setLegendFontSize(int size);
    void setLegendOrientation(Qt::Orientation);
    void setLegendPosition(Position);
    void setLegendAlignment(Qt::Alignment);

    void shapeSelectionChanged();

private:
    class Private;
    Private * const d;
};

} // namespace KoChart


#endif // KCHART_CHARTTOOL_H
