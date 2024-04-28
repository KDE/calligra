/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoEventActionRegistry.h"

#include <QGlobalStatic>
#include <QHash>

#include <FlakeDebug.h>

#include "KoEventAction.h"
#include "KoEventActionFactoryBase.h"
#include <KoPluginLoader.h>
#include <KoXmlNS.h>
#include <KoXmlReader.h>

class KoEventActionRegistry::Singleton
{
public:
    Singleton()
        : initDone(false)
    {
    }

    KoEventActionRegistry q;
    bool initDone;
};

Q_GLOBAL_STATIC(KoEventActionRegistry::Singleton, singleton)

class Q_DECL_HIDDEN KoEventActionRegistry::Private
{
public:
    QHash<QString, KoEventActionFactoryBase *> presentationEventActionFactories;
    QHash<QString, KoEventActionFactoryBase *> presentationEventActions;
    QHash<QString, KoEventActionFactoryBase *> scriptEventActionFactories;
};

KoEventActionRegistry *KoEventActionRegistry::instance()
{
    KoEventActionRegistry *registry = &(singleton->q);
    if (!singleton->initDone) {
        singleton->initDone = true;
        registry->init();
    }
    return registry;
}

KoEventActionRegistry::KoEventActionRegistry()
    : d(new Private())
{
}

KoEventActionRegistry::~KoEventActionRegistry()
{
    delete d;
}

void KoEventActionRegistry::addPresentationEventAction(KoEventActionFactoryBase *factory)
{
    const QString &action = factory->action();
    if (!action.isEmpty()) {
        d->presentationEventActionFactories.insert(factory->id(), factory);
        d->presentationEventActions.insert(action, factory);
    }
}

void KoEventActionRegistry::addScriptEventAction(KoEventActionFactoryBase *factory)
{
    d->scriptEventActionFactories.insert(factory->id(), factory);
}

QList<KoEventActionFactoryBase *> KoEventActionRegistry::presentationEventActions()
{
    return d->presentationEventActionFactories.values();
}

QList<KoEventActionFactoryBase *> KoEventActionRegistry::scriptEventActions()
{
    return d->scriptEventActionFactories.values();
}

void KoEventActionRegistry::init()
{
    KoPluginLoader::PluginsConfig config;
    config.whiteList = "PresentationEventActionPlugins";
    config.blacklist = "PresentationEventActionPluginsDisabled";
    config.group = "calligra";
    KoPluginLoader::load(QStringLiteral("calligra/presentationeventactions"), config);

    config.whiteList = "ScriptEventActionPlugins";
    config.blacklist = "ScriptEventActionPluginsDisabled";
    KoPluginLoader::load(QStringLiteral("calligra/scripteventactions"), config);
}

QSet<KoEventAction *> KoEventActionRegistry::createEventActionsFromOdf(const KoXmlElement &e, KoShapeLoadingContext &context) const
{
    QSet<KoEventAction *> eventActions;

    if (e.namespaceURI() == KoXmlNS::office && e.tagName() == "event-listeners") {
        KoXmlElement element;
        forEachElement(element, e)
        {
            if (element.tagName() == "event-listener") {
                if (element.namespaceURI() == KoXmlNS::presentation) {
                    QString action(element.attributeNS(KoXmlNS::presentation, "action", QString()));
                    QHash<QString, KoEventActionFactoryBase *>::const_iterator it(d->presentationEventActions.find(action));

                    if (it != d->presentationEventActions.constEnd()) {
                        KoEventAction *eventAction = it.value()->createEventAction();
                        if (eventAction) {
                            if (eventAction->loadOdf(element, context)) {
                                eventActions.insert(eventAction);
                            } else {
                                delete eventAction;
                            }
                        }
                    } else {
                        warnFlake << "presentation:event-listerer action = " << action << "not supported";
                    }
                } else if (element.namespaceURI() == KoXmlNS::script) {
                    // TODO
                } else {
                    warnFlake << "element" << e.namespaceURI() << e.tagName() << "not supported";
                }
            } else {
                warnFlake << "element" << e.namespaceURI() << e.tagName() << "not supported";
            }
        }
    } else {
        warnFlake << "office:event-listeners not found got:" << e.namespaceURI() << e.tagName();
    }

    return eventActions;
}
