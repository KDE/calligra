/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
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
