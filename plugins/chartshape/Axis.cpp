/* This file is part of the KDE project

   Copyright 2007 Johannes Simon <johannes.simon@gmail.com>
   Copyright 2009 Inge Wallin    <inge@lysator.liu.se>
   Copyright 2017 Dag Andersen   <danders@get2net.dk>

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
#include "Axis.h"

// Qt
#include <QList>
#include <QString>
#include <QTextDocument>
#include <QPointer>

// Calligra
#include <KoShapeLoadingContext.h>
#include <KoShapeSavingContext.h>
#include <KoShapeRegistry.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>
#include <KoGenStyles.h>
#include <KoXmlNS.h>
#include <KoTextShapeData.h>
#include <KoOdfStylesReader.h>
#include <KoUnit.h>
#include <KoStyleStack.h>
#include <KoOdfLoadingContext.h>
#include <KoCharacterStyle.h>
#include <KoOdfGraphicStyles.h>
#include <KoOdfWorkaround.h>
#include <KoTextDocumentLayout.h>
#include <KoOdfNumberStyles.h>

// KChart
#include <KChartChart>
#include <KChartLegend>
#include <KChartCartesianAxis>
#include <KChartCartesianCoordinatePlane>
#include <KChartRadarCoordinatePlane>
#include <KChartGridAttributes>
#include <KChartBarDiagram>
#include <KChartLineDiagram>
#include <KChartPieDiagram>
#include <KChartPlotter>
#include <KChartStockDiagram>
#include <KChartRingDiagram>
#include <KChartRadarDiagram>
#include <KChartBarAttributes>
#include <KChartPieAttributes>
#include <KChartThreeDBarAttributes>
#include <KChartThreeDPieAttributes>
#include <KChartThreeDLineAttributes>
#include <KChartBackgroundAttributes>
#include <KChartRulerAttributes>
#include <kchart_version.h>

// KoChart
#include "PlotArea.h"
#include "KChartModel.h"
#include "DataSet.h"
#include "Legend.h"
#include "KChartConvertions.h"
#include "ChartProxyModel.h"
#include "TextLabelDummy.h"
#include "ChartLayout.h"
#include "OdfLoadingHelper.h"
#include "OdfHelper.h"
#include "ChartDebug.h"


using namespace KoChart;

class Axis::Private
{
public:
    Private(Axis *axis, AxisDimension dim);
    ~Private();

    void adjustAllDiagrams();
    /// Updates the axis position in the chart's layout
    /// FIXME: We should instead implement a generic layout position method
    /// and have the layout find out about our position when it changes.
    void updatePosition();

    void registerDiagram(KChart::AbstractDiagram *diagram);

    KChart::AbstractDiagram *getDiagramAndCreateIfNeeded(ChartType chartType);
    KChart::AbstractDiagram *getDiagram(ChartType chartType);
    void deleteDiagram(ChartType chartType);
    void deleteDiagram(KChart::AbstractDiagram *diagram);

    void restoreDiagrams();

    void createBarDiagram();
    void createLineDiagram();
    void createAreaDiagram();
    void createCircleDiagram();
    void createRingDiagram();
    void createRadarDiagram(bool filled);
    void createScatterDiagram();
    void createStockDiagram();
    void createBubbleDiagram();
    void createSurfaceDiagram();
    void createGanttDiagram();
    void applyAttributesToDataSet(DataSet* set, ChartType newCharttype);

    // Pointer to Axis that owns this Private instance
    Axis * const q;

    PlotArea *plotArea;

    const AxisDimension dimension;

    KoShape *title;
    TextLabelData *titleData;

    /// FIXME: Unused variable 'id', including id() getter
    QString id;
    QList<DataSet*> dataSets;
    qreal majorInterval;
    int minorIntervalDivisor;
    bool showInnerMinorTicks;
    bool showOuterMinorTicks;
    bool showInnerMajorTicks;
    bool showOuterMajorTicks;
    bool logarithmicScaling;
    bool showMajorGrid;
    bool showMinorGrid;
    bool useAutomaticMajorInterval;
    bool useAutomaticMinorInterval;
    bool useAutomaticMinimumRange;
    bool useAutomaticMaximumRange;

    KChart::CartesianAxis            *const kdAxis;
    KChart::CartesianCoordinatePlane *kdPlane;
    KChart::PolarCoordinatePlane     *kdPolarPlane;
    KChart::RadarCoordinatePlane     *kdRadarPlane;
    KoOdfNumberStyles::NumericStyleFormat *numericStyleFormat;

    QList<QPointer<KChart::AbstractCartesianDiagram> > diagrams;

    QPointer<KChart::BarDiagram> kdBarDiagram;
    QPointer<KChart::LineDiagram> kdLineDiagram;
    QPointer<KChart::LineDiagram> kdAreaDiagram;
    QPointer<KChart::PieDiagram>  kdCircleDiagram;
    QPointer<KChart::RingDiagram>  kdRingDiagram;
    QPointer<KChart::RadarDiagram> kdRadarDiagram;
    QPointer<KChart::Plotter>     kdScatterDiagram;
    QPointer<KChart::StockDiagram> kdStockDiagram;
    QPointer<KChart::Plotter>  kdBubbleDiagram;
    // FIXME BUG: Somehow we need to visualize something for these
    //            missing chart types.  We have some alternatives:
    //            1. Show an empty area
    //            2. Show a text "unsupported chart type"
    //            3. Exchange for something else, e.g. a bar chart.
    //            ... More?
    //
    // NOTE: Whatever we do, we should always store the data so that
    //       it can be saved back into the file for a perfect
    //       roundtrip.
    QPointer<KChart::BarDiagram> kdSurfaceDiagram;
    QPointer<KChart::BarDiagram> kdGanttDiagram;

    ChartType     plotAreaChartType;
    ChartSubtype  plotAreaChartSubType;

    // If KChart::LineDiagram::centerDataPoints() property is set to true,
    // the data points drawn in a line (i.e., also an area) diagram start at
    // an offset of 0.5, that is, in the middle of a column in the diagram.
    // Set flag to true if at least one dataset is attached to this axis
    // that belongs to a horizontal bar chart
    bool centerDataPoints;

    int gapBetweenBars;
    int gapBetweenSets;

    // TODO: Save
    // See ODF v1.2 $19.12 (chart:display-label)
    bool showLabels;
    bool showOverlappingDataLabels;

    bool isVisible;
    QString name;

    QString axisPosition;
    QString axisLabelsPosition;

};

class CartesianAxis : public KChart::CartesianAxis
{
public:
    CartesianAxis(KoChart::Axis *_axis) : KChart::CartesianAxis(), axis(_axis) {}
    virtual ~CartesianAxis() {}
    const QString customizedLabel(const QString& label) const override {
        if (KoOdfNumberStyles::NumericStyleFormat *n = axis->numericStyleFormat())
            return KoOdfNumberStyles::format(label, *n);
        return label;
    }
private:
    KoChart::Axis *axis;
};

Axis::Private::Private(Axis *axis, AxisDimension dim)
    : q(axis)
    , dimension(dim)
    , kdAxis(new CartesianAxis(axis))
    , kdPlane(0)
    , kdPolarPlane(0)
    , kdRadarPlane(0)
    , numericStyleFormat(0)
{
    centerDataPoints = false;

    gapBetweenBars = 0;
    gapBetweenSets = 100;

    isVisible = true;

    useAutomaticMajorInterval = true;
    useAutomaticMinorInterval = true;
    useAutomaticMinimumRange = true;
    useAutomaticMaximumRange = true;

    majorInterval = 2;
    minorIntervalDivisor = 1;

    showMajorGrid = false;
    showMinorGrid = false;

    logarithmicScaling = false;

    showInnerMinorTicks = false;
    showOuterMinorTicks = false;
    showInnerMajorTicks = false;
    showOuterMajorTicks = true;

    showOverlappingDataLabels = false;
    showLabels = true;

    title = 0;
    titleData = 0;

    KChart::RulerAttributes attr = kdAxis->rulerAttributes();
    attr.setShowRulerLine(true);
    attr.setRulerLinePen(QPen());
    kdAxis->setRulerAttributes(attr);
}

Axis::Private::~Private()
{
    Q_ASSERT(plotArea);

    q->removeAxisFromDiagrams();

    delete kdBarDiagram;
    delete kdLineDiagram;
    delete kdAreaDiagram;
    delete kdCircleDiagram;
    delete kdRingDiagram;
    delete kdRadarDiagram;
    delete kdScatterDiagram;
    delete kdStockDiagram;
    delete kdBubbleDiagram;
    delete kdSurfaceDiagram;
    delete kdGanttDiagram;

    delete numericStyleFormat;

    delete kdAxis;

    foreach(DataSet *dataSet, dataSets)
        dataSet->setAttachedAxis(0);
}

void Axis::Private::registerDiagram(KChart::AbstractDiagram *diagram)
{
    QObject::connect(plotArea->proxyModel(), SIGNAL(columnsInserted(QModelIndex,int,int)),
                     diagram->model(), SLOT(slotColumnsInserted(QModelIndex,int,int)));

    QObject::connect(diagram, SIGNAL(propertiesChanged()),
                     plotArea, SLOT(plotAreaUpdate()));
    QObject::connect(diagram, SIGNAL(layoutChanged(AbstractDiagram*)),
                     plotArea, SLOT(plotAreaUpdate()));
    QObject::connect(diagram, SIGNAL(modelsChanged()),
                     plotArea, SLOT(plotAreaUpdate()));
    QObject::connect(diagram, SIGNAL(dataHidden()),
                     plotArea, SLOT(plotAreaUpdate()));
}

KChart::AbstractDiagram *Axis::Private::getDiagramAndCreateIfNeeded(ChartType chartType)
{
    KChart::AbstractDiagram *diagram = 0;

    switch (chartType) {
    case BarChartType:
        if (!kdBarDiagram)
            createBarDiagram();
        diagram = kdBarDiagram;
        break;
    case LineChartType:
        if (!kdLineDiagram)
            createLineDiagram();
        diagram = kdLineDiagram;
        break;
    case AreaChartType:
        if (!kdAreaDiagram)
            createAreaDiagram();
        diagram = kdAreaDiagram;
        break;
    case CircleChartType:
        if (!kdCircleDiagram)
            createCircleDiagram();
        diagram = kdCircleDiagram;
        break;
    case RingChartType:
        if (!kdRingDiagram)
            createRingDiagram();
        diagram = kdRingDiagram;
        break;
    case RadarChartType:
    case FilledRadarChartType:
        if (!kdRadarDiagram)
            createRadarDiagram(chartType == FilledRadarChartType);
        diagram = kdRadarDiagram;
        break;
    case ScatterChartType:
        if (!kdScatterDiagram)
            createScatterDiagram();
        diagram = kdScatterDiagram;
        break;
    case StockChartType:
        if (!kdStockDiagram)
            createStockDiagram();
        diagram = kdStockDiagram;
        break;
    case BubbleChartType:
        if (!kdBubbleDiagram)
            createBubbleDiagram();
        diagram = kdBubbleDiagram;
        break;
    case SurfaceChartType:
        if (!kdSurfaceDiagram)
            createSurfaceDiagram();
        diagram = kdSurfaceDiagram;
        break;
    case GanttChartType:
        if (!kdGanttDiagram)
            createGanttDiagram();
        diagram = kdGanttDiagram;
        break;
    default:
        ;
    }
    diagram->setObjectName(q->name()); // for debug
    adjustAllDiagrams();
    debugChartAxis<<q->name()<<"diagram"<<diagram<<"for"<<chartType;
    return diagram;
}

/**
 * Returns currently used internal KChart diagram for the specified chart type
 */
