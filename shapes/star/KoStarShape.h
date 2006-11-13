/* This file is part of the KDE project
   Copyright (C) 2006 Jan Hambrecht <jaham@gmx.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KOSTARSHAPE_H
#define KOSTARSHAPE_H

#include "KoParameterShape.h"

#define KoStarShapeId "KoStarShape"

class KoStarShape : public KoParameterShape
{
public:
    KoStarShape();
    ~KoStarShape();

    /**
     * Sets the number of corners.
     *
     * The minimum accepted number of corners is 3.
     *
     * @param cornerCount the new number of corners
     */
    void setCornerCount( uint cornerCount );

    /// Returns the number of corners
    uint cornerCount();

    /**
     * Sets the radius of the base points.
     * @param baseRadius the new base radius
     */
    void setBaseRadius( double baseRadius );

    /// Returns the base radius
    double baseRadius();

    /**
     * Sets the radius of the tip points.
     * @param tipRadius the new tip radius
     */
    void setTipRadius( double tipRadius );

    /// Returns the tip radius
    double tipRadius();

    /**
     * Sets the roundness at the base points.
     *
     * A roundness value of zero disables the roundness.
     *
     * @param baseRoundness the new base roundness
     */
    void setBaseRoundness( double baseRoundness );

    /**
     * Sets the roundness at the tip points.
     *
     * A roundness value of zero disables the roundness.
     *
     * @param tipRoundness the new base roundness
     */
    void setTipRoundness( double tipRoundness );

    virtual void resize( const QSizeF &newSize );
protected:
    void moveHandleAction( int handleId, const QPointF & point, Qt::KeyboardModifiers modifiers = Qt::NoModifier );
    void updatePath( const QSizeF &size );
    void createPath();

private:
    /// Computes the star center point from the inner points
    QPointF computeCenter() const;
    enum Handles { base = 0, tip = 1 };
    uint m_cornerCount; ///< number of corners
    double m_radius[2]; ///< the different radii
    double m_angles[2]; ///< the offset angles
    double m_zoomX; ///< scaling in x
    double m_zoomY; ///< scaling in y
    double m_roundness[2]; ///< the roundness at the handles
    QPointF m_center;   ///< the star center point
    KoSubpath m_points;
};

#endif /* KOSTARSHAPE_H */


