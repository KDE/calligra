/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2007 Johannes Simon <johannes.simon@gmail.com>
   SPDX-FileCopyrightText: 2010 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "Legend.h"

// Qt
#include <QBrush>
#include <QColor>
#include <QFont>
#include <QImage>
#include <QPainterPath>
#include <QPen>
#include <QSizeF>
#include <QString>

// Calligra
#include <KoColorBackground.h>
#include <KoGenStyles.h>
#include <KoOdfLoadingContext.h>
#include <KoShapeLoadingContext.h>
#include <KoShapeSavingContext.h>
#include <KoShapeStroke.h>
#include <KoStyleStack.h>
#include <KoUnit.h>
#include <KoXmlNS.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>

// KChart
#include "KChartConvertions.h"
#include "kchart_version.h"
#include <KChartAbstractDiagram>
#include <KChartBackgroundAttributes>
#include <KChartBarDiagram>
#include <KChartChart>
#include <KChartFrameAttributes>
#include <KChartLegend>

// KoChart
#include "ChartLayout.h"
#include "OdfHelper.h"
#include "OdfLoadingHelper.h"
#include "PlotArea.h"
#include "ScreenConversions.h"

using namespace KoChart;

class Legend::Private
{
public:
    Private();
    ~Private();

    ChartShape *shape;

    // Properties of the Legend
    QString title;
    LegendExpansion expansion;
    Position position;
    QFont font;
    QFont titleFont;
    QColor fontColor;
    Qt::Alignment alignment;
    KoShapeStroke *lineBorder;

    // The connection to KChart
    KChart::Legend *kdLegend;

    QImage image;

    mutable bool pixmapRepaintRequested;
    QSizeF lastSize;
    QPointF lastZoomLevel;
};

Legend::Private::Private()
{
    lineBorder = new KoShapeStroke(0.5, Qt::black);
    expansion = HighLegendExpansion;
    alignment = Qt::AlignCenter;
    pixmapRepaintRequested = true;
    position = EndPosition;
}

Legend::Private::~Private()
{
    delete lineBorder;
}

Legend::Legend(ChartShape *parent)
    : QObject(parent)
    , d(new Private())
{
    Q_ASSERT(parent);

    setShapeId("ChartShapeLegend");

    d->shape = parent;

    d->kdLegend = new KChart::Legend();
    d->kdLegend->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // we use the shape to display frame and background
    KChart::FrameAttributes frameAttr = d->kdLegend->frameAttributes();
    frameAttr.setVisible(false);
    d->kdLegend->setFrameAttributes(frameAttr);

    setTitleFontSize(10);
    setTitle(QString());
    setFontSize(8);

    update();

    parent->addShape(this);

    setAllowedInteraction(KoShape::ResizeAllowed, false);
    setAllowedInteraction(KoShape::RotationAllowed, false);

    connect(d->kdLegend, &KChart::Legend::propertiesChanged, this, &Legend::slotKdLegendChanged);
    connect(parent, &ChartShape::chartTypeChanged, this, &Legend::slotChartTypeChanged);
}

Legend::~Legend()
{
    delete d->kdLegend;
    delete d;
}

QString Legend::title() const
{
    return d->title;
}

void Legend::setTitle(const QString &title)
{
    d->title = title;
    d->kdLegend->setTitleText(title);
    d->pixmapRepaintRequested = true;

    Q_EMIT updateConfigWidget();
}

QFont Legend::font() const
{
    return d->font;
}

void Legend::setFont(const QFont &font)
{
    d->font = font;

    // KChart
    KChart::TextAttributes attributes = d->kdLegend->textAttributes();
    attributes.setFont(font);
    d->kdLegend->setTextAttributes(attributes);

    d->pixmapRepaintRequested = true;
    Q_EMIT updateConfigWidget();
}

qreal Legend::fontSize() const
{
    return d->font.pointSizeF();
}

