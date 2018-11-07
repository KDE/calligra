/* This file is part of the KDE project
   Copyright (C) 2018 Dag Andersen <danders@get2net.dk>
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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
   Boston, MA 02110-1301, USA.
*/

// Own
#include "ChartShapeFactory.h"

// Qt
#include <QStringList>

// KF5
#include <kiconloader.h>
#include <kpluginfactory.h>
#include <klocalizedstring.h>

// Calligra
#include <KoIcon.h>
#include <KoToolRegistry.h>
#include <KoShapeRegistry.h>
#include <KoShapeLoadingContext.h>
#include <KoOdfLoadingContext.h>
#include <KoDocumentResourceManager.h>
#include <KoStyleStack.h>
#include <KoProperties.h>

// Chart shape
#include "ChartShape.h"
#include "ChartToolFactory.h"
#include "ChartProxyModel.h"
#include "ChartTableModel.h"
#include "PlotArea.h"
#include "Axis.h"
#include "Legend.h"
#include "TableSource.h"
#include "ChartLayout.h"

using namespace KoChart;

K_PLUGIN_FACTORY_WITH_JSON(ChartShapePluginFactory, "calligra_shape_chart.json",
                           registerPlugin<ChartShapePlugin>();)

ChartShapePlugin::ChartShapePlugin(QObject * parent, const QVariantList&)
    : QObject(parent)
{
    // Register the chart shape factory.
    KoShapeRegistry::instance()->add(new ChartShapeFactory());

    // Register all tools for the chart shape.
    KoToolRegistry::instance()->add(new ChartToolFactory());
}


