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

#ifndef KPRSHAPEANIMATIONS_H
#define KPRSHAPEANIMATIONS_H

#define BOOST_MULTI_INDEX_DISABLE_SERIALIZATION
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/ordered_index.hpp>

#include <QMap>

#include "kpresenter_export.h"
#include "shapeanimations/KPrShapeAnimationOld.h"

struct shape_step_key : boost::multi_index::composite_key<
    KPrShapeAnimationOld,
    boost::multi_index::const_mem_fun<KPrShapeAnimationOld, KoShape *, &KPrShapeAnimationOld::shape>,
    boost::multi_index::const_mem_fun<KPrShapeAnimationOld, int, &KPrShapeAnimationOld::step>
>
{
};

typedef boost::multi_index_container<
    KPrShapeAnimationOld *,
    boost::multi_index::indexed_by<
        boost::multi_index::ordered_unique<
            shape_step_key
        >,
        boost::multi_index::ordered_non_unique<
            boost::multi_index::const_mem_fun<KPrShapeAnimationOld, int, &KPrShapeAnimationOld::step>
        >
    >
> ShapeAnimations;

typedef boost::multi_index::nth_index<ShapeAnimations,1>::type ShapeAnimationsByStep;

class KPRESENTER_TEST_EXPORT KPrShapeAnimations
{
public:
    KPrShapeAnimations();
    ~KPrShapeAnimations();

    /**
     * Add animation to the animations
     *
     * It the animation for this shape/step already exists it is replaced by the given one
     *
     * @parama animation the animation to insert
     */
    void add( KPrShapeAnimationOld * animation );

    /**
     * Remove  animation to the animations
     *
     * @parama animation the animation to remove
     */
    void remove( KPrShapeAnimationOld * animation );

    /**
     * Get the animations for the given step
     *
     * @param step the step for which the animations should be returned
     * @return A map of the shape -> animation if the animation is 0 the shape 
     *         is not visible
     */
    QMap<KoShape *, KPrShapeAnimationOld *> animations( int step ) const;

    /**
     * Get a list of used steps in the animations
     *
     * Step 0 is the step for showing a new page. The list will always contain 
     * step 0 even when there is no animation for it.
     */
    QList<int> steps() const;

private:
    ShapeAnimations m_shapeAnimations;
};

#endif /* KPRSHAPEANIMATIONS_H */