void Legend::setFontSize(qreal size)
{
    d->font.setPointSizeF(size);

    // KChart
    KChart::TextAttributes attributes = d->kdLegend->textAttributes();
    KChart::Measure m = attributes.fontSize();
    m.setValue(size);
    attributes.setFontSize(m);
    d->kdLegend->setTextAttributes(attributes);

    d->pixmapRepaintRequested = true;
    Q_EMIT updateConfigWidget();
}

void Legend::setFontColor(const QColor &color)
{
    KChart::TextAttributes attributes = d->kdLegend->textAttributes();
    QPen pen = attributes.pen();
    pen.setColor(color);
    attributes.setPen(pen);
    d->kdLegend->setTextAttributes(attributes);

    d->pixmapRepaintRequested = true;
}

QColor Legend::fontColor() const
{
    KChart::TextAttributes attributes = d->kdLegend->textAttributes();
    QPen pen = attributes.pen();
    return pen.color();
}

QFont Legend::titleFont() const
{
    return d->titleFont;
}

void Legend::setTitleFont(const QFont &font)
{
    d->titleFont = font;

    // KChart
    KChart::TextAttributes attributes = d->kdLegend->titleTextAttributes();
    attributes.setFont(font);
    d->kdLegend->setTitleTextAttributes(attributes);

    d->pixmapRepaintRequested = true;
}

qreal Legend::titleFontSize() const
{
    return d->titleFont.pointSizeF();
}

void Legend::setTitleFontSize(qreal size)
{
    d->titleFont.setPointSizeF(size);

    // KChart
    KChart::TextAttributes attributes = d->kdLegend->titleTextAttributes();
    attributes.setFontSize(KChart::Measure(size, KChartEnums::MeasureCalculationModeAbsolute));
    d->kdLegend->setTitleTextAttributes(attributes);

    d->pixmapRepaintRequested = true;
}

LegendExpansion Legend::expansion() const
{
    return d->expansion;
}

void Legend::setExpansion(LegendExpansion expansion)
{
    d->expansion = expansion;
    d->kdLegend->setOrientation(LegendExpansionToQtOrientation(expansion));
    d->pixmapRepaintRequested = true;

    Q_EMIT updateConfigWidget();
}

Qt::Alignment Legend::alignment() const
{
    return d->alignment;
}

void Legend::setAlignment(Qt::Alignment alignment)
{
    d->alignment = alignment;
}

Position Legend::legendPosition() const
{
    return d->position;
}

void Legend::setLegendPosition(Position position)
{
    d->position = position;
    d->pixmapRepaintRequested = true;
}
// Note that size is controlled by the KChart::Legend
// via the propertyChanged() signal
// so size will change dependent on amount of data
void Legend::setSize(const QSizeF &newSize)
{
    KoShape::setSize(newSize);
}

void Legend::paintPixmap(QPainter &painter, const KoViewConverter &converter)
{
    // Adjust the size of the painting area to the current zoom level
    const QSize paintRectSize = converter.documentToView(d->lastSize).toSize();
    d->image = QImage(paintRectSize, QImage::Format_ARGB32);

    QPainter pixmapPainter(&d->image);
    pixmapPainter.setRenderHints(painter.renderHints());
    pixmapPainter.setRenderHint(QPainter::Antialiasing, false);

    // Scale the painter's coordinate system to fit the current zoom level.
    applyConversion(pixmapPainter, converter);
    d->kdLegend->paint(&pixmapPainter);
}

