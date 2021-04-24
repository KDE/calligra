/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2008 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
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

