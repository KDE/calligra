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
#include "stacked_paintop_plugin.h"


#include <klocale.h>

#include <kis_debug.h>
#include <kpluginfactory.h>

#include <kis_paintop_registry.h>
#include "kis_simple_paintop_factory.h"

#include "stacked_paintop.h"
#include "stacked_paintop_settings_widget.h"
#include "stacked_paintop_settings.h"
#include "stacked_paintop_settings.h"

#include "kis_global.h"

K_PLUGIN_FACTORY(StackedPaintOpPluginFactory, registerPlugin<StackedPaintOpPlugin>();)
K_EXPORT_PLUGIN(StackedPaintOpPluginFactory("krita"))


StackedPaintOpPlugin::StackedPaintOpPlugin(QObject *parent, const QVariantList &)
    : QObject(parent)
{
    KisPaintOpRegistry *r = KisPaintOpRegistry::instance();
    r->add(new KisSimplePaintOpFactory<StackedPaintOp, StackedPaintOpSettings, StackedPaintOpSettingsWidget>("stackedbrush", i18n("Stacked Brush"),
            KisPaintOpFactory::categoryStable(), "krita-stacked.png"));
}

StackedPaintOpPlugin::~StackedPaintOpPlugin()
{
}

#include "stacked_paintop_plugin.moc"