ChartShapeFactory::ChartShapeFactory()
    : KoShapeFactoryBase(ChartShapeId, i18n("Chart"))
{
    setXmlElementNames("urn:oasis:names:tc:opendocument:xmlns:drawing:1.0", QStringList("object"));
    setToolTip(i18n("Business charts"));

    KIconLoader::global()->addAppDir("kchart");
    setIconName(koIconNameNeededWithSubs("", "x-shape-chart", "office-chart-bar"));

    // Default 'app specific' config pages i.e. unless an app defines
    // other config pages, these are used.
    QList<KoShapeConfigFactoryBase*> panelFactories;
    // panelFactories.append(new ChartDataConfigFactory());
    setOptionPanels(panelFactories);

    setFamily("chart");

    int order = 0;
    KoShapeTemplate params;
    params.id = ChartShapeId;
    params.family = family();

    params.templateId = "Bar";
    params.name = i18n("Bar Chart");
    params.toolTip = i18n("Normal bar chart");
    params.iconName = chartTypeIconName(BarChartType, NormalChartSubtype);
    KoProperties *properties = new KoProperties();
    properties->setProperty("chart-type", BarChartType);
    properties->setProperty("chart-sub-type", NormalChartSubtype);
    params.properties = properties;
    params.order = order++;
    addTemplate(params);

    params.templateId = "BarPercent";
    params.name = i18n("Stacked Bar Chart");
    params.toolTip = i18n("Stacked bar chart");
    params.iconName = chartTypeIconName(BarChartType, StackedChartSubtype);
    properties = new KoProperties();
    properties->setProperty("chart-type", BarChartType);
    properties->setProperty("chart-sub-type", StackedChartSubtype);
    params.properties = properties;
    params.order = order++;
    addTemplate(params);

    params.templateId = "BarStacked";
    params.name = i18n("Percent Bar Chart");
    params.toolTip = i18n("Percent bar chart");
    params.iconName = chartTypeIconName(BarChartType, PercentChartSubtype);
    properties = new KoProperties();
    properties->setProperty("chart-type", BarChartType);
    properties->setProperty("chart-sub-type", PercentChartSubtype);
    params.properties = properties;
    params.order = order++;
    addTemplate(params);

    params.templateId = "Line";
    params.name = i18n("Line Chart");
    params.toolTip = i18n("Normal line chart");
    params.iconName = chartTypeIconName(LineChartType, NormalChartSubtype);
    properties = new KoProperties();
    properties->setProperty("chart-type", LineChartType);
    properties->setProperty("chart-sub-type", NormalChartSubtype);
    params.properties = properties;
    params.order = order++;
    addTemplate(params);

    params.templateId = "LinePercent";
    params.name = i18n("Stacked Line Chart");
    params.toolTip = i18n("Stacked line chart");
    params.iconName = chartTypeIconName(LineChartType, StackedChartSubtype);
    properties = new KoProperties();
    properties->setProperty("chart-type", LineChartType);
    properties->setProperty("chart-sub-type", StackedChartSubtype);
    params.properties = properties;
    params.order = order++;
    addTemplate(params);

    params.templateId = "LineStacked";
    params.name = i18n("Percent Line Chart");
    params.toolTip = i18n("Percent line chart");
    params.iconName = chartTypeIconName(LineChartType, PercentChartSubtype);
    properties = new KoProperties();
    properties->setProperty("chart-type", LineChartType);
    properties->setProperty("chart-sub-type", PercentChartSubtype);
    params.properties = properties;
    params.order = order++;
    addTemplate(params);

    params.templateId = "Area";
    params.name = i18n("Area Chart");
    params.toolTip = i18n("Normal area chart");
    params.iconName = chartTypeIconName(AreaChartType, NormalChartSubtype);
    properties = new KoProperties();
    properties->setProperty("chart-type", AreaChartType);
    properties->setProperty("chart-sub-type", NormalChartSubtype);
    params.properties = properties;
    params.order = order++;
    addTemplate(params);

    params.templateId = "AreaPercent";
    params.name = i18n("Stacked Area Chart");
    params.toolTip = i18n("Stacked area chart");
    params.iconName = chartTypeIconName(AreaChartType, StackedChartSubtype);
    properties = new KoProperties();
    properties->setProperty("chart-type", AreaChartType);
    properties->setProperty("chart-sub-type", StackedChartSubtype);
    params.properties = properties;
    params.order = order++;
    addTemplate(params);

    params.templateId = "AreaStacked";
    params.name = i18n("Percent Area Chart");
    params.toolTip = i18n("Percent area chart");
    params.iconName = chartTypeIconName(AreaChartType, PercentChartSubtype);
    properties = new KoProperties();
    properties->setProperty("chart-type", AreaChartType);
    properties->setProperty("chart-sub-type", PercentChartSubtype);
    params.properties = properties;
    params.order = order++;
    addTemplate(params);

    params.templateId = "Pie";
    params.name = i18n("Pie Chart");
    params.toolTip = i18n("Pie chart");
    params.iconName = chartTypeIconName(CircleChartType, NoChartSubtype);
    properties = new KoProperties();
    properties->setProperty("chart-type", CircleChartType);
    properties->setProperty("chart-sub-type", NoChartSubtype);
    params.properties = properties;
    params.order = order++;
    addTemplate(params);

    params.templateId = "Ring";
    params.name = i18n("Ring Chart");
    params.toolTip = i18n("Ring chart");
    params.iconName = chartTypeIconName(RingChartType, NoChartSubtype);
    properties = new KoProperties();
    properties->setProperty("chart-type", RingChartType);
    properties->setProperty("chart-sub-type", NoChartSubtype);
    params.properties = properties;
    params.order = order++;
    addTemplate(params);

    params.templateId = "Bubble";
    params.name = i18n("Bubble Chart");
    params.toolTip = i18n("Bubble chart");
    params.iconName = chartTypeIconName(BubbleChartType, NoChartSubtype);
    properties = new KoProperties();
    properties->setProperty("chart-type", BubbleChartType);
    properties->setProperty("chart-sub-type", NoChartSubtype);
    params.properties = properties;
    params.order = order++;
    addTemplate(params);

    params.templateId = "Scatter";
    params.name = i18n("Scatter Chart");
    params.toolTip = i18n("Scatter chart");
    params.iconName = chartTypeIconName(ScatterChartType, NoChartSubtype);
    properties = new KoProperties();
    properties->setProperty("chart-type", ScatterChartType);
    properties->setProperty("chart-sub-type", NoChartSubtype);
    params.properties = properties;
    params.order = order++;
    addTemplate(params);

    params.templateId = "Radar";
    params.name = i18n("Radar Chart");
    params.toolTip = i18n("Radar chart");
    params.iconName = chartTypeIconName(RadarChartType, NoChartSubtype);
    properties = new KoProperties();
    properties->setProperty("chart-type", RadarChartType);
    properties->setProperty("chart-sub-type", NoChartSubtype);
    params.properties = properties;
    params.order = order++;
    addTemplate(params);

    params.templateId = "RadarFilled";
    params.name = i18n("Filled Radar Chart");
    params.toolTip = i18n("Filled radar chart");
    params.iconName = chartTypeIconName(FilledRadarChartType, NoChartSubtype);
    properties = new KoProperties();
    properties->setProperty("chart-type", FilledRadarChartType);
    properties->setProperty("chart-sub-type", NoChartSubtype);
    params.properties = properties;
    params.order = order++;
    addTemplate(params);

    params.templateId = "StockCandle";
    params.name = i18n("Candle Stick Stock Chart");
    params.toolTip = i18n("Japanese candle stick stock chart");
    params.iconName = chartTypeIconName(StockChartType, CandlestickChartSubtype);
    properties = new KoProperties();
    properties->setProperty("chart-type", StockChartType);
    properties->setProperty("chart-sub-type", CandlestickChartSubtype);
    params.properties = properties;
    params.order = order++;
    addTemplate(params);

    params.templateId = "StockOHLC";
    params.name = i18n("OHLC Stock Chart");
    params.toolTip = i18n("Open High Low Close stock chart");
    params.iconName = chartTypeIconName(StockChartType, OpenHighLowCloseChartSubtype);
    properties = new KoProperties();
    properties->setProperty("chart-type", StockChartType);
    properties->setProperty("chart-sub-type", OpenHighLowCloseChartSubtype);
    params.properties = properties;
    params.order = order++;
    addTemplate(params);

    params.templateId = "StockHLC";
    params.name = i18n("HLC Stock Chart");
    params.toolTip = i18n("High Low Close stock chart");
    params.iconName = chartTypeIconName(StockChartType, HighLowCloseChartSubtype);
    properties = new KoProperties();
    properties->setProperty("chart-type", StockChartType);
    properties->setProperty("chart-sub-type", HighLowCloseChartSubtype);
    params.properties = properties;
    params.order = order++;
    addTemplate(params);
}


