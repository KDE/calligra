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

#ifndef KPRSHAPEANIMATION_H
#define KPRSHAPEANIMATION_H

#include <QTimeLine>

class QPainter;
class QRectF;
class KoShape;
class KoCanvasBase;

class KPrShapeAnimation
{
public:
	KPrShapeAnimation( KoShape * shape, KoCanvasBase * canvas );
    virtual ~KPrShapeAnimation();

    /**
     * @brief Animate the shape
     *
     * This is done by maniplating the painter used for painting the shape.
     *
     * @param painter The painter used for painting the shape
     */
    virtual void animate( QPainter &painter ) = 0;

    /**
     * @brief Get the bounding rect of the shape in the animation
     *
     * @param rect The bounding rect of the shape
     *
     * @return The bounding rect in the animation
     */
    virtual QRectF animateRect( const QRectF & rect ) = 0;

    /**
     * @brief Trigger an update of the canvas needed for the given time
     *
     * @param currentTime
     */
    virtual void next( int currentTime ) = 0;

    /**
     * @brief Finish the shape animation
     */
    virtual void finish() = 0;

    /**
     * Get the duration of the shape animation
     *
     * @return The duration of the shape animation
     */
    int duration();

protected:
    // the shape for which is aminated
    KoShape * m_shape;
    // the canvas on which the animation takes place
    KoCanvasBase * m_canvas;
    // the timeline used for calculating the animation position
    QTimeLine m_timeLine;
};

#endif // KPRSHAPEANIMATION_H
