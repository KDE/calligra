/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008-2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoSnapGuide.h"
#include "KoSnapProxy.h"
#include "KoSnapStrategy.h"

#include <KoCanvasBase.h>
#include <KoPathPoint.h>
#include <KoPathShape.h>
#include <KoViewConverter.h>

#include <QPainter>
#include <QPainterPath>

#include <math.h>

class Q_DECL_HIDDEN KoSnapGuide::Private
{
public:
    Private(KoCanvasBase *parentCanvas)
        : canvas(parentCanvas)
        , editedShape(nullptr)
        , currentStrategy(nullptr)
        , active(true)
        , snapDistance(10)
    {
    }

    ~Private()
    {
        qDeleteAll(strategies);
        strategies.clear();
    }

    KoCanvasBase *canvas;
    KoShape *editedShape;

    QList<KoSnapStrategy *> strategies;
    KoSnapStrategy *currentStrategy;

    KoSnapGuide::Strategies usedStrategies;
    bool active;
    int snapDistance;
    QList<KoPathPoint *> ignoredPoints;
    QList<KoShape *> ignoredShapes;
};

KoSnapGuide::KoSnapGuide(KoCanvasBase *canvas)
    : d(new Private(canvas))
{
    d->strategies.append(new GridSnapStrategy());
    d->strategies.append(new NodeSnapStrategy());
    d->strategies.append(new OrthogonalSnapStrategy());
    d->strategies.append(new ExtensionSnapStrategy());
    d->strategies.append(new IntersectionSnapStrategy());
    d->strategies.append(new BoundingBoxSnapStrategy());
    d->strategies.append(new LineGuideSnapStrategy());
}

KoSnapGuide::~KoSnapGuide()
{
    delete d;
}

void KoSnapGuide::setEditedShape(KoShape *shape)
{
    d->editedShape = shape;
}

KoShape *KoSnapGuide::editedShape() const
{
    return d->editedShape;
}

void KoSnapGuide::enableSnapStrategies(Strategies strategies)
{
    d->usedStrategies = strategies;
}

KoSnapGuide::Strategies KoSnapGuide::enabledSnapStrategies() const
{
    return d->usedStrategies;
}

bool KoSnapGuide::addCustomSnapStrategy(KoSnapStrategy *customStrategy)
{
    if (!customStrategy || customStrategy->type() != CustomSnapping)
        return false;

    d->strategies.append(customStrategy);
    return true;
}

void KoSnapGuide::enableSnapping(bool on)
{
    d->active = on;
}

bool KoSnapGuide::isSnapping() const
{
    return d->active;
}

void KoSnapGuide::setSnapDistance(int distance)
{
    d->snapDistance = qAbs(distance);
}

int KoSnapGuide::snapDistance() const
{
    return d->snapDistance;
}

QPointF KoSnapGuide::snap(const QPointF &mousePosition, Qt::KeyboardModifiers modifiers)
{
    d->currentStrategy = nullptr;

    if (!d->active || (modifiers & Qt::ShiftModifier))
        return mousePosition;

    KoSnapProxy proxy(this);

    qreal minDistance = HUGE_VAL;

    qreal maxSnapDistance = d->canvas->viewConverter()->viewToDocument(QSizeF(d->snapDistance, d->snapDistance)).width();

    foreach (KoSnapStrategy *strategy, d->strategies) {
        if (d->usedStrategies & strategy->type() || strategy->type() == GridSnapping || strategy->type() == CustomSnapping) {
            if (!strategy->snap(mousePosition, &proxy, maxSnapDistance))
                continue;

            QPointF snapCandidate = strategy->snappedPosition();
            qreal distance = KoSnapStrategy::squareDistance(snapCandidate, mousePosition);
            if (distance < minDistance) {
                d->currentStrategy = strategy;
                minDistance = distance;
            }
        }
    }

    if (!d->currentStrategy)
        return mousePosition;

    return d->currentStrategy->snappedPosition();
}

QRectF KoSnapGuide::boundingRect() const
{
    QRectF rect;

    if (d->currentStrategy) {
        rect = d->currentStrategy->decoration(*d->canvas->viewConverter()).boundingRect();
        return rect.adjusted(-2, -2, 2, 2);
    } else {
        return rect;
    }
}

void KoSnapGuide::paint(QPainter &painter, const KoViewConverter &converter)
{
    if (!d->currentStrategy || !d->active)
        return;

    QPainterPath decoration = d->currentStrategy->decoration(converter);

    painter.setBrush(Qt::NoBrush);

    QPen whitePen(Qt::white, 0);
    whitePen.setStyle(Qt::SolidLine);
    painter.setPen(whitePen);
    painter.drawPath(decoration);

    QPen redPen(Qt::red, 0);
    redPen.setStyle(Qt::DotLine);
    painter.setPen(redPen);
    painter.drawPath(decoration);
}

KoCanvasBase *KoSnapGuide::canvas() const
{
    return d->canvas;
}

void KoSnapGuide::setIgnoredPathPoints(const QList<KoPathPoint *> &ignoredPoints)
{
    d->ignoredPoints = ignoredPoints;
}

QList<KoPathPoint *> KoSnapGuide::ignoredPathPoints() const
{
    return d->ignoredPoints;
}

void KoSnapGuide::setIgnoredShapes(const QList<KoShape *> &ignoredShapes)
{
    d->ignoredShapes = ignoredShapes;
}

QList<KoShape *> KoSnapGuide::ignoredShapes() const
{
    return d->ignoredShapes;
}

void KoSnapGuide::reset()
{
    d->currentStrategy = nullptr;
    d->editedShape = nullptr;
    d->ignoredPoints.clear();
    d->ignoredShapes.clear();
    // remove all custom strategies
    int strategyCount = d->strategies.count();
    for (int i = strategyCount - 1; i >= 0; --i) {
        if (d->strategies[i]->type() == CustomSnapping) {
            delete d->strategies[i];
            d->strategies.removeAt(i);
        }
    }
}
