/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoGradientBackground.h"
#include "KoFlake.h"
#include "KoShapeBackground_p.h"
#include <KoOdfGraphicStyles.h>
#include <KoOdfLoadingContext.h>
#include <KoShapeSavingContext.h>
#include <KoStyleStack.h>
#include <KoXmlNS.h>

#include <FlakeDebug.h>

#include <QBrush>
#include <QPainter>
#include <QSharedPointer>

class KoGradientBackgroundPrivate : public KoShapeBackgroundPrivate
{
public:
    KoGradientBackgroundPrivate()
        : gradient(nullptr)
    {
    }

    QGradient *gradient;
    QTransform matrix;
};

KoGradientBackground::KoGradientBackground(QGradient *gradient, const QTransform &matrix)
    : KoShapeBackground(*(new KoGradientBackgroundPrivate()))
{
    Q_D(KoGradientBackground);
    d->gradient = gradient;
    d->matrix = matrix;
    Q_ASSERT(d->gradient);
    Q_ASSERT(d->gradient->coordinateMode() == QGradient::ObjectBoundingMode);
}

KoGradientBackground::KoGradientBackground(const QGradient &gradient, const QTransform &matrix)
    : KoShapeBackground(*(new KoGradientBackgroundPrivate()))
{
    Q_D(KoGradientBackground);
    d->gradient = KoFlake::cloneGradient(&gradient);
    d->matrix = matrix;
    Q_ASSERT(d->gradient);
    Q_ASSERT(d->gradient->coordinateMode() == QGradient::ObjectBoundingMode);
}

KoGradientBackground::~KoGradientBackground()
{
    Q_D(KoGradientBackground);
    delete d->gradient;
}

void KoGradientBackground::setTransform(const QTransform &matrix)
{
    Q_D(KoGradientBackground);
    d->matrix = matrix;
}

QTransform KoGradientBackground::transform() const
{
    Q_D(const KoGradientBackground);
    return d->matrix;
}

void KoGradientBackground::setGradient(const QGradient &gradient)
{
    Q_D(KoGradientBackground);
    delete d->gradient;

    d->gradient = KoFlake::cloneGradient(&gradient);
    Q_ASSERT(d->gradient);
    Q_ASSERT(d->gradient->coordinateMode() == QGradient::ObjectBoundingMode);
}

const QGradient *KoGradientBackground::gradient() const
{
    Q_D(const KoGradientBackground);
    return d->gradient;
}

void KoGradientBackground::paint(QPainter &painter,
                                 const KoViewConverter & /*converter*/,
                                 KoShapePaintingContext & /*context*/,
                                 const QPainterPath &fillPath) const
{
    Q_D(const KoGradientBackground);
    if (!d->gradient)
        return;
    QBrush brush(*d->gradient);
    brush.setTransform(d->matrix);

    painter.setBrush(brush);
    painter.drawPath(fillPath);
}

void KoGradientBackground::fillStyle(KoGenStyle &style, KoShapeSavingContext &context)
{
    Q_D(KoGradientBackground);
    if (!d->gradient)
        return;
    QBrush brush(*d->gradient);
    brush.setTransform(d->matrix);
    KoOdfGraphicStyles::saveOdfFillStyle(style, context.mainStyles(), brush);
}

bool KoGradientBackground::loadStyle(KoOdfLoadingContext &context, const QSizeF &shapeSize)
{
    Q_D(KoGradientBackground);
    KoStyleStack &styleStack = context.styleStack();
    if (!styleStack.hasProperty(KoXmlNS::draw, "fill"))
        return false;

    QString fillStyle = styleStack.property(KoXmlNS::draw, "fill");
    if (fillStyle == "gradient") {
        QBrush brush = KoOdfGraphicStyles::loadOdfGradientStyle(styleStack, context.stylesReader(), shapeSize);
        const QGradient *gradient = brush.gradient();
        if (gradient) {
            d->gradient = KoFlake::cloneGradient(gradient);
            d->matrix = brush.transform();

            // Gopalakrishna Bhat: If the brush has transparency then we ignore the draw:opacity property and use the brush transparency.
            //  Brush will have transparency if the svg:linearGradient stop point has stop-opacity property otherwise it is opaque
            if (brush.isOpaque() && styleStack.hasProperty(KoXmlNS::draw, "opacity")) {
                QString opacityPercent = styleStack.property(KoXmlNS::draw, "opacity");
                if (!opacityPercent.isEmpty() && opacityPercent.right(1) == "%") {
                    float opacity = qMin(QStringView{opacityPercent}.left(opacityPercent.length() - 1).toDouble(), 100.0) / 100;
                    QGradientStops stops;
                    foreach (QGradientStop stop, d->gradient->stops()) {
                        stop.second.setAlphaF(opacity);
                        stops << stop;
                    }
                    d->gradient->setStops(stops);
                }
            }

            return true;
        }
    }
    return false;
}
