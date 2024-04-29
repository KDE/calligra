/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2008 Johannes Simon <johannes.simon@gmail.com>
   SPDX-FileCopyrightText: 2008 Inge Wallin <inge@lysator.liu.se>
   SPDX-FileCopyrightText: 2018 Dag Andersen <danders@get2net.dk>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOCHART_DATACONFIGWIDGET
#define KOCHART_DATACONFIGWIDGET

#include "ChartShape.h"
#include "ConfigSubWidgetBase.h"

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

/**
 * Chart type configuration widget.
 */
class DataSetConfigWidget : public ConfigSubWidgetBase
{
    Q_OBJECT

public:
    DataSetConfigWidget(QWidget *parent = nullptr);
    DataSetConfigWidget(QList<ChartType> types, QWidget *parent = nullptr);
    ~DataSetConfigWidget();

    void open(ChartShape *shape) override;
    QAction *createAction();

    /// reimplemented
    virtual bool showOnShapeCreate()
    {
        return true;
    }

    /// Delete all open dialogs.
    /// This is called when e.g. the tool is deactivated.
    void deleteSubDialogs(KoChart::ChartType type = LastChartType);

    void updateMarkers();

public Q_SLOTS:
    void updateData(KoChart::ChartType type, KoChart::ChartSubtype subtype) override;

    void slotShowFormatErrorBarDialog();

    void dataSetChartTypeSelected(QAction *action);
    void datasetMarkerSelected(QAction *action);
    void datasetBrushSelected(const QColor &color);
    void datasetPenSelected(const QColor &color);
    void ui_datasetShowCategoryChanged(bool b);
    void ui_datasetShowErrorBarChanged(bool b);
    void ui_dataSetShowNumberChanged(bool b);
    void ui_datasetShowPercentChanged(bool b);
    void ui_datasetShowSymbolChanged(bool b);
    void ui_dataSetSelectionChanged(int index);
    void ui_dataSetAxisSelectionChanged(int index);
    void ui_dataSetHasChartTypeChanged(bool b);
    void ui_dataSetErrorBarTypeChanged();

Q_SIGNALS:
    void dataSetChartTypeChanged(KoChart::DataSet *dataSet, KoChart::ChartType type, KoChart::ChartSubtype subType);

    void showVerticalLinesChanged(bool b);
    void showHorizontalLinesChanged(bool b);

    void datasetPenChanged(KoChart::DataSet *dataSet, const QColor &color, int section);
    void datasetBrushChanged(KoChart::DataSet *dataSet, const QColor &color, int section);
    void dataSetMarkerChanged(KoChart::DataSet *dataSet, KoChart::OdfSymbolType type, KoChart::OdfMarkerStyle style);
    void datasetShowCategoryChanged(KoChart::DataSet *dataSet, bool b, int section);
    void dataSetShowNumberChanged(KoChart::DataSet *dataSet, bool b, int section);
    void datasetShowPercentChanged(KoChart::DataSet *dataSet, bool b, int section);
    void datasetShowSymbolChanged(KoChart::DataSet *dataSet, bool b, int section);
    void dataSetAxisChanged(KoChart::DataSet *dataSet, KoChart::Axis *axis);

    void axisAdded(KoChart::AxisDimension, const QString &title);

private:
    void setupDialogs();
    void createActions();

    class Private;
    Private *const d;
};

} // namespace KoChart

#endif // KOCHART_DATASETCONFIGWIDGET
