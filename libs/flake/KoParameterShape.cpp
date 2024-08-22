/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2007, 2009 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoParameterShape.h"
#include "KoParameterShape_p.h"

#include <FlakeDebug.h>
#include <QPainter>

KoParameterShape::KoParameterShape()
    : KoPathShape(*(new KoParameterShapePrivate(this)))
{
}

KoParameterShape::KoParameterShape(KoParameterShapePrivate &dd)
    : KoPathShape(dd)
{
}

KoParameterShape::~KoParameterShape() = default;

void KoParameterShape::moveHandle(int handleId, const QPointF &point, Qt::KeyboardModifiers modifiers)
{
    Q_D(KoParameterShape);
    if (handleId >= d->handles.size()) {
        warnFlake << "handleId out of bounds";
        return;
    }

    update();
    // function to do special stuff
    moveHandleAction(handleId, documentToShape(point), modifiers);

    updatePath(size());
    update();
    d->shapeChanged(ParameterChanged);
}

int KoParameterShape::handleIdAt(const QRectF &rect) const
{
    Q_D(const KoParameterShape);
    int handle = -1;

    for (int i = 0; i < d->handles.size(); ++i) {
        if (rect.contains(d->handles.at(i))) {
            handle = i;
            break;
        }
    }
    return handle;
}

QPointF KoParameterShape::handlePosition(int handleId) const
{
    Q_D(const KoParameterShape);
    return d->handles.value(handleId);
}

void KoParameterShape::paintHandles(QPainter &painter, const KoViewConverter &converter, int handleRadius)
{
    Q_D(KoParameterShape);
    applyConversion(painter, converter);

    QTransform worldMatrix = painter.worldTransform();
    painter.setTransform(QTransform());

    QTransform matrix;
    matrix.rotate(45.0);
    QPolygonF poly(d->handleRect(QPointF(0, 0), handleRadius));
    poly = matrix.map(poly);

    // There exists a problem on msvc with for(each) and QVector<QPointF>
    for (int i = 0; i < d->handles.count(); ++i) {
        const QPointF moveVector = worldMatrix.map(d->handles[i]);
        poly.translate(moveVector.x(), moveVector.y());
        painter.drawPolygon(poly);
        poly.translate(-moveVector.x(), -moveVector.y());
    }
}

void KoParameterShape::paintHandle(QPainter &painter, const KoViewConverter &converter, int handleId, int handleRadius)
{
    Q_D(KoParameterShape);
    applyConversion(painter, converter);

    QTransform worldMatrix = painter.worldTransform();
    painter.setTransform(QTransform());

    QTransform matrix;
    matrix.rotate(45.0);
    QPolygonF poly(d->handleRect(QPointF(0, 0), handleRadius));
    poly = matrix.map(poly);
    poly.translate(worldMatrix.map(d->handles[handleId]));
    painter.drawPolygon(poly);
}

void KoParameterShape::setSize(const QSizeF &newSize)
{
    Q_D(KoParameterShape);
    QTransform matrix(resizeMatrix(newSize));

    for (int i = 0; i < d->handles.size(); ++i) {
        d->handles[i] = matrix.map(d->handles[i]);
    }

    KoPathShape::setSize(newSize);
}

QPointF KoParameterShape::normalize()
{
    Q_D(KoParameterShape);
    QPointF offset(KoPathShape::normalize());
    QTransform matrix;
    matrix.translate(-offset.x(), -offset.y());

    for (int i = 0; i < d->handles.size(); ++i) {
        d->handles[i] = matrix.map(d->handles[i]);
    }

    return offset;
}

bool KoParameterShape::isParametricShape() const
{
    Q_D(const KoParameterShape);
    return d->parametric;
}

void KoParameterShape::setParametricShape(bool parametric)
{
    Q_D(KoParameterShape);
    d->parametric = parametric;
    update();
}

QVector<QPointF> KoParameterShape::handles() const
{
    Q_D(const KoParameterShape);
    return d->handles;
}

void KoParameterShape::setHandles(const QVector<QPointF> &handles)
{
    Q_D(KoParameterShape);
    d->handles = handles;
}

int KoParameterShape::handleCount() const
{
    Q_D(const KoParameterShape);
    return d->handles.count();
}
