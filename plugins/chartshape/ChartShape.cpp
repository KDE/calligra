/* This file is part of the KDE project

   Copyright 2007 Stefan Nikolaus     <stefan.nikolaus@kdemail.net>
   Copyright 2007-2010 Inge Wallin    <inge@lysator.liu.se>
   Copyright 2007-2008 Johannes Simon <johannes.simon@gmail.com>
   Copyright 2017 Dag Andersen <danders@get2net.dk>

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
#include "ChartShape.h"

// Posix
#include <float.h> // For basic data types characteristics.

// Qt
#include <QPointF>
#include <QPainter>
#include <QPainterPath>
#include <QSizeF>
#include <QTextDocument>
#include <QUrl>

// KF5
#include <kmessagebox.h>

// KChart
#include <KChartChart>
#include <KChartAbstractDiagram>
#include <KChartCartesianAxis>
#include <KChartCartesianCoordinatePlane>
#include <KChartPolarCoordinatePlane>
#include "KChartConvertions.h"
// Attribute Classes
#include <KChartDataValueAttributes>
#include <KChartGridAttributes>
#include <KChartTextAttributes>
#include <KChartMarkerAttributes>
#include <KChartThreeDPieAttributes>
#include <KChartThreeDBarAttributes>
#include <KChartThreeDLineAttributes>
// Diagram Classes
#include <KChartBarDiagram>
#include <KChartPieDiagram>
#include <KChartLineDiagram>
#include <KChartRingDiagram>
#include <KChartPolarDiagram>

// Calligra
#include <KoShapeLoadingContext.h>
#include <KoOdfLoadingContext.h>
#include <KoEmbeddedDocumentSaver.h>
#include <KoStore.h>
#include <KoDocument.h>
#include <KoShapeSavingContext.h>
#include <KoViewConverter.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>
#include <KoXmlNS.h>
#include <KoGenStyles.h>
#include <KoStyleStack.h>
#include <KoShapeRegistry.h>
#include <KoTextShapeData.h>
#include <KoTextDocumentLayout.h>
#include <KoCanvasBase.h>
#include <KoShapeManager.h>
#include <KoSelection.h>
#include <KoShapeBackground.h>
#include <KoInsets.h>
#include <KoShapeStrokeModel.h>
#include <KoColorBackground.h>
#include <KoShapeStroke.h>
#include <KoOdfWorkaround.h>
#include <KoTextDocument.h>
#include <KoUnit.h>
#include <KoShapePaintingContext.h>
#include <KoTextShapeDataBase.h>
#include <KoCanvasResourceIdentities.h>

// KoChart
#include "Axis.h"
#include "DataSet.h"
#include "Legend.h"
#include "PlotArea.h"
#include "Surface.h"
#include "ChartProxyModel.h"
#include "TextLabelDummy.h"
#include "ChartDocument.h"
#include "ChartTableModel.h"
#include "ChartLayout.h"
#include "TableSource.h"
#include "OdfLoadingHelper.h"
#include "SingleModelHelper.h"
#include "OdfHelper.h"
#include "ChartDebug.h"


// Define the protocol used here for embedded documents' URL
// This used to "store" but KUrl didn't like it,
// so let's simply make it "tar" !
#define STORE_PROTOCOL "tar"
#define INTERNAL_PROTOCOL "intern"

namespace KoChart {

/// @see ChartShape::setEnableUserInteraction()
static bool ENABLE_USER_INTERACTION = true;

static const char * const ODF_CHARTTYPES[NUM_CHARTTYPES] = {
    "chart:bar",
    "chart:line",
    "chart:area",
    "chart:circle",
    "chart:ring",
    "chart:scatter",
    "chart:radar",
    "chart:filled-radar",
    "chart:stock",
    "chart:bubble",
    "chart:surface",
    "chart:gantt"
};

static const ChartSubtype defaultSubtypes[NUM_CHARTTYPES] = {
    NormalChartSubtype,         // Bar
    NormalChartSubtype,         // Line
    NormalChartSubtype,         // Area
    NoChartSubtype,             // Circle
    NoChartSubtype,             // Ring
    NoChartSubtype,             // Scatter
    NormalChartSubtype,         // Radar
    NormalChartSubtype,         // Filled Radar
    HighLowCloseChartSubtype,   // Stock
    NoChartSubtype,             // Bubble
    NoChartSubtype,             // Surface
    NoChartSubtype              // Gantt
};

const char * odfCharttype(int charttype)
{
    Q_ASSERT(charttype < LastChartType);
    if (charttype >= LastChartType || charttype < 0) {
        charttype = 0;
    }
    return ODF_CHARTTYPES[charttype];
}


static const int NUM_DEFAULT_DATASET_COLORS = 12;

static const char * const defaultDataSetColors[NUM_DEFAULT_DATASET_COLORS] =
{
    "#004586",
    "#ff420e",
    "#ffd320",
    "#579d1c",
    "#7e0021",
    "#83caff",
    "#314004",
    "#aecf00",
    "#4b1f6f",
    "#ff950e",
    "#c5000b",
    "#0084d1",
};

QColor defaultDataSetColor(int dataSetNum)
{
    dataSetNum %= NUM_DEFAULT_DATASET_COLORS;
    return QColor(defaultDataSetColors[dataSetNum]);
}

// ================================================================
//                     The Private class


class ChartShape::Private
{
public:
    Private(ChartShape *shape);
    ~Private();

    void setChildVisible(KoShape *label, bool doShow);

    // The components of a chart
    KoShape   *title;
    KoShape   *subTitle;
    KoShape   *footer;
    Legend    *legend;
    PlotArea  *plotArea;

    // Data
    ChartProxyModel     *proxyModel;	 /// What's presented to KChart
    ChartTableModel  *internalModel;
    TableSource          tableSource;
    SingleModelHelper   *internalModelHelper;

    bool usesInternalModelOnly; /// @see usesInternalModelOnly()

    ChartDocument *document;

    ChartShape *shape;		// The chart that owns this ChartShape::Private

    KoDocumentResourceManager *resourceManager;
};


ChartShape::Private::Private(ChartShape *shape)
    : internalModel(0)
    , internalModelHelper(0)
    , resourceManager(0)

{
    // Register the owner.
    this->shape = shape;

    // Components
    title    = 0;
    subTitle = 0;
    footer   = 0;
    legend   = 0;
    plotArea = 0;

    // Data
    proxyModel    = 0;

    // If not explicitly set otherwise, this chart provides its own data.
    usesInternalModelOnly = true;

    document = 0;
}

ChartShape::Private::~Private()
{
}


//
// Show a child, which means either the Title, Subtitle, Footer or Axis Title.
//
// If there is too little room, then make space by shrinking the Plotarea.
//
void ChartShape::Private::setChildVisible(KoShape *child, bool doShow)
{
    Q_ASSERT(child);

    if (child->isVisible() == doShow)
        return;

    child->setVisible(doShow);
    // FIXME: Shouldn't there be a KoShape::VisibilityChanged for KoShape::shapeChanged()?
    shape->layout()->scheduleRelayout();
}


// ================================================================
//                         Class ChartShape
// ================================================================


ChartShape::ChartShape(KoDocumentResourceManager *resourceManager)
    : KoFrameShape(KoXmlNS::draw, "object")
    , KoShapeContainer(new ChartLayout)
    , d (new Private(this))
{
    d->resourceManager = resourceManager;
    setShapeId(ChartShapeId);

    // Instantiated all children first
    d->proxyModel = new ChartProxyModel(this, &d->tableSource);

    d->plotArea = new PlotArea(this);
    d->document = new ChartDocument(this);
    d->legend   = new Legend(this);

    // Configure the plotarea.
    // We need this as the very first step, because some methods
    // here rely on the d->plotArea pointer.
    addShape(d->plotArea);
    d->plotArea->plotAreaInit();
    d->plotArea->setZIndex(0);
    setClipped(d->plotArea, true);
    setInheritsTransform(d->plotArea, true);
    d->plotArea->setDeletable(false);
    d->plotArea->setToolDelegates(QSet<KoShape*>()<<this); // Enable chart tool
    d->plotArea->setAllowedInteraction(KoShape::ShearingAllowed, false);

    // Configure the legend.
    d->legend->setVisible(true);
    d->legend->setZIndex(1);
    setClipped(d->legend, true);
    setInheritsTransform(d->legend, true);
    d->legend->setDeletable(false);
    d->legend->setToolDelegates(QSet<KoShape*>()<<this); // Enable chart tool
    d->legend->setAllowedInteraction(KoShape::ShearingAllowed, false);

    // A few simple defaults (chart type and subtype in this case)
    setChartType(BarChartType);
    setChartSubType(NormalChartSubtype);

    // Create the Title, which is a standard TextShape.
    KoShapeFactoryBase *textShapeFactory = KoShapeRegistry::instance()->value(TextShapeId);
    if (textShapeFactory)
        d->title = textShapeFactory->createDefaultShape(resourceManager);
    // Potential problem 1) No TextShape installed
    if (!d->title) {
        d->title = new TextLabelDummy;
        if (ENABLE_USER_INTERACTION)
            KMessageBox::error(0, i18n("The plugin needed for displaying text labels in a chart is not available."),
                                   i18n("Plugin Missing"));
    // Potential problem 2) TextShape incompatible
    } else if (dynamic_cast<TextLabelData*>(d->title->userData()) == 0 &&
                ENABLE_USER_INTERACTION)
            KMessageBox::error(0, i18n("The plugin needed for displaying text labels is not compatible with the current version of the chart Flake shape."),
                                   i18n("Plugin Incompatible"));

    // In both cases we need a KoTextShapeData instance to function. This is
    // enough for unit tests, so there has to be no TextShape plugin doing the
    // actual text rendering, we just need KoTextShapeData which is in the libs.
    TextLabelData *labelData = dynamic_cast<TextLabelData*>(d->title->userData());
    if (labelData == 0) {
        labelData = new TextLabelData;
        KoTextDocumentLayout *documentLayout = new KoTextDocumentLayout(labelData->document());
        labelData->document()->setDocumentLayout(documentLayout);
        d->title->setUserData(labelData);
    }

    // Add the title to the shape
    addShape(d->title);
    QFont font = titleData()->document()->defaultFont();
    font.setPointSizeF(12.0);
    titleData()->document()->setDefaultFont(font);
    titleData()->document()->setPlainText(i18n("Title"));
    // Set a reasonable size, it will be resized automatically
    d->title->setSize(QSizeF(CM_TO_POINT(5), CM_TO_POINT(0.7)));
    d->title->setVisible(false);
    d->title->setZIndex(2);
    setClipped(d->title, true);
    setInheritsTransform(d->title, true);
    d->title->setDeletable(false);
    d->title->setToolDelegates(QSet<KoShape*>()<<this<<d->title); // Enable chart tool
    labelData->setResizeMethod(KoTextShapeDataBase::AutoResize);
    d->title->setAdditionalStyleAttribute("chart:auto-position", "true");
    d->title->setAllowedInteraction(KoShape::ShearingAllowed, false);

    // Create the Subtitle and add it to the shape.
    if (textShapeFactory)
        d->subTitle = textShapeFactory->createDefaultShape(resourceManager);
    if (!d->subTitle) {
        d->subTitle = new TextLabelDummy;
    }
    labelData = dynamic_cast<TextLabelData*>(d->subTitle->userData());
    if (labelData == 0) {
        labelData = new TextLabelData;
        KoTextDocumentLayout *documentLayout = new KoTextDocumentLayout(labelData->document());
        labelData->document()->setDocumentLayout(documentLayout);
        d->subTitle->setUserData(labelData);
    }
    addShape(d->subTitle);
    font = subTitleData()->document()->defaultFont();
    font.setPointSizeF(10.0);
    subTitleData()->document()->setDefaultFont(font);
    subTitleData()->document()->setPlainText(i18n("Subtitle"));
    // Set a reasonable size, it will be resized automatically
    d->subTitle->setSize(QSizeF(CM_TO_POINT(5), CM_TO_POINT(0.7)));
    d->subTitle->setVisible(false);
    d->subTitle->setZIndex(3);
    setClipped(d->subTitle, true);
    setInheritsTransform(d->subTitle, true);
    d->subTitle->setDeletable(false);
    d->subTitle->setToolDelegates(QSet<KoShape*>()<<this<<d->subTitle); // Enable chart tool
    labelData->setResizeMethod(KoTextShapeDataBase::AutoResize);
    d->subTitle->setAdditionalStyleAttribute("chart:auto-position", "true");
    d->subTitle->setAllowedInteraction(KoShape::ShearingAllowed, false);

    // Create the Footer and add it to the shape.
    if (textShapeFactory)
        d->footer = textShapeFactory->createDefaultShape(resourceManager);
    if (!d->footer) {
        d->footer = new TextLabelDummy;
    }
    labelData = dynamic_cast<TextLabelData*>(d->footer->userData());
    if (labelData == 0) {
        labelData = new TextLabelData;
        KoTextDocumentLayout *documentLayout = new KoTextDocumentLayout(labelData->document());
        labelData->document()->setDocumentLayout(documentLayout);
        d->footer->setUserData(labelData);
    }
    addShape(d->footer);
    font = footerData()->document()->defaultFont();
    font.setPointSizeF(10.0);
    footerData()->document()->setDefaultFont(font);
    footerData()->document()->setPlainText(i18n("Footer"));
    // Set a reasonable size, it will be resized automatically
    d->footer->setSize(QSizeF(CM_TO_POINT(5), CM_TO_POINT(0.7)));
    d->footer->setVisible(false);
    d->footer->setZIndex(4);
    setClipped(d->footer, true);
    setInheritsTransform(d->footer, true);
    d->footer->setDeletable(false);
    d->footer->setToolDelegates(QSet<KoShape*>()<<this<<d->footer); // Enable chart tool
    labelData->setResizeMethod(KoTextShapeDataBase::AutoResize);
    d->footer->setAdditionalStyleAttribute("chart:auto-position", "true");
    d->footer->setAllowedInteraction(KoShape::ShearingAllowed, false);

    // Set default contour (for how text run around is done around this shape)
    // to prevent a crash in LO
    setTextRunAroundContour(KoShape::ContourBox);

    QSharedPointer<KoColorBackground> background(new KoColorBackground(Qt::white));
    setBackground(background);

    KoShapeStroke *stroke = new KoShapeStroke(0, Qt::black);
    setStroke(stroke);

    setSize(QSizeF(CM_TO_POINT(8), CM_TO_POINT(5)));

    // Tell layout about item types
    ChartLayout *l = layout();
    l->setItemType(d->plotArea, PlotAreaType);
    l->setItemType(d->title, TitleLabelType);
    l->setItemType(d->subTitle, SubTitleLabelType);
    l->setItemType(d->footer, FooterLabelType);
    l->setItemType(d->legend, LegendType);
    l->layout();
    requestRepaint();
}

ChartShape::~ChartShape()
{
    delete d->title;
    delete d->subTitle;
    delete d->footer;

    delete d->legend;
    delete d->plotArea;

    delete d->proxyModel;

    delete d->document;

    delete d;
}

ChartProxyModel *ChartShape::proxyModel() const
{
    return d->proxyModel;
}

KoShape *ChartShape::title() const
{
    return d->title;
}

TextLabelData *ChartShape::titleData() const
{
    TextLabelData *data = qobject_cast<TextLabelData*>(d->title->userData());
    return data;
}


KoShape *ChartShape::subTitle() const
{
    return d->subTitle;
}

TextLabelData *ChartShape::subTitleData() const
{
    TextLabelData *data = qobject_cast<TextLabelData*>(d->subTitle->userData());
    return data;
}

KoShape *ChartShape::footer() const
{
    return d->footer;
}

TextLabelData *ChartShape::footerData() const
{
    TextLabelData *data = qobject_cast<TextLabelData*>(d->footer->userData());
    return data;
}

QList<KoShape*> ChartShape::labels() const
{
    QList<KoShape*> labels;
    labels.append(d->title);
    labels.append(d->footer);
    labels.append(d->subTitle);
    foreach(Axis *axis, plotArea()->axes()) {
        labels.append(axis->title());
    }
    return labels;
}

Legend *ChartShape::legend() const
{
    // There has to be a valid legend even, if it's hidden.
    Q_ASSERT(d->legend);
    return d->legend;
}

PlotArea *ChartShape::plotArea() const
{
    return d->plotArea;
}

ChartLayout *ChartShape::layout() const
{
    ChartLayout *l = dynamic_cast<ChartLayout*>(KoShapeContainer::model());
    Q_ASSERT(l);
    return l;
}


void ChartShape::showTitle(bool doShow)
{
    d->setChildVisible(d->title, doShow);
}

void ChartShape::showSubTitle(bool doShow)
{
    d->setChildVisible(d->subTitle, doShow);
}

void ChartShape::showFooter(bool doShow)
{
    d->setChildVisible(d->footer, doShow);
}

ChartTableModel *ChartShape::internalModel() const
{
    return d->internalModel;
}

void ChartShape::setInternalModel(ChartTableModel *model)
{
    Table *table = d->tableSource.get(model);
    Q_ASSERT(table);
    delete d->internalModelHelper;
    delete d->internalModel;
    d->internalModel = model;
    d->internalModelHelper = new SingleModelHelper(table, d->proxyModel);
}

TableSource *ChartShape::tableSource() const
{
    return &d->tableSource;
}

bool ChartShape::usesInternalModelOnly() const
{
    return d->usesInternalModelOnly;
}

void ChartShape::setUsesInternalModelOnly(bool doesSo)
{
    d->usesInternalModelOnly = doesSo;
}


// ----------------------------------------------------------------
//                         getters and setters


ChartType ChartShape::chartType() const
{
    Q_ASSERT(d->plotArea);
    return d->plotArea->chartType();
}

ChartSubtype ChartShape::chartSubType() const
{
    Q_ASSERT(d->plotArea);
    return d->plotArea->chartSubType();
}

bool ChartShape::isThreeD() const
{
    Q_ASSERT(d->plotArea);
    return d->plotArea->isThreeD();
}

void ChartShape::setSheetAccessModel(QAbstractItemModel *model)
{
    d->tableSource.setSheetAccessModel(model);
}

void ChartShape::reset(const QString &region,
                       bool firstRowIsLabel,
                       bool firstColumnIsLabel,
                       Qt::Orientation dataDirection)
{
    // This method is provided via KoChartInterface, which is
    // used by embedding applications.
    d->usesInternalModelOnly = false;
    d->proxyModel->setFirstRowIsLabel(firstRowIsLabel);
    d->proxyModel->setFirstColumnIsLabel(firstColumnIsLabel);
    d->proxyModel->setDataDirection(dataDirection);
    d->proxyModel->reset(CellRegion(&d->tableSource, region));
}

void ChartShape::setChartType(ChartType type)
{
    Q_ASSERT(d->plotArea);
    ChartType prev = chartType();
    d->proxyModel->setDataDimensions(numDimensions(type));
    d->plotArea->setChartType(type);
    emit chartTypeChanged(type, prev);
}

void ChartShape::setChartSubType(ChartSubtype subType, bool reset)
{
    Q_ASSERT(d->plotArea);
    ChartSubtype prev = d->plotArea->chartSubType();
    d->plotArea->setChartSubType(subType);
    if (reset && chartType() == StockChartType && prev != subType && d->internalModel  && d->usesInternalModelOnly) {
        // HACK to get reasonable behaviour in most cases
        // Stock charts are special because subtypes interpretes data differently from another:
        // - HighLowCloseChartSubtype assumes High = row 0, Low = row 1 and Close = row 2
        // - The other types assumes Open = row 0,  High = row 1, Low = row 2 and Close = row 3
        // This makes switching between them a bit unintuitive.
        if (subType == HighLowCloseChartSubtype && d->internalModel->rowCount() > 3) {
            d->proxyModel->removeRows(0, 1); // remove Open
        } else {
            // just reset and hope for the best
            CellRegion region(d->tableSource.get(d->internalModel), QRect(1, 1, d->internalModel->columnCount(), d->internalModel->rowCount()));
            d->proxyModel->reset(region);
        }
    }
    emit updateConfigWidget();
}

void ChartShape::setThreeD(bool threeD)
{
    Q_ASSERT(d->plotArea);
    d->plotArea->setThreeD(threeD);
}


// ----------------------------------------------------------------


void ChartShape::paintComponent(QPainter &painter,
                                const KoViewConverter &converter, KoShapePaintingContext &paintContext)
{
    // Only does a relayout if scheduled
    layout()->layout();

    // Paint the background
    applyConversion(painter, converter);
    if (background()) {
        // Calculate the clipping rect
        QRectF paintRect = QRectF(QPointF(0, 0), size());
        painter.setClipRect(paintRect, Qt::IntersectClip);

        QPainterPath p;
        p.addRect(paintRect);
        background()->paint(painter, converter, paintContext, p);
    }
    // Paint border if showTextShapeOutlines is set
    // This means that it will be painted in words but not eg in sheets
    if (paintContext.showTextShapeOutlines) {
        if (qAbs(rotation()) > 1) {
            painter.setRenderHint(QPainter::Antialiasing);
        }
        QPen pen(QColor(210, 210, 210), 0); // use cosmetic pen
        QPointF onePixel = converter.viewToDocument(QPointF(1.0, 1.0));
        QRectF rect(QPointF(0.0, 0.0), size() - QSizeF(onePixel.x(), onePixel.y()));
        painter.setPen(pen);
        painter.drawRect(rect);
    }
}

void ChartShape::paintDecorations(QPainter &painter,
                                  const KoViewConverter &converter,
                                  const KoCanvasBase *canvas)
{
    // This only is a helper decoration, do nothing if we're already
    // painting handles anyway.
    Q_ASSERT(canvas);
    if (canvas->shapeManager()->selection()->selectedShapes().contains(this))
        return;

    if (stroke())
        return;

    QRectF border = QRectF(QPointF(-1.5, -1.5),
                           converter.documentToView(size()) + QSizeF(1.5, 1.5));

    painter.setPen(QPen(Qt::lightGray, 0));
    painter.drawRect(border);
}


// ----------------------------------------------------------------
//                         Loading and Saving


bool ChartShape::loadEmbeddedDocument(KoStore *store,
                                      const KoXmlElement &objectElement,
                                      const KoOdfLoadingContext &loadingContext)
{
    if (!objectElement.hasAttributeNS(KoXmlNS::xlink, "href")) {
        errorChart << "Object element has no valid xlink:href attribute";
        return false;
    }

    QString url = objectElement.attributeNS(KoXmlNS::xlink, "href");

    // It can happen that the url is empty e.g. when it is a
    // presentation:placeholder.
    if (url.isEmpty()) {
        return true;
    }

    QString tmpURL;
    if (url[0] == '#')
        url.remove(0, 1);

    if (QUrl::fromUserInput(url).isRelative()) {
        if (url.startsWith(QLatin1String("./")))
            tmpURL = QString(INTERNAL_PROTOCOL) + ":/" + url.mid(2);
        else
            tmpURL = QString(INTERNAL_PROTOCOL) + ":/" + url;
    }
    else
        tmpURL = url;

    QString path = tmpURL;
    if (tmpURL.startsWith(INTERNAL_PROTOCOL)) {
        path = store->currentPath();
        if (!path.isEmpty() && !path.endsWith('/'))
            path += '/';
        QString relPath = QUrl::fromUserInput(tmpURL).path();
        path += relPath.mid(1); // remove leading '/'
    }
    if (!path.endsWith('/'))
        path += '/';

    const QString mimeType = loadingContext.mimeTypeForPath(path);
    //debugChart << "path for manifest file=" << path << "mimeType=" << mimeType;
    if (mimeType.isEmpty()) {
        //debugChart << "Manifest doesn't have media-type for" << path;
        return false;
    }

    const bool isOdf = mimeType.startsWith(QLatin1String("application/vnd.oasis.opendocument"));
    if (!isOdf) {
        tmpURL += "/maindoc.xml";
        //debugChart << "tmpURL adjusted to" << tmpURL;
    }

    //debugChart << "tmpURL=" << tmpURL;

    bool res = true;
    if (tmpURL.startsWith(STORE_PROTOCOL)
         || tmpURL.startsWith(INTERNAL_PROTOCOL)
         || QUrl::fromUserInput(tmpURL).isRelative())
    {
        if (isOdf) {
            store->pushDirectory();
            Q_ASSERT(tmpURL.startsWith(INTERNAL_PROTOCOL));
            QString relPath = QUrl::fromUserInput(tmpURL).path().mid(1);
            store->enterDirectory(relPath);
            res = d->document->loadOasisFromStore(store);
            store->popDirectory();
        } else {
            if (tmpURL.startsWith(INTERNAL_PROTOCOL))
                tmpURL = QUrl::fromUserInput(tmpURL).path().mid(1);
            res = d->document->loadFromStore(store, tmpURL);
        }
        d->document->setStoreInternal(true);
    }
    else {
        // Reference to an external document. Hmmm...
        d->document->setStoreInternal(false);
        QUrl url = QUrl::fromUserInput(tmpURL);
        if (!url.isLocalFile()) {
            //QApplication::restoreOverrideCursor();

            // For security reasons we need to ask confirmation if the
            // url is remote.
            int result = KMessageBox::warningYesNoCancel(
                0, i18n("This document contains an external link to a remote document\n%1", tmpURL),
                i18n("Confirmation Required"), KGuiItem(i18n("Download")), KGuiItem(i18n("Skip")));

            if (result == KMessageBox::Cancel) {
                //d->m_parent->setErrorMessage("USER_CANCELED");
                return false;
            }
            if (result == KMessageBox::Yes)
                res = d->document->openUrl(url);
            // and if == No, res will still be false so we'll use a kounavail below
        }
        else
            res = d->document->openUrl(url);
    }

    if (!res) {
        QString errorMessage = d->document->errorMessage();
        return false;
    }
        // Still waiting...
        //QApplication::setOverrideCursor(Qt::WaitCursor);

    tmpURL.clear();

   //QApplication::restoreOverrideCursor();

    return res;
}

bool ChartShape::loadOdf(const KoXmlElement &element,
                         KoShapeLoadingContext &context)
{
    //struct Timer{QTime t;Timer(){t.start();} ~Timer(){debugChart<<">>>>>"<<t.elapsed();}} timer;

    // Load common attributes of (frame) shapes.  If you change here,
    // don't forget to also change in saveOdf().
    loadOdfAttributes(element, context, OdfAllAttributes);
    bool r = loadOdfFrame(element, context);

    return r;
}

// Used to load the actual contents from the ODF frame that surrounds
// the chart in the ODF file.
bool ChartShape::loadOdfFrameElement(const KoXmlElement &element,
                                     KoShapeLoadingContext &context)
{
    if (element.tagName() == "object")
        return loadEmbeddedDocument(context.odfLoadingContext().store(),
                                    element,
                                    context.odfLoadingContext());

    warnChart << "Unknown frame element <" << element.tagName() << ">";
    return false;
}

bool ChartShape::loadOdfChartElement(const KoXmlElement &chartElement,
                                     KoShapeLoadingContext &context)
{
    // Use a helper-class created on the stack to be sure a we always leave
    // this method with a call to endLoading proxyModel()->endLoading()
    struct ProxyModelLoadState {
        ChartProxyModel *m;
        ChartLayout *l;
        ProxyModelLoadState(ChartProxyModel *m, ChartLayout *l) : m(m), l(l) { m->beginLoading(); l->setLayoutingEnabled(false); }
        ~ProxyModelLoadState() { m->endLoading(); l->setLayoutingEnabled(true); }
    };
    ProxyModelLoadState proxyModelLoadState(proxyModel(), layout());

    // The shared data will automatically be deleted in the destructor
    // of KoShapeLoadingContext
    OdfLoadingHelper *helper = new OdfLoadingHelper;
    helper->tableSource = &d->tableSource;
    helper->chartUsesInternalModelOnly = d->usesInternalModelOnly;

    // Get access to sheets in Calligra Sheets
    QAbstractItemModel *sheetAccessModel = 0;
    if (resourceManager() && resourceManager()->hasResource(Sheets::CanvasResource::AccessModel)) {
        QVariant var = resourceManager()->resource(Sheets::CanvasResource::AccessModel);
        sheetAccessModel = static_cast<QAbstractItemModel*>(var.value<void*>());
        if (sheetAccessModel) {
            // We're embedded in Calligra Sheets, which means Calligra Sheets provides the data
            d->usesInternalModelOnly = false;
            d->tableSource.setSheetAccessModel(sheetAccessModel);
            helper->chartUsesInternalModelOnly = d->usesInternalModelOnly;
        }
    }
    context.addSharedData(OdfLoadingHelperId, helper);

    KoStyleStack &styleStack = context.odfLoadingContext().styleStack();
    styleStack.clear();
    if (chartElement.hasAttributeNS(KoXmlNS::chart, "style-name")) {
        context.odfLoadingContext().fillStyleStack(chartElement, KoXmlNS::chart, "style-name", "chart");

        styleStack.setTypeProperties("graphic");
        KoInsets padding = layout()->padding();
        if (styleStack.hasProperty(KoXmlNS::fo, "padding")) {
            padding.left = KoUnit::parseValue(styleStack.property(KoXmlNS::fo, "padding"));
            padding.top = padding.left;
            padding.right = padding.left;
            padding.bottom = padding.left;
            debugChartOdf<<"load padding"<<padding.left;
        }
        if (styleStack.hasProperty(KoXmlNS::fo, "padding-left")) {
            padding.left = KoUnit::parseValue(styleStack.property(KoXmlNS::fo, "padding-left"));
            debugChartOdf<<"load padding-left"<<padding.left;
        }
        if (styleStack.hasProperty(KoXmlNS::fo, "padding-top")) {
            padding.top = KoUnit::parseValue(styleStack.property(KoXmlNS::fo, "padding-top"));
            debugChartOdf<<"load padding-top"<<padding.top;
        }
        if (styleStack.hasProperty(KoXmlNS::fo, "padding-right")) {
            padding.right = KoUnit::parseValue(styleStack.property(KoXmlNS::fo, "padding-right"));
            debugChartOdf<<"load padding-right"<<padding.right;
        }
        if (styleStack.hasProperty(KoXmlNS::fo, "padding-bottom")) {
            padding.bottom = KoUnit::parseValue(styleStack.property(KoXmlNS::fo, "padding-bottom"));
            debugChartOdf<<"load padding-bottom"<<padding.bottom;
        }
        layout()->setPadding(padding);
    }
    // Also load the size here as it, if specified here, overwrites the frame's size,
    // See ODF specs for chart:chart element for more details.
    loadOdfAttributes(chartElement, context,
                      OdfAdditionalAttributes | OdfMandatories | OdfCommonChildElements | OdfStyle | OdfSize);

#ifndef NWORKAROUND_ODF_BUGS
    if (!background()) {
        const QColor color = KoOdfWorkaround::fixMissingFillColor(chartElement, context);
        if (color.isValid()) // invalid color means do not set KoColorBackground but be transparent instead
            setBackground(QSharedPointer<KoColorBackground>(new KoColorBackground(color)));
    }
#endif

    // Check if we're loading an embedded document
    if (!chartElement.hasAttributeNS(KoXmlNS::chart, "class")) {
        debugChart << "Error: Embedded document has no chart:class attribute.";
        return false;
    }

    Q_ASSERT(d->plotArea);


    // 1. Load the chart type.
    // NOTE: Chart type and -subtype is a bit tricky as stock charts and bubble charts
    //       needs special treatment.
    //       So we do not call the ChartShape::setChart... methods here in odf code,
    //       but the plot area methods directly.
    const QString chartClass = chartElement.attributeNS(KoXmlNS::chart, "class", QString());
    KoChart::ChartType chartType = KoChart::BarChartType;
    // Find out what charttype the chart class corresponds to.
    bool  knownType = false;
    for (int type = 0; type < (int)LastChartType; ++type) {
        if (chartClass == ODF_CHARTTYPES[(ChartType)type]) {
            chartType = (ChartType)type;
            // Set the dimensionality of the data points, we can not call
            // setChartType here as bubble charts requires that the datasets already exist
            proxyModel()->setDataDimensions(numDimensions(chartType));
            knownType = true;
            debugChartOdf <<"found chart of type" << chartClass<<chartType;
            break;
        }
    }

    // If we can't find out what charttype it is, we might as well end here.
    if (!knownType) {
        // FIXME: Find out what the equivalent of
        //        KoDocument::setErrorMessage() is for KoShape.
        //setErrorMessage(i18n("Unknown chart type %1" ,chartClass));
        warnChartOdf<<"Unknown chart type:"<<chartClass;
        return false;
    }

    // 2. Load the data
//     int dimensions = numDimensions(chartType);
//     debugChart << "DIMENSIONS" << dimensions;
//     d->proxyModel->setDataDimensions(dimensions);
//     debugChart << d->proxyModel->dataSets().count();
    KoXmlElement  dataElem = KoXml::namedItemNS(chartElement, KoXmlNS::table, "table");
    if (!dataElem.isNull()) {
        if (!loadOdfData(dataElem, context))
            return false;
    }

    // 3. Load the plot area (this is where the meat is!).
    KoXmlElement  plotareaElem = KoXml::namedItemNS(chartElement, KoXmlNS::chart, "plot-area");

    if (!plotareaElem.isNull()) {
        d->plotArea->setChartType(chartType);
        d->plotArea->setChartSubType(chartSubType());
        if (!d->plotArea->loadOdf(plotareaElem, context)) {
            return false;
        }
//         d->plotArea->setChartType(chartType);
//         d->plotArea->setChartSubType(chartSubType());
    }

    // 4. Load the title.
    KoXmlElement titleElem = KoXml::namedItemNS(chartElement,
                                                 KoXmlNS::chart, "title");
    d->setChildVisible(d->title, !titleElem.isNull());
    if (!titleElem.isNull()) {
        if (!OdfHelper::loadOdfTitle(d->title, titleElem, context))
            return false;
    }

    // 5. Load the subtitle.
    KoXmlElement subTitleElem = KoXml::namedItemNS(chartElement, KoXmlNS::chart, "subtitle");
    d->setChildVisible(d->subTitle, !subTitleElem.isNull());
    if (!subTitleElem.isNull()) {
        if (!OdfHelper::loadOdfTitle(d->subTitle, subTitleElem, context))
            return false;
    }

    // 6. Load the footer.
    KoXmlElement footerElem = KoXml::namedItemNS(chartElement, KoXmlNS::chart, "footer");
    d->setChildVisible(d->footer, !footerElem.isNull());
    if (!footerElem.isNull()) {
        if (!OdfHelper::loadOdfTitle(d->footer, footerElem, context))
            return false;
    }

    // 7. Load the legend.
    KoXmlElement legendElem = KoXml::namedItemNS(chartElement, KoXmlNS::chart, "legend");
    d->setChildVisible(d->legend, !legendElem.isNull());
    if (!legendElem.isNull()) {
        if (!d->legend->loadOdf(legendElem, context))
            return false;
    }

    // 8. Sets the chart type
    // since chart type in plot area is already set before axes were loaded, we need to do axes here
    for (Axis *a : d->plotArea->axes()) {
        a->plotAreaChartTypeChanged(chartType);
    }
    debugChartOdf<<"loaded:"<<this->chartType()<<chartSubType();

    updateAll();
    requestRepaint();

    return true;
}

bool ChartShape::loadOdfData(const KoXmlElement &tableElement,
                             KoShapeLoadingContext &context)
{
    // There is no table element to load
    if (tableElement.isNull() || !tableElement.isElement())
        return true;

    // An internal model might have been set before in ChartShapeFactory.
    if (d->internalModel) {
        Table *oldInternalTable = d->tableSource.get(d->internalModel);
        Q_ASSERT(oldInternalTable);
        d->tableSource.remove(oldInternalTable->name());
    }

    // FIXME: Make model->loadOdf() return a bool, and use it here.
    // Create a table with data from document, add it as table source
    // and reset the proxy only with data from this new table.
    ChartTableModel *internalModel = new ChartTableModel;
    internalModel->loadOdf(tableElement, context);

    QString tableName = tableElement.attributeNS(KoXmlNS::table, "name");
    debugChartOdf<<"Loaded table:"<<tableName;
    d->tableSource.add(tableName, internalModel);
    // TODO: d->tableSource.setAvoidNameClash(tableName)
    setInternalModel(internalModel);

    return true;
}

void ChartShape::saveOdf(KoShapeSavingContext & context) const
{
    Q_ASSERT(d->plotArea);

    KoXmlWriter&  bodyWriter = context.xmlWriter();

    // Check if we're saving to a chart document. If not, embed a
    // chart document.  ChartShape::saveOdf() will then be called
    // again later, when the current document saves the embedded
    // documents.
    //
    // FIXME: The check isEmpty() fixes a crash that happened when a
    //        chart shape was saved from Words.  There are two
    //        problems with this fix:
    //        1. Checking the tag hierarchy is hardly the right way to do this
    //        2. The position doesn't seem to be saved yet.
    //
    //        Also, I have to check with the other apps, e.g. Calligra Sheets,
    //        if it works there too.
    //
    QList<const char*>  tagHierarchy = bodyWriter.tagHierarchy();
    if (tagHierarchy.isEmpty()
        || QString(tagHierarchy.last()) != "office:chart")
    {
        bodyWriter.startElement("draw:frame");
        // See also loadOdf() in loadOdfAttributes.
        saveOdfAttributes(context, OdfAllAttributes);

        bodyWriter.startElement("draw:object");
        context.embeddedSaver().embedDocument(bodyWriter, d->document);
        bodyWriter.endElement(); // draw:object

        bodyWriter.endElement(); // draw:frame
        return;
    }

    bodyWriter.startElement("chart:chart");

    saveOdfAttributes(context, OdfSize);

    context.setStyleFamily("ch");
    KoGenStyle style(KoGenStyle::ChartAutoStyle, "chart");
    KoInsets padding = layout()->padding();
    style.addPropertyPt("fo:padding-left", padding.left, KoGenStyle::GraphicType);
    style.addPropertyPt("fo:padding-top", padding.top, KoGenStyle::GraphicType);
    style.addPropertyPt("fo:padding-right", padding.right, KoGenStyle::GraphicType);
    style.addPropertyPt("fo:padding-bottom", padding.bottom, KoGenStyle::GraphicType);
    debugChartOdf<<"save padding:"<<padding;
    bodyWriter.addAttribute("chart:style-name", saveStyle(style, context));

    // 1. Write the chart type.
    bodyWriter.addAttribute("chart:class", ODF_CHARTTYPES[d->plotArea->chartType() ]);

    // 2. Write the title.
    OdfHelper::saveOdfTitle(d->title, bodyWriter, "chart:title", context);

    // 3. Write the subtitle.
    OdfHelper::saveOdfTitle(d->subTitle, bodyWriter, "chart:subtitle", context);

    // 4. Write the footer.
    OdfHelper::saveOdfTitle(d->footer, bodyWriter, "chart:footer", context);

    // 5. Write the legend.
    if (d->legend->isVisible())
        d->legend->saveOdf(context);

    // 6. Write the plot area (this is where the real action is!).
    d->plotArea->saveOdf(context);

    // 7. Save the data
    saveOdfData(bodyWriter, context.mainStyles());

    bodyWriter.endElement(); // chart:chart
}

static void saveOdfDataRow(KoXmlWriter &bodyWriter, QAbstractItemModel *table, int row)
{
    bodyWriter.startElement("table:table-row");
    const int cols = table->columnCount();
    for (int col = 0; col < cols; ++col) {
        //QVariant value(internalModel.cellVal(row, col));
        QModelIndex  index = table->index(row, col);
        QVariant     value = table->data(index);

        bool ok;
        double val = value.toDouble(&ok);
        if (ok) {
            value = val;
        }

        QString  valType;
        QString  valStr;

        switch (value.type()) {
        case QVariant::Invalid:
            break;
        case QVariant::String:
            valType = "string";
            valStr  = value.toString();
            break;
        case QVariant::Double:
            valType = "float";
            valStr  = QString::number(value.toDouble(), 'g', DBL_DIG);
            break;
        case QVariant::DateTime:

            valType = "date";
            valStr  = ""; /* like in saveXML, but why? */
            break;
        default:
            debugChart <<"ERROR: cell" << row <<"," << col
                          << " has unknown type." << endl;
        }

        // Add the value type and the string to the XML tree.
        bodyWriter.startElement("table:table-cell");
        if (!valType.isEmpty()) {
            bodyWriter.addAttribute("office:value-type", valType);
            if (value.type() == QVariant::Double)
                bodyWriter.addAttribute("office:value", valStr);

            bodyWriter.startElement("text:p");
            bodyWriter.addTextNode(valStr);
            bodyWriter.endElement(); // text:p
        }

        bodyWriter.endElement(); // table:table-cell
    }

    bodyWriter.endElement(); // table:table-row
}

