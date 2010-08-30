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

#ifndef KPRSHAPEANIMATIONOLD_H
#define KPRSHAPEANIMATIONOLD_H

#include <QTimeLine>

#include "kpresenter_export.h"

#define TIMEFACTOR 1000.0

class QPainter;
class QRectF;
class KoShape;
class KoShapeManager;
class KoCanvasBase;
class KoViewConverter;
class KPrAnimationData;

/**
 * This is the base class for shape animations.
 *
 * In the animation itself the state of the animation is not stored.
 * With this design it is possible to use the same animation object 
 * for running the same animation e.g. on different views or at a 
 * different time.
 * The state of the animation is kept in the animationData and is 
 * passed to the ainmation when it is run e.g. on a special view.
 */
class KPRESENTER_TEST_EXPORT KPrShapeAnimationOld
{
public:
    enum Type
    {
        Appear,
        Disappear
    };

    virtual ~KPrShapeAnimationOld();

    /**
     * Get a animation data object
     *
     * The object is created on the heap by new so the caller of this function
     * has to make sure to delete the object when he no longer needs it to 
     * avoid leaking memory. The object holds the data needed for running an 
     * animation.
     *
     * @param canvas The canvas on which the animation will take place
     * @return animationData the caller has to delete the animationData when
     *                       it is no longer used.
     */
    virtual KPrAnimationData * animationData( KoCanvasBase * canvas, KoShapeManager * shapeManager, const QRectF & pageRect ) = 0;

    /**
     * @brief Animate the shape
     *
     * This is done by maniplating the painter used for painting the shape.
     *
     * @param painter The painter used for painting the shape
     * @param converter The converter to convert between internal and view coordinates
     * @param animationData The data needed for running the animation
     *
     * @return true when the animations is finished
     */
    virtual bool animate( QPainter &painter, const KoViewConverter &converter, KPrAnimationData * animationData ) = 0;

    /**
     * @brief Update the bounding rect of the shape in the animation
     *
     * @param rect The bounding rect of the shape to update
     * @param animationData The data needed for running the animation
     */
    virtual void animateRect( QRectF & rect, KPrAnimationData * animationData ) = 0;

    /**
     * @brief Trigger an update of the canvas needed for the given time
     *
     * @param currentTime
     * @param animationData The data needed for running the animation
     */
    virtual void next( int currentTime, KPrAnimationData * animationData ) = 0;

    /**
     * @brief Finish the shape animation
     *
     * @param animationData The data needed for running the animation
     */
    virtual void finish( KPrAnimationData * animationData ) = 0;

    /**
     * Get the duration of the shape animation
     *
     * @return The duration of the shape animation
     */
    int duration() const;

    /**
     * @brief Get the step on which the animation is shown.
     */
    int step() const;

    /**
     * @brief Set the step on which the animation is shown.
     */
    void setStep( int step );

    /**
     * @brief Get the shape the animation is for
     */
    KoShape * shape() const;

    /**
     * @brief Get the type of the animation
     */
    Type type() const;

protected:
    /**
     * Constructor
     *
     * Only to be called form derived classes
     */
    KPrShapeAnimationOld( KoShape * shape, int step, Type type );

    // the shape for which is aminated
    KoShape * m_shape;
    // the timeline used for calculating the animation position
    QTimeLine m_timeLine;
    // the step ( click ) on which the animation is done
    int m_step;
    // The type of animation
    Type m_type;
    // Indicates that the animation is finished
    bool m_finished;
};

#endif // KPRSHAPEANIMATIONOLD_H
