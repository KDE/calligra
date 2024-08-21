/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007-2008 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrPageEffectRegistry.h"

#include <QGlobalStatic>
#include <QString>

#include <KoPluginLoader.h>
#include <KoXmlNS.h>
#include <KoXmlReader.h>

#include "StageDebug.h"
#include <pageeffects/KPrPageEffectFactory.h>

class KPrPageEffectRegistry::Singleton
{
public:
    Singleton()
        : initDone(false)
    {
    }

    KPrPageEffectRegistry q;
    bool initDone;
};

struct Q_DECL_HIDDEN KPrPageEffectRegistry::Private {
    QHash<QPair<QString, bool>, KPrPageEffectFactory *> tagToFactory;
};

Q_GLOBAL_STATIC(KPrPageEffectRegistry::Singleton, singleton)

KPrPageEffectRegistry *KPrPageEffectRegistry::instance()
{
    KPrPageEffectRegistry *registry = &(singleton->q);
    if (!singleton->initDone) {
        singleton->initDone = true;
        registry->init();
    }
    return registry;
}

KPrPageEffect *KPrPageEffectRegistry::createPageEffect(const KoXmlElement &element)
{
    Q_UNUSED(element);

    KPrPageEffect *pageEffect = nullptr;
    if (element.hasAttributeNS(KoXmlNS::smil, "type")) {
        QString smilType(element.attributeNS(KoXmlNS::smil, "type"));
        bool reverse = false;
        if (element.hasAttributeNS(KoXmlNS::smil, "direction") && element.attributeNS(KoXmlNS::smil, "direction") == "reverse") {
            reverse = true;
        }

        QHash<QPair<QString, bool>, KPrPageEffectFactory *>::ConstIterator it(d->tagToFactory.constFind(QPair<QString, bool>(smilType, reverse)));

        // call the factory to create the page effect
        if (it != d->tagToFactory.constEnd()) {
            pageEffect = it.value()->createPageEffect(element);
        } else {
            warnStagePageEffect << "page effect of smil:type" << smilType << "not supported";
        }
    }
    // return it
    return pageEffect;
}

KPrPageEffectRegistry::KPrPageEffectRegistry()
    : d(new Private())
{
}

KPrPageEffectRegistry::~KPrPageEffectRegistry()
{
    qDeleteAll(doubleEntries());
    qDeleteAll(values());
    delete d;
}

void KPrPageEffectRegistry::init()
{
    KoPluginLoader::PluginsConfig config;
    config.whiteList = "PageEffectPlugins";
    config.blacklist = "PageEffectPluginsDisabled";
    config.group = "stage";

    // The plugins are responsible for adding a factory to the registry
    KoPluginLoader::load(QStringLiteral("calligrastage/pageeffects"), config);

    const QList<KPrPageEffectFactory *> factories = values();

    for (KPrPageEffectFactory *factory : factories) {
        const QList<QPair<QString, bool>> tags(factory->tags());
        QList<QPair<QString, bool>>::ConstIterator it(tags.begin());
        for (; it != tags.end(); ++it) {
            d->tagToFactory.insert(*it, factory);
        }
    }
}