bool ChartShapeFactory::supports(const KoXmlElement &element, KoShapeLoadingContext &context) const
{
    if (element.namespaceURI() == "urn:oasis:names:tc:opendocument:xmlns:drawing:1.0"
        && element.tagName() == "object") {

        QString href = element.attribute("href");
        if (!href.isEmpty()) {
            // check the mimetype
            if (href.startsWith(QLatin1String("./"))) {
                href.remove(0, 2);
            }
            const QString mimetype = context.odfLoadingContext().mimeTypeForPath(href);
            return mimetype.isEmpty() || mimetype == "application/vnd.oasis.opendocument.chart";
        }
    }
    return false;
}

KoShape *ChartShapeFactory::createShape(const KoProperties* properties, KoDocumentResourceManager *documentResources) const
{
    qInfo()<<Q_FUNC_INFO<<properties->property("chart-type")<<properties->property("chart-sub-type");
    switch (properties->intProperty("chart-type")) {
        case BarChartType:
            return createBarChart(documentResources, properties->intProperty("chart-sub-type"));
        case LineChartType:
            return createLineChart(documentResources, properties->intProperty("chart-sub-type"));
        case AreaChartType:
            return createAreaChart(documentResources, properties->intProperty("chart-sub-type"));
        case StockChartType:
            return createStockChart(documentResources, properties->intProperty("chart-sub-type"));
        case CircleChartType:
            return createPieChart(documentResources);
        case RingChartType:
            return createRingChart(documentResources);
        case BubbleChartType:
            return createBubbleChart(documentResources);
        case ScatterChartType:
            return createScatterChart(documentResources);
        case RadarChartType:
            return createRadarChart(documentResources);
        case FilledRadarChartType:
            return createFilledRadarChart(documentResources);
        default:
            return createDefaultShape(documentResources);
    }
    Q_ASSERT(false);
    return 0;
}

KoShape *ChartShapeFactory::createShapeFromOdf(const KoXmlElement &element,
                                               KoShapeLoadingContext &context)
{
    ChartShape* shape = new ChartShape(context.documentResourceManager());

    if (shape->shapeId().isEmpty())
        shape->setShapeId(id());

    context.odfLoadingContext().styleStack().save();
    bool loaded = shape->loadOdf(element, context);
    context.odfLoadingContext().styleStack().restore();

    if (!loaded) {
        delete shape;
        return 0;
    }

    return shape;
}

