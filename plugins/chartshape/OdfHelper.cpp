/* This file is part of the KDE project

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


#include "OdfHelper.h"

#include "ChartDebug.h"

// Posix
#include <float.h> // For basic data types characteristics.

// Qt
#include <QPointF>
#include <QPainter>
#include <QSizeF>
#include <QTextDocument>
#include <QStandardItemModel>
#include <QUrl>

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
#include <KoShapeShadow.h>
#include <KoBorder.h>
#include <KoHatchBackground.h>
#include <KoOdfGradientBackground.h>
#include <KoPatternBackground.h>
#include <KoGradientBackground.h>
#include <KoOdfGraphicStyles.h>
#include "kochart_global.h"

namespace KoChart {
namespace OdfHelper {


// HACK: To get correct position also for rotated titles
QPointF itemPosition(const KoShape *shape)
{
    return QPointF(shape->transformation().dx(), shape->transformation().dy());
}

//fo:font-weight attribute are normal, bold, 100, 200, 300, 400, 500, 600, 700, 800 or 900.
int fromOdfFontWeight(const QString &odfweight) {
    if (odfweight.isEmpty() || odfweight == "normal") {
        return QFont::Normal;
    }
    if (odfweight == "bold") {
        return QFont::Bold;
    }
    bool ok;
    int weight = odfweight.toInt(&ok);
    if (!ok) {
        return 50;
    }
    switch (weight) {
        case 100: weight = 1; break;
        case 200: weight = 17; break;
        case 300: weight = 33; break;
        case 400: weight = 50; break;
        case 500: weight = 58; break;
        case 600: weight = 66; break;
        case 700: weight = 75; break;
        case 800: weight = 87; break;
        case 900: weight = 99; break;
        default: weight = 50; break;
    }
    return weight;
}

QString toOdfFontWeight(int weight) {
    QString w;
    if (weight < 8) {
        w = "100";
    } else if (weight < 25) {
        w = "200";
    } else if (weight < 41) {
        w = "300";
    } else if (weight < 54) {
        w = "normal";
    } else if (weight < 62) {
        w = "500";
    } else if (weight < 70) {
        w = "600";
    } else if (weight < 81) {
        w = "bold";
    } else if (weight < 92) {
        w = "800";
    } else {
        w = "900";
    }
    return w;
}

void saveOdfFont(KoGenStyle &style, const QFont& font, const QColor& color)
{
    style.addProperty("fo:font-family", font.family(), KoGenStyle::TextType);
    style.addPropertyPt("fo:font-size", font.pointSize(), KoGenStyle::TextType);
    style.addProperty("fo:color", color.isValid() ? color.name() : "#000000", KoGenStyle::TextType);
    style.addProperty("fo:font-weight", toOdfFontWeight(font.weight()), KoGenStyle::TextType);
    style.addProperty("fo:font-style", font.italic() ? "italic" : "normal", KoGenStyle::TextType);
}

QString saveOdfFont(KoGenStyles& mainStyles,
                    const QFont& font,
                    const QColor& color)
{
    KoGenStyle autoStyle(KoGenStyle::ParagraphAutoStyle, "chart", 0);
    saveOdfFont(autoStyle, font, color);
    return mainStyles.insert(autoStyle, "ch");
}

void saveOdfTitleStyle(KoShape *title, KoGenStyle &style, KoShapeSavingContext &context)
{
    TextLabelData *titleData = qobject_cast<TextLabelData*>(title->userData());
    Q_ASSERT(titleData);
    QTextCursor cursor(titleData->document());
    QFont titleFont = cursor.charFormat().font();
    QColor color = cursor.charFormat().foreground().color();

    saveOdfFont(style, titleFont, color);

    // title->saveStyle(style, context) is protected,
    // so we duplicate some code:
    KoShapeStrokeModel *sm = title->stroke();
    if (sm) {
        sm->fillStyle(style, context);
    } else {
        style.addProperty("draw:stroke", "none", KoGenStyle::GraphicType);
    }
    KoShapeShadow *s = title->shadow();
    if (s) {
        s->fillStyle(style, context);
    }
    QSharedPointer<KoShapeBackground> bg = title->background();
    if (bg) {
        bg->fillStyle(style, context);
    } else {
        style.addProperty("draw:fill", "none", KoGenStyle::GraphicType);
    }

    KoBorder *b = title->border();
    if (b) {
        b->saveOdf(style);
    }

    QMap<QByteArray, QString>::const_iterator it(title->additionalStyleAttributes().constBegin());
    for (; it != title->additionalStyleAttributes().constEnd(); ++it) {
        style.addProperty(it.key(), it.value(), KoGenStyle::ChartType);
    }
    style.addProperty("chart:auto-size", (titleData->resizeMethod() == KoTextShapeDataBase::AutoResize), KoGenStyle::ChartType);
}

void saveOdfTitle(KoShape *title, KoXmlWriter &bodyWriter, const char *titleType, KoShapeSavingContext &context)
{
    // Don't save hidden titles, as that's the way of removing them
    // from a chart.
    if (!title->isVisible())
        return;

    TextLabelData *titleData = qobject_cast<TextLabelData*>(title->userData());
    if (!titleData)
        return;

    bodyWriter.startElement(titleType);

    KoGenStyle autoStyle(KoGenStyle::ChartAutoStyle, "chart", 0);
    autoStyle.addPropertyPt("style:rotation-angle", 360 - title->rotation());
    saveOdfTitleStyle(title, autoStyle, context);

    // always save position and size and let consumer decide if they are used
    QPointF position = itemPosition(title);
    bodyWriter.addAttributePt("svg:x", position.x());
    bodyWriter.addAttributePt("svg:y", position.y());

    const QSizeF size = title->size();
    bodyWriter.addAttributePt("svg:width", size.width());
    bodyWriter.addAttributePt("svg:height", size.height());

    bodyWriter.addAttribute("chart:style-name", context.mainStyles().insert(autoStyle, "ch"));

    // lo (and odf?) does not support formatted text :(
    bodyWriter.startElement("text:p");
    bodyWriter.addTextNode(titleData->document()->toPlainText());
    bodyWriter.endElement(); // text:p

    // save calligra specific formatted text
    bodyWriter.startElement("calligra:text");
    titleData->saveOdf(context);
    bodyWriter.endElement(); // calligra:text

    bodyWriter.endElement(); // chart:title/subtitle/footer
}

QString getStyleProperty(const char *property, KoShapeLoadingContext &context)
{
    KoStyleStack &styleStack = context.odfLoadingContext().styleStack();
    QString value;

    if (styleStack.hasProperty(KoXmlNS::draw, property)) {
        value = styleStack.property(KoXmlNS::draw, property);
    }

    return value;
}

QSharedPointer<KoShapeBackground> loadOdfFill(KoShape *title, KoShapeLoadingContext &context)
{
    QString fill = getStyleProperty("fill", context);
    QSharedPointer<KoShapeBackground> bg;
    if (fill == "solid") {
        bg = QSharedPointer<KoShapeBackground>(new KoColorBackground());
    }
    else if (fill == "hatch") {
        bg = QSharedPointer<KoShapeBackground>(new KoHatchBackground());
    } else if (fill == "gradient") {
        QString styleName = getStyleProperty("fill-gradient-name", context);
        KoXmlElement *e = context.odfLoadingContext().stylesReader().drawStyles("gradient").value(styleName);
        QString style;
        if (e) {
            style = e->attributeNS(KoXmlNS::draw, "style", QString());
        }
        if ((style == "rectangular") || (style == "square")) {
            bg = QSharedPointer<KoShapeBackground>(new KoOdfGradientBackground());
        } else {
            QGradient *gradient = new QLinearGradient();
            gradient->setCoordinateMode(QGradient::ObjectBoundingMode);
            bg = QSharedPointer<KoShapeBackground>(new KoGradientBackground(gradient));
        }
    } else if (fill == "bitmap") {
        bg = QSharedPointer<KoShapeBackground>(new KoPatternBackground(context.imageCollection()));
#ifndef NWORKAROUND_ODF_BUGS
    } else if (fill.isEmpty()) {
        bg = QSharedPointer<KoShapeBackground>(KoOdfWorkaround::fixBackgroundColor(title, context));
        return bg;
#endif
    } else {
        return QSharedPointer<KoShapeBackground>(0);
    }

    if (!bg->loadStyle(context.odfLoadingContext(), title->size())) {
        return QSharedPointer<KoShapeBackground>(0);
    }

    return bg;
}

KoShapeStrokeModel *loadOdfStroke(KoShape *title, const KoXmlElement &element, KoShapeLoadingContext &context)
{
    KoStyleStack &styleStack = context.odfLoadingContext().styleStack();
    KoOdfStylesReader &stylesReader = context.odfLoadingContext().stylesReader();

    QString stroke = getStyleProperty("stroke", context);
    if (stroke == "solid" || stroke == "dash") {
        QPen pen = KoOdfGraphicStyles::loadOdfStrokeStyle(styleStack, stroke, stylesReader);

        KoShapeStroke *stroke = new KoShapeStroke();

        if (styleStack.hasProperty(KoXmlNS::calligra, "stroke-gradient")) {
            QString gradientName = styleStack.property(KoXmlNS::calligra, "stroke-gradient");
            QBrush brush = KoOdfGraphicStyles::loadOdfGradientStyleByName(stylesReader, gradientName, title->size());
            stroke->setLineBrush(brush);
        } else {
            stroke->setColor(pen.color());
        }

#ifndef NWORKAROUND_ODF_BUGS
        KoOdfWorkaround::fixPenWidth(pen, context);
#endif
        stroke->setLineWidth(pen.widthF());
        stroke->setJoinStyle(pen.joinStyle());
        stroke->setLineStyle(pen.style(), pen.dashPattern());
        stroke->setCapStyle(pen.capStyle());

        return stroke;
#ifndef NWORKAROUND_ODF_BUGS
    } else if (stroke.isEmpty()) {
        QPen pen = KoOdfGraphicStyles::loadOdfStrokeStyle(styleStack, "solid", stylesReader);
        if (KoOdfWorkaround::fixMissingStroke(pen, element, context, title)) {
            KoShapeStroke *stroke = new KoShapeStroke();

#ifndef NWORKAROUND_ODF_BUGS
            KoOdfWorkaround::fixPenWidth(pen, context);
#endif
            stroke->setLineWidth(pen.widthF());
            stroke->setJoinStyle(pen.joinStyle());
            stroke->setLineStyle(pen.style(), pen.dashPattern());
            stroke->setCapStyle(pen.capStyle());
            stroke->setColor(pen.color());

            return stroke;
        }
#endif
    }

    return 0;
}

KoShapeShadow *loadOdfShadow(KoShape *title, KoShapeLoadingContext &context)
{
    Q_UNUSED(title);
    KoStyleStack &styleStack = context.odfLoadingContext().styleStack();
    QString shadowStyle = getStyleProperty("shadow", context);
    if (shadowStyle == "visible" || shadowStyle == "hidden") {
        KoShapeShadow *shadow = new KoShapeShadow();
        QColor shadowColor(styleStack.property(KoXmlNS::draw, "shadow-color"));
        qreal offsetX = KoUnit::parseValue(styleStack.property(KoXmlNS::draw, "shadow-offset-x"));
        qreal offsetY = KoUnit::parseValue(styleStack.property(KoXmlNS::draw, "shadow-offset-y"));
        shadow->setOffset(QPointF(offsetX, offsetY));
        qreal blur = KoUnit::parseValue(styleStack.property(KoXmlNS::calligra, "shadow-blur-radius"));
        shadow->setBlur(blur);

        QString opacity = styleStack.property(KoXmlNS::draw, "shadow-opacity");
        if (! opacity.isEmpty() && opacity.right(1) == "%")
            shadowColor.setAlphaF(opacity.leftRef(opacity.length() - 1).toFloat() / 100.0);
        shadow->setColor(shadowColor);
        shadow->setVisible(shadowStyle == "visible");

        return shadow;
    }
    return 0;
}

KoBorder *loadOdfBorder(KoShape *title, KoShapeLoadingContext &context)
{
    Q_UNUSED(title);
    KoStyleStack &styleStack = context.odfLoadingContext().styleStack();

    KoBorder *border = new KoBorder();
    if (border->loadOdf(styleStack)) {
        return border;
    }
    delete border;
    return 0;
}

bool loadOdfTitle(KoShape *title, KoXmlElement &titleElement, KoShapeLoadingContext &context)
{
    TextLabelData *titleData = qobject_cast<TextLabelData*>(title->userData());
    if (!titleData)
        return false;

    // Following will always return false cause KoTextShapeData::loadOdf will try to load
    // a frame while our text:p is not within a frame. So, let's just not call loadOdf then...
    //title->loadOdf(titleElement, context);

    QTextDocument* doc = titleData->document();
    doc->setPlainText(QString()); // remove default text
    QTextCursor cursor(doc);
    QTextCharFormat charFormat = cursor.charFormat();

    title->setSize(QSize(0,0));
    title->setPosition(QPointF(0,0));

    bool autoPosition = !(titleElement.hasAttributeNS(KoXmlNS::svg, "x") && titleElement.hasAttributeNS(KoXmlNS::svg, "y"));
    bool autoSize = !(titleElement.hasAttributeNS(KoXmlNS::svg, "width") && titleElement.hasAttributeNS(KoXmlNS::svg, "height"));
    qreal rotationAngle = title->rotation();
    // Set the styles
    if (titleElement.hasAttributeNS(KoXmlNS::chart, "style-name")) {
        KoStyleStack &styleStack = context.odfLoadingContext().styleStack();
        styleStack.clear();
        context.odfLoadingContext().fillStyleStack(titleElement, KoXmlNS::chart, "style-name", "chart");

        styleStack.setTypeProperties("chart");
        if (styleStack.hasProperty(KoXmlNS::style, "rotation-angle")) {
            rotationAngle = 360 - KoUnit::parseValue(styleStack.property(KoXmlNS::style, "rotation-angle"));
            if (rotationAngle != title->rotation()) {
                title->rotate(rotationAngle);
            }
        }
        if (styleStack.hasProperty(KoXmlNS::style, "auto-position")) {
            autoPosition |= styleStack.property(KoXmlNS::style, "auto-position") == "true";
        }
        if (styleStack.hasProperty(KoXmlNS::style, "auto-size")) {
            autoSize |= styleStack.property(KoXmlNS::style, "auto-size") == "true" ;
        }
        // title->loadStyle(titleElement, context) is protected
        // so we duplicate some code:
        styleStack.setTypeProperties("graphic");

        title->setBackground(loadOdfFill(title, context));
        title->setStroke(loadOdfStroke(title, titleElement, context));
        title->setShadow(loadOdfShadow(title, context));
        title->setBorder(loadOdfBorder(title, context));

        styleStack.setTypeProperties("text");

        QFont font = doc->defaultFont();
        if (styleStack.hasProperty(KoXmlNS::fo, "font-size")) {
            const qreal fontSize = KoUnit::parseValue(styleStack.property(KoXmlNS::fo, "font-size"));
            font.setPointSizeF(fontSize);
        }
        if (styleStack.hasProperty(KoXmlNS::fo, "font-family")) {
            const QString fontFamily = styleStack.property(KoXmlNS::fo, "font-family");
            font.setFamily(fontFamily);
        }
        if (styleStack.hasProperty(KoXmlNS::fo, "font-style")) {
            QString fontStyle = styleStack.property(KoXmlNS::fo, "font-style");
            if (fontStyle == "italic") {
                font.setItalic(true);
            } else if (fontStyle == "oblique") {
                font.setStyle(QFont::StyleOblique);
            }
        }
        if (styleStack.hasProperty(KoXmlNS::fo, "font-weight")) {
            QString fontWeight = styleStack.property(KoXmlNS::fo, "font-weight");
            font.setWeight(fromOdfFontWeight(fontWeight));
        }
        doc->setDefaultFont(font);
        charFormat.setFont(doc->defaultFont());

        if (styleStack.hasProperty(KoXmlNS::fo, "color")) {
            const QColor color(styleStack.property(KoXmlNS::fo, "color"));
            charFormat.setForeground(color);
        }
        cursor.setCharFormat(charFormat);
    }
    title->setAdditionalStyleAttribute("chart:auto-position", autoPosition ? "true" : "false");
    if (!autoPosition) {
        QPointF pos;
        pos.setX(KoUnit::parseValue(titleElement.attributeNS(KoXmlNS::svg, "x", QString())));
        if (pos.x() < 0) {
            pos.setX(0);
        }
        pos.setY(KoUnit::parseValue(titleElement.attributeNS(KoXmlNS::svg, "y", QString())));
        if (pos.y() < 0) {
            pos.setY(0);
        }
        title->setPosition(pos);
        debugChartOdf<<"position:"<<"odf:"<<pos<<"title"<<title->position();
    }
    if (autoSize) {
        titleData->setResizeMethod(KoTextShapeDataBase::AutoResize);
    } else {
        titleData->setResizeMethod(KoTextShapeDataBase::NoResize);
        QSizeF size;
        size.setWidth(KoUnit::parseValue(titleElement.attributeNS(KoXmlNS::svg, "width")));
        size.setHeight(KoUnit::parseValue(titleElement.attributeNS(KoXmlNS::svg, "height")));
        title->setSize(size);
        debugChartOdf<<"size:"<<"odf:"<<size<<"title"<<title->size();
    }

    // load text
    bool loaded = false;
    if (context.documentResourceManager()) {
        const KoXmlElement textElement = KoXml::namedItemNS(titleElement, KoXmlNS::calligra, "text");
        if (!textElement.isNull()) {
            loaded = titleData->loadOdf(textElement, context, 0, title);
            title->setVisible(true);
        }
    }
    if (!loaded) {
        const KoXmlElement textElement = KoXml::namedItemNS(titleElement, KoXmlNS::text, "p");
        if (!textElement.isNull()) {
            cursor.insertText(textElement.text(), charFormat);
            title->setVisible(true);
        }
    }
    debugChartOdf<<title->position()<<title->size()<<titleData->document()->toPlainText();
    return true;
}

} // Namespace OdfHelper
} // Namespace KoChart