KChart::AbstractDiagram *Axis::Private::getDiagram(ChartType chartType)
{
    switch (chartType) {
        case BarChartType:
            return kdBarDiagram;
        case LineChartType:
            return kdLineDiagram;
        case AreaChartType:
            return kdAreaDiagram;
        case CircleChartType:
            return kdCircleDiagram;
        case RingChartType:
            return kdRingDiagram;
        case RadarChartType:
        case FilledRadarChartType:
            return kdRadarDiagram;
        case ScatterChartType:
            return kdScatterDiagram;
        case StockChartType:
            return kdStockDiagram;
        case BubbleChartType:
            return kdBubbleDiagram;
        case SurfaceChartType:
            return kdSurfaceDiagram;
        case GanttChartType:
            return kdGanttDiagram;
        case LastChartType:
            return 0;
        // Compiler warning for unhandled chart type is intentional.
    }
    Q_ASSERT(!"Unhandled chart type");
    return 0;
}

void Axis::Private::deleteDiagram(KChart::AbstractDiagram *diagram)
{
    Q_ASSERT(diagram);
    if (diagram->coordinatePlane()) {
        diagram->coordinatePlane()->takeDiagram(diagram);
    }
    delete diagram;
    adjustAllDiagrams();
}

void Axis::Private::deleteDiagram(ChartType chartType)
{
    KChart::AbstractDiagram *diagram = getDiagram(chartType);
    if (diagram) {
        deleteDiagram(diagram);
    }
}


void Axis::Private::createBarDiagram()
{
    Q_ASSERT(kdBarDiagram == 0);

    kdBarDiagram = new KChart::BarDiagram(plotArea->kdChart(), kdPlane);
    KChartModel *model = new KChartModel(plotArea, kdBarDiagram);
    kdBarDiagram->setModel(model);
    registerDiagram(kdBarDiagram);

    // By 'vertical', KChart means the orientation of a chart's bars,
    // not the orientation of the x axis.
    kdBarDiagram->setOrientation(plotArea->isVertical() ? Qt::Horizontal : Qt::Vertical);
    kdBarDiagram->setPen(QPen(Qt::black, 0.0));

    kdBarDiagram->setAllowOverlappingDataValueTexts(showOverlappingDataLabels);

    if (plotAreaChartSubType == StackedChartSubtype)
        kdBarDiagram->setType(KChart::BarDiagram::Stacked);
    else if (plotAreaChartSubType == PercentChartSubtype) {
        kdBarDiagram->setType(KChart::BarDiagram::Percent);
        kdBarDiagram->setUnitSuffix("%", kdBarDiagram->orientation());
    }

    if (isVisible) {
        kdBarDiagram->addAxis(kdAxis);
        q->registerDiagram(kdBarDiagram);
    }
    kdPlane->addDiagram(kdBarDiagram);

    Q_ASSERT(plotArea);
    foreach (Axis *axis, plotArea->axes()) {
        if (axis->isVisible() && axis->dimension() == XAxisDimension) {
            kdBarDiagram->addAxis(axis->kdAxis());
            axis->registerDiagram(kdBarDiagram);
        }
    }

    // Set default bar diagram attributes
    q->setGapBetweenBars(gapBetweenBars);
    q->setGapBetweenSets(gapBetweenSets);

    // Propagate existing settings
    KChart::ThreeDBarAttributes attributes(kdBarDiagram->threeDBarAttributes());
    attributes.setEnabled(plotArea->isThreeD());
    attributes.setThreeDBrushEnabled(plotArea->isThreeD());
    kdBarDiagram->setThreeDBarAttributes(attributes);

    q->plotAreaIsVerticalChanged();

    plotArea->parent()->legend()->kdLegend()->addDiagram(kdBarDiagram);
}

void Axis::Private::createLineDiagram()
{
    Q_ASSERT(kdLineDiagram == 0);

    kdLineDiagram = new KChart::LineDiagram(plotArea->kdChart(), kdPlane);
    KChartModel *model = new KChartModel(plotArea, kdLineDiagram);
    kdLineDiagram->setModel(model);
    registerDiagram(kdLineDiagram);

    kdLineDiagram->setAllowOverlappingDataValueTexts(showOverlappingDataLabels);

    if (plotAreaChartSubType == StackedChartSubtype)
        kdLineDiagram->setType(KChart::LineDiagram::Stacked);
    else if (plotAreaChartSubType == PercentChartSubtype)
        kdLineDiagram->setType(KChart::LineDiagram::Percent);

    if (isVisible) {
        kdLineDiagram->addAxis(kdAxis);
        q->registerDiagram(kdLineDiagram);
    }
    kdPlane->addDiagram(kdLineDiagram);

    Q_ASSERT(plotArea);
    foreach (Axis *axis, plotArea->axes()) {
        if (axis->isVisible() && axis->dimension() == XAxisDimension) {
            kdLineDiagram->addAxis(axis->kdAxis());
            axis->registerDiagram(kdLineDiagram);
        }
    }

    // Propagate existing settings
    KChart::ThreeDLineAttributes attributes(kdLineDiagram->threeDLineAttributes());
    attributes.setEnabled(plotArea->isThreeD());
    attributes.setThreeDBrushEnabled(plotArea->isThreeD());
    kdLineDiagram->setThreeDLineAttributes(attributes);

    KChart::LineAttributes lineAttr = kdLineDiagram->lineAttributes();
    lineAttr.setMissingValuesPolicy(KChart::LineAttributes::MissingValuesHideSegments);
    kdLineDiagram->setLineAttributes(lineAttr);

    plotArea->parent()->legend()->kdLegend()->addDiagram(kdLineDiagram);
}

void Axis::Private::createAreaDiagram()
{
    Q_ASSERT(kdAreaDiagram == 0);

    kdAreaDiagram = new KChart::LineDiagram(plotArea->kdChart(), kdPlane);
    KChartModel *model = new KChartModel(plotArea, kdAreaDiagram);
    kdAreaDiagram->setModel(model);
    registerDiagram(kdAreaDiagram);
    KChart::LineAttributes attr = kdAreaDiagram->lineAttributes();
    // Draw the area under the lines. This makes this diagram an area chart.
    attr.setDisplayArea(true);
    kdAreaDiagram->setLineAttributes(attr);
    kdAreaDiagram->setPen(QPen(Qt::black, 0.0));
    // KD Chart by default draws the first data set as last line in a normal
    // line diagram, we however want the first series to appear in front.
    kdAreaDiagram->setReverseDatasetOrder(true);

    kdAreaDiagram->setAllowOverlappingDataValueTexts(showOverlappingDataLabels);

    if (plotAreaChartSubType == StackedChartSubtype)
        kdAreaDiagram->setType(KChart::LineDiagram::Stacked);
    else if (plotAreaChartSubType == PercentChartSubtype)
    {
        kdAreaDiagram->setType(KChart::LineDiagram::Percent);
        kdAreaDiagram->setUnitSuffix("%", Qt::Vertical);
    }

    if (isVisible) {
        kdAreaDiagram->addAxis(kdAxis);
        q->registerDiagram(kdAreaDiagram);
    }
    kdPlane->addDiagram(kdAreaDiagram);

    Q_ASSERT(plotArea);
    foreach (Axis *axis, plotArea->axes()) {
        if (axis->isVisible() && axis->dimension() == XAxisDimension) {
            kdAreaDiagram->addAxis(axis->kdAxis());
            axis->registerDiagram(kdAreaDiagram);
        }
    }

    // Propagate existing settings
    KChart::ThreeDLineAttributes attributes(kdAreaDiagram->threeDLineAttributes());
    attributes.setEnabled(plotArea->isThreeD());
    attributes.setThreeDBrushEnabled(plotArea->isThreeD());
    kdAreaDiagram->setThreeDLineAttributes(attributes);

    plotArea->parent()->legend()->kdLegend()->addDiagram(kdAreaDiagram);
}

void Axis::Private::createCircleDiagram()
{
    Q_ASSERT(kdCircleDiagram == 0);

    kdCircleDiagram = new KChart::PieDiagram(plotArea->kdChart(), kdPolarPlane);
    KChartModel *model = new KChartModel(plotArea, kdCircleDiagram);
    kdCircleDiagram->setModel(model);
    registerDiagram(kdCircleDiagram);

    model->setDataDirection(Qt::Horizontal);

    plotArea->parent()->legend()->kdLegend()->addDiagram(kdCircleDiagram);
    kdPolarPlane->addDiagram(kdCircleDiagram);

    // Propagate existing settings
    KChart::ThreeDPieAttributes attributes(kdCircleDiagram->threeDPieAttributes());
    attributes.setEnabled(plotArea->isThreeD());
    attributes.setThreeDBrushEnabled(plotArea->isThreeD());
    kdCircleDiagram->setThreeDPieAttributes(attributes);

    // Initialize with default values that are specified in PlotArea
    // Note: KChart takes an int here, though ODF defines the offset to be a double.
    kdPolarPlane->setStartPosition((int)plotArea->angleOffset());
}

void Axis::Private::createRingDiagram()
{
    Q_ASSERT(kdRingDiagram == 0);

    kdRingDiagram = new KChart::RingDiagram(plotArea->kdChart(), kdPolarPlane);
    KChartModel *model = new KChartModel(plotArea, kdRingDiagram);
    kdRingDiagram->setModel(model);
    registerDiagram(kdRingDiagram);

    model->setDataDirection(Qt::Horizontal);

    plotArea->parent()->legend()->kdLegend()->addDiagram(kdRingDiagram);
    kdPolarPlane->addDiagram(kdRingDiagram);

    // Propagate existing settings
    KChart::ThreeDPieAttributes attributes(kdRingDiagram->threeDPieAttributes());
    attributes.setEnabled(plotArea->isThreeD());
    attributes.setThreeDBrushEnabled(plotArea->isThreeD());
    kdRingDiagram->setThreeDPieAttributes(attributes);

    // Initialize with default values that are specified in PlotArea
    // Note: KChart takes an int here, though ODF defines the offset to be a double.
    kdPolarPlane->setStartPosition((int)plotArea->angleOffset());
}

