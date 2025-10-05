/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2012 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoHatchBackground.h"
#include "KoColorBackground_p.h"

#include <KoGenStyle.h>
#include <KoGenStyles.h>
#include <KoOdfLoadingContext.h>
#include <KoShapeSavingContext.h>
#include <KoStyleStack.h>
#include <KoUnit.h>
#include <KoXmlNS.h>

#include <FlakeDebug.h>

#include <QColor>
#include <QPainter>
#include <QPainterPath>
#include <QString>

using namespace Qt::StringLiterals;

class KoHatchBackgroundPrivate : public KoColorBackgroundPrivate
{
public:
    KoHatchBackgroundPrivate()
        : angle(0.0)
        , distance(1.0)
        , style(KoHatchBackground::Single)
    {
    }

    QColor lineColor;
    int angle;
    qreal distance;
    KoHatchBackground::HatchStyle style;
    QString name;
};

KoHatchBackground::KoHatchBackground()
    : KoColorBackground(*(new KoHatchBackgroundPrivate()))
{
}

void KoHatchBackground::paint(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &context, const QPainterPath &fillPath) const
{
    Q_D(const KoHatchBackground);
    if (d->color.isValid()) {
        // paint background color if set by using the color background
        KoColorBackground::paint(painter, converter, context, fillPath);
    }

    const QRectF targetRect = fillPath.boundingRect();
    painter.save();
    painter.setClipPath(fillPath);
    QPen pen(d->lineColor);
    // we set the pen width to 0.5 pt for the hatch. This is not defined in the spec.
    pen.setWidthF(0.5);
    painter.setPen(pen);
    QVector<QLineF> lines;

    // The different styles are handled by painting the lines multiple times with a different
    // angel offset as basically it just means we paint the lines also at a different angle.
    // This are the angle offsets we need to apply to the different lines of a style.
    // -90 is for single, 0 for the 2nd line in double and -45 for the 3th line in triple.
    const int angleOffset[] = {-90, 0, -45};
    // The number of loops is defined by the style.
    int loops = (d->style == Single) ? 1 : (d->style == Double) ? 2 : 3;

    for (int i = 0; i < loops; ++i) {
        int angle = d->angle - angleOffset[i];
        qreal cosAngle = ::cos(angle / 180.0 * M_PI);
        // if cos is nearly 0 the lines are horizontal. Use a special case for that
        if (qAbs(cosAngle) > 0.00001) {
            qreal xDiff = tan(angle / 180.0 * M_PI) * targetRect.height();
            // calculate the distance we need to increase x when creating the lines so that the
            // distance between the lines is also correct for rotated lines.
            qreal xOffset = qAbs(d->distance / cosAngle);

            // if the lines go to the right we need to start more to the left. Get the correct start.
            qreal xStart = 0;
            while (-xDiff < xStart) {
                xStart -= xOffset;
            }

            // if the lines go to the left we need to stop more at the right. Get the correct end offset
            qreal xEndOffset = 0;
            if (xDiff < 0) {
                while (xDiff < -xEndOffset) {
                    xEndOffset += xOffset;
                }
            }
            // create line objects.
            lines.reserve(lines.size() + int((targetRect.width() + xEndOffset - xStart) / xOffset) + 1);
            for (qreal x = xStart; x < targetRect.width() + xEndOffset; x += xOffset) {
                lines.append(QLineF(x, 0, x + xDiff, targetRect.height()));
            }
        } else {
            // horizontal lines
            lines.reserve(lines.size() + int(targetRect.height() / d->distance) + 1);
            for (qreal y = 0; y < targetRect.height(); y += d->distance) {
                lines.append(QLineF(0, y, targetRect.width(), y));
            }
        }
    }

    painter.drawLines(lines);
    painter.restore();
}