void ChartShape::saveOdfData(KoXmlWriter &bodyWriter, KoGenStyles &mainStyles) const
{
    Q_UNUSED(mainStyles);

    // FIXME: Move this method to a sane place
    ChartTableModel *internalModel = d->internalModel;
    Table *internalTable = d->tableSource.get(internalModel);
    Q_ASSERT(internalTable);

    // Only save the data if we actually have some.
    if (!internalModel)
        return;

    const int rows = internalModel->rowCount();
    const int cols = internalModel->columnCount();

    bodyWriter.startElement("table:table");
    bodyWriter.addAttribute("table:name", internalTable->name());

    // Exactly one header column, always.
    bodyWriter.startElement("table:table-header-columns");
    bodyWriter.startElement("table:table-column");
    bodyWriter.endElement(); // table:table-column
    bodyWriter.endElement(); // table:table-header-columns

    // Then "cols" columns
    bodyWriter.startElement("table:table-columns");
    bodyWriter.startElement("table:table-column");
    bodyWriter.addAttribute("table:number-columns-repeated", cols);
    bodyWriter.endElement(); // table:table-column
    bodyWriter.endElement(); // table:table-columns

    int row = 0;

    bodyWriter.startElement("table:table-header-rows");
    if (rows > 0)
        saveOdfDataRow(bodyWriter, internalModel, row++);
    bodyWriter.endElement(); // table:table-header-rows

    // Here start the actual data rows.
    bodyWriter.startElement("table:table-rows");
    //QStringList::const_iterator rowLabelIt = m_rowLabels.begin();
    for (; row < rows ; ++row)
        saveOdfDataRow(bodyWriter, internalModel, row);

    bodyWriter.endElement(); // table:table-rows
    bodyWriter.endElement(); // table:table
}

void ChartShape::updateAll()
{
    d->legend->update();
    d->plotArea->plotAreaUpdate();
    relayout();
    update();
}

void ChartShape::update() const
{
    KoShape::update();
    layout()->scheduleRelayout();

    emit updateConfigWidget();
}

void ChartShape::relayout() const
{
    Q_ASSERT(d->plotArea);
    d->plotArea->relayout();
    KoShape::update();
}

void ChartShape::requestRepaint() const
{
    Q_ASSERT(d->plotArea);
    d->plotArea->requestRepaint();
}

KoDocumentResourceManager *ChartShape::resourceManager() const
{
    return d->resourceManager;
}

void ChartShape::setEnableUserInteraction(bool enable)
{
    ENABLE_USER_INTERACTION = enable;
}

void ChartShape::shapeChanged(ChangeType type, KoShape *shape)
{
    Q_UNUSED(shape)
    layout()->containerChanged(this, type);
}

ChartDocument *ChartShape::document() const
{
    return d->document;
}

} // Namespace KoChart
