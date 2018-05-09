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

#include "kochart_global.h"

namespace KoChart {
namespace OdfHelper {
        
// TODO: what todo?
static bool libreOfficeCompatible = true;

void saveOdfFont(KoGenStyle &style, const QFont& font, const QColor& color)
{
    style.addProperty("fo:font-family", font.family(), KoGenStyle::TextType);
    style.addPropertyPt("fo:font-size", font.pointSize(), KoGenStyle::TextType);
    style.addProperty("fo:color", color.isValid() ? color.name() : "#000000", KoGenStyle::TextType);
    int w = font.weight();
    style.addProperty("fo:font-weight", w == 50 ? "normal" : w == 75 ? "bold" : QString::number(qRound(w / 10.0) * 100), KoGenStyle::TextType);
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

    bodyWriter.addAttributePt("svg:x", title->position().x());
    bodyWriter.addAttributePt("svg:y", title->position().y());
    if (titleData->resizeMethod() == KoTextShapeDataBase::NoResize) {
        bodyWriter.addAttributePt("svg:width", title->size().width());
        bodyWriter.addAttributePt("svg:height", title->size().height());
    }

    QTextCursor cursor(titleData->document());
    QFont titleFont = cursor.charFormat().font();
    QColor color = cursor.charFormat().foreground().color();

    KoGenStyle autoStyle(KoGenStyle::ChartAutoStyle, "chart", 0);
    autoStyle.addPropertyPt("style:rotation-angle", 360 - title->rotation());
    saveOdfFont(autoStyle, titleFont, color);
    bodyWriter.addAttribute("chart:style-name", context.mainStyles().insert(autoStyle, "ch"));

    if (libreOfficeCompatible) {
        // lo does not support formatted text :(
        bodyWriter.startElement("text:p");
        bodyWriter.addTextNode(titleData->document()->toPlainText());
        bodyWriter.endElement(); // text:p
    } else {
        titleData->saveOdf(context);
    }
    bodyWriter.endElement(); // chart:title/subtitle/footer
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

    // Set the position
    QPointF pos = title->position();
    bool posChanged = false;
    if (titleElement.hasAttributeNS(KoXmlNS::svg, "x")) {
        pos.setX(KoUnit::parseValue(titleElement.attributeNS(KoXmlNS::svg, "x", QString())));
        posChanged = true;
    }
    if (titleElement.hasAttributeNS(KoXmlNS::svg, "y")) {
        pos.setY(KoUnit::parseValue(titleElement.attributeNS(KoXmlNS::svg, "y", QString())));
        posChanged = true;
    }
    if (posChanged) {
        title->setPosition(pos);
    }

    // Set the styles
    if (titleElement.hasAttributeNS(KoXmlNS::chart, "style-name")) {
        KoStyleStack &styleStack = context.odfLoadingContext().styleStack();
        styleStack.clear();
        context.odfLoadingContext().fillStyleStack(titleElement, KoXmlNS::chart, "style-name", "chart");

        styleStack.setTypeProperties("chart");
        if (styleStack.hasProperty(KoXmlNS::style, "rotation-angle")) {
            qreal rotationAngle = 360 - KoUnit::parseValue(styleStack.property(KoXmlNS::style, "rotation-angle"));
            title->rotate(rotationAngle);
        }

        styleStack.setTypeProperties("text");

        if (styleStack.hasProperty(KoXmlNS::fo, "font-size")) {
            const qreal fontSize = KoUnit::parseValue(styleStack.property(KoXmlNS::fo, "font-size"));
            QFont font = doc->defaultFont();
            font.setPointSizeF(fontSize);
            doc->setDefaultFont(font);
        }

        if (styleStack.hasProperty(KoXmlNS::fo, "font-family")) {
            const QString fontFamily = styleStack.property(KoXmlNS::fo, "font-family");
            QFont font = doc->defaultFont();
            font.setFamily(fontFamily);
            doc->setDefaultFont(font);
        }
        if (styleStack.hasProperty(KoXmlNS::fo, "font-style")) {
            QString fontStyle = styleStack.property(KoXmlNS::fo, "font-style");
            if (fontStyle == "italic") {
                QFont font = doc->defaultFont();
                font.setItalic(true);
                doc->setDefaultFont(font);
            } else if (fontStyle == "oblique") {
                // TODO
            }
        }
        if (styleStack.hasProperty(KoXmlNS::fo, "font-weight")) {
            QString fontWeight = styleStack.property(KoXmlNS::fo, "font-weight");
            //fo:font-weight attribute are normal, bold, 100, 200, 300, 400, 500, 600, 700, 800 or 900.
            if (fontWeight == "bold") {
                QFont font = doc->defaultFont();
                font.setBold(true);
                doc->setDefaultFont(font);
            } else {
                // TODO
            }
        }

        if (styleStack.hasProperty(KoXmlNS::fo, "color")) {
            const QColor color(styleStack.property(KoXmlNS::fo, "color"));
            charFormat.setForeground(color);
        }
        cursor.insertText(QString(), charFormat);
    }

    // load text
    if (libreOfficeCompatible) {
        const KoXmlElement textElement = KoXml::namedItemNS(titleElement, KoXmlNS::text, "p");
        if (!textElement.isNull()) {
            title->setVisible(true);
            cursor.insertText(textElement.text(), charFormat);
        }
    } else if (context.documentResourceManager()) {
        titleData->loadOdf(titleElement, context, 0, title);
    } else {
        // at least unit test can use this
        const KoXmlElement textElement = KoXml::namedItemNS(titleElement, KoXmlNS::text, "p");
        if (!textElement.isNull()) {
            title->setVisible(true);
            titleData->document()->setPlainText(textElement.text());
        }
    }

    // Set the size
    if (titleElement.hasAttributeNS(KoXmlNS::svg, "width") && titleElement.hasAttributeNS(KoXmlNS::svg, "height")) {
        const qreal width = KoUnit::parseValue(titleElement.attributeNS(KoXmlNS::svg, "width"));
        const qreal height = KoUnit::parseValue(titleElement.attributeNS(KoXmlNS::svg, "height"));
        title->setSize(QSizeF(width, height));
    }

    return true;
}

} // Namespace OdfHelper
} // Namespace KoChart
