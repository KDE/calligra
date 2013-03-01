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
#include <kdebug.h>

class KoAnnotationLayoutManager::Private
{
public:
    Private(qreal annotationX, qreal pageHeight) :
        x(annotationX),
        pageHeight(pageHeight)
    {}

    qreal x; // FIXME:  WE should set Canvas X
    qreal pageHeight;
    qreal shapeHeight;
    QList<KoShape*> annotationShapeCollectio;
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

void KoAnnotationLayoutManager::addAnnotationShape(KoShape *annotationShape) {
    // Don't add repeated shapes.
    if (!d->annotationShapeCollectio.contains(annotationShape)) {
        d->annotationShapeCollectio.append(annotationShape);
        layoutAnnotationShapes();
    }
}

void KoAnnotationLayoutManager::removeAnnotationShape(KoShape *annotationShape) {
    for (int index = 0; index < d->annotationShapeCollectio.length(); index++) {
        if (d->annotationShapeCollectio.at(index)->position().y() == annotationShape->position().y()) {
            d->annotationShapeCollectio.removeAt(index);
            break;
        }
    }
    layoutAnnotationShapes();
}

void KoAnnotationLayoutManager::layoutAnnotationShapes()
{
    qreal sumY = 0.0; // add each shape height to this sumY to finde next shape Y position.

    // Calculate the shape height.
    qreal height = d->pageHeight / d->annotationShapeCollectio.count();
    if ( height <= d->shapeHeight) {
        d->shapeHeight = height;
    }

    foreach (KoShape *shape, d->annotationShapeCollectio) {
        shape->setSize(QSize(shapeWidth, d->shapeHeight));
        shape->setPosition(QPointF(d->x, sumY));
        sumY += shape->size().height();
    }
}
