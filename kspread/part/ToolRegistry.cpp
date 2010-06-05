/* This file is part of the KDE project
   Copyright 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "ToolRegistry.h"

#include "CellTool.h"
#include "CellToolFactory.h"

#include <KGlobal>
#include <KPluginInfo>
#include <KServiceTypeTrader>

#include <KoToolRegistry.h>

using namespace KSpread;


class ToolRegistry::Private
{
public:
};


ToolRegistry::ToolRegistry()
        : d(new Private)
{
    // Add the built-in cell tool.
    KoToolRegistry::instance()->add(new CellToolFactory(this, "KSpreadCellToolId"));
    // Load the tool plugins.
    loadTools();
}

ToolRegistry::~ToolRegistry()
{
    delete d;
}

ToolRegistry* ToolRegistry::instance()
{
    K_GLOBAL_STATIC(ToolRegistry, s_instance)
    return s_instance;
}

void ToolRegistry::loadTools()
{
    const QString serviceType = QLatin1String("KSpread/Plugin");
    const QString query = QLatin1String("([X-KSpread-InterfaceVersion] == 0) and "
                                        "([X-KDE-PluginInfo-Category] == 'Tool')");
    const KService::List offers = KServiceTypeTrader::self()->query(serviceType, query);
    const KConfigGroup moduleGroup = KGlobal::config()->group("Plugins");
    const KPluginInfo::List pluginInfos = KPluginInfo::fromServices(offers, moduleGroup);
    foreach(KPluginInfo pluginInfo, pluginInfos) {
        KPluginFactory *factory = KPluginLoader(*pluginInfo.service()).factory();
        if (!factory) {
            kDebug(36002) << "Unable to create plugin factory for" << pluginInfo.name();
            continue;
        }
        CellToolFactory* toolFactory = factory->create<CellToolFactory>(this);
        if (!toolFactory) {
            kDebug(36002) << "Unable to create tool factory for" << pluginInfo.name();
            continue;
        }
        pluginInfo.load(); // load activation state
        if (pluginInfo.isPluginEnabled()) {
            // Tool already registered?
            if (KoToolRegistry::instance()->contains(toolFactory->id())) {
                continue;
            }
            toolFactory->setIcon(pluginInfo.service()->icon());
            toolFactory->setPriority(10);
            toolFactory->setToolTip(pluginInfo.service()->comment());
            KoToolRegistry::instance()->add(toolFactory);
        } else {
            // Tool not registered?
            if (!KoToolRegistry::instance()->contains(toolFactory->id())) {
                continue;
            }
            delete KoToolRegistry::instance()->value(toolFactory->id());
            KoToolRegistry::instance()->remove(toolFactory->id());
        }
    }
}
