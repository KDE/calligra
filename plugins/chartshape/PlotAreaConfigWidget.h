/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2008 Johannes Simon <johannes.simon@gmail.com>
   SPDX-FileCopyrightText: 2008 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOCHART_PLOTAREACONFIGWIDGET
#define KOCHART_PLOTAREACONFIGWIDGET

#include "ChartShape.h"
#include "ConfigWidgetBase.h"

class KoShape;
class QAction;

namespace KChart
{
class Position;
class CartesianAxis;
}

namespace KoChart
{
class ChartShape;
class AxesConfigWidget;
class DataSetConfigWidget;
class PieConfigWidget;
class RingConfigWidget;
class StockConfigWidget;
class RadarDataSetConfigWidget;

/**
 * Chart type configuration widget.
 */
class PlotAreaConfigWidget : public ConfigWidgetBase
{
    Q_OBJECT

public:
    PlotAreaConfigWidget();
    ~PlotAreaConfigWidget();

    void deactivate() override;
    void open(KoShape *shape) override;
    QAction *createAction();

    /// reimplemented
    bool showOnShapeCreate() override
    {
        return true;
    }

    /// Delete all open dialogs.
    /// This is called when e.g. the tool is deactivated.
    void deleteSubDialogs(ChartType type = LastChartType) override;

    AxesConfigWidget *cartesianAxesConfigWidget() const;
    DataSetConfigWidget *cartesianDataSetConfigWidget() const;
    PieConfigWidget *pieConfigWidget() const;
    RingConfigWidget *ringConfigWidget() const;
    StockConfigWidget *stockConfigWidget() const;
    AxesConfigWidget *stockAxesConfigWidget() const;
    RadarDataSetConfigWidget *radarDataSetConfigWidget() const;

public Q_SLOTS:
    void chartTypeSelected(QAction *action);
    void setThreeDMode(bool threeD);
    void updateData() override;

    void slotShowTableEditor();
    void slotShowCellRegionDialog();

private Q_SLOTS:
    void ui_dataSetSelectionChanged_CellRegionDialog(int);
    void ui_dataSetXDataRegionChanged();
    void ui_dataSetYDataRegionChanged();
    void ui_dataSetLabelDataRegionChanged();
    void ui_dataSetCategoryDataRegionChanged();
    void ui_dataSetCustomDataRegionChanged();

    void ui_chartOrientationChanged(int value);

Q_SIGNALS:
    void chartTypeChanged(KoChart::ChartType type, KoChart::ChartSubtype subType);
    void chartSubTypeChanged(KoChart::ChartSubtype subType);

    void threeDModeToggled(bool threeD);
    void chartOrientationChanged(Qt::Orientation);

    void dataSetXDataRegionChanged(KoChart::DataSet *dataSet, const KoChart::CellRegion &region);
    void dataSetYDataRegionChanged(KoChart::DataSet *dataSet, const KoChart::CellRegion &region);
    void dataSetCustomDataRegionChanged(KoChart::DataSet *dataSet, const KoChart::CellRegion &region);
    void dataSetCategoryDataRegionChanged(KoChart::DataSet *dataSet, const KoChart::CellRegion &region);
    void dataSetLabelDataRegionChanged(KoChart::DataSet *dataSet, const KoChart::CellRegion &region);

private:
    void setupWidgets();
    void setupDialogs();
    void createActions();

    class Private;
    Private *const d;
};

} // namespace KoChart

#endif // KOCHART_PLOTAREACONFIGWIDGET
