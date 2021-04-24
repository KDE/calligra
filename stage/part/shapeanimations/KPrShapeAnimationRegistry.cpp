/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007-2008 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrShapeAnimationRegistry.h"
#include "KPrShapeAnimationFactory.h"

#include <KoPluginLoader.h>
#include <KoXmlReader.h>

class KPrShapeAnimationRegistry::Singleton
{
public:
    Singleton()
    {
        // XXX: Add hard-coded animations
        loadPlugins();
    }

    void loadPlugins()
    {
        KoPluginLoader::PluginsConfig config;
        config.whiteList = "ShapeAnimationPlugins";
        config.blacklist = "ShapeAnimationPluginsDisabled";
        config.group = "stage";
        
        // XXX: Use minversion here?
        // The plugins are responsible for adding a factory to the registry
        KoPluginLoader::load(QStringLiteral("calligrastage/shapeanimations"), config);
    }

    KPrShapeAnimationRegistry q;
};

K_GLOBAL_STATIC( KPrShapeAnimationRegistry::Singleton, singleton )

KPrShapeAnimationRegistry * KPrShapeAnimationRegistry::instance()
{
    return &( singleton->q );
}

KPrShapeAnimationOld * KPrShapeAnimationRegistry::createShapeAnimation( const KoXmlElement & element )
{
    Q_UNUSED(element);
    //TODO
    return 0;
}

KPrShapeAnimationRegistry::KPrShapeAnimationRegistry()
{
    qDeleteAll(doubleEntries());
    qDeleteAll(values());
}

KPrShapeAnimationRegistry::~KPrShapeAnimationRegistry()
{
    foreach ( KPrShapeAnimationFactory* factory, values() )
    {
        delete factory;
    }
}
