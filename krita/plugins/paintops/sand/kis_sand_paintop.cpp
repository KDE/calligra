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
    m_opacityOption.readOptionSetting(settings);
    m_opacityOption.sensor()->reset();

    m_properties.readOptionSetting(settings);

    KoColorTransformation* transfo = 0;

    m_sandBrush = new SandBrush( &m_properties, transfo );

    if(m_image->annotation("Particle")){
        qDebug() << "Retrieving particles...\n" ;
        QList<Particle *> p;
        m_sandBrush->getGrains(p);
        retrieveParticles(p);
        qDebug() << "Setting the grains..." ;
        m_sandBrush->setGrains(p);
    }
    qDebug() << "SandPaintop creation done." ;
}

KisSandPaintOp::~KisSandPaintOp()
{
    
    delete m_sandBrush;
}


/**
 * Where the painting operation really happens...
 */
qreal KisSandPaintOp::paintAt(const KisPaintInformation& info)
{
//     Particle *p = new Particle(true, 0.5, 1, 100,
//                                0.4, 1, new QPoint(1,3),
//                                new QPointF(2,4), new QPointF(3,5));
//     Particle *p2 = new Particle(true);
//     QByteArray * b_array = new QByteArray();
//     QDataStream stream(b_array, QIODevice::ReadWrite);
//     
//     stream << *p;
// 
//     stream.device()->reset();
//     stream >> *p2;
// 
//     qDebug() << "p " << p->radius();
//     qDebug() << "p2 " << p2->radius();

    if (!painter()) return 1.0;

    if (!m_dab) {
        m_dab = new KisPaintDevice(painter()->device()->colorSpace());
    } else {
        m_dab->clear();
    }

    qreal x1, y1;

    x1 = info.pos().x();
    y1 = info.pos().y();

    quint8 origOpacity = m_opacityOption.apply(painter(), info);
    m_sandBrush->paint(m_dab, x1, y1, painter()->paintColor(), info);

    QRect rc = m_dab->extent();

    painter()->bitBlt(rc.x(), rc.y(), m_dab, rc.x(), rc.y(), rc.width(), rc.height());
    painter()->renderMirrorMask(rc,m_dab);
    painter()->setOpacity(origOpacity);

    /*
     * Add particles to annotation
     */

    //Serialize the particles in the m_grains
    QList<Particle *> parts;
    m_sandBrush->getGrains(parts);
    if(parts.size() > 0){
        QByteArray * b_array = new QByteArray();
        QDataStream stream(b_array, QIODevice::ReadWrite);
        for(int i = 0; i < parts.size(); i++){
            stream << *parts.at(i);
        }

    m_image->addAnnotation(KisAnnotationSP(new KisAnnotation("Particle", "Set of grains that was added by the paintop", *b_array)));

    }
    
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