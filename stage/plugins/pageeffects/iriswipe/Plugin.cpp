/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Carlos Licea <carlos.licea@kdemail.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "Plugin.h"
#include <KPluginFactory>
#include <KPrPageEffectRegistry.h>

#include "arrowHeadWipe/KPrArrowHeadWipeEffectFactory.h"
#include "ellipseWipe/KPrEllipseWipeEffectFactory.h"
#include "eyeWipe/KPrEyeWipeEffectFactory.h"
#include "hexagonWipe/KPrHexagonWipeEffectFactory.h"
#include "irisWipe/KPrIrisWipeEffectFactory.h"
#include "miscShapeWipe/KPrMiscShapeWipeEffectFactory.h"
#include "pentagonWipe/KPrPentagonWipeEffectFactory.h"
#include "roundRectWipe/KPrRoundRectWipeEffectFactory.h"
#include "starWipe/KPrStarWipeEffectFactory.h"
#include "triangleWipe/KPrTriangleWipeEffectFactory.h"

K_PLUGIN_FACTORY_WITH_JSON(PluginFactory, "kpr_pageeffect_iriswipe.json", registerPlugin<Plugin>();)

Plugin::Plugin(QObject *parent, const QVariantList &)
    : QObject(parent)
{
    KPrPageEffectRegistry::instance()->add(new KPrIrisWipeEffectFactory());
    KPrPageEffectRegistry::instance()->add(new KPrTriangleWipeEffectFactory());
    KPrPageEffectRegistry::instance()->add(new KPrArrowHeadWipeEffectFactory());
    KPrPageEffectRegistry::instance()->add(new KPrEllipseWipeEffectFactory());
    KPrPageEffectRegistry::instance()->add(new KPrRoundRectWipeEffectFactory());
    KPrPageEffectRegistry::instance()->add(new KPrPentagonWipeEffectFactory());
    KPrPageEffectRegistry::instance()->add(new KPrHexagonWipeEffectFactory());
    KPrPageEffectRegistry::instance()->add(new KPrStarWipeEffectFactory());
    KPrPageEffectRegistry::instance()->add(new KPrEyeWipeEffectFactory());
    KPrPageEffectRegistry::instance()->add(new KPrMiscShapeWipeEffectFactory());
}

#include "Plugin.moc"
