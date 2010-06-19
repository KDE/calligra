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

#include "KPrPageEffectRegistry.h"

#include <QString>

#include <kglobal.h>
#include <KoXmlNS.h>
#include <KoXmlReader.h>
#include <KoPluginLoader.h>
#include <pageeffects/KPrPageEffectFactory.h>
#include <kdebug.h>

class KPrPageEffectRegistry::Singleton
{
public:
    Singleton()
    : initDone( false )
    {
    }

    KPrPageEffectRegistry q;
    bool initDone;
};

struct KPrPageEffectRegistry::Private
{
    QHash<QPair<QString, bool>, KPrPageEffectFactory *> tagToFactory;
};

K_GLOBAL_STATIC( KPrPageEffectRegistry::Singleton, singleton )

KPrPageEffectRegistry * KPrPageEffectRegistry::instance()
{
    KPrPageEffectRegistry * registry = &( singleton->q );
    if ( ! singleton->initDone ) {
        singleton->initDone = true;
        registry->init();
    }
    return registry;
}

KPrPageEffect * KPrPageEffectRegistry::createPageEffect( const KoXmlElement & element )
{
    Q_UNUSED(element);

    KPrPageEffect * pageEffect = 0;
    if ( element.hasAttributeNS( KoXmlNS::smil, "type" ) ) {
        QString smilType( element.attributeNS( KoXmlNS::smil, "type" ) );
        bool reverse = false;
        if ( element.hasAttributeNS( KoXmlNS::smil, "direction" ) && element.attributeNS( KoXmlNS::smil, "direction" ) == "reverse" ) {
            reverse = true;
        }

        QHash<QPair<QString, bool>, KPrPageEffectFactory *>::iterator it( d->tagToFactory.find( QPair<QString, bool>( smilType, reverse ) ) );

        // call the factory to create the page effect 
        if ( it != d->tagToFactory.end() ) {
            pageEffect = it.value()->createPageEffect( element );
        }
        else {
            kWarning(33002) << "page effect of smil:type" << smilType << "not supported";
        }
    }
    // return it
    return pageEffect;
}

KPrPageEffectRegistry::KPrPageEffectRegistry()
: d( new Private() )
{
}

KPrPageEffectRegistry::~KPrPageEffectRegistry()
{
    foreach ( KPrPageEffectFactory* factory, values() )
    {
        delete factory;
    }
    delete d;
}

void KPrPageEffectRegistry::init()
{
    KoPluginLoader::PluginsConfig config;
    config.whiteList = "PageEffectPlugins";
    config.blacklist = "PageEffectPluginsDisabled";
    config.group = "kpresenter";

    // XXX: Use minversion here?
    // The plugins are responsible for adding a factory to the registry
    KoPluginLoader::instance()->load( QString::fromLatin1("KPresenter/PageEffect"),
            QString::fromLatin1("[X-KPresenter-Version] <= 0"),
            config);

    QList<KPrPageEffectFactory*> factories = values();

    foreach ( KPrPageEffectFactory * factory, factories ) {
        QList<QPair<QString, bool> > tags( factory->tags() );
        QList<QPair<QString, bool> >::iterator it( tags.begin() );
        for ( ; it != tags.end(); ++it ) {
            d->tagToFactory.insert( *it, factory );
        }
    }
}
