/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2010 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoInlineObjectRegistry.h"
#include "InsertVariableAction_p.h"
#include "KoInlineObjectFactoryBase.h"

#include <KoCanvasBase.h>
#include <KoInlineObject.h>
#include <KoPluginLoader.h>
#include <KoXmlReader.h>

#include <QGlobalStatic>

#include "TextDebug.h"

Q_GLOBAL_STATIC(KoInlineObjectRegistry, s_instance)

class Q_DECL_HIDDEN KoInlineObjectRegistry::Private
{
public:
    void insertFactory(KoInlineObjectFactoryBase *factory);
    void init(KoInlineObjectRegistry *q);

    QHash<QPair<QString, QString>, KoInlineObjectFactoryBase *> factories;
};

void KoInlineObjectRegistry::Private::init(KoInlineObjectRegistry *q)
{
    KoPluginLoader::PluginsConfig config;
    config.whiteList = "TextInlinePlugins";
    config.blacklist = "TextInlinePluginsDisabled";
    config.group = "calligra";
    KoPluginLoader::load(QStringLiteral("calligra/textinlineobjects"), config);

    foreach (KoInlineObjectFactoryBase *factory, q->values()) {
        QString nameSpace = factory->odfNameSpace();
        if (nameSpace.isEmpty() || factory->odfElementNames().isEmpty()) {
            debugText << "Variable factory" << factory->id() << " does not have odfNameSpace defined, ignoring";
        } else {
            foreach (const QString &elementName, factory->odfElementNames()) {
                factories.insert(QPair<QString, QString>(nameSpace, elementName), factory);

                debugText << "Inserting variable factory" << factory->id() << " for" << nameSpace << ":" << elementName;
            }
        }
    }
}

KoInlineObjectRegistry *KoInlineObjectRegistry::instance()
{
    if (!s_instance.exists()) {
        s_instance->d->init(s_instance);
    }
    return s_instance;
}

QList<QAction *> KoInlineObjectRegistry::createInsertVariableActions(KoCanvasBase *host) const
{
    QList<QAction *> answer;
    foreach (const QString &key, keys()) {
        KoInlineObjectFactoryBase *factory = value(key);
        if (factory->type() == KoInlineObjectFactoryBase::TextVariable) {
            foreach (const KoInlineObjectTemplate &templ, factory->templates()) {
                answer.append(new InsertVariableAction(host, factory, templ));
            }
#ifndef NDEBUG
            if (factory->templates().isEmpty()) {
                warnText << "Variable factory" << factory->id() << "has no templates, skipping.";
            }
#endif
        }
    }
    return answer;
}

KoInlineObject *KoInlineObjectRegistry::createFromOdf(const KoXmlElement &element, KoShapeLoadingContext &context) const
{
    KoInlineObjectFactoryBase *factory = d->factories.value(QPair<QString, QString>(element.namespaceURI(), element.tagName()));
    if (factory == nullptr) {
        debugText << "No factory for" << element.namespaceURI() << ":" << element.tagName();
        return nullptr;
    }

    KoInlineObject *object = factory->createInlineObject(nullptr);
    if (object) {
        object->loadOdf(element, context);
    }

    return object;
}

KoInlineObjectRegistry::~KoInlineObjectRegistry()
{
    qDeleteAll(doubleEntries());
    qDeleteAll(values());
    delete d;
}

KoInlineObjectRegistry::KoInlineObjectRegistry()
    : d(new Private())
{
}
