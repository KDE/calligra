/* This file is part of the KDE project

   Copyright 2007-2008 Johannes Simon <johannes.simon@gmail.com>
   Copyright 2009-2010 Inge Wallin <inge@lysator.liu.se>
   Copyright 2018 Dag Andersen <danders@get2net.dk>

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
#include "PlotArea.h"

// Qt
#include <QPointF>
#include <QSizeF>
#include <QList>
#include <QImage>
#include <QPainter>
#include <QPainterPath>

// Calligra
#include <KoXmlNS.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>
#include <KoGenStyles.h>
#include <KoStyleStack.h>
#include <KoOdfLoadingContext.h>
#include <Ko3dScene.h>
#include <KoOdfGraphicStyles.h>
#include <KoShapeLoadingContext.h>
#include <KoShapeSavingContext.h>
#include <KoTextShapeData.h>
#include <KoViewConverter.h>
#include <KoShapeBackground.h>

// KChart
#include <KChartChart>
#include <KChartCartesianAxis>
#include <KChartAbstractDiagram>
#include <kchart_version.h>
#include <KChartAbstractCartesianDiagram>
#include <KChartBarAttributes>
#include <KChartCartesianCoordinatePlane>
#include <KChartPolarCoordinatePlane>
#include <KChartRadarCoordinatePlane>
// Attribute Classes
#include <KChartFrameAttributes>
#include <KChartDataValueAttributes>
#include <KChartGridAttributes>
#include <KChartTextAttributes>
#include <KChartMarkerAttributes>
// Diagram Classes
#include <KChartBarDiagram>
#include <KChartPieDiagram>
#include <KChartLineDiagram>
#include <KChartRingDiagram>
#include <KChartPolarDiagram>

// KoChart
#include "Legend.h"
#include "Surface.h"
#include "Axis.h"
#include "DataSet.h"
#include "ChartProxyModel.h"
#include "ScreenConversions.h"
#include "ChartLayout.h"
#include "ChartDebug.h"

using namespace KoChart;

const int MAX_PIXMAP_SIZE = 1000;

Q_DECLARE_METATYPE(QPointer<QAbstractItemModel>)
typedef QList<KChart::AbstractCoordinatePlane*> CoordinatePlaneList;

class PlotArea::Private
{
public:
    Private(PlotArea *q, ChartShape *parent);
    ~Private();

    void initAxes();
    void updateAxesPosition();
    CoordinatePlaneList coordinatePlanesForChartType(ChartType type);
    void autoHideAxisTitles();

    PlotArea *q;
    // The parent chart shape
    ChartShape *shape;

    // ----------------------------------------------------------------
    // Parts and properties of the chart

    ChartType     chartType;
    ChartSubtype  chartSubtype;

    Surface       *wall;
    Surface       *floor;       // Only used in 3D charts

    // The axes
    QList<Axis*>     axes;
    QList<KoShape*>  automaticallyHiddenAxisTitles;

    // 3D properties
    bool       threeD;
    Ko3dScene *threeDScene;

    // ----------------------------------------------------------------
    // Data specific to each chart type

    // 1. Bar charts
    // FIXME: OpenOffice stores these attributes in the axes' elements.
    // The specs don't say anything at all about what elements can have
    // these style attributes.
    // chart:vertical attribute: see ODF v1.2,19.63
    bool  vertical;

    // 2. Polar charts (pie/ring)
    qreal angleOffset;       // in degrees
    qreal holeSize;

    // ----------------------------------------------------------------
    // The embedded KD Chart

    // The KD Chart parts
    KChart::Chart                    *const kdChart;
    KChart::CartesianCoordinatePlane *const kdCartesianPlanePrimary;
    KChart::CartesianCoordinatePlane *const kdCartesianPlaneSecondary;
    KChart::PolarCoordinatePlane     *const kdPolarPlane;
    KChart::RadarCoordinatePlane     *const kdRadarPlane;
    QList<KChart::AbstractDiagram*>   kdDiagrams;

    // Caching: We can rerender faster if we cache KChart's output
    QImage   image;
    bool     paintPixmap;
    QPointF  lastZoomLevel;
    QSizeF   lastSize;
    mutable bool pixmapRepaintRequested;

    QPen stockRangeLinePen;
    QBrush stockGainBrush;
    QBrush stockLossBrush;

    QString symbolType;
    QString symbolName;
    DataSet::ValueLabelType valueLabelType;
};

PlotArea::Private::Private(PlotArea *q, ChartShape *parent)
    : q(q)
    , shape(parent)
    // Default type: normal bar chart
    , chartType(BarChartType)
    , chartSubtype(NormalChartSubtype)
    , wall(0)
    , floor(0)
    , threeD(false)
    , threeDScene(0)
    // By default, x and y axes are not swapped.
    , vertical(false)
    // OpenOffice.org's default. It means the first pie slice starts at the
    // very top (and then going counter-clockwise).
    , angleOffset(90.0)
    , holeSize(50.0) // KCharts approx default
    // KD Chart stuff
    , kdChart(new KChart::Chart())
    , kdCartesianPlanePrimary(new KChart::CartesianCoordinatePlane(kdChart))
    , kdCartesianPlaneSecondary(new KChart::CartesianCoordinatePlane(kdChart))
    , kdPolarPlane(new KChart::PolarCoordinatePlane(kdChart))
    , kdRadarPlane(new KChart::RadarCoordinatePlane(kdChart))
    // Cache
    , paintPixmap(true)
    , pixmapRepaintRequested(true)
    , symbolType("automatic")
{
    kdCartesianPlanePrimary->setObjectName("primary");
    kdCartesianPlaneSecondary->setObjectName("secondary");
    // --- Prepare Primary Cartesian Coordinate Plane ---
    KChart::GridAttributes gridAttributes;
    gridAttributes.setGridVisible(false);
    gridAttributes.setGridGranularitySequence(KChartEnums::GranularitySequence_10_50);
    kdCartesianPlanePrimary->setGlobalGridAttributes(gridAttributes);

    // --- Prepare Secondary Cartesian Coordinate Plane ---
    kdCartesianPlaneSecondary->setGlobalGridAttributes(gridAttributes);

    // --- Prepare Polar Coordinate Plane ---
    KChart::GridAttributes polarGridAttributes;
    polarGridAttributes.setGridVisible(false);
    kdPolarPlane->setGlobalGridAttributes(polarGridAttributes);

    // --- Prepare Radar Coordinate Plane ---
    KChart::GridAttributes radarGridAttributes;
    polarGridAttributes.setGridVisible(true);
    kdRadarPlane->setGlobalGridAttributes(radarGridAttributes);

    // By default we use a cartesian chart (bar chart), so the polar planes
    // are not needed yet. They will be added on demand in setChartType().
    kdChart->takeCoordinatePlane(kdPolarPlane);
    kdChart->takeCoordinatePlane(kdRadarPlane);

    shape->proxyModel()->setDataDimensions(1);

    stockRangeLinePen.setWidthF(2.0);
    stockGainBrush = QBrush(QColor(Qt::white));
    stockLossBrush = QBrush(QColor(Qt::black));
}

PlotArea::Private::~Private()
{
    // remove first to avoid crash
    while (!kdChart->coordinatePlanes().isEmpty()) {
        kdChart->takeCoordinatePlane(kdChart->coordinatePlanes().last());
    }

    qDeleteAll(axes);
    delete kdCartesianPlanePrimary;
    delete kdCartesianPlaneSecondary;
    delete kdPolarPlane;
    delete kdRadarPlane;
    delete kdChart;
    delete wall;
    delete floor;
    delete threeDScene;
}

void PlotArea::Private::initAxes()
{
    // The category data region is anchored to an axis and will be set on addAxis if the
    // axis defines the Axis::categoryDataRegion(). So, clear it now.
    q->proxyModel()->setCategoryDataRegion(CellRegion());
    // Remove all old axes
    while(!axes.isEmpty()) {
        Axis *axis = axes.takeLast();
        Q_ASSERT(axis);
        if (axis->title())
            automaticallyHiddenAxisTitles.removeAll(axis->title());
        delete axis;
    }
    // There need to be at least these two axes. Their constructor will
    // automatically add them to the plot area as child shape.
    new Axis(q, XAxisDimension);
    Axis *yAxis = new Axis(q, YAxisDimension);
    yAxis->setShowMajorGrid(true);

    updateAxesPosition();
}

void PlotArea::Private::updateAxesPosition()
{
    debugChartAxis<<axes;
    for (int i = 0; i < axes.count(); ++i) {
        axes.at(i)->updateKChartAxisPosition();
    }
}

PlotArea::PlotArea(ChartShape *parent)
    : QObject()
    , KoShape()
    , d(new Private(this, parent))
{
    setShapeId("ChartShapePlotArea"); // NB! used by defaulttool/ChartResizeStrategy.cpp

    Q_ASSERT(d->shape);
    Q_ASSERT(d->shape->proxyModel());

    setAdditionalStyleAttribute("chart:auto-position", "true");
    setAdditionalStyleAttribute("chart:auto-size", "true");

    connect(d->shape->proxyModel(), SIGNAL(modelReset()),
            this,                   SLOT(proxyModelStructureChanged()));
    connect(d->shape->proxyModel(), SIGNAL(rowsInserted(QModelIndex,int,int)),
            this,                   SLOT(proxyModelStructureChanged()));
    connect(d->shape->proxyModel(), SIGNAL(rowsRemoved(QModelIndex,int,int)),
            this,                   SLOT(proxyModelStructureChanged()));
    connect(d->shape->proxyModel(), SIGNAL(columnsInserted(QModelIndex,int,int)),
            this,                   SLOT(proxyModelStructureChanged()));
    connect(d->shape->proxyModel(), SIGNAL(columnsRemoved(QModelIndex,int,int)),
            this,                   SLOT(proxyModelStructureChanged()));
    connect(d->shape->proxyModel(), SIGNAL(columnsInserted(QModelIndex,int,int)),
            this,                   SLOT(plotAreaUpdate()));
    connect(d->shape->proxyModel(), SIGNAL(columnsRemoved(QModelIndex,int,int)),
            this,                   SLOT(plotAreaUpdate()));
    connect(d->shape->proxyModel(), SIGNAL(dataChanged()),
            this,                   SLOT(plotAreaUpdate()));
}

PlotArea::~PlotArea()
{
    delete d;
}


void PlotArea::plotAreaInit()
{
    d->kdChart->resize(size().toSize());
    d->kdChart->replaceCoordinatePlane(d->kdCartesianPlanePrimary);
    d->kdCartesianPlaneSecondary->setReferenceCoordinatePlane(d->kdCartesianPlanePrimary);
    d->kdChart->addCoordinatePlane(d->kdCartesianPlaneSecondary);

    KChart::FrameAttributes attr = d->kdChart->frameAttributes();
    attr.setVisible(false);
    d->kdChart->setFrameAttributes(attr);

    d->wall = new Surface(this);
    //d->floor = new Surface(this);

    d->initAxes();

    addAxesTitlesToLayout();
}

void PlotArea::proxyModelStructureChanged()
{
    if (proxyModel()->isLoading())
        return;

    Q_ASSERT(xAxis());
    Q_ASSERT(yAxis());
    QMap<DataSet*, Axis*> attachedAxes;
    QList<DataSet*> dataSets = proxyModel()->dataSets();

    // Remember to what y axis each data set belongs
    foreach(DataSet *dataSet, dataSets)
        attachedAxes.insert(dataSet, dataSet->attachedAxis());

    // Proxy structure and thus data sets changed, drop old state and
    // clear all axes of data sets
    foreach(Axis *axis, axes())
        axis->clearDataSets();

    // Now add the new list of data sets to the axis they belong to
    foreach(DataSet *dataSet, dataSets) {
        xAxis()->attachDataSet(dataSet);
        // If they weren't assigned to a y axis before, use default y axis
        if (attachedAxes[dataSet])
            attachedAxes[dataSet]->attachDataSet(dataSet);
        else
            yAxis()->attachDataSet(dataSet);
    }
}

ChartProxyModel *PlotArea::proxyModel() const
{
    return d->shape->proxyModel();
}


QList<Axis*> PlotArea::axes() const
{
    return d->axes;
}

QList<DataSet*> PlotArea::dataSets() const
{
    return proxyModel()->dataSets();
}

Axis *PlotArea::xAxis() const
{
    foreach(Axis *axis, d->axes) {
        if (axis->dimension() == XAxisDimension)
            return axis;
    }

    return 0;
}

Axis *PlotArea::yAxis() const
{
    foreach(Axis *axis, d->axes) {
        if (axis->dimension() == YAxisDimension)
            return axis;
    }

    return 0;
}

Axis *PlotArea::secondaryXAxis() const
{
    bool firstXAxisFound = false;

    foreach(Axis *axis, d->axes) {
        if (axis->dimension() == XAxisDimension) {
            if (firstXAxisFound)
                return axis;
            else
                firstXAxisFound = true;
        }
    }

    return 0;
}

Axis *PlotArea::secondaryYAxis() const
{
    bool firstYAxisFound = false;

    foreach(Axis *axis, d->axes) {
        if (axis->dimension() == YAxisDimension) {
            if (firstYAxisFound)
                return axis;
            else
                firstYAxisFound = true;
        }
    }

    return 0;
}

ChartType PlotArea::chartType() const
{
    return d->chartType;
}

ChartSubtype PlotArea::chartSubType() const
{
    return d->chartSubtype;
}

bool PlotArea::isThreeD() const
{
    return d->threeD;
}

bool PlotArea::isVertical() const
{
    return d->chartType == BarChartType && d->vertical;
}

Ko3dScene *PlotArea::threeDScene() const
{
    return d->threeDScene;
}

qreal PlotArea::angleOffset() const
{
    return d->angleOffset;
}

qreal PlotArea::holeSize() const
{
    return d->holeSize;
}

void PlotArea::setHoleSize(qreal value)
{
    d->holeSize = value;
}

// FIXME: this should add the axxis as a child (set axis->parent())
bool PlotArea::addAxis(Axis *axis)
{
    if (d->axes.contains(axis)) {
        warnChart << "PlotArea::addAxis(): Trying to add already added axis.";
        return false;
    }

    if (!axis) {
        warnChart << "PlotArea::addAxis(): Pointer to axis is NULL!";
        return false;
    }
    d->axes.append(axis);

    if (axis->dimension() == XAxisDimension) {
        // let each axis know about the other axis
        foreach (Axis *_axis, d->axes) {
            if (_axis->isVisible())
                _axis->registerAxis(axis);
        }
    }

    requestRepaint();

    return true;
}

bool PlotArea::removeAxis(Axis *axis)
{
    bool removed = takeAxis(axis);
    if (removed) {
        // This also removes the axis' title, which is a shape as well
        delete axis;
    }
    return removed;
}

// FIXME: this should remove the axis as a child (set axis->parent())
bool PlotArea::takeAxis(Axis *axis)
{
    if (!d->axes.contains(axis)) {
        warnChart << "PlotArea::takeAxis(): Trying to remove non-added axis.";
        return false;
    }
    if (!axis) {
        warnChart << "PlotArea::takeAxis(): Pointer to axis is NULL!";
        return false;
    }
    if (axis->title()) {
        d->automaticallyHiddenAxisTitles.removeAll(axis->title());
    }
    d->axes.removeAll(axis);
    axis->removeAxisFromDiagrams(true);
    requestRepaint();
    return true;
}

CoordinatePlaneList PlotArea::Private::coordinatePlanesForChartType(ChartType type)
{
    CoordinatePlaneList result;
    switch (type) {
    case BarChartType:
    case LineChartType:
    case AreaChartType:
    case ScatterChartType:
    case GanttChartType:
    case SurfaceChartType:
    case StockChartType:
    case BubbleChartType:
        result.append(kdCartesianPlanePrimary);
        result.append(kdCartesianPlaneSecondary);
        break;
    case CircleChartType:
    case RingChartType:
        result.append(kdPolarPlane);
        break;
    case RadarChartType:
    case FilledRadarChartType:
        result.append(kdRadarPlane);
        break;
    case LastChartType:
        Q_ASSERT("There's no coordinate plane for LastChartType");
        break;
    }

    Q_ASSERT(!result.isEmpty());
    return result;
}


void PlotArea::Private::autoHideAxisTitles()
{
    automaticallyHiddenAxisTitles.clear();
    foreach (Axis *axis, axes) {
        if (axis->title()->isVisible()) {
            axis->title()->setVisible(false);
            automaticallyHiddenAxisTitles.append(axis->title());
        }
    }
}

void PlotArea::setChartType(ChartType type)
{
    if (d->chartType == type)
        return;

    // Lots of things to do if the old and new types of coordinate
    // systems don't match.
    if (!isPolar(d->chartType) && isPolar(type)) {
        d->autoHideAxisTitles();
    }
    else if (isPolar(d->chartType) && !isPolar(type)) {
        foreach (KoShape *title, d->automaticallyHiddenAxisTitles) {
            title->setVisible(true);
        }
        d->automaticallyHiddenAxisTitles.clear();
    }
    CellRegion region = d->shape->proxyModel()->cellRangeAddress();
    if (type == CircleChartType || type == RingChartType) {
        d->shape->proxyModel()->setManualControl(false);
        xAxis()->clearDataSets();
        yAxis()->clearDataSets();
        if (secondaryYAxis()) {
            secondaryYAxis()->clearDataSets();
        }
        if (secondaryXAxis()) {
            secondaryXAxis()->clearDataSets();
        }
    }
    CoordinatePlaneList planesToRemove;
    // First remove secondary cartesian plane as it references the primary
    // plane, otherwise KChart will come down crashing on us. Note that
    // removing a plane that's not in the chart is not a problem.
    planesToRemove << d->kdCartesianPlaneSecondary << d->kdCartesianPlanePrimary
                   << d->kdPolarPlane << d->kdRadarPlane;
    foreach(KChart::AbstractCoordinatePlane *plane, planesToRemove)
        d->kdChart->takeCoordinatePlane(plane);
    CoordinatePlaneList newPlanes = d->coordinatePlanesForChartType(type);
    foreach(KChart::AbstractCoordinatePlane *plane, newPlanes)
        d->kdChart->addCoordinatePlane(plane);
    Q_ASSERT(d->kdChart->coordinatePlanes() == newPlanes);

    d->chartType = type;

    foreach (Axis *axis, d->axes) {
        axis->plotAreaChartTypeChanged(type);
    }
    if (type == CircleChartType || type == RingChartType) {
        d->shape->proxyModel()->reset(region);
    }
    if (type != BarChartType) {
        setVertical(false); // Only supported by bar charts
    }
    requestRepaint();
}

void PlotArea::setChartSubType(ChartSubtype subType)
{
    d->chartSubtype = subType;

    foreach (Axis *axis, d->axes) {
        axis->plotAreaChartSubTypeChanged(subType);
    }
}

void PlotArea::setThreeD(bool threeD)
{
    d->threeD = threeD;

    foreach(Axis *axis, d->axes)
        axis->setThreeD(threeD);

    requestRepaint();
}

void PlotArea::setVertical(bool vertical)
{
    d->vertical = vertical;
    foreach(Axis *axis, d->axes)
        axis->plotAreaIsVerticalChanged();
}

// ----------------------------------------------------------------
//                         loading and saving


bool PlotArea::loadOdf(const KoXmlElement &plotAreaElement,
                       KoShapeLoadingContext &context)
{
    KoStyleStack &styleStack = context.odfLoadingContext().styleStack();
    KoOdfStylesReader &stylesReader = context.odfLoadingContext().stylesReader();

    // The exact position defined in ODF overwrites the default layout position
    // NOTE: Do not do this as it means functionallity changes just because you save and load.
    // I don't think odf has an element/attribute that can hold this type of info.
    // Also afaics libreoffice do not do this.
//     if (plotAreaElement.hasAttributeNS(KoXmlNS::svg, "x") ||
//         plotAreaElement.hasAttributeNS(KoXmlNS::svg, "y") ||
//         plotAreaElement.hasAttributeNS(KoXmlNS::svg, "width") ||
//         plotAreaElement.hasAttributeNS(KoXmlNS::svg, "height"))
//     {
//         parent()->layout()->setPosition(this, FloatingPosition);
//     }

    bool autoPosition = !(plotAreaElement.hasAttributeNS(KoXmlNS::svg, "x") && plotAreaElement.hasAttributeNS(KoXmlNS::svg, "y"));
    bool autoSize = !(plotAreaElement.hasAttributeNS(KoXmlNS::svg, "width") && plotAreaElement.hasAttributeNS(KoXmlNS::svg, "height"));

    context.odfLoadingContext().fillStyleStack(plotAreaElement, KoXmlNS::chart, "style-name", "chart");
    loadOdfAttributes(plotAreaElement, context, OdfAllAttributes);

    // First step is to clear all old axis instances.
    while (!d->axes.isEmpty()) {
        Axis *axis = d->axes.takeLast();
        Q_ASSERT(axis);
        // Clear this axis of all data sets, deleting any diagram associated with it.
        axis->clearDataSets();
        if (axis->title())
            d->automaticallyHiddenAxisTitles.removeAll(axis->title());
        delete axis;
    }

    // Now find out about things that are in the plotarea style.
    //
    // These things include chart subtype, special things for some
    // chart types like line charts, stock charts, etc.
    //
    // Note that this has to happen BEFORE we create a axis and call
    // there loadOdf method cause the axis will evaluate settings
    // like the PlotArea::isVertical boolean.
    bool candleStick = false;
    if (plotAreaElement.hasAttributeNS(KoXmlNS::chart, "style-name")) {
        styleStack.clear();
        context.odfLoadingContext().fillStyleStack(plotAreaElement, KoXmlNS::chart, "style-name", "chart");

        styleStack.setTypeProperties("graphic");
        styleStack.setTypeProperties("chart");

        if (styleStack.hasProperty(KoXmlNS::chart, "auto-position")) {
            autoPosition |= styleStack.property(KoXmlNS::chart, "auto-position") == "true";
        } else {
            // To be backwards compatible we set auto-position to true as this was the original behaviour
            // and is the way LO works
            autoPosition = true;
        }
        if (styleStack.hasProperty(KoXmlNS::chart, "auto-size")) {
            autoSize |= styleStack.property(KoXmlNS::chart, "auto-size") == "true" ;
        } else {
            // To be backwards compatible we set auto-size to true as this was the original behaviour
            // and is the way LO works
            autoSize = true;
        }

        // ring and pie
        if (styleStack.hasProperty(KoXmlNS::chart, "angle-offset")) {
            bool ok;
            const qreal angleOffset = styleStack.property(KoXmlNS::chart, "angle-offset").toDouble(&ok);
            if (ok) {
                setAngleOffset(angleOffset);
            }
        }
        // ring
        if (styleStack.hasProperty(KoXmlNS::chart, "hole-size")) {
            bool ok;
            const qreal value = styleStack.property(KoXmlNS::chart, "hole-size").toDouble(&ok);
            if (ok) {
                setHoleSize(value);
            }
        }

        // Check for 3D.
        if (styleStack.hasProperty(KoXmlNS::chart, "three-dimensional"))
            setThreeD(styleStack.property(KoXmlNS::chart, "three-dimensional") == "true");
        d->threeDScene = load3dScene(plotAreaElement);

        // Set subtypes stacked or percent.
        // These are valid for Bar, Line, Area and Radar types.
        if (styleStack.hasProperty(KoXmlNS::chart, "percentage")
             && styleStack.property(KoXmlNS::chart, "percentage") == "true")
        {
            setChartSubType(PercentChartSubtype);
        }
        else if (styleStack.hasProperty(KoXmlNS::chart, "stacked")
                  && styleStack.property(KoXmlNS::chart, "stacked") == "true")
        {
            setChartSubType(StackedChartSubtype);
        }

        // Data specific to bar charts
        if (styleStack.hasProperty(KoXmlNS::chart, "vertical"))
            setVertical(styleStack.property(KoXmlNS::chart, "vertical") == "true");

        // Data specific to stock charts
        if (styleStack.hasProperty(KoXmlNS::chart, "japanese-candle-stick")) {
            candleStick = styleStack.property(KoXmlNS::chart, "japanese-candle-stick") == "true";
        }

        // Special properties for various chart types
#if 0
        switch () {
        case BarChartType:
            if (styleStack)
                ;
        }
#endif
        styleStack.clear();
        context.odfLoadingContext().fillStyleStack(plotAreaElement, KoXmlNS::chart, "style-name", "chart");
    }
    setAdditionalStyleAttribute("chart:auto-position", autoPosition ? "true" : "false");
    setAdditionalStyleAttribute("chart:auto-size", autoSize ? "true" : "false");

    // Now create and load the axis from the ODF. This needs to happen
    // AFTER we did set some of the basic settings above so the axis
    // can use those basic settings to evaluate it's own settings
    // depending on them. This is especially required for the
    // PlotArea::isVertical() boolean flag else things will go wrong.
    KoXmlElement n;
    forEachElement (n, plotAreaElement) {
        if (n.namespaceURI() != KoXmlNS::chart)
            continue;

        if (n.localName() == "axis") {
            if (!n.hasAttributeNS(KoXmlNS::chart, "dimension")) {
                // We have to know what dimension the axis is supposed to be..
                qInfo()<<Q_FUNC_INFO<<"No axis dimension";
                continue;
            }
            const QString dimension = n.attributeNS(KoXmlNS::chart, "dimension", QString());
            AxisDimension dim;
            if      (dimension == "x") dim = XAxisDimension;
            else if (dimension == "y") dim = YAxisDimension;
            else if (dimension == "z") dim = ZAxisDimension;
            else continue;
            Axis *axis = new Axis(this, dim);
            if (dim == YAxisDimension) {
                if (axis == yAxis()) {
                } else if (axis == secondaryYAxis()) {
                }
            }
            debugChartOdf<<"axis dimension"<<dimension<<dim;
            axis->loadOdf(n, context);
        }
    }

    // Two axes are mandatory, check that we have them.
    if (!xAxis()) {
        Axis *xAxis = new Axis(this, XAxisDimension);
        xAxis->setVisible(false);
    }
    if (!yAxis()) {
        Axis *yAxis = new Axis(this, YAxisDimension);
        yAxis->setVisible(false);
    }

    // Now, after the axes, load the datasets.
    // Note that this only contains properties of the datasets, the
    // actual data is not stored here.
    //
    // FIXME: Isn't the proxy model a strange place to store this data?
    proxyModel()->loadOdf(plotAreaElement, context, d->chartType);

    // Now load the surfaces (wall and possibly floor)
    // FIXME: Use named tags instead of looping?
    forEachElement (n, plotAreaElement) {
        if (n.namespaceURI() != KoXmlNS::chart)
            continue;

        if (n.localName() == "wall") {
            d->wall->loadOdf(n, context);
        }
        else if (n.localName() == "floor") {
            // The floor is not always present, so allocate it if needed.
            // FIXME: Load floor, even if we don't really support it yet
            // and save it back to ODF.
            //if (!d->floor)
            //    d->floor = new Surface(this);
            //d->floor->loadOdf(n, context);
        } else if (n.localName() == "stock-gain-marker") {
            styleStack.clear();
            context.odfLoadingContext().fillStyleStack(n, KoXmlNS::chart, "style-name", "chart");
            styleStack.setTypeProperties("graphic");
            if (styleStack.hasProperty(KoXmlNS::draw, "fill")) {
                d->stockGainBrush = KoOdfGraphicStyles::loadOdfFillStyle(styleStack, styleStack.property(KoXmlNS::draw, "fill"), stylesReader);
                debugChartOdf<<n.localName()<<d->stockGainBrush;
            } else {
                warnChartOdf<<n.localName()<<"Missing 'draw:fill' property in style"<<n.attributeNS(KoXmlNS::chart, "style-name");
            }
        } else if (n.localName() == "stock-loss-marker") {
            styleStack.clear();
            context.odfLoadingContext().fillStyleStack(n, KoXmlNS::chart, "style-name", "chart");
            styleStack.setTypeProperties("graphic");
            if (styleStack.hasProperty(KoXmlNS::draw, "fill")) {
                d->stockLossBrush = KoOdfGraphicStyles::loadOdfFillStyle(styleStack, styleStack.property(KoXmlNS::draw, "fill"), stylesReader);
                debugChartOdf<<n.localName()<<d->stockLossBrush;
            } else {
                warnChartOdf<<n.localName()<<"Missing 'draw:fill' property in style"<<n.attributeNS(KoXmlNS::chart, "style-name");
            }
        } else if (n.localName() == "stock-range-line") {
            styleStack.clear();
            context.odfLoadingContext().fillStyleStack(n, KoXmlNS::chart, "style-name", "chart");
            styleStack.setTypeProperties("graphic");
            if (styleStack.hasProperty(KoXmlNS::draw, "stroke")) {
                d->stockRangeLinePen = KoOdfGraphicStyles::loadOdfStrokeStyle(styleStack, styleStack.property(KoXmlNS::draw, "stroke"), stylesReader);
                debugChartOdf<<n.localName()<<d->stockRangeLinePen;
            } else {
                warnChartOdf<<n.localName()<<"Missing 'draw:stroke' property in style"<<n.attributeNS(KoXmlNS::chart, "style-name");
            }
        } else if (n.localName() != "axis" && n.localName() != "series") {
            warnChart << "PlotArea::loadOdf(): Unknown tag name " << n.localName();
        }
    }
    if (d->chartType == StockChartType) {
        // The number of data sets determines stock chart subtype
        if (proxyModel()->rowCount() > 3) {
            if (candleStick) {
                setChartSubType(CandlestickChartSubtype);
            } else {
                setChartSubType(OpenHighLowCloseChartSubtype);
            }
        }
    }

    // Connect axes to datasets and cleanup
    foreach(DataSet *ds, d->shape->proxyModel()->dataSets()) {
        foreach(Axis *axis, d->axes) {
            if (axis->name() == ds->axisName()) {
                axis->attachDataSet(ds);
            }
        }
    }
    debugChartOdf<<d->chartType<<d->chartSubtype<<d->axes;
    if (isPolar(d->chartType)) {
        d->autoHideAxisTitles();
    }
    foreach(Axis *axis, d->axes) {
        axis->setName(QString());
    }

    // update kchart axis position for all axes
    d->updateAxesPosition();
    // add axes titles to layout
    addAxesTitlesToLayout();

    return true;
}

void PlotArea::saveOdf(KoShapeSavingContext &context) const
{
    KoXmlWriter &bodyWriter = context.xmlWriter();
    //KoGenStyles &mainStyles = context.mainStyles();
    bodyWriter.startElement("chart:plot-area");

    KoGenStyle plotAreaStyle(KoGenStyle::ChartAutoStyle, "chart");

    // Data direction
    const Qt::Orientation direction = proxyModel()->dataDirection();
    plotAreaStyle.addProperty("chart:series-source",
                               (direction == Qt::Horizontal)
                               ? "rows" : "columns");

    // Save chart subtype
    saveOdfSubType(bodyWriter, plotAreaStyle);

    // Save extra stuff (like auto-position)
    QMap<QByteArray, QString>::const_iterator it(additionalStyleAttributes().constBegin());
    for (; it != additionalStyleAttributes().constEnd(); ++it) {
        plotAreaStyle.addProperty(it.key(), it.value(), KoGenStyle::ChartType);
    }

    // save graphic-properties and insert style
    bodyWriter.addAttribute("chart:style-name",
                             saveStyle(plotAreaStyle, context));

    const QSizeF s(size());
    const QPointF p(position());
    bodyWriter.addAttributePt("svg:width",  s.width());
    bodyWriter.addAttributePt("svg:height", s.height());
    bodyWriter.addAttributePt("svg:x", p.x());
    bodyWriter.addAttributePt("svg:y", p.y());

    CellRegion cellRangeAddress = d->shape->proxyModel()->cellRangeAddress();
    bodyWriter.addAttribute("table:cell-range-address", cellRangeAddress.toString());

    // About the data:
    //   Save if the first row / column contain headers.
    QString  dataSourceHasLabels;
    if (proxyModel()->firstRowIsLabel()) {
        if (proxyModel()->firstColumnIsLabel())
            dataSourceHasLabels = "both";
        else
            dataSourceHasLabels = "row";
    } else {
        if (proxyModel()->firstColumnIsLabel())
            dataSourceHasLabels = "column";
        else
            dataSourceHasLabels = "none";
    }
    // Note: this is saved in the plotarea attributes and not the style.
    bodyWriter.addAttribute("chart:data-source-has-labels", dataSourceHasLabels);

    if (d->threeDScene) {
        d->threeDScene->saveOdfAttributes(bodyWriter);
    }
    if (d->chartType == StockChartType) {
        QString styleName;

        bodyWriter.startElement("chart:stock-gain-marker");
        KoGenStyle stockGainStyle(KoGenStyle::ChartAutoStyle, "chart");
        KoOdfGraphicStyles::saveOdfFillStyle(stockGainStyle, context.mainStyles(), d->stockGainBrush);
        styleName = context.mainStyles().insert(stockGainStyle, "ch");
        bodyWriter.addAttribute("chart:style-name", styleName);
        bodyWriter.endElement(); // chart:stock-gain-marker

        bodyWriter.startElement("chart:stock-loss-marker");
        KoGenStyle stockLossStyle(KoGenStyle::ChartAutoStyle, "chart");
        KoOdfGraphicStyles::saveOdfFillStyle(stockLossStyle, context.mainStyles(), d->stockLossBrush);
        styleName = context.mainStyles().insert(stockLossStyle, "ch");
        bodyWriter.addAttribute("chart:style-name", styleName);
        bodyWriter.endElement(); // chart:stock-loss-marker

        bodyWriter.startElement("chart:stock-range-line");
        KoGenStyle stockRangeStyle(KoGenStyle::ChartAutoStyle, "chart");
        KoOdfGraphicStyles::saveOdfStrokeStyle(stockRangeStyle, context.mainStyles(), d->stockRangeLinePen);
        styleName = context.mainStyles().insert(stockRangeStyle, "ch");
        bodyWriter.addAttribute("chart:style-name", styleName);
        bodyWriter.endElement(); // chart:stock-range-line
    }

    // Done with the attributes, start writing the children.

    // Save the axes.
    foreach(Axis *axis, d->axes) {
        axis->saveOdf(context);
    }

    if (d->threeDScene) {
        d->threeDScene->saveOdfChildren(bodyWriter);
    }

    // Save data series
    d->shape->proxyModel()->saveOdf(context);

    // Save the floor and wall of the plotarea.
    d->wall->saveOdf(context, "chart:wall");
    //if (d->floor)
    //    d->floor->saveOdf(context, "chart:floor");

    bodyWriter.endElement(); // chart:plot-area
}

void PlotArea::saveOdfSubType(KoXmlWriter& xmlWriter,
                               KoGenStyle& plotAreaStyle) const
{
    Q_UNUSED(xmlWriter);

    switch (d->chartType) {
    case BarChartType:
        switch(d->chartSubtype) {
        case NoChartSubtype:
        case NormalChartSubtype:
            break;
        case StackedChartSubtype:
            plotAreaStyle.addProperty("chart:stacked", "true");
            break;
        case PercentChartSubtype:
            plotAreaStyle.addProperty("chart:percentage", "true");
            break;
        }

        if (d->threeD) {
            plotAreaStyle.addProperty("chart:three-dimensional", "true");
        }

        // Data specific to bar charts
        if (d->vertical)
            plotAreaStyle.addProperty("chart:vertical", "true");
        // Don't save this if zero, because that's the default.
        //plotAreaStyle.addProperty("chart:lines-used", 0); // FIXME: for now
        break;

    case LineChartType:
        switch(d->chartSubtype) {
        case NoChartSubtype:
        case NormalChartSubtype:
            break;
        case StackedChartSubtype:
            plotAreaStyle.addProperty("chart:stacked", "true");
            break;
        case PercentChartSubtype:
            plotAreaStyle.addProperty("chart:percentage", "true");
            break;
        }
        if (d->threeD) {
            plotAreaStyle.addProperty("chart:three-dimensional", "true");
            // FIXME: Save all 3D attributes too.
        }
        // FIXME: What does this mean?
        plotAreaStyle.addProperty("chart:symbol-type", "automatic");
        break;

    case AreaChartType:
        switch(d->chartSubtype) {
        case NoChartSubtype:
        case NormalChartSubtype:
            break;
        case StackedChartSubtype:
            plotAreaStyle.addProperty("chart:stacked", "true");
            break;
        case PercentChartSubtype:
            plotAreaStyle.addProperty("chart:percentage", "true");
            break;
        }

        if (d->threeD) {
            plotAreaStyle.addProperty("chart:three-dimensional", "true");
            // FIXME: Save all 3D attributes too.
        }
        break;

    case CircleChartType:
        plotAreaStyle.addProperty("chart:angle-offset", QString::number(d->angleOffset));
        break;

    case RingChartType:
        plotAreaStyle.addProperty("chart:angle-offset", QString::number(d->angleOffset));
        plotAreaStyle.addProperty("chart:hole-size", QString::number(d->holeSize));
        break;

    case ScatterChartType:
        // FIXME
        break;
    case RadarChartType:
    case FilledRadarChartType:
        // Save subtype of the Radar chart.
        switch(d->chartSubtype) {
        case NoChartSubtype:
        case NormalChartSubtype:
            break;
        case StackedChartSubtype:
            plotAreaStyle.addProperty("chart:stacked", "true");
            break;
        case PercentChartSubtype:
            plotAreaStyle.addProperty("chart:percentage", "true");
            break;
        }
        break;

    case StockChartType: {
        switch(d->chartSubtype) {
        case NoChartSubtype:
        case HighLowCloseChartSubtype:
        case OpenHighLowCloseChartSubtype:
            plotAreaStyle.addProperty("chart:japanese-candle-stick", "false");
            break;
        case CandlestickChartSubtype:
            plotAreaStyle.addProperty("chart:japanese-candle-stick", "true");
            break;
        }
    }
    case BubbleChartType:
    case SurfaceChartType:
    case GanttChartType:
        // FIXME
        break;

        // This is not a valid type, but needs to be handled to avoid
        // a warning from gcc.
    case LastChartType:
    default:
        // FIXME
        break;
    }
}

void PlotArea::setAngleOffset(qreal angle)
{
    d->angleOffset = angle;

    emit angleOffsetChanged(angle);
}

ChartShape *PlotArea::parent() const
{
    // There has to be a valid parent
    Q_ASSERT(d->shape);
    return d->shape;
}

KChart::CartesianCoordinatePlane *PlotArea::kdCartesianPlane(Axis *axis) const
{
    if (axis) {
        Q_ASSERT(d->axes.contains(axis));
        // Only a secondary y axis gets the secondary plane
        if (axis->dimension() == YAxisDimension && axis != yAxis())
            return d->kdCartesianPlaneSecondary;
    }

    return d->kdCartesianPlanePrimary;
}

KChart::PolarCoordinatePlane *PlotArea::kdPolarPlane() const
{
    return d->kdPolarPlane;
}

KChart::RadarCoordinatePlane *PlotArea::kdRadarPlane() const
{
    return d->kdRadarPlane;
}

KChart::Chart *PlotArea::kdChart() const
{
    return d->kdChart;
}

bool PlotArea::registerKdDiagram(KChart::AbstractDiagram *diagram)
{
    if (d->kdDiagrams.contains(diagram))
        return false;

    d->kdDiagrams.append(diagram);
    return true;
}

bool PlotArea::deregisterKdDiagram(KChart::AbstractDiagram *diagram)
{
    if (!d->kdDiagrams.contains(diagram))
        return false;

    d->kdDiagrams.removeAll(diagram);
    return true;
}

// HACK to get kdChart to recognize secondary planes
void PlotArea::registerKdPlane(KChart::AbstractCoordinatePlane *plane)
{
    int pos = d->kdChart->coordinatePlanes().indexOf(plane);
    if (pos >= 1) {
        // secondary plane
        d->kdChart->takeCoordinatePlane(plane);
        d->kdChart->insertCoordinatePlane(pos, plane);
    } else if (pos < 0) {
        d->kdChart->addCoordinatePlane(plane);
    }
}

void PlotArea::plotAreaUpdate()
{
    parent()->legend()->update();
    if (d->chartType == StockChartType) {
        updateKChartStockAttributes();
    }
    requestRepaint();
    foreach(Axis* axis, d->axes)
        axis->update();

    KoShape::update();
}

void PlotArea::requestRepaint() const
{
    d->pixmapRepaintRequested = true;
}

void PlotArea::paintPixmap(QPainter &painter, const KoViewConverter &converter)
{
    // Adjust the size of the painting area to the current zoom level
    const QSize paintRectSize = converter.documentToView(size()).toSize();
    const QSize plotAreaSize = size().toSize();
    const int borderX = 4;
    const int borderY = 4;

    // Only use a pixmap with sane sizes
    d->paintPixmap = false;//paintRectSize.width() < MAX_PIXMAP_SIZE || paintRectSize.height() < MAX_PIXMAP_SIZE;

    if (d->paintPixmap) {
        d->image = QImage(paintRectSize, QImage::Format_RGB32);

        // Copy the painter's render hints, such as antialiasing
        QPainter pixmapPainter(&d->image);
        pixmapPainter.setRenderHints(painter.renderHints());
        pixmapPainter.setRenderHint(QPainter::Antialiasing, false);

        // scale the painter's coordinate system to fit the current zoom level
        applyConversion(pixmapPainter, converter);

        d->kdChart->paint(&pixmapPainter, QRect(QPoint(borderX, borderY),
                                                QSize(plotAreaSize.width() - 2 * borderX,
                                                      plotAreaSize.height() - 2 * borderY)));
    } else {
        d->kdChart->paint(&painter, QRect(QPoint(borderX, borderY),
                                          QSize(plotAreaSize.width() - 2 * borderX,
                                                plotAreaSize.height() - 2 * borderY)));
    }
}

void PlotArea::paint(QPainter& painter, const KoViewConverter& converter, KoShapePaintingContext &paintContext)
{
    //painter.save();

    // First of all, scale the painter's coordinate system to fit the current zoom level
    applyConversion(painter, converter);

    // Calculate the clipping rect
    QRectF paintRect = QRectF(QPointF(0, 0), size());
    painter.setClipRect(paintRect, Qt::IntersectClip);

    // Paint the background
    if (background()) {
        QPainterPath p;
        p.addRect(paintRect);
        background()->paint(painter, converter, paintContext, p);
    }

    // Get the current zoom level
    QPointF zoomLevel;
    converter.zoom(&zoomLevel.rx(), &zoomLevel.ry());

    // Only repaint the pixmap if it is scheduled, the zoom level
    // changed or the shape was resized.
    /*if (   d->pixmapRepaintRequested
         || d->lastZoomLevel != zoomLevel
         || d->lastSize      != size()
         || !d->paintPixmap) {
        // TODO (js): What if two zoom levels are constantly being
        //            requested?  At the moment, this *is* the case,
        //            due to the fact that the shape is also rendered
        //            in the page overview in Stage. Every time
        //            the window is hidden and shown again, a repaint
        //            is requested --> laggy performance, especially
        //            when quickly switching through windows.
        //
        // ANSWER (iw): what about having a small mapping between size
        //              in pixels and pixmaps?  The size could be 2 or
        //              at most 3.  We could manage the replacing
        //              using LRU.
        paintPixmap(painter, converter);
        d->pixmapRepaintRequested = false;
        d->lastZoomLevel = zoomLevel;
        d->lastSize      = size();
    }*/
    painter.setRenderHint(QPainter::Antialiasing, false);

    // KChart thinks in pixels, Calligra in pt
    ScreenConversions::scaleFromPtToPx(painter);

    // Only paint the actual chart if there is a certain minimal size,
    // because otherwise kdchart will crash.
    QRect kdchartRect = ScreenConversions::scaleFromPtToPx(paintRect, painter);
    // Turn off clipping so that border (or "frame") drawn by KChart::Chart
    // is not not cut off.
    painter.setClipping(false);
    if (kdchartRect.width() > 10 && kdchartRect.height() > 10) {
        d->kdChart->paint(&painter, kdchartRect);
    }
    //painter.restore();

    // Paint the cached pixmap if we got a GO from paintPixmap()
    //if (d->paintPixmap)
    //    painter.drawImage(0, 0, d->image);
}