void Axis::Private::createRadarDiagram(bool filled)
{
    Q_ASSERT(kdRadarDiagram == 0);

    //kdRadarDiagramModel->setDataDimensions(2);
    //kdRadarDiagramModel->setDataDirection(Qt::Horizontal);

    kdRadarDiagram = new KChart::RadarDiagram(plotArea->kdChart(), kdRadarPlane);
    KChartModel *model = new KChartModel(plotArea, kdRadarDiagram);
    kdRadarDiagram->setModel(model);
    registerDiagram(kdRadarDiagram);

    kdRadarDiagram->setCloseDatasets(true);

    if (filled) {
        // Don't use a solid fill of 1.0 but a more transparent one so the
        // grid and the data-value-labels are still visible plus it provides
        // a better look (other areas can still be seen) even if it's slightly
        // different from what OO.org does.
        kdRadarDiagram->setFillAlpha(0.4);
    }

#if 0  // Stacked and Percent not supported by KChart.
    if (plotAreaChartSubType == StackedChartSubtype)
        kdRadarDiagram->setType(KChart::PolarDiagram::Stacked);
    else if (plotAreaChartSubType == PercentChartSubtype)
        kdRadarDiagram->setType(KChart::PolarDiagram::Percent);
#endif
    plotArea->parent()->legend()->kdLegend()->addDiagram(kdRadarDiagram);
    kdRadarPlane->addDiagram(kdRadarDiagram);
}

void Axis::Private::createScatterDiagram()
{
    Q_ASSERT(kdScatterDiagram == 0);
    Q_ASSERT(plotArea);

    kdScatterDiagram = new KChart::Plotter(plotArea->kdChart(), kdPlane);
    KChartModel *model = new KChartModel(plotArea, kdScatterDiagram);
    kdScatterDiagram->setModel(model);
    registerDiagram(kdScatterDiagram);

    model->setDataDimensions(2);

    kdScatterDiagram->setPen(Qt::NoPen);

    if (isVisible) {
        kdScatterDiagram->addAxis(kdAxis);
        q->registerDiagram(kdScatterDiagram);
    }
    kdPlane->addDiagram(kdScatterDiagram);


    foreach (Axis *axis, plotArea->axes()) {
        if (axis->isVisible() && axis->dimension() == XAxisDimension) {
            kdScatterDiagram->addAxis(axis->kdAxis());
            axis->registerDiagram(kdScatterDiagram);
        }
    }

    // Propagate existing settings
    KChart::ThreeDLineAttributes attributes(kdScatterDiagram->threeDLineAttributes());
    attributes.setEnabled(plotArea->isThreeD());
    attributes.setThreeDBrushEnabled(plotArea->isThreeD());
    kdScatterDiagram->setThreeDLineAttributes(attributes);

    plotArea->parent()->legend()->kdLegend()->addDiagram(kdScatterDiagram);
}

void Axis::Private::createStockDiagram()
{
    Q_ASSERT(kdStockDiagram == 0);

    kdStockDiagram = new KChart::StockDiagram(plotArea->kdChart(), kdPlane);
    KChartModel *model = new KChartModel(plotArea, kdStockDiagram);
    kdStockDiagram->setModel(model);
    switch (plotAreaChartSubType) {
        case HighLowCloseChartSubtype:
            kdStockDiagram->setType(KChart::StockDiagram::HighLowClose);
            break;
        case OpenHighLowCloseChartSubtype:
            kdStockDiagram->setType(KChart::StockDiagram::OpenHighLowClose);
            break;
        case CandlestickChartSubtype:
            kdStockDiagram->setType(KChart::StockDiagram::Candlestick);
            break;
    }
    registerDiagram(kdStockDiagram);

    model->setDataDimensions(numDimensions(StockChartType));

#if 0  // Stacked and Percent not supported by KChart.
    if (plotAreaChartSubType == StackedChartSubtype)
        kdStockDiagram->setType(KChart::StockDiagram::Stacked);
    else if (plotAreaChartSubType == PercentChartSubtype)
        kdStockDiagram->setType(KChart::StockDiagram::Percent);
#endif

    if (isVisible) {
        kdStockDiagram->addAxis(kdAxis);
        q->registerDiagram(kdStockDiagram);
    }
    kdPlane->addDiagram(kdStockDiagram);

    Q_ASSERT(plotArea);
    foreach (Axis *axis, plotArea->axes()) {
        if (axis->isVisible() && axis->dimension() == XAxisDimension) {
            kdStockDiagram->addAxis(axis->kdAxis());
            axis->registerDiagram(kdStockDiagram);
        }
    }

    plotArea->parent()->legend()->kdLegend()->addDiagram(kdStockDiagram);
    q->updateKChartStockAttributes();
}

void Axis::Private::createBubbleDiagram()
{
    Q_ASSERT(kdBubbleDiagram == 0);
    Q_ASSERT(plotArea);

    kdBubbleDiagram = new KChart::Plotter(plotArea->kdChart(), kdPlane);
    KChartModel *model = new KChartModel(plotArea, kdBubbleDiagram);
    kdBubbleDiagram->setModel(model);
    registerDiagram(kdBubbleDiagram);

    model->setDataDimensions(2);

    kdPlane->addDiagram(kdBubbleDiagram);

    foreach (Axis *axis, plotArea->axes()) {
        //if (axis->dimension() == XAxisDimension)
        if (axis->isVisible() && axis->dimension() == XAxisDimension) {
            kdBubbleDiagram->addAxis(axis->kdAxis());
            q->registerDiagram(kdBubbleDiagram);
        }
    }

     // disable the connecting line
    KChart::LineAttributes la = kdBubbleDiagram->lineAttributes();
    la.setVisible(false);
    kdBubbleDiagram->setLineAttributes(la);

    plotArea->parent()->legend()->kdLegend()->addDiagram(kdBubbleDiagram);
}

void Axis::Private::createSurfaceDiagram()
{
    Q_ASSERT(!kdSurfaceDiagram);

    // This is a so far a by KChart unsupported chart type.
    // Fall back to bar diagram for now.
    kdSurfaceDiagram = new KChart::BarDiagram(plotArea->kdChart(), kdPlane);
    KChartModel *model = new KChartModel(plotArea, kdSurfaceDiagram);
    kdSurfaceDiagram->setModel(model);
    registerDiagram(kdSurfaceDiagram);
    plotArea->parent()->legend()->kdLegend()->addDiagram(kdSurfaceDiagram);
    kdPlane->addDiagram(kdSurfaceDiagram);
}

void Axis::Private::createGanttDiagram()
{
    // This is a so far a by KChart unsupported chart type (through KDGantt got merged into KChart with 2.3)
    Q_ASSERT(!kdGanttDiagram);

    // This is a so far a by KChart unsupported chart type.
    // Fall back to bar diagram for now.
    kdGanttDiagram = new KChart::BarDiagram(plotArea->kdChart(), kdPlane);
    KChartModel *model = new KChartModel(plotArea, kdGanttDiagram);
    kdGanttDiagram->setModel(model);
    registerDiagram(kdGanttDiagram);
    plotArea->parent()->legend()->kdLegend()->addDiagram(kdGanttDiagram);
    kdPlane->addDiagram(kdGanttDiagram);
}

/**
 * Automatically adjusts the diagram so that all currently displayed
 * diagram types fit together.
 */
void Axis::Private::adjustAllDiagrams()
{
    // If at least one dataset is attached that belongs to a
    // horizontal bar chart, set centerDataPoints to true.
    centerDataPoints = kdBarDiagram != 0;
    if (kdLineDiagram)
        kdLineDiagram->setCenterDataPoints(centerDataPoints);
    if (kdAreaDiagram)
        kdAreaDiagram->setCenterDataPoints(centerDataPoints);
}


// ================================================================
//                             class Axis

// FIXME: make it possible to create an axis without parent, and
//        when it is removed, actually remove it from parent (signals and all)

Axis::Axis(PlotArea *parent, AxisDimension dimension)
    : d(new Private(this, dimension))
{
    Q_ASSERT(parent);

    parent->addAxis(this);

    d->plotArea = parent;
    d->kdAxis->setObjectName(name());
    KChart::BackgroundAttributes batt(d->kdAxis->backgroundAttributes());
    batt.setBrush(QBrush(Qt::white));
    d->kdAxis->setBackgroundAttributes(batt);
    setFontSize(8.0); // also sets MeasureCalculationModeAbsolute
    d->kdPlane = parent->kdCartesianPlane(this);
    d->kdPolarPlane = parent->kdPolarPlane();
    d->kdRadarPlane = parent->kdRadarPlane();

    d->plotAreaChartType    = d->plotArea->chartType();
    d->plotAreaChartSubType = d->plotArea->chartSubType();

    setOdfAxisPosition("start");
    setOdfAxisLabelsPosition("near-axis");

    KoShapeFactoryBase *textShapeFactory = KoShapeRegistry::instance()->value(TextShapeId);
    if (textShapeFactory)
        d->title = textShapeFactory->createDefaultShape(parent->parent()->resourceManager());
    if (d->title) {
        d->titleData = qobject_cast<TextLabelData*>(d->title->userData());
        if (d->titleData == 0) {
            d->titleData = new TextLabelData;
            d->title->setUserData(d->titleData);
        }

        QFont font = d->titleData->document()->defaultFont();
        font.setPointSizeF(9);
        d->titleData->document()->setDefaultFont(font);
    }
    else {
        d->title = new TextLabelDummy;
        d->titleData = new TextLabelData;
        KoTextDocumentLayout *documentLayout = new KoTextDocumentLayout(d->titleData->document());
        d->titleData->document()->setDocumentLayout(documentLayout);
        d->title->setUserData(d->titleData);
    }
    d->title->setSize(QSizeF(CM_TO_POINT(3), CM_TO_POINT(0.75)));

    d->plotArea->parent()->addShape(d->title);
    d->plotArea->parent()->setClipped(d->title, true);
    d->plotArea->parent()->setInheritsTransform(d->title, true);
    d->title->setDeletable(false);
    d->title->setZIndex(5);
    d->title->setToolDelegates(QSet<KoShape*>()<<parent->parent()<<d->title); // Enable chart tool
    d->titleData->setResizeMethod(KoTextShapeDataBase::AutoResize);
    d->title->setAdditionalStyleAttribute("chart:auto-position", "true");
    d->title->setAllowedInteraction(KoShape::ShearingAllowed, false);
    d->title->setAllowedInteraction(KoShape::RotationAllowed, false);
    d->title->setAllowedInteraction(KoShape::ResizeAllowed, false);
    d->title->setVisible(false); // Needed to avoid problems when creating secondary axes (Axis creation needs review/refactoring)

    connect(d->plotArea, SIGNAL(angleOffsetChanged(qreal)), this, SLOT(setAngleOffset(qreal)));
    connect(d->plotArea, SIGNAL(holeSizeChanged(qreal)), this, SLOT(setHoleSize(qreal)));

    d->updatePosition();
}

