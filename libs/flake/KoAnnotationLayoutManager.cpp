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

#include <QList>
#include <QHash>
#include <kdebug.h>

class KoAnnotationLayoutManager::Private
{
public:
    Private(qreal annotationX, qreal pageHeight) :
        x(annotationX),
        pageHeight(pageHeight)
    {}

    qreal x;
    qreal pageHeight;
    qreal shapeHeight;
    QHash<int, QList<KoShape*> > annotationShapeCollectio;
};

KoAnnotationLayoutManager::KoAnnotationLayoutManager(qreal annotationX, qreal pageHeight, QObject *parent)
    :d(new Private(annotationX, pageHeight))
{
    Q_UNUSED(parent);
    d->shapeHeight = default_shapeHeight;
}

KoAnnotationLayoutManager::~KoAnnotationLayoutManager()
{
}

void KoAnnotationLayoutManager::addAnnotationShape(KoShape *annotationShape, int pageNumber) {
    if (d->annotationShapeCollectio.keys().contains(pageNumber)) {
        // Check for repeated shape.
        if (d->annotationShapeCollectio.value(pageNumber).contains(annotationShape)) {
            return;
        }
        else {
            QList<KoShape*> list = d->annotationShapeCollectio.value(pageNumber);
            list.append(annotationShape);
            d->annotationShapeCollectio.insert(pageNumber, list);
            layoutAnnotationShapes();
        }
    }
    else {
        QList<KoShape*> list;
        list.append(annotationShape);
        d->annotationShapeCollectio.insert(pageNumber, list);
        layoutAnnotationShapes();
    }
}

void KoAnnotationLayoutManager::removeAnnotationShape(KoShape *annotationShape) {
    foreach (int key, d->annotationShapeCollectio.keys()) {
        if (d->annotationShapeCollectio.value(key).contains(annotationShape)) {
            QList<KoShape*> list = d->annotationShapeCollectio.value(key);
            list.removeOne(annotationShape);
            d->annotationShapeCollectio.insert(key, list);
            break;
        }
    }
    layoutAnnotationShapes();
}

void KoAnnotationLayoutManager::layoutAnnotationShapes()
{
    foreach(int key, d->annotationShapeCollectio.keys()) {
        // Set Default shape height for new page.
        d->shapeHeight = default_shapeHeight;
        QList<KoShape*> shapesList = d->annotationShapeCollectio.value(key);

        // Calculate the shape height.
        qreal height = d->pageHeight / shapesList.count();
        if ( height <= d->shapeHeight) {
            d->shapeHeight = height;
        }

        // Add each shape height and 20 (space area betwwen pages) to this sumY to finde next shape Y position.
        qreal sumY = ((key - 1) * (d->pageHeight + 20.0));
        foreach (KoShape *shape, shapesList) {
            shape->setSize(QSize(shapeWidth, d->shapeHeight));
            shape->setPosition(QPointF(d->x, sumY));
            sumY += shape->size().height();
        }
    }
}
