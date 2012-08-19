/*
 *  Copyright (c) 2012 Francisco Fernandes <francisco.fernandes.j@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) ag_numy later version.
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



#include "kis_sand_paintop.h"
#include "kis_sand_paintop_settings.h"

#include <cmath>
#include <QRect>
#include <QFile>
#include <KoColor.h>
#include <KoColorSpace.h>

#include <kis_image.h>
#include <kis_debug.h>

#include <kis_global.h>
#include <kis_paint_device.h>
#include <kis_painter.h>
#include <kis_types.h>
#include <kis_paintop.h>
#include <kis_paint_information.h>

#include <kis_pressure_opacity_option.h>

KisSandPaintOp::KisSandPaintOp(const KisSandPaintOpSettings *settings, KisPainter * painter, KisImageWSP image)
        : KisPaintOp(painter),
          m_image(image)
{
    g_numx = g_numy = 100;
    m_opacityOption.readOptionSetting(settings);
    m_opacityOption.sensor()->reset();

    //Read the particle settings in the settings widget
    m_properties.readOptionSetting(settings);
    KoColorTransformation* transfo = 0;

    
    m_sandBrush = new SandBrush( &m_properties, transfo );

    //If there is an annotation with previouly added particles...
    if(m_image->annotation("Particle")){

        //Retrieving particles from an annotation...
        QList<Particle *> p;
        retrieveParticles(p);
        
        //Set all the particles in the m_grains
        setGrains(p);
        
        //BUild the grid structure with the actual particles positions
        makeGrid();

        //Create neighborhood
        makeNeighbors();
    }

}

KisSandPaintOp::~KisSandPaintOp()
{
    //Serialize the particles added by the pouring and hold them in an annotation
    QList<Particle *> parts;
    getGrains(parts);
    if(parts.size() > 0){

        QByteArray * b_array = new QByteArray();
        QDataStream stream(b_array, QIODevice::ReadWrite);
        for(int i = 0; i < parts.size(); i++){
            stream << *parts.at(i);
        }

        /*
        * Add particles to the "Particle" annotation
        */
        m_image->addAnnotation(KisAnnotationSP(new KisAnnotation( "Particle",
                                                                "Set of grains created by the paintop",
                                                                *b_array)
                                            )
                                );
    }

    delete m_sandBrush;
}

/**
 * Where the painting operation really happens...
 */
qreal KisSandPaintOp::paintAt(const KisPaintInformation& info)
{

    if (!painter())
        return 1.0;

    if (!m_dab) {
        m_dab = new KisPaintDevice(painter()->device()->colorSpace());
    } else {
        m_dab->clear();
    }

    //Gets the mouse position
    qreal x1, y1;
    x1 = info.pos().x();
    y1 = info.pos().y();

    quint8 origOpacity = m_opacityOption.apply(painter(), info);

    //if the user selected to do some grain pouring...
    if(!m_properties.mode){

        //Add particles to the canvas
        m_sandBrush->pouring(m_dab, x1, y1, painter()->paintColor(), info, m_image->size().width(), m_image->size().height() );

        //Isso ta meio estranho... depois ver se isso é necessario
        QList<Particle *> parts;
        m_sandBrush->getGrains(parts);

        //add recently added particles
        setGrains(parts);
        parts.clear();
        m_sandBrush->setGrains(parts);

    }

    //if spread is selected AND you have already spread some grains on the canvas ... 
    if(m_properties.mode && m_grains.size()){

        QList<Particle *> parts;
        getGrains(parts);
        
        //Get the grid cell where the mouse is now
        int gx = g_numx*x1/m_image->size().width();
        int gy = g_numy*y1/m_image->size().height();

        //Retrieve the particles from that cell and its neighborhood
        QList<Particle *> cell;
        if(gx <= g_numx && gy <= g_numy){
            retrieveCellParticles(gx, gy, cell);

            if(cell.size()){
                //Set the particles of this operation
                m_sandBrush->setGrains(cell);

                //Do the spread operations
                m_sandBrush->spread(m_dab, x1, y1, painter()->backgroundColor(), painter()->paintColor(), info, m_image->size().width(), m_image->size().height() );
            }
        }
    }
    
    QRect rc = m_dab->extent();
    painter()->bitBlt(rc.x(), rc.y(), m_dab, rc.x(), rc.y(), rc.width(), rc.height());
    painter()->renderMirrorMask(rc,m_dab);
    painter()->setOpacity(origOpacity);
    
    return 1.0;
}

void KisSandPaintOp::retrieveParticles(QList<Particle *> &p)
{
    KisAnnotationSP annot = m_image->annotation("Particle");
    QByteArray * array = &annot->annotation();

    QDataStream data( array , QIODevice::ReadWrite);

    while(!data.atEnd()){
        Particle *part = new Particle(true);
        data >> *part;
        p.append(part);
    }
}