void KoHatchBackground::fillStyle(KoGenStyle &style, KoShapeSavingContext &context)
{
    Q_D(KoHatchBackground);

    KoGenStyle::Type type = style.type();
    KoGenStyle::PropertyType propertyType = (type == KoGenStyle::GraphicStyle || type == KoGenStyle::GraphicAutoStyle || type == KoGenStyle::DrawingPageStyle
                                             || type == KoGenStyle::DrawingPageAutoStyle)
        ? KoGenStyle::DefaultType
        : KoGenStyle::GraphicType;

    style.addProperty("draw:fill", "hatch", propertyType);
    style.addProperty("draw:fill-hatch-name", saveHatchStyle(context), propertyType);
    bool fillHatchSolid = d->color.isValid();
    style.addProperty("draw:fill-hatch-solid", fillHatchSolid, propertyType);
    if (fillHatchSolid) {
        style.addProperty("draw:fill-color", d->color.name(), propertyType);
    }
}

QString KoHatchBackground::saveHatchStyle(KoShapeSavingContext &context) const
{
    Q_D(const KoHatchBackground);
    KoGenStyle hatchStyle(KoGenStyle::HatchStyle /*no family name*/);
    hatchStyle.addAttribute("draw:display-name", d->name);
    hatchStyle.addAttribute("draw:color", d->lineColor.name());

    hatchStyle.addAttributePt("draw:distance", d->distance);

    hatchStyle.addAttribute("draw:rotation", QString("%1").arg(d->angle * 10));

    switch (d->style) {
    case Single:
        hatchStyle.addAttribute("draw:style", "single");
        break;
    case Double:
        hatchStyle.addAttribute("draw:style", "double");
        break;
    case Triple:
        hatchStyle.addAttribute("draw:style", "triple");
        break;
    }

    return context.mainStyles().insert(hatchStyle, "hatch");
}

bool KoHatchBackground::loadStyle(KoOdfLoadingContext &context, const QSizeF &shapeSize)
{
    // <draw:hatch draw:name="hatchStyle3" draw:color="#000000" draw:display-name="#000000 Vertical" draw:distance="0.102cm" draw:rotation="900"
    // draw:style="single"/>
    Q_D(KoHatchBackground);
    Q_UNUSED(shapeSize);

    KoStyleStack &styleStack = context.styleStack();
    QString fillStyle = styleStack.property(KoXmlNS::draw, "fill");
    if (fillStyle == "hatch") {
        QString style = styleStack.property(KoXmlNS::draw, "fill-hatch-name");
        debugFlake << " hatch style is  :" << style;

        KoXmlElement *draw = context.stylesReader().drawStyles("hatch").value(style);
        if (draw) {
            debugFlake << "Hatch style found for:" << style;

            QString angle = draw->attributeNS(KoXmlNS::draw, "rotation", QString("0"));
            if (angle.at(angle.size() - 1).isLetter()) {
                d->angle = KoUnit::parseAngle(angle);
            } else {
                // OO saves the angle value without unit and multiplied by a factor of 10
                d->angle = int(angle.toInt() / 10);
            }

            debugFlake << "angle :" << d->angle;

            d->name = draw->attributeNS(KoXmlNS::draw, "display-name");

            // use 2mm as default, just in case it is not given in a document so we show something sensible.
            d->distance = KoUnit::parseValue(draw->attributeNS(KoXmlNS::draw, "distance", "2mm"));

            bool fillHatchSolid = styleStack.property(KoXmlNS::draw, "fill-hatch-solid") == QLatin1String("true");
            if (fillHatchSolid) {
                QString fillColor = styleStack.property(KoXmlNS::draw, "fill-color");
                if (!fillColor.isEmpty()) {
                    d->color = QColor::fromString(fillColor);
                } else {
                    d->color = QColor();
                }
            } else {
                d->color = QColor();
            }
            d->lineColor = QColor::fromString(draw->attributeNS(KoXmlNS::draw, "color", QString("#000000")));

            QString style = draw->attributeNS(KoXmlNS::draw, "style", QString());
            if (style == "double"_L1) {
                d->style = Double;
            } else if (style == "triple"_L1) {
                d->style = Triple;
            } else {
                d->style = Single;
            }
        }

        return true;
    }

    return false;
}
