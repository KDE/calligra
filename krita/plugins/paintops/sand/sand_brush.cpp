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

#if defined(_WIN32) || defined(_WIN64)
#include <stdlib.h>
#define srand48 srand
inline double drand48() {
    return double(rand()) / RAND_MAX;
}
#endif

#include "sand_brush.h"

#include <KoColor.h>
#include <KoColorSpace.h>
#include <KoColorTransformation.h>
#include <kis_paint_information.h>
#include <kis_painter.h>
#include <QVariant>
#include <QHash>

#include "kis_random_accessor_ng.h"
#include <cmath>
#include <ctime>
#include "particle.h"


SandBrush::SandBrush(const SandProperties* properties, KoColorTransformation* transformation)
{
    m_transfo = transformation;
    if (m_transfo) {
        m_transfo->setParameter(m_transfo->parameterId("h"), 0.0);
        m_saturationId = m_transfo->parameterId("s");
        m_transfo->setParameter(m_transfo->parameterId("v"), 0.0);
    }
    else {
        m_saturationId = -1;
    }
    
    m_properties = properties;
    m_grainCount = properties->amount * 100; //obs.: modify this later
    m_counter = 0;
    srand48(time(0));
    m_prevTime = 0;
    m_prevVel = QPointF(0,0);
    
}


SandBrush::~SandBrush()
{
    delete m_transfo;
}


void SandBrush::pouring(KisPaintDeviceSP dev, qreal x, qreal y, const KoColor &color, const KisPaintInformation& info, int width, int height)
{
    //We do not need to, in every mouse movement, add pixels, so it has been randomized (as in
    // the dirtThreshold), so it can drop particles more faster

    if(m_properties->sandDepletion && m_grainCount == 0)
        return;
    
    double moveThreshold = 0.9;
    double mRan = drand48();
    if ( mRan < moveThreshold ) {
        return;
    }

    int time = m_prevTime;

    /*
     * Mouse instant velocity and acceleration calculation
     */
    
    if(info.currentTime() != m_prevTime)
        time = info.currentTime() - time;
    
    QPointF pos = toQPointF(info.movement());
    QPointF vel(pos.x()/time, pos.y()/time);    //current velocity
    QPointF accel( (m_prevVel.x() - vel.x())/time, (m_prevVel.y() - vel.y())/time ); //current accel
    
    m_counter++;

    //IGNORE THIS FOR NOW
    // sand depletion: it's not working as it should. Get an better assymptotic function to reduce the radius
    qreal result = 0;
//     if (m_properties->sandDepletion) {
//         result = log((qreal)m_counter * m_properties->radius)/10;
//     }

    //Brush radius
    int r = m_properties->radius - int(result)*m_properties->radius;

    //Current color
    m_inkColor = color;

    //Current pixel
    int pixelX, pixelY;

    int radiusSquared =  r*r;

    //Threshold to the sand spread
    double dirtThreshold = 0.9;
    
    KisPainter drawer(dev);
    drawer.setPaintColor(m_inkColor);
    KisRandomAccessorSP accessor = dev->createRandomAccessorNG((int)x, (int)y);
    
    for (int by = -r; by <= r; by++) {
        int bySquared = by*by;
        for (int bx = -r; bx <= r; bx++) {

            double ran = drand48();
            if ( ((bx*bx + bySquared) > radiusSquared) || ran < dirtThreshold) {
                continue;
            }


            /*
             * This is not working as desired if the mouse movement is only in the
             * vertical or horizontal direction (all particles are aligned and it draws
             * a line, instead of spreading the sand)
             */
            pixelX = qRound(x + bx*vel.x()*10);
            pixelY = qRound(y + by*vel.y()*10);

            //Create a particle with the current settings in the widget
            Particle *p = new Particle ( true,
                                float( m_properties->mass), //mass
                                0.0,                        //force
                                float( m_properties->size), //particle radius
                                1000,                       //lifespan (not used for now)
                                float( m_properties->friction), //friction (used in the force application
                                0.0,                            //dissipation (not used for now)
                                new QPoint(pixelX, pixelY), //position
                                new QPointF(vel.x(), vel.y()),  //velocity 
                                new QPointF(accel.x(), accel.y()) //acceleration
                            );
            //to normalization
            p->setBounds(new QPoint(width, height));

            //Draw the particle on the canvas
            drawParticle(drawer, p);

            //Put the particle in the list
            m_grains.append(p);
            
            if(m_grainCount > 0)
                m_grainCount--;
        }
    }

    //Update the velocity and time of the mouse
    m_prevVel = vel;
    m_prevTime = info.currentTime();
//     qDebug() << "Amount added : " << m_gr4ains.size();
}

