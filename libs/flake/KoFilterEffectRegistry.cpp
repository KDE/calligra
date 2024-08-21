/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "KoFilterEffectRegistry.h"

#include <QGlobalStatic>

#include "KoFilterEffect.h"
#include <KoPluginLoader.h>

#include <FlakeDebug.h>
#include <KoXmlReader.h>

Q_GLOBAL_STATIC(KoFilterEffectRegistry, s_instance)

KoFilterEffectRegistry::KoFilterEffectRegistry()
    : d(nullptr)
{
}

void KoFilterEffectRegistry::init()
{
    KoPluginLoader::PluginsConfig config;
    config.whiteList = "FilterEffectPlugins";
    config.blacklist = "FilterEffectPluginsDisabled";
    KoPluginLoader::load(QStringLiteral("calligra/shapefiltereffects"), config);
}

KoFilterEffectRegistry::~KoFilterEffectRegistry()
{
    qDeleteAll(doubleEntries());
    qDeleteAll(values());
}

KoFilterEffectRegistry *KoFilterEffectRegistry::instance()
{
    if (!s_instance.exists()) {
        s_instance->init();
    }
    return s_instance;
}

KoFilterEffect *KoFilterEffectRegistry::createFilterEffectFromXml(const KoXmlElement &element, const KoFilterEffectLoadingContext &context)
{
    KoFilterEffectFactoryBase *factory = get(element.tagName());
    if (!factory)
        return nullptr;

    KoFilterEffect *filterEffect = factory->createFilterEffect();
    if (filterEffect->load(element, context))
        return filterEffect;

    delete filterEffect;
    return nullptr;
}
