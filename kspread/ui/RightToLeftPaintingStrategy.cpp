/* This file is part of the KDE project
   Copyright 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "RightToLeftPaintingStrategy.h"

#include <QPainter>

#include <KoCanvasBase.h>
#include <KoCanvasController.h>
#include <KoShape.h>
#include <KoShapeManager.h>
#include <KoViewConverter.h>

using namespace KSpread;

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

void RightToLeftPaintingStrategy::paint(KoShape *shape, QPainter &painter,
                                        const KoViewConverter &converter, bool forPrint)
{
    painter.save();
    const double width = d->canvas->canvasWidget()->width();
//    const double offsetX = d->canvas->canvasController()->canvasOffsetX();
    painter.translate(/*-2 * offsetX*/ + width, 0);
//     painter.scale(-1, 1);

    painter.setTransform(shape->absoluteTransformation(&converter) * painter.transform());

    if (shapeManager()) {
        shapeManager()->paintShape(shape, painter, converter, forPrint);
    }

    painter.restore();  // for the matrix
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