KoShape *ChartShapeFactory::createDefaultShape(KoDocumentResourceManager *documentResources) const
{
    ChartShape* shape = new ChartShape(documentResources);
    ChartProxyModel *proxyModel = shape->proxyModel();

    // Fill cells with data.
    ChartTableModel *chartData = new ChartTableModel();
    Table *internalTable = shape->tableSource()->add("local-data", chartData);
    Q_ASSERT(!shape->internalModel());
    // setInternalModel() assumes that chartData has already been added to shape->tableSource().
    shape->setInternalModel(chartData);
    // TODO (not implemented yet)
    // shape->tableSource()->setRenameOnNameClash(internalTable);
    chartData->setRowCount(4);
    chartData->setColumnCount(5);

    // Vertical header data
    chartData->setData(chartData->index(1, 0), i18n("January"));
    chartData->setData(chartData->index(2, 0), i18n("July"));
    chartData->setData(chartData->index(3, 0), i18n("December"));

    // Horizontal header data
    chartData->setData(chartData->index(0, 1), i18n("Column %1", 1));
    chartData->setData(chartData->index(0, 2), i18n("Column %1", 2));
    chartData->setData(chartData->index(0, 3), i18n("Column %1", 3));
    chartData->setData(chartData->index(0, 4), i18n("Column %1", 4));

    // First row
    chartData->setData(chartData->index(1, 1), 5.7);
    chartData->setData(chartData->index(1, 2), 3.4);
    chartData->setData(chartData->index(1, 3), 1.2);
    chartData->setData(chartData->index(1, 4), 8.4);

    // Second row
    chartData->setData(chartData->index(2, 1), 2.1);
    chartData->setData(chartData->index(2, 2), 6.5);
    chartData->setData(chartData->index(2, 3), 0.9);
    chartData->setData(chartData->index(2, 4), 1.5);

    // Third row
    chartData->setData(chartData->index(3, 1), 7.9);
    chartData->setData(chartData->index(3, 2), 3.5);
    chartData->setData(chartData->index(3, 3), 8.6);
    chartData->setData(chartData->index(3, 4), 4.3);

    proxyModel->setFirstRowIsLabel(true);
    proxyModel->setFirstColumnIsLabel(true);
    proxyModel->reset(CellRegion(internalTable, QRect(1, 1, 5, 4)));

    shape->plotArea()->xAxis()->setTitleText(i18n("Month"));
    shape->plotArea()->xAxis()->title()->setVisible(true);
    shape->plotArea()->yAxis()->setTitleText(i18n("Growth in %"));
    shape->plotArea()->yAxis()->title()->setVisible(true);

    shape->layout()->scheduleRelayout();
    shape->layout()->layout();

    return shape;
}

QList<KoShapeConfigWidgetBase*> ChartShapeFactory::createShapeOptionPanels()
{
    return QList<KoShapeConfigWidgetBase*>();
}

void ChartShapeFactory::newDocumentResourceManager(KoDocumentResourceManager *manager) const
{
    Q_UNUSED(manager);
}

ChartShape *ChartShapeFactory::createBarChart(KoDocumentResourceManager *documentResources, int subtype) const
{
    ChartShape* shape = static_cast<ChartShape*>(createDefaultShape(documentResources));
    shape->setChartType(BarChartType);
    shape->setChartSubType(static_cast<ChartSubtype>(subtype));
    return shape;
}

ChartShape *ChartShapeFactory::createLineChart(KoDocumentResourceManager *documentResources, int subtype) const
{
    ChartShape* shape = static_cast<ChartShape*>(createDefaultShape(documentResources));
    shape->setChartType(LineChartType);
    shape->setChartSubType(static_cast<ChartSubtype>(subtype));
    return shape;
}

ChartShape *ChartShapeFactory::createAreaChart(KoDocumentResourceManager *documentResources, int subtype) const
{
    ChartShape* shape = static_cast<ChartShape*>(createDefaultShape(documentResources));
    shape->setChartType(AreaChartType);
    shape->setChartSubType(static_cast<ChartSubtype>(subtype));
    return shape;
}

