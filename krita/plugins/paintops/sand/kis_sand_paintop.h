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

/**
 * KisSandPaintOp acts as a manager of the sand painting operation. It holds all the particles
 * created by the SandBrush class ( SandBrush::pouring() ), sending a smaller set of particles
 * to SandBrush to execute the spread ( SandBrush::spread() ), creates the canvas grid and its
 * neighborhood relationships to speed up the particles selection and physicals operations.
 * 
 * Some of the main interactions with outside classes, like annotations and image information
 * reading is done here and passed to SandBrush.
 * 
 */

class KisSandPaintOp : public KisPaintOp
{

public:

    KisSandPaintOp(const KisSandPaintOpSettings *settings, KisPainter * painter, KisImageWSP image);
    virtual ~KisSandPaintOp();

    /**
     */
    qreal paintAt(const KisPaintInformation& info);
    
    /**
     * @brief Fill the QList with the particles previously added by brush strokes.
     * The purpose is to populate the list of particles of the SandBrush class,
     * so it can do the spread.
     *
     * @param p The list which holds the retrived particles.
     */
    void retrieveParticles(QList<Particle *> &p);

    /**
     * @brief Construct the image grid to retrieve a particle and its neightbors.
     * 
     * @attention Does not take any parameter, since the grid dimensions are a property
     * of this class.
     */
    void makeGrid();

    /**
     * Vefify if two pairs of particles positions indicate a valid neighborhood relation.
     *
     * @param ix x-position of the first grid cell
     * @param iy y-position of the first grid cell
     * @param iix x-position of the second grid cell
     * @param iiy y-position of the second grid cell
     * 
     */
    bool isValidNeighbor(int ix, int iy, int iix, int iiy);

    /**
     * @brief Construct the neighborhood relations between particles to gain performance in
     * particle selection/collision operations.
     *
     * @attention In this stage, this method does not have a decisive operation, since the
     * colisions are only between the mouse and the particles already in the canvas. However,
     * to future improvements, this method will have a very important role, since it will
     * construct possible relations between grains.
     * 
     */
    void makeNeighbors();

    /**
     * @brief Retrieve particles from a given grid cell, so it can be used to any operation.
     * This method speed up the search of particles based on mouse position, so the physics
     * operations can be more responsive.
     *
     * @param gx grid cell x-position
     * @param gy grid cell y-position
     * @param p the QList that will hold the particles from this cell.
     *
     */
    void retrieveCellParticles(int gx, int gy, QList<Particle *> &p);

    /**
     * @brief Retrieve the neighbors relationship from a given grid cell.
     * The QPair< a, b> represents that a particle of index value "a" in the
     * grains of the SandBrush is neighbor of the particle of the "b"
     *
     * @param gx grid cell x-position
     * @param gy grid cell y-position
     * @param n QVector of pairs representing indices of neighbor particles in the m_grains list
     * 
     */
    void getNeighborhood(int gx, int gy, QVector<QPair<int,int > > n);

    /**
     * @brief Retrieve the current Particles in use
     *
     * @param p the QList that will hold the particles.
     */
    void getGrains(QList<Particle *> &g_copy);

    /**
     * @brief Set the Particles to be used in the desired operations
     *
     * @param p the QList that will hold the particles.
     */
    void setGrains(QList<Particle *> &g_copy);

    /**
     * @brief Prepare the particle settings when the brush is in the spread mode
     */
    void setSpreadParticles();

private:
    
    KisPaintDeviceSP m_dab;
    KisPressureOpacityOption m_opacityOption;
    SandProperties m_properties;
    
    //Used to manipulate the KisAnnotations that hold the particles
    KisImageWSP m_image;

    KisAnnotationSP m_annot;

    //Image to use in the grid's cell calculations
    uint m_imgWidth;
    uint m_imgHeight;

    //Do mathematical operations of the brush
    SandBrush * m_sandBrush;

    /**
     * Hold all the particles created by this paintop. In the constructor, its
     * called the method retrieveParticles so it can populate with the previously
     * added particles.
     *
     * Smaller sets of particles are taken from here so it can be passed to do
     * more faster operations.
     * 
     */
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
     * If we have a grid of 7x7 cells of size 2, in pixels, if we want all the neightbors of B,
     * I get the cell where B is, and :
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
     * QList< int> b_neighbors = grid[3][4]
     *
     */
    QVector< QVector<QVector<int> > > m_grid;

    /**
     * Vector representing the neighborhood of the particles.
     * The pair< int, int> represents a pair of neighbor particles
     * This relation is built based on search of half of neighborhood space
     * of one grid cell.
     */  
    QVector<QVector<QVector<QPair<int,int> > > > m_neighbors;


    /// Number of grid cells in the horizontal direction
    int m_gridX;

    /// Number of grid cells in the vertical direction
    int m_gridY;

};

#endif // KIS_SAND_PAINTOP_H_
