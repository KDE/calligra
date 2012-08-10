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

#ifndef SAND_BRUSH_H_
#define SAND_BRUSH_H_

#include <QList>

#include <KoColor.h>
#include <kis_painter.h>

#include "kis_sandop_option.h"
#include "kis_paint_device.h"

#include "particle.h"

class KisPaintInformation;
class Particle;

/**
 * Do the direct operations in the canvas, like adding and manipulating the particles
 * to do the desired effects of the painting.
 */
class SandBrush
{

public:
    SandBrush(const SandProperties * properties, KoColorTransformation* transformation);
    ~SandBrush();

    ///Add particles in the canvas
    void pouring(KisPaintDeviceSP dev, qreal x, qreal y, const KoColor &color, const KisPaintInformation& info, int width, int height);

    ///Spread the particles in the canvas
    void spread(KisPaintDeviceSP dev, qreal x, qreal y, const KoColor &color, const KisPaintInformation& info, int width, int height);
    
    ///Draw a Particle in the canvas
    void drawParticle(KisPainter &painter, Particle *p); //Obs

    ///Retrieve the current Particles in use in the current operation
    void getGrains(QList<Particle *> &g_copy);

    ///Set the Particles to be used in the current operations
    void setGrains(QList<Particle *> &g_copy);

    ///Calculate the amount of particles to be spread
    unsigned int m_grainCount;
    
private:
    KoColor m_inkColor;

    /**
     * Counter to calculate the mouse reduction in the sand depletion (used in the asymptotic
     * function of paint operation)
     */
    int m_counter;



    ///Time of the previous mouse movement (to dynamic calculation purposes)
    int m_prevTime;

    ///Previous mouse velocity to calculate acceleration
    QPointF m_prevVel;

    ///Hold a few particles for this operation step
    QList<Particle *> m_grains;
    
    int m_saturationId;
    const SandProperties * m_properties;
    KoColorTransformation* m_transfo;
};

#endif
