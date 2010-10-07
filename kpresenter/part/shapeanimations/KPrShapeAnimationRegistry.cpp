/* This file is part of the KDE project
   Copyright (C) 2007-2008 Thorsten Zachmann <zachmann@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#include "KPrShapeAnimationRegistry.h"
#include "KPrShapeAnimationFactory.h"

#include <kglobal.h>
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
        config.group = "kpresenter";
        
        // XXX: Use minversion here?
        // The plugins are responsible for adding a factory to the registry
        KoPluginLoader::instance()->load( QString::fromLatin1("KPresenter/ShapeAnimation"),
                                          QString::fromLatin1("[X-KPresenter-Version] <= 0"),
                                          config);
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