Axis::~Axis()
{
    Q_ASSERT(d->plotArea);
    d->plotArea->parent()->KoShapeContainer::removeShape(d->title);

    Q_ASSERT(d->title);
    delete d->title;

    delete d;
}

PlotArea* Axis::plotArea() const
{
    return d->plotArea;
}

KoShape *Axis::title() const
{
    return d->title;
}

QString Axis::titleText() const
{
    return d->titleData->document()->toPlainText();
}

bool Axis::showLabels() const
{
    return d->showLabels;
}

bool Axis::showOverlappingDataLabels() const
{
    return d->showOverlappingDataLabels;
}

QString Axis::id() const
{
    return d->id;
}

AxisDimension Axis::dimension() const
{
    return d->dimension;
}

QList<DataSet*> Axis::dataSets() const
{
    return d->dataSets;
}

bool Axis::attachDataSet(DataSet *dataSet)
{
    Q_ASSERT(!d->dataSets.contains(dataSet));
    if (d->dataSets.contains(dataSet))
        return false;

    d->dataSets.append(dataSet);

    if (dimension() == YAxisDimension) {
        dataSet->setAttachedAxis(this);

        ChartType chartType = dataSet->chartType();
        if (chartType == LastChartType)
            chartType = d->plotAreaChartType;

        KChart::AbstractDiagram *diagram = d->getDiagramAndCreateIfNeeded(chartType);
        Q_ASSERT(diagram);
        KChartModel *model = dynamic_cast<KChartModel*>(diagram->model());
        Q_ASSERT(model);

        model->addDataSet(dataSet);

        layoutPlanes();
        requestRepaint();
    }

    return true;
}

bool Axis::detachDataSet(DataSet *dataSet, bool silent)
{
    Q_ASSERT(d->dataSets.contains(dataSet));
    if (!d->dataSets.contains(dataSet))
        return false;
    d->dataSets.removeAll(dataSet);

    if (dimension() == YAxisDimension) {
        ChartType chartType = dataSet->chartType();
        if (chartType == LastChartType)
            chartType = d->plotAreaChartType;

        KChart::AbstractDiagram *oldDiagram = d->getDiagram(chartType);
        Q_ASSERT(oldDiagram);
        KChartModel *oldModel = dynamic_cast<KChartModel*>(oldDiagram->model());
        Q_ASSERT(oldModel);

        const int rowCount = oldModel->dataDirection() == Qt::Vertical
                                 ? oldModel->columnCount() : oldModel->rowCount();
        // If there's only as many rows as needed for *one*
        // dataset, that means that the dataset we're removing is
        // the last one in the model --> delete model
        if (rowCount == oldModel->dataDimensions())
            d->deleteDiagram(chartType);
        else
            oldModel->removeDataSet(dataSet, silent);

        dataSet->setKdChartModel(0);
        dataSet->setAttachedAxis(0);

        if (!silent) {
            layoutPlanes();
            requestRepaint();
        }
    }

    return true;
}

void Axis::clearDataSets()
{
    QList<DataSet*> list = d->dataSets;
    foreach(DataSet *dataSet, list)
        detachDataSet(dataSet, true);
}

bool Axis::showRuler() const
{
    return d->kdAxis->rulerAttributes().showRulerLine();
}

void Axis::setShowRuler(bool show)
{
    KChart::RulerAttributes attr = d->kdAxis->rulerAttributes();
    attr.setShowRulerLine(!attr.showRulerLine());
    d->kdAxis->setRulerAttributes(attr);
}

qreal Axis::majorInterval() const
{
    return d->majorInterval;
}

void Axis::setMajorInterval(qreal interval)
{
    // Don't overwrite if automatic interval is being requested (for
    // interval = 0)
    if (interval != 0.0) {
        d->majorInterval = interval;
        d->useAutomaticMajorInterval = false;
    } else
        d->useAutomaticMajorInterval = true;

    // KChart
    KChart::GridAttributes attributes = d->kdPlane->gridAttributes(orientation());
    attributes.setGridStepWidth(interval);
    d->kdPlane->setGridAttributes(orientation(), attributes);

    attributes = d->kdPolarPlane->gridAttributes(true);
    attributes.setGridStepWidth(interval);
    d->kdPolarPlane->setGridAttributes(true, attributes);

    // FIXME: Hide minor tick marks more appropriately
    if (!d->showMinorGrid && interval != 0.0)
        setMinorInterval(interval);

    requestRepaint();
}

qreal Axis::minorInterval() const
{
    return (d->majorInterval / (qreal)d->minorIntervalDivisor);
}

void Axis::setMinorInterval(qreal interval)
{
    if (interval == 0.0)
        setMinorIntervalDivisor(0);
    else
        setMinorIntervalDivisor(int(qRound(d->majorInterval / interval)));
}

int Axis::minorIntervalDivisor() const
{
    return d->minorIntervalDivisor;
}

void Axis::setMinorIntervalDivisor(int divisor)
{
    // A divisor of 0.0 means automatic minor interval calculation
    if (divisor != 0) {
        d->minorIntervalDivisor = divisor;
        d->useAutomaticMinorInterval = false;
    } else
        d->useAutomaticMinorInterval = true;

    // KChart
    KChart::GridAttributes attributes = d->kdPlane->gridAttributes(orientation());
    attributes.setGridSubStepWidth((divisor != 0) ? (d->majorInterval / divisor) : 0.0);
    d->kdPlane->setGridAttributes(orientation(), attributes);

    attributes = d->kdPolarPlane->gridAttributes(true);
    attributes.setGridSubStepWidth((divisor != 0) ? (d->majorInterval / divisor) : 0.0);
    d->kdPolarPlane->setGridAttributes(true, attributes);

    requestRepaint();
}

bool Axis::useAutomaticMajorInterval() const
{
    return d->useAutomaticMajorInterval;
}

bool Axis::useAutomaticMinorInterval() const
{
    return d->useAutomaticMinorInterval;
}

void Axis::setUseAutomaticMajorInterval(bool automatic)
{
    d->useAutomaticMajorInterval = automatic;
    // A value of 0.0 will activate automatic intervals,
    // but not change d->majorInterval
    setMajorInterval(automatic ? 0.0 : majorInterval());
}

void Axis::setUseAutomaticMinorInterval(bool automatic)
{
    d->useAutomaticMinorInterval = automatic;
    // A value of 0.0 will activate automatic intervals,
    // but not change d->minorIntervalDivisor
    setMinorInterval(automatic ? 0.0 : minorInterval());
}

bool Axis::showInnerMinorTicks() const
{
    return d->showInnerMinorTicks;
}

bool Axis::showOuterMinorTicks() const
{
    return d->showOuterMinorTicks;
}

bool Axis::showInnerMajorTicks() const
{
    return d->showInnerMinorTicks;
}

bool Axis::showOuterMajorTicks() const
{
    return d->showOuterMajorTicks;
}

void Axis::setShowInnerMinorTicks(bool showTicks)
{
    d->showInnerMinorTicks = showTicks;
    KChart::RulerAttributes attr = kdAxis()->rulerAttributes();
    attr.setShowMinorTickMarks(d->showInnerMinorTicks || d->showOuterMinorTicks);
    kdAxis()->setRulerAttributes(attr);
}

void Axis::setShowOuterMinorTicks(bool showTicks)
{
    d->showOuterMinorTicks = showTicks;
    KChart::RulerAttributes attr = kdAxis()->rulerAttributes();
    attr.setShowMinorTickMarks(d->showInnerMinorTicks || d->showOuterMinorTicks);
    kdAxis()->setRulerAttributes(attr);
}

void Axis::setShowInnerMajorTicks(bool showTicks)
{
    d->showInnerMajorTicks = showTicks;
    KChart::RulerAttributes attr = kdAxis()->rulerAttributes();
    attr.setShowMajorTickMarks(d->showInnerMajorTicks || d->showOuterMajorTicks);
    kdAxis()->setRulerAttributes(attr);
}

void Axis::setShowOuterMajorTicks(bool showTicks)
{
    d->showOuterMajorTicks = showTicks;
    KChart::RulerAttributes attr = kdAxis()->rulerAttributes();
    attr.setShowMajorTickMarks(d->showInnerMajorTicks || d->showOuterMajorTicks);
    kdAxis()->setRulerAttributes(attr);
}

void Axis::setScalingLogarithmic(bool logarithmicScaling)
{
    d->logarithmicScaling = logarithmicScaling;

    if (dimension() != YAxisDimension)
        return;

    d->kdPlane->setAxesCalcModeY(d->logarithmicScaling
                                  ? KChart::AbstractCoordinatePlane::Logarithmic
                                  : KChart::AbstractCoordinatePlane::Linear);
    d->kdPlane->layoutPlanes();

    requestRepaint();
}

bool Axis::scalingIsLogarithmic() const
{
    return d->logarithmicScaling;
}

bool Axis::showMajorGrid() const
{
    return d->showMajorGrid;
}

void Axis::setShowMajorGrid(bool showGrid)
{
    d->showMajorGrid = showGrid;

    // KChart
    KChart::GridAttributes  attributes = d->kdPlane->gridAttributes(orientation());
    attributes.setGridVisible(d->showMajorGrid);
    d->kdPlane->setGridAttributes(orientation(), attributes);

    attributes = d->kdPolarPlane->gridAttributes(true);
    attributes.setGridVisible(d->showMajorGrid);
    d->kdPolarPlane->setGridAttributes(true, attributes);

    requestRepaint();
}

bool Axis::showMinorGrid() const
{
    return d->showMinorGrid;
}

void Axis::setShowMinorGrid(bool showGrid)
{
    d->showMinorGrid = showGrid;

    // KChart
    KChart::GridAttributes  attributes = d->kdPlane->gridAttributes(orientation());
    attributes.setSubGridVisible(d->showMinorGrid);
    d->kdPlane->setGridAttributes(orientation(), attributes);

    attributes = d->kdPolarPlane->gridAttributes(true);
    attributes.setSubGridVisible(d->showMinorGrid);
    d->kdPolarPlane->setGridAttributes(true, attributes);

    requestRepaint();
}

