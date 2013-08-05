 /* This file is part of the KDE project
 * Copyright (C) 2013 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#include "KoAnnotationLayoutManager.h"
#include <KoShape.h>
//#include <KoTextShapeData.h>

#include <QList>
#include <QHash>
#include <QMap>
#include <QtAlgorithms>
#include <QPainter>
#include <QPen>
#include <QLine>
#include <QLineF>

#include <kdebug.h>

#define default_shapeHeight 100.0
#define shapeWidth 150.0

int compare(const QPair < QPointF, KoShape * > &a, const QPair < QPointF, KoShape * > &b)
{
    if (a.first.y() == b.first.y()) {
        return a.first.x() < b.first.x();
    }
    return a.first.y() < b.first.y();
}

class KoAnnotationLayoutManager::Private
{
public:
    Private(qreal annotationX)
        : x(annotationX)
    {}

    qreal x;
    QList< QPair < QPointF, KoShape * > > annotationShapePositions;
};

KoAnnotationLayoutManager::KoAnnotationLayoutManager(qreal annotationX, QObject *parent)
    :d(new Private(annotationX))
{
    Q_UNUSED(parent);
}

KoAnnotationLayoutManager::~KoAnnotationLayoutManager()
{
}

void KoAnnotationLayoutManager::registerAnnotationRefPosition(KoShape *annotationShape, QPointF pos) {
    QList< QPair < QPointF, KoShape * > >::iterator i = d->annotationShapePositions.begin();
    while (i != d->annotationShapePositions.end()) {
        KoShape *shape = i->second;
        if (shape == annotationShape) {
            d->annotationShapePositions.erase(i);
            break;
        }
        i++;
    }
    d->annotationShapePositions.append(QPair< QPointF, KoShape * >(pos, annotationShape));
    kDebug() << "An annotation shape inserted to list: " << d->annotationShapePositions.count();

    layoutAnnotationShapes();
}

void KoAnnotationLayoutManager::removeAnnotationShape(KoShape *annotationShape) {
    QList< QPair < QPointF, KoShape * > >::iterator i = d->annotationShapePositions.begin();
    while (i != d->annotationShapePositions.constEnd()) {
        if (i->second == annotationShape) {
            d->annotationShapePositions.erase(i);
            break;
        }
        i++;
    }
    layoutAnnotationShapes();
}

void KoAnnotationLayoutManager::layoutAnnotationShapes()
{
    qreal currentY = 0.0;
    qStableSort(d->annotationShapePositions.begin(), d->annotationShapePositions.end(), compare);

    QList< QPair < QPointF, KoShape * > >::const_iterator i = d->annotationShapePositions.constBegin();
    while (i != d->annotationShapePositions.end()) {
        KoShape *shape = i->second;
        qreal refPosition = i->first.y();
        if (refPosition > currentY) {
            currentY = refPosition;
        }
        shape->update();
        shape->setSize(QSize(shapeWidth, shape->size().height()));
        shape->setPosition(QPointF(d->x, currentY));
        shape->update();
        currentY += shape->size().height() + 10.0;

        i++;
    }
}

void KoAnnotationLayoutManager::paintConnections(QPainter &painter, KoViewConverter *viewConverter)
{
    QPen pen(QColor(230, 216, 87));
    pen.setStyle(Qt::DashLine);
    pen.setWidth(2);
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setCapStyle(Qt::RoundCap);

    painter.setPen(pen);

    QList< QPair < QPointF, KoShape * > >::const_iterator i = d->annotationShapePositions.constBegin();
    while (i != d->annotationShapePositions.end() && !d->annotationShapePositions.isEmpty()) {
        KoShape *shape = i->second;

        kDebug() << "Shape Position:" << shape->position() << "text Position:" << i->first;

        QPointF shapePosition(viewConverter->documentToView(QPointF(shape->position().x(), (shape->position().y() + 25))));
        QPointF refTextPosition(viewConverter->documentToView(QPointF(i->first.x(), (i->first.y()))));
        QPointF connectionPoint(viewConverter->documentToView(QPointF((shape->position().x() - 50), (i->first.y()))));
        QPointF pointLine(viewConverter->documentToView(QPointF(i->first.x(), (i->first.y() + 5))));


        //draw first line, from shape to connectionPint.
        painter.drawLine(shapePosition, connectionPoint);
        // draw second line, from connectio point to reftext position.
        painter.drawLine(connectionPoint, refTextPosition);
        // draw pointer.
        painter.drawLine(refTextPosition, pointLine);

        i++;
    }
}
