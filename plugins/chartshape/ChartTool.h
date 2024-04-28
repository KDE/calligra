/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2007 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
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
    void activate(ToolActivation toolActivation, const QSet<KoShape *> &shapes) override;
    /// reimplemented from superclass
    void deactivate() override;
    /// reimplemented from superclass
    QList<QPointer<QWidget>> createOptionWidgets() override;

private Q_SLOTS:
    void setChartType(KoChart::ChartType type, KoChart::ChartSubtype subtype);
    void setChartSubType(KoChart::ChartSubtype subtype);
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
    void setDataSetXDataRegion(KoChart::DataSet *dataSet, const KoChart::CellRegion &region);
    void setDataSetYDataRegion(KoChart::DataSet *dataSet, const KoChart::CellRegion &region);
    void setDataSetCustomDataRegion(KoChart::DataSet *dataSet, const KoChart::CellRegion &region);
    void setDataSetLabelDataRegion(KoChart::DataSet *dataSet, const KoChart::CellRegion &region);
    void setDataSetCategoryDataRegion(KoChart::DataSet *dataSet, const KoChart::CellRegion &region);

    void setDataSetChartType(KoChart::DataSet *dataSet, KoChart::ChartType type, KoChart::ChartSubtype subType);

    void setDataSetShowCategory(KoChart::DataSet *dataSet, bool b, int section = -1);
    void setDataSetShowNumber(KoChart::DataSet *dataSet, bool b, int section = -1);
    void setDataSetShowPercent(KoChart::DataSet *dataSet, bool b, int section = -1);
    void setDataSetShowSymbol(KoChart::DataSet *dataSet, bool b, int section = -1);

    void setDataSetPen(KoChart::DataSet *dataSet, const QColor &color, int section = -1);
    void setDataSetBrush(KoChart::DataSet *dataSet, const QColor &color, int section = -1);
    void setDataSetMarker(KoChart::DataSet *dataSet, KoChart::OdfSymbolType type, KoChart::OdfMarkerStyle style);
    void setDataSetAxis(KoChart::DataSet *dataSet, KoChart::Axis *axis);

    // Plot Area
    void setGapBetweenBars(KoChart::Axis *axis, int percent);
    void setGapBetweenSets(KoChart::Axis *axis, int percent);
    void setPieExplodeFactor(KoChart::DataSet *dataSet, int section, int percent);

    // Axes
    void addAxis(KoChart::AxisDimension, const QString &title = QString());
    void removeAxis(KoChart::Axis *axis);
    void setShowAxis(KoChart::Axis *axis, bool show);
    void setAxisPosition(KoChart::Axis *axis, const QString &pos);
    void setAxisLabelsPosition(KoChart::Axis *axis, const QString &pos);
    void setAxisShowTitle(KoChart::Axis *axis, bool show);
    void setAxisShowLabels(KoChart::Axis *axis, bool b);
    void setAxisShowMajorGridLines(KoChart::Axis *axis, bool b = true);
    void setAxisShowMinorGridLines(KoChart::Axis *axis, bool b = true);
    void setAxisUseLogarithmicScaling(KoChart::Axis *axis, bool b = true);
    void setAxisStepWidth(KoChart::Axis *axis, qreal width);
    void setAxisSubStepWidth(KoChart::Axis *axis, qreal width);
    void setAxisUseAutomaticStepWidth(KoChart::Axis *axis, bool automatic);
    void setAxisUseAutomaticSubStepWidth(KoChart::Axis *axis, bool automatic);
    void setAxisLabelsFont(KoChart::Axis *axis, const QFont &font);

    // Legend
    void setShowLegend(bool show);
    void setLegendTitle(const QString &title);
    void setLegendFont(const QFont &font);
    void setLegendFontSize(int size);
    void setLegendOrientation(Qt::Orientation);
    void setLegendPosition(KoChart::Position);
    void setLegendAlignment(Qt::Alignment);

    void shapeSelectionChanged();

private:
    class Private;
    Private *const d;
};

} // namespace KoChart

#endif // KCHART_CHARTTOOL_H