void Axis::setTitleText(const QString &text)
{
    d->titleData->document()->setPlainText(text);
}

void Axis::setShowLabels(bool show)
{
    d->showLabels = show;

    KChart::TextAttributes textAttr = d->kdAxis->textAttributes();
    textAttr.setVisible(show);
    d->kdAxis->setTextAttributes(textAttr);
}

void Axis::setShowOverlappingDataLabels(bool show)
{
    d->showOverlappingDataLabels = show;
}

Qt::Orientation Axis::orientation() const
{
    bool chartIsVertical = d->plotArea->isVertical();
    bool horizontal = d->dimension == (chartIsVertical ? YAxisDimension
                                                       : XAxisDimension);
    return horizontal ? Qt::Horizontal : Qt::Vertical;
}

bool Axis::loadOdf(const KoXmlElement &axisElement, KoShapeLoadingContext &context)
{
    KoStyleStack &styleStack = context.odfLoadingContext().styleStack();
    KoOdfStylesReader &stylesReader = context.odfLoadingContext().stylesReader();
    OdfLoadingHelper *helper = (OdfLoadingHelper*)context.sharedData(OdfLoadingHelperId);
    bool reverseAxis = false;

    d->title->setVisible(false);

    QPen gridPen(Qt::NoPen);
    QPen subGridPen(Qt::NoPen);

    d->showMajorGrid = false;
    d->showMinorGrid = false;

    d->showInnerMinorTicks = false;
    d->showOuterMinorTicks = false;
    d->showInnerMajorTicks = false;
    d->showOuterMajorTicks = true;

    // Use automatic interval calculation by default
    setMajorInterval(0.0);
    setMinorInterval(0.0);

    if (!axisElement.isNull()) {

        QString styleName = axisElement.attributeNS(KoXmlNS::chart, "style-name", QString());
        const KoXmlElement *stylElement = stylesReader.findStyle(styleName, "chart");
        if (stylElement) {
            const QString dataStyleName = stylElement->attributeNS(KoXmlNS::style, "data-style-name", QString());
            if (!dataStyleName.isEmpty() && stylesReader.dataFormats().contains(dataStyleName)) {
                delete d->numericStyleFormat;
                d->numericStyleFormat = new KoOdfNumberStyles::NumericStyleFormat(stylesReader.dataFormats()[dataStyleName].first);
            }
        }

        KoXmlElement n;
        forEachElement (n, axisElement) {
            if (n.namespaceURI() != KoXmlNS::chart)
                continue;
            if (n.localName() == "title") {
                OdfHelper::loadOdfTitle(d->title, n, context);
                // title shall *always* have AutoResize
                d->titleData->setResizeMethod(KoTextShapeDataBase::AutoResize);
            }
            else if (n.localName() == "grid") {
                bool major = false;
                if (n.hasAttributeNS(KoXmlNS::chart, "class")) {
                    const QString className = n.attributeNS(KoXmlNS::chart, "class");
                    if (className == "major")
                        major = true;
                } else {
                    warnChart << "Error: Axis' <chart:grid> element contains no valid class. It must be either \"major\" or \"minor\".";
                    continue;
                }

                if (major) {
                    d->showMajorGrid = true;
                } else {
                    d->showMinorGrid = true;
                }

                if (n.hasAttributeNS(KoXmlNS::chart, "style-name")) {
                    styleStack.clear();
                    context.odfLoadingContext().fillStyleStack(n, KoXmlNS::style, "style-name", "chart");
                    styleStack.setTypeProperties("graphic");
                    if (styleStack.hasProperty(KoXmlNS::svg, "stroke-color")) {
                        const QString strokeColor = styleStack.property(KoXmlNS::svg, "stroke-color");
                        //d->showMajorGrid = true;
                        if (major)
                            gridPen = QPen(QColor(strokeColor));
                        else
                            subGridPen = QPen(QColor(strokeColor));
                    }
                }
            }
            else if (n.localName() == "categories") {
                if (n.hasAttributeNS(KoXmlNS::table, "cell-range-address")) {
                    const CellRegion region = CellRegion(helper->tableSource, n.attributeNS(KoXmlNS::table, "cell-range-address"));
                    helper->categoryRegionSpecifiedInXAxis = true;
                    plotArea()->proxyModel()->setCategoryDataRegion(region);
                }
            }
        }

        if (axisElement.hasAttributeNS(KoXmlNS::chart, "axis-name")) {
            const QString name = axisElement.attributeNS(KoXmlNS::chart, "name", QString());
            setName(name);
        }

        // NOTE: chart:dimension already handled by PlotArea before and passed
        // explicitly in the constructor.
    } else {
        warnChartOdf<<"No axis element";
    }

    if (axisElement.hasAttributeNS(KoXmlNS::chart, "style-name")) {
        styleStack.clear();
        context.odfLoadingContext().fillStyleStack(axisElement, KoXmlNS::chart, "style-name", "chart");

        KoCharacterStyle charStyle;
        charStyle.loadOdf(&axisElement, context);
        setFont(charStyle.font());

        styleStack.setTypeProperties("chart");

        if (styleStack.hasProperty(KoXmlNS::chart, "logarithmic")
            && styleStack.property(KoXmlNS::chart, "logarithmic") == "true")
        {
            setScalingLogarithmic(true);
        }

        if (styleStack.hasProperty(KoXmlNS::chart, "reverse-direction")
            && styleStack.property(KoXmlNS::chart, "reverse-direction") == "true")
        {
            reverseAxis = true;
        }

        if (styleStack.hasProperty(KoXmlNS::chart, "interval-major"))
            setMajorInterval(KoUnit::parseValue(styleStack.property(KoXmlNS::chart, "interval-major")));
        if (styleStack.hasProperty(KoXmlNS::chart, "interval-minor-divisor"))
            setMinorIntervalDivisor(KoUnit::parseValue(styleStack.property(KoXmlNS::chart, "interval-minor-divisor")));
        else if (styleStack.hasProperty(KoXmlNS::chart, "interval-minor"))
            setMinorInterval(KoUnit::parseValue(styleStack.property(KoXmlNS::chart, "interval-minor")));

        if (styleStack.hasProperty(KoXmlNS::chart, "tick-marks-minor-inner"))
            setShowInnerMinorTicks(styleStack.property(KoXmlNS::chart, "tick-marks-minor-inner") == "true");
        if (styleStack.hasProperty(KoXmlNS::chart, "tick-marks-minor-outer"))
            setShowOuterMinorTicks(styleStack.property(KoXmlNS::chart, "tick-marks-minor-outer") == "true");
        if (styleStack.hasProperty(KoXmlNS::chart, "tick-marks-major-inner"))
            setShowInnerMajorTicks(styleStack.property(KoXmlNS::chart, "tick-marks-major-inner") == "true");
        if (styleStack.hasProperty(KoXmlNS::chart, "tick-marks-major-outer"))
            setShowOuterMajorTicks(styleStack.property(KoXmlNS::chart, "tick-marks-major-outer") == "true");

        if (styleStack.hasProperty(KoXmlNS::chart, "display-label"))
            setShowLabels(styleStack.property(KoXmlNS::chart, "display-label") != "false");
        if (styleStack.hasProperty(KoXmlNS::chart, "text-overlap"))
            setShowOverlappingDataLabels(styleStack.property(KoXmlNS::chart, "text-overlap") != "false");
        if (styleStack.hasProperty(KoXmlNS::chart, "visible"))
            setVisible(styleStack.property(KoXmlNS::chart, "visible")  != "false");
        if (styleStack.hasProperty(KoXmlNS::chart, "minimum")) {
            const qreal minimum = styleStack.property(KoXmlNS::chart, "minimum").toDouble();
            const qreal maximum = orientation() == Qt::Vertical
                                    ? d->kdPlane->verticalRange().second
                                    : d->kdPlane->horizontalRange().second;
            if (orientation() == Qt::Vertical)
                d->kdPlane->setVerticalRange(qMakePair(minimum, maximum));
            else
                d->kdPlane->setHorizontalRange(qMakePair(minimum, maximum));
            d->useAutomaticMinimumRange = false;
        }
        if (styleStack.hasProperty(KoXmlNS::chart, "maximum")) {
            const qreal minimum = orientation() == Qt::Vertical
                                    ? d->kdPlane->verticalRange().first
                                    : d->kdPlane->horizontalRange().first;
            const qreal maximum = styleStack.property(KoXmlNS::chart, "maximum").toDouble();
            if (orientation() == Qt::Vertical)
                d->kdPlane->setVerticalRange(qMakePair(minimum, maximum));
            else
                d->kdPlane->setHorizontalRange(qMakePair(minimum, maximum));
            d->useAutomaticMaximumRange = false;
        }
        /*if (styleStack.hasProperty(KoXmlNS::chart, "origin")) {
            const qreal origin = KoUnit::parseValue(styleStack.property(KoXmlNS::chart, "origin"));
        }*/
        styleStack.setTypeProperties("chart");
        if (styleStack.hasProperty(KoXmlNS::chart, "axis-position")) {
            d->axisPosition = styleStack.property(KoXmlNS::chart, "axis-position");
        }
        if (styleStack.hasProperty(KoXmlNS::chart, "axis-label-position")) {
            d->axisLabelsPosition = styleStack.property(KoXmlNS::chart, "axis-label-position");
        }

        styleStack.setTypeProperties("text");
        if (styleStack.hasProperty(KoXmlNS::fo, "font-size")) {
            const qreal fontSize = KoUnit::parseValue(styleStack.property(KoXmlNS::fo, "font-size"));
            setFontSize(fontSize);
        }
        if (styleStack.hasProperty(KoXmlNS::fo, "font-color")) {
            QString fontColorString =  styleStack.property(KoXmlNS::fo, "font-color");
            QColor color(fontColorString);
            if (color.isValid()) {
                KChart::TextAttributes tatt =  kdAxis()->textAttributes();
                QPen pen = tatt.pen();
                pen.setColor(color);
                tatt.setPen(pen);
                kdAxis()->setTextAttributes(tatt);
            }
        }
        if (styleStack.hasProperty(KoXmlNS::fo, "font-family")) {
            QString fontFamilyString = styleStack.property(KoXmlNS::fo, "font-family");
            if (!fontFamilyString.isEmpty()) {
                QFont f = this->font();
                f.setFamily(fontFamilyString);
                setFont(f);
            }
        }
        if (styleStack.hasProperty(KoXmlNS::fo, "font-style")) {
            QString fontStyle = styleStack.property(KoXmlNS::fo, "font-style");
            if (fontStyle == "italic") {
                QFont f = this->font();
                f.setItalic(true);
                setFont(f);
            } else if (fontStyle == "oblique") {
                // TODO
            }
        }
        if (styleStack.hasProperty(KoXmlNS::fo, "font-weight")) {
            QString fontWeight = styleStack.property(KoXmlNS::fo, "font-weight");
            //fo:font-weight attribute are normal, bold, 100, 200, 300, 400, 500, 600, 700, 800 or 900.
            if (fontWeight == "bold") {
                QFont f = this->font();
                f.setBold(true);
                setFont(f);
            } else {
                // TODO
            }
        }
    } else {
        warnChartOdf<<"Axis element has no style information";
        setShowLabels(KoOdfWorkaround::fixMissingStyle_DisplayLabel(axisElement, context));
    }

    KChart::GridAttributes gridAttr = d->kdPlane->gridAttributes(orientation());
    gridAttr.setGridVisible(d->showMajorGrid);
    gridAttr.setSubGridVisible(d->showMinorGrid);
    if (gridPen.style() != Qt::NoPen)
        gridAttr.setGridPen(gridPen);
    if (subGridPen.style() != Qt::NoPen)
        gridAttr.setSubGridPen(subGridPen);
    d->kdPlane->setGridAttributes(orientation(), gridAttr);

    gridAttr = d->kdPolarPlane->gridAttributes(orientation());
    gridAttr.setGridVisible(d->showMajorGrid);
    gridAttr.setSubGridVisible(d->showMinorGrid);
    if (gridPen.style() != Qt::NoPen)
        gridAttr.setGridPen(gridPen);
    if (subGridPen.style() != Qt::NoPen)
        gridAttr.setSubGridPen(subGridPen);
//     if (plotArea()->chartType() == RadarChartType || plotArea()->chartType() == FilledRadarChartType)
//         d->kdPolarPlane->setGridAttributes(false, gridAttr);
//     else
    d->kdPolarPlane->setGridAttributes(true, gridAttr);

    gridAttr = d->kdRadarPlane->globalGridAttributes();
    gridAttr.setGridVisible(d->showMajorGrid);
    gridAttr.setSubGridVisible(d->showMinorGrid);
    if (gridPen.style() != Qt::NoPen)
        gridAttr.setGridPen(gridPen);
    if (subGridPen.style() != Qt::NoPen)
        gridAttr.setSubGridPen(subGridPen);
    d->kdRadarPlane->setGlobalGridAttributes(gridAttr);
    KChart::TextAttributes ta(d->kdRadarPlane->textAttributes());
    ta.setVisible(helper->categoryRegionSpecifiedInXAxis);
    ta.setFont(font());
    ta.setFontSize(50);
    d->kdRadarPlane->setTextAttributes(ta);

    if (reverseAxis) {
        KChart::CartesianCoordinatePlane *plane = dynamic_cast<KChart::CartesianCoordinatePlane*>(kdPlane());
        if (plane) {
            if (orientation() == Qt::Horizontal)
                plane->setHorizontalRangeReversed(reverseAxis);
            else // Qt::Vertical
                plane->setVerticalRangeReversed(reverseAxis);
        }
    }

    // Style of axis is still in styleStack
    if (!loadOdfChartSubtypeProperties(axisElement, context)) {
        return false;
    }
    if (titleText().isEmpty()) {
        // do not allow visible empty text
        d->title->setVisible(false);
    }
    requestRepaint();

    debugChartOdf<<"Loaded axis:"<<name()<<"dimension:"<<dimension();
    return true;
}

