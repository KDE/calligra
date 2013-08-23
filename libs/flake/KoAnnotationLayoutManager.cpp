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

#define default_shapeHeight 150.0
#define shapeWidth 200.0

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
        : x(annotationX),
          shapeManager(0),
          canvas(0)
    {}

    qreal x;
    QList< QPair < QPointF, KoShape * > > annotationShapePositions;
    KoShapeManager *shapeManager;
    KoCanvasBase *canvas;
};

KoAnnotationLayoutManager::KoAnnotationLayoutManager(QObject *parent)
    :d(new Private(612.0))
{
    Q_UNUSED(parent);
}

void KoAnnotationLayoutManager::setShapeManager(KoShapeManager *shapeManager)
{
    if (d->shapeManager) {
        disconnect(d->shapeManager, SIGNAL(shapeChanged(KoShape*)), this, SLOT(updateLayout(KoShape*)));
    }
    d->shapeManager = shapeManager;
    connect(d->shapeManager, SIGNAL(shapeChanged(KoShape*)), this, SLOT(updateLayout(KoShape*)));
}

KoAnnotationLayoutManager::~KoAnnotationLayoutManager()
{
}

void KoAnnotationLayoutManager::registerAnnotationRefPosition(KoShape *annotationShape, QPointF pos) {
    QList< QPair < QPointF, KoShape * > >::iterator i = d->annotationShapePositions.begin();
    bool yPositionChanged = false;
    while (i != d->annotationShapePositions.end()) {
        KoShape *shape = i->second;
        if (shape == annotationShape) {
            if (i->first.y() != pos.y()) {
                yPositionChanged = true;
            }
            d->annotationShapePositions.erase(i);
            break;
        }
        i++;
    }
    d->annotationShapePositions.append(QPair< QPointF, KoShape * >(pos, annotationShape));
    layoutAnnotationShapes();
    // Update the free space between page outline rect and textarea (90).
    // 729.001 is viewMode content size height.
    //FIXME: Help me to find correct width.
    if (yPositionChanged) {
        d->canvas->updateCanvas(QRectF((d->x-90), 0, 90, 729.001));
    }
}
void KoAnnotationLayoutManager::setCanvasBase(KoCanvasBase *canvas)
{
    d->canvas = canvas;
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

    painter.save();

    QPen pen(QColor(230, 216, 87));
    pen.setStyle(Qt::DashLine);
    pen.setWidth(2);
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setCapStyle(Qt::RoundCap);

    painter.setPen(pen);

    QList< QPair < QPointF, KoShape * > >::const_iterator i = d->annotationShapePositions.constBegin();
    while (i != d->annotationShapePositions.end() && !d->annotationShapePositions.isEmpty()) {
        KoShape *shape = i->second;

        QPointF shapePosition(d->canvas->viewConverter()->documentToView(QPointF(shape->position().x(), (shape->position().y() + 25))));
        QPointF refTextPosition(d->canvas->viewConverter()->documentToView(QPointF(i->first.x(), (i->first.y()))));
        QPointF connectionPoint(d->canvas->viewConverter()->documentToView(QPointF((shape->position().x() - 50), (i->first.y()))));
        QPointF pointLine(d->canvas->viewConverter()->documentToView(QPointF(i->first.x(), (i->first.y() + 5))));


        //draw first line, from shape to connectionPint.
        painter.drawLine(shapePosition, connectionPoint);
        // draw second line, from connectio point to reftext position.
        painter.drawLine(connectionPoint, refTextPosition);
        // draw pointer.
        painter.drawLine(refTextPosition, pointLine);

        i++;
    }
    painter.restore();

}

void KoAnnotationLayoutManager::updateLayout(KoShape *shape)
{
    QList< QPair < QPointF, KoShape * > >::const_iterator i = d->annotationShapePositions.constBegin();
    while (i != d->annotationShapePositions.end() && !d->annotationShapePositions.isEmpty()) {
        if (i->second == shape) {
            layoutAnnotationShapes();
            break;
        }
        i++;
    }
}
