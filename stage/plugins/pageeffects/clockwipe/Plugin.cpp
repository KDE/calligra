/* This file is part of the KDE project
 *
 * Copyright (C) 2008 Boudewijn Rempt <boud@valdyas.org>
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

#include "clockwipe/KPrClockWipeEffectFactory.h"
#include "pinwheelwipe/KPrPinWheelWipeEffectFactory.h"
#include "singlesweepwipe/KPrSingleSweepWipeEffectFactory.h"
#include "fanwipe/KPrFanWipeEffectFactory.h"
#include "doublefanwipe/KPrDoubleFanWipeEffectFactory.h"
#include "doublesweepwipe/KPrDoubleSweepWipeEffectFactory.h"
#include "saloondoorwipe/KPrSaloonDoorWipeEffectFactory.h"
#include "windshieldwipe/KPrWindShieldWipeEffectFactory.h"


K_PLUGIN_FACTORY_WITH_JSON(PluginFactory, "kpr_pageeffect_clockwipe.json",
                           registerPlugin<Plugin>();)

Plugin::Plugin(QObject *parent, const QVariantList &)
    : QObject(parent)
{
    KPrPageEffectRegistry::instance()->add(new KPrClockWipeEffectFactory());
    KPrPageEffectRegistry::instance()->add(new KPrPinWheelWipeEffectFactory());
    KPrPageEffectRegistry::instance()->add(new KPrSingleSweepWipeEffectFactory());
    KPrPageEffectRegistry::instance()->add(new KPrFanWipeEffectFactory());
    KPrPageEffectRegistry::instance()->add(new KPrDoubleFanWipeEffectFactory());
    KPrPageEffectRegistry::instance()->add(new KPrDoubleSweepWipeEffectFactory());
    KPrPageEffectRegistry::instance()->add(new KPrSaloonDoorWipeEffectFactory());
    KPrPageEffectRegistry::instance()->add(new KPrWindShieldWipeEffectFactory());

}

#include "Plugin.moc"

