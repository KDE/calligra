/*
 * Copyright (c) 2012 Cyrille Berger <cberger@cberger.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "kis_rijn_palette.h"

#include <kis_image.h>
#include <kis_paintop_preset.h>
#include <kis_paintop_settings.h>
#include <kis_paintop_registry.h>

#include "kis_rijn_brush.h"

KisRijnPalette::KisRijnPalette()
{

}

OpenRijn::AbstractBrush* KisRijnPalette::createCircleBrush(float _size, float _hardness) const
{
    KisPaintOpPresetSP    preset    = KisPaintOpRegistry::instance()->defaultPreset(KoID("paintbrush"), 0);
    QString brush_definition_template = "<Brush type=\"auto_brush\" randomness=\"0\" density=\"1\" BrushVersion=\"2\" spacing=\"0.1\" angle=\"0\"> <MaskGenerator ratio=\"1\" type=\"circle\" vfade=\"%1\" id=\"default\" spikes=\"2\" hfade=\"%2\" diameter=\"%3\"/> </Brush>";
    preset->settings()->setProperty("brush_definition", brush_definition_template.arg(_hardness).arg(_hardness).arg(_size));
    
    return new KisRijnBrush(preset);
}

