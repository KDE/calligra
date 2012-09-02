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
#include "quickpix_paintop_plugin.h"


#include <klocale.h>
#include <kiconloader.h>
#include <kcomponentdata.h>
#include <kstandarddirs.h>
#include <kis_debug.h>
#include <kpluginfactory.h>

#include <kis_paintop_registry.h>


#include "kis_quickpix_paintop.h"
#include "kis_simple_paintop_factory.h"

#include "kis_quickpix_paintop_settings.h"
#include "kis_quickpix_paintop_settings_widget.h"

#include "kis_global.h"

K_PLUGIN_FACTORY(QuickPixPaintOpPluginFactory, registerPlugin<QuickPixPaintOpPlugin>();)
K_EXPORT_PLUGIN(QuickPixPaintOpPluginFactory("krita"))


QuickPixPaintOpPlugin::QuickPixPaintOpPlugin(QObject *parent, const QVariantList &)
        : QObject(parent)
{
    //
    //setComponentData(QuickPixPaintOpPluginFactory::componentData());
    KisPaintOpRegistry *r = KisPaintOpRegistry::instance();
    r->add(new KisSimplePaintOpFactory<KisQuickPixPaintOp, KisQuickPixPaintOpSettings, KisQuickPixPaintOpSettingsWidget>("quickpixbrush", i18n("QuickPix brush"), 
                                                          KisPaintOpFactory::categoryExperimental(), "krita-quickpix.png"));

}

QuickPixPaintOpPlugin::~QuickPixPaintOpPlugin()
{
}

#include "quickpix_paintop_plugin.moc"