//I'm actually trying to do this function work properly
void SandBrush::spread(KisPaintDeviceSP dev, qreal x, qreal y, const KoColor &color, const KisPaintInformation& info, int width, int height)
{

    KisPainter drawer(dev);
//     m_inkColor = color; //
    drawer.setPaintColor(m_inkColor);
    //(1) Retrieve the neighbor particles where the mouse is positioned (done in the KisSandPaintOp)

    //(2) Take the mouse dynamic properties (as in the pouring operation)
    int time = m_prevTime;
    
    //mouse velocity and acceleration calc
    if(info.currentTime() != m_prevTime)
        time = info.currentTime() - time;
    

    QPointF disp = toQPointF(info.movement()); //obs: this is an inline function
    QPointF pos(info.pos().x(), info.pos().y());
    QPointF vel(disp.x()/time, disp.y()/time);    //current velocity
    QPointF accel( (m_prevVel.x() - vel.x())/time, (m_prevVel.y() - vel.y())/time ); //current acce

    qDebug() << " time :" << time << " Pos : " << pos << " vel : " << vel << " accel : " << accel;

    //iterate over the set of grains in the neighborhood, assigned to m_grains
    for(int i = 0; i < m_grains.size(); i++){
        //(3.a) Verify which ones the mouse is really "colliding"
    //(3.b) and apply the force in the particles that the mouse touched (made in the applyForce method of Particle)
        
        m_grains.at(i)->applyForce(pos, vel, m_properties, width, height);

        //(4) Animate the movements based on past forces
        if(m_grains.at(i)->force()){
            m_grains.at(i)->integrationStep(double(time));
        }

        //verify if a particle is out of bounds
        if(m_grains.at(i)->pos()->x() > width || m_grains.at(i)->pos()->y() > height){
            m_grains.removeAt(i);
        }
        else{
            //have to delete the previous positions before painting them again
            drawParticle(drawer, m_grains.at(i));
        }


    }

    m_prevTime = info.currentTime();
    //(5) Update the canvas (have to do the erase particle operation)

}


void SandBrush::drawParticle(KisPainter &painter, Particle *p)
{
    QVector<QPointF> points;
    qreal cx, cy;
    int steps = 10;
    int radius = m_properties->size;


    qreal length = 2.0 * 3.14;
    qreal step = 1.0 / steps;
    for (int i = 0; i < steps; i++) {
        cx = cos(i * step * length);
        cy = sin(i * step * length);

        cx *= radius;
        cy *= radius;

        cx += p->pos()->x();
        cy += p->pos()->y();

        points.append(QPointF(cx, cy));
    }

    painter.setFillStyle(KisPainter::FillStyleForegroundColor);
    painter.paintPolygon(points);
}

void SandBrush::getGrains(QList<Particle *> &g_copy){
        for(int i=0; i < m_grains.size(); i++)
            g_copy.append(m_grains[i]);
}

void SandBrush::setGrains(QList<Particle *> &g_copy){
    m_grains.clear();
    for(int i = 0; i < g_copy.size(); i++)
        m_grains.append(g_copy[i]);

//     qDebug() << "set m_grains :" << m_grains.size();
}