ChartShape *ChartShapeFactory::createStockChart(KoDocumentResourceManager *documentResources, int subtype) const
{
    ChartShape* shape = new ChartShape(documentResources);
    shape->setChartType(StockChartType);
    shape->setChartSubType(CandlestickChartSubtype); // we create data for this by default, and switch below if needed
    ChartProxyModel *proxyModel = shape->proxyModel();

    // Fill cells with data.
    ChartTableModel  *chartData = new ChartTableModel();
    Table *internalTable = shape->tableSource()->add("local-data", chartData);
    Q_ASSERT(!shape->internalModel());
    // setInternalModel() assumes that chartData has already been added to shape->tableSource().
    shape->setInternalModel(chartData);
    // TODO (not implemented yet)
    // shape->tableSource()->setRenameOnNameClash(internalTable);
    chartData->setRowCount(4);
    chartData->setColumnCount(5);

    // Vertical header data (not used)
    chartData->setData(chartData->index(1, 0), i18n("Share A"));
    chartData->setData(chartData->index(2, 0), i18n("Share B"));
    chartData->setData(chartData->index(3, 0), i18n("Share C"));

    // Horizontal header data
    chartData->setData(chartData->index(0, 1), i18n("Open"));
    chartData->setData(chartData->index(0, 2), i18n("High"));
    chartData->setData(chartData->index(0, 3), i18n("Low"));
    chartData->setData(chartData->index(0, 4), i18n("Close"));

    QList<qreal> openValues; openValues << 10 << 15 << 20;
    QList<qreal> highValues; highValues << 12 << 15 << 30;
    QList<qreal> lowValues; lowValues << 6 << 13 << 20;
    QList<qreal> closeValues; closeValues << 7 << 13 << 30;
    int col = 1;

    // Open
    chartData->setData(chartData->index(1, col), openValues.at(0));
    chartData->setData(chartData->index(2, col), openValues.at(1));
    chartData->setData(chartData->index(3, col), openValues.at(2));
    ++col;

    // High
    chartData->setData(chartData->index(1, col), highValues.at(0));
    chartData->setData(chartData->index(2, col), highValues.at(1));
    chartData->setData(chartData->index(3, col), highValues.at(2));
    ++col;

    // Low
    chartData->setData(chartData->index(1, col), lowValues.at(0));
    chartData->setData(chartData->index(2, col), lowValues.at(1));
    chartData->setData(chartData->index(3, col), lowValues.at(2));
    ++col;

    // Close
    chartData->setData(chartData->index(1, col), closeValues.at(0));
    chartData->setData(chartData->index(2, col), closeValues.at(1));
    chartData->setData(chartData->index(3, col), closeValues.at(2));

    proxyModel->setFirstRowIsLabel(true);
    proxyModel->setFirstColumnIsLabel(true);
    proxyModel->reset(CellRegion(internalTable, QRect(1, 1, 5, 4)));

    shape->plotArea()->yAxis()->title()->setVisible(false);
    shape->plotArea()->xAxis()->title()->setVisible(false);
    shape->legend()->setVisible(false);

    shape->setChartSubType(static_cast<ChartSubtype>(subtype), true);

    shape->layout()->scheduleRelayout();
    shape->layout()->layout();

    return shape;
}

ChartShape *ChartShapeFactory::createPieChart(KoDocumentResourceManager *documentResources) const
{
    ChartShape* shape = static_cast<ChartShape*>(createDefaultShape(documentResources));
    shape->setChartType(CircleChartType);
    shape->setChartSubType(NoChartSubtype);
    return shape;
}

ChartShape *ChartShapeFactory::createRingChart(KoDocumentResourceManager *documentResources) const
{
    ChartShape* shape = static_cast<ChartShape*>(createDefaultShape(documentResources));
    shape->setChartType(RingChartType);
    shape->setChartSubType(NoChartSubtype);
    return shape;
}

