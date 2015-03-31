/* This file is part of the KDE project
*
* Copyright (C) 2010 Benjamin Port <port.benjamin@gmail.com>
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
#include "Plugin.h"
#include <kpluginfactory.h>
#include <KPrPageEffectRegistry.h>
#include "KPrSpaceRotationEffectFactory.h"

K_PLUGIN_FACTORY_WITH_JSON(PluginFactory, "kpr_pageeffect_spacerotation.json",
                           registerPlugin<Plugin>();)

Plugin::Plugin(QObject *parent, const QVariantList &)
: QObject(parent)
{
    KPrPageEffectRegistry::instance()->add(new KPrSpaceRotationEffectFactory());
}

#include "Plugin.moc"
