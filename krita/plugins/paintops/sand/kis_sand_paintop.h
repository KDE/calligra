/*
 *  Copyright (c) 2012 Francisco Fernandes <francisco.fernandes.j@gmail.com>
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

#ifndef KIS_SAND_PAINTOP_H_
#define KIS_SAND_PAINTOP_H_

#include <kis_paintop.h>
#include <kis_types.h>
#include <kis_annotation.h>

#include "sand_brush.h"
#include "kis_sand_paintop_settings.h"

class KisPainter;

class KisSandPaintOp : public KisPaintOp
{

public:

    KisSandPaintOp(const KisSandPaintOpSettings *settings, KisPainter * painter, KisImageWSP image);
    virtual ~KisSandPaintOp();

    qreal paintAt(const KisPaintInformation& info);
    
    /**
     * Fill the QList with the particles previously added by brush strokes.
     * The purpose is to populate the list of particles of the SandBrush class,
     * so it can do the spread.
     */
    void retrieveParticles(QList<Particle *> &p);

private:
    KisImageWSP m_image;
    KisPaintDeviceSP m_dab;
    SandBrush * m_sandBrush;
    KisPressureOpacityOption m_opacityOption;
    SandProperties m_properties;

};

#endif // KIS_SAND_PAINTOP_H_
