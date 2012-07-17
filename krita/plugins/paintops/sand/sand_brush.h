/*
 *  Copyright (c) 2008-2010 Lukáš Tvrdý <lukast.dev@gmail.com>
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

class SandBrush
{

public:
    SandBrush(const SandProperties * properties, KoColorTransformation* transformation);
    ~SandBrush();
    void paint(KisPaintDeviceSP dev, qreal x, qreal y, const KoColor &color, const KisPaintInformation& info);
    void drawParticle(KisPainter &painter, qreal x, qreal y, QPointF vel, QPointF accel); //Obs
    void getGrains(QList<Particle *> &g_copy){
        for(int i=0; i < m_grains.size(); i++)
            g_copy.append(m_grains[i]);
    };

    void setGrains(QList<Particle *> &g_copy){
        for(int i=0; i < g_copy.size(); i++)
            m_grains.append(g_copy[i]);
    }

private:
    KoColor m_inkColor;
    int m_counter;                          //to calculate the mouse reduction in the sand depletion
    unsigned int m_grainCount;              //to calculate the amount of particles to be spread
    const SandProperties * m_properties;
    KoColorTransformation* m_transfo;
    int m_saturationId;
    int m_prevTime;
    QPointF m_prevVel;                      //previous mouse velocity to calculate acceleration
    QList<Particle *> m_grains;             //hold the particles created by this paintop

    /**
     * Hold the grid structure. The grid is created by taking the canvas width and height and
     * dividing each by a number of rows/columns disired.
     * Supose we have a m_grains list with the following particle structure :
     * 
     *  0 1 2 3 4
     *  | | | | |
     * [C,A,D,B,E]
     *
     * This container holds the grid cells where the particle is:
     *
     *    _0__1__2__3__4__5__6__7__8__9__10_11_12_13_
     * 0  |__|__|__|__|__|__|__|__|__|__|__|__|__|__|
     * 1  |__|__|__|__|__|__|__|__|_D|__|__|__|__|__|
     * 2  |__|__|__|__|__|__|__|__|__|__|__|__|__|__|
     * 3  |__|__|__|__|__|__|__|__|__|__|__|__|__|__|
     * 4  |__|__|__|__|__|_A|__|__|__|__|__|__|__|__|
     * 5  |__|__|__|__|__|__|__|__|__|__|__|__|__|__|
     * 6  |__|__|__|__|__|__|__|__|__|__|__|__|__|__|
     * 7  |__|__|__|__|__|__|__|__|__|__|__|__|__|__|
     * 8  |__|__|__|__|__|__|B_|__|__|__|__|__|__|__|
     * 9  |__|__|__|__|__|__|__|__|__|__|__|__|__|__|
     * 10 |__|__|__|__|__|__|__|__|__|__|__|__|__|__|
     * 11 |_E|__|__|__|__|__|__|__|__|_C|__|__|__|__|
     * 12 |__|__|__|__|__|__|__|__|__|__|__|__|__|__|
     * 13 |__|__|__|__|__|__|__|__|__|__|__|__|__|__|
     * 
     * If we have a grid of 7x7 cells of size 2, in pixels, if we want all the neightbors of B, I get the cell
     * where B is, and :
     * 
     * -> get B position (6,8)
     * -> divide by the image size (14,14)
     * -> multiply by the number of grids( 7,7)
     * 
     * => grid_b = (7,7)*((6,8)/(14,14)) = (3,4)
     * 
     *
     * In the position (3,4) of 'grid', we have a list of indices of all particles that
     * are closest to B.
     *
     * QList<uint> b_neighbors = grid.at(3).at(4);
     * 
     */
    QList< QList<QList<uint> > > grid;
};

#endif
