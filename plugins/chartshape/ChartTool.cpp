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
#include <KoInteractionTool.h>

// KChart
#include <KChartChart>
#include <KChartCartesianAxis>
#include <KChartGridAttributes>
#include <KChartAbstractCartesianDiagram>
#include <KChartCartesianCoordinatePlane>
#include <KChartPosition>
#include <KChartPieAttributes>

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
#include "PieConfigWidget.h"
#include "RingConfigWidget.h"
#include "RadarDataSetConfigWidget.h"
#include "KChartConvertions.h"
#include "commands/ChartTypeCommand.h"
#include "commands/LegendCommand.h"
#include "commands/AxisCommand.h"
#include "commands/DatasetCommand.h"
#include "commands/ChartTextShapeCommand.h"
#include "commands/AddRemoveAxisCommand.h"
#include "commands/GapCommand.h"
#include "commands/PlotAreaCommand.h"
#include "commands/DatasetCommand.h"
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
    connect(canvas->shapeManager(), SIGNAL(selectionChanged()), this, SLOT(shapeSelectionChanged()));
}

ChartTool::~ChartTool()
{
    delete d;
}

void ChartTool::shapeSelectionChanged()
{
    // When this chart tool is activated with one chart and a new chart is created,
    // the new chart is selected but the deactivate method is not called,
    // so this tool will still operate on the old chart.
    // We activate the default tool to rectify this.
    // FIXME: could probably be done in KoToolBase (or wherever appropriate)

    if (!d->shape) {
        return;
    }
    QList<KoShape*> lst = canvas()->shapeManager()->selection()->selectedShapes(KoFlake::StrippedSelection);
    if (lst.contains(d->shape)) {
        return;
    }
    for (KoShape *s : lst) {
        ChartShape *chart = dynamic_cast<ChartShape*>(s);
        if (chart && chart != d->shape) {
            activateTool(KoInteractionTool_ID);
        }
    }
}

void ChartTool::paint(QPainter &painter, const KoViewConverter &converter)
{
    if (d->shape) {
        QPen pen;
        //Use the #00adf5 color with 50% opacity
        pen.setColor(QColor(0, 173, 245, 127));
        pen.setWidth(qMax((uint)1, handleRadius() / 2));
        pen.setJoinStyle(Qt::RoundJoin);
        painter.setPen(pen);

        QTransform painterMatrix = painter.worldTransform();
        painter.setWorldTransform(d->shape->absoluteTransformation(&converter) * painterMatrix);
        KoShape::applyConversion(painter, converter);
        painter.drawRect(QRectF(QPointF(), d->shape->size()));
    }
}

