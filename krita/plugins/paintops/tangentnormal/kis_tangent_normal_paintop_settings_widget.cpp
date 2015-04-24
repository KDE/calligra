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

#include "kis_tangent_normal_paintop_settings_widget.h"
#include "kis_brush_based_paintop_settings.h"
#include "kis_tangent_tilt_option.h"

#include <kis_properties_configuration.h>
#include <kis_paintop_options_widget.h>
#include <kis_pressure_size_option.h>
#include <kis_curve_option_widget.h>
#include <kis_pressure_rotation_option.h>
#include <kis_pressure_scatter_option_widget.h>
#include <kis_pressure_opacity_option.h>
#include <kis_pressure_spacing_option_widget.h>



KisTangentNormalPaintOpSettingsWidget::KisTangentNormalPaintOpSettingsWidget(QWidget* parent):
    KisBrushBasedPaintopOptionWidget(parent)
{
    setObjectName("brush option widget");
    setPrecisionEnabled(true);
    addPaintOpOption(new KisTangentTiltOption());

//    addPaintOpOption(new KisCompositeOpOption(true)); No composite opp... for now.
    addPaintOpOption(new KisCurveOptionWidget(new KisPressureOpacityOption()));
    addPaintOpOption(new KisCurveOptionWidget(new KisPressureSizeOption()));
    addPaintOpOption(new KisPressureSpacingOptionWidget());
    addMirrorOption();

    addPaintOpOption(new KisCurveOptionWidget(new KisPressureRotationOption()));
    addPaintOpOption(new KisPressureScatterOptionWidget());

    addTextureOptions();

}

KisTangentNormalPaintOpSettingsWidget::~KisTangentNormalPaintOpSettingsWidget() { }

KisPropertiesConfiguration* KisTangentNormalPaintOpSettingsWidget::configuration() const
{
    KisBrushBasedPaintOpSettings *config = new KisBrushBasedPaintOpSettings();
    config->setOptionsWidget(const_cast<KisTangentNormalPaintOpSettingsWidget*>(this));
    config->setProperty("paintop", "tangentnormal");
    writeConfiguration(config);
    return config;
}


#include "kis_tangent_normal_paintop_settings_widget.moc"
