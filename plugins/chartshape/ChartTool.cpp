/* This file is part of the KDE project
 *
 * Copyright (C) 2007, 2010  Inge Wallin <inge@lysator.liu.se>
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

// Own
#include "ChartTool.h"

// Qt
#include <QAction>
#include <QGridLayout>
#include <QToolButton>
#include <QCheckBox>
#include <QTabWidget>
#include <QPen>
#include <QBrush>
#include <QPainter>

// KF5
#include <klocalizedstring.h>

// Calligra
#include <KoCanvasBase.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoPointerEvent.h>
#include <KoTextShapeData.h>
#include <KoViewConverter.h>

// KChart
#include <KChartChart>
#include <KChartCartesianAxis>
#include <KChartGridAttributes>
#include <KChartAbstractCartesianDiagram>
#include <KChartCartesianCoordinatePlane>
#include <KChartPosition>

// KoChart
#include "Surface.h"
#include "PlotArea.h"
#include "ChartLayout.h"
#include "Axis.h"
#include "DataSet.h"
#include "Legend.h"
#include "ChartProxyModel.h"
#include "TitlesConfigWidget.h"
#include "LegendConfigWidget.h"
#include "PlotAreaConfigWidget.h"
#include "AxesConfigWidget.h"
#include "DataSetConfigWidget.h"
#include "KChartConvertions.h"
#include "commands/ChartTypeCommand.h"
#include "commands/LegendCommand.h"
#include "commands/AxisCommand.h"
#include "commands/DatasetCommand.h"
#include "commands/ChartTextShapeCommand.h"
#include "commands/AddRemoveAxisCommand.h"
#include "commands/GapCommand.h"
#include "ChartDebug.h"


using namespace KoChart;


class ChartTool::Private
{
public:
    Private();
    ~Private();

    ChartShape  *shape;
    QModelIndex  datasetSelection;
    QPen         datasetSelectionPen;
    QBrush       datasetSelectionBrush;

    void setDataSetShowLabel(DataSet *dataSet, bool *number, bool *percentage, bool *category, bool *symbol);
};

ChartTool::Private::Private()
    : shape(0)
{
}

ChartTool::Private::~Private()
{
}

ChartTool::ChartTool(KoCanvasBase *canvas)
    : KoToolBase(canvas),
      d(new Private())
{
    // Create QActions here.
#if 0
    QActionGroup *group = new QActionGroup(this);
    m_foo  = new QAction(koIcon("this-action"), i18n("Do something"), this);
    m_foo->setCheckable(true);
    group->addAction(m_foo);
    connect(m_foo, SIGNAL(toggled(bool)), this, SLOT(catchFoo(bool)));

    m_bar  = new QAction(koIcon("that-action"), i18n("Do something else"), this);
    m_bar->setCheckable(true);
    group->addAction(m_bar);
    connect(m_foo, SIGNAL(toggled(bool)), this, SLOT(catchBar(bool)));

#endif
    connect(canvas->shapeManager()->selection(), SIGNAL(selectionChanged()),
            this, SLOT(shapeSelectionChanged()));
}

ChartTool::~ChartTool()
{
    delete d;
}

void ChartTool::shapeSelectionChanged()
{
    // Get the chart shape that the tool is working on.
    // Let d->shape point to it.
    d->shape = 0; // to be sure we don't deal with an old value if nothing is found
    KoSelection  *selection = canvas()->shapeManager()->selection();
    foreach (KoShape *shape, selection->selectedShapes()) {
        d->shape = dynamic_cast<ChartShape*>(shape);
        if (!d->shape) {
            d->shape = dynamic_cast<ChartShape*>(shape->parent());
        }
        if (d->shape) {
            foreach (QWidget *w, optionWidgets()) {
                ConfigWidgetBase *widget = dynamic_cast<ConfigWidgetBase*>(w);
                Q_ASSERT(widget);
                if (widget) {
                    widget->open(d->shape);
                }
            }
            break;
        }
    }
    // If we couldn't determine a chart shape, then there is nothing to do.
    if (!d->shape) { // none found
        emit done();
        return;
    }
}


void ChartTool::paint(QPainter &painter, const KoViewConverter &converter)
{
    Q_UNUSED(painter);
    Q_UNUSED(converter);
}

void ChartTool::mousePressEvent(KoPointerEvent *event)
{
#if 1  // disabled
    Q_UNUSED(event);
    return;
#else
    // Select dataset
    if (   !d->shape || !d->shape->kdChart() || ! d->shape->kdChart()->coordinatePlane()
        || !d->shape->kdChart()->coordinatePlane()->diagram())
        return;
    QPointF point = event->point - d->shape->position();
    QModelIndex selection = d->shape->kdChart()->coordinatePlane()->diagram()->indexAt(point.toPoint());
    // Note: the dataset will always stay column() due to the transformations being
    // done internally by the ChartProxyModel
    int dataset = selection.column();

    if (d->datasetSelection.isValid()) {
        d->shape->kdChart()->coordinatePlane()->diagram()->setPen(d->datasetSelection.column(), d->datasetSelectionPen);
        //d->shape->kdChart()->coordinatePlane()->diagram()->setBrush(d->datasetSelection, d->datasetSelectionBrush);
    }
    if (selection.isValid()) {
        d->datasetSelection = selection;

        QPen pen(Qt::DotLine);
        pen.setColor(Qt::darkGray);
        pen.setWidth(1);

        d->datasetSelectionBrush = d->shape->kdChart()->coordinatePlane()->diagram()->brush(selection);
        d->datasetSelectionPen   = d->shape->kdChart()->coordinatePlane()->diagram()->pen(dataset);

        d->shape->kdChart()->coordinatePlane()->diagram()->setPen(dataset, pen);
        //d->shape->kdChart()->coordinatePlane()->diagram()->setBrush(selection, QBrush(Qt::lightGray));
    }
    ((ChartConfigWidget*)optionWidget())->selectDataset(dataset);

    d->shape->update();
#endif
}

void ChartTool::mouseMoveEvent(KoPointerEvent *event)
{
    event->ignore();
}

void ChartTool::mouseReleaseEvent(KoPointerEvent *event)
{
    event->ignore();
}


void ChartTool::activate(ToolActivation, const QSet<KoShape*> &shapes)
{
    Q_UNUSED(shapes);

    useCursor(Qt::ArrowCursor);

    // cause on ChartTool::deactivate we set d->shape to NULL it is needed
    // to call shapeSelectionChanged() even if the selection did not change
    // to be sure d->shape is proper set again.
    shapeSelectionChanged();
}

void ChartTool::deactivate()
{
    d->shape = 0;

    // Tell the config widget to delete all open dialogs.
    //
    // The reason why we want to do that explicitly here is because
    // they are connected to the models, which may disappear when the
    // chart shape is destructed.
    foreach (QWidget *w, optionWidgets()) {
        ConfigWidgetBase *configWidget = dynamic_cast<ConfigWidgetBase*>(w);
        if (configWidget)
            configWidget->deleteSubDialogs();
    }
}

QList<QPointer<QWidget> > ChartTool::createOptionWidgets()
{
    QList<QPointer<QWidget> > widgets;

    PlotAreaConfigWidget  *plotarea = new PlotAreaConfigWidget();
    plotarea->setWindowTitle(i18n("Chart"));
    widgets.append(plotarea);
    connect(plotarea, SIGNAL(chartTypeChanged(ChartType,ChartSubtype)),
            this,   SLOT(setChartType(ChartType,ChartSubtype)));
    connect(plotarea, SIGNAL(chartSubTypeChanged(ChartSubtype)),
            this,   SLOT(setChartSubType(ChartSubtype)));
    connect(plotarea, SIGNAL(threeDModeToggled(bool)),
            this,   SLOT(setThreeDMode(bool)));

    // data set edit dialog
    connect(plotarea, SIGNAL(dataSetXDataRegionChanged(DataSet*,CellRegion)),
            this,   SLOT(setDataSetXDataRegion(DataSet*,CellRegion)));
    connect(plotarea, SIGNAL(dataSetYDataRegionChanged(DataSet*,CellRegion)),
            this,   SLOT(setDataSetYDataRegion(DataSet*,CellRegion)));
    connect(plotarea, SIGNAL(dataSetCustomDataRegionChanged(DataSet*,CellRegion)),
            this,   SLOT(setDataSetCustomDataRegion(DataSet*,CellRegion)));
    connect(plotarea, SIGNAL(dataSetLabelDataRegionChanged(DataSet*,CellRegion)),
            this,   SLOT(setDataSetLabelDataRegion(DataSet*,CellRegion)));
    connect(plotarea, SIGNAL(dataSetCategoryDataRegionChanged(DataSet*,CellRegion)),
            this,   SLOT(setDataSetCategoryDataRegion(DataSet*,CellRegion)));

    connect(d->shape, SIGNAL(updateConfigWidget()), plotarea, SLOT(updateData()));

    TitlesConfigWidget *titles = new TitlesConfigWidget();
    titles->setWindowTitle(i18n("Titles"));
    widgets.append(titles);
    connect(titles, SIGNAL(showTitleChanged(bool)),
            this,   SLOT(setShowTitle(bool)));
    connect(titles, SIGNAL(showSubTitleChanged(bool)),
            this,   SLOT(setShowSubTitle(bool)));
    connect(titles, SIGNAL(showFooterChanged(bool)),
            this,   SLOT(setShowFooter(bool)));

    connect(d->shape, SIGNAL(updateConfigWidget()), titles, SLOT(updateData()));

    LegendConfigWidget *legend = new LegendConfigWidget();
    legend->setWindowTitle(i18n("Legend"));
    widgets.append(legend);
    connect(legend, SIGNAL(showLegendChanged(bool)),
            this,   SLOT(setShowLegend(bool)));
    connect(legend, SIGNAL(legendTitleChanged(QString)),
            this,   SLOT(setLegendTitle(QString)));
    connect(legend, SIGNAL(legendFontChanged(QFont)),
            this,   SLOT(setLegendFont(QFont)));
    connect(legend, SIGNAL(legendFontSizeChanged(int)),
            this,   SLOT(setLegendFontSize(int)));
    connect(legend, SIGNAL(legendOrientationChanged(Qt::Orientation)),
            this,   SLOT(setLegendOrientation(Qt::Orientation)));
    connect(legend, SIGNAL(legendPositionChanged(Position)),
            this,   SLOT(setLegendPosition(Position)));
    connect(legend, SIGNAL(legendAlignmentChanged(Qt::Alignment)),
            this,   SLOT(setLegendAlignment(Qt::Alignment)));

    connect(d->shape->legend(), SIGNAL(updateConfigWidget()), legend, SLOT(updateData()));

    AxesConfigWidget *axes = new AxesConfigWidget();
    axes->setWindowTitle(i18n("Axes"));
    widgets.append(axes);
    connect(axes, SIGNAL(axisAdded(AxisDimension,QString)),
            this,   SLOT(addAxis(AxisDimension,QString)));
    connect(axes, SIGNAL(axisRemoved(Axis*)),
            this,   SLOT(removeAxis(Axis*)));
    connect(axes, SIGNAL(axisShowTitleChanged(Axis*,bool)),
            this,   SLOT(setAxisShowTitle(Axis*,bool)));
    connect(axes, SIGNAL(axisShowChanged(Axis*,bool)),
            this,   SLOT(setShowAxis(Axis*,bool)));
    connect(axes, SIGNAL(axisShowMajorGridLinesChanged(Axis*,bool)),
            this,   SLOT(setAxisShowMajorGridLines(Axis*,bool)));
    connect(axes, SIGNAL(axisShowMinorGridLinesChanged(Axis*,bool)),
            this,   SLOT(setAxisShowMinorGridLines(Axis*,bool)));
    // scaling dialog
    connect(axes, SIGNAL(axisUseLogarithmicScalingChanged(Axis*,bool)),
            this,   SLOT(setAxisUseLogarithmicScaling(Axis*,bool)));
    connect(axes, SIGNAL(axisStepWidthChanged(Axis*,qreal)),
            this,   SLOT(setAxisStepWidth(Axis*,qreal)));
    connect(axes, SIGNAL(axisSubStepWidthChanged(Axis*,qreal)),
            this,   SLOT(setAxisSubStepWidth(Axis*,qreal)));
    connect(axes, SIGNAL(axisUseAutomaticStepWidthChanged(Axis*,bool)),
            this,   SLOT(setAxisUseAutomaticStepWidth(Axis*,bool)));
    connect(axes, SIGNAL(axisUseAutomaticSubStepWidthChanged(Axis*,bool)),
            this,   SLOT(setAxisUseAutomaticSubStepWidth(Axis*,bool)));
    // font dialog
    connect(axes, SIGNAL(axisLabelsFontChanged(Axis*,QFont)),
            this,   SLOT(setAxisLabelsFont(Axis*,QFont)));

    connect(axes, SIGNAL(gapBetweenBarsChanged(Axis*,int)),
            this,   SLOT(setGapBetweenBars(Axis*,int)));
    connect(axes, SIGNAL(gapBetweenSetsChanged(Axis*,int)),
            this,   SLOT(setGapBetweenSets(Axis*,int)));

    connect(d->shape, SIGNAL(updateConfigWidget()), axes, SLOT(updateData()));

    DataSetConfigWidget  *dataset = new DataSetConfigWidget();
    dataset->setWindowTitle(i18n("Data Sets"));
    widgets.append(dataset);

    connect(dataset, SIGNAL(dataSetChartTypeChanged(DataSet*,ChartType)),
            this,   SLOT(setDataSetChartType(DataSet*,ChartType)));
    connect(dataset, SIGNAL(dataSetChartSubTypeChanged(DataSet*,ChartSubtype)),
            this,   SLOT(setDataSetChartSubType(DataSet*,ChartSubtype)));
    connect(dataset, SIGNAL(datasetBrushChanged(DataSet*,QColor)),
            this, SLOT(setDataSetBrush(DataSet*,QColor)));
    connect(dataset, SIGNAL(dataSetMarkerChanged(DataSet*,OdfMarkerStyle)),
            this, SLOT(setDataSetMarker(DataSet*,OdfMarkerStyle)));
    connect(dataset, SIGNAL(datasetPenChanged(DataSet*,QColor)),
            this, SLOT(setDataSetPen(DataSet*,QColor)));
    connect(dataset, SIGNAL(datasetShowCategoryChanged(DataSet*,bool)),
            this, SLOT(setDataSetShowCategory(DataSet*,bool)));
    connect(dataset, SIGNAL(dataSetShowNumberChanged(DataSet*,bool)),
            this, SLOT(setDataSetShowNumber(DataSet*,bool)));
    connect(dataset, SIGNAL(datasetShowPercentChanged(DataSet*,bool)),
            this, SLOT(setDataSetShowPercent(DataSet*,bool)));
    connect(dataset, SIGNAL(datasetShowSymbolChanged(DataSet*,bool)),
            this, SLOT(setDataSetShowSymbol(DataSet*,bool)));
    connect(dataset, SIGNAL(dataSetAxisChanged(DataSet*,Axis*)),
            this, SLOT(setDataSetAxis(DataSet*,Axis*)));
    connect(dataset, SIGNAL(axisAdded(AxisDimension,QString)),
            this,   SLOT(addAxis(AxisDimension,QString)));

    connect(plotarea, SIGNAL(dataSetLabelDataRegionChanged(DataSet*, const CellRegion&)), dataset, SLOT(updateData()));

    connect(dataset, SIGNAL(pieExplodeFactorChanged(DataSet*,int)),
            this,   SLOT(setPieExplodeFactor(DataSet*,int)));

    connect(d->shape, SIGNAL(updateConfigWidget()), dataset, SLOT(updateData()));

    return widgets;
}


void ChartTool::setChartType(ChartType type, ChartSubtype subtype)
{
    Q_ASSERT(d->shape);
    if (!d->shape) {
        return;
    }
    ChartTypeCommand *command = new ChartTypeCommand(d->shape);
    if (command!=0) {
        command->setChartType(type, subtype);
        canvas()->addCommand(command);
    }
    foreach (QWidget *w, optionWidgets()) {
        ConfigWidgetBase *cw = dynamic_cast<ConfigWidgetBase*>(w);
        if (cw) {
            cw->updateData();
        }
    }
}


void ChartTool::setChartSubType(ChartSubtype subtype)
{
    Q_ASSERT(d->shape);
    if (!d->shape)
        return;

    d->shape->setChartSubType(subtype);
    d->shape->update();
}


void ChartTool::setDataSetXDataRegion(DataSet *dataSet, const CellRegion &region)
{
    if (!dataSet)
        return;

    dataSet->setXDataRegion(region);
}

void ChartTool::setDataSetYDataRegion(DataSet *dataSet, const CellRegion &region)
{
    if (!dataSet)
        return;

    dataSet->setYDataRegion(region);
}

void ChartTool::setDataSetCustomDataRegion(DataSet *dataSet, const CellRegion &region)
{
    if (!dataSet)
        return;

    dataSet->setCustomDataRegion(region);
}

void ChartTool::setDataSetLabelDataRegion(DataSet *dataSet, const CellRegion &region)
{
    if (!dataSet)
        return;

    dataSet->setLabelDataRegion(region);
}

void ChartTool::setDataSetCategoryDataRegion(DataSet *dataSet, const CellRegion &region)
{
    if (!dataSet)
        return;

    dataSet->setCategoryDataRegion(region);
}


void ChartTool::setDataSetChartType(DataSet *dataSet, ChartType type)
{
    Q_ASSERT(d->shape);
    Q_ASSERT(dataSet);
    if (dataSet)
        dataSet->setChartType(type);
    d->shape->update();
    d->shape->legend()->update();
}

void ChartTool::setDataSetChartSubType(DataSet *dataSet, ChartSubtype subType)
{
    Q_ASSERT(dataSet);
    if (dataSet)
        dataSet->setChartSubType(subType);
    d->shape->update();
}


void ChartTool::setDataSetBrush(DataSet *dataSet, const QColor& color)
{
    Q_ASSERT(d->shape);
    if (!dataSet)
        return;

    DatasetCommand *command = new DatasetCommand(dataSet, d->shape);
    command->setDataSetBrush(color);
    canvas()->addCommand(command);

    d->shape->update();
}
void ChartTool::setDataSetPen(DataSet *dataSet, const QColor& color)
{
    Q_ASSERT(d->shape);
    if (!dataSet)
        return;

    DatasetCommand *command = new DatasetCommand(dataSet, d->shape);
    command->setDataSetPen(color);
    canvas()->addCommand(command);

    d->shape->update();
}

void ChartTool::setDataSetMarker(DataSet *dataSet, OdfMarkerStyle style)
{
    Q_ASSERT(d->shape);
    if (!dataSet)
        return;

    DatasetCommand *command = new DatasetCommand(dataSet, d->shape);
    command->setDataSetMarker(style);
    canvas()->addCommand(command);

    d->shape->update();
}
void ChartTool::setDataSetAxis(DataSet *dataSet, Axis *axis)
{
    Q_ASSERT(d->shape);
    if (!dataSet || !axis)
        return;

    DatasetCommand *command = new DatasetCommand(dataSet, d->shape);
    command->setDataSetAxis(axis);
    canvas()->addCommand(command);
}

void ChartTool::Private::setDataSetShowLabel(DataSet *dataSet, bool *number, bool *percentage, bool *category, bool *symbol)
{
    Q_ASSERT(shape);
    if (!dataSet)
        return;

    DataSet::ValueLabelType type = dataSet->valueLabelType();
    if (number) type.number = *number;
    if (percentage) type.percentage = *percentage;
    if (category) type.category = *category;
    if (symbol) type.symbol = *symbol;
    dataSet->setValueLabelType(type);

    // its necessary to set this for all data value
    //TODO we need to allow to differ in the UI between the datasets vs
    //     the global setting and then allow to edit them separatly.
    for (int i = 0; i < dataSet->size(); ++i) {
        DataSet::ValueLabelType type = dataSet->valueLabelType(i);
        if (number) type.number = *number;
        if (percentage) type.percentage = *percentage;
        if (category) type.category = *category;
        if (symbol) type.symbol = *symbol;
        dataSet->setValueLabelType(type, i);
    }

    shape->update();
}

void ChartTool::setDataSetShowCategory(DataSet *dataSet, bool b)
{
    //d->setDataSetShowLabel(dataSet, 0, 0, &b, 0);
    Q_ASSERT(d->shape);
    if (!dataSet)
        return;

    DatasetCommand *command = new DatasetCommand(dataSet, d->shape);
    command->setDataSetShowCategory(b);
    canvas()->addCommand(command);

    d->shape->update();
}

void ChartTool::setDataSetShowNumber(DataSet *dataSet, bool b)
{
    //d->setDataSetShowLabel(dataSet, &b, 0, 0, 0);
    Q_ASSERT(d->shape);
    if (!dataSet)
        return;

    DatasetCommand *command = new DatasetCommand(dataSet, d->shape);
    command->setDataSetShowNumber(b);
    canvas()->addCommand(command);

    d->shape->update();
}

void ChartTool::setDataSetShowPercent(DataSet *dataSet, bool b)
{
    //d->setDataSetShowLabel(dataSet, 0, &b, 0, 0);
    Q_ASSERT(d->shape);
    if (!dataSet)
        return;

    DatasetCommand *command = new DatasetCommand(dataSet, d->shape);
    command->setDataSetShowPercent(b);
    canvas()->addCommand(command);

    d->shape->update();
}

void ChartTool::setDataSetShowSymbol(DataSet *dataSet, bool b)
{
    //d->setDataSetShowLabel(dataSet, 0, 0, 0, &b);
    Q_ASSERT(d->shape);
    if (!dataSet)
        return;

    DatasetCommand *command = new DatasetCommand(dataSet, d->shape);
    command->setDataSetShowSymbol(b);
    canvas()->addCommand(command);

    d->shape->update();
}

void ChartTool::setThreeDMode(bool threeD)
{
    Q_ASSERT(d->shape);
    if (!d->shape)
        return;

    d->shape->setThreeD(threeD);
    d->shape->update();
}

void ChartTool::setShowTitle(bool show)
{
    Q_ASSERT(d->shape);
    if (!d->shape)
        return;

    ChartTextShapeCommand *command = new ChartTextShapeCommand(d->shape->title(), d->shape, show);
    canvas()->addCommand(command);

    d->shape->update();
}

void ChartTool::setShowSubTitle(bool show)
{
    Q_ASSERT(d->shape);
    if (!d->shape)
        return;

    ChartTextShapeCommand *command = new ChartTextShapeCommand(d->shape->subTitle(), d->shape, show);
    canvas()->addCommand(command);

    d->shape->update();
}

void ChartTool::setShowFooter(bool show)
{
    Q_ASSERT(d->shape);
    if (!d->shape)
        return;

    ChartTextShapeCommand *command = new ChartTextShapeCommand(d->shape->footer(), d->shape, show);
    canvas()->addCommand(command);

    d->shape->update();
}

void ChartTool::setDataDirection(Qt::Orientation direction)
{
    Q_ASSERT(d->shape);
    if (!d->shape)
        return;

    d->shape->proxyModel()->setDataDirection(direction);
    d->shape->relayout();
}


void ChartTool::setLegendTitle(const QString &title)
{
    Q_ASSERT(d->shape);
    Q_ASSERT(d->shape->legend());

    LegendCommand *command = new LegendCommand(d->shape->legend());
    command->setLegendTitle(title);
    canvas()->addCommand(command);
}

void ChartTool::setLegendFont(const QFont &font)
{
    Q_ASSERT(d->shape);
    Q_ASSERT(d->shape->legend());

    // There only is a general font, for the legend items and the legend title
    LegendCommand *command = new LegendCommand(d->shape->legend());
    command->setLegendFont(font);
    canvas()->addCommand(command);
}

void ChartTool::setLegendFontSize(int size)
{
    Q_ASSERT(d->shape);
    Q_ASSERT(d->shape->legend());

    LegendCommand *command = new LegendCommand(d->shape->legend());
    command->setLegendFontSize(size);
    canvas()->addCommand(command);
}

void ChartTool::setLegendOrientation(Qt::Orientation orientation)
{
    Q_ASSERT(d->shape);
    Q_ASSERT(d->shape->legend());

    LegendCommand *command = new LegendCommand(d->shape->legend());
    command->setLegendExpansion(QtOrientationToLegendExpansion(orientation));
    canvas()->addCommand(command);
}

void ChartTool::setLegendPosition(Position pos)
{
    Q_ASSERT(d->shape);
    Q_ASSERT(d->shape->legend());

    d->shape->legend()->setLegendPosition(pos);
    d->shape->legend()->update();
    d->shape->layout()->layout();
}

void ChartTool::setLegendAlignment(Qt::Alignment alignment)
{
    Q_ASSERT(d->shape);
    Q_ASSERT(d->shape->legend());

    d->shape->legend()->setAlignment(alignment);
    d->shape->legend()->update();
    d->shape->layout()->scheduleRelayout();
    d->shape->layout()->layout();
}

void ChartTool::addAxis(AxisDimension dimension, const QString& title)
{
    Q_ASSERT(d->shape);

    Axis *axis = new Axis(d->shape->plotArea(), dimension); // automatically adds axis to plot area
    if (axis == d->shape->plotArea()->secondaryYAxis()) {
        axis->title()->rotate(90);
        axis->setOdfAxisPosition("end"); // right
        axis->updateKChartAxisPosition();
    } else if (axis == d->shape->plotArea()->secondaryXAxis()) {
        axis->setOdfAxisPosition("end"); // top
        axis->updateKChartAxisPosition();
    }
    d->shape->plotArea()->takeAxis(axis); // so we remove it again, sigh
    axis->setTitleText(title);
    AddRemoveAxisCommand *command = new AddRemoveAxisCommand(axis, d->shape, true, canvas()->shapeManager());
    canvas()->addCommand(command);
}

void ChartTool::removeAxis(Axis *axis)
{
    Q_ASSERT(d->shape);

    AddRemoveAxisCommand *command = new AddRemoveAxisCommand(axis, d->shape, false, canvas()->shapeManager());
    canvas()->addCommand(command);
}

void ChartTool::setAxisTitle(Axis *axis, const QString& title)
{
    Q_ASSERT(d->shape);

    AxisCommand *command = new AxisCommand(axis, d->shape);
    command->setAxisTitle(title);
    canvas()->addCommand(command);

    d->shape->update();
}

void ChartTool::setAxisShowTitle(Axis *axis, bool show)
{
    Q_ASSERT(d->shape);
    if (show && axis->titleText().isEmpty()) {
        axis->setTitleText(i18n("Axistitle"));
    }
    AxisCommand *command = new AxisCommand(axis, d->shape);
    command->setAxisShowTitle(show);
    canvas()->addCommand(command);

    d->shape->update();
}

void ChartTool::setShowAxis(Axis *axis, bool show)
{
    Q_ASSERT(d->shape);
    debugChartTool<<axis<<show;
    AxisCommand *command = new AxisCommand(axis, d->shape);
    command->setShowAxis(show);
    canvas()->addCommand(command);

    d->shape->update();
}

void ChartTool::setAxisShowMajorGridLines(Axis *axis, bool b)
{
    Q_ASSERT(d->shape);

    AxisCommand *command = new AxisCommand(axis, d->shape);
    command->setAxisShowMajorGridLines(b);
    canvas()->addCommand(command);

    d->shape->update();
}

void ChartTool::setAxisShowMinorGridLines(Axis *axis, bool b)
{
    Q_ASSERT(d->shape);

    AxisCommand *command = new AxisCommand(axis, d->shape);
    command->setAxisShowMinorGridLines(b);
    canvas()->addCommand(command);

    d->shape->update();
}

void ChartTool::setAxisUseLogarithmicScaling(Axis *axis, bool b)
{
    Q_ASSERT(d->shape);

    AxisCommand *command = new AxisCommand(axis, d->shape);
    command->setAxisUseLogarithmicScaling(b);
    canvas()->addCommand(command);

    d->shape->update();
}

void ChartTool::setAxisStepWidth(Axis *axis, qreal width)
{
    Q_ASSERT(d->shape);

    AxisCommand *command = new AxisCommand(axis, d->shape);
    command->setAxisStepWidth(width);
    canvas()->addCommand(command);

    d->shape->update();
}

void ChartTool::setAxisSubStepWidth(Axis *axis, qreal width)
{
    Q_ASSERT(d->shape);

    AxisCommand *command = new AxisCommand(axis, d->shape);
    command->setAxisSubStepWidth(width);
    canvas()->addCommand(command);

    d->shape->update();
}

void ChartTool::setAxisUseAutomaticStepWidth(Axis *axis, bool automatic)
{
    Q_ASSERT(d->shape);

    AxisCommand *command = new AxisCommand(axis, d->shape);
    command->setAxisUseAutomaticStepWidth(automatic);
    canvas()->addCommand(command);

    d->shape->update();
}

void ChartTool::setAxisUseAutomaticSubStepWidth(Axis *axis, bool automatic)
{
    Q_ASSERT(d->shape);

    AxisCommand *command = new AxisCommand(axis, d->shape);
    command->setAxisUseAutomaticSubStepWidth(automatic);
    canvas()->addCommand(command);

    d->shape->update();
}

void ChartTool::setAxisLabelsFont(Axis *axis, const QFont &font)
{
    Q_ASSERT(d->shape);

    AxisCommand *command = new AxisCommand(axis, d->shape);
    command->setAxisLabelsFont(font);
    canvas()->addCommand(command);

    d->shape->update();
}


void ChartTool::setGapBetweenBars(Axis *axis, int percent)
{
    Q_ASSERT(d->shape);
    debugChartTool<<axis<<percent;
    GapCommand *command = new GapCommand(axis, d->shape);
    command->setGapBetweenBars(percent);
    canvas()->addCommand(command);
}

void ChartTool::setGapBetweenSets(Axis *axis, int percent)
{
    Q_ASSERT(d->shape);
    debugChartTool<<axis<<percent;
    GapCommand *command = new GapCommand(axis, d->shape);
    command->setGapBetweenSets(percent);
    canvas()->addCommand(command);
}

void ChartTool::setPieExplodeFactor(DataSet *dataSet, int percent)
{
    Q_ASSERT(d->shape);

    dataSet->setPieExplodeFactor(percent);
    d->shape->update();
}

void ChartTool::setShowLegend(bool show)
{
    Q_ASSERT(d->shape);

    ChartTextShapeCommand *command = new ChartTextShapeCommand(d->shape->legend(), d->shape, show);
    if (show) {
        command->setText(kundo2_i18n("Show Legend"));
    } else {
        command->setText(kundo2_i18n("Hide Legend"));
    }
    canvas()->addCommand(command);

    d->shape->legend()->update();
}