bool Axis::loadOdfChartSubtypeProperties(const KoXmlElement &axisElement,
                                          KoShapeLoadingContext &context)
{
    Q_UNUSED(axisElement);
    KoStyleStack &styleStack = context.odfLoadingContext().styleStack();
    styleStack.setTypeProperties("chart");

    // Load these attributes regardless of the actual chart type. They'll have
    // no effect if their respective chart type is not in use.
    // However, they'll be saved back to ODF that way.
    if (styleStack.hasProperty(KoXmlNS::chart, "gap-width"))
        setGapBetweenSets(KoUnit::parseValue(styleStack.property(KoXmlNS::chart, "gap-width")));
    if (styleStack.hasProperty(KoXmlNS::chart, "overlap"))
        // The minus is intended!
        setGapBetweenBars(-KoUnit::parseValue(styleStack.property(KoXmlNS::chart, "overlap")));

    return true;
}

void Axis::setName(const QString &name)
{
    d->name = name;
}

// NOTE: only used during save/load to enable attaching axis to datasets
QString Axis::name() const
{
    if (!d->name.isEmpty()) {
        return d->name;
    }
    QString name;
    switch(dimension()) {
        case XAxisDimension:
            name = QLatin1Char('x');
            break;
        case YAxisDimension:
            name = QLatin1Char('y');
            break;
        case ZAxisDimension:
            name = QLatin1Char('z');
            break;
    }
    int i = 1;
    foreach (Axis *axis, d->plotArea->axes()) {
        if (axis == this)
            break;
        if (axis->dimension() == dimension())
            i++;
    }
    if (i == 1)
        name = "primary-" + name;
    else if (i == 2)
        name = "secondary-" + name;
    // Usually, there's not more than two axes of the same dimension.
    // But use a fallback name here nevertheless.
    else
        name = QString::number(i) + '-' + name;

    return name;
}

void Axis::saveOdf(KoShapeSavingContext &context)
{
    KoXmlWriter &bodyWriter = context.xmlWriter();
    KoGenStyles &mainStyles = context.mainStyles();
    bodyWriter.startElement("chart:axis");

    KoGenStyle axisStyle(KoGenStyle::ChartAutoStyle, "chart");
    axisStyle.addProperty("chart:logarithmic", scalingIsLogarithmic());


    axisStyle.addProperty("chart:reverse-direction", axisDirectionReversed());
    if (!d->axisPosition.isEmpty()) {
        axisStyle.addProperty("chart:axis-position", d->axisPosition);
    }
    if (!d->axisLabelsPosition.isEmpty()) {
        axisStyle.addProperty("chart:axis-label-position", d->axisLabelsPosition);
    }

    axisStyle.addProperty("chart:tick-marks-minor-inner", showInnerMinorTicks());
    axisStyle.addProperty("chart:tick-marks-minor-outer", showOuterMinorTicks());
    axisStyle.addProperty("chart:tick-marks-major-inner", showInnerMajorTicks());
    axisStyle.addProperty("chart:tick-marks-major-outer", showOuterMajorTicks());

    axisStyle.addProperty("chart:display-label", showLabels());
    axisStyle.addProperty("chart:text-overlap", showOverlappingDataLabels());
    axisStyle.addProperty("chart:visible", isVisible());
    if (dimension() == YAxisDimension) {
        axisStyle.addProperty("chart:gap-width", d->gapBetweenSets);
        axisStyle.addProperty("chart:overlap", -d->gapBetweenBars);
    }

    if (!d->useAutomaticMinimumRange) {
        const qreal minimum = orientation() == Qt::Vertical
                            ? d->kdPlane->verticalRange().first
                            : d->kdPlane->horizontalRange().first;
        axisStyle.addProperty("chart:minimum", (int)minimum);
    }
    if (!d->useAutomaticMaximumRange) {
        const qreal maximum = orientation() == Qt::Vertical
                            ? d->kdPlane->verticalRange().second
                            : d->kdPlane->horizontalRange().second;
        axisStyle.addProperty("chart:maximum", (int)maximum);
    }

    //axisStyle.addPropertyPt("chart:origin", origin);

    KChart::TextAttributes tatt =  kdAxis()->textAttributes();
    QPen pen = tatt.pen();
    axisStyle.addProperty("fo:font-color", pen.color().name(), KoGenStyle::TextType);
    axisStyle.addProperty("fo:font-family", tatt.font().family(), KoGenStyle::TextType);
    axisStyle.addPropertyPt("fo:font-size", fontSize(), KoGenStyle::TextType);
    if (font().bold()) {
        axisStyle.addProperty("fo:font-weight", "bold" , KoGenStyle::TextType);
        // TODO support other weights
    }
    if (font().italic()) {
        axisStyle.addProperty("fo:font-style", "italic" , KoGenStyle::TextType);
        // TODO oblique
    }

    const QString styleName = mainStyles.insert(axisStyle, "ch");
    bodyWriter.addAttribute("chart:style-name", styleName);

    // TODO scale: logarithmic/linear
    // TODO visibility

    if (dimension() == XAxisDimension)
        bodyWriter.addAttribute("chart:dimension", "x");
    else if (dimension() == YAxisDimension)
        bodyWriter.addAttribute("chart:dimension", "y");

    bodyWriter.addAttribute("chart:name", name());

    OdfHelper::saveOdfTitle(d->title, bodyWriter, "chart:title", context);

    if (plotArea()->proxyModel()->categoryDataRegion().isValid()) {
        bodyWriter.startElement("chart:categories");
        bodyWriter.addAttribute("table:cell-range-address", plotArea()->proxyModel()->categoryDataRegion().toString());
        bodyWriter.endElement();
    }

    if (showMajorGrid())
        saveOdfGrid(context, OdfMajorGrid);
    if (showMinorGrid())
        saveOdfGrid(context, OdfMinorGrid);

    bodyWriter.endElement(); // chart:axis
}

void Axis::saveOdfGrid(KoShapeSavingContext &context, OdfGridClass gridClass)
{
    KoXmlWriter &bodyWriter = context.xmlWriter();
    KoGenStyles &mainStyles = context.mainStyles();

    KoGenStyle gridStyle(KoGenStyle::GraphicAutoStyle, "chart");

    KChart::GridAttributes attributes = d->kdPlane->gridAttributes(orientation());
    QPen gridPen = (gridClass == OdfMinorGrid ? attributes.subGridPen() : attributes.gridPen());
    KoOdfGraphicStyles::saveOdfStrokeStyle(gridStyle, mainStyles, gridPen);

    bodyWriter.startElement("chart:grid");
    bodyWriter.addAttribute("chart:class", gridClass == OdfMinorGrid ? "minor" : "major");

    bodyWriter.addAttribute("chart:style-name", mainStyles.insert(gridStyle, "ch"));
    bodyWriter.endElement(); // chart:grid
}

void Axis::update() const
{
    if (d->kdBarDiagram) {
        d->kdBarDiagram->doItemsLayout();
        d->kdBarDiagram->update();
    }

    if (d->kdLineDiagram) {
        d->kdLineDiagram->doItemsLayout();
        d->kdLineDiagram->update();
    }

    if (d->kdStockDiagram) {
        d->kdStockDiagram->doItemsLayout();
        d->kdStockDiagram->update();
    }

    d->plotArea->parent()->requestRepaint();
}

