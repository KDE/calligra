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
#include <kdebug.h>

#define default_shapeHeight 100.0
#define shapeWidth 150.0


class KoAnnotationLayoutManager::Private
{
public:
    Private(qreal annotationX)
        : x(annotationX)
    {}

    qreal x;
    QMultiMap<qreal,KoShape *> annotationShapePositions;
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
    if (!d->annotationShapePositions.values().contains(annotationShape)) {
        d->annotationShapePositions.insert(pos.y(), annotationShape);
    }
    layoutAnnotationShapes();
}

void KoAnnotationLayoutManager::removeAnnotationShape(KoShape *annotationShape) {
    QMap<qreal, KoShape*>::const_iterator i = d->annotationShapePositions.constBegin();
    while (i != d->annotationShapePositions.constEnd()) {
        if (i.value() == annotationShape) {
            d->annotationShapePositions.remove(i.key(), i.value());
        }
        i++;
    }
    layoutAnnotationShapes();
}

void KoAnnotationLayoutManager::layoutAnnotationShapes()
{
    qreal currentY = 0.0;
    QMap<qreal, KoShape*>::const_iterator i = d->annotationShapePositions.constBegin();
    while (i != d->annotationShapePositions.constEnd()) {
        KoShape *shape = i.value();
        qreal refPosition = i.key();
        if (refPosition > currentY) {
            currentY = refPosition;
        }
        shape->setSize(QSize(shapeWidth, shape->size().height()));
        shape->setPosition(QPointF(d->x, currentY));
        currentY += shape->size().height() + 20.0;

        i++;
    }
}
