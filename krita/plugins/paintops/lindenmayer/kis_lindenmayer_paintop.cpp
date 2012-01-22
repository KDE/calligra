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

#include "kis_lindenmayer_paintop.h"
#include "kis_lindenmayer_paintop_settings.h"

#include <cmath>
#include <QRect>

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


KisLindenmayerPaintOp::KisLindenmayerPaintOp(const KisLindenmayerPaintOpSettings *settings, KisPainter * painter, KisImageWSP image)
    : KisPaintOp(painter), m_productions(*this)
{
    Q_UNUSED(image);
    m_opacityOption.readOptionSetting(settings);
    m_opacityOption.sensor()->reset();

    m_properties.readOptionSetting(settings);

//    KoColorTransformation* transfo = 0;
//    if (m_properties.inkDepletion && m_properties.useSaturation){
//        transfo = painter->device()->colorSpace()->createColorTransformation("hsv_adjustment", QHash<QString, QVariant>());
//    }
//    m_lindenmayerBrush = new LindenmayerBrush( &m_properties, transfo );

    m_productions.setCode(m_properties.code);
    m_firstPaint = true;
}

KisLindenmayerPaintOp::~KisLindenmayerPaintOp()
{
//    delete m_lindenmayerBrush;

    foreach(KisLindenmayerLetter* letter, m_letters) {
        delete letter;
    }
}

qreal KisLindenmayerPaintOp::paintAt(const KisPaintInformation& info)
{
    if (!painter()) return 1.0;

    m_paintInformation = &info;

    if (!m_dab) {
        m_dab = new KisPaintDevice(painter()->device()->colorSpace());
    } else {
        m_dab->clear();
    }

    qreal x, y;

    x = info.pos().x();
    y = info.pos().y();

    QList<KisLindenmayerLetter*> newLetters;
//    KisLindenmayerProduction production(*this);
    if(m_firstPaint) {
        m_firstPaint = false;
        m_letters.append(new KisLindenmayerLetter(QPointF(x, y), 0, this));
//        production.runTests();
    }
    for(int i=m_letters.size()-1; i>=0; i--) {
        newLetters.append(m_productions.produce(m_letters.at(i)));
    }
    m_letters = newLetters;
    if(m_letters.size() > 100) {
        qDebug() << "WARNING: to many lindenmayerletters, there is a maximum of 100 letters. only the first 100 letters will be kept.";
        for(int i= m_letters.size()-1; i>=100; i--) {
            delete m_letters.at(i);
            m_letters.removeLast();
        }
    }



    quint8 origOpacity = m_opacityOption.apply(painter(), info);
//    m_lindenmayerBrush->paint(m_dab, x1, y1, painter()->paintColor());

    KisPainter dabPainter;
    QRect limits = painter()->device()->extent();
    dabPainter.setMaskImageSize(limits.width(), limits.height());
    dabPainter.setBounds(limits);


    dabPainter.begin(m_dab);
    dabPainter.setFillStyle(KisPainter::FillStyleForegroundColor);
    dabPainter.setPaintColor(painter()->paintColor());
    dabPainter.setBackgroundColor(painter()->paintColor());


//    dabPainter.begin();
    foreach (KisLindenmayerLetter* letter, m_letters) {
        if(letter->getParameter("drawn").toBool() == false) {
            dabPainter.drawThickLine(letter->position(), letter->lineEndPosition(), 1, 1);
            letter->setParameter("drawn", true);
        }
//        dabPainter.paintLine(KisPaintInformation(letter->position()), KisPaintInformation(letter->lineEndPosition()));
//        painter()->addDirtyRect(QRectF(letter->position(), letter->lineEndPosition()).normalized().toRect());
    }
//    dabPainter.end();

    QRect rc = m_dab->extent();
    painter()->bitBlt(rc.x(), rc.y(), m_dab, rc.x(), rc.y(), rc.width(), rc.height());


//    painter()->renderMirrorMask(rc,m_dab);
    painter()->setOpacity(origOpacity);


    m_paintInformation = 0;

    return 1.0;
}

const KisPaintInformation& KisLindenmayerPaintOp::getSunInformations() const {
    Q_ASSERT(m_paintInformation);
    return *m_paintInformation;
}
