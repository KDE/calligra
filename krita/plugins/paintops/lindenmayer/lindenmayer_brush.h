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

#ifndef LINDENMAYER_BRUSH_H_
#define LINDENMAYER_BRUSH_H_

#include <QVector>

#include <KoColor.h>

#include "kis_lindenmayerop_option.h"
#include "kis_paint_device.h"

class LindenmayerBrush
{

public:
    LindenmayerBrush(const LindenmayerProperties * properties, KoColorTransformation* transformation);
    ~LindenmayerBrush();
    void paint(KisPaintDeviceSP dev, qreal x, qreal y, const KoColor &color);

private:
    /// paints wu particle, similar to spray version but you can turn on respecting opacity of the tool and add weight to opacity
    /// also the particle respects opacity in the destination pixel buffer
    void paintParticle(KisRandomAccessor& writeAccessor,KoColorSpace * cs, QPointF pos, const KoColor& color, qreal weight, bool respectOpacity);

    KoColor m_inkColor;
    int m_counter;
    const LindenmayerProperties * m_properties;
    KoColorTransformation* m_transfo;
    int m_saturationId;

    bool m_firstDab;
    QList<

};

#endif
