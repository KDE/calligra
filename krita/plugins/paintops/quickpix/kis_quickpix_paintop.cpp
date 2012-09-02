/*
 *  Copyright (c) 2012 José Luis Vergara <pentalis@gmail.com>
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

#include "kis_quickpix_paintop.h"
#include "kis_quickpix_paintop_settings.h"

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
#include <kis_brush_based_paintop.h>
#include <kis_paint_information.h>

#include <kis_pressure_opacity_option.h>

#include <QPointF>

#include "kis_fixed_paint_device.h"

KisQuickPixPaintOp::KisQuickPixPaintOp(const KisQuickPixPaintOpSettings *settings, KisPainter * painter, KisImageWSP image)
        : KisBrushBasedPaintOp(settings, painter)
        
{
    m_dab = new KisFixedPaintDevice(painter->device()->colorSpace());
    m_dab->setRect(QRect(0, 0, 1000, 1000));
    m_dab->initialize(128);
}

KisQuickPixPaintOp::~KisQuickPixPaintOp()
{
}

// Use this method to paint each point that is pressed. Yo can also
// override the paintLine method and use that to paint from a starting
// point to an end point
qreal KisQuickPixPaintOp::paintAt(const KisPaintInformation& info)
{
    if (!painter()->device()) return 1;
    
    QPointF pt = info.pos();
    pt.setX(pt.x() - 500);
    pt.setY(pt.y() - 500);

    painter()->bltFixed(pt.x(), pt.y(),
               m_dab,
               0, 0,
               1000, 1000);;
	return 100;
}

