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
        m_saturationId = m_transfo->parameterId("s"); // cache for later usage
        m_transfo->setParameter(m_transfo->parameterId("v"), 0.0);
    }
    else {
        m_saturationId = -1;
    }
    
//     m_counter = m_properties->radius;
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


void SandBrush::paint(KisPaintDeviceSP dev, qreal x, qreal y, const KoColor &color, const KisPaintInformation& info)
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
    //mouse velocity and acceleration calc
    if(info.currentTime() != m_prevTime)
        time = info.currentTime() - time;
    
    QPointF pos = toQPointF(info.movement()); //obs: this is an inline function
    QPointF vel(pos.x()/time, pos.y()/time);    //current velocity
    QPointF accel( (m_prevVel.x() - vel.x())/time, (m_prevVel.y() - vel.y())/time ); //current accel

//     qDebug() <<"time : " << time << "\n";
//     qDebug() <<"(pos, vel, accel ) : " << "(" << pos.x() << ", " << pos.y() << ")\n "
//                                     << "(" << vel.x() << ", " << vel.y() << ")\n "
//                                     << "(" << accel.x() << ", " << accel.y() << ")\n ";
    
    m_counter++;

    // sand depletion: it's not working as it should. get an assymptotic function to reduce the radius
    qreal result = 0;
//     if (m_properties->sandDepletion) {
//         result = log((qreal)m_counter * m_properties->radius)/10;
//     }

    int r = m_properties->radius - int(result)*m_properties->radius;
    m_inkColor = color;
    int pixelX, pixelY;
    int radiusSquared =  r*r;

    double dirtThreshold = 0.9;
    
    KisPainter drawer(dev);
    drawer.setPaintColor(m_inkColor);
  

//     qint32 pixelSize = dev->colorSpace()->pixelSize();
    KisRandomAccessorSP accessor = dev->createRandomAccessorNG((int)x, (int)y);

//     qDebug() <<"(r, result, m_counter )" << "(" << r << ", " << result << ", " << m_counter ;
    
    for (int by = -r; by <= r; by++) {
        int bySquared = by*by;
        for (int bx = -r; bx <= r; bx++) {
            // let's call that noise from ground to sand :)
            double ran = drand48();
            if ( ((bx*bx + bySquared) > radiusSquared) || ran < dirtThreshold) {
                continue;
            }


            
            pixelX = qRound(x + bx*vel.x()*10);
            pixelY = qRound(y + by*vel.y()*10);


            drawParticle(drawer, pixelX , pixelY, vel, accel);
            if(m_grainCount > 0)
                m_grainCount--; //decrease the amount of grains
            else
                m_grainCount = 0;
        }
    }

    m_prevVel = vel;
    m_prevTime = info.currentTime();
}

void SandBrush::drawParticle(KisPainter &painter, qreal x, qreal y, QPointF vel, QPointF accel)
{
    Particle *p = new Particle ( true,
                                 float( m_properties->mass),
                                 float( m_properties->size),
                                 1000,
                                 float( m_properties->friction),
                                 0.0,
                                 new QPoint(x, y),
                                 new QPointF(vel.x(), vel.y()),
                                 new QPointF(accel.x(), accel.y())
                               );
//     qDebug() << "Size -> " << m_properties->size;

    m_grains.append(p);

//     qDebug() << "Particle : \n"  << "Life" << p->lifespan() << "\n"
//                             << "Mass" << p->mass() << "\n"
//                             << "Radius" << p->radius() << "\n"
//                             << "Friction" << p->friction() << "\n"
//                             << "Dissipation" << p->dissipation() << "\n"
//                             << "Pos(x)" << p->pos()->x() << "\n"
//                             << "Pos(y)" << p->pos()->y() << "\n"
//                             << "Vel(x)" << p->vel()->x() << "\n"
//                             << "Vel(y)" << p->vel()->y() << "\n"
//                             << "Accel(x)" << p->accel()->x() << "\n"
//                             << "Accel(y)" << p->accel()->y() << "\n";
    
    QVector<QPointF> points;
    // circle x, circle y
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

        cx += x;
        cy += y;

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
    for(int i = 0; i < g_copy.size(); i++)
        m_grains.append(g_copy[i]);
}