void PlotArea::relayout() const
{
    d->kdCartesianPlanePrimary->relayout();
    d->kdCartesianPlaneSecondary->relayout();
    d->kdPolarPlane->relayout();
    d->kdRadarPlane->relayout();
    update();
}

void PlotArea::addTitleToLayout()
{
    addAxesTitlesToLayout(); // for now
}

void PlotArea::addAxesTitlesToLayout()
{
    ChartLayout *layout = d->shape->layout();
    Axis *axis = xAxis();
    if (axis) {
        layout->remove(axis->title());
        layout->setItemType(axis->title(), XAxisTitleType);
    }
    axis = yAxis();
    if (axis) {
        layout->remove(axis->title());
        layout->setItemType(axis->title(), YAxisTitleType);
    }
    axis = secondaryXAxis();
    if (axis) {
        layout->remove(axis->title());
        layout->setItemType(axis->title(), SecondaryXAxisTitleType);
    }
    axis = secondaryYAxis();
    if (axis) {
        layout->remove(axis->title());
        layout->setItemType(axis->title(), SecondaryYAxisTitleType);
    }
}

void PlotArea::setStockRangeLinePen(const QPen &pen)
{
    d->stockRangeLinePen = pen;
}

QPen PlotArea::stockRangeLinePen() const
{
    return d->stockRangeLinePen;
}

void PlotArea::setStockGainBrush(const QBrush &brush)
{
    d->stockGainBrush = brush;
}

QBrush PlotArea::stockGainBrush() const
{
    return d->stockGainBrush;
}

void PlotArea::setStockLossBrush(const QBrush &brush)
{
    d->stockLossBrush = brush;
}

QBrush PlotArea::stockLossBrush() const
{
    return d->stockLossBrush;
}

void PlotArea::updateKChartStockAttributes()
{
    for (Axis *a : d->axes) {
        a->updateKChartStockAttributes();
    }
}

DataSet::ValueLabelType PlotArea::valueLabelType() const
{
    return d->valueLabelType;
}

QString PlotArea::symbolType() const
{
    return d->symbolType;
}

void PlotArea::setSymbolType(const QString &type)
{
    d->symbolType = type;
}

QString PlotArea::symbolName() const
{
    return d->symbolName;
}

void PlotArea::setSymbolName(const QString &name)
{
    d->symbolName = name;
}

void PlotArea::setValueLabelType(const DataSet::ValueLabelType &type)
{
    d->valueLabelType = type;
}