ChartShape *ChartShapeFactory::createBubbleChart(KoDocumentResourceManager *documentResources) const
{
    ChartShape* shape = new ChartShape(documentResources);
    shape->setChartType(BubbleChartType);
    shape->setChartSubType(NoChartSubtype);
    ChartProxyModel *proxyModel = shape->proxyModel();

    // Fill cells with data.
    ChartTableModel  *chartData = new ChartTableModel();
    Table *internalTable = shape->tableSource()->add("local-data", chartData);
    Q_ASSERT(!shape->internalModel());
    // setInternalModel() assumes that chartData has already been added to shape->tableSource().
    shape->setInternalModel(chartData);
    // TODO (not implemented yet)
    // shape->tableSource()->setRenameOnNameClash(internalTable);
    chartData->setRowCount(4);
    chartData->setColumnCount(5);

    // Vertical header data (not used)
    chartData->setData(chartData->index(1, 0), i18n("January"));
    chartData->setData(chartData->index(2, 0), i18n("July"));
    chartData->setData(chartData->index(3, 0), i18n("December"));

    // Horizontal header data
    chartData->setData(chartData->index(0, 1), i18n("Column %1", 1));
    chartData->setData(chartData->index(0, 2), i18n("Column %1", 2));
    chartData->setData(chartData->index(0, 3), i18n("Column %1", 3)); // not used
    chartData->setData(chartData->index(0, 4), i18n("Column %1", 4)); // not used

    // First row
    chartData->setData(chartData->index(1, 1), 4.7); // Y value series 1
    chartData->setData(chartData->index(1, 2), 3.4); // bubble size series 1
    chartData->setData(chartData->index(1, 3), 1.2); // Y value series 2
    chartData->setData(chartData->index(1, 4), 8.4); // bubble size series 2

    // Second row
    chartData->setData(chartData->index(2, 1), 2.1);
    chartData->setData(chartData->index(2, 2), 6.5);
    chartData->setData(chartData->index(2, 3), 4.9);
    chartData->setData(chartData->index(2, 4), 3.5);

    // Third row
    chartData->setData(chartData->index(3, 1), 7.9);
    chartData->setData(chartData->index(3, 2), 1.5);
    chartData->setData(chartData->index(3, 3), 4.6);
    chartData->setData(chartData->index(3, 4), 4.3);

    proxyModel->setFirstRowIsLabel(true);
    proxyModel->setFirstColumnIsLabel(true);
    proxyModel->reset(CellRegion(internalTable, QRect(1, 1, 5, 4)));

    shape->plotArea()->xAxis()->title()->setVisible(false);
    shape->plotArea()->yAxis()->setTitleText(i18n("Growth"));

    shape->layout()->scheduleRelayout();
    shape->layout()->layout();

    return shape;
}

ChartShape *ChartShapeFactory::createScatterChart(KoDocumentResourceManager *documentResources) const
{
    ChartShape* shape = static_cast<ChartShape*>(createDefaultShape(documentResources));
    shape->setChartType(ScatterChartType);
    shape->setChartSubType(NoChartSubtype);
    return shape;
}

ChartShape *ChartShapeFactory::createRadarChart(KoDocumentResourceManager *documentResources) const
{
    ChartShape* shape = new ChartShape(documentResources);
    shape->setChartType(RadarChartType);
    shape->setChartSubType(NoChartSubtype);
    radarData(shape);
    return shape;
}

ChartShape *ChartShapeFactory::createFilledRadarChart(KoDocumentResourceManager *documentResources) const
{
    ChartShape* shape = new ChartShape(documentResources);
    shape->setChartType(FilledRadarChartType);
    shape->setChartSubType(NoChartSubtype);
    radarData(shape);
    return shape;
}

void ChartShapeFactory::radarData(ChartShape *shape) const
{
    ChartProxyModel *proxyModel = shape->proxyModel();

    // Fill cells with data.
    ChartTableModel  *chartData = new ChartTableModel();
    Table *internalTable = shape->tableSource()->add("local-data", chartData);
    Q_ASSERT(!shape->internalModel());
    // setInternalModel() assumes that chartData has already been added to shape->tableSource().
    shape->setInternalModel(chartData);
    // TODO (not implemented yet)
    // shape->tableSource()->setRenameOnNameClash(internalTable);
    chartData->setRowCount(4);
    chartData->setColumnCount(3);

    // Vertical header data (not used)
    chartData->setData(chartData->index(1, 0), i18n("January"));
    chartData->setData(chartData->index(2, 0), i18n("July"));
    chartData->setData(chartData->index(3, 0), i18n("December"));

    // Horizontal header data
    chartData->setData(chartData->index(0, 1), i18n("Column %1", 1));
    chartData->setData(chartData->index(0, 2), i18n("Column %1", 2));

    // First row
    chartData->setData(chartData->index(1, 1), 10);
    chartData->setData(chartData->index(1, 2), 3);
    chartData->setData(chartData->index(1, 3), 6);

    // Second row
    chartData->setData(chartData->index(2, 1), 4);
    chartData->setData(chartData->index(2, 2), 8);
    chartData->setData(chartData->index(2, 3), 10);

    // Third row
    chartData->setData(chartData->index(3, 1), 5);
    chartData->setData(chartData->index(3, 2), 10);
    chartData->setData(chartData->index(3, 3), 13);

    proxyModel->setFirstRowIsLabel(true);
    proxyModel->setFirstColumnIsLabel(true);
    proxyModel->reset(CellRegion(internalTable, QRect(1, 1, chartData->columnCount(), chartData->rowCount())));
}

#include "ChartShapeFactory.moc"
