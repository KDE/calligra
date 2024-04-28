/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "FilterEffectsPlugin.h"
#include "BlendEffectFactory.h"
#include "BlurEffectFactory.h"
#include "ColorMatrixEffectFactory.h"
#include "ComponentTransferEffectFactory.h"
#include "CompositeEffectFactory.h"
#include "ConvolveMatrixEffectFactory.h"
#include "FloodEffectFactory.h"
#include "ImageEffectFactory.h"
#include "MergeEffectFactory.h"
#include "MorphologyEffectFactory.h"
#include "OffsetEffectFactory.h"

#include "KoFilterEffectRegistry.h"

#include <KPluginFactory>

K_PLUGIN_FACTORY_WITH_JSON(FilterEffectsPluginFacory, "calligra_filtereffects.json", registerPlugin<FilterEffectsPlugin>();)

FilterEffectsPlugin::FilterEffectsPlugin(QObject *parent, const QList<QVariant> &)
    : QObject(parent)
{
    KoFilterEffectRegistry::instance()->add(new BlurEffectFactory());
    KoFilterEffectRegistry::instance()->add(new OffsetEffectFactory());
    KoFilterEffectRegistry::instance()->add(new MergeEffectFactory());
    KoFilterEffectRegistry::instance()->add(new ColorMatrixEffectFactory());
    KoFilterEffectRegistry::instance()->add(new FloodEffectFactory());
    KoFilterEffectRegistry::instance()->add(new CompositeEffectFactory());
    KoFilterEffectRegistry::instance()->add(new BlendEffectFactory());
    KoFilterEffectRegistry::instance()->add(new ComponentTransferEffectFactory());
    KoFilterEffectRegistry::instance()->add(new ImageEffectFactory());
    KoFilterEffectRegistry::instance()->add(new MorphologyEffectFactory());
    KoFilterEffectRegistry::instance()->add(new ConvolveMatrixEffectFactory());
}

#include <FilterEffectsPlugin.moc>
