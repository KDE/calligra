/*
 *  Copyright (C) 2015 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
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

#include "kis_tangent_normal_paintop.h"

#include <QRect>

#include <KoColorSpaceRegistry.h>
#include <KoColor.h>
#include <KoCompositeOpRegistry.h>

#include <kis_brush.h>
#include <kis_global.h>
#include <kis_paint_device.h>
#include <kis_painter.h>
#include <kis_image.h>
#include <kis_selection.h>
#include <kis_brush_based_paintop_settings.h>
#include <kis_cross_device_color_picker.h>
#include <kis_fixed_paint_device.h>

KisTangentNormalPaintOp::KisTangentNormalPaintOp(const KisBrushBasedPaintOpSettings* settings, KisPainter* painter, KisNodeSP node, KisImageSP image):
    KisBrushBasedPaintOp(settings, painter),
    m_tempDev(painter->device()->createCompositionSourceDevice())
    
{
    //Init, read settings, etc//
    m_tangentTiltOption.readOptionSetting(settings);
    m_sizeOption.readOptionSetting(settings);
    m_opacityOption.readOptionSetting(settings);
    m_spacingOption.readOptionSetting(settings);
    m_rotationOption.readOptionSetting(settings);
    m_scatterOption.readOptionSetting(settings);
    
    m_sizeOption.resetAllSensors();
    m_opacityOption.resetAllSensors();
    m_spacingOption.resetAllSensors();
    m_rotationOption.resetAllSensors();
    m_scatterOption.resetAllSensors();
}

KisTangentNormalPaintOp::~KisTangentNormalPaintOp()
{
    //destroy things here//
}

KisSpacingInformation KisTangentNormalPaintOp::paintAt(const KisPaintInformation& info)
{
    //For the colour, we'd ideally figure out the image colour deth, and then retreive an RGB colour space in the image colour depth, but first let's go for 8bit.
    const KoColorSpace* rgbColorSpace = KoColorSpaceRegistry::instance()->rgb8();
    
    quint8 data[4];
    
    data[0] = 255;//blue
    data[1] = 128;//green
    data[2] = 128;//red
    data[3] = 255;//alpha, leave alone.
    
    quint8 r, g, b;
    m_tangentTiltOption.apply(info, &r, &g, &b);
    
    data[0] = b;//blue
    data[1] = g;//green
    data[2] = r;//red
    
    KoColor color(data, rgbColorSpace);//Should be default RGB(0.5,0.5,1.0)
    
    //draw stuff here, return kisspacinginformation.
    KisBrushSP brush = m_brush;
    
    if (!painter()->device() || !brush || !brush->canPaintFor(info)) {
        return KisSpacingInformation(1.0);
    }
    
    qreal scale    = m_sizeOption.apply(info);
    qreal rotation = m_rotationOption.apply(info);
    
    if (checkSizeTooSmall(scale)) return KisSpacingInformation();

    setCurrentScale(scale);
    setCurrentRotation(rotation);
    
    QPointF scatteredPos =
        m_scatterOption.apply(info,
                              brush->maskWidth(scale, rotation, 0, 0, info),
                              brush->maskHeight(scale, rotation, 0, 0, info));
                              
    QPointF hotSpot = brush->hotSpot(scale, scale, rotation, info);
    
    m_maskDab =
        m_dabCache->fetchDab(rgbColorSpace, color, info.pos(),
                             scale, scale, rotation,
                             info, 1.0,
                             &m_dstDabRect);

    if (m_dstDabRect.isEmpty()) return KisSpacingInformation(1.0);

    QRect dabRect = m_maskDab->bounds();

    // sanity check
    Q_ASSERT(m_dstDabRect.size() == dabRect.size());
    
    quint8  oldOpacity = painter()->opacity();
    QString oldCompositeOpId = painter()->compositeOp()->id();
    qreal   fpOpacity  = (qreal(oldOpacity) / 255.0) * 1.0;//m_opacityOption.getOpacityf(info);
    
    //paint with the default color? Copied this from color smudge.//
    //painter()->setCompositeOp(COMPOSITE_COPY);
    //painter()->fill(0, 0, m_dstDabRect.width(), m_dstDabRect.height(), color);
    painter()->bltFixed(m_dstDabRect.topLeft(), m_maskDab, m_maskDab->bounds());
    painter()->renderMirrorMaskSafe(m_dstDabRect, m_maskDab, !m_dabCache->needSeparateOriginal());

    // restore orginal opacity and composite mode values
    painter()->setOpacity(oldOpacity);
    painter()->setCompositeOp(oldCompositeOpId);
    
    return effectiveSpacing(scale, rotation);
}


