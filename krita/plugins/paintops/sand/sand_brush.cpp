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
    m_counter = 0;
    m_properties = properties;
    srand48(time(0));
}


SandBrush::~SandBrush()
{
    delete m_transfo;
}


void SandBrush::paint(KisPaintDeviceSP dev, qreal x, qreal y, const KoColor &color)
{

    //We do not need to, in every mouse movement, add pixels, so it has been randomized (as in
    // the dirtThreshold), so it can drop particles more faster
    double moveThreshold = 0.9;
    double mRan = drand48();
    if ( mRan < moveThreshold) {
        return;
    }
    
    m_counter++;

    qreal result = 0;
    if (m_properties->sandDepletion) {
        result = log((qreal)m_counter * m_properties->radius)/10;
    }

    int r = m_properties->radius - int(result)*m_properties->radius;
    m_inkColor = color;
    int pixelX, pixelY;
    int radiusSquared =  r*r;
    double dirtThreshold = 0.9;
    
    KisPainter drawer(dev);
    drawer.setPaintColor(m_inkColor);
  

    qint32 pixelSize = dev->colorSpace()->pixelSize();
    KisRandomAccessorSP accessor = dev->createRandomAccessorNG((int)x, (int)y);

    qDebug() <<"(r, result, m_counter )" << "(" << r << ", " << result << ", " << m_counter ;
    
    for (int by = -r; by <= r; by++) {
        int bySquared = by*by;
        for (int bx = -r; bx <= r; bx++) {
            // let's call that noise from ground to sand :)
            double ran = drand48();
            if ( ((bx*bx + bySquared) > radiusSquared) || ran < dirtThreshold) {
                continue;
            }

            pixelX = qRound((x + (bx*ran*10)));
            pixelY = qRound((y + (by*ran*10)));
//             pixelX = qRound(x + bx);
//             pixelY = qRound(y + by);

            drawParticle(drawer, pixelX , pixelY);
        }
    }
}

void SandBrush::drawParticle(KisPainter &painter, qreal x, qreal y)
{
    QVector<QPointF> points;
    // circle x, circle y
    qreal cx, cy;
    int steps = 10;
    int radius = 5;


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