void KisSandPaintOp::makeGrid()
{
    //Set the grid size
    grid.resize(g_numx);
    for(int ix = 0; ix < grid.size(); ix++){
        grid[ix].resize(g_numy);
    }

    //Get the grid cells where each particle is and hold it in the grid list
    for(int i = 0; i < m_grains.size(); i++){
        /*
         * Calculate the indices of the particles
         */
        int ix = int( g_numx * m_grains[i]->pos()->x()/m_image->width() );
        int iy = int( g_numy * m_grains[i]->pos()->y()/m_image->height() );

        /*
         * Verify the indices and the grid limits
         */
        if(( ix >= 0) && ( ix < g_numx ) && ( iy >= 0) && ( iy < g_numy )){
            //if the position is correct
            grid[ix][iy].push_back(i);
        } else {
            //if the position is out of the grid size
//             qDebug() << "out of bounds";
//             return;
        }
    }
    
}

bool KisSandPaintOp::is_valid_neighbor(int ix, int iy, int iix, int iiy)
{
    //(iix,iiy) is the lower-right corner cell of (ix,iy)
    if((iix == (ix-1+g_numx)%g_numx) && (iiy == (iy+1+g_numy)%g_numy))
        return true;

    //(iix,iiy) is the lower cell of (ix,iy)
    if((iix == (ix+g_numx)%g_numx) && (iiy == (iy+1+g_numy)%g_numy))
        return true;

    //(iix,iiy) is the lower-left corner cell of (ix,iy)
    if((iix == (ix+1+g_numx)%g_numx) && (iiy == (iy+1+g_numy)%g_numy))
        return true;

    //(iix,iiy) is the right cell of (ix,iy)
    if((iix == (ix+1+g_numx)%g_numx) && (iiy == (iy+g_numy)%g_numy))
        return true;

    //Otherwise...
    return false;
}

void KisSandPaintOp::makeNeighbors()
{

    int iix;
    int iiy;

    // Defining the X-coordinates for the neighborhood
    neighbors.resize(g_numx);

    for(int ix = 0; ix < g_numx; ix++){
        // Defining the Y-coordinates for the neighborhood
        neighbors[ix].resize(g_numy);
    }

    /*
     * Searching in the neighborhood (grid)
     */
    for(int ix = 0; ix < g_numx; ix++){
        for(int iy = 0; iy < g_numy; iy++){

            /*
             * Define the search space for cell (ix,iy)
             * It's sufficient to search only in the up, right, upper-right
             * and upper-left corners neighbor grids, since the commutativity
             * of the neighborhood can be used to restrict this search space.
             */
            for(int dx = -1; dx <= 1; dx++){
                for(int dy = -1; dy <= 1; dy++){

                    //Calculate all the neighbors of (ix,iy)
                    iix = ( ix + dx + g_numx)%g_numx;
                    iiy = ( iy + dy + g_numx)%g_numx;

                    /*
                     * Restric the neighborhood based on commutativity
                     * The result should be only the upper-left, up, upper-right,
                     * right neighbor grids of (ix,iy)
                     */
                    if(is_valid_neighbor(ix,iy,iix,iiy)) {
                        neighbors[ix][iy].push_back(QPair<int,int>(iix,iiy));
                    }
                }
            }
        }
    }
}


void KisSandPaintOp::retrieveCellParticles(int gx, int gy, QList<Particle *> &p)
{

    if(gx >= g_numx || gy >= g_numy || !grid[gx][gy].size()){
        return;
    }

    QVector<QPair<int,int > > n; //cell neighborhood
    getNeighborhood(gx, gy, n);
    
    //current cell particles
    for(int i = 0; i < grid[gx][gy].size(); i++){
        int index = grid[gx][gy][i];
        p.append(m_grains.at(index));
    }

    //neighbor cells particles
    for(int j = 0; j < n.size(); j++){
        int nx = n[j].first;
        int ny = n[j].second;
        
        for(int i = 0; i < grid[gx][gy].size(); i++){
            int index = grid[nx][ny][i];
            p.append(m_grains.at(index));
        }
    }
}

void KisSandPaintOp::getNeighborhood(int gx, int gy, QVector<QPair<int,int > > n)
{
    for(int i = 0; i < neighbors[gx][gy].size(); i++){
        QPair<int,int> pair = neighbors[gx][gy][i];    //index of the particle in the list
        n.push_back(pair);
    }
    
}

void KisSandPaintOp::getGrains(QList<Particle *> &g_copy){
        for(int i=0; i < m_grains.size(); i++)
            g_copy.append(m_grains[i]);
}

void KisSandPaintOp::setGrains(QList<Particle *> &g_copy){
    for(int i = 0; i < g_copy.size(); i++)
        m_grains.append(g_copy[i]);
}