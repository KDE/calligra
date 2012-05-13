/* This file is part of the KDE project
 * Copyright ( C ) 2007 Thorsten Zachmann <zachmann@kde.org>
 * Copyright (C) 2010 Benjamin Port <port.benjamin@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (  at your option ) any later version.
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

#include "KPrShapeManagerAnimationStrategy.h"

#include <QPainter>
#include <KoShape.h>
#include <KoShapeManager.h>
#include <KoPAPageBase.h>

#include "KPrShapeAnimations.h"
#include "KPrPlaceholderShape.h"
#include "animations/KPrAnimationCache.h"
#include "KPrPageSelectStrategyBase.h"
#include "kdebug.h"

KPrShapeManagerAnimationStrategy::KPrShapeManagerAnimationStrategy( KoShapeManager *shapeManager, KPrAnimationCache* animationCache,
                                                                    KPrPageSelectStrategyBase * strategy )
: KoShapeManagerPaintingStrategy( shapeManager )
, m_animationCache( animationCache )
, m_strategy( strategy )
{
}

KPrShapeManagerAnimationStrategy::~KPrShapeManagerAnimationStrategy()
{
    delete m_strategy;
}

void KPrShapeManagerAnimationStrategy::paint( KoShape * shape, QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &paintContext)
{
    if ( ! dynamic_cast<KPrPlaceholderShape *>( shape ) && m_strategy->page()->displayShape( shape ) ) {
        if ( m_animationCache->value(shape, "visibility", true).toBool() ) {
            painter.save();
            QTransform animationTransform = m_animationCache->value(shape, "transform", QTransform()).value<QTransform>();;
            QTransform transform(painter.transform() * shape->absoluteTransformation(&converter));
            if (animationTransform.isScaling()) {
                transform = animationTransform * transform;
            } else {
                transform = transform * animationTransform;
            }

            painter.setTransform(transform);

            //Try to draw a swipe
            QPainterPath m_shape;
            m_shape.addEllipse(0, 0, 10, 10 );

            int width = shape->boundingRect().width();
            int height = shape->boundingRect().height();
            int currPos = (1 / shape->boundingRect().width());//*0.5;
            int scaleStep = 0;
            //m_shape.addEllipse(-width/2,-height/2, width/2, height/2);
            if( width > height )
            {
                scaleStep = 1 / shape->boundingRect().width();
            }
            else
            {
                scaleStep = 1 / shape->boundingRect().height();
            }
            QTransform matrix;
            matrix.translate( width/2+5, height/2+5 );
            //matrix.scale(currPos*scaleStep, currPos*scaleStep);
            //m_shape.moveTo(shape->position().x()+width/2, shape->position().y()+height/2);
            matrix.scale(5, 5);

            painter.setClipPath(matrix.map(m_shape));
            // paint shape
            shapeManager()->paintShape( shape, painter, converter, paintContext);
            painter.restore();  // for the transform
        }
    }
}

void KPrShapeManagerAnimationStrategy::adapt( KoShape * shape, QRectF & rect )
{
    Q_UNUSED(shape)
    Q_UNUSED(rect)
}