void Legend::paint(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &paintContext)
{
    // painter.save();

    // First of all, scale the painter's coordinate system to fit the current zoom level
    applyConversion(painter, converter);

    // Calculate the clipping rect
    QRectF paintRect = QRectF(QPointF(0, 0), size());
    // clipRect.intersected(paintRect);
    painter.setClipRect(paintRect, Qt::IntersectClip);

    // Get the current zoom level
    QPointF zoomLevel;
    converter.zoom(&zoomLevel.rx(), &zoomLevel.ry());

    // Only repaint the pixmap if it is scheduled, the zoom level changed or the shape was resized
    /*if (   d->pixmapRepaintRequested
         || d->lastZoomLevel != zoomLevel
         || d->lastSize      != size()) {
        // TODO: What if two zoom levels are constantly being requested?
        // At the moment, this *is* the case, due to the fact
        // that the shape is also rendered in the page overview
        // in Stage
        // Every time the window is hidden and shown again, a repaint is
        // requested --> laggy performance, especially when quickly
        // switching through windows
        d->pixmapRepaintRequested = false;
        d->lastZoomLevel = zoomLevel;
        d->lastSize      = size();

        paintPixmap(painter, converter);
    }*/

    // Paint the background
    if (background()) {
        QPainterPath p;
        p.addRect(paintRect);
        background()->paint(painter, converter, paintContext, p);
    }

    disconnect(d->kdLegend, &KChart::Legend::propertiesChanged, this, &Legend::slotKdLegendChanged);

    // KChart thinks in pixels, Calligra in pt
    ScreenConversions::scaleFromPtToPx(painter);
    const QRect rect = ScreenConversions::scaleFromPtToPx(paintRect, painter);
    // KChart works with its logicalDpi which may differ from ours if set with --dpi <x,y>
    ScreenConversions::scaleToWidgetDpi(d->kdLegend, painter);
    d->kdLegend->paint(&painter, rect);

    connect(d->kdLegend, &KChart::Legend::propertiesChanged, this, &Legend::slotKdLegendChanged);
}

// ----------------------------------------------------------------
//                     loading and saving

bool Legend::loadOdf(const KoXmlElement &legendElement, KoShapeLoadingContext &context)
{
    KoStyleStack &styleStack = context.odfLoadingContext().styleStack();
    styleStack.clear();

    // FIXME: If the style isn't present we shouldn't care about it at all
    // and move everything related to the legend style in this if clause
    if (legendElement.hasAttributeNS(KoXmlNS::chart, "style-name")) {
        context.odfLoadingContext().fillStyleStack(legendElement, KoXmlNS::chart, "style-name", "chart");
        styleStack.setTypeProperties("graphic");
    }

    if (!legendElement.isNull()) {
        int attributesToLoad = OdfAllAttributes;
        QString lp = legendElement.attributeNS(KoXmlNS::chart, "legend-position", QString());

        // Note: load position even if it might not be used
        loadOdfAttributes(legendElement, context, attributesToLoad);

        QString lalign = legendElement.attributeNS(KoXmlNS::chart, "legend-align", QString());

        if (legendElement.hasAttributeNS(KoXmlNS::style, "legend-expansion")) {
            QString lexpansion = legendElement.attributeNS(KoXmlNS::style, "legend-expansion", QString());
            if (lexpansion == "wide")
                setExpansion(WideLegendExpansion);
            else if (lexpansion == "high")
                setExpansion(HighLegendExpansion);
            else
                setExpansion(BalancedLegendExpansion);
        }

        if (lalign == "start") {
            setAlignment(Qt::AlignLeft);
        } else if (lalign == "end") {
            setAlignment(Qt::AlignRight);
        } else {
            setAlignment(Qt::AlignCenter); // default
        }

        if (lp == "start") {
            setLegendPosition(StartPosition);
        } else if (lp == "top") {
            setLegendPosition(TopPosition);
        } else if (lp == "bottom") {
            setLegendPosition(BottomPosition);
        } else if (lp == "end") {
            setLegendPosition(EndPosition);
        } else if (lp == "top-start") {
            setLegendPosition(TopStartPosition);
        } else if (lp == "bottom-start") {
            setLegendPosition(BottomStartPosition);
        } else if (lp == "top-end") {
            setLegendPosition(TopEndPosition);
        } else if (lp == "bottom-end") {
            setLegendPosition(BottomEndPosition);
        } else {
            setLegendPosition(FloatingPosition);
        }

        if (legendElement.hasAttributeNS(KoXmlNS::office, "title")) {
            setTitle(legendElement.attributeNS(KoXmlNS::office, "title", QString()));
        }

        styleStack.setTypeProperties("text");

        if (styleStack.hasProperty(KoXmlNS::fo, "font-family")) {
            QString fontFamily = styleStack.property(KoXmlNS::fo, "font-family");
            QFont font = d->font;
            font.setFamily(fontFamily);
            setFont(font);
        }
        if (styleStack.hasProperty(KoXmlNS::fo, "font-size")) {
            qreal fontSize = KoUnit::parseValue(styleStack.property(KoXmlNS::fo, "font-size"));
            setFontSize(fontSize);
        }
        if (styleStack.hasProperty(KoXmlNS::fo, "font-color")) {
            QColor color = styleStack.property(KoXmlNS::fo, "font-color");
            if (color.isValid()) {
                setFontColor(color);
            }
        }
    } else {
        // No legend element, use default legend.
        setLegendPosition(EndPosition);
        setAlignment(Qt::AlignCenter);
    }

    d->pixmapRepaintRequested = true;

    return true;
}