void ChartTool::mousePressEvent(KoPointerEvent *event)
{
#if 1  // disabled
    Q_UNUSED(event);
    return;
#else
    // Select dataset
    if (!d->shape || !d->shape->kdChart() || ! d->shape->kdChart()->coordinatePlane()
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
    debugChartTool<<shapes;
    d->shape = 0;
    for (KoShape *s : shapes) {
        d->shape = dynamic_cast<ChartShape*>(s);
        if (!d->shape) {
            for (KoShape *parent = s->parent(); parent; parent = parent->parent()) {
                d->shape = dynamic_cast<ChartShape*>(parent);
                if (d->shape) {
                    break;
                }
            }
        }
        if (d->shape) {
            break;
        }
    }
    debugChartTool<<shapes<<d->shape;
    if (!d->shape) {
        emit done();
        return;
    }
    useCursor(Qt::ArrowCursor);

    foreach (QWidget *w, optionWidgets()) {
        ConfigWidgetBase *widget = dynamic_cast<ConfigWidgetBase*>(w);
        Q_ASSERT(widget);
        if (widget) {
            widget->open(d->shape);
        }
    }
    foreach (QWidget *w, optionWidgets()) {
        ConfigWidgetBase *widget = dynamic_cast<ConfigWidgetBase*>(w);
        Q_ASSERT(widget);
        if (widget) {
            widget->updateData();
        }
    }
    d->shape->update(); // to paint decoration
}

void ChartTool::deactivate()
{
    debugChartTool<<d->shape;

    foreach (QWidget *w, optionWidgets()) {
        ConfigWidgetBase *configWidget = dynamic_cast<ConfigWidgetBase*>(w);
        if (configWidget)
            configWidget->deactivate();
    }
    if (d->shape) {
        d->shape->update(); // to get rid of decoration
    }
    d->shape = 0;
}

QList<QPointer<QWidget> > ChartTool::createOptionWidgets()
{
    QList<QPointer<QWidget> > widgets;

    TitlesConfigWidget *titles = new TitlesConfigWidget();
    titles->setWindowTitle(i18n("Titles"));
    widgets.append(titles);
    connect(titles->ui.showTitle, SIGNAL(toggled(bool)), this, SLOT(setShowTitle(bool)));
    connect(titles->ui.titlePositioning, SIGNAL(currentIndexChanged(int)), this, SLOT(setTitlePositioning(int)));
    connect(titles->ui.titleResize, SIGNAL(currentIndexChanged(int)), this, SLOT(setTitleResize(int)));
    connect(titles->ui.showSubTitle, SIGNAL(toggled(bool)), this, SLOT(setShowSubTitle(bool)));
    connect(titles->ui.subtitlePositioning, SIGNAL(currentIndexChanged(int)), this, SLOT(setSubTitlePositioning(int)));
    connect(titles->ui.subtitleResize, SIGNAL(currentIndexChanged(int)), this, SLOT(setSubTitleResize(int)));
    connect(titles->ui.showFooter, SIGNAL(toggled(bool)), this, SLOT(setShowFooter(bool)));
    connect(titles->ui.footerPositioning, SIGNAL(currentIndexChanged(int)), this, SLOT(setFooterPositioning(int)));
    connect(titles->ui.footerResize, SIGNAL(currentIndexChanged(int)), this, SLOT(setFooterResize(int)));

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

    PlotAreaConfigWidget  *plotarea = new PlotAreaConfigWidget();
    plotarea->setWindowTitle(i18n("Plot Area"));
    widgets.append(plotarea);

    connect(plotarea, SIGNAL(chartTypeChanged(ChartType,ChartSubtype)),
            this,   SLOT(setChartType(ChartType,ChartSubtype)));
    connect(plotarea, SIGNAL(chartSubTypeChanged(ChartSubtype)),
            this,   SLOT(setChartSubType(ChartSubtype)));
    connect(plotarea, SIGNAL(threeDModeToggled(bool)),
            this,   SLOT(setThreeDMode(bool)));
    connect(plotarea, SIGNAL(chartOrientationChanged(Qt::Orientation)),
            this,   SLOT(setChartOrientation(Qt::Orientation)));

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

    AxesConfigWidget *axes = plotarea->cartesianAxesConfigWidget();
    connect(axes, SIGNAL(axisAdded(AxisDimension,QString)),
            this,   SLOT(addAxis(AxisDimension,QString)));
    connect(axes, SIGNAL(axisRemoved(Axis*)),
            this,   SLOT(removeAxis(Axis*)));
    connect(axes, SIGNAL(axisShowTitleChanged(Axis*,bool)),
            this,   SLOT(setAxisShowTitle(Axis*,bool)));
    connect(axes, SIGNAL(axisShowChanged(Axis*,bool)),
            this,   SLOT(setShowAxis(Axis*,bool)));
    connect(axes, SIGNAL(axisPositionChanged(Axis*,QString)),
            this,   SLOT(setAxisPosition(Axis*,QString)));
    connect(axes, SIGNAL(axisLabelsPositionChanged(Axis*,QString)),
            this,   SLOT(setAxisLabelsPosition(Axis*,QString)));
    connect(axes, SIGNAL(axisShowLabelsChanged(Axis*,bool)),
            this,   SLOT(setAxisShowLabels(Axis*,bool)));

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


    DataSetConfigWidget *dataset = plotarea->cartesianDataSetConfigWidget();
    connect(dataset, SIGNAL(dataSetChartTypeChanged(DataSet*,ChartType,ChartSubtype)),
            this,   SLOT(setDataSetChartType(DataSet*,ChartType,ChartSubtype)));
    connect(dataset, SIGNAL(datasetBrushChanged(DataSet*,QColor,int)),
            this, SLOT(setDataSetBrush(DataSet*,QColor,int)));
    connect(dataset, SIGNAL(dataSetMarkerChanged(DataSet*,OdfSymbolType,OdfMarkerStyle)),
            this, SLOT(setDataSetMarker(DataSet*,OdfSymbolType,OdfMarkerStyle)));
    connect(dataset, SIGNAL(datasetPenChanged(DataSet*,QColor,int)),
            this, SLOT(setDataSetPen(DataSet*,QColor,int)));
    connect(dataset, SIGNAL(datasetShowCategoryChanged(DataSet*,bool,int)),
            this, SLOT(setDataSetShowCategory(DataSet*,bool,int)));
    connect(dataset, SIGNAL(dataSetShowNumberChanged(DataSet*,bool,int)),
            this, SLOT(setDataSetShowNumber(DataSet*,bool,int)));
    connect(dataset, SIGNAL(datasetShowPercentChanged(DataSet*,bool,int)),
            this, SLOT(setDataSetShowPercent(DataSet*,bool,int)));
    connect(dataset, SIGNAL(datasetShowSymbolChanged(DataSet*,bool,int)),
            this, SLOT(setDataSetShowSymbol(DataSet*,bool,int)));
    connect(dataset, SIGNAL(dataSetAxisChanged(DataSet*,Axis*)),
            this, SLOT(setDataSetAxis(DataSet*,Axis*)));
    connect(dataset, SIGNAL(axisAdded(AxisDimension,QString)),
            this,   SLOT(addAxis(AxisDimension,QString)));


    PieConfigWidget *pie = plotarea->pieConfigWidget();
    connect(pie, SIGNAL(explodeFactorChanged(DataSet*,int, int)), this, SLOT(setPieExplodeFactor(DataSet*,int, int)));
    connect(pie, SIGNAL(brushChanged(DataSet*,QColor,int)), this, SLOT(setDataSetBrush(DataSet*,QColor,int)));
    connect(pie, SIGNAL(penChanged(DataSet*,QColor,int)), this, SLOT(setDataSetPen(DataSet*,QColor,int)));
    connect(pie, SIGNAL(showCategoryChanged(DataSet*,bool,int)), this, SLOT(setDataSetShowCategory(DataSet*,bool,int)));
    connect(pie, SIGNAL(showNumberChanged(DataSet*,bool,int)), this, SLOT(setDataSetShowNumber(DataSet*,bool,int)));
    connect(pie, SIGNAL(showPercentChanged(DataSet*,bool,int)), this, SLOT(setDataSetShowPercent(DataSet*,bool,int)));

    RingConfigWidget *ring = plotarea->ringConfigWidget();
    connect(ring, SIGNAL(explodeFactorChanged(DataSet*,int, int)), this, SLOT(setPieExplodeFactor(DataSet*,int, int)));
    connect(ring, SIGNAL(brushChanged(DataSet*,QColor,int)), this, SLOT(setDataSetBrush(DataSet*,QColor,int)));
    connect(ring, SIGNAL(penChanged(DataSet*,QColor,int)), this, SLOT(setDataSetPen(DataSet*,QColor,int)));
    connect(ring, SIGNAL(showCategoryChanged(DataSet*,bool,int)), this, SLOT(setDataSetShowCategory(DataSet*,bool,int)));
    connect(ring, SIGNAL(showNumberChanged(DataSet*,bool,int)), this, SLOT(setDataSetShowNumber(DataSet*,bool,int)));
    connect(ring, SIGNAL(showPercentChanged(DataSet*,bool,int)), this, SLOT(setDataSetShowPercent(DataSet*,bool,int)));

    axes = plotarea->stockAxesConfigWidget();
    connect(axes, SIGNAL(axisAdded(AxisDimension,QString)),
            this,   SLOT(addAxis(AxisDimension,QString)));
    connect(axes, SIGNAL(axisRemoved(Axis*)),
            this,   SLOT(removeAxis(Axis*)));
    connect(axes, SIGNAL(axisShowTitleChanged(Axis*,bool)),
            this,   SLOT(setAxisShowTitle(Axis*,bool)));
    connect(axes, SIGNAL(axisShowChanged(Axis*,bool)),
            this,   SLOT(setShowAxis(Axis*,bool)));
    connect(axes, SIGNAL(axisPositionChanged(Axis*,QString)),
            this,   SLOT(setAxisPosition(Axis*,QString)));
    connect(axes, SIGNAL(axisLabelsPositionChanged(Axis*,QString)),
            this,   SLOT(setAxisLabelsPosition(Axis*,QString)));
    connect(axes, SIGNAL(axisShowLabelsChanged(Axis*,bool)),
            this,   SLOT(setAxisShowLabels(Axis*,bool)));

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

    // Radar
    RadarDataSetConfigWidget *rdataset = plotarea->radarDataSetConfigWidget();
    connect(rdataset, SIGNAL(datasetBrushChanged(DataSet*,QColor,int)),
            this, SLOT(setDataSetBrush(DataSet*,QColor,int)));
    connect(rdataset, SIGNAL(dataSetMarkerChanged(DataSet*,OdfSymbolType,OdfMarkerStyle)),
            this, SLOT(setDataSetMarker(DataSet*,OdfSymbolType,OdfMarkerStyle)));
    connect(rdataset, SIGNAL(datasetPenChanged(DataSet*,QColor,int)),
            this, SLOT(setDataSetPen(DataSet*,QColor,int)));
    connect(rdataset, SIGNAL(datasetShowCategoryChanged(DataSet*,bool,int)),
            this, SLOT(setDataSetShowCategory(DataSet*,bool,int)));
    connect(rdataset, SIGNAL(dataSetShowNumberChanged(DataSet*,bool,int)),
            this, SLOT(setDataSetShowNumber(DataSet*,bool,int)));
    connect(rdataset, SIGNAL(datasetShowPercentChanged(DataSet*,bool,int)),
            this, SLOT(setDataSetShowPercent(DataSet*,bool,int)));
    connect(rdataset, SIGNAL(datasetShowSymbolChanged(DataSet*,bool,int)),
            this, SLOT(setDataSetShowSymbol(DataSet*,bool,int)));

    connect(d->shape, SIGNAL(updateConfigWidget()), plotarea, SLOT(updateData()));

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
    debugChartTool<<dataSet<<region.toString();
    if (!dataSet)
        return;

    dataSet->setXDataRegion(region);
    d->shape->update();
}

void ChartTool::setDataSetYDataRegion(DataSet *dataSet, const CellRegion &region)
{
    if (!dataSet)
        return;

    dataSet->setYDataRegion(region);
    d->shape->update();
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
    d->shape->update();
    d->shape->legend()->update();
}

void ChartTool::setDataSetCategoryDataRegion(DataSet *dataSet, const CellRegion &region)
{
    if (!dataSet) {
        return;
    }
    if (isCartesian(d->shape->chartType())) {
        // FIXME: Seems strange the way things are stored in multiple places
        // Categories are labels on the categories axis
        dataSet->setCategoryDataRegion(region); // probably should not be stored here, as datasets cannot have individual categories
        d->shape->plotArea()->proxyModel()->setCategoryDataRegion(region); // this seems to be for odf only!?
    } else {
        // Categories are legend texts
        dataSet->setCategoryDataRegion(region);
    }
    d->shape->update();
    d->shape->legend()->update();
}


void ChartTool::setDataSetChartType(DataSet *dataSet, ChartType type, ChartSubtype subType)
{
    Q_ASSERT(d->shape);
    Q_ASSERT(dataSet);
    if (dataSet) {
        DatasetCommand *cmd = new DatasetCommand(dataSet, d->shape);
        cmd->setDataSetChartType(type, subType);
        canvas()->addCommand(cmd);
    }
    d->shape->update();
    d->shape->legend()->update();
}


void ChartTool::setDataSetBrush(DataSet *dataSet, const QColor& color, int section)
{
    Q_ASSERT(d->shape);
    Q_ASSERT(dataSet || section >= 0);
    debugChartTool<<dataSet<<color<<section;
    if (!dataSet) {
        QList<DataSet*> lst = d->shape->proxyModel()->dataSets();
        if (lst.isEmpty()) {
            return;
        }
        // we set brush for section in all datasets
        KUndo2Command *command = new KUndo2Command();
        for (int i = 0; i < lst.count(); ++i) {
            DatasetCommand *cmd = new DatasetCommand(lst.at(i), d->shape, section, command);
            cmd->setDataSetBrush(color);
            command->setText(cmd->text());
        }
        canvas()->addCommand(command);
    } else {
        DatasetCommand *command = new DatasetCommand(dataSet, d->shape, section);
        command->setDataSetBrush(color);
        canvas()->addCommand(command);
    }
}

void ChartTool::setDataSetPen(DataSet *dataSet, const QColor& color, int section)
{
    Q_ASSERT(d->shape);
    Q_ASSERT(dataSet || section >= 0);
    debugChartTool<<color<<section;
    if (!dataSet) {
        QList<DataSet*> lst = d->shape->proxyModel()->dataSets();
        if (lst.isEmpty()) {
            return;
        }
        // we set brush for section in all datasets
        KUndo2Command *command = new KUndo2Command();
        for (int i = 0; i < lst.count(); ++i) {
            DatasetCommand *cmd = new DatasetCommand(lst.at(i), d->shape, section, command);
            cmd->setDataSetPen(color);
            command->setText(cmd->text());
        }
        canvas()->addCommand(command);
    } else {
        DatasetCommand *command = new DatasetCommand(dataSet, d->shape, section);
        command->setDataSetPen(color);
        canvas()->addCommand(command);
    }
}

void ChartTool::setDataSetMarker(DataSet *dataSet, OdfSymbolType type, OdfMarkerStyle style)
{
    Q_ASSERT(d->shape);
    if (!dataSet) {
        return;
    }
    DatasetCommand *command = new DatasetCommand(dataSet, d->shape);
    command->setDataSetMarker(type, style);
    canvas()->addCommand(command);
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

void ChartTool::setDataSetShowCategory(DataSet *dataSet, bool b, int section)
{
    Q_ASSERT(d->shape);
    Q_ASSERT(dataSet || section >= 0);
    if (!dataSet) {
        QList<DataSet*> lst = d->shape->proxyModel()->dataSets();
        if (lst.isEmpty()) {
            return;
        }
        // we set brush for section in all datasets
        KUndo2Command *command = new KUndo2Command();
        for (int i = 0; i < lst.count(); ++i) {
            DatasetCommand *cmd = new DatasetCommand(lst.at(i), d->shape, section, command);
            cmd->setDataSetShowCategory(b);
            command->setText(cmd->text());
        }
        canvas()->addCommand(command);
    } else {
        DatasetCommand *command = new DatasetCommand(dataSet, d->shape, section);
        command->setDataSetShowCategory(b);
        canvas()->addCommand(command);
    }

    debugChartTool<<section<<b<<':'<<dataSet->valueLabelType(section).category;
}

void ChartTool::setDataSetShowNumber(DataSet *dataSet, bool b, int section)
{
    debugChartTool<<b<<section<<dataSet;
    Q_ASSERT(d->shape);
    Q_ASSERT(dataSet || section >= 0);
    if (!dataSet) {
        QList<DataSet*> lst = d->shape->proxyModel()->dataSets();
        if (lst.isEmpty()) {
            return;
        }
        // we set brush for section in all datasets
        KUndo2Command *command = new KUndo2Command();
        for (int i = 0; i < lst.count(); ++i) {
            DatasetCommand *cmd = new DatasetCommand(lst.at(i), d->shape, section, command);
            cmd->setDataSetShowNumber(b);
            command->setText(cmd->text());
        }
        canvas()->addCommand(command);
    } else {
        DatasetCommand *command = new DatasetCommand(dataSet, d->shape, section);
        command->setDataSetShowNumber(b);
        canvas()->addCommand(command);
    }
    debugChartTool<<section<<b<<':'<<dataSet->valueLabelType(section).number;
}

void ChartTool::setDataSetShowPercent(DataSet *dataSet, bool b, int section)
{
    Q_ASSERT(d->shape);
    Q_ASSERT(dataSet || section >= 0);
    if (!dataSet) {
        QList<DataSet*> lst = d->shape->proxyModel()->dataSets();
        if (lst.isEmpty()) {
            return;
        }
        // we set brush for section in all datasets
        KUndo2Command *command = new KUndo2Command();
        for (int i = 0; i < lst.count(); ++i) {
            DatasetCommand *cmd = new DatasetCommand(lst.at(i), d->shape, section, command);
            cmd->setDataSetShowPercent(b);
            command->setText(cmd->text());
        }
        canvas()->addCommand(command);
    } else {
        DatasetCommand *command = new DatasetCommand(dataSet, d->shape, section);
        command->setDataSetShowPercent(b);
        canvas()->addCommand(command);
    }

    debugChartTool<<section<<b<<':'<<dataSet->valueLabelType(section).percentage;
}

void ChartTool::setDataSetShowSymbol(DataSet *dataSet, bool b, int section)
{
    Q_ASSERT(d->shape);
    Q_ASSERT(dataSet || section >= 0);
    if (!dataSet) {
        QList<DataSet*> lst = d->shape->proxyModel()->dataSets();
        if (lst.isEmpty()) {
            return;
        }
        // we set brush for section in all datasets
        KUndo2Command *command = new KUndo2Command();
        for (int i = 0; i < lst.count(); ++i) {
            DatasetCommand *cmd = new DatasetCommand(lst.at(i), d->shape, section, command);
            cmd->setDataSetShowSymbol(b);
            command->setText(cmd->text());
        }
        canvas()->addCommand(command);
    } else {
        DatasetCommand *command = new DatasetCommand(dataSet, d->shape, section);
        command->setDataSetShowSymbol(b);
        canvas()->addCommand(command);
    }
    debugChartTool<<section<<b<<':'<<dataSet->valueLabelType(section).symbol;
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
}

void ChartTool::setTitlePositioning(int index)
{
    Q_ASSERT(d->shape);
    if (!d->shape) {
        return;
    }
    // TODD: undo command
    d->shape->title()->setAdditionalStyleAttribute("chart:auto-position", index == 0 ? "true" : "false");
    d->shape->layout()->scheduleRelayout();
    d->shape->layout()->layout();
    d->shape->update();
}

void ChartTool::setTitleResize(int index)
{
    Q_ASSERT(d->shape);
    if (!d->shape) {
        return;
    }
    // TODD: undo command
    TextLabelData *labelData = dynamic_cast<TextLabelData*>(d->shape->title()->userData());
    if (labelData == 0) {
        return;
    }
    labelData->setResizeMethod(index == 0 ? KoTextShapeDataBase::AutoResize : KoTextShapeDataBase::NoResize);
    d->shape->layout()->scheduleRelayout();
    d->shape->layout()->layout();
    d->shape->update();
}

void ChartTool::setShowSubTitle(bool show)
{
    Q_ASSERT(d->shape);
    if (!d->shape)
        return;

    ChartTextShapeCommand *command = new ChartTextShapeCommand(d->shape->subTitle(), d->shape, show);
    canvas()->addCommand(command);
}

void ChartTool::setSubTitlePositioning(int index)
{
    Q_ASSERT(d->shape);
    if (!d->shape) {
        return;
    }
    // TODD: undo command
    d->shape->subTitle()->setAdditionalStyleAttribute("chart:auto-position", index == 0 ? "true" : "false");
    d->shape->layout()->scheduleRelayout();
    d->shape->layout()->layout();
    d->shape->update();
}

void ChartTool::setSubTitleResize(int index)
{
    Q_ASSERT(d->shape);
    if (!d->shape) {
        return;
    }
    // TODD: undo command
    TextLabelData *labelData = dynamic_cast<TextLabelData*>(d->shape->subTitle()->userData());
    if (labelData == 0) {
        return;
    }
    labelData->setResizeMethod(index == 0 ? KoTextShapeDataBase::AutoResize : KoTextShapeDataBase::NoResize);
    d->shape->layout()->scheduleRelayout();
    d->shape->layout()->layout();
    d->shape->update();
}

void ChartTool::setShowFooter(bool show)
{
    Q_ASSERT(d->shape);
    if (!d->shape)
        return;

    ChartTextShapeCommand *command = new ChartTextShapeCommand(d->shape->footer(), d->shape, show);
    canvas()->addCommand(command);
}

void ChartTool::setFooterPositioning(int index)
{
    Q_ASSERT(d->shape);
    if (!d->shape) {
        return;
    }
    // TODD: undo command
    d->shape->footer()->setAdditionalStyleAttribute("chart:auto-position", index == 0 ? "true" : "false");
    d->shape->layout()->scheduleRelayout();
    d->shape->layout()->layout();
    d->shape->update();
}

void ChartTool::setFooterResize(int index)
{
    Q_ASSERT(d->shape);
    if (!d->shape) {
        return;
    }
    // TODD: undo command
    TextLabelData *labelData = dynamic_cast<TextLabelData*>(d->shape->footer()->userData());
    if (labelData == 0) {
        return;
    }
    labelData->setResizeMethod(index == 0 ? KoTextShapeDataBase::AutoResize : KoTextShapeDataBase::NoResize);
    d->shape->layout()->scheduleRelayout();
    d->shape->layout()->layout();
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

void ChartTool::setChartOrientation(Qt::Orientation direction)
{
    Q_ASSERT(d->shape);
    if (!d->shape) {
        return;
    }
    PlotAreaCommand *command = new PlotAreaCommand(d->shape->plotArea());
    command->setOrientation(direction);
    canvas()->addCommand(command);
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

    // TODO undo command
    d->shape->legend()->setLegendPosition(pos);
    d->shape->legend()->update();
    d->shape->layout()->scheduleRelayout();
    d->shape->layout()->layout();
}

void ChartTool::setLegendAlignment(Qt::Alignment alignment)
{
    Q_ASSERT(d->shape);
    Q_ASSERT(d->shape->legend());

    // TODO undo command
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
        axis->setOdfAxisPosition("end"); // right
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

void ChartTool::setAxisShowTitle(Axis *axis, bool show)
{
    Q_ASSERT(d->shape);
    if (show && axis->titleText().isEmpty()) {
        axis->setTitleText(i18n("Axistitle"));
    }
    AxisCommand *command = new AxisCommand(axis, d->shape);
    command->setAxisShowTitle(show);
    canvas()->addCommand(command);
}

void ChartTool::setShowAxis(Axis *axis, bool show)
{
    Q_ASSERT(d->shape);
    debugChartTool<<axis<<show;
    AxisCommand *command = new AxisCommand(axis, d->shape);
    command->setShowAxis(show);
    canvas()->addCommand(command);
}

void ChartTool::setAxisPosition(Axis *axis, const QString &pos)
{
    Q_ASSERT(d->shape);
    debugChartTool<<axis<<pos;
    AxisCommand *command = new AxisCommand(axis, d->shape);
    command->setAxisPosition(pos);
    canvas()->addCommand(command);
}

void ChartTool::setAxisLabelsPosition(Axis *axis, const QString &pos)
{
    Q_ASSERT(d->shape);
    debugChartTool<<axis<<pos;
    AxisCommand *command = new AxisCommand(axis, d->shape);
    command->setAxisLabelsPosition(pos);
    canvas()->addCommand(command);
}

void ChartTool::setAxisShowLabels(Axis *axis, bool b)
{
    Q_ASSERT(d->shape);

    AxisCommand *command = new AxisCommand(axis, d->shape);
    command->setAxisShowLabels(b);
    canvas()->addCommand(command);
}

void ChartTool::setAxisShowMajorGridLines(Axis *axis, bool b)
{
    Q_ASSERT(d->shape);

    AxisCommand *command = new AxisCommand(axis, d->shape);
    command->setAxisShowMajorGridLines(b);
    canvas()->addCommand(command);
}

void ChartTool::setAxisShowMinorGridLines(Axis *axis, bool b)
{
    Q_ASSERT(d->shape);

    AxisCommand *command = new AxisCommand(axis, d->shape);
    command->setAxisShowMinorGridLines(b);
    canvas()->addCommand(command);
}

void ChartTool::setAxisUseLogarithmicScaling(Axis *axis, bool b)
{
    Q_ASSERT(d->shape);

    AxisCommand *command = new AxisCommand(axis, d->shape);
    command->setAxisUseLogarithmicScaling(b);
    canvas()->addCommand(command);
}

void ChartTool::setAxisStepWidth(Axis *axis, qreal width)
{
    Q_ASSERT(d->shape);

    AxisCommand *command = new AxisCommand(axis, d->shape);
    command->setAxisStepWidth(width);
    canvas()->addCommand(command);
}

void ChartTool::setAxisSubStepWidth(Axis *axis, qreal width)
{
    Q_ASSERT(d->shape);

    AxisCommand *command = new AxisCommand(axis, d->shape);
    command->setAxisSubStepWidth(width);
    canvas()->addCommand(command);
}

void ChartTool::setAxisUseAutomaticStepWidth(Axis *axis, bool automatic)
{
    Q_ASSERT(d->shape);

    AxisCommand *command = new AxisCommand(axis, d->shape);
    command->setAxisUseAutomaticStepWidth(automatic);
    canvas()->addCommand(command);
}

void ChartTool::setAxisUseAutomaticSubStepWidth(Axis *axis, bool automatic)
{
    Q_ASSERT(d->shape);

    AxisCommand *command = new AxisCommand(axis, d->shape);
    command->setAxisUseAutomaticSubStepWidth(automatic);
    canvas()->addCommand(command);
}

void ChartTool::setAxisLabelsFont(Axis *axis, const QFont &font)
{
    Q_ASSERT(d->shape);

    AxisCommand *command = new AxisCommand(axis, d->shape);
    command->setAxisLabelsFont(font);
    canvas()->addCommand(command);
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

void ChartTool::setPieExplodeFactor(DataSet *dataSet, int section, int percent)
{
    Q_ASSERT(d->shape);

    dataSet->setPieExplodeFactor(section, percent);
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
}