KChart::CartesianAxis *Axis::kdAxis() const
{
    return d->kdAxis;
}

KChart::AbstractCoordinatePlane *Axis::kdPlane() const
{
    return d->kdPlane;
}

void Axis::plotAreaChartTypeChanged(ChartType newChartType)
{
    if (dimension() != YAxisDimension)
        return;

    // Return if there's nothing to do
    if (newChartType == d->plotAreaChartType)
        return;

    if (d->dataSets.isEmpty()) {
        d->plotAreaChartType = newChartType;
        return;
    }

    //qDebug() << "changed ChartType";

    ChartType oldChartType = d->plotAreaChartType;

    debugChartAxis<<oldChartType<<"->"<<newChartType;
    // Change only the fill in case of type change from RadarChartType to FilledRadarChartType
    // or viceversa as rest of the properties remain same
    if (newChartType == RadarChartType && oldChartType == FilledRadarChartType) {
        d->kdRadarDiagram->setFillAlpha(0);
    } else if (newChartType == FilledRadarChartType && oldChartType == RadarChartType) {
        d->kdRadarDiagram->setFillAlpha(0.4);
    } else {
        KChart::AbstractDiagram *newDiagram = d->getDiagram(newChartType);
        if (newDiagram) {
            debugChartAxis<<"already exists:"<<newDiagram;
            // Some dataset(s) have been attached to this diagram,
            // we delete it to get a fresh start
            d->deleteDiagram(newDiagram);
        }
        newDiagram = d->getDiagramAndCreateIfNeeded(newChartType);

        KChartModel *newModel = dynamic_cast<KChartModel*>(newDiagram->model());
        // FIXME: This causes a crash on unimplemented types. We should
        //        handle that in some other way.
        Q_ASSERT(newModel);

        foreach (DataSet *dataSet, d->dataSets) {
            //if (dataSet->chartType() != LastChartType) {
                dataSet->setChartType(LastChartType);
                dataSet->setChartSubType(NoChartSubtype);
            //}
        }

        KChart::AbstractDiagram *oldDiagram = d->getDiagram(oldChartType);
        Q_ASSERT(oldDiagram);
        // We need to know the old model so that we can remove the data sets
        // from the old model that we added to the new model.
        KChartModel *oldModel = dynamic_cast<KChartModel*>(oldDiagram->model());
        Q_ASSERT(oldModel);

        foreach (DataSet *dataSet, d->dataSets) {
            if (dataSet->chartType() != LastChartType) {
                continue;
            }
            newModel->addDataSet(dataSet);
            const int dataSetCount = oldModel->dataDirection() == Qt::Vertical
                                     ? oldModel->columnCount() : oldModel->rowCount();
            if (dataSetCount == oldModel->dataDimensions()) {
                d->deleteDiagram(oldChartType);
            } else {
                oldModel->removeDataSet(dataSet);
            }
        }
    }

    d->plotAreaChartType = newChartType;

    layoutPlanes();

    requestRepaint();
}

void Axis::plotAreaChartSubTypeChanged(ChartSubtype subType)
{
    d->plotAreaChartSubType = subType;
    if (d->kdBarDiagram) {
        d->kdBarDiagram->setUnitSuffix("", d->kdBarDiagram->orientation());
    }
    switch (d->plotAreaChartType) {
    case BarChartType:
        if (d->kdBarDiagram) {
            KChart::BarDiagram::BarType type;
            switch (subType) {
            case StackedChartSubtype:
                type = KChart::BarDiagram::Stacked; break;
            case PercentChartSubtype:
                type = KChart::BarDiagram::Percent;
                d->kdBarDiagram->setUnitSuffix("%", d->kdBarDiagram->orientation());
                break;
            default:
                type = KChart::BarDiagram::Normal;
            }
            d->kdBarDiagram->setType(type);
        }
        break;
    case LineChartType:
        if (d->kdLineDiagram) {
            KChart::LineDiagram::LineType type;
            switch (subType) {
            case StackedChartSubtype:
                type = KChart::LineDiagram::Stacked; break;
            case PercentChartSubtype:
                type = KChart::LineDiagram::Percent;
                d->kdLineDiagram->setUnitSuffix("%", Qt::Vertical);
                break;
            default:
                type = KChart::LineDiagram::Normal;
            }
            d->kdLineDiagram->setType(type);
        }
        break;
    case AreaChartType:
        if (d->kdAreaDiagram) {
            KChart::LineDiagram::LineType type;
            switch (subType) {
            case StackedChartSubtype:
                type = KChart::LineDiagram::Stacked; break;
            case PercentChartSubtype:
                type = KChart::LineDiagram::Percent;
                d->kdAreaDiagram->setUnitSuffix("%", Qt::Vertical);
                break;
            default:
                type = KChart::LineDiagram::Normal;
            }
            d->kdAreaDiagram->setType(type);
        }
        break;
    case RadarChartType:
    case FilledRadarChartType:
#if 0 // FIXME: Stacked and Percent not supported by KChart
        if (d->kdRadarDiagram) {
            KChart::PolarDiagram::PolarType type;
            switch (subType) {
            case StackedChartSubtype:
                type = KChart::PolarDiagram::Stacked; break;
            case PercentChartSubtype:
                type = KChart::PolarDiagram::Percent; break;
            default:
                type = KChart::PolarDiagram::Normal;
            }
            d->kdRadarDiagram->setType(type);
        }
#endif
        break;
    case StockChartType:
        if (d->kdStockDiagram) {
            KChart::StockDiagram::Type type;
            switch (subType) {
            case CandlestickChartSubtype:
                type = KChart::StockDiagram::Candlestick;
                break;
            case OpenHighLowCloseChartSubtype:
                type = KChart::StockDiagram::OpenHighLowClose;
                break;
            default:
                type = KChart::StockDiagram::HighLowClose;
                break;
            }
            d->kdStockDiagram->setType(type);
        }
        break;
    default:;
        // FIXME: Implement more chart types
    }
    Q_FOREACH(DataSet* set,  d->dataSets) {
        set->setChartType(d->plotAreaChartType);
        set->setChartSubType(subType);
    }
}

void Axis::plotAreaIsVerticalChanged()
{
    if (d->kdBarDiagram) {
        d->kdBarDiagram->setOrientation(d->plotArea->isVertical() ? Qt::Horizontal : Qt::Vertical);
    }
    updateKChartAxisPosition();
}

void Axis::Private::updatePosition()
{
//     // Is the first x or y axis?
//     bool first = (dimension == XAxisDimension) ? plotArea->xAxis() == q
//                                                : plotArea->yAxis() == q;
//
//     Position position;
//     ItemType type = GenericItemType;
//     if (q->orientation() == Qt::Horizontal) {
//         position = first ? BottomPosition : TopPosition;
//         type = first ? XAxisTitleType : SecondaryXAxisTitleType;
//     } else {
//         position = first ? StartPosition : EndPosition;
//         type = first ? YAxisTitleType : SecondaryYAxisTitleType;
//     }
//     // KChart
//     kdAxis->setPosition(PositionToKChartAxisPosition(position));
//     ChartLayout *layout = plotArea->parent()->layout();
//     layout->setPosition(title, position, type);
//     layout->layout();
//
//     q->requestRepaint();
}

void Axis::registerAxis(Axis *axis)
{
    if (d->kdBarDiagram) {
        d->kdBarDiagram->addAxis(axis->kdAxis());
        axis->registerDiagram(d->kdBarDiagram);
    }
    if (d->kdLineDiagram) {
        d->kdLineDiagram->addAxis(axis->kdAxis());
        axis->registerDiagram(d->kdLineDiagram);
    }
    if (d->kdAreaDiagram) {
        d->kdAreaDiagram->addAxis(axis->kdAxis());
        axis->registerDiagram(d->kdAreaDiagram);
    }
    if (d->kdScatterDiagram) {
        d->kdScatterDiagram->addAxis(axis->kdAxis());
        axis->registerDiagram(d->kdScatterDiagram);
    }
    if (d->kdStockDiagram) {
        d->kdStockDiagram->addAxis(axis->kdAxis());
        axis->registerDiagram(d->kdStockDiagram);
    }
    if (d->kdBubbleDiagram) {
        d->kdBubbleDiagram->addAxis(axis->kdAxis());
        axis->registerDiagram(d->kdBubbleDiagram);
    }
    // FIXME: Add all diagrams here

}

void Axis::registerDiagram(KChart::AbstractCartesianDiagram *diagram)
{
    if (!d->diagrams.contains(diagram)) {
        d->diagrams << diagram;
    }
}

void Axis::Private::restoreDiagrams()
{
    diagrams.removeAll(nullptr);
    for (KChart::AbstractCartesianDiagram *diag : diagrams) {
        diag->addAxis(kdAxis);
    }
}

void Axis::removeAxisFromDiagrams(bool clear)
{
    // HACK to remove an x-axis from a y-axis diagram
    d->diagrams.removeAll(nullptr);
    for (KChart::AbstractCartesianDiagram *diag : d->diagrams) {
        diag->takeAxis(d->kdAxis);
    }
    if (clear) {
        d->diagrams.clear();
    }
}

void Axis::setThreeD(bool threeD)
{
    // FIXME: Setting KD Chart attributes does not belong here. They should be
    // determined dynamically somehow.
    // KChart
    if (d->kdBarDiagram) {
        KChart::ThreeDBarAttributes attributes(d->kdBarDiagram->threeDBarAttributes());
        attributes.setEnabled(threeD);
        attributes.setDepth(15.0);
        attributes.setThreeDBrushEnabled(threeD);
        d->kdBarDiagram->setThreeDBarAttributes(attributes);
    }

    if (d->kdLineDiagram) {
        KChart::ThreeDLineAttributes attributes(d->kdLineDiagram->threeDLineAttributes());
        attributes.setEnabled(threeD);
        attributes.setDepth(15.0);
        attributes.setThreeDBrushEnabled(threeD);
        d->kdLineDiagram->setThreeDLineAttributes(attributes);
    }

    if (d->kdAreaDiagram) {
        KChart::ThreeDLineAttributes attributes(d->kdAreaDiagram->threeDLineAttributes());
        attributes.setEnabled(threeD);
        attributes.setDepth(15.0);
        attributes.setThreeDBrushEnabled(threeD);
        d->kdAreaDiagram->setThreeDLineAttributes(attributes);
    }

    if (d->kdCircleDiagram) {
        KChart::ThreeDPieAttributes attributes(d->kdCircleDiagram->threeDPieAttributes());
        attributes.setEnabled(threeD);
        attributes.setDepth(15.0);
        attributes.setThreeDBrushEnabled(threeD);
        d->kdCircleDiagram->setThreeDPieAttributes(attributes);
    }

    if (d->kdRingDiagram) {
        KChart::ThreeDPieAttributes attributes(d->kdRingDiagram->threeDPieAttributes());
        attributes.setEnabled(threeD);
        attributes.setDepth(15.0);
        attributes.setThreeDBrushEnabled(threeD);
        d->kdRingDiagram->setThreeDPieAttributes(attributes);
    }

    // The following types don't support 3D, at least not in KChart:
    // scatter, radar, stock, bubble, surface, gantt

    requestRepaint();
}