void Legend::saveOdf(KoShapeSavingContext &context) const
{
    KoXmlWriter &bodyWriter = context.xmlWriter();

    bodyWriter.startElement("chart:legend");
    saveOdfAttributes(context, OdfPosition);

    // Legend specific attributes
    QString lp = PositionToString(d->position);
    if (!lp.isEmpty()) {
        bodyWriter.addAttribute("chart:legend-position", lp);
    }
    QString lalign;
    switch (d->alignment) {
    case Qt::AlignLeft:
        lalign = "start";
        break;
    case Qt::AlignRight:
        lalign = "end";
        break;
    case Qt::AlignCenter:
        lalign = "center";
        break;
    default:
        break;
    }
    if (!lalign.isEmpty()) {
        bodyWriter.addAttribute("chart:legend-align", lalign);
    }

    // Legend style FIXME: Check if more styling then just the font goes here.
    KoGenStyle style(KoGenStyle::ChartAutoStyle, "chart", nullptr);
    OdfHelper::saveOdfFont(style, d->font, d->fontColor);
    bodyWriter.addAttribute("chart:style-name", saveStyle(style, context));

    QString lexpansion;
    switch (expansion()) {
    case WideLegendExpansion:
        lexpansion = "wide";
        break;
    case HighLegendExpansion:
        lexpansion = "high";
        break;
    case BalancedLegendExpansion:
        lexpansion = "balanced";
        break;
    };
    bodyWriter.addAttribute("style:legend-expansion", lexpansion);

    if (!title().isEmpty())
        bodyWriter.addAttribute("office:title", title());

    bodyWriter.endElement(); // chart:legend
}

KChart::Legend *Legend::kdLegend() const
{
    // There has to be a valid KChart instance of this legend
    Q_ASSERT(d->kdLegend);
    return d->kdLegend;
}

void Legend::rebuild()
{
    d->kdLegend->forceRebuild();
    update();
}

void Legend::update() const
{
    d->pixmapRepaintRequested = true;
    KoShape::update();
}

void Legend::slotKdLegendChanged()
{
    // FIXME: Update legend properly by implementing all *DataChanged() slots
    // in KChartModel. Right now, only yDataChanged() is implemented.
    // d->kdLegend->forceRebuild();
    QSizeF size = ScreenConversions::scaleFromPxToPt(d->kdLegend->sizeHint());
    setSize(ScreenConversions::fromWidgetDpi(d->kdLegend, size));
    update();
}

void Legend::slotChartTypeChanged(ChartType chartType)
{
    // TODO: Once we support markers, this switch will have to be
    // more clever.
    switch (chartType) {
    case LineChartType:
    case ScatterChartType:
        d->kdLegend->setLegendStyle(KChart::Legend::MarkersAndLines);
        break;
    default:
        d->kdLegend->setLegendStyle(KChart::Legend::MarkersOnly);
        break;
    }
}
