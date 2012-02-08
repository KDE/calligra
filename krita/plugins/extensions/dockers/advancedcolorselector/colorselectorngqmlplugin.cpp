/*
 * Copyright (C) 2012 Cyrille Berger <cberger@cberger.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 sure
 * USA
 */

#include "colorselectorngqmlplugin.h"

#include <kcomponentdata.h>
#include <kpluginfactory.h>
#include <qdeclarative.h>
#include "kis_color_selector_item.h"

K_PLUGIN_FACTORY(ColorSelectorNgQmlPluginFactory, registerPlugin<ColorSelectorNgQmlPlugin>();)
K_EXPORT_PLUGIN(ColorSelectorNgQmlPluginFactory("krita"))

ColorSelectorNgQmlPlugin::ColorSelectorNgQmlPlugin(QObject *parent, const QVariantList &)
{
    qmlRegisterType<KisColorSelectorItem> ("KritaTouch.Extensions.AdvancedColorSelectors", 1, 0, "ColorSelector");
}

ColorSelectorNgQmlPlugin::~ColorSelectorNgQmlPlugin()
{
}

#include "colorselectorngqmlplugin.moc"
