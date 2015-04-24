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

#ifndef KIS_TANGENT_TILT_OPTION_H
#define KIS_TANGENT_TILT_OPTION_H

#include <kis_paint_information.h>
#include <kis_types.h>

#include <kis_paintop_option.h>

class KisPropertiesConfiguration;
class KisPainter;

class KisTangentTiltOption: public KisPaintOpOption//not really//
{
public:
    KisTangentTiltOption();
    ~KisTangentTiltOption();
    
    //takes the RGB values and will deform them depending on tilt.
    void apply(const KisPaintInformation& info,quint8 *r,quint8 *g,quint8 *b);
private:
};

#endif // KIS_TANGENT_TILT_OPTION_H
