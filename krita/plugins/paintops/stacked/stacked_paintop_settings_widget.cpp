/*
 *  Copyright (c) 2015 Boudewijn Rempt <boud@valdyas.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#include "stacked_paintop_settings_widget.h"

#include <kis_paint_action_type_option.h>

#include "stacked_paintop_settings.h"
#include "stackedop_option.h"

StackedPaintOpSettingsWidget:: StackedPaintOpSettingsWidget(QWidget* parent)
    : KisPaintOpSettingsWidget(parent)
{
    m_stackedOption = new StackedOpOption();
    addPaintOpOption(m_stackedOption, i18n("Brush size"));
    addPaintOpOption(new KisPaintActionTypeOption(), i18n("Painting Mode"));
}

StackedPaintOpSettingsWidget::~ StackedPaintOpSettingsWidget()
{
}

KisPropertiesConfiguration*  StackedPaintOpSettingsWidget::configuration() const
{
    StackedPaintOpSettings* config = new StackedPaintOpSettings();
    config->setOptionsWidget(const_cast<StackedPaintOpSettingsWidget*>(this));
    config->setProperty("paintop", "stackedbrush"); // XXX: make this a const id string
    writeConfiguration(config);
    return config;
}

void StackedPaintOpSettingsWidget::changePaintOpSize(qreal x, qreal y)
{
}

QSizeF StackedPaintOpSettingsWidget::paintOpSize() const
{
    return QSizeF(10, 10);
}
