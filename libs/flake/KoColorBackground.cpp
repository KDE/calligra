/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoColorBackground.h"
#include "KoColorBackground_p.h"
#include "KoShapeSavingContext.h"
#include <KoOdfGraphicStyles.h>
#include <KoOdfLoadingContext.h>
#include <KoStyleStack.h>
#include <KoXmlNS.h>

#include <QColor>
#include <QPainter>

KoColorBackground::KoColorBackground()
    : KoShapeBackground(*(new KoColorBackgroundPrivate()))
{
}

KoColorBackground::KoColorBackground(KoShapeBackgroundPrivate &dd)
    : KoShapeBackground(dd)
{
}

KoColorBackground::KoColorBackground(const QColor &color, Qt::BrushStyle style)
    : KoShapeBackground(*(new KoColorBackgroundPrivate()))
{
    Q_D(KoColorBackground);
    if (style < Qt::SolidPattern || style >= Qt::LinearGradientPattern)
        style = Qt::SolidPattern;
    d->style = style;
    d->color = color;
}

KoColorBackground::~KoColorBackground() = default;

QColor KoColorBackground::color() const
{
    Q_D(const KoColorBackground);
    return d->color;
}

void KoColorBackground::setColor(const QColor &color)
{
    Q_D(KoColorBackground);
    d->color = color;
}

Qt::BrushStyle KoColorBackground::style() const
{
    Q_D(const KoColorBackground);
    return d->style;
}

void KoColorBackground::paint(QPainter &painter,
                              const KoViewConverter & /*converter*/,
                              KoShapePaintingContext & /*context*/,
                              const QPainterPath &fillPath) const
{
    Q_D(const KoColorBackground);
    painter.setBrush(QBrush(d->color, d->style));
    painter.drawPath(fillPath);
}

void KoColorBackground::fillStyle(KoGenStyle &style, KoShapeSavingContext &context)
{
    Q_D(KoColorBackground);
    KoOdfGraphicStyles::saveOdfFillStyle(style, context.mainStyles(), QBrush(d->color, d->style));
}

bool KoColorBackground::loadStyle(KoOdfLoadingContext &context, const QSizeF &)
{
    Q_D(KoColorBackground);
    KoStyleStack &styleStack = context.styleStack();
    if (!styleStack.hasProperty(KoXmlNS::draw, "fill"))
        return false;

    QString fillStyle = styleStack.property(KoXmlNS::draw, "fill");
    if (fillStyle == "solid" || fillStyle == "hatch") {
        QBrush brush = KoOdfGraphicStyles::loadOdfFillStyle(styleStack, fillStyle, context.stylesReader());
        d->color = brush.color();
        d->style = brush.style();
        return true;
    }

    return false;
}
