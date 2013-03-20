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

#include <KoPluginLoader.h>
#include <KoToolRegistry.h>

using namespace Calligra::Sheets;


class ToolRegistry::Private
{
public:
};


ToolRegistry::ToolRegistry()
    : d(new Private)
{
    // Add the built-in cell tool.
    KoToolRegistry::instance()->add(new CellToolFactory("KSpreadCellToolId"));
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
    QList<QObject*> toolPlugins = KoPluginLoader::instance()->retrievePlugins(this,
                                                                              QLatin1String("CalligraSheets/Plugin"),
                                                                              QLatin1String("([X-CalligraSheets-InterfaceVersion] == 0) and "
                                                                                            "([X-KDE-PluginInfo-Category] == 'Tool')"));
    foreach(QObject *plugin, toolPlugins) {
        CellToolFactory* toolFactory = qobject_cast<CellToolFactory*>(plugin);
        if (plugin->property("plugin-info:enabled").toBool()) {
            if (KoToolRegistry::instance()->contains(toolFactory->id())) {
                continue;
            }
            toolFactory->setIconName(plugin->property("plugin:icon").toString());
            toolFactory->setPriority(10);
            toolFactory->setToolTip(plugin->property("plugin:comment").toString());
            KoToolRegistry::instance()->add(toolFactory);
        }
        else {
            // Tool not registered?
            if (!KoToolRegistry::instance()->contains(toolFactory->id())) {
                continue;
            }
            delete KoToolRegistry::instance()->value(toolFactory->id());
            KoToolRegistry::instance()->remove(toolFactory->id());
        }
    }
}