void Axis::requestRepaint() const
{
    d->plotArea->requestRepaint();
}

void Axis::layoutPlanes()
{
    d->kdPlane->layoutPlanes();
    d->kdPolarPlane->layoutPlanes();
    d->kdRadarPlane->layoutPlanes();
}

int Axis::gapBetweenBars() const
{
    return d->gapBetweenBars;
}

void Axis::setGapBetweenBars(int percent)
{
    // This method is also used to override KChart's default attributes.
    // Do not just return and do nothing if value doesn't differ from stored one.
    d->gapBetweenBars = percent;

    if (d->kdBarDiagram) {
        KChart::BarAttributes attributes = d->kdBarDiagram->barAttributes();
        attributes.setBarGapFactor((float)percent / 100.0);
        d->kdBarDiagram->setBarAttributes(attributes);
    }

    requestRepaint();
}

int Axis::gapBetweenSets() const
{
    return d->gapBetweenSets;
}

void Axis::setGapBetweenSets(int percent)
{
    // This method is also used to override KChart's default attributes.
    // Do not just return and do nothing if value doesn't differ from stored one.
    d->gapBetweenSets = percent;

    if (d->kdBarDiagram) {
        KChart::BarAttributes attributes = d->kdBarDiagram->barAttributes();
        attributes.setGroupGapFactor((float)percent / 100.0);
        d->kdBarDiagram->setBarAttributes(attributes);
    }

    requestRepaint();
}

void Axis::setAngleOffset(qreal angle)
{
    // only set if we already have a diagram else the value will be picked up on creating the diagram
    if (d->kdPolarPlane->diagram()) {
        d->kdPolarPlane->setStartPosition(angle);

        requestRepaint();
    }
}

void Axis::setHoleSize(qreal value)
{
    //TODO KChart does not support
}

QFont Axis::font() const
{
    return d->kdAxis->textAttributes().font();
}

void Axis::setFont(const QFont &font)
{
    // Set the KChart axis to use this font
    KChart::TextAttributes attr = d->kdAxis->textAttributes();
    attr.setFont(font);
    d->kdAxis->setTextAttributes(attr);
}

qreal Axis::fontSize() const
{
    return d->kdAxis->textAttributes().fontSize().value();
}

void Axis::setFontSize(qreal size)
{
    // KChart has its own fontsize storage, it does not use QFont
    KChart::TextAttributes attributes = d->kdAxis->textAttributes();
    attributes.setFontSize(KChart::Measure(size, KChartEnums::MeasureCalculationModeAbsolute));
    d->kdAxis->setTextAttributes(attributes);

    // Keep font in sync
    QFont f = font();
    f.setPointSizeF(size);
    setFont(f);
}

bool Axis::isVisible() const
{
    return d->isVisible;
}

void Axis::setVisible(bool visible)
{
    debugChartAxis<<d->isVisible<<"->"<<visible<<d->kdBarDiagram;
    d->isVisible = visible;
    if (visible) {
        d->restoreDiagrams();
    } else {
        removeAxisFromDiagrams();
    }
}

KoOdfNumberStyles::NumericStyleFormat *Axis::numericStyleFormat() const
{
    return d->numericStyleFormat;
}

void Axis::SetNumericStyleFormat(KoOdfNumberStyles::NumericStyleFormat *numericStyleFormat) const
{
    delete d->numericStyleFormat;
    d->numericStyleFormat = numericStyleFormat;
}

void Axis::setOdfAxisPosition(const QString &odfpos)
{
    d->axisPosition = odfpos;
}

QString Axis::odfAxisPosition() const
{
    return d->axisPosition;
}

void Axis::updateKChartAxisPosition()
{
    if (!isCartesian(d->plotArea->chartType())) {
        debugChartAxis<<name()<<"Not a cartesian chart"<<d->plotArea->chartType();
        return;
    }
    KChart::CartesianAxis::Position pos;
    if (d->plotArea->xAxis() == this) {
        if (d->plotArea->isVertical()) {
            pos = KChart::CartesianAxis::Left;
            if (d->axisPosition == "end") {
                pos = KChart::CartesianAxis::Right;
            }
            Axis *yAxis = d->plotArea->yAxis();
            if (yAxis && yAxis->axisDirectionReversed()) {
                pos = pos == KChart::CartesianAxis::Left ? KChart::CartesianAxis::Right : KChart::CartesianAxis::Left;
            }
        } else {
            pos = KChart::CartesianAxis::Bottom;
            if (d->axisPosition == "end") {
                pos = KChart::CartesianAxis::Top;
            }
            Axis *yAxis = d->plotArea->yAxis();
            if (yAxis && yAxis->axisDirectionReversed()) {
                pos = pos == KChart::CartesianAxis::Bottom ? KChart::CartesianAxis::Top : KChart::CartesianAxis::Bottom;
            }
        }
        d->kdAxis->setPosition(pos);
    } else if (d->plotArea->yAxis() == this) {
        if (d->plotArea->isVertical()) {
            pos = KChart::CartesianAxis::Bottom;
            if (d->axisPosition == "end") {
                pos = KChart::CartesianAxis::Top;
            }
            Axis *xAxis = d->plotArea->xAxis();
            if (xAxis && xAxis->axisDirectionReversed()) {
                pos = pos == KChart::CartesianAxis::Bottom ? KChart::CartesianAxis::Top : KChart::CartesianAxis::Bottom;
            }
        } else {
            pos = KChart::CartesianAxis::Left;
            if (d->axisPosition == "end") {
                pos = KChart::CartesianAxis::Right;
            }
            Axis *xAxis = d->plotArea->xAxis();
            if (xAxis && xAxis->axisDirectionReversed()) {
                pos = pos == KChart::CartesianAxis::Left ? KChart::CartesianAxis::Right : KChart::CartesianAxis::Left;
            }
        }
        d->kdAxis->setPosition(pos);
    } else if (d->plotArea->secondaryXAxis() == this) {
        if (d->plotArea->isVertical()) {
            pos = KChart::CartesianAxis::Right;
            if (d->axisPosition == "start") {
                pos = KChart::CartesianAxis::Left;
            }
            Axis *yAxis = d->plotArea->yAxis();
            if (yAxis && yAxis->axisDirectionReversed()) {
                pos = pos == KChart::CartesianAxis::Left ? KChart::CartesianAxis::Right : KChart::CartesianAxis::Left;
            }
        } else {
            pos = KChart::CartesianAxis::Top;
            if (d->axisPosition == "start") {
                pos = KChart::CartesianAxis::Bottom;
            }
            Axis *yAxis = d->plotArea->yAxis();
            if (yAxis && yAxis->axisDirectionReversed()) {
                pos = pos == KChart::CartesianAxis::Top ? KChart::CartesianAxis::Bottom : KChart::CartesianAxis::Top;
            }
        }
        d->kdAxis->setPosition(pos);
    } else if (d->plotArea->secondaryYAxis() == this) {
        if (d->plotArea->isVertical()) {
            pos = KChart::CartesianAxis::Top;
            if (d->axisPosition == "start") {
                pos = KChart::CartesianAxis::Bottom;
            }
            Axis *xAxis = d->plotArea->xAxis();
            if (xAxis && xAxis->axisDirectionReversed()) {
                pos = pos == KChart::CartesianAxis::Bottom ? KChart::CartesianAxis::Top : KChart::CartesianAxis::Bottom;
            }
        } else {
            pos = KChart::CartesianAxis::Right;
            if (d->axisPosition == "start") {
                pos = KChart::CartesianAxis::Left;
            }
            Axis *xAxis = d->plotArea->xAxis();
            if (xAxis && xAxis->axisDirectionReversed()) {
                pos = pos == KChart::CartesianAxis::Right ? KChart::CartesianAxis::Left : KChart::CartesianAxis::Right;
            }
        }
        d->kdAxis->setPosition(pos);
    }
    debugChartAxis<<name()<<d->kdAxis<<pos<<d->kdAxis->isAbscissa();
    d->plotArea->plotAreaUpdate();
}

CartesianAxis::Position Axis::kchartAxisPosition() const
{
    return d->kdAxis->position();
}

CartesianAxis::Position Axis::actualAxisPosition() const
{
    CartesianAxis::Position pos = d->kdAxis->position();
    if (d->plotArea->isVertical()) {
        switch (pos) {
            case KChart::CartesianAxis::Bottom:
                pos = KChart::CartesianAxis::Left;
                break;
            case KChart::CartesianAxis::Top:
                pos = KChart::CartesianAxis::Right;
                break;
            case KChart::CartesianAxis::Left:
                pos = KChart::CartesianAxis::Bottom;
                break;
            case KChart::CartesianAxis::Right:
                pos = KChart::CartesianAxis::Top;
                break;
        }
    }
    return pos;
}

bool Axis::axisDirectionReversed() const
{
    bool reversed = false;
    KChart::CartesianCoordinatePlane *plane = dynamic_cast<KChart::CartesianCoordinatePlane*>(kdPlane());
    if (plane) {
        if (orientation() == Qt::Horizontal)
            reversed = plane->isHorizontalRangeReversed();
        else // Qt::Vertical
            reversed = plane->isVerticalRangeReversed();
    }
    return reversed;
}

void Axis::setOdfAxisLabelsPosition(const QString &odfpos)
{
    d->axisLabelsPosition = odfpos;
}

QString Axis::odfAxisLabelsPosition() const
{
    return d->axisLabelsPosition;
}

void Axis::updateKChartStockAttributes()
{
    if (d->kdStockDiagram) {
        d->kdStockDiagram->setLowHighLinePen(d->plotArea->stockRangeLinePen());
        d->kdStockDiagram->setUpTrendCandlestickBrush(d->plotArea->stockGainBrush());
        d->kdStockDiagram->setDownTrendCandlestickBrush(d->plotArea->stockLossBrush());
        d->kdStockDiagram->setUpTrendCandlestickPen(d->plotArea->stockRangeLinePen());
        d->kdStockDiagram->setDownTrendCandlestickPen(d->plotArea->stockRangeLinePen());
    }
}

QDebug operator<<(QDebug dbg, KoChart::Axis *a)
{
    dbg.nospace().noquote() <<"Axis["<<a->name()<<']';
    return dbg.space().quote();
}
