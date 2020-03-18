/* This file is part of the KDE project

   Copyright 2008 Johannes Simon <johannes.simon@gmail.com>
   Copyright 2008 Inge Wallin    <inge@lysator.liu.se>

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
 * Boston, MA 02110-1301, USA.
*/


#ifndef KOCHART_PLOTAREACONFIGWIDGET
#define KOCHART_PLOTAREACONFIGWIDGET


#include "ConfigWidgetBase.h"
#include "ChartShape.h"


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
    void open(KoShape* shape) override;
    QAction * createAction();

    /// reimplemented
    bool showOnShapeCreate() override { return true; }

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
    void chartTypeChanged(ChartType type, ChartSubtype subType);
    void chartSubTypeChanged(ChartSubtype subType);

    void threeDModeToggled(bool threeD);
    void chartOrientationChanged(Qt::Orientation);


    void dataSetXDataRegionChanged(DataSet *dataSet, const CellRegion &region);
    void dataSetYDataRegionChanged(DataSet *dataSet, const CellRegion &region);
    void dataSetCustomDataRegionChanged(DataSet *dataSet, const CellRegion &region);
    void dataSetCategoryDataRegionChanged(DataSet *dataSet, const CellRegion &region);
    void dataSetLabelDataRegionChanged(DataSet *dataSet, const CellRegion &region);

private:
    void setupWidgets();
    void setupDialogs();
    void createActions();

    class Private;
    Private * const d;
};

}  // namespace KoChart


#endif // KOCHART_PLOTAREACONFIGWIDGET
