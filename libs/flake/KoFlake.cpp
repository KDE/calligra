/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jos van den Oever <jos@vandenoever.info>
 * SPDX-FileCopyrightText: 2009 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2010 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoFlake.h"
#include "KoShape.h"

#include <QGradient>
#include <math.h>

QGradient *KoFlake::cloneGradient(const QGradient *gradient)
{
    if (!gradient)
        return nullptr;

    QGradient *clone = nullptr;

    switch (gradient->type()) {
    case QGradient::LinearGradient: {
        const QLinearGradient *lg = static_cast<const QLinearGradient *>(gradient);
        clone = new QLinearGradient(lg->start(), lg->finalStop());
        break;
    }
    case QGradient::RadialGradient: {
        const QRadialGradient *rg = static_cast<const QRadialGradient *>(gradient);
        clone = new QRadialGradient(rg->center(), rg->radius(), rg->focalPoint());
        break;
    }
    case QGradient::ConicalGradient: {
        const QConicalGradient *cg = static_cast<const QConicalGradient *>(gradient);
        clone = new QConicalGradient(cg->center(), cg->angle());
        break;
    }
    default:
        return nullptr;
    }

    clone->setCoordinateMode(gradient->coordinateMode());
    clone->setSpread(gradient->spread());
    clone->setStops(gradient->stops());

    return clone;
}

QPointF KoFlake::toRelative(const QPointF &absolute, const QSizeF &size)
{
    return QPointF(size.width() == 0 ? 0 : absolute.x() / size.width(), size.height() == 0 ? 0 : absolute.y() / size.height());
}

QPointF KoFlake::toAbsolute(const QPointF &relative, const QSizeF &size)
{
    return QPointF(relative.x() * size.width(), relative.y() * size.height());
}
