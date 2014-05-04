/* This file is part of the KDE project
 * Copyright (C) 2014 Spencer Brown <sbrown655@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef KIS_RADIAXIAL_STROKE_SHORTCUT_H
#define KIS_RADIAXIAL_STROKE_SHORTCUT_H

#include <qpoint.h>
#include <qline.h>
#include "kis_abstract_shortcut.h"
#include "kis_stroke_shortcut.h"

class KRITAUI_EXPORT KisRadiaxialStrokeShortcut : public KisStrokeShortcut
{
public:
    // assumes distance threshold of infinity before differentiating axis
    KisRadiaxialStrokeShortcut(KisAbstractInputAction* inputAction, int index);
    // same here
    KisRadiaxialStrokeShortcut(KisAbstractInputAction* inputAction, int index,
                               QPointF const& initialPoint);


    // this ctor copies the passed point
    KisRadiaxialStrokeShortcut(KisAbstractInputAction* inputAction, int index,
                               QPointF const& initialPoint, int distanceThreshold);
    ~KisRadiaxialStrokeShortcut();
    KisRadiaxialStrokeShortcut(KisRadiaxialStrokeShortcut const& src);

    /* The following three methods, setInitialPoint, setCurrentPoint, and
     * setDistanceThreshold affect the circle that the cursor must pass
     * to differentiate the shortcut axially. For example, if you wanted to
     * place a straight guide ruler horizontally or vertically if the user
     * presses the shift key and strokes on the tablet, you would probably set
     * the threshold to 10 pixels or so.
     *
     * Once the stroke passes the threshold according to these methods, the
     * KisRadiaxialStrokeShortcut picks the closest axis according to the
     * direction the stroke was in and finalizes the shortcut. So in the ruler
     * case, you would place a ruler, enable stroke assistants (temporarily if
     * not already enabled) and start a normal stroke along that ruler.
     *
     * If you want the shortcut to avoid permanently differentiating axes
     * (e.g. you're making a shortcut for the rect tool to limit it to placing
     * squares and you want to always allow the user to make squares in any
     * quadrant), just set the distance threshold to -1 in the constructor or
     * with one of these methods. This will prevent the shortcut from passing the
     * threshold.
     **/

    /* Calling setInitialPoint will not cause the threshold to trigger, even if the
     * distance between the points is greater than the threshold
     *
     * So if you need to change the initial point for some reason, change the initial
     * point first, then the current point, or you may trigger the threshold, causing
     * a difficult-to-trace bug.
     **/
    void setInitialPoint(QPointF const& newPoint);
    void setCurrentPoint(QPointF const& newPoint);

    /* distance in pixels */
    void setDistanceThreshold(int distance);

    /* number of axes and the offset amount determine where the calculated axis will be */
    /* currently we only support constructing equally-spaced axes */

    /* gets the unit vector in the direction of the axis that the shortcut currently picks
     * based on the initial and current points */
    /* getAxis().p1() is always equal to the initial point */
    /* once the threshold is passed, the value this returns will never change */
    QLineF const& getAxis();

    /* once the current point moves outside of the threshold the axis will be finalized
     * and will no longer change */
    bool isAxisFinalized();

private:
    class Private;

    Private* const m_d;
};

#endif // KIS_RADIAXIAL_STROKE_SHORTCUT_H
