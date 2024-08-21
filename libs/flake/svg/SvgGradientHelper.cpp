/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007, 2009 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2010 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "SvgGradientHelper.h"

#include <QConicalGradient>
#include <QLinearGradient>
#include <QRadialGradient>

#include <KoFlake.h>
#include <cmath>

SvgGradientHelper::SvgGradientHelper()
    : m_gradient(nullptr)
    , m_gradientUnits(ObjectBoundingBox)
{
}

SvgGradientHelper::~SvgGradientHelper()
{
    delete m_gradient;
}

SvgGradientHelper::SvgGradientHelper(const SvgGradientHelper &other)
    : m_gradient(nullptr)
    , m_gradientUnits(ObjectBoundingBox)
{
    m_gradientUnits = other.m_gradientUnits;
    m_gradientTransform = other.m_gradientTransform;
    copyGradient(other.m_gradient);
}

SvgGradientHelper &SvgGradientHelper::operator=(const SvgGradientHelper &rhs)
{
    if (this == &rhs)
        return *this;

    m_gradientUnits = rhs.m_gradientUnits;
    m_gradientTransform = rhs.m_gradientTransform;
    copyGradient(rhs.m_gradient);

    return *this;
}

void SvgGradientHelper::setGradientUnits(Units units)
{
    m_gradientUnits = units;
}

SvgGradientHelper::Units SvgGradientHelper::gradientUnits() const
{
    return m_gradientUnits;
}

QGradient *SvgGradientHelper::gradient()
{
    return m_gradient;
}

void SvgGradientHelper::setGradient(QGradient *g)
{
    delete m_gradient;
    m_gradient = g;
}

void SvgGradientHelper::copyGradient(QGradient *other)
{
    delete m_gradient;
    m_gradient = duplicateGradient(other, QTransform());
}

QBrush SvgGradientHelper::adjustedFill(const QRectF &bound)
{
    QBrush brush;

    QGradient *g = adjustedGradient(bound);
    if (g) {
        brush = QBrush(*g);
        delete g;
    }

    return brush;
}

QTransform SvgGradientHelper::transform() const
{
    return m_gradientTransform;
}

void SvgGradientHelper::setTransform(const QTransform &transform)
{
    m_gradientTransform = transform;
}

QGradient *SvgGradientHelper::adjustedGradient(const QRectF &bound) const
{
    QTransform matrix;
    matrix.scale(0.01 * bound.width(), 0.01 * bound.height());

    return duplicateGradient(m_gradient, matrix);
}

QGradient *SvgGradientHelper::duplicateGradient(const QGradient *originalGradient, const QTransform &transform)
{
    if (!originalGradient)
        return nullptr;

    QGradient *duplicatedGradient = nullptr;

    switch (originalGradient->type()) {
    case QGradient::ConicalGradient: {
        const QConicalGradient *o = static_cast<const QConicalGradient *>(originalGradient);
        QConicalGradient *g = new QConicalGradient();
        g->setAngle(o->angle());
        g->setCenter(transform.map(o->center()));
        duplicatedGradient = g;
    } break;
    case QGradient::LinearGradient: {
        const QLinearGradient *o = static_cast<const QLinearGradient *>(originalGradient);
        QLinearGradient *g = new QLinearGradient();
        g->setStart(transform.map(o->start()));
        g->setFinalStop(transform.map(o->finalStop()));
        duplicatedGradient = g;
    } break;
    case QGradient::RadialGradient: {
        const QRadialGradient *o = static_cast<const QRadialGradient *>(originalGradient);
        QRadialGradient *g = new QRadialGradient();
        g->setCenter(transform.map(o->center()));
        g->setFocalPoint(transform.map(o->focalPoint()));
        g->setRadius(transform.map(QPointF(o->radius(), 0.0)).x());
        duplicatedGradient = g;
    } break;
    default:
        return nullptr;
    }

    duplicatedGradient->setCoordinateMode(originalGradient->coordinateMode());
    duplicatedGradient->setStops(originalGradient->stops());
    duplicatedGradient->setSpread(originalGradient->spread());

    return duplicatedGradient;
}

QGradient *SvgGradientHelper::convertGradient(const QGradient *originalGradient, const QSizeF &size)
{
    if (!originalGradient)
        return nullptr;

    if (originalGradient->coordinateMode() != QGradient::LogicalMode) {
        return duplicateGradient(originalGradient, QTransform());
    }

    QGradient *duplicatedGradient = nullptr;

    switch (originalGradient->type()) {
    case QGradient::ConicalGradient: {
        const QConicalGradient *o = static_cast<const QConicalGradient *>(originalGradient);
        QConicalGradient *g = new QConicalGradient();
        g->setAngle(o->angle());
        g->setCenter(KoFlake::toRelative(o->center(), size));
        duplicatedGradient = g;
    } break;
    case QGradient::LinearGradient: {
        const QLinearGradient *o = static_cast<const QLinearGradient *>(originalGradient);
        QLinearGradient *g = new QLinearGradient();
        g->setStart(KoFlake::toRelative(o->start(), size));
        g->setFinalStop(KoFlake::toRelative(o->finalStop(), size));
        duplicatedGradient = g;
    } break;
    case QGradient::RadialGradient: {
        const QRadialGradient *o = static_cast<const QRadialGradient *>(originalGradient);
        QRadialGradient *g = new QRadialGradient();
        g->setCenter(KoFlake::toRelative(o->center(), size));
        g->setFocalPoint(KoFlake::toRelative(o->focalPoint(), size));
        g->setRadius(KoFlake::toRelative(QPointF(o->radius(), 0.0), QSizeF(sqrt(size.width() * size.width() + size.height() * size.height()), 0.0)).x());
        duplicatedGradient = g;
    } break;
    default:
        return nullptr;
    }

    duplicatedGradient->setCoordinateMode(QGradient::ObjectBoundingMode);
    duplicatedGradient->setStops(originalGradient->stops());
    duplicatedGradient->setSpread(originalGradient->spread());

    return duplicatedGradient;
}
