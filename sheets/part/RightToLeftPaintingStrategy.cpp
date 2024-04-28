/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "RightToLeftPaintingStrategy.h"

#include <QGraphicsWidget>
#include <QPainter>
#include <QWidget>

#include <KoCanvasBase.h>
#include <KoShape.h>
#include <KoShapeManager.h>

using namespace Calligra::Sheets;

class RightToLeftPaintingStrategy::Private
{
public:
    KoCanvasBase *canvas;
};

RightToLeftPaintingStrategy::RightToLeftPaintingStrategy(KoShapeManager *shapeManager, KoCanvasBase *canvas)
    : KoShapeManagerPaintingStrategy(shapeManager)
    , d(new Private)
{
    d->canvas = canvas;
}

RightToLeftPaintingStrategy::~RightToLeftPaintingStrategy()
{
    delete d;
}

void RightToLeftPaintingStrategy::paint(KoShape *shape, QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &paintContext)
{
    painter.save();
    const double width = d->canvas->canvasWidget()                 ? d->canvas->canvasWidget()->width()
        : dynamic_cast<QGraphicsWidget *>(d->canvas->canvasItem()) ? dynamic_cast<QGraphicsWidget *>(d->canvas->canvasItem())->size().width()
                                                                   : 0;
    //    const double offsetX = d->canvas->canvasController()->canvasOffsetX();
    painter.translate(/*-2 * offsetX*/ +width, 0);
    //     painter.scale(-1, 1);

    painter.setTransform(shape->absoluteTransformation(&converter) * painter.transform());

    if (shapeManager()) {
        shapeManager()->paintShape(shape, painter, converter, paintContext);
    }

    painter.restore(); // for the matrix
}

void RightToLeftPaintingStrategy::adapt(KoShape *shape, QRectF &rect)
{
    Q_UNUSED(shape)
    Q_UNUSED(rect)
    /*    const double width = d->canvas->canvasWidget()->width();
        const double offsetX = d->canvas->canvasController()->canvasOffsetX();
        const qreal left = width - rect.right();
        const qreal right = width - rect.left();
        rect.setLeft(left);
        rect.setRight(right);*/
    //     rect.translate(/*-2 * offsetX +*/ width, 0);
}
