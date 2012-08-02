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

    ///Construct the image grid to retrieve a particle and its neightbors
    void makeGrid();

    ///Vefify if two pairs of particles positions indicate a valid neighborhood relation
    bool is_valid_neighbor(int ix, int iy, int iix, int iiy);

    /**
     * Construct the neighborhood relations between particles to gain performance in particle
     * selection/collision operations
     */
    void makeNeighbors();

    /**
     * Retrieve particles from a given grid cell
     */

    void retrieveCellParticles(int gx, int gy, QList<Particle *> &p);

    /**
     * Retrieve the neighbors relationship from a given grid cell.
     * The QPair< a, b> represents that a particle of index value "a" in the
     * grains of the SandBrush is neighbor of the particle of the "b"
     */
    void getNeighborhood(int gx, int gy, QVector<QPair<int,int > > n);

    ///Retrieve the current Particles in use
    void getGrains(QList<Particle *> &g_copy);

    ///Set the Particles to be used in the desired operations
    void setGrains(QList<Particle *> &g_copy);



private:
    KisImageWSP m_image;
    KisPaintDeviceSP m_dab;
    SandBrush * m_sandBrush;
    KisPressureOpacityOption m_opacityOption;
    SandProperties m_properties;

    ///Hold the particles created by this paintop
    QList<Particle *> m_grains;

     /**
     * Hold the grid structure. The grid is created by taking the canvas width and height and
     * dividing each by a number of rows/columns disired.
     * Supose we have the m_grains list with the following particle structure :
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
     * are on this grid cell and making possible to search in the neightborhood of the
     * cell for the particles closest to B
     *
     * QList<uint> b_neighbors = grid.at(3).at(4);
     *
     */
    QVector< QVector<QVector<int> > > grid;

    /**
     * Vector representing the neighborhood of the particles.
     * The pair< int, int> represents a pair of neighbor particles
     * This relation is built based on search of half of neighborhood space
     * of one grid cell.
     */  
    QVector<QVector<QVector<QPair<int,int> > > > neighbors;


    /// Number of grid cells in the horizontal direction
    int g_numx;

    /// Number of grid cells in the vertical direction
    int g_numy;

};

#endif // KIS_SAND_PAINTOP_H_
