/* This file is part of the KDE project
 * Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>
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

#ifndef KOENHANCEDPATHHANDLE_H
#define KOENHANCEDPATHHANDLE_H

#include <QPointF>

class KoEnhancedPathShape;
class KoEnhancedPathParameter;

/**
 * An interaction handle used by the KoEnhancedPathShape for
 * changing the shape interactively.
 */
class KoEnhancedPathHandle
{
public:
    /**
     * Constructs a new handle with the given coordinate parameters.
     *
     * @param x the x-coordinate of the handle position
     * @param y the y-coordinate of the handle position
     */
    KoEnhancedPathHandle( KoEnhancedPathParameter * x, KoEnhancedPathParameter * y );

    /// Destroys the handle
    ~KoEnhancedPathHandle();

    /**
     * Evaluates the position of the handle.
     * @param path the enhanced path the handle is referenced from
     * @return the actual handle position
     */
    QPointF position( KoEnhancedPathShape * path );

    /**
     * Attemps to changes the position of the handle.
     * Only the coordinates of the handle which reference a modifier
     * can be changed. The new position is automatically stored into
     * the modifier of the given enhanced path.
     *
     * @param position the new position the handle to set
     * @param path the enhanced path the handle is referenced from
     */
    void setPosition( const QPointF &position, KoEnhancedPathShape * path );

    /**
     * Sets the range of the handles x-coordinate.
     *
     * A zero pointer has the effect of no maximum/minimum value.
     *
     * @param minX the minimum x-coordinate
     * @param maxX the maximum x-coordinate
     */
    void setRangeX( KoEnhancedPathParameter *minX, KoEnhancedPathParameter *maxX );

    /**
     * Sets the range of the handles y-coordinate.
     *
     * A zero pointer has the effect of no maximum/minimum value.
     *
     * @param minY the minimum y-coordinate
     * @param maxY the maximum y-coordinate
     */
    void setRangeY( KoEnhancedPathParameter *minY, KoEnhancedPathParameter *maxY );

private:
    KoEnhancedPathParameter * m_positionX; ///< the position x-coordinate 
    KoEnhancedPathParameter * m_positionY; ///< the position y-coordinate
    KoEnhancedPathParameter * m_minimumX;  ///< the minimum x-coordinate
    KoEnhancedPathParameter * m_minimumY;  ///< the minmum y-coordinate
    KoEnhancedPathParameter * m_maximumX;  ///< the maximum x-coordinate
    KoEnhancedPathParameter * m_maximumY;  ///< the maximum y-coordinate
};

#endif // KOENHANCEDPATHHANDLE_H
