/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2007, 2009 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoShapeManagerPaintingStrategy.h"

#include "KoShape.h"
#include "KoShapeManager.h"
#include <QPainter>

class Q_DECL_HIDDEN KoShapeManagerPaintingStrategy::Private
{
public:
    Private(KoShapeManager *manager)
        : shapeManager(manager)
    {
    }

    KoShapeManager *shapeManager;
};

KoShapeManagerPaintingStrategy::KoShapeManagerPaintingStrategy(KoShapeManager *shapeManager)
    : d(new KoShapeManagerPaintingStrategy::Private(shapeManager))
{
}

KoShapeManagerPaintingStrategy::~KoShapeManagerPaintingStrategy()
{
    delete d;
}

void KoShapeManagerPaintingStrategy::paint(KoShape *shape, QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &paintContext)
{
    if (d->shapeManager) {
        painter.save();
        painter.setTransform(shape->absoluteTransformation(&converter) * painter.transform());
        d->shapeManager->paintShape(shape, painter, converter, paintContext);
        painter.restore(); // for the matrix
    }
}

void KoShapeManagerPaintingStrategy::adapt(KoShape *shape, QRectF &rect)
{
    Q_UNUSED(shape);
    Q_UNUSED(rect);
}

void KoShapeManagerPaintingStrategy::setShapeManager(KoShapeManager *shapeManager)
{
    d->shapeManager = shapeManager;
}

KoShapeManager *KoShapeManagerPaintingStrategy::shapeManager()
{
    return d->shapeManager;
}
