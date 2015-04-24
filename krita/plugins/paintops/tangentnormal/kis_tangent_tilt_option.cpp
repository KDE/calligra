/* This file is part of the KDE project
 *
 * Copyright (C) 2015 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
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


#include "kis_tangent_tilt_option.h"
#include <cmath>

KisTangentTiltOption::KisTangentTiltOption()
: KisPaintOpOption(i18n("Tangent Tilt"), KisPaintOpOption::generalCategory(), false)
{

}
KisTangentTiltOption::~KisTangentTiltOption()
{

}
void KisTangentTiltOption::apply(const KisPaintInformation& info,quint8 *r,quint8 *g,quint8 *b)
{
    //formula based on http://nl.mathworks.com/help/matlab/ref/sph2cart.html
    //and http://www.cerebralmeltdown.com/programming_projects/Altitude%20and%20Azimuth%20to%20Vector/index.html
    
    //TODO: Have these take higher bitspaces into account, including floating point.
    qreal halfvalue = 128;
    qreal maxvalue = 255;
    
    //have the azimuth and altitude in degrees.
    qreal direction = KisPaintInformation::tiltDirection(info, true)*360.0;
    qreal elevation= (info.tiltElevation(info, 60.0, 60.0, true)*90.0);
    
    //TODO:subtract/add the rotation of the canvas.
    
    //TODO:limit the directin/elevation
    
    //convert to radians.
    //TODO: Convert this to kis_global's radian function.
    direction = direction*M_PI / 180.0;
    elevation = elevation*M_PI / 180.0;
    
    
    //make variables for axes for easy switching later on.
    qreal horizontal, vertical, depth;
    
    //spherical coordinates always center themselves around the origin, leading to values. We need to work around those...
    
    horizontal = cos(elevation)*sin(direction);
    if (horizontal>0.0) {
        horizontal= halfvalue+(fabs(horizontal)*halfvalue);
    }
    else {
        horizontal= halfvalue-(fabs(horizontal)*halfvalue);
        
    }
    vertical = cos(elevation)*cos(direction);
    if (vertical>0.0) {
        vertical = halfvalue+(fabs(vertical)*halfvalue);
    }
    else {
        vertical = halfvalue-(fabs(vertical)*halfvalue);
    }
    
    depth = sin(elevation)*maxvalue;//zTilt*255;
    
    //TODO: Allow for swizzle to decide this.(or something...)
    //assign right components to correct axes.
    *r = horizontal;
    *g = vertical;
    *b = depth;
}

