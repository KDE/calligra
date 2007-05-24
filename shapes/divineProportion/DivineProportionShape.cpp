/* This file is part of the KDE project
 * Copyright (C) 2006 Boudewijn Rempt <boud@valdyas.org>
 * Copyright (C) 2006-2007 Thomas Zander <zander@kde.org>
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

#include "DivineProportionShape.h"

#include <QPen>
#include <QPainter>
#include <KDebug>

#include <math.h>


DivineProportionShape::DivineProportionShape()
    : DivineProportion( (1 + sqrt(5)) / 2.0)
{
    setShapeId(DivineProportionShape_SHAPEID);
}

DivineProportionShape::~DivineProportionShape() {
}

void DivineProportionShape::paintDecorations(QPainter &painter, const KoViewConverter &converter, const KoCanvasBase *canvas) {
    applyConversion(painter, converter);
    painter.setPen(QPen(QColor(172, 196, 206)));
    QRectF rect(QPointF(0,0), size());
    divideVertical(painter, rect, false);

    painter.setPen(QPen(QColor(173, 123, 134)));
    painter.drawLine(QPointF(rect.width() / DivineProportion, 0), rect.bottomRight());
    painter.drawLine(rect.bottomLeft(), rect.topRight());
}

void DivineProportionShape::saveOdf(KoShapeSavingContext & context) const {
    // TODO
}

bool DivineProportionShape::loadOdf( const KoXmlElement & element, KoShapeLoadingContext &context ) {
    return false; // TODO
}

void DivineProportionShape::divideHorizontal(QPainter &painter, const QRectF &rect, bool top) {
    if(rect.height() < 5)
        return;
    const double y = rect.height() / DivineProportion;
    const double offset = top ? rect.bottom() - y : rect.top() + y;

    // draw horizontal line.
    painter.drawLine(QPointF(rect.left(), offset), QPointF(rect.right(), offset));
    divideVertical(painter, QRectF( QPointF(rect.left(), top ? rect.top() : offset),
                QSizeF(rect.width(), rect.height() - y)), top);
}

void DivineProportionShape::divideVertical(QPainter &painter, const QRectF &rect, bool left) {
    if(rect.width() < 5)
        return;
    const double x = rect.width() / DivineProportion;
    const double offset = left ? rect.right() - x : rect.left() + x;
    // draw vertical line
    painter.drawLine(QPointF(offset, rect.top()), QPointF(offset, rect.bottom()));
    divideHorizontal(painter, QRectF(QPointF( left ? rect.left() : offset, rect.top()),
                QSizeF(rect.width() - x, rect.height())), !left);
}
