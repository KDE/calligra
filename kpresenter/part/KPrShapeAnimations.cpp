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

#include "KPrShapeAnimations.h"

KPrShapeAnimations::KPrShapeAnimations()
{
}

KPrShapeAnimations::~KPrShapeAnimations()
{
}

void KPrShapeAnimations::add( KPrShapeAnimationOld * animation )
{
    ShapeAnimations::iterator it( m_shapeAnimations.find( boost::make_tuple( animation->shape(), animation->step() ) ) );

    if ( it == m_shapeAnimations.end() ) {
        m_shapeAnimations.insert( animation );
    }
    else {
        m_shapeAnimations.replace( it, animation );
    }
}

void KPrShapeAnimations::remove( KPrShapeAnimationOld * animation )
{
    ShapeAnimations::iterator it( m_shapeAnimations.find( boost::make_tuple( animation->shape(), animation->step() ) ) );

    if ( it != m_shapeAnimations.end() ) {
        m_shapeAnimations.erase( it );
    }
}

QMap<KoShape *, KPrShapeAnimationOld *> KPrShapeAnimations::animations( int step ) const
{
    QMap<KoShape *, KPrShapeAnimationOld *> animationData;

    ShapeAnimations::const_iterator it = m_shapeAnimations.begin();

    while ( it != m_shapeAnimations.end() )
    {
        ShapeAnimations::const_iterator upper = m_shapeAnimations.upper_bound( boost::make_tuple( ( *it )->shape() ) );

        KPrShapeAnimationOld * animation = *it;
        if ( step < animation->step() )
        {
            // if the first animation is not Appear we don't need an entry
            if ( animation->type() == KPrShapeAnimationOld::Appear )
            {
                animationData.insert( animation->shape(), 0 );
            }
        }
        else
        {
            ShapeAnimations::const_iterator lower = m_shapeAnimations.lower_bound( boost::make_tuple( ( *it )->shape(), step ) );

            if ( lower != upper && ( *lower )->step() == step )
            {
                // animation for the step found
                animationData.insert( ( *lower )->shape(), *lower );
            }
            else {
                --lower;
                // if the last animation that was done was a disappear effect it is no longer visible
                if ( ( *lower )->type() == KPrShapeAnimationOld::Disappear )
                {
                    animationData.insert( animation->shape(), 0 );
                }
            }
        }

        it = upper;
    }

    return animationData;
}

QList<int> KPrShapeAnimations::steps() const
{
    QList<int> usedSteps;
    const ShapeAnimationsByStep & shapeAnimationsByStep = boost::multi_index::get<1>( m_shapeAnimations );
    ShapeAnimationsByStep::const_iterator it( shapeAnimationsByStep.begin() );

    while ( it != shapeAnimationsByStep.end() ) {
        usedSteps.push_back( ( *it )->step() );
        it = shapeAnimationsByStep.upper_bound( ( *it )->step() );
    }

    if ( usedSteps.empty() || usedSteps.at( 0 ) != 0 ) {
        usedSteps.insert( 0, 0 );
    }

    return usedSteps;
}
