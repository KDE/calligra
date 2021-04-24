/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2008 Boudewijn Rempt <boud@valdyas.org>
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "Plugin.h"
#include <kpluginfactory.h>
#include <KPrPageEffectRegistry.h>

#include "diagonalwipe/DiagonalWipeEffectFactory.h"
#include "miscdiagonalwipe/MiscDiagonalWipeEffectFactory.h"
#include "boxwipe/BoxWipeEffectFactory.h"
#include "fourboxwipe/FourBoxWipeEffectFactory.h"
#include "barndoorwipe/BarnDoorWipeEffectFactory.h"
#include "veewipe/VeeWipeEffectFactory.h"
#include "barnveewipe/BarnVeeWipeEffectFactory.h"
#include "zigzagwipe/ZigZagWipeEffectFactory.h"
#include "barnzigzagwipe/BarnZigZagWipeEffectFactory.h"

K_PLUGIN_FACTORY_WITH_JSON(PluginFactory, "kpr_pageeffect_edgewipe.json",
                           registerPlugin<Plugin>();)

Plugin::Plugin(QObject *parent, const QVariantList &)
    : QObject(parent)
{
    KPrPageEffectRegistry::instance()->add(new DiagonalWipeEffectFactory());
    KPrPageEffectRegistry::instance()->add(new MiscDiagonalWipeEffectFactory());
    KPrPageEffectRegistry::instance()->add(new BoxWipeEffectFactory());
    KPrPageEffectRegistry::instance()->add(new FourBoxWipeEffectFactory());
    KPrPageEffectRegistry::instance()->add(new BarnDoorWipeEffectFactory());
    KPrPageEffectRegistry::instance()->add(new VeeWipeEffectFactory());
    KPrPageEffectRegistry::instance()->add(new BarnVeeWipeEffectFactory());
    KPrPageEffectRegistry::instance()->add(new ZigZagWipeEffectFactory());
    KPrPageEffectRegistry::instance()->add(new BarnZigZagWipeEffectFactory());
}

#include "Plugin.moc"

