/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2008 Johannes Simon <johannes.simon@gmail.com>
   SPDX-FileCopyrightText: 2008 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOCHART_AXESCONFIGWIDGET
#define KOCHART_AXESCONFIGWIDGET

#include "ChartShape.h"
#include "ConfigSubWidgetBase.h"

#include <QObject>

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
class AxesConfigWidget : public ConfigSubWidgetBase
{
    Q_OBJECT

public:
    AxesConfigWidget(QWidget *parent = nullptr);
    AxesConfigWidget(QList<ChartType> types, QWidget *parent = nullptr);
    ~AxesConfigWidget();

    void open(ChartShape *shape) override;

    /// Delete all open dialogs.
    /// This is called when e.g. the tool is deactivated.
    void deleteSubDialogs(ChartType type = LastChartType);

public Q_SLOTS:
    void updateData(KoChart::ChartType type, KoChart::ChartSubtype subtype) override;

    void ui_axisSelectionChanged(int index);
    void ui_axisShowTitleChanged(bool b);
    void ui_axisShowChanged(bool b);
    void ui_axisPositionChanged(int index);
    void ui_axisLabelsPositionChanged(int index);
    void ui_axisShowLabelsChanged(bool value);
    void ui_axisShowMajorGridLinesChanged(bool b);
    void ui_axisShowMinorGridLinesChanged(bool b);
    void ui_axisUseLogarithmicScalingChanged(bool b);
    void ui_axisStepWidthChanged(double width);
    void ui_axisUseAutomaticStepWidthChanged(bool b);
    void ui_axisSubStepWidthChanged(double width);
    void ui_axisUseAutomaticSubStepWidthChanged(bool b);
    void ui_axisScalingButtonClicked();
    void ui_axisEditFontButtonClicked();
    void ui_axisLabelsFontChanged();

Q_SIGNALS:
    void axisAdded(KoChart::AxisDimension, const QString &title);
    void axisRemoved(KoChart::Axis *axis);
    void axisShowTitleChanged(KoChart::Axis *axis, bool b);
    void axisShowChanged(KoChart::Axis *axis, bool b);
    void axisPositionChanged(KoChart::Axis *axis, const QString &pos);
    void axisLabelsPositionChanged(KoChart::Axis *axis, const QString &pos);
    void axisShowLabelsChanged(KoChart::Axis *axis, bool value);
    void axisShowMajorGridLinesChanged(KoChart::Axis *axis, bool b);
    void axisShowMinorGridLinesChanged(KoChart::Axis *axis, bool b);
    void axisUseLogarithmicScalingChanged(KoChart::Axis *axis, bool b);
    void axisStepWidthChanged(KoChart::Axis *axis, qreal width);
    void axisSubStepWidthChanged(KoChart::Axis *axis, qreal width);
    void axisUseAutomaticStepWidthChanged(KoChart::Axis *axis, bool automatic);
    void axisUseAutomaticSubStepWidthChanged(KoChart::Axis *axis, bool automatic);
    void axisLabelsFontChanged(KoChart::Axis *axis, const QFont &font);

    void gapBetweenBarsChanged(KoChart::Axis *axis, int percent);
    void gapBetweenSetsChanged(KoChart::Axis *axis, int percent);

private Q_SLOTS:
    void slotGapBetweenBars();
    void slotGapBetweenSets();

private:
    Axis *axis(int index) const;
    void setupDialogs();
    void createActions();

    class Private;
    Private *const d;
};

} // namespace KoChart

#endif // KOCHART_AXESCONFIGWIDGET
