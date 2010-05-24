/* This file is part of the KDE project
 * Copyright ( C ) 2007 Thorsten Zachmann <zachmann@kde.org>
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

#include "KPrShapeAnimations.h"
#include "KPrPlaceholderShape.h"
#include "KPrAnimationDirector.h"
#include "shapeanimations/KPrShapeAnimationOld.h"

#include "kdebug.h"

KPrShapeManagerAnimationStrategy::KPrShapeManagerAnimationStrategy( KoShapeManager * shapeManager, KPrAnimationDirector * animationDirector )
: KoShapeManagerPaintingStrategy( shapeManager )
, m_animationDirector( animationDirector )
{
}

KPrShapeManagerAnimationStrategy::~KPrShapeManagerAnimationStrategy()
{
}

void KPrShapeManagerAnimationStrategy::paint( KoShape * shape, QPainter &painter, const KoViewConverter &converter, bool forPrint )
{
    if ( ! dynamic_cast<KPrPlaceholderShape *>( shape ) ) {
        if ( m_animationDirector->shapeShown( shape ) ) {
            //kDebug() << shape;
            painter.save();
            painter.setMatrix( shape->absoluteTransformation( &converter ) * painter.matrix() );
            // animate shape
            QPair<KPrShapeAnimationOld *, KPrAnimationData *> animation = m_animationDirector->shapeAnimation( shape );
            if ( animation.first ) {
                animation.first->animate( painter, converter, animation.second );
            }
            // paint shape
            shapeManager()->paintShape( shape, painter, converter, forPrint );
            painter.restore();  // for the matrix
        }
    }
}

void KPrShapeManagerAnimationStrategy::adapt( KoShape * shape, QRectF & rect )
{
    QPair<KPrShapeAnimationOld *, KPrAnimationData *> animation = m_animationDirector->shapeAnimation( shape );
    if ( animation.first ) {
        animation.first->animateRect( rect, animation.second );
    }
}
