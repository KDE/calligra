/* This file is part of the KDE project
 * Copyright (C) 2008 Jan Hambrecht <jaham@gmx.net>
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

#include "KarbonOutlinePaintingStrategy.h"

#include <KoShapeManager.h>
#include <KoShape.h>
#include <KoShapeStroke.h>

#include <QPen>
#include <QPainter>
#include <QPainterPath>

class OutlineStroke : public KoShapeStroke {
public:
    OutlineStroke()
        : m_pen(Qt::black) {
    }

    using KoShapeStroke::paint;

    void paint(KoShape *shape, QPainter &painter, const KoViewConverter &converter) override
    {
        KoShape::applyConversion(painter, converter);
        painter.strokePath(shape->outline(), m_pen);
    }

private:
    QPen m_pen;
};

KarbonOutlinePaintingStrategy::KarbonOutlinePaintingStrategy(KoShapeManager * shapeManager)
    : KoShapeManagerPaintingStrategy(shapeManager), m_stroke(new OutlineStroke())
{
    Q_ASSERT(shapeManager);
    shapeManager->setPaintingStrategy(this);
}

KarbonOutlinePaintingStrategy::~KarbonOutlinePaintingStrategy()
{
    delete m_stroke;
}

void KarbonOutlinePaintingStrategy::paint(KoShape * shape, QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &/*paintContext*/)
{
    painter.save();
    painter.setTransform(shape->absoluteTransformation(&converter) * painter.transform());

    m_stroke->paint(shape, painter, converter);

    painter.restore();
